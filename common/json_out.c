#include <bitcoin/short_channel_id.h>
#include <ccan/json_out/json_out.h>
#include <common/amount.h>
#include <common/json_out.h>
#include <common/node_id.h>
#include <common/route_hop.h>
#include <common/type_to_string.h>
#include <inttypes.h>

void json_out_add_u64(struct json_out *jout, const char *fieldname,
		      u64 val)
{
	json_out_add(jout, fieldname, false, "%"PRIu64, val);
}

void json_out_add_u32(struct json_out *jout, const char *fieldname,
		      u32 val)
{
	json_out_add(jout, fieldname, false, "%u", val);
}

void json_out_add_int(struct json_out *jout, const char *fieldname,
		      int val)
{
	json_out_add(jout, fieldname, false, "%d", val);
}

void json_out_add_amount_msat_only(struct json_out *jout, const char *msatfieldname,
				   struct amount_msat msat)
{
	json_out_addstr(jout, msatfieldname,
			type_to_string(tmpctx, struct amount_msat, &msat));
}

void json_out_add_amount_msat_compat(struct json_out *jout, struct amount_msat msat,
				     const char *rawfieldname, const char *msatfieldname)
{
	json_out_add_u64(jout, rawfieldname, msat.millisatoshis); /* Raw: low-level helper */
	json_out_add_amount_msat_only(jout, msatfieldname, msat);
}

void json_out_add_route_hop(struct json_out *jout, struct route_hop *hop)
{
	json_out_start(jout, NULL, '{');
	json_out_addstr(jout, "id",
			take(node_id_to_hexstr(NULL,
			     &hop->nodeid)));
	json_out_addstr(jout, "channel",
			take(short_channel_id_to_str(NULL,
			     &hop->channel_id)));
	json_out_add_int(jout, "direction", hop->direction);
	json_out_add_amount_msat_compat(jout, hop->amount,
					"msatoshi", "amount_msat");
	json_out_add_u32(jout, "delay", hop->delay);
	json_out_end(jout, '}');
}

void json_out_add_route(struct json_out *jout, struct route_hop *route,
			struct route_hop *hint)
{
	json_out_start(jout, NULL, '{');
	json_out_start(jout, "route", '[');

	for(size_t i = 0; i < tal_count(route); i++)
		json_out_add_route_hop(jout, &route[i]);

	if (hint)
		json_out_add_route_hop(jout, hint);

	json_out_end(jout, ']');
	json_out_end(jout, '}');
}
