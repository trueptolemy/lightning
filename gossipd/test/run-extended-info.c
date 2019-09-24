#include "config.h"

#if EXPERIMENTAL_FEATURES == 0
/* Can't run without EXPERIMENTAL_FEATURES */
int main(void)
{
	return 0;
}
#else
#define main gossipd_main
int gossipd_main(int argc, char *argv[]);

#define ZLIB_EVEN_IF_EXPANDS 1

#include "../gossipd.c"
#undef main
#include <ccan/str/hex/hex.h>
#include <common/json.h>
#include <common/json_helpers.h>
#include <stdio.h>

#ifdef NDEBUG
#error "assert required for tests"
#endif

/* AUTOGENERATED MOCKS START */
/* Generated stub for check_ping_make_pong */
bool check_ping_make_pong(const tal_t *ctx UNNEEDED, const u8 *ping UNNEEDED, u8 **pong UNNEEDED)
{ fprintf(stderr, "check_ping_make_pong called!\n"); abort(); }
/* Generated stub for daemon_conn_new_ */
struct daemon_conn *daemon_conn_new_(const tal_t *ctx UNNEEDED, int fd UNNEEDED,
				     struct io_plan *(*recv)(struct io_conn * UNNEEDED,
							     const u8 * UNNEEDED,
							     void *) UNNEEDED,
				     void (*outq_empty)(void *) UNNEEDED,
				     void *arg UNNEEDED)
{ fprintf(stderr, "daemon_conn_new_ called!\n"); abort(); }
/* Generated stub for daemon_conn_read_next */
struct io_plan *daemon_conn_read_next(struct io_conn *conn UNNEEDED,
				      struct daemon_conn *dc UNNEEDED)
{ fprintf(stderr, "daemon_conn_read_next called!\n"); abort(); }
/* Generated stub for daemon_conn_send */
void daemon_conn_send(struct daemon_conn *dc UNNEEDED, const u8 *msg UNNEEDED)
{ fprintf(stderr, "daemon_conn_send called!\n"); abort(); }
/* Generated stub for daemon_conn_send_fd */
void daemon_conn_send_fd(struct daemon_conn *dc UNNEEDED, int fd UNNEEDED)
{ fprintf(stderr, "daemon_conn_send_fd called!\n"); abort(); }
/* Generated stub for daemon_conn_wake */
void daemon_conn_wake(struct daemon_conn *dc UNNEEDED)
{ fprintf(stderr, "daemon_conn_wake called!\n"); abort(); }
/* Generated stub for daemon_shutdown */
void daemon_shutdown(void)
{ fprintf(stderr, "daemon_shutdown called!\n"); abort(); }
/* Generated stub for decode_scid_query_flags */
bigsize_t *decode_scid_query_flags(const tal_t *ctx UNNEEDED,
				   const struct tlv_query_short_channel_ids_tlvs_query_flags *qf UNNEEDED)
{ fprintf(stderr, "decode_scid_query_flags called!\n"); abort(); }
/* Generated stub for decode_short_ids */
struct short_channel_id *decode_short_ids(const tal_t *ctx UNNEEDED, const u8 *encoded UNNEEDED)
{ fprintf(stderr, "decode_short_ids called!\n"); abort(); }
/* Generated stub for dump_memleak */
bool dump_memleak(struct htable *memtable UNNEEDED)
{ fprintf(stderr, "dump_memleak called!\n"); abort(); }
/* Generated stub for first_chan */
struct chan *first_chan(const struct node *node UNNEEDED, struct chan_map_iter *i UNNEEDED)
{ fprintf(stderr, "first_chan called!\n"); abort(); }
/* Generated stub for free_chan */
void free_chan(struct routing_state *rstate UNNEEDED, struct chan *chan UNNEEDED)
{ fprintf(stderr, "free_chan called!\n"); abort(); }
/* Generated stub for fromwire_amount_below_minimum */
bool fromwire_amount_below_minimum(const tal_t *ctx UNNEEDED, const void *p UNNEEDED, struct amount_msat *htlc_msat UNNEEDED, u8 **channel_update UNNEEDED)
{ fprintf(stderr, "fromwire_amount_below_minimum called!\n"); abort(); }
/* Generated stub for fromwire_expiry_too_soon */
bool fromwire_expiry_too_soon(const tal_t *ctx UNNEEDED, const void *p UNNEEDED, u8 **channel_update UNNEEDED)
{ fprintf(stderr, "fromwire_expiry_too_soon called!\n"); abort(); }
/* Generated stub for fromwire_fee_insufficient */
bool fromwire_fee_insufficient(const tal_t *ctx UNNEEDED, const void *p UNNEEDED, struct amount_msat *htlc_msat UNNEEDED, u8 **channel_update UNNEEDED)
{ fprintf(stderr, "fromwire_fee_insufficient called!\n"); abort(); }
/* Generated stub for fromwire_gossipctl_init */
bool fromwire_gossipctl_init(const tal_t *ctx UNNEEDED, const void *p UNNEEDED, struct bitcoin_blkid *chain_hash UNNEEDED, struct node_id *id UNNEEDED, u8 **globalfeatures UNNEEDED, u8 rgb[3] UNNEEDED, u8 alias[32] UNNEEDED, u32 *update_channel_interval UNNEEDED, struct wireaddr **announcable UNNEEDED, u32 **dev_gossip_time UNNEEDED)
{ fprintf(stderr, "fromwire_gossipctl_init called!\n"); abort(); }
/* Generated stub for fromwire_gossip_dev_set_max_scids_encode_size */
bool fromwire_gossip_dev_set_max_scids_encode_size(const void *p UNNEEDED, u32 *max UNNEEDED)
{ fprintf(stderr, "fromwire_gossip_dev_set_max_scids_encode_size called!\n"); abort(); }
/* Generated stub for fromwire_gossip_dev_set_time */
bool fromwire_gossip_dev_set_time(const void *p UNNEEDED, u32 *dev_gossip_time UNNEEDED)
{ fprintf(stderr, "fromwire_gossip_dev_set_time called!\n"); abort(); }
/* Generated stub for fromwire_gossip_dev_suppress */
bool fromwire_gossip_dev_suppress(const void *p UNNEEDED)
{ fprintf(stderr, "fromwire_gossip_dev_suppress called!\n"); abort(); }
/* Generated stub for fromwire_gossipd_get_update */
bool fromwire_gossipd_get_update(const void *p UNNEEDED, struct short_channel_id *short_channel_id UNNEEDED)
{ fprintf(stderr, "fromwire_gossipd_get_update called!\n"); abort(); }
/* Generated stub for fromwire_gossipd_local_channel_update */
bool fromwire_gossipd_local_channel_update(const void *p UNNEEDED, struct short_channel_id *short_channel_id UNNEEDED, bool *disable UNNEEDED, u16 *cltv_expiry_delta UNNEEDED, struct amount_msat *htlc_minimum_msat UNNEEDED, u32 *fee_base_msat UNNEEDED, u32 *fee_proportional_millionths UNNEEDED, struct amount_msat *htlc_maximum_msat UNNEEDED)
{ fprintf(stderr, "fromwire_gossipd_local_channel_update called!\n"); abort(); }
/* Generated stub for fromwire_gossip_get_addrs */
bool fromwire_gossip_get_addrs(const void *p UNNEEDED, struct node_id *id UNNEEDED)
{ fprintf(stderr, "fromwire_gossip_get_addrs called!\n"); abort(); }
/* Generated stub for fromwire_gossip_get_channel_peer */
bool fromwire_gossip_get_channel_peer(const void *p UNNEEDED, struct short_channel_id *channel_id UNNEEDED)
{ fprintf(stderr, "fromwire_gossip_get_channel_peer called!\n"); abort(); }
/* Generated stub for fromwire_gossip_getchannels_request */
bool fromwire_gossip_getchannels_request(const tal_t *ctx UNNEEDED, const void *p UNNEEDED, struct short_channel_id **short_channel_id UNNEEDED, struct node_id **source UNNEEDED, struct short_channel_id **prev UNNEEDED)
{ fprintf(stderr, "fromwire_gossip_getchannels_request called!\n"); abort(); }
/* Generated stub for fromwire_gossip_get_incoming_channels */
bool fromwire_gossip_get_incoming_channels(const tal_t *ctx UNNEEDED, const void *p UNNEEDED, bool **private_too UNNEEDED)
{ fprintf(stderr, "fromwire_gossip_get_incoming_channels called!\n"); abort(); }
/* Generated stub for fromwire_gossip_getnodes_request */
bool fromwire_gossip_getnodes_request(const tal_t *ctx UNNEEDED, const void *p UNNEEDED, struct node_id **id UNNEEDED)
{ fprintf(stderr, "fromwire_gossip_getnodes_request called!\n"); abort(); }
/* Generated stub for fromwire_gossip_getroute_request */
bool fromwire_gossip_getroute_request(const tal_t *ctx UNNEEDED, const void *p UNNEEDED, struct node_id **source UNNEEDED, struct node_id *destination UNNEEDED, struct amount_msat *msatoshi UNNEEDED, u64 *riskfactor_by_million UNNEEDED, u32 *final_cltv UNNEEDED, double *fuzz UNNEEDED, struct exclude_entry ***excluded UNNEEDED, u32 *max_hops UNNEEDED)
{ fprintf(stderr, "fromwire_gossip_getroute_request called!\n"); abort(); }
/* Generated stub for fromwire_gossip_get_txout_reply */
bool fromwire_gossip_get_txout_reply(const tal_t *ctx UNNEEDED, const void *p UNNEEDED, struct short_channel_id *short_channel_id UNNEEDED, struct amount_sat *satoshis UNNEEDED, u8 **outscript UNNEEDED)
{ fprintf(stderr, "fromwire_gossip_get_txout_reply called!\n"); abort(); }
/* Generated stub for fromwire_gossip_local_channel_close */
bool fromwire_gossip_local_channel_close(const void *p UNNEEDED, struct short_channel_id *short_channel_id UNNEEDED)
{ fprintf(stderr, "fromwire_gossip_local_channel_close called!\n"); abort(); }
/* Generated stub for fromwire_gossip_new_peer */
bool fromwire_gossip_new_peer(const void *p UNNEEDED, struct node_id *id UNNEEDED, bool *gossip_queries_feature UNNEEDED, bool *initial_routing_sync UNNEEDED)
{ fprintf(stderr, "fromwire_gossip_new_peer called!\n"); abort(); }
/* Generated stub for fromwire_gossip_outpoint_spent */
bool fromwire_gossip_outpoint_spent(const void *p UNNEEDED, struct short_channel_id *short_channel_id UNNEEDED)
{ fprintf(stderr, "fromwire_gossip_outpoint_spent called!\n"); abort(); }
/* Generated stub for fromwire_gossip_payment_failure */
bool fromwire_gossip_payment_failure(const tal_t *ctx UNNEEDED, const void *p UNNEEDED, struct node_id *erring_node UNNEEDED, struct short_channel_id *erring_channel UNNEEDED, u8 *erring_channel_direction UNNEEDED, u8 **error UNNEEDED)
{ fprintf(stderr, "fromwire_gossip_payment_failure called!\n"); abort(); }
/* Generated stub for fromwire_gossip_ping */
bool fromwire_gossip_ping(const void *p UNNEEDED, struct node_id *id UNNEEDED, u16 *num_pong_bytes UNNEEDED, u16 *len UNNEEDED)
{ fprintf(stderr, "fromwire_gossip_ping called!\n"); abort(); }
/* Generated stub for fromwire_gossip_query_channel_range */
bool fromwire_gossip_query_channel_range(const void *p UNNEEDED, struct node_id *id UNNEEDED, u32 *first_blocknum UNNEEDED, u32 *number_of_blocks UNNEEDED)
{ fprintf(stderr, "fromwire_gossip_query_channel_range called!\n"); abort(); }
/* Generated stub for fromwire_gossip_query_scids */
bool fromwire_gossip_query_scids(const tal_t *ctx UNNEEDED, const void *p UNNEEDED, struct node_id *id UNNEEDED, struct short_channel_id **ids UNNEEDED)
{ fprintf(stderr, "fromwire_gossip_query_scids called!\n"); abort(); }
/* Generated stub for fromwire_gossip_send_timestamp_filter */
bool fromwire_gossip_send_timestamp_filter(const void *p UNNEEDED, struct node_id *id UNNEEDED, u32 *first_timestamp UNNEEDED, u32 *timestamp_range UNNEEDED)
{ fprintf(stderr, "fromwire_gossip_send_timestamp_filter called!\n"); abort(); }
/* Generated stub for fromwire_hsm_cupdate_sig_reply */
bool fromwire_hsm_cupdate_sig_reply(const tal_t *ctx UNNEEDED, const void *p UNNEEDED, u8 **cu UNNEEDED)
{ fprintf(stderr, "fromwire_hsm_cupdate_sig_reply called!\n"); abort(); }
/* Generated stub for fromwire_hsm_node_announcement_sig_reply */
bool fromwire_hsm_node_announcement_sig_reply(const void *p UNNEEDED, secp256k1_ecdsa_signature *signature UNNEEDED)
{ fprintf(stderr, "fromwire_hsm_node_announcement_sig_reply called!\n"); abort(); }
/* Generated stub for fromwire_incorrect_cltv_expiry */
bool fromwire_incorrect_cltv_expiry(const tal_t *ctx UNNEEDED, const void *p UNNEEDED, u32 *cltv_expiry UNNEEDED, u8 **channel_update UNNEEDED)
{ fprintf(stderr, "fromwire_incorrect_cltv_expiry called!\n"); abort(); }
/* Generated stub for fromwire_temporary_channel_failure */
bool fromwire_temporary_channel_failure(const tal_t *ctx UNNEEDED, const void *p UNNEEDED, u8 **channel_update UNNEEDED)
{ fprintf(stderr, "fromwire_temporary_channel_failure called!\n"); abort(); }
/* Generated stub for get_node */
struct node *get_node(struct routing_state *rstate UNNEEDED,
		      const struct node_id *id UNNEEDED)
{ fprintf(stderr, "get_node called!\n"); abort(); }
/* Generated stub for get_route */
struct route_hop *get_route(const tal_t *ctx UNNEEDED, struct routing_state *rstate UNNEEDED,
			    const struct node_id *source UNNEEDED,
			    const struct node_id *destination UNNEEDED,
			    const struct amount_msat msat UNNEEDED, double riskfactor UNNEEDED,
			    u32 final_cltv UNNEEDED,
			    double fuzz UNNEEDED,
			    u64 seed UNNEEDED,
			    struct exclude_entry **excluded UNNEEDED,
			    u32 max_hops UNNEEDED)
{ fprintf(stderr, "get_route called!\n"); abort(); }
/* Generated stub for gossip_peerd_wire_type_name */
const char *gossip_peerd_wire_type_name(int e UNNEEDED)
{ fprintf(stderr, "gossip_peerd_wire_type_name called!\n"); abort(); }
/* Generated stub for gossip_store_compact */
bool gossip_store_compact(struct gossip_store *gs UNNEEDED)
{ fprintf(stderr, "gossip_store_compact called!\n"); abort(); }
/* Generated stub for gossip_store_get */
const u8 *gossip_store_get(const tal_t *ctx UNNEEDED,
			   struct gossip_store *gs UNNEEDED,
			   u64 offset UNNEEDED)
{ fprintf(stderr, "gossip_store_get called!\n"); abort(); }
/* Generated stub for gossip_store_load */
bool gossip_store_load(struct routing_state *rstate UNNEEDED, struct gossip_store *gs UNNEEDED)
{ fprintf(stderr, "gossip_store_load called!\n"); abort(); }
/* Generated stub for gossip_store_readonly_fd */
int gossip_store_readonly_fd(struct gossip_store *gs UNNEEDED)
{ fprintf(stderr, "gossip_store_readonly_fd called!\n"); abort(); }
/* Generated stub for gossip_time_now */
struct timeabs gossip_time_now(const struct routing_state *rstate UNNEEDED)
{ fprintf(stderr, "gossip_time_now called!\n"); abort(); }
/* Generated stub for got_pong */
const char *got_pong(const u8 *pong UNNEEDED, size_t *num_pings_outstanding UNNEEDED)
{ fprintf(stderr, "got_pong called!\n"); abort(); }
/* Generated stub for handle_channel_announcement */
u8 *handle_channel_announcement(struct routing_state *rstate UNNEEDED,
				const u8 *announce TAKES UNNEEDED,
				const struct short_channel_id **scid UNNEEDED)
{ fprintf(stderr, "handle_channel_announcement called!\n"); abort(); }
/* Generated stub for handle_channel_update */
u8 *handle_channel_update(struct routing_state *rstate UNNEEDED, const u8 *update TAKES UNNEEDED,
			  const char *source UNNEEDED,
			  struct short_channel_id *unknown_scid UNNEEDED)
{ fprintf(stderr, "handle_channel_update called!\n"); abort(); }
/* Generated stub for handle_local_add_channel */
bool handle_local_add_channel(struct routing_state *rstate UNNEEDED, const u8 *msg UNNEEDED,
			      u64 index UNNEEDED)
{ fprintf(stderr, "handle_local_add_channel called!\n"); abort(); }
/* Generated stub for handle_node_announcement */
u8 *handle_node_announcement(struct routing_state *rstate UNNEEDED, const u8 *node UNNEEDED)
{ fprintf(stderr, "handle_node_announcement called!\n"); abort(); }
/* Generated stub for handle_pending_cannouncement */
bool handle_pending_cannouncement(struct routing_state *rstate UNNEEDED,
				  const struct short_channel_id *scid UNNEEDED,
				  const struct amount_sat sat UNNEEDED,
				  const u8 *txscript UNNEEDED)
{ fprintf(stderr, "handle_pending_cannouncement called!\n"); abort(); }
/* Generated stub for make_ping */
u8 *make_ping(const tal_t *ctx UNNEEDED, u16 num_pong_bytes UNNEEDED, u16 padlen UNNEEDED)
{ fprintf(stderr, "make_ping called!\n"); abort(); }
/* Generated stub for master_badmsg */
void master_badmsg(u32 type_expected UNNEEDED, const u8 *msg)
{ fprintf(stderr, "master_badmsg called!\n"); abort(); }
/* Generated stub for memleak_enter_allocations */
struct htable *memleak_enter_allocations(const tal_t *ctx UNNEEDED,
					 const void *exclude1 UNNEEDED,
					 const void *exclude2 UNNEEDED)
{ fprintf(stderr, "memleak_enter_allocations called!\n"); abort(); }
/* Generated stub for memleak_remove_referenced */
void memleak_remove_referenced(struct htable *memtable UNNEEDED, const void *root UNNEEDED)
{ fprintf(stderr, "memleak_remove_referenced called!\n"); abort(); }
/* Generated stub for new_reltimer_ */
struct oneshot *new_reltimer_(struct timers *timers UNNEEDED,
			      const tal_t *ctx UNNEEDED,
			      struct timerel expire UNNEEDED,
			      void (*cb)(void *) UNNEEDED, void *arg UNNEEDED)
{ fprintf(stderr, "new_reltimer_ called!\n"); abort(); }
/* Generated stub for new_routing_state */
struct routing_state *new_routing_state(const tal_t *ctx UNNEEDED,
					const struct chainparams *chainparams UNNEEDED,
					const struct node_id *local_id UNNEEDED,
					u32 prune_timeout UNNEEDED,
					struct list_head *peers UNNEEDED,
					const u32 *dev_gossip_time UNNEEDED)
{ fprintf(stderr, "new_routing_state called!\n"); abort(); }
/* Generated stub for next_chan */
struct chan *next_chan(const struct node *node UNNEEDED, struct chan_map_iter *i UNNEEDED)
{ fprintf(stderr, "next_chan called!\n"); abort(); }
/* Generated stub for notleak_ */
void *notleak_(const void *ptr UNNEEDED, bool plus_children UNNEEDED)
{ fprintf(stderr, "notleak_ called!\n"); abort(); }
/* Generated stub for read_addresses */
struct wireaddr *read_addresses(const tal_t *ctx UNNEEDED, const u8 *ser UNNEEDED)
{ fprintf(stderr, "read_addresses called!\n"); abort(); }
/* Generated stub for remove_channel_from_store */
void remove_channel_from_store(struct routing_state *rstate UNNEEDED,
			       struct chan *chan UNNEEDED)
{ fprintf(stderr, "remove_channel_from_store called!\n"); abort(); }
/* Generated stub for route_prune */
void route_prune(struct routing_state *rstate UNNEEDED)
{ fprintf(stderr, "route_prune called!\n"); abort(); }
/* Generated stub for routing_failure */
void routing_failure(struct routing_state *rstate UNNEEDED,
		     const struct node_id *erring_node UNNEEDED,
		     const struct short_channel_id *erring_channel UNNEEDED,
		     int erring_direction UNNEEDED,
		     enum onion_type failcode UNNEEDED,
		     const u8 *channel_update UNNEEDED)
{ fprintf(stderr, "routing_failure called!\n"); abort(); }
/* Generated stub for status_failed */
void status_failed(enum status_failreason code UNNEEDED,
		   const char *fmt UNNEEDED, ...)
{ fprintf(stderr, "status_failed called!\n"); abort(); }
/* Generated stub for status_setup_async */
void status_setup_async(struct daemon_conn *master UNNEEDED)
{ fprintf(stderr, "status_setup_async called!\n"); abort(); }
/* Generated stub for subdaemon_setup */
void subdaemon_setup(int argc UNNEEDED, char *argv[])
{ fprintf(stderr, "subdaemon_setup called!\n"); abort(); }
/* Generated stub for timer_expired */
void timer_expired(tal_t *ctx UNNEEDED, struct timer *timer UNNEEDED)
{ fprintf(stderr, "timer_expired called!\n"); abort(); }
/* Generated stub for towire_errorfmt */
u8 *towire_errorfmt(const tal_t *ctx UNNEEDED,
		    const struct channel_id *channel UNNEEDED,
		    const char *fmt UNNEEDED, ...)
{ fprintf(stderr, "towire_errorfmt called!\n"); abort(); }
/* Generated stub for towire_gossip_dev_compact_store_reply */
u8 *towire_gossip_dev_compact_store_reply(const tal_t *ctx UNNEEDED, bool success UNNEEDED)
{ fprintf(stderr, "towire_gossip_dev_compact_store_reply called!\n"); abort(); }
/* Generated stub for towire_gossip_dev_memleak_reply */
u8 *towire_gossip_dev_memleak_reply(const tal_t *ctx UNNEEDED, bool leak UNNEEDED)
{ fprintf(stderr, "towire_gossip_dev_memleak_reply called!\n"); abort(); }
/* Generated stub for towire_gossipd_get_update_reply */
u8 *towire_gossipd_get_update_reply(const tal_t *ctx UNNEEDED, const u8 *update UNNEEDED)
{ fprintf(stderr, "towire_gossipd_get_update_reply called!\n"); abort(); }
/* Generated stub for towire_gossipd_new_store_fd */
u8 *towire_gossipd_new_store_fd(const tal_t *ctx UNNEEDED, u64 offset_shorter UNNEEDED)
{ fprintf(stderr, "towire_gossipd_new_store_fd called!\n"); abort(); }
/* Generated stub for towire_gossip_get_addrs_reply */
u8 *towire_gossip_get_addrs_reply(const tal_t *ctx UNNEEDED, const struct wireaddr *addrs UNNEEDED)
{ fprintf(stderr, "towire_gossip_get_addrs_reply called!\n"); abort(); }
/* Generated stub for towire_gossip_get_channel_peer_reply */
u8 *towire_gossip_get_channel_peer_reply(const tal_t *ctx UNNEEDED, const struct node_id *peer_id UNNEEDED)
{ fprintf(stderr, "towire_gossip_get_channel_peer_reply called!\n"); abort(); }
/* Generated stub for towire_gossip_getchannels_reply */
u8 *towire_gossip_getchannels_reply(const tal_t *ctx UNNEEDED, bool complete UNNEEDED, const struct gossip_getchannels_entry **nodes UNNEEDED)
{ fprintf(stderr, "towire_gossip_getchannels_reply called!\n"); abort(); }
/* Generated stub for towire_gossip_get_incoming_channels_reply */
u8 *towire_gossip_get_incoming_channels_reply(const tal_t *ctx UNNEEDED, const struct route_info *route_info UNNEEDED)
{ fprintf(stderr, "towire_gossip_get_incoming_channels_reply called!\n"); abort(); }
/* Generated stub for towire_gossip_getnodes_reply */
u8 *towire_gossip_getnodes_reply(const tal_t *ctx UNNEEDED, const struct gossip_getnodes_entry **nodes UNNEEDED)
{ fprintf(stderr, "towire_gossip_getnodes_reply called!\n"); abort(); }
/* Generated stub for towire_gossip_getroute_reply */
u8 *towire_gossip_getroute_reply(const tal_t *ctx UNNEEDED, const struct route_hop *hops UNNEEDED)
{ fprintf(stderr, "towire_gossip_getroute_reply called!\n"); abort(); }
/* Generated stub for towire_gossip_get_txout */
u8 *towire_gossip_get_txout(const tal_t *ctx UNNEEDED, const struct short_channel_id *short_channel_id UNNEEDED)
{ fprintf(stderr, "towire_gossip_get_txout called!\n"); abort(); }
/* Generated stub for towire_gossip_new_peer_reply */
u8 *towire_gossip_new_peer_reply(const tal_t *ctx UNNEEDED, bool success UNNEEDED, const struct gossip_state *gs UNNEEDED)
{ fprintf(stderr, "towire_gossip_new_peer_reply called!\n"); abort(); }
/* Generated stub for towire_gossip_ping_reply */
u8 *towire_gossip_ping_reply(const tal_t *ctx UNNEEDED, const struct node_id *id UNNEEDED, bool sent UNNEEDED, u16 totlen UNNEEDED)
{ fprintf(stderr, "towire_gossip_ping_reply called!\n"); abort(); }
/* Generated stub for towire_gossip_query_channel_range_reply */
u8 *towire_gossip_query_channel_range_reply(const tal_t *ctx UNNEEDED, u32 final_first_block UNNEEDED, u32 final_num_blocks UNNEEDED, bool final_complete UNNEEDED, const struct short_channel_id *scids UNNEEDED)
{ fprintf(stderr, "towire_gossip_query_channel_range_reply called!\n"); abort(); }
/* Generated stub for towire_gossip_scids_reply */
u8 *towire_gossip_scids_reply(const tal_t *ctx UNNEEDED, bool ok UNNEEDED, bool complete UNNEEDED)
{ fprintf(stderr, "towire_gossip_scids_reply called!\n"); abort(); }
/* Generated stub for towire_hsm_cupdate_sig_req */
u8 *towire_hsm_cupdate_sig_req(const tal_t *ctx UNNEEDED, const u8 *cu UNNEEDED)
{ fprintf(stderr, "towire_hsm_cupdate_sig_req called!\n"); abort(); }
/* Generated stub for towire_hsm_node_announcement_sig_req */
u8 *towire_hsm_node_announcement_sig_req(const tal_t *ctx UNNEEDED, const u8 *announcement UNNEEDED)
{ fprintf(stderr, "towire_hsm_node_announcement_sig_req called!\n"); abort(); }
/* Generated stub for towire_wireaddr */
void towire_wireaddr(u8 **pptr UNNEEDED, const struct wireaddr *addr UNNEEDED)
{ fprintf(stderr, "towire_wireaddr called!\n"); abort(); }
/* Generated stub for wireaddr_eq */
bool wireaddr_eq(const struct wireaddr *a UNNEEDED, const struct wireaddr *b UNNEEDED)
{ fprintf(stderr, "wireaddr_eq called!\n"); abort(); }
/* Generated stub for wire_sync_read */
u8 *wire_sync_read(const tal_t *ctx UNNEEDED, int fd UNNEEDED)
{ fprintf(stderr, "wire_sync_read called!\n"); abort(); }
/* Generated stub for wire_sync_write */
bool wire_sync_write(int fd UNNEEDED, const void *msg TAKES UNNEEDED)
{ fprintf(stderr, "wire_sync_write called!\n"); abort(); }
/* AUTOGENERATED MOCKS END */

#if DEVELOPER
/* Generated stub for memleak_remove_htable */
void memleak_remove_htable(struct htable *memtable UNNEEDED, const struct htable *ht UNNEEDED)
{ fprintf(stderr, "memleak_remove_htable called!\n"); abort(); }
/* Generated stub for memleak_remove_intmap_ */
void memleak_remove_intmap_(struct htable *memtable UNNEEDED, const struct intmap *m UNNEEDED)
{ fprintf(stderr, "memleak_remove_intmap_ called!\n"); abort(); }
#endif

/* Generated stub for status_fmt */
void status_fmt(enum log_level level UNNEEDED, const char *fmt UNNEEDED, ...)
{
}

static const char *test_vectors[] = {
	"{\n"
	"  \"msg\" : {\n"
	"    \"type\" : \"QueryChannelRange\",\n"
	"    \"chainHash\" : \"0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206\",\n"
	"    \"firstBlockNum\" : 100000,\n"
	"    \"numberOfBlocks\" : 1500,\n"
	"    \"extensions\" : [ ]\n"
	"  },\n"
	"  \"hex\" : \"01070f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206000186a0000005dc\"\n"
	"}\n",
	"{\n"
	"  \"msg\" : {\n"
	"    \"type\" : \"QueryChannelRange\",\n"
	"    \"chainHash\" : \"0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206\",\n"
	"    \"firstBlockNum\" : 35000,\n"
	"    \"numberOfBlocks\" : 100,\n"
	"    \"extensions\" : [ \"WANT_TIMESTAMPS | WANT_CHECKSUMS\" ]\n"
	"  },\n"
	"  \"hex\" : \"01070f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206000088b800000064010103\"\n"
	"}\n",
	"{\n"
	"  \"msg\" : {\n"
	"    \"type\" : \"ReplyChannelRange\",\n"
	"    \"chainHash\" : \"0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206\",\n"
	"    \"firstBlockNum\" : 756230,\n"
	"    \"numberOfBlocks\" : 1500,\n"
	"    \"complete\" : 1,\n"
	"    \"shortChannelIds\" : {\n"
	"      \"encoding\" : \"UNCOMPRESSED\",\n"
	"      \"array\" : [ \"0x0x142\", \"0x0x15465\", \"0x69x42692\" ]\n"
	"    }\n"
	"  },\n"
	"  \"hex\" : \"01080f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206000b8a06000005dc01001900000000000000008e0000000000003c69000000000045a6c4\"\n"
	"}\n",
	"{\n"
	"  \"msg\" : {\n"
	"    \"type\" : \"ReplyChannelRange\",\n"
	"    \"chainHash\" : \"0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206\",\n"
	"    \"firstBlockNum\" : 1600,\n"
	"    \"numberOfBlocks\" : 110,\n"
	"    \"complete\" : 1,\n"
	"    \"shortChannelIds\" : {\n"
	"      \"encoding\" : \"COMPRESSED_ZLIB\",\n"
	"      \"array\" : [ \"0x0x142\", \"0x0x15465\", \"0x4x3318\" ]\n"
	"    }\n"
	"  },\n"
	"  \"hex\" : \"01080f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206000006400000006e01001601789c636000833e08659309a65878be010010a9023a\"\n"
	"}\n",
	"{\n"
	"  \"msg\" : {\n"
	"    \"type\" : \"ReplyChannelRange\",\n"
	"    \"chainHash\" : \"0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206\",\n"
	"    \"firstBlockNum\" : 122334,\n"
	"    \"numberOfBlocks\" : 1500,\n"
	"    \"complete\" : 1,\n"
	"    \"shortChannelIds\" : {\n"
	"      \"encoding\" : \"UNCOMPRESSED\",\n"
	"      \"array\" : [ \"0x0x12355\", \"0x7x30934\", \"0x70x57793\" ]\n"
	"    },\n"
	"    \"timestamps\" : {\n"
	"      \"encoding\" : \"UNCOMPRESSED\",\n"
	"      \"timestamps\" : [ {\n"
	"        \"timestamp1\" : 164545,\n"
	"        \"timestamp2\" : 948165\n"
	"      }, {\n"
	"        \"timestamp1\" : 489645,\n"
	"        \"timestamp2\" : 4786864\n"
	"      }, {\n"
	"        \"timestamp1\" : 46456,\n"
	"        \"timestamp2\" : 9788415\n"
	"      } ]\n"
	"    },\n"
	"    \"checksums\" : {\n"
	"      \"checksums\" : [ {\n"
	"        \"checksum1\" : 1111,\n"
	"        \"checksum2\" : 2222\n"
	"      }, {\n"
	"        \"checksum1\" : 3333,\n"
	"        \"checksum2\" : 4444\n"
	"      }, {\n"
	"        \"checksum1\" : 5555,\n"
	"        \"checksum2\" : 6666\n"
	"      } ]\n"
	"    }\n"
	"  },\n"
	"  \"hex\" : \"01080f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e22060001ddde000005dc01001900000000000000304300000000000778d6000000000046e1c1011900000282c1000e77c5000778ad00490ab00000b57800955bff031800000457000008ae00000d050000115c000015b300001a0a\"\n"
	"}\n",
	"{\n"
	"  \"msg\" : {\n"
	"    \"type\" : \"ReplyChannelRange\",\n"
	"    \"chainHash\" : \"0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206\",\n"
	"    \"firstBlockNum\" : 122334,\n"
	"    \"numberOfBlocks\" : 1500,\n"
	"    \"complete\" : 1,\n"
	"    \"shortChannelIds\" : {\n"
	"      \"encoding\" : \"COMPRESSED_ZLIB\",\n"
	"      \"array\" : [ \"0x0x12355\", \"0x7x30934\", \"0x70x57793\" ]\n"
	"    },\n"
	"    \"timestamps\" : {\n"
	"      \"encoding\" : \"COMPRESSED_ZLIB\",\n"
	"      \"timestamps\" : [ {\n"
	"        \"timestamp1\" : 164545,\n"
	"        \"timestamp2\" : 948165\n"
	"      }, {\n"
	"        \"timestamp1\" : 489645,\n"
	"        \"timestamp2\" : 4786864\n"
	"      }, {\n"
	"        \"timestamp1\" : 46456,\n"
	"        \"timestamp2\" : 9788415\n"
	"      } ]\n"
	"    },\n"
	"    \"checksums\" : {\n"
	"      \"checksums\" : [ {\n"
	"        \"checksum1\" : 1111,\n"
	"        \"checksum2\" : 2222\n"
	"      }, {\n"
	"        \"checksum1\" : 3333,\n"
	"        \"checksum2\" : 4444\n"
	"      }, {\n"
	"        \"checksum1\" : 5555,\n"
	"        \"checksum2\" : 6666\n"
	"      } ]\n"
	"    }\n"
	"  },\n"
	"  \"hex\" : \"01080f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e22060001ddde000005dc01001801789c63600001036730c55e710d4cbb3d3c080017c303b1012201789c63606a3ac8c0577e9481bd622d8327d7060686ad150c53a3ff0300554707db031800000457000008ae00000d050000115c000015b300001a0a\"\n"
	"}\n",
	"{\n"
	"  \"msg\" : {\n"
	"    \"type\" : \"QueryShortChannelIds\",\n"
	"    \"chainHash\" : \"0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206\",\n"
	"    \"shortChannelIds\" : {\n"
	"      \"encoding\" : \"UNCOMPRESSED\",\n"
	"      \"array\" : [ \"0x0x142\", \"0x0x15465\", \"0x69x42692\" ]\n"
	"    },\n"
	"    \"extensions\" : [ ]\n"
	"  },\n"
	"  \"hex\" : \"01050f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206001900000000000000008e0000000000003c69000000000045a6c4\"\n"
	"}\n",
	"{\n"
	"  \"msg\" : {\n"
	"    \"type\" : \"QueryShortChannelIds\",\n"
	"    \"chainHash\" : \"0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206\",\n"
	"    \"shortChannelIds\" : {\n"
	"      \"encoding\" : \"COMPRESSED_ZLIB\",\n"
	"      \"array\" : [ \"0x0x4564\", \"0x2x47550\", \"0x69x42692\" ]\n"
	"    },\n"
	"    \"extensions\" : [ ]\n"
	"  },\n"
	"  \"hex\" : \"01050f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206001801789c63600001c12b608a69e73e30edbaec0800203b040e\"\n"
	"}\n",
	"{\n"
	"  \"msg\" : {\n"
	"    \"type\" : \"QueryShortChannelIds\",\n"
	"    \"chainHash\" : \"0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206\",\n"
	"    \"shortChannelIds\" : {\n"
	"      \"encoding\" : \"UNCOMPRESSED\",\n"
	"      \"array\" : [ \"0x0x12232\", \"0x0x15556\", \"0x69x42692\" ]\n"
	"    },\n"
	"    \"extensions\" : [ {\n"
	"      \"encoding\" : \"COMPRESSED_ZLIB\",\n"
	"      \"array\" : [ 1, 2, 4 ]\n"
	"    } ]\n"
	"  },\n"
	"  \"hex\" : \"01050f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e22060019000000000000002fc80000000000003cc4000000000045a6c4010c01789c6364620100000e0008\"\n"
	"}\n",
	"{\n"
	"  \"msg\" : {\n"
	"    \"type\" : \"QueryShortChannelIds\",\n"
	"    \"chainHash\" : \"0f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206\",\n"
	"    \"shortChannelIds\" : {\n"
	"      \"encoding\" : \"COMPRESSED_ZLIB\",\n"
	"      \"array\" : [ \"0x0x14200\", \"0x0x46645\", \"0x69x42692\" ]\n"
	"    },\n"
	"    \"extensions\" : [ {\n"
	"      \"encoding\" : \"COMPRESSED_ZLIB\",\n"
	"      \"array\" : [ 1, 2, 4 ]\n"
	"    } ]\n"
	"  },\n"
	"  \"hex\" : \"01050f9188f13cb7b2c71f2a335e3a4fc328bf5beb436012afca590b1a11466e2206001801789c63600001f30a30c5b0cd144cb92e3b020017c6034a010c01789c6364620100000e0008\"\n"
	"}\n",
};

