#!/usr/bin/env python3
"""This plugin is used to check that unusual_event calls are working correctly.
"""
from lightning import Plugin

plugin = Plugin()


@plugin.init()
def init(configuration, options, plugin):
    plugin.log("initialized")


@plugin.subscribe("disconnect")
def notify_unusual(plugin, writes):
    plugin.log("Received unusual log: {} from {} at {}".format(log, source, time))

plugin.log("test unusual event notification", 'unusual')
plugin.run()