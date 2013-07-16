#ifndef __XFCE_POLKIT_LISTENER_H__

#define POLKIT_AGENT_I_KNOW_API_IS_SUBJECT_TO_CHANGE
#include <polkitagent/polkitagent.h>

G_BEGIN_DECLS

#define XFCE_TYPE_POLKIT_LISTENER             (xfce_polkit_listener_get_type())
#define XFCE_POLKIT_LISTENER(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), XFCE_TYPE_POLKIT_LISTENER, XfcePolkitListener))
#define XFCE_POLKIT_LISTENER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), XFCE_TYPE_POLKIT_LISTENER, XfcePolkitListenerClass))
#define XFCE_IS_POLKIT_LISTENER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj), XFCE_TYPE_POLKIT_LISTENER))
#define XFCE_IS_POLKIT_LISTENER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), XFCE_TYPE_POLKIT_LISTENER))
#define XFCE_POLKIT_LISTENER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS((obj), XFCE_TYPE_POLKIT_LISTENER, XfcePolkitListenerClass))


typedef struct _XfcePolkitListener	XfcePolkitListener;
typedef struct _XfcePolkitListenerClass	XfcePolkitListenerClass;

struct _XfcePolkitListener {
		PolkitAgentListener parent;
};

struct _XfcePolkitListenerClass {
		PolkitAgentListenerClass parent_class;
};

GType xfce_polkit_listener_get_type(void);
PolkitAgentListener* xfce_polkit_listener_new(void);

G_END_DECLS

#endif

