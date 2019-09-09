#include <ccan/array_size/array_size.h>
#include <ccan/json_out/json_out.h>
#include <common/json_out.h>
#include <common/param.h>
#include <common/type_to_string.h>
#include <gossipd/routing.h>
#include <lightningd/json.h>
#include <plugins/libplugin.h>

/* Public key of this node. */
static struct node_id my_id;

/*~ In theory, this could have been built as a plugin on top
 * of `getroute`.
 * However, the `max_hops` argument of `getroute` does not
 * limit the graph that the `getroute` algorithm traverses;
 * instead, it scans the entire graph, and if the resulting
 * route is longer than `max_hops` will ***re-scan*** the
 * entire graph with a tweaked cost function until it finds
 * a route that fits.
 *
 * As the speed of `permuteroute` is due solely to restricting
 * the graph we scan, we just use a depth-first iterative
 * algorithm until we reach any node after the point at which
 * the payment fails.
 *
 * Arguably a Djikstra with limited hops would be better, but
 * simplicity wins for this case as we can avoid heap
 * allocations and keep data in hot stack memory.
 */

/*~ Glossary:
 *
 * Pivot node - the node which signalled the routing failure
 * for a channel-level failure, and from which we start the
 * search to heal the route.
 *
 * Return node - the node to which we found a route from the
 * pivot.
 *
 * Prefix - the part of the path from the payer to the pivot.
 *
 * Postfix - the part of the path from the return node to the
 * payee.
 */

/*~ Possibly stores the results of a successful healing of the
 * original route.
 *
 * This is intended to be stack-allocated instead of tal-allocated.
 *
 * Our model is that the current route is broken at permute_after,
 * i.e. the channel indexed by permute_after in current_route has
 * failed, and the node indexed by permute_after was not reached
 * (but every node before permute_after was reached).
 * Our "pivot" is the node just before the failing channel.
 * Our "return" is the node at or after the failing channel where
 * we go back to the original route to the destination.
 */

static struct command_result *
param_exclude_array(struct command *cmd, const char *name,
		    const char *buf, const jsmntok_t *tok,
		    const char ***excluded)
{
	struct command_result *result;
	const jsmntok_t *excludetok;
	const jsmntok_t *t;
	size_t i;

	result = param_array(cmd, name, buf, tok, &excludetok);
	if (result)
		return result;

	*excluded = tal_arr(cmd, const char *, excludetok->size);

	json_for_each_arr(i, t, excludetok) {
		struct short_channel_id_dir *chan_id = tal(tmpctx, struct short_channel_id_dir);
		if (!short_channel_id_dir_from_str(buf + t->start,
						   t->end - t->start,
						   chan_id)) {

			struct node_id *node_id = tal(tmpctx, struct node_id);

			if (!json_to_node_id(buf, t, node_id))
				return command_fail(cmd, JSONRPC2_INVALID_PARAMS,
						    "%.*s is not a valid"
						    " short_channel_id/node_id",
						    t->end - t->start,
						    buf + t->start);
		}
		*excluded[i] = tal_strndup(excluded, buf + t->start, t->end - t->start);
	}
	return NULL;
}

struct permuteroute_command;

struct recalcroute_command {
	struct route_hop *route;
	struct amount_msat *msat;
	u32 *cltv;
	struct node_id *source;
	u32 search_index;
	struct route_hop *hint;
	struct permuteroute_command *internal_call;
};

static struct command_result *
recalculate_success(struct command *cmd, struct recalcroute_command *rc)
{
	struct json_out *ret = json_out_new(NULL);

	json_out_add_route(ret, rc->route, rc->hint);
	return command_success(cmd, ret);
}

static struct command_result *
permuteroute_success(struct command *cmd, struct permuteroute_command *pc,
		     struct route_hop *route);

static struct command_result *
do_recalculate_route(struct command *cmd, const char *buf, const jsmntok_t *result,
		     struct recalcroute_command *rc)
{
	const jsmntok_t *channelstok = json_get_member(buf, result, "channels");
	const jsmntok_t *chan;
	if (!channelstok->size)
		return command_fail(cmd, LIGHTNINGD,
				    "Unknow channel: %s",
				    take(short_channel_id_to_str(NULL,
					 &rc->route[rc->search_index].channel_id)));

