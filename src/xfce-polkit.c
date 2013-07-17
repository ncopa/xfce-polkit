#include <libxfce4ui/libxfce4ui.h>
#include "xfce-polkit-listener.h"

int main(int argc, char *argv[])
{
	PolkitAgentListener *listener;
	PolkitSubject* session;
	GError* err = NULL;
	int rc = 0;

	gtk_init(&argc, &argv);

	listener = xfce_polkit_listener_new();
	session = polkit_unix_session_new_for_process_sync(getpid(), NULL, NULL);

	if(!polkit_agent_listener_register(listener,
					   POLKIT_AGENT_REGISTER_FLAGS_NONE,
					   session, NULL, NULL, &err)) {
		xfce_dialog_show_error(NULL, err, "XFCE PolicyKit Agent");
		rc = 1;
	} else {
		gtk_main();
	}

	g_object_unref(listener);
	g_object_unref(session);
	return rc;
}
