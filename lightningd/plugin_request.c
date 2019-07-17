#include <ccan/array_size/array_size.h>
#include <ccan/io/io.h>
#include <common/memleak.h>
#include <common/utils.h>
#include <lightningd/jsonrpc.h>
#include <lightningd/plugin_request.h>
#include <wallet/db.h>

const char *plugin_request_labels[] = {
	"bitcoin"
};

bool check_request_label(const char *label)
{
	for (size_t i = 0; i < ARRAY_SIZE(plugin_request_labels); i++) {
		if (streq(plugin_request_labels[i], label))
			return true;
	}
	return false;
}

static struct plugin_request **plugin_request_by_label(const tal_t *ctx,
						const char *label)
{
	static struct plugin_request **requests = NULL;
	static size_t num_requests;
	struct plugin_request **req = tal_arr(ctx, struct plugin_request *, 0);
	bool found = false;

	if (!requests)
		requests = autodata_get(requests, &num_requests);

	for (size_t i = 0; i < num_requests; i++)
		if (streq(requests[i]->label, label)) {
			if (!found)
				found = true;
			tal_arr_expand(&req, requests[i]);
		}

	if (!found)
		*req = NULL;

	return req;
}

static bool check_plugin_request_regist(struct plugin_request *req)
{
	if (req->plugin)
		return true;
	return false;
}

bool check_plugin_request_consistency(void)
{
	bool regist;
	struct plugin_request **req;

	for (size_t i = 0; i < ARRAY_SIZE(plugin_request_labels); i++) {
		req = plugin_request_by_label(tmpctx, plugin_request_labels[i]);
		if (tal_count(req) < 2)
			continue;
		regist = check_plugin_request_regist(req[0]);
		for (size_t i = 1; i < tal_count(req) - 1; i++) {
			regist ^= !check_plugin_request_regist(req[i]);
			if (!regist)
				return false;
		}
	}
	return true;
}

/* Struct containing all the information needed to deserialize and
 * dispatch an eventual plugin_request response. */
struct plugin_request_req {
	const struct plugin_request *request;
	void *cb_arg;
	struct db *db;
};

static struct plugin_request *plugin_request_by_name(const char *name)
{
	static struct plugin_request **requests = NULL;
	static size_t num_requests;
	if (!requests)
		requests = autodata_get(requests, &num_requests);

	for (size_t i=0; i<num_requests; i++)
		if (streq(requests[i]->name, name))
			return requests[i];
	return NULL;
}

bool plugin_request_register(struct plugin *plugin, const char *method)
{
	struct plugin_request *request = plugin_request_by_name(method);
	if (!request) {
		/* No such request name registered */
		return false;
	} else if (request->plugin != NULL) {
		/* Another plugin already registered for this name */
		return false;
	}
	request->plugin = plugin;
	return true;
}

/* FIXME: Remove dummy hook, once we have a real one */
REGISTER_PLUGIN_REQUEST(hello, NULL, void *, NULL, void *, bitcoin);

/**
 * Callback to be passed to the jsonrpc_request.
 *
 * Unbundles the arguments, deserializes the response and dispatches
 * it to the request callback.
 */
static void plugin_request_callback(const char *buffer, const jsmntok_t *toks,
				 const jsmntok_t *idtok,
				 struct plugin_request_req *r)
{
	const jsmntok_t *resulttok = json_get_member(buffer, toks, "result");

	if (!resulttok)
		fatal("Plugin for %s returned non-result response %.*s",
		      r->request->name,
		      toks->end - toks->start, buffer + toks->start);

	db_begin_transaction(r->db);
	r->request->response_cb(r->cb_arg, buffer, resulttok);
	db_commit_transaction(r->db);
	tal_free(r);
}

void plugin_request_call_(struct lightningd *ld, const struct plugin_request *request,
		       void *payload, void *cb_arg)
{
	struct jsonrpc_request *req;
	struct plugin_request_req *pr_req;
	if (request->plugin) {
		/* If we have a plugin that has registered for this
		 * request, serialize and call it */
		/* FIXME: technically this is a leak, but we don't
		 * currently have a list to store these. We might want
		 * to eventually to inspect in-flight requests. */
		pr_req = notleak(tal(request->plugin, struct plugin_request_req));
		req = jsonrpc_request_start(NULL, request->name,
					    plugin_get_log(request->plugin),
					    plugin_request_callback, pr_req);
		pr_req->request = request;
		pr_req->cb_arg = cb_arg;
		pr_req->db = ld->wallet->db;
		request->serialize_payload(payload, req->stream);
		jsonrpc_request_end(req);
		plugin_request_send(request->plugin, req);
	} else {
		/* If no plugin has registered for this request, just
		 * call the callback with a NULL result. Saves us the
		 * roundtrip to the serializer and deserializer. If we
		 * were expecting a default response it should have
		 * been part of the `cb_arg`. */
		request->response_cb(cb_arg, NULL, NULL);
	}
}

