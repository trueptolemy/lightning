#ifndef LIGHTNING_GOSSIPD_GOSSIPD_H
#define LIGHTNING_GOSSIPD_GOSSIPD_H
#include "config.h"
#include <bitcoin/block.h>
#include <ccan/bitmap/bitmap.h>
#include <ccan/list/list.h>
#include <ccan/short_types/short_types.h>
#include <ccan/timer/timer.h>
#include <common/bigsize.h>
#include <common/node_id.h>

/* We talk to `hsmd` to sign our gossip messages with the node key */
#define HSM_FD 3
/* connectd asks us for help finding nodes, and gossip fds for new peers */
#define CONNECTD_FD 4

struct chan;
struct broadcastable;

/*~ The core daemon structure: */
struct daemon {
	/* Who am I?  Helps us find ourself in the routing map. */
	struct node_id id;

	/* Peers we are gossiping to: id is unique */
	struct list_head peers;

	/* Current blockheight: 0 means we're not up-to-date. */
	u32 current_blockheight;

	/* Connection to lightningd. */
	struct daemon_conn *master;

	/* Connection to connect daemon. */
	struct daemon_conn *connectd;

	/* Routing information */
	struct routing_state *rstate;

	/* chainhash for checking/making gossip msgs */
	struct bitcoin_blkid chain_hash;

	/* Timers: we batch gossip, and also refresh announcements */
	struct timers timers;

	/* Global features to list in node_announcement. */
	u8 *globalfeatures;

	/* Alias (not NUL terminated) and favorite color for node_announcement */
	u8 alias[32];
	u8 rgb[3];

	/* What addresses we can actually announce. */
	struct wireaddr *announcable;

	/* Do we think we're missing gossip?  Contains timer to re-check */
	struct oneshot *gossip_missing;

	/* Channels we've heard about, but don't know. */
	struct short_channel_id *unknown_scids;

	/* Timer until we can send a new node_announcement */
	struct oneshot *node_announce_timer;

	/* Channels we have an announce for, but aren't deep enough. */
	struct short_channel_id *deferred_txouts;
};

/*~ How gossipy do we ask a peer to be? */
enum gossip_level {
	/* Give us everything since epoch */
	GOSSIP_HIGH,
	/* Give us everything from 24 hours ago. */
	GOSSIP_MEDIUM,
	/* Give us everything from now. */
	GOSSIP_LOW,
	/* Give us nothing. */
	GOSSIP_NONE,
};

/* This represents each peer we're gossiping with */
struct peer {
	/* daemon->peers */
	struct list_node list;

	/* parent pointer. */
	struct daemon *daemon;

	/* The ID of the peer (always unique) */
	struct node_id id;

	/* The two features gossip cares about (so far) */
	bool gossip_queries_feature, initial_routing_sync_feature;

	/* Are there outstanding responses for queries on short_channel_ids? */
	const struct short_channel_id *scid_queries;
	const bigsize_t *scid_query_flags;
	size_t scid_query_idx;

	/* Are there outstanding node_announcements from scid_queries? */
	struct node_id *scid_query_nodes;
	size_t scid_query_nodes_idx;

	/* Do we have an scid_query outstanding?  Was it internal? */
	bool scid_query_outstanding;
	bool scid_query_was_internal;

	/* How many pongs are we expecting? */
	size_t num_pings_outstanding;

	/* Map of outstanding channel_range requests. */
	bitmap *query_channel_blocks;
	/* What we're querying: [range_first_blocknum, range_end_blocknum) */
	u32 range_first_blocknum, range_end_blocknum;
	u32 range_blocks_remaining;
	struct short_channel_id *query_channel_scids;

	/* Are we asking this peer to give us lot of gossip? */
	enum gossip_level gossip_level;

	/* The daemon_conn used to queue messages to/from the peer. */
	struct daemon_conn *dc;
};

/* Search for a peer. */
struct peer *find_peer(struct daemon *daemon, const struct node_id *id);

/* Queue a gossip message for the peer: the subdaemon on the other end simply
 * forwards it to the peer. */
void queue_peer_msg(struct peer *peer, const u8 *msg TAKES);

/* Queue a gossip_store message for the peer: the subdaemon on the
 * other end simply forwards it to the peer. */
void queue_peer_from_store(struct peer *peer,
			   const struct broadcastable *bcast);

#endif /* LIGHTNING_GOSSIPD_GOSSIPD_H */