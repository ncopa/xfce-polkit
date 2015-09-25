
NULL=

libexec_PROGRAMS = xfce-polkit
xfce_polkit_SOURCES = \
	xfce-polkit.c \
	xfce-polkit-listener.c \
	xfce-polkit-listener.h \
	$(NULL)

xfce_polkit_CFLAGS = @GLIB_CFLAGS@ \
	@LIBXFCE4UI_CFLAGS@ \
	@POLKIT_AGENT_CFLAGS@


xfce_polkit_LDADD = @GLIB_LIBS@ \
	@LIBXFCE4UI_LIBS@ \
	@POLKIT_AGENT_LIBS@


#xfce-polkit-listener.o: xfce-polkit-listener.h

