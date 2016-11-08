#ifndef LIGHTNING_STATE_H
#define LIGHTNING_STATE_H
#include "config.h"

#include <ccan/tal/tal.h>
#include <state_types.h>
#include <stdbool.h>

/*
 * This is the core state machine.
 *
 * Calling the state machine updates updates peer->state, and may call
 * various peer_ callbacks.  It also returns the status of the current
 * command.
 */

static inline bool state_is_error(enum state s)
{
	return s >= STATE_ERR_BREAKDOWN && s <= STATE_ERR_INTERNAL;
}

static inline bool state_is_shutdown(enum state s)
{
	return s == STATE_SHUTDOWN || s == STATE_SHUTDOWN_COMMITTING;
}

static inline bool state_is_onchain(enum state s)
{
	return s >= STATE_CLOSE_ONCHAIN_CHEATED
		&& s <= STATE_CLOSE_ONCHAIN_MUTUAL;
}

static inline bool state_is_normal(enum state s)
{
	return s == STATE_NORMAL || s == STATE_NORMAL_COMMITTING;
}

static inline bool state_is_opening(enum state s)
{
	return s < STATE_NORMAL;
}

static inline bool state_is_waiting_for_anchor(enum state s)
{
	return s == STATE_OPEN_WAITING_OURANCHOR
		|| s == STATE_OPEN_WAITING_OURANCHOR_THEYCOMPLETED
		|| s == STATE_OPEN_WAITING_THEIRANCHOR
		|| s == STATE_OPEN_WAITING_THEIRANCHOR_THEYCOMPLETED;
}

static inline bool state_can_io(enum state s)
{
	if (state_is_error(s))
		return false;
	if (s == STATE_CLOSED)
		return false;
	if (state_is_onchain(s))
		return false;
	return true;
}

static inline bool state_can_commit(enum state s)
{
	return s == STATE_NORMAL || s == STATE_SHUTDOWN;
}

/* BOLT #2:
 *
 * A node MUST NOT send a `update_add_htlc` after a `close_shutdown`
 */
static inline bool state_can_add_htlc(enum state s)
{
	return state_is_normal(s);
}

static inline bool state_can_remove_htlc(enum state s)
{
	return state_is_normal(s) || state_is_shutdown(s);
}

#endif /* LIGHTNING_STATE_H */