	size_t i;
	assert(channelstok->size == 2);
	json_for_each_arr(i, chan, channelstok) {
		struct node_id dest, source;
		u32 base;
		u32 fee_per;
		u32 cltv;

		if (!json_to_node_id(buf, json_get_member(buf, chan, "destination"), &dest))
			return command_fail(cmd, LIGHTNINGD,
					    "Error destination field in listchannel: %.*s",
					    channelstok->end - channelstok->start,
					    buf + channelstok->start);

		if (!node_id_eq(&dest, &rc->route[rc->search_index].nodeid)) {
			if (i)
				return command_fail(cmd, LIGHTNINGD,
						    "Error hop(%s) with channel(%s) in route? "
						    "Wrong destination.",
						    take(node_id_to_hexstr(NULL,
							 &rc->route[rc->search_index].nodeid)),
						    take(short_channel_id_to_str(NULL,
							 &rc->route[rc->search_index].channel_id)));
			continue;
		}

		if (!json_to_node_id(buf, json_get_member(buf, chan, "source"), &source))
			return command_fail(cmd, LIGHTNINGD,
					    "Error source field in listchannel: %.*s",
					    channelstok->end - channelstok->start,
					    buf + channelstok->start);

		if (rc->search_index) {
			if (!node_id_eq(&source, &rc->route[rc->search_index - 1].nodeid)) {
				if (i)
					return command_fail(cmd, LIGHTNINGD,
							    "Error hop(%s) with channel(%s) in route? "
							    "Wrong source.",
							    take(node_id_to_hexstr(NULL,
								 &rc->route[rc->search_index].nodeid)),
							    take(short_channel_id_to_str(NULL,
								 &rc->route[rc->search_index].channel_id)));
				continue;
			}
		} else {
			if (!node_id_eq(&source, rc->source)) {
				if (i)
					return command_fail(cmd, LIGHTNINGD,
							    "Error hop(%s) with channel(%s) in route?",
							    take(node_id_to_hexstr(NULL,
								 &rc->route[rc->search_index].nodeid)),
							    take(short_channel_id_to_str(NULL,
								 &rc->route[rc->search_index].channel_id)));
				continue;
			}
		}

		if (rc->search_index == tal_count(rc->route) - 1)
			break;

		if (!json_to_number(buf, json_get_member(buf, chan, "base_fee_millisatoshi"), &base))
			return command_fail(cmd, LIGHTNINGD,
					    "Error base_fee_millisatoshi field in listchannel: %.*s",
					    channelstok->end - channelstok->start,
					    buf + channelstok->start);

		if (!json_to_number(buf, json_get_member(buf, chan, "fee_per_millionth"), &fee_per))
			return command_fail(cmd, LIGHTNINGD,
					    "Error fee_per_millionth field in listchannel: %.*s",
					    channelstok->end - channelstok->start,
					    buf + channelstok->start);

		/* get_route should use size_t(%zu) */
		if (!amount_msat_add_fee(rc->msat, base, fee_per))
			return command_fail(cmd, LIGHTNINGD,
					    "recalculate overflow in route[%u]: %s + %u/%u!?",
					     rc->search_index,
					     take(type_to_string(NULL, struct amount_msat,
								 rc->msat)),
					     base, fee_per);

		/* Don't use if total would exceed 1/4 of our time allowance. */
		if (!json_to_number(buf, json_get_member(buf, chan, "delay"), &cltv))
			return command_fail(cmd, LIGHTNINGD,
					    "Error delay field in listchannel: %.*s",
					    channelstok->end - channelstok->start,
					    buf + channelstok->start);

		*rc->cltv += cltv;

		rc->route[rc->search_index].amount = *rc->msat;
		rc->route[rc->search_index].delay = *rc->cltv;

		break;
	}

	if (rc->search_index) {
		rc->search_index--;
		const char *chan_id_str = short_channel_id_to_str(tmpctx,
						&rc->route[rc->search_index].channel_id);
		return send_outreq(cmd, "listchannels",
				   do_recalculate_route, forward_error, rc,
				   take(json_out_obj(NULL, "short_channel_id",
						     chan_id_str)));
	}

	if (rc->internal_call)
		return permuteroute_success(cmd, rc->internal_call, rc->route);

	return recalculate_success(cmd, rc);
}

static struct command_result *
recalculate_route(struct command *cmd, struct route_hop *route,
		  struct amount_msat *msat, u32 *cltv,
		  struct node_id *source,
		  struct route_hop *hint,
		  struct permuteroute_command *internal_call)
{
	struct recalcroute_command* rc = tal(cmd, struct recalcroute_command);
	rc->route = tal_steal(rc, route);
	rc->search_index = tal_count(rc->route) - 1;
	if (!msat) {
		rc->msat = tal(rc, struct amount_msat);
		*rc->msat = rc->route[rc->search_index].amount;
	} else {
		rc->msat = tal_steal(rc, msat);
		rc->route[rc->search_index].amount = *rc->msat;
	}

	if (!cltv) {
		rc->cltv = tal(rc, u32);
		*rc->cltv = rc->route[rc->search_index].delay;
	} else {
		rc->cltv = tal_steal(rc, cltv);
		rc->route[rc->search_index].amount = *msat;
	}

	if (hint)
		rc->hint = tal_steal(rc, hint);
	else
		rc->hint = NULL;

	if (!source)
		rc->source = &my_id;
	else
		rc->source = tal_steal(rc, source);

	rc->internal_call = internal_call;

	const char *chan_id_str = short_channel_id_to_str(tmpctx,
					&rc->route[rc->search_index].channel_id);

	return send_outreq(cmd, "listchannels",
			   do_recalculate_route, forward_error, rc,
			   take(json_out_obj(NULL, "short_channel_id", chan_id_str)));
}

static struct command_result *
json_recalcroute(struct command *cmd, const char *buffer, const jsmntok_t *params)
{
	struct route_hop *route;
	struct amount_msat *msat;
	u32 *cltv;
	struct node_id *source;
	struct route_hop *hint;

	if (!param(cmd, buffer, params,
		   p_req("route", param_route, &route),
		   p_opt("msatoshi", param_msat, &msat),
		   p_opt("cltv", param_number, &cltv),
		   p_opt("fromid", param_node_id, &source),
		   p_opt("hint", param_route_hop, &hint),
		   NULL))
		return NULL;

	return recalculate_route(cmd, route, msat, cltv, source, hint, NULL);
}

struct smoothenroute_command {
	struct route_hop *route;
	struct node_id *source;
};

static void do_smoothen_route(struct smoothenroute_command *sc)
{
	int i, j;
	size_t len, orig_len;
	struct node_id *n;
	struct node_id *o;
	struct route_hop *route;

	route = sc->route;
	orig_len = len = tal_count(sc->route) - 1;

	n = sc->source;
	for (i = 0; i < len; i++) {
		o = n;
		j = i;
		while (j < len) {
			o = &route[j].nodeid;
			j++;

			if (n == o) {
				/* Delete the intervening.  */
				memmove(&route[i], &route[j],
					(len - j) * sizeof(struct route_hop));
				len -= j - i;
				j = i;
			}

		}
		n = &route[i].nodeid;
	}

	/* Do resize after the loop.  */
	if (orig_len != len) {
		bool res = tal_resize(&route, len);
		assert(res);
	}
}

static struct command_result *
smoothen_route(struct command *cmd, struct route_hop *route,
	       struct node_id *source,
	       struct permuteroute_command *internal_call);

static struct command_result *
smoothen_route(struct command *cmd, struct route_hop *route,
	       struct node_id *source,
	       struct permuteroute_command *internal_call)
{
	struct smoothenroute_command* sc = tal(cmd, struct smoothenroute_command);

	if (!source)
		sc->source = &my_id;
	else
		sc->source = tal_steal(sc, source);

	sc->route = tal_steal(sc, route);
	do_smoothen_route(sc);

	return recalculate_route(cmd,
				 tal_dup_arr(cmd, struct route_hop,
					     sc->route, tal_count(sc->route) - 1, 0),
				 NULL, NULL, sc->source,
				 &sc->route[tal_count(sc->route) - 1],
				 internal_call);
}

/*
 * smoothen_route - Remove any loops from a route (a tal_arr of
 * pointers to chan, starting at the source node).
 * This functions is relevant when routes are concatenated, where
 * each sub-part of the route was derived separately from other
 * parts of the route, possibly passing through a node multiple
 * times
*/
static struct command_result *
json_smoothenroute(struct command *cmd, const char *buffer, const jsmntok_t *params)
{
	struct route_hop *route;
	struct node_id *source;

	if (!param(cmd, buffer, params,
		   p_req("route", param_route, &route),
		   p_opt("fromid", param_node_id, &source),
		   NULL))
		return NULL;

	return NULL;
}

