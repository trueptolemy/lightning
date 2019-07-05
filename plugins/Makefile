PLUGIN_PAY_SRC := plugins/pay.c
PLUGIN_PAY_OBJS := $(PLUGIN_PAY_SRC:.c=.o)

PLUGIN_AUTOCLEAN_SRC := plugins/autoclean.c
PLUGIN_AUTOCLEAN_OBJS := $(PLUGIN_AUTOCLEAN_SRC:.c=.o)

PLUGIN_BITCOIN_SRC := plugins/bitcoin.c		\
	plugins/bitcoinrpc.c
PLUGIN_BITCOIN_OBJS := $(PLUGIN_BITCOIN_SRC:.c=.o)
PLUGIN_BITCOIN_HEADERS := plugins/bitcoinrpc.h
$(PLUGIN_BITCOIN_OBJS): $(PLUGIN_BITCOIN_HEADERS)

PLUGIN_LIB_SRC := plugins/libplugin.c
PLUGIN_LIB_HEADER := plugins/libplugin.h
PLUGIN_LIB_OBJS := $(PLUGIN_LIB_SRC:.c=.o)

PLUGIN_COMMON_OBJS :=				\
	bitcoin/base58.o			\
	bitcoin/block.o			\
	bitcoin/chainparams.o			\
	bitcoin/pubkey.o			\
	bitcoin/pullpush.o			\
	bitcoin/script.o			\
	bitcoin/shadouble.o			\
	bitcoin/short_channel_id.o		\
	bitcoin/signature.o			\
	bitcoin/tx.o				\
	bitcoin/varint.o			\
	common/amount.o				\
	common/bech32.o				\
	common/bech32_util.o			\
	common/bolt11.o				\
	common/daemon.o				\
	common/hash_u5.o			\
	common/json.o				\
	common/json_helpers.o			\
	common/json_tok.o			\
	common/memleak.o			\
	common/node_id.o			\
	common/param.o				\
	common/pseudorand.o			\
	common/timeout.o			\
	common/type_to_string.o			\
	common/utils.o				\
	common/version.o			\
	wire/fromwire.o				\
	wire/towire.o

PLUGIN_COMMON_HEADERS := bitcoin/feerate.h

$(PLUGIN_COMMON_OBJS) : $(PLUGIN_COMMON_HEADERS)

plugins/pay: $(PLUGIN_PAY_OBJS) $(PLUGIN_LIB_OBJS) $(PLUGIN_COMMON_OBJS) $(JSMN_OBJS) $(CCAN_OBJS)

plugins/autoclean: $(PLUGIN_AUTOCLEAN_OBJS) $(PLUGIN_LIB_OBJS) $(PLUGIN_COMMON_OBJS) $(JSMN_OBJS) $(CCAN_OBJS)

plugins/bitcoin: $(PLUGIN_BITCOIN_OBJS) $(PLUGIN_LIB_OBJS) $(PLUGIN_COMMON_OBJS) $(JSMN_OBJS) $(CCAN_OBJS)

$(PLUGIN_PAY_OBJS) $(PLUGIN_AUTOCLEAN_OBJS) $(PLUGIN_BITCOIN_OBJS) $(PLUGIN_LIB_OBJS): $(PLUGIN_LIB_HEADER)

# Make sure these depend on everything.
ALL_PROGRAMS += plugins/pay plugins/autoclean plugins/bitcoin
ALL_OBJS += $(PLUGIN_PAY_OBJS) $(PLUGIN_AUTOCLEAN_OBJS) $(PLUGIN_BITCOIN_OBJS) $(PLUGIN_LIB_OBJS)

check-source: $(PLUGIN_PAY_SRC:%=check-src-include-order/%) $(PLUGIN_AUTOCLEAN_SRC:%=check-src-include-order/%) $(PLUGIN_BITCOIN_SRC:%=check-src-include-order/%)
check-source-bolt: $(PLUGIN_PAY_SRC:%=bolt-check/%) $(PLUGIN_AUTOCLEAN_SRC:%=bolt-check/%) $(PLUGIN_BITCOIN_SRC:%=bolt-check/%)
check-whitespace: $(PLUGIN_PAY_SRC:%=check-whitespace/%) $(PLUGIN_AUTOCLEAN_SRC:%=check-whitespace/%) $(PLUGIN_BITCOIN_SRC:%=check-whitespace/%)

clean: plugin-clean

plugin-clean:
	$(RM) $(PLUGIN_PAY_OBJS) $(PLUGIN_AUTOCLEAN_OBJS) $(PLUGIN_BITCOIN_OBJS) $(PLUGIN_LIB_OBJS)
