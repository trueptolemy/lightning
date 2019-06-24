#include "lightningd/notification.h"
#include <ccan/array_size/array_size.h>
#include <lightningd/channel.h>
#include <lightningd/json.h>
#include <lightningd/peer_htlcs.h>

const char *notification_topics[] = {
	"connect",
	"disconnect",
	"warning",
	"forward_event",
	"sendpay_result"
};

bool notifications_have_topic(const char *topic)
{
	for (size_t i=0; i<ARRAY_SIZE(notification_topics); i++)
		if (streq(topic, notification_topics[i]))
			return true;
	return false;
}

void notify_connect(struct lightningd *ld, struct node_id *nodeid,
		    struct wireaddr_internal *addr)
{
	struct jsonrpc_notification *n =
	    jsonrpc_notification_start(NULL, notification_topics[0]);
	json_add_node_id(n->stream, "id", nodeid);
	json_add_address_internal(n->stream, "address", addr);
	jsonrpc_notification_end(n);
	plugins_notify(ld->plugins, take(n));
}

void notify_disconnect(struct lightningd *ld, struct node_id *nodeid)
{
	struct jsonrpc_notification *n =
	    jsonrpc_notification_start(NULL, notification_topics[1]);
	json_add_node_id(n->stream, "id", nodeid);
	jsonrpc_notification_end(n);
	plugins_notify(ld->plugins, take(n));
}

/*'warning' is based on LOG_UNUSUAL/LOG_BROKEN level log
 *(in plugin module, they're 'warn'/'error' level). */
void notify_warning(struct lightningd *ld, struct log_entry *l)
{
	struct jsonrpc_notification *n =
	    jsonrpc_notification_start(NULL, notification_topics[2]);
	json_object_start(n->stream, "warning");
	/* Choose "BROKEN"/"UNUSUAL" to keep consistent with the habit
	 * of plugin. But this may confuses the users who want to 'getlog'
	 * with the level indicated by notifications. It is the duty of a
	 * plugin to eliminate this misunderstanding.
	 */
	json_add_string(n->stream, "level",
			l->level == LOG_BROKEN ? "error"
			: "warn");
	/* unsuaul/broken event is rare, plugin pay more attentions on
	 * the absolute time, like when channels failed. */
	json_add_time(n->stream, "time", l->time.ts);
	json_add_string(n->stream, "source", l->prefix);
	json_add_string(n->stream, "log", l->log);
	json_object_end(n->stream); /* .warning */
	jsonrpc_notification_end(n);
	plugins_notify(ld->plugins, take(n));
}

void notify_forward_event(struct lightningd *ld,
				  const struct htlc_in *in,
				  const struct htlc_out *out,
				  enum forward_status state,
				  enum onion_type failcode,
				  struct timeabs *resolved_time)
{
	struct jsonrpc_notification *n =
	    jsonrpc_notification_start(NULL, notification_topics[3]);
	/* Here is more neat to initial a forwarding structure than
	 * to pass in a bunch of parameters directly*/
	struct forwarding *cur = tal(tmpctx, struct forwarding);
	cur->channel_in = *in->key.channel->scid;
	cur->channel_out = *out->key.channel->scid;
	cur->msat_in = in->msat;
	cur->msat_out = out->msat;
	cur->payment_hash = tal(cur, struct sha256_double);
	cur->payment_hash->sha = in->payment_hash;
	cur->status = state;
	cur->failcode = failcode;
	cur->received_time = in->received_time;
	cur->resolved_time = tal_steal(cur, resolved_time);
	assert(amount_msat_sub(&cur->fee, in->msat, out->msat));

	json_format_forwarding_object(n->stream, "forward_payment", cur);

	jsonrpc_notification_end(n);
	plugins_notify(ld->plugins, take(n));
}

void notify_sendpay_result(struct lightningd *ld,
				  const struct wallet_payment *payment,
				  const int *pay_errcode,
				  const u8 *onionreply,
				  const struct routing_failure *fail,
				  const char *details)
{
	struct jsonrpc_notification *n =
	    jsonrpc_notification_start(NULL, notification_topics[4]);
	json_object_start(n->stream, "sendpay_result");

	if (payment->status == PAYMENT_COMPLETE)
		json_add_payment_fields(n->stream, payment);
	else {
		json_sendpay_fail_fields(n->stream,
							     payment,
							     *pay_errcode,
							     onionreply,
							     fail,
							     details);
	}
	json_object_end(n->stream); /* .sendpay_result */
	jsonrpc_notification_end(n);
	plugins_notify(ld->plugins, take(n));
}