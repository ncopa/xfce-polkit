SUBDIRS = src

NULL=

desktopdir=$(sysconfdir)/xdg/autostart
desktop_in_files = \
	xfce-polkit.desktop.in \
	$(NULL)

desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

EXTRA_DIST = \
	$(desktop_in_files) \
	$(NULL)

xfce-polkit.desktop: xfce-polkit.desktop.in
	sed -e 's,@xfce_polkit_libexecdir@,$(libexecdir),' $< > $@

clean-local:
	-rm -f xfce-polkit.desktop
