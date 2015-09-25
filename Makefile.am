SUBDIRS = src

NULL=

desktopdir=$(sysconfdir)/xdg/autostart
desktop_in_files = \
	xfce-polkit.desktop.in \
	$(NULL)

desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

EXTRA_DIST = \
	$(desktop_DATA) \
	$(NULL)

xfce-polkit.desktop: xfce-polkit.desktop.in
	sed -e 's,@libexecdir@,$(libexecdir),' $< > $@

