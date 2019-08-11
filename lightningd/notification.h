#ifndef LIGHTNING_LIGHTNINGD_NOTIFICATION_H
#define LIGHTNING_LIGHTNINGD_NOTIFICATION_H
#include "config.h"
#include <bitcoin/short_channel_id.h>
#include <bitcoin/tx.h>
#include <ccan/autodata/autodata.h>
#include <ccan/json_escape/json_escape.h>
#include <ccan/time/time.h>
#include <common/amount.h>
#include <common/node_id.h>
#include <lightningd/htlc_end.h>
#include <lightningd/jsonrpc.h>
#include <lightningd/lightningd.h>
#include <lightningd/log.h>
#include <lightningd/plugin.h>
#include <wallet/wallet.h>
#include <wire/gen_onion_wire.h>

bool notifications_have_topic(const char *topic);

void notify_channel_opened(struct lightningd *ld, struct node_id *node_id,
			   struct amount_sat *funding_sat, struct bitcoin_txid *funding_txid,
			   bool *funding_locked);

void notify_forward_event(struct lightningd *ld,
			  const struct htlc_in *in,
			  const struct htlc_out *out,
			  enum forward_status state,
			  enum onion_type failcode,
			  struct timeabs *resolved_time);

struct notification {
	const char *topic;
	void (*serialize_payload)(void *src, struct json_stream *dest);
};

AUTODATA_TYPE(notifications, struct notification);

void notification_call(struct lightningd *ld, const char* topic,
		       void *payload);

/* FIXME: Find a way to avoid back-to-back declaration and definition */
#define REGISTER_NOTIFICATION(topic, serialize_payload, payload_type)                               \
	struct notification topic##_notification_gen = {                                            \
	    stringify(topic),                                                                       \
	    typesafe_cb_cast(void (*)(void *, struct json_stream *),                                \
			     void (*)(payload_type, struct json_stream *),                          \
			     serialize_payload),                                                    \
	};                                                                                          \
	AUTODATA(notifications, &topic##_notification_gen);

struct connect_notification_payload {
	struct node_id *nodeid;
	struct wireaddr_internal *addr;
};

struct disconnect_notification_payload {
	struct node_id *nodeid;
};

struct warning_notification_payload {
	struct log_entry *log_entry;
};

struct invoice_payment_notification_payload {
	struct amount_msat *amount;
	struct preimage *preimage;
	const struct json_escape *label;
};

#endif /* LIGHTNING_LIGHTNINGD_NOTIFICATION_H */
