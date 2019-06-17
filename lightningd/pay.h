#ifndef LIGHTNING_LIGHTNINGD_PAY_H
#define LIGHTNING_LIGHTNINGD_PAY_H
#include "config.h"
#include <ccan/short_types/short_types.h>

struct htlc_out;
struct lightningd;
struct preimage;
struct sha256;
struct json_stream;
struct wallet_payment;
struct routing_failure;

void payment_succeeded(struct lightningd *ld, struct htlc_out *hout,
		       const struct preimage *rval);

void payment_failed(struct lightningd *ld, const struct htlc_out *hout,
		    const char *localfail);

/* Inform payment system to save the payment. */
void payment_store(struct lightningd *ld, const struct sha256 *payment_hash);

/* Also used in 'sendpay_result' */
void json_add_payment_fields(struct json_stream *response,
			const struct wallet_payment *t);

/* Also used in 'sendpay_result' */
void json_sendpay_fail_fields(struct json_stream *js,
			const struct wallet_payment *payment,
			const int pay_errcode,
			const u8 *onionreply,
			const struct routing_failure *fail,
			const char *details);
#endif /* LIGHTNING_LIGHTNINGD_PAY_H */
