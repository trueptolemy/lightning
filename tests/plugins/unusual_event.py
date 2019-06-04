#!/usr/bin/env python3
"""This plugin is used to check that unusual_event calls are working correctly.
"""
from lightning import Plugin

plugin = Plugin()


@plugin.init()
def init(configuration, options, plugin):
    plugin.log("initialized")


@plugin.subscribe("unusual_event")
def notify_unusual(plugin, unusual_event):
    plugin.log("Received unusual log:")
    plugin.log("time: {}".format(unusual_event[0]))
    plugin.log("source: {}".format(unusual_event[1]))
    plugin.log("log: {}".format(unusual_event[2]))

plugin.log("Test unusual event notification", 'unusual')
plugin.run()