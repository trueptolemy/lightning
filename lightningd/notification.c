#include <ccan/array_size/array_size.h>
#include <lightningd/channel.h>
#include <lightningd/json.h>
#include <lightningd/notification.h>
#include <lightningd/peer_htlcs.h>

const char *notification_topics[] = {
	"forward_event"
};

static struct notification *find_notification_by_topic(const char* topic)
{
	static struct notification **notilist = NULL;
	static size_t num_notis;
	if (!notilist)
		notilist = autodata_get(notifications, &num_notis);

	for (size_t i=0; i<num_notis; i++)
		if (streq(notilist[i]->topic, topic))
			return notilist[i];
	return NULL;
}

bool notifications_have_topic(const char *topic)
{
	struct notification *noti = find_notification_by_topic(topic);
	if (noti)
		return true;

	return false;
}

static void connect_notification_serialize(struct json_stream *stream,
					   struct node_id *nodeid,
					   struct wireaddr_internal *addr)
{
	json_add_node_id(stream, "id", nodeid);
	json_add_address_internal(stream, "address", addr);
}

REGISTER_NOTIFICATION(connect,
		      connect_notification_serialize);

void notify_connect(struct lightningd *ld, struct node_id *nodeid,
		    struct wireaddr_internal *addr)
{
	struct jsonrpc_notification *n
		= jsonrpc_notification_start(NULL, connect_notification_gen.topic);
	connect_notification_gen.serialize(n->stream, nodeid, addr);
	jsonrpc_notification_end(n);
	plugins_notify(ld->plugins, take(n));
}

static void disconnect_notification_serialize(
			struct disconnect_notification_payload *payload,
			struct json_stream *stream)
{
	json_add_node_id(stream, "id", payload->nodeid);
}

REGISTER_NOTIFICATION(disconnect,
		      disconnect_notification_serialize,
		      struct disconnect_notification_payload *);

/*'warning' is based on LOG_UNUSUAL/LOG_BROKEN level log
 *(in plugin module, they're 'warn'/'error' level). */
static void warning_notification_serialize(
			struct warning_notification_payload *payload,
			struct json_stream *stream)
{
	json_object_start(stream, "warning");
	/* Choose "BROKEN"/"UNUSUAL" to keep consistent with the habit
	 * of plugin. But this may confuses the users who want to 'getlog'
	 * with the level indicated by notifications. It is the duty of a
	 * plugin to eliminate this misunderstanding.
	 */
	json_add_string(stream, "level",
			payload->log_entry->level == LOG_BROKEN ? "error"
			: "warn");
	/* unsuaul/broken event is rare, plugin pay more attentions on
	 * the absolute time, like when channels failed. */
	json_add_time(stream, "time", payload->log_entry->time.ts);
	json_add_string(stream, "source", payload->log_entry->prefix);
	json_add_string(stream, "log", payload->log_entry->log);
	json_object_end(stream); /* .warning */
}

REGISTER_NOTIFICATION(warning,
		      warning_notification_serialize,
		      struct warning_notification_payload *);

static void invoice_payment_notification_serialize(
			struct invoice_payment_notification_payload *payload,
			struct json_stream *stream)
{
	json_object_start(stream, "invoice_payment");
	json_add_amount_msat_only(stream, "msat", *payload->amount);
	json_add_hex(stream, "preimage", payload->preimage,
		     sizeof(*payload->preimage));
	json_add_escaped_string(stream, "label", payload->label);
	json_object_end(stream);
}

REGISTER_NOTIFICATION(invoice_payment,
		      invoice_payment_notification_serialize,
		      struct invoice_payment_notification_payload *);

static void channel_opened_notification_serialize(
			struct channel_opened_notification_payload *payload,
			struct json_stream *stream)
{
	json_object_start(stream, "channel_opened");
	json_add_node_id(stream, "id", payload->node_id);
	json_add_amount_sat_only(stream, "amount", *payload->funding_sat);
	json_add_txid(stream, "funding_txid", payload->funding_txid);
	json_add_bool(stream, "funding_locked", payload->funding_locked);
	json_object_end(stream);
}

REGISTER_NOTIFICATION(channel_opened,
		      channel_opened_notification_serialize,
		      struct channel_opened_notification_payload *);

static void forward_event_notification_serialize(
			struct forward_event_notification_payload *payload,
			struct json_stream *stream)
{
	/* Here is more neat to initial a forwarding structure than
	 * to pass in a bunch of parameters directly*/
	struct forwarding *cur = tal(tmpctx, struct forwarding);
	cur->channel_in = *payload->in->key.channel->scid;
	cur->msat_in = payload->in->msat;
	if (payload->out) {
		cur->channel_out = *payload->out->key.channel->scid;
		cur->msat_out = payload->out->msat;
		assert(amount_msat_sub(&cur->fee, payload->in->msat,
				       payload->out->msat));
	} else {
		cur->channel_out.u64 = 0;
		cur->msat_out = AMOUNT_MSAT(0);
		cur->fee = AMOUNT_MSAT(0);
	}
	cur->payment_hash = tal_dup(cur, struct sha256, &payload->in->payment_hash);
	cur->status = payload->state;
	cur->failcode = payload->failcode;
	cur->received_time = payload->in->received_time;
	cur->resolved_time = tal_steal(cur, payload->resolved_time);

	json_format_forwarding_object(stream, "forward_event", cur);
}

REGISTER_NOTIFICATION(forward_event,
		      forward_event_notification_serialize,
		      struct forward_event_notification_payload *);

