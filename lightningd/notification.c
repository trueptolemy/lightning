#include "lightningd/notification.h"
#include <ccan/array_size/array_size.h>

const char *notification_topics[] = {
	"connect",
	"disconnect",
	"unusual_event"
};

bool notifications_have_topic(const char *topic)
{
	for (size_t i=0; i<ARRAY_SIZE(notification_topics); i++)
		if (streq(topic, notification_topics[i]))
			return true;
	return false;
}

void notify_connect(struct lightningd *ld, struct node_id *nodeid,
		    struct wireaddr_internal *addr)
{
	struct jsonrpc_notification *n =
	    jsonrpc_notification_start(NULL, notification_topics[0]);
	json_add_node_id(n->stream, "id", nodeid);
	json_add_address_internal(n->stream, "address", addr);
	jsonrpc_notification_end(n);
	plugins_notify(ld->plugins, take(n));
}

void notify_disconnect(struct lightningd *ld, struct node_id *nodeid)
{
	struct jsonrpc_notification *n =
	    jsonrpc_notification_start(NULL, notification_topics[1]);
	json_add_node_id(n->stream, "id", nodeid);
	jsonrpc_notification_end(n);
	plugins_notify(ld->plugins, take(n));
}

void notify_unusual_event(struct lightningd *ld, struct log *log,
		    struct log_entry *l)
{
	struct jsonrpc_notification *n =
	    jsonrpc_notification_start(NULL, notification_topics[2]);
	struct timerel diff = time_between(l->time, log->lr->init_time);
	json_object_start(n->stream, "unusual_event");
	json_add_time(n->stream, "time", diff.ts);
	json_add_string(n->stream, "source", l->prefix);
	json_add_string(n->stream, "log", l->log);
	json_object_end(n->stream); /* .unusual_event */
	jsonrpc_notification_end(n);
	plugins_notify(ld->plugins, take(n));
}