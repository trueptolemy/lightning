/* helper for route_hop */
#ifndef LIGHTNING_COMMON_ROUTE_HOP_H
#define LIGHTNING_COMMON_ROUTE_HOP_H
#include "config.h"
#include <ccan/short_types/short_types.h>
#include <common/json.h>

struct command;
struct command_result;

struct route_hop {
	struct short_channel_id channel_id;
	int direction;
	struct node_id nodeid;
	struct amount_msat amount;
	u32 delay;
};

struct command_result *param_route_hop(struct command *cmd, const char *name,
				       const char *buffer, const jsmntok_t *tok,
				       struct route_hop **hop);

struct command_result *param_route(struct command *cmd, const char *name,
				   const char *buffer, const jsmntok_t *tok,
				   struct route_hop **route);

#endif /* LIGHTNING_COMMON_ROUTE_HOP_H */