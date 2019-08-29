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
    plugin.failure_list = []


@plugin.subscribe("sendpay_success")
def notify_sendpay_success(plugin, sendpay_success):
    plugin.log("receive a sendpay_success recored, id: {}, payment_hash: {}".format(sendpay_success['id'], sendpay_success['payment_hash']))
    plugin.success_list.append(sendpay_success)


@plugin.subscribe("sendpay_failure")
def notify_sendpay_failure(plugin, sendpay_failure):
    plugin.log("receive a sendpay_failure recored, id: {}, payment_hash: {}".format(sendpay_failure['data']['id'],
               sendpay_failure['data']['payment_hash']))
    plugin.failure_list.append(sendpay_failure)


@plugin.method('listsendpays_plugin')
def record_lookup(payment_hash, response, plugin):
    return {'sendpay_success': plugin.success_list,
            'sendpay_failure': plugin.failure_list}


plugin.run()
