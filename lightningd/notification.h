#ifndef LIGHTNING_LIGHTNINGD_NOTIFICATION_H
#define LIGHTNING_LIGHTNINGD_NOTIFICATION_H
#include "config.h"
#include <lightningd/jsonrpc.h>
#include <lightningd/lightningd.h>
#include <lightningd/log.h>
#include <lightningd/pay.h>
#include <lightningd/plugin.h>
#include <wallet/wallet.h>

bool notifications_have_topic(const char *topic);

void notify_connect(struct lightningd *ld, struct node_id *nodeid,
		    struct wireaddr_internal *addr);
void notify_disconnect(struct lightningd *ld, struct node_id *nodeid);

void notify_warning(struct lightningd *ld, struct log_entry *l);

void notify_forward_event(struct lightningd *ld,
		    const struct htlc_in *in,
		    const struct htlc_out *out,
		    enum forward_status state,
		    enum onion_type failcode,
		    struct timeabs *resolved_time);

void notify_sendpay_success(struct lightningd *ld,
			    const struct wallet_payment *payment);

void notify_sendpay_fail(struct lightningd *ld,
			 const struct wallet_payment *payment,
			 int pay_errcode,
			 const u8 *onionreply,
			 const struct routing_failure *fail,
			 char *errmsg);

#endif /* LIGHTNING_LIGHTNINGD_NOTIFICATION_H */
