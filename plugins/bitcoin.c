/* Code for talking to bitcoind.  We use bitcoin-cli. */
#include "bitcoin/base58.h"
#include "bitcoin/block.h"
#include "bitcoin/feerate.h"
#include "bitcoin/shadouble.h"
#include <ccan/array_size/array_size.h>
#include <ccan/cast/cast.h>
#include <ccan/io/io.h>
#include <ccan/pipecmd/pipecmd.h>
#include <ccan/short_types/short_types.h>
#include <ccan/str/hex/hex.h>
#include <ccan/take/take.h>
#include <ccan/tal/grab_file/grab_file.h>
#include <ccan/tal/path/path.h>
#include <ccan/tal/str/str.h>
#include <ccan/time/time.h>
#include <ccan/typesafe_cb/typesafe_cb.h>
#include <common/json_helpers.h>
#include <common/memleak.h>
#include <common/timeout.h>
#include <common/utils.h>
#include <errno.h>
#include <inttypes.h>
#include <plugins/bitcoinrpc.h>
#include <plugins/libplugin.h>

static struct bitcoind *bitcoind;

static void initial_bitcoind(struct bitcoind *bitcoind, struct plugin_conn *rpc)
{
    const char *chainparams;
    bitcoind = tal(NULL, struct bitcoind);

    chainparams = rpc_delve(tmpctx, "getinfo",
			  take(json_out_obj(NULL, NULL, NULL)), rpc, ".network");

	/* Use testnet by default, change later if we want another network */
	bitcoind->chainparams = chainparams_for_network(chainparams);
	bitcoind->cli = rpc_delve(bitcoind, "listconfigs",
			  take(json_out_obj(NULL,
					    "config", "bitcoin-cli")),
			  rpc, ".bitcoin-cli");

	bitcoind->datadir = rpc_delve(bitcoind, "listconfigs",
			  take(json_out_obj(NULL,
					    "config", "bitcoin-datadir")),
			  rpc, ".bitcoin-datadir");

	for (size_t i = 0; i < BITCOIND_NUM_PRIO; i++) {
		bitcoind->num_requests[i] = 0;
		list_head_init(&bitcoind->pending[i]);
	}

	bitcoind->shutdown = false;
	bitcoind->error_count = 0;

	bitcoind->rpcuser = rpc_delve(bitcoind, "listconfigs",
			  take(json_out_obj(NULL,
					    "config", "bitcoin-rpcuser")),
			  rpc, ".bitcoin-rpcuser");

	bitcoind->rpcpass = rpc_delve(bitcoind, "listconfigs",
			  take(json_out_obj(NULL,
					    "config", "bitcoin-rpcpassword")),
			  rpc, ".bitcoin-rpcpassword");

	bitcoind->rpcconnect = rpc_delve(bitcoind, "listconfigs",
			  take(json_out_obj(NULL,
					    "config", "bitcoin-rpcconnect")),
			  rpc, ".bitcoin-rpcconnect");

	bitcoind->rpcport = rpc_delve(bitcoind, "listconfigs",
			  take(json_out_obj(NULL,
					    "config", "bitcoin-rpcport")),
			  rpc, ".bitcoin-rpcport");

    bitcoind->timers = tal(bitcoind, struct timers);
	timers_init(bitcoind->timers, time_mono());
    bitcoind->blockcount = 0;
    bitcoind->blkid = NULL;
    bitcoind->blk = NULL;
    memset(&bitcoind->feerate, 0, sizeof(bitcoind->feerate));
	tal_add_destructor(bitcoind, destroy_bitcoind);
}

static void init(struct plugin_conn *rpc)
{
    initial_bitcoind(bitcoind, rpc);
    wait_for_bitcoind(bitcoind);
}

static const struct plugin_command commands[] = {};
static const struct plugin_option options[] = {};
static const struct plugin_subscription subscriptions[] = {};
static const struct plugin_hook hooks[] = {};

int main(int argc, char *argv[])
{
	setup_locale();
	plugin_main(argv, init, options, ARRAY_SIZE(options),
		    commands, ARRAY_SIZE(commands), subscriptions,
		    ARRAY_SIZE(subscriptions), hooks, ARRAY_SIZE(hooks));
}