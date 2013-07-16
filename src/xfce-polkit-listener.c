
#include "xfce-polkit-listener.h"

G_DEFINE_TYPE(XfcePolkitListener, xfce_polkit_listener, POLKIT_AGENT_TYPE_LISTENER);

static void initiate_authentication(PolkitAgentListener  *listener,
				    const gchar          *action_id,
				    const gchar          *message,
				    const gchar          *icon_name,
				    PolkitDetails        *details,
				    const gchar          *cookie,
				    GList                *identities,
				    GCancellable         *cancellable,
				    GAsyncReadyCallback   callback,
				    gpointer              user_data)
{
	char** p;
	xfce_dialog_show_info(NULL, "Initiate authentication", "XFCE PolKit");
	for(p = polkit_details_get_keys(details); *p; ++p)
		g_debug("initiate_authentication: %s: %s", *p, polkit_details_lookup(details, *p));
}

static gboolean initiate_authentication_finish(PolkitAgentListener *listener,
				 GAsyncResult *res, GError **error)
{
	g_debug("initiate_authentication_finish");
	return !g_simple_async_result_propagate_error(G_SIMPLE_ASYNC_RESULT(res),
						      error);
}

static void xfce_polkit_listener_finalize(GObject *object)
{
	XfcePolkitListener *self;

	g_return_if_fail(object != NULL);
	g_return_if_fail(XFCE_IS_POLKIT_LISTENER(object));

	self = XFCE_POLKIT_LISTENER(object);

	G_OBJECT_CLASS(xfce_polkit_listener_parent_class)->finalize(object);
}

static void xfce_polkit_listener_class_init(XfcePolkitListenerClass *klass)
{
	GObjectClass *g_object_class;
	PolkitAgentListenerClass* pkal_class;
	g_object_class = G_OBJECT_CLASS(klass);
	g_object_class->finalize = xfce_polkit_listener_finalize;

	pkal_class = POLKIT_AGENT_LISTENER_CLASS(klass);
	pkal_class->initiate_authentication = initiate_authentication;
	pkal_class->initiate_authentication_finish = initiate_authentication_finish;
}

static void xfce_polkit_listener_init(XfcePolkitListener *self)
{
}

PolkitAgentListener* xfce_polkit_listener_new(void)
{
	return g_object_new(XFCE_TYPE_POLKIT_LISTENER, NULL);
}


