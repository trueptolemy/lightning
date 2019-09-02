#include <ccan/array_size/array_size.h>
#include <lightningd/channel.h>
#include <lightningd/json.h>
#include <lightningd/notification.h>
#include <lightningd/peer_htlcs.h>

const char *notification_topics[] = {
	"forward_event"
};

static struct notification *find_notification_by_topic(const char* topic)
{
	static struct notification **notilist = NULL;
	static size_t num_notis;
	if (!notilist)
		notilist = autodata_get(notifications, &num_notis);

	for (size_t i=0; i<num_notis; i++)
		if (streq(notilist[i]->topic, topic))
			return notilist[i];
	return NULL;
}

bool notifications_have_topic(const char *topic)
{
	struct notification *noti = find_notification_by_topic(topic);
	if (noti)
		return true;

	return false;
}

static void connect_notification_serialize(struct json_stream *stream,
					   struct node_id *nodeid,
					   struct wireaddr_internal *addr)
{
	json_add_node_id(stream, "id", nodeid);
	json_add_address_internal(stream, "address", addr);
}

REGISTER_NOTIFICATION(connect,
		      connect_notification_serialize);

void notify_connect(struct lightningd *ld, struct node_id *nodeid,
		    struct wireaddr_internal *addr)
{
	struct jsonrpc_notification *n
		= jsonrpc_notification_start(NULL, connect_notification_gen.topic);
	connect_notification_gen.serialize(n->stream, nodeid, addr);
	jsonrpc_notification_end(n);
	plugins_notify(ld->plugins, take(n));
}

