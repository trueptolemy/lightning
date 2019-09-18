/* More specialized json_out helpers. */
#ifndef LIGHTNING_COMMON_JSON_OUT_H
#define LIGHTNING_COMMON_JSON_OUT_H
#include "config.h"
#include <ccan/short_types/short_types.h>

struct amount_msat;
struct json_out;
struct route_hop;

void json_out_add_u64(struct json_out *jout, const char *fieldname, u64 val);

void json_out_add_u32(struct json_out *jout, const char *fieldname, u32 val);

void json_out_add_int(struct json_out *jout, const char *fieldname, int val);

void json_out_add_amount_msat_only(struct json_out *jout, const char *msatfieldname,
				   struct amount_msat msat);

void json_out_add_amount_msat_compat(struct json_out *jout, struct amount_msat msat,
				     const char *rawfieldname, const char *msatfieldname);

void json_out_add_route_hop(struct json_out *jout, struct route_hop *hop);

void json_out_add_route(struct json_out *jout, struct route_hop *route,
			struct route_hop *hint);

#endif /* LIGHTNING_COMMON_JSON_OUT_H */
