#!/usr/bin/env python3
"""This plugin is used to check that warning(unusual/broken level log) calls are working correctly.
"""
from lightning import Plugin

plugin = Plugin()


@plugin.init()
def init(configuration, options, plugin):
    plugin.log("initialized")


@plugin.subscribe("warning")
def notify_warning(plugin, warning):
    plugin.log("Received warning(unusual log):")
    plugin.log("level: {}".format(warning['level']))
    plugin.log("time: {}".format(warning['time']))
    plugin.log("source: {}".format(warning['source']))
    plugin.log("log: {}".format(warning['log']))


@plugin.method("bad")
def pretendbad(plugin, event):
    """Log an 'unusual' level entry.
    And in plugin, we use 'warn' instead of 'unusual'
    """
    plugin.log("{}".format(event), 'warn')


plugin.run()
