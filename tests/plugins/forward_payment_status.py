#!/usr/bin/env python3
"""This plugin is used to check that forward_event calls are working correctly.
"""
from lightning import Plugin

plugin = Plugin()


def check(forward, dbforward):
    record = forward
    if record['status'] == 'offered':
        if dbforward['status'] == 'local_failed':
            record['failcode'] = dbforward['failcode']
            record['failreason'] = dbforward['failreason']
        elif dbforward['status'] != 'offered':
            record['resolved_time'] = dbforward['resolved_time']
    record['status'] = dbforward['status']
    if record == dbforward:
        return True
    else:
        return False


@plugin.init()
def init(configuration, options, plugin):
    plugin.forward_list = []


@plugin.subscribe("forward_event")
def notify_warning(plugin, forward_payment):
    plugin.log("receive a forward recored, status: {}, payment_hash: {}".format(forward_payment['status'], forward_payment['payment_hash']))
    plugin.forward_list.append(forward_payment)


@plugin.method('recordcheck')
def record_lookup(payment_hash, status, dbforward, plugin):
    plugin.log("recordcheck: payment_hash: {}, status: {}".format(payment_hash, status))
    for forward in plugin.forward_list:
        if forward['payment_hash'] == payment_hash and forward['status'] == status:
            plugin.log("record exists")
            check_result = check(forward, dbforward)
            return check_result
    plugin.log("no record")
    return False


plugin.run()
