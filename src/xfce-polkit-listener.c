
#include <libxfce4ui/libxfce4ui.h>

#include "xfce-polkit-listener.h"

G_DEFINE_TYPE(XfcePolkitListener, xfce_polkit_listener, POLKIT_AGENT_TYPE_LISTENER);

typedef struct _AuthDlgData AuthDlgData;
struct _AuthDlgData {
	PolkitAgentSession *session;
	gchar *action_id;
	gchar *cookie;
	GCancellable* cancellable;
	GTask* task;
	GtkWidget *auth_dlg;
	GtkWidget *entry_label;
	GtkWidget *entry;
	GtkWidget *id_combo;
	GtkWidget *status;
};

static void on_cancelled(GCancellable* cancellable, AuthDlgData* d);
static void on_id_combo_user_changed(GtkComboBox *box, AuthDlgData *d);

static void auth_dlg_data_free(AuthDlgData *d)
{
	gtk_widget_destroy(d->auth_dlg);
	g_signal_handlers_disconnect_by_func(d->cancellable, on_cancelled, d);

	g_object_unref(d->cancellable);
	g_object_unref(d->task);
	g_object_unref(d->session);
	g_free(d->action_id);
	g_free(d->cookie);
	g_slice_free(AuthDlgData, d);
}

static void on_cancelled(GCancellable *cancellable, AuthDlgData *d)
{
	if (d->session)
		polkit_agent_session_cancel(d->session);
	else
		auth_dlg_data_free(d);
}

static void on_auth_dlg_response(GtkDialog *dlg, int response, AuthDlgData *d)
{
	if (response == GTK_RESPONSE_OK) {
		const char *txt = gtk_entry_get_text(GTK_ENTRY(d->entry));
		polkit_agent_session_response(d->session, txt);
		gtk_widget_set_sensitive(d->auth_dlg, FALSE);
	} else
		g_cancellable_cancel(d->cancellable);
}

static void on_session_completed(PolkitAgentSession* session,
				 gboolean authorized, AuthDlgData* d)
{
	gtk_widget_set_sensitive(d->auth_dlg, TRUE);
	if (authorized || g_cancellable_is_cancelled(d->cancellable)) {
		gtk_label_set_text(GTK_LABEL(d->status), NULL);
		g_task_return_pointer(d->task, NULL, NULL);
		auth_dlg_data_free(d);
		return;
	}
	gtk_label_set_text(GTK_LABEL(d->status), "Failed. Wrong password?");
	g_object_unref(d->session);
	d->session = NULL;
	gtk_entry_set_text(GTK_ENTRY(d->entry), "");
	gtk_widget_grab_focus(d->entry);
	on_id_combo_user_changed(GTK_COMBO_BOX(d->id_combo), d);
}

static void on_session_request(PolkitAgentSession* session, gchar *req,
			       gboolean visibility, AuthDlgData *d)
{
	g_debug("Request: %s\nVisibility: %i\n", req, visibility);
	gtk_label_set_text(GTK_LABEL(d->entry_label), req);
	gtk_entry_set_visibility(GTK_ENTRY(d->entry), visibility);
}

static void on_id_combo_user_changed(GtkComboBox *combo, AuthDlgData *d)
{
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_combo_box_get_model(combo);
	PolkitIdentity *id;

	if (!gtk_combo_box_get_active_iter(combo, &iter))
		return;

	gtk_tree_model_get(model, &iter, 1, &id, -1);
	if (d->session) {
		g_signal_handlers_disconnect_matched(d->session,
						     G_SIGNAL_MATCH_DATA,
						     0, 0, NULL, NULL, d);
		polkit_agent_session_cancel(d->session);
		g_object_unref(d->session);
	}

	d->session = polkit_agent_session_new(id, d->cookie);
	g_object_unref(id);
	g_signal_connect(d->session, "completed",
			 G_CALLBACK(on_session_completed), d);
	g_signal_connect(d->session, "request",
			 G_CALLBACK(on_session_request), d);
	polkit_agent_session_initiate(d->session);
}

static void add_identities(GtkComboBox *combo, GList *identities)
{
	GList *p;
	GtkCellRenderer *column;
	GtkListStore *store;

	store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_OBJECT);
	for (p = identities; p != NULL; p = p->next) {
		PolkitIdentity *id = (PolkitIdentity *)p->data;
		gchar *str = polkit_identity_to_string(id);
		gtk_list_store_insert_with_values(store, NULL, -1,
						  0, str,
						  1, id,
						  -1);
		g_free(str);
	}
	gtk_combo_box_set_model(combo, GTK_TREE_MODEL(store));
	g_object_unref(store);

	column = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo), column, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo), column,
				       "text", 0, NULL);
}

static GtkWidget *grid2x2(GtkWidget *top_left, GtkWidget *top_right,
			 GtkWidget *bottom_left, GtkWidget *bottom_right)
{
	GtkWidget *table = gtk_table_new(2, 2, FALSE);
	gtk_table_attach_defaults(GTK_TABLE(table), top_left, 0, 1, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(table), top_right, 1, 2, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(table), bottom_left, 0, 1, 1, 2);
	gtk_table_attach_defaults(GTK_TABLE(table), bottom_right, 1, 2, 1, 2);
	gtk_widget_show(table);
	return table;
}

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
	GtkWidget *content;
	GtkWidget *combo_label;
	GtkWidget *grid;
	AuthDlgData *d = g_slice_new0(AuthDlgData);

	char** p;

	for(p = polkit_details_get_keys(details); *p; ++p)
		g_debug("initiate_authentication: %s: %s", *p, polkit_details_lookup(details, *p));

	d->task = g_task_new(listener, cancellable, callback, user_data);
	d->cancellable = cancellable;
	d->action_id = g_strdup(action_id);
	d->cookie = g_strdup(cookie);
	d->auth_dlg = xfce_message_dialog_new(NULL, "XFCE",
			GTK_STOCK_DIALOG_AUTHENTICATION,
			"XFCE PolicyKit Agent",
			message,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OK, GTK_RESPONSE_OK,
			NULL);
	content = gtk_dialog_get_content_area(GTK_DIALOG(d->auth_dlg));

	combo_label = gtk_label_new("Identity:");
	gtk_widget_show(combo_label);

	d->id_combo = gtk_combo_box_new();
	add_identities(GTK_COMBO_BOX(d->id_combo), identities);
	g_signal_connect(d->id_combo, "changed",
			 G_CALLBACK(on_id_combo_user_changed), d);
	gtk_combo_box_set_active(GTK_COMBO_BOX(d->id_combo), 0);
	gtk_widget_show(d->id_combo);

	d->entry_label = gtk_label_new(NULL);
	gtk_widget_show(d->entry_label);

	d->entry = gtk_entry_new();
	gtk_entry_set_visibility(GTK_ENTRY(d->entry), FALSE);
	gtk_widget_show(d->entry);

	grid = grid2x2(combo_label, d->id_combo, d->entry_label, d->entry);
	gtk_box_pack_start(GTK_BOX(content), grid, TRUE, TRUE, 0);

	d->status = gtk_label_new(NULL);
	gtk_box_pack_start(GTK_BOX(content), d->status, TRUE, TRUE, 0);
	gtk_widget_show(d->status);

	g_signal_connect(cancellable, "cancelled", G_CALLBACK(on_cancelled), d);
	g_signal_connect(d->auth_dlg, "response",
			 G_CALLBACK(on_auth_dlg_response), d);

	gtk_window_present(GTK_WINDOW(d->auth_dlg));
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