static void get_chainhash(const char *test_vector,
			  const jsmntok_t *obj,
			  struct bitcoin_blkid *chain_hash)
{
	const jsmntok_t *tok = json_get_member(test_vector, obj, "chainHash");
	size_t hexlen = tok->end - tok->start;
	assert(hex_decode(test_vector + tok->start, hexlen,
			  chain_hash, sizeof(*chain_hash)));
}

static u8 *get_scid_array(const tal_t *ctx,
			  const char *test_vector,
			  const jsmntok_t *obj)
{
	const jsmntok_t *scids, *arr, *encoding, *t;
	size_t i;
	u8 *encoded;

	scids = json_get_member(test_vector, obj, "shortChannelIds");
	arr = json_get_member(test_vector, scids, "array");

	encoded = encoding_start(ctx);
	encoding = json_get_member(test_vector, scids, "encoding");
	json_for_each_arr(i, t, arr) {
		struct short_channel_id scid;
		assert(json_to_short_channel_id(test_vector, t, &scid));
		encoding_add_short_channel_id(&encoded, &scid);
	}
	if (json_tok_streq(test_vector, encoding, "UNCOMPRESSED")) {
		encoding_end_no_compress(&encoded, 1);
		encoded[0] = SHORTIDS_UNCOMPRESSED;
	} else {
		assert(json_tok_streq(test_vector, encoding, "COMPRESSED_ZLIB"));
		assert(encoding_end_zlib(&encoded, 1));
		encoded[0] = SHORTIDS_ZLIB;
	}

	return encoded;
}

static u8 *test_query_channel_range(const char *test_vector, const jsmntok_t *obj)
{
	struct bitcoin_blkid chain_hash;
	u32 firstBlockNum, numberOfBlocks;
	const jsmntok_t *opt, *t;
	struct tlv_query_channel_range_tlvs *tlvs
		= tlv_query_channel_range_tlvs_new(NULL);
	u8 *msg;
	size_t i;

	get_chainhash(test_vector, obj, &chain_hash);
	assert(json_to_number(test_vector, json_get_member(test_vector, obj, "firstBlockNum"), &firstBlockNum));
	assert(json_to_number(test_vector, json_get_member(test_vector, obj, "numberOfBlocks"), &numberOfBlocks));
	opt = json_get_member(test_vector, obj, "extensions");
	json_for_each_arr(i, t, opt) {
		assert(json_tok_streq(test_vector, t,
				      "WANT_TIMESTAMPS | WANT_CHECKSUMS"));
		tlvs->query_option = tal(tlvs,
					struct tlv_query_channel_range_tlvs_query_option);
		tlvs->query_option->query_option_flags =
			QUERY_ADD_TIMESTAMPS | QUERY_ADD_CHECKSUMS;
	}
	msg = towire_query_channel_range(NULL, &chain_hash, firstBlockNum, numberOfBlocks, tlvs);

	tal_free(tlvs);
	return msg;
}

static u8 *test_reply_channel_range(const char *test_vector, const jsmntok_t *obj)
{
	struct bitcoin_blkid chain_hash;
	u32 firstBlockNum, numberOfBlocks, complete;
	const jsmntok_t *opt, *t;
	size_t i;
	u8 *msg;
	u8 *encoded_scids;

	u8 *ctx = tal(NULL, u8);
	struct tlv_reply_channel_range_tlvs *tlvs
		= tlv_reply_channel_range_tlvs_new(ctx);

	get_chainhash(test_vector, obj, &chain_hash);
	assert(json_to_number(test_vector, json_get_member(test_vector, obj, "firstBlockNum"), &firstBlockNum));
	assert(json_to_number(test_vector, json_get_member(test_vector, obj, "numberOfBlocks"), &numberOfBlocks));
	assert(json_to_number(test_vector, json_get_member(test_vector, obj, "complete"), &complete));

	encoded_scids = get_scid_array(ctx, test_vector, obj);

	opt = json_get_member(test_vector, obj, "timestamps");
	if (opt) {
		const jsmntok_t *encodingtok, *tstok;
		tlvs->timestamps_tlv
			= tal(tlvs, struct tlv_reply_channel_range_tlvs_timestamps_tlv);

		tlvs->timestamps_tlv->encoded_timestamps
			= encoding_start(tlvs->timestamps_tlv);
		encodingtok = json_get_member(test_vector, opt, "encoding");
		tstok = json_get_member(test_vector, opt, "timestamps");
		json_for_each_arr(i, t, tstok) {
			struct channel_update_timestamps ts;
			assert(json_to_number(test_vector,
					      json_get_member(test_vector, t,
							      "timestamp1"),
					      &ts.timestamp_node_id_1));
			assert(json_to_number(test_vector,
					      json_get_member(test_vector, t,
							      "timestamp2"),
					      &ts.timestamp_node_id_2));
			encoding_add_timestamps(&tlvs->timestamps_tlv->encoded_timestamps, &ts);
		}
		if (json_tok_streq(test_vector, encodingtok, "UNCOMPRESSED")) {
			encoding_end_no_compress(&tlvs->timestamps_tlv->encoded_timestamps,
						 0);
			tlvs->timestamps_tlv->encoding_type = SHORTIDS_UNCOMPRESSED;
		} else {
			assert(json_tok_streq(test_vector, encodingtok,
					      "COMPRESSED_ZLIB"));
			assert(encoding_end_zlib(&tlvs->timestamps_tlv->encoded_timestamps,
						 0));
			tlvs->timestamps_tlv->encoding_type = SHORTIDS_ZLIB;
		}
	}

	opt = json_get_member(test_vector, obj, "checksums");
	if (opt) {
		const jsmntok_t *cstok;
		tlvs->checksums_tlv
			= tal(tlvs, struct tlv_reply_channel_range_tlvs_checksums_tlv);

		tlvs->checksums_tlv->checksums
			= tal_arr(tlvs->checksums_tlv,
				  struct channel_update_checksums, 0);

		cstok = json_get_member(test_vector, opt, "checksums");
		json_for_each_arr(i, t, cstok) {
			struct channel_update_checksums cs;
			assert(json_to_number(test_vector,
				      json_get_member(test_vector, t,
							      "checksum1"),
					      &cs.checksum_node_id_1));
			assert(json_to_number(test_vector,
					      json_get_member(test_vector, t,
							      "checksum2"),
					      &cs.checksum_node_id_2));
			tal_arr_expand(&tlvs->checksums_tlv->checksums, cs);
		}
	}

	msg = towire_reply_channel_range(
		NULL, &chain_hash, firstBlockNum, numberOfBlocks,
		complete, encoded_scids, tlvs);
	tal_free(ctx);
	return msg;
}

static struct tlv_query_short_channel_ids_tlvs_query_flags *
get_query_flags_array(const tal_t *ctx,
		      const char *test_vector,
		      const jsmntok_t *opt)
{
	const jsmntok_t *encoding, *arr, *t;
	struct tlv_query_short_channel_ids_tlvs_query_flags *tlv
		= tal(ctx, struct tlv_query_short_channel_ids_tlvs_query_flags);
	size_t i;

	arr = json_get_member(test_vector, opt, "array");

	tlv->encoded_query_flags = encoding_start(tlv);
	encoding = json_get_member(test_vector, opt, "encoding");
	json_for_each_arr(i, t, arr) {
		bigsize_t f;
		assert(json_to_u64(test_vector, t, &f));
		encoding_add_query_flag(&tlv->encoded_query_flags, f);
	}
	if (json_tok_streq(test_vector, encoding, "UNCOMPRESSED")) {
		encoding_end_no_compress(&tlv->encoded_query_flags, 0);
		tlv->encoding_type = SHORTIDS_UNCOMPRESSED;
	} else {
		assert(json_tok_streq(test_vector, encoding, "COMPRESSED_ZLIB"));
		assert(encoding_end_zlib(&tlv->encoded_query_flags, 0));
		tlv->encoding_type = SHORTIDS_ZLIB;
	}

	return tlv;
}

