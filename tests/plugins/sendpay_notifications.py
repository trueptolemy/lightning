#!/usr/bin/env python3
"""This plugin is used to check that sendpay_success and sendpay_failure calls are working correctly.
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
    plugin.success_list = []
    plugin.fail_list = []
    plugin.log("sendpay_result plugin initialed")


@plugin.subscribe("sendpay_success")
def notify_sendpay_success(plugin, sendpay_success):
    plugin.log("receive a sendpay_success recored, id: {}, payment_hash: {}".format(sendpay_success['id'], sendpay_success['payment_hash']))
    plugin.success_list.append(sendpay_success)


@plugin.subscribe("sendpay_failure")
def notify_sendpay_failure(plugin, sendpay_failure):
    plugin.log("receive a sendpay_failure recored, id: {}, payment_hash: {}".format(sendpay_failure['data']['id'],
               sendpay_failure['data']['payment_hash']))
    plugin.fail_list.append(sendpay_failure)


@plugin.method('recordcheck')
def record_lookup(payment_hash, response, plugin):
    plugin.log("recordcheck: payment_hash: {}".format(payment_hash))
    for success in plugin.success_list:
        if success['payment_hash'] == payment_hash:
            plugin.log("success record exists")
            check_result = check(success, response)
            return check_result
    for fail in plugin.fail_list:
        if fail['data']['payment_hash'] == payment_hash:
            plugin.log("fail record exists")
            check_result = check(fail, response)
            return check_result
    plugin.log("no record")
    return False


plugin.run()
