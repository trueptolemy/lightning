#!/usr/bin/env python3
"""This plugin is used to check that forward_event calls are working correctly.
"""
from lightning import Plugin

plugin = Plugin()


def check(result, response):
    if result == response:
        return True
    else:
        return False


@plugin.init()
def init(configuration, options, plugin):
    plugin.result_list = []
    plugin.log("sendpay_result plugin initialed")


@plugin.subscribe("sendpay_result")
def notify_sendpay_result(plugin, result):
    plugin.log("receive a sendpay_result recored, id: {}, payment_hash: {}".format(result['id'], result['payment_hash']))
    plugin.result_list.append(result)


@plugin.method('recordcheck')
def record_lookup(payment_hash, response, plugin):
    plugin.log("recordcheck: payment_hash: {}".format(payment_hash))
    for result in plugin.result_list:
        if result['payment_hash'] == payment_hash:
            plugin.log("record exists")
            check_result = check(result, response)
            return check_result
    plugin.log("no record")
    return False


plugin.run()