static u8 *test_query_short_channel_ids(const char *test_vector,
					const jsmntok_t *obj)
{
	struct bitcoin_blkid chain_hash;
	const jsmntok_t *opt;
	u8 *encoded, *msg;
	struct tlv_query_short_channel_ids_tlvs *tlvs;

	get_chainhash(test_vector, obj, &chain_hash);
	encoded = get_scid_array(NULL, test_vector, obj);

	opt = json_get_member(test_vector, obj, "extensions");
	if (opt && opt->size != 0) {
		/* Only handle a single */
		assert(opt->size == 1);
		tlvs = tlv_query_short_channel_ids_tlvs_new(encoded);
		tlvs->query_flags
			= get_query_flags_array(tlvs, test_vector, opt + 1);
	} else
		tlvs = NULL;
	msg = towire_query_short_channel_ids(NULL,
					     &chain_hash, encoded, tlvs);

	tal_free(encoded);
	return msg;
}

int main(void)
{
	jsmntok_t *toks = tal_arr(NULL, jsmntok_t, 1000);

	setup_locale();

	for (size_t i = 0; i < ARRAY_SIZE(test_vectors); i++) {
		jsmn_parser parser;
		u8 *m;
		const char *hex_m;
		const jsmntok_t *msg, *type, *hex;

		jsmn_init(&parser);
		assert(jsmn_parse(&parser,
				  test_vectors[i], strlen(test_vectors[i]),
				  toks, tal_count(toks)) > 0);

		msg = json_get_member(test_vectors[i], toks, "msg");
		hex = json_get_member(test_vectors[i], toks, "hex");
		type = json_get_member(test_vectors[i], msg, "type");
		if (json_tok_streq(test_vectors[i], type, "QueryChannelRange"))
			m = test_query_channel_range(test_vectors[i], msg);
		else if (json_tok_streq(test_vectors[i], type, "ReplyChannelRange"))
			m = test_reply_channel_range(test_vectors[i], msg);
		else if (json_tok_streq(test_vectors[i], type, "QueryShortChannelIds"))
			m = test_query_short_channel_ids(test_vectors[i], msg);
		else
			abort();
		hex_m = tal_hex(m, m);
		assert(json_tok_streq(test_vectors[i], hex, hex_m));
		tal_free(m);
	}
	tal_free(toks);
	return 0;
}
#endif /* EXPERIMENTAL_FEATURES */
