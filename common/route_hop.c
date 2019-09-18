#include <bitcoin/short_channel_id.h>
#include <ccan/tal/str/str.h>
#include <common/amount.h>
#include <common/json_command.h>
#include <common/json_tok.h>
#include <common/jsonrpc_errors.h>
#include <common/node_id.h>
#include <common/param.h>
#include <common/route_hop.h>
#include <common/type_to_string.h>
#include <common/utils.h>

struct command_result *param_route_hop(struct command *cmd, const char *name,
				       const char *buffer, const jsmntok_t *tok,
				       struct route_hop **hop)
{
	struct amount_msat *msat, *amount_msat;
	struct node_id *id;
	struct short_channel_id *channel;
	unsigned *delay, *direction;
	*hop = tal(cmd, struct route_hop);

	if (!param(cmd, buffer, tok,
		   /* Only *one* of these is required */
		   p_opt("msatoshi", param_msat, &msat),
		   p_opt("amount_msat", param_msat, &amount_msat),
		   /* These three actually required */
		   p_opt("id", param_node_id, &id),
		   p_opt("delay", param_number, &delay),
		   p_opt("channel", param_short_channel_id, &channel),
		   p_opt("direction", param_number, &direction),
		   NULL))
		return command_fail(cmd, JSONRPC2_INVALID_PARAMS,
				    "invalid route hop");

	if (!msat && !amount_msat)
		return command_fail(cmd, JSONRPC2_INVALID_PARAMS,
				    "route hop: must have msatoshi"
				    " or amount_msat");
	if (!id || !channel || !delay)
		return command_fail(cmd, JSONRPC2_INVALID_PARAMS,
				    "route hop: must have id, channel"
				    " and delay");
	if (msat && amount_msat && !amount_msat_eq(*msat, *amount_msat))
		return command_fail(cmd, JSONRPC2_INVALID_PARAMS,
				    "route hop: msatoshi %s != amount_msat %s",
				    type_to_string(tmpctx,
						   struct amount_msat,
						   msat),
				    type_to_string(tmpctx,
						   struct amount_msat,
						   amount_msat));
	if (!msat)
		msat = amount_msat;

	(*hop)->amount = *msat;
	(*hop)->nodeid = *id;
	(*hop)->delay = *delay;
	(*hop)->channel_id = *channel;
	/* FIXME: Actually ignored by sendpay code! */
	(*hop)->direction = direction ? *direction : 0;

	return NULL;
}

struct command_result *param_route(struct command *cmd, const char *name,
				   const char *buffer, const jsmntok_t *tok,
				   struct route_hop **route)
{
	struct command_result *result;
	const jsmntok_t *routetok;
	const jsmntok_t *t;
	size_t i;

	result = param_array(cmd, name, buffer, tok, &routetok);
	if (result)
		return result;

	if (routetok->size == 0)
		return command_fail(cmd, JSONRPC2_INVALID_PARAMS,
				    "Empty route");

	*route = tal_arr(cmd, struct route_hop, routetok->size);
	json_for_each_arr(i, t, routetok) {
		struct amount_msat *msat, *amount_msat;
		struct node_id *id;
		struct short_channel_id *channel;
		unsigned *delay, *direction;

		if (!param(cmd, buffer, t,
			   /* Only *one* of these is required */
			   p_opt("msatoshi", param_msat, &msat),
			   p_opt("amount_msat", param_msat, &amount_msat),
			   /* These three actually required */
			   p_opt("id", param_node_id, &id),
			   p_opt("delay", param_number, &delay),
			   p_opt("channel", param_short_channel_id, &channel),
			   p_opt("direction", param_number, &direction),
			   NULL))
			return command_fail(cmd, JSONRPC2_INVALID_PARAMS,
					    "invalid route[%zi]", i);

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

		(*route)[i].amount = *msat;
		(*route)[i].nodeid = *id;
		(*route)[i].delay = *delay;
		(*route)[i].channel_id = *channel;
		/* FIXME: Actually ignored by sendpay code! */
		(*route)[i].direction = direction ? *direction : 0;
	}

	return NULL;
}