struct permuteroute_command {
	struct route_hop *original_route;
	u32 *erring_index;
	u32 *cltv;
	struct amount_msat *msat;
	double *riskfactor;
	const char **excludes;
	struct node_id *source;
	u32 *max_hops;
	struct node_id *pivot;
	unsigned int *permute_index;
	u32 *max_scan_nodes;
};

static struct route_hop *
build_final_permuted_route(struct command *cmd,
			   struct route_hop *new_route,
			   struct permuteroute_command *pc,
			   size_t *new_route_len)
{
	size_t postfix_len;
	plugin_log(LOG_INFORM,
		   "permute_route: Generated %zu-hop route "
		   "to return node %s "
		   "giving %s (%"PRIu32" delay)",
		   tal_count(new_route),
		   take(node_id_to_hexstr(NULL,
					  &new_route[tal_count(new_route) - 1].nodeid)),
		   take(type_to_string(NULL, struct amount_msat,
				       &new_route[tal_count(new_route) - 1].amount)),
		   new_route[tal_count(new_route) - 1].delay);

	postfix_len = tal_count(pc->original_route) - 1 - (size_t)*pc->erring_index;
	/* Compute final size of route.  */
	*new_route_len = tal_count(new_route) + postfix_len;
	if (*new_route_len > *pc->max_hops) {
		plugin_log(LOG_INFORM,
			   "permute_route: Route length %zu > max_hops %"PRIu32"",
			   *new_route_len, *pc->max_hops);
		return tal_free(new_route);
	}

	/* Fill in the postfix.  */
	tal_resize(&new_route, *new_route_len);
	for (int i = 0; i < postfix_len; ++i)
		new_route[tal_count(new_route) + i] =
			pc->original_route[*pc->erring_index + 1 + i];

	plugin_log(LOG_INFORM,"permute_route: Generated %zu-hop route.",
		   *new_route_len);

	return new_route;
}

static struct command_result *
permuteroute_success(struct command *cmd, struct permuteroute_command *pc,
		     struct route_hop *route)
{
	size_t new_route_len;
	struct json_out *ret;

	tal_steal(pc, route);
	route = build_final_permuted_route(cmd, route, pc, &new_route_len);
	if (!route)
		return command_fail(cmd, LIGHTNINGD,
				    "permute_route: Route length "
				    "%zu > max_hops %"PRIu32"",
				    new_route_len, *pc->max_hops);

	ret = json_out_new(NULL);
	json_out_add_route(ret, route, NULL);

	return command_success(cmd, ret);
}

static struct command_result *
build_prefix_route(struct command *cmd,
		   struct route_hop *raw_subroute,
		   struct permuteroute_command *pc)
{
	size_t prefix_len, sub_len;
	struct route_hop *new_prefix_route;

	if (*pc->erring_index == 0)
		prefix_len = 0;
	else
		prefix_len = (size_t)*pc->erring_index;

	new_prefix_route = tal_dup_arr(cmd, struct route_hop,
				       (const struct route_hop *)pc->original_route,
				       prefix_len, 0);
	sub_len = tal_count(raw_subroute);

	tal_resize(&new_prefix_route, prefix_len + sub_len);
	/* Fill in the new extension.  */
	for (int i = 0; i < sub_len; i++)
		new_prefix_route[prefix_len + i] = raw_subroute[i];

	/* Smoothen the prefix.  */
	return smoothen_route(cmd, new_prefix_route, pc->source, pc);
}

static struct command_result *
getroute_for_permute_done(struct command *cmd,
			  const char *buf,
			  const jsmntok_t *result,
			  struct permuteroute_command *pc)
{
	struct route_hop *raw_subroute;
	const jsmntok_t *hop;
	const jsmntok_t *routetok = json_get_member(buf, result, "route");
	if (!routetok)
		plugin_err("getroute gave no 'route'? '%.*s'",
			   result->end - result->start, buf);

	/* FIXME: new command fail type */
	if (routetok->size == 0)
		return command_fail(cmd, LIGHTNINGD,
				    "permuteroute: Could not find a route");

