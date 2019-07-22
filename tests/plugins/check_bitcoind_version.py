#!/usr/bin/env python3
"""This plugin is used to check that getclientversion calls are working correctly.
"""
import bitcoin
from bitcoin.rpc import JSONRPCError, Proxy
from lightning import Plugin
import os

plugin = Plugin()


BITCOIND_CONFIG = {
    "regtest": 1,
    "rpcuser": "rpcuser",
    "rpcpassword": "rpcpass",
}


def write_config(filename, opts, regtest_opts=None):
    with open(filename, 'w') as f:
        for k, v in opts.items():
            f.write("{}={}\n".format(k, v))
        if regtest_opts:
            f.write("[regtest]\n")
            for k, v in regtest_opts.items():
                f.write("{}={}\n".format(k, v))


@plugin.request("getclientversion")
def get_bitcoind_version(command, plugin):
    plugin.log("Receive getclientversion request: {}. ".format(command))
    brpc = Proxy(btc_conf_file=plugin.bitcoin_conf_file)
    try:
        info = brpc._call('getnetworkinfo')
    except JSONRPCError as e:
        code = e.error['code']
        return {'errorcode': code}
    return {'client': 'bitcoind', 'version': info['version']}


@plugin.request_init()
def request_init(requests, configuration, plugin):
    bitcoin.SelectParams('regtest')
    dir = "/tmp/bitcoind-test"
    if not os.path.exists(dir):
            os.makedirs(dir)
    conf_file = os.path.join(dir, 'bitcoin.conf')
    regtestdir = os.path.join(dir, 'regtest')
    if not os.path.exists(regtestdir):
            os.makedirs(regtestdir)

    rpcport = plugin.rpc.listconfigs()['bitcoin-rpcport']
    BITCOIND_CONFIG['rpcport'] = rpcport
    BITCOIND_REGTEST = {'rpcport': rpcport}
    write_config(conf_file, BITCOIND_CONFIG, BITCOIND_REGTEST)
    plugin.bitcoin_conf_file = conf_file


plugin.run()
