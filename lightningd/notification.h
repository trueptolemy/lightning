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

void notify_disconnect(struct lightningd *ld, struct node_id *nodeid);

void notify_warning(struct lightningd *ld, struct log_entry *l);

void notify_invoice_payment(struct lightningd *ld, struct amount_msat amount,
			    struct preimage preimage, const struct json_escape *label);

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

void notify_call_(struct lightningd *ld, const struct notification *noti, void *payload);

#define NOTIFY_CALL_DEF(topic, payload_type)                                                        \
	UNNEEDED static inline void notify_##topic(                                                 \
	    struct lightningd *ld, payload_type payload)                                            \
	{                                                                                           \
		notify_call_(ld, &topic##_notification_gen, (void *)payload);                       \
	}

/* FIXME: Find a way to avoid back-to-back declaration and definition */
#define REGISTER_NOTIFICATION(topic, serialize_payload, payload_type)                               \
	struct notification topic##_notification_gen = {                                            \
	    stringify(topic),                                                                       \
	    typesafe_cb_cast(void (*)(void *, struct json_stream *),                                \
			     void (*)(payload_type, struct json_stream *),                          \
			     serialize_payload),                                                    \
	};                                                                                          \
	AUTODATA(notifications, &topic##_notification_gen);                                         \
	NOTIFY_CALL_DEF(topic, payload_type);

struct connect_notification_payload {
	struct node_id *nodeid;
	struct wireaddr_internal *addr;
};

void connect_notification_serialize(
			struct connect_notification_payload *payload,
			struct json_stream *stream);

REGISTER_NOTIFICATION(connect,
		      connect_notification_serialize,
		      struct connect_notification_payload *);

#endif /* LIGHTNING_LIGHTNINGD_NOTIFICATION_H */
