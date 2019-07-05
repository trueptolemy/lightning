#ifndef LIGHTNING_PLUGINS_BITCOINRPC_H
#define LIGHTNING_PLUGINS_BITCOINRPC_H
#include "config.h"
#include <bitcoin/chainparams.h>
#include <bitcoin/tx.h>
#include <ccan/list/list.h>
#include <ccan/short_types/short_types.h>
#include <ccan/tal/tal.h>
#include <ccan/time/time.h>
#include <ccan/typesafe_cb/typesafe_cb.h>
#include <stdbool.h>

struct bitcoin_blkid;
struct bitcoin_tx_output;
struct block;
struct lightningd;
struct ripemd160;
struct bitcoin_tx;
struct bitcoin_block;

enum feerate {
	FEERATE_URGENT, /* Aka: aim for next block. */
	FEERATE_NORMAL, /* Aka: next 4 blocks or so. */
	FEERATE_SLOW, /* Aka: next 100 blocks or so. */
};
#define NUM_FEERATES (FEERATE_SLOW+1)

enum feerate_style {
	FEERATE_PER_KSIPA,
	FEERATE_PER_KBYTE
};

enum bitcoind_mode {
	BITCOIND_MAINNET = 1,
	BITCOIND_TESTNET,
	BITCOIND_REGTEST
};

enum bitcoind_prio {
	BITCOIND_LOW_PRIO,
	BITCOIND_HIGH_PRIO
};
#define BITCOIND_NUM_PRIO (BITCOIND_HIGH_PRIO+1)

struct bitcoind {
	/* eg. "bitcoin-cli" */
	const char *cli;

	/* -datadir arg for bitcoin-cli. */
	const char *datadir;

	/* How many high/low prio requests are we running (it's ratelimited) */
	size_t num_requests[BITCOIND_NUM_PRIO];

	/* Pending requests (high and low prio). */
	struct list_head pending[BITCOIND_NUM_PRIO];

	/* What network are we on? */
	const struct chainparams *chainparams;

	/* If non-zero, time we first hit a bitcoind error. */
	unsigned int error_count;
	struct timemono first_error_time;

	/* Ignore results, we're shutting down. */
	bool shutdown;

	/* Passthrough parameters for bitcoin-cli */
	const char *rpcuser, *rpcpass, *rpcconnect, *rpcport;

    /* How often to poll. */
	u32 poll_seconds;

    struct timers *timers;

    u32 blockcount;

    struct bitcoin_blkid *blkid;
    struct bitcoin_block *blk;

    u32 feerate[NUM_FEERATES];
};

u32 feerate_from_style(u32 feerate, enum feerate_style style);
u32 feerate_to_style(u32 feerate_perkw, enum feerate_style style);

void wait_for_bitcoind(struct bitcoind *bitcoind);

void bitcoind_estimate_fees_(struct bitcoind *bitcoind,
			     const u32 blocks[], const char *estmode[],
			     size_t num_estimates,
			     void (*cb)(struct bitcoind *bitcoind,
					const u32 satoshi_per_kw[], void *),
			     void *arg);

#define bitcoind_estimate_fees(bitcoind_, blocks, estmode, num, cb, arg) \
	bitcoind_estimate_fees_((bitcoind_), (blocks), (estmode), (num), \
				typesafe_cb_preargs(void, void *,	\
						    (cb), (arg),	\
						    struct bitcoind *,	\
						    const u32 *),	\
				(arg))

void bitcoind_sendrawtx_(struct bitcoind *bitcoind,
			 const char *hextx,
			 void (*cb)(struct bitcoind *bitcoind,
				    int exitstatus, const char *msg, void *),
			 void *arg);

#define bitcoind_sendrawtx(bitcoind_, hextx, cb, arg)			\
	bitcoind_sendrawtx_((bitcoind_), (hextx),			\
			    typesafe_cb_preargs(void, void *,		\
						(cb), (arg),		\
						struct bitcoind *,	\
						int, const char *),	\
			    (arg))

void bitcoind_getblockcount_(struct bitcoind *bitcoind,
			     void (*cb)(struct bitcoind *bitcoind,
					u32 blockcount,
					void *arg),
			     void *arg);

#define bitcoind_getblockcount(bitcoind_, cb, arg)			\
	bitcoind_getblockcount_((bitcoind_),				\
				typesafe_cb_preargs(void, void *,	\
						    (cb), (arg),	\
						    struct bitcoind *,	\
						    u32 blockcount),	\
				(arg))

/* blkid is NULL if call fails. */
void bitcoind_getblockhash_(struct bitcoind *bitcoind,
			    u32 height,
			    void (*cb)(struct bitcoind *bitcoind,
				       const struct bitcoin_blkid *blkid,
				       void *arg),
			    void *arg);
#define bitcoind_getblockhash(bitcoind_, height, cb, arg)		\
	bitcoind_getblockhash_((bitcoind_),				\
			       (height),				\
			       typesafe_cb_preargs(void, void *,	\
						   (cb), (arg),		\
						   struct bitcoind *,	\
						   const struct bitcoin_blkid *), \
			       (arg))

void bitcoind_getrawblock_(struct bitcoind *bitcoind,
			   const struct bitcoin_blkid *blockid,
			   void (*cb)(struct bitcoind *bitcoind,
				      struct bitcoin_block *blk,
				      void *arg),
			   void *arg);
#define bitcoind_getrawblock(bitcoind_, blkid, cb, arg)			\
	bitcoind_getrawblock_((bitcoind_), (blkid),			\
			      typesafe_cb_preargs(void, void *,		\
						  (cb), (arg),		\
						  struct bitcoind *,	\
						  struct bitcoin_block *), \
			      (arg))

void bitcoind_getoutput_(struct bitcoind *bitcoind,
			 unsigned int blocknum, unsigned int txnum,
			 unsigned int outnum,
			 void (*cb)(struct bitcoind *bitcoind,
				    const struct bitcoin_tx_output *output,
				    void *arg),
			 void *arg);
#define bitcoind_getoutput(bitcoind_, blocknum, txnum, outnum, cb, arg)	\
	bitcoind_getoutput_((bitcoind_), (blocknum), (txnum), (outnum),	\
			    typesafe_cb_preargs(void, void *,		\
						(cb), (arg),		\
						struct bitcoind *,	\
						const struct bitcoin_tx_output*), \
			    (arg))

void bitcoind_gettxout(struct bitcoind *bitcoind,
		       const struct bitcoin_txid *txid, const u32 outnum,
		       void (*cb)(struct bitcoind *bitcoind,
				  const struct bitcoin_tx_output *txout,
				  void *arg),
		       void *arg);

void destroy_bitcoind(struct bitcoind *bitcoind);

#endif /* LIGHTNING_PLUGINS_BITCOINRPC_H */