	/* FIXME: should limit the PERMUTE_ROUTE_DISTANCE? */

	raw_subroute = tal_arr(pc, struct route_hop, routetok->size);
	size_t i;
	/* FIXME: update `getroute` document. */
	json_for_each_arr(i, hop, routetok) {
		struct amount_msat *msat, *amount_msat;
		struct node_id *id;
		struct short_channel_id *channel;
		unsigned *delay, *direction;

		// FIXME: add interface
		if (!param(cmd, buf, hop,
			   /* Only *one* of these is required */
			   p_opt("msatoshi", param_msat, &msat),
			   p_opt("amount_msat", param_msat, &amount_msat),
			   /* These three actually required */
			   p_opt("id", param_node_id, &id),
			   p_opt("delay", param_number, &delay),
			   p_opt("channel", param_short_channel_id, &channel),
			   p_opt("direction", param_number, &direction),
			   NULL))
			return NULL;

		if (!msat && !amount_msat)
			return command_fail(cmd, JSONRPC2_INVALID_PARAMS,
					    "route[%zi]: must have msatoshi"
					    " or amount_msat", i);

		if (!id || !channel || !delay)
			return command_fail(cmd, JSONRPC2_INVALID_PARAMS,
					    "route[%zi]: must have id, channel"
					    " and delay", i);

		if (msat && amount_msat && !amount_msat_eq(*msat, *amount_msat))
			return command_fail(cmd, JSONRPC2_INVALID_PARAMS,
					    "route[%zi]: msatoshi %s != amount_msat %s",
					    i,
					    type_to_string(tmpctx,
							   struct amount_msat,
							   msat),
					    type_to_string(tmpctx,
							   struct amount_msat,
							   amount_msat));
		if (!msat)
			msat = amount_msat;

		raw_subroute[i].amount = *msat;
		raw_subroute[i].nodeid = *id;
		raw_subroute[i].delay = *delay;
		raw_subroute[i].channel_id = *channel;
		/* FIXME: Actually ignored by sending code! */
		raw_subroute[i].direction = direction ? *direction : 0;
	}

	return build_prefix_route(cmd, raw_subroute, pc);
}

static struct command_result *
permuteroute_split_and_getroute(struct command *cmd,
				struct permuteroute_command *pc)
{
	struct json_out *req_params;

	/*~ The pivot is the specific node from which we start
	 * our search.
	 *
	 * permute_after indicates how many successful channel
	 * hops occurred.
	 * Thus if it is 0, we should pivot around the source,
	 * else we should pivot around permute_after - 1 in
	 * the route.
	 *
	 * We cannot permute_after at route length, as that
	 * implies that the entire route succeeded and there
	 * would be no reason to permute the route.
	 */
	if (*pc->erring_index == 0)
		pc->pivot = pc->source;
	else
		pc->pivot = &pc->original_route[*pc->erring_index - 1].nodeid;

	*pc->permute_index = *pc->erring_index + 1;

	/* OK, ask for route to destination */
	req_params = json_out_new(NULL);
	json_out_start(req_params, NULL, '{');
	/* FIXME: type_to_string only used in log */
	json_out_addstr(req_params, "id",
			take(node_id_to_hexstr(NULL,
			     &pc->original_route[*pc->erring_index + 1].nodeid)));
	json_out_addstr(req_params, "msatoshi",
			type_to_string(tmpctx, struct amount_msat, pc->msat));
	json_out_add(req_params, "riskfactor", false, "%f", *pc->riskfactor);
	json_out_add_u32(req_params, "cltv", *pc->cltv);
	json_out_add(req_params, "fromid", false, "%s",
		     take(node_id_to_hexstr(NULL, pc->pivot)));
	if (tal_count(pc->excludes) != 0) {
		json_out_start(req_params, "exclude", '[');
		for (size_t i = 0; i < tal_count(pc->excludes); i++)
			json_out_addstr(req_params, NULL, pc->excludes[i]);
		json_out_end(req_params, ']');
	}
	json_out_add_u32(req_params, "maxhops", *pc->max_hops);
	json_out_add_u32(req_params, "maxscannodes", *pc->max_scan_nodes);
	json_out_end(req_params, '}');

	/* FIXME: interface */
	return send_outreq(cmd, "getroute",
			   getroute_for_permute_done, forward_error, pc,
			   take(req_params));
}

static struct command_result *json_permuteroute(struct command *cmd,
						const char *buffer,
						const jsmntok_t *params)
{
	struct route_hop *route;
	struct amount_msat *msat;
	u32 *cltv;
	struct node_id *source;
	struct permuteroute_command *pc = tal(cmd, struct permuteroute_command);

	//FIXME: PERMUTE_ROUTING_MAX_SCAN_NODES
	if (!param(cmd, buffer, params,
		   p_req("route", param_route, &route),
		   p_req("erring_index", param_number, &pc->erring_index),
		   p_opt_def("riskfactor", param_double, &pc->riskfactor, 10),
		   p_opt("msatoshi", param_msat, &msat),
		   p_opt("cltv", param_number, &cltv),
		   p_opt("exclude", param_exclude_array, &pc->excludes),
		   p_opt("sourece", param_node_id, &source),
		   p_opt_def("maxhops", param_number, &pc->max_hops,
			     ROUTING_MAX_HOPS),
		   p_opt_def("maxscannodes", param_number, &pc->max_scan_nodes,
			     ROUTING_MAX_SCAN_NODES),
		   NULL))
		return NULL;

	/* FIXME: if erring_index >= tal_count(route) we can error
	 * at this point.
	 */

	if (!route || tal_count(route) == 0)
		return command_fail(cmd, LIGHTNINGD,
				    "permuteroute: Empty input route. No route?");

	pc->original_route = tal_steal(pc, route);

	if (*pc->erring_index >= tal_count(pc->original_route))
		return command_fail(cmd, LIGHTNINGD,
				    "permuteroute: erring_index(%"PRIu32") "
				    "is beyond the route",
				    *pc->erring_index);

	if (*pc->erring_index + 1 == tal_count(pc->original_route))
		return command_fail(cmd, LIGHTNINGD,
				    "Can't permuteroute: error is at "
				    "the destination(%s)",
				    take(node_id_to_hexstr(NULL,
					 &pc->original_route[*pc->erring_index].nodeid)));

	if (!msat) {
		pc->msat = tal(pc, struct amount_msat);
		*pc->msat = pc->original_route[*pc->erring_index + 1].amount;
	} else
		pc->msat = tal_steal(pc, msat);

	if (!cltv) {
		pc->cltv = tal(pc, u32);
		*pc->cltv = pc->original_route[*pc->erring_index + 1].delay;
	} else
		pc->cltv = tal_steal(pc, cltv);

	if (!source)
		pc->source = &my_id;
	else
		pc->source = tal_steal(pc, source);

	return permuteroute_split_and_getroute(cmd, pc);
}

static void init(struct plugin_conn *rpc,
		  const char *buf UNUSED, const jsmntok_t *config UNUSED)
{
	const char *field;

	field = rpc_delve(tmpctx, "getinfo",
			  take(json_out_obj(NULL, NULL, NULL)), rpc, ".id");
	if (!node_id_from_hexstr(field, strlen(field), &my_id))
		plugin_err("getinfo didn't contain valid id: '%s'", field);
}

static const struct plugin_command commands[] = { {
		"recalcroute",
		"channels",
		"Recompute the fees and delays of the given route, or fail if any "
		"channel or node is not existing in our local routemap.",
		"If msatoshi and cltv are unspecified, they will be extracted from "
		"the last hop of the input route.",
		json_recalcroute,
	}, {
		"smoothenroute",
		"channels",
		"Remove any loops from a route.",
		"This command is relevant when routes are concatenated, where "
		"each sub-part of the route was derived separately from other "
		"parts of the route, possibly passing through a node multiple "
		" times ",
		json_smoothenroute,
	}, {
		"permuteroute",
		"channels",
		"Modify a {route} that failed at the hop indexed by {erring_index}, "
		"returning a modified route to the same destination. ",
		"{exclude} an array of short-channel-id/direction (e.g. "
		"[ '564334x877x1/0', '564195x1292x0/1' ]) from consideration. "
		"If specified the route starts from {fromid} otherwise the route "
		"starts at this node. "
		"Set the {maxhops} the route can take (default 20).",
		json_permuteroute,
	}
};

int main(int argc, char *argv[])
{
	setup_locale();
	plugin_main(argv, init, PLUGIN_RESTARTABLE, commands, ARRAY_SIZE(commands), NULL);
}
