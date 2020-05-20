project('xfce-polkit', 'c', version : '0.3.0')

prefix = get_option('prefix')
libexecdir = join_paths(prefix, get_option('libexecdir'))
sysconfdir = join_paths(prefix, get_option('sysconfdir'))
autostartdir = join_paths(sysconfdir, 'xdg', 'autostart')

deps = [
  dependency('glib-2.0'),
  dependency('libxfce4ui-2'),
  dependency('polkit-agent-1'),
]

sources = files([
  'src/xfce-polkit.c',
  'src/xfce-polkit-listener.c',
])

executable('xfce-polkit', sources,
  dependencies: deps,
  install: true,
  install_dir: libexecdir
)

desktopconf = configuration_data()
desktopconf.set('xfce_polkit_libexecdir', libexecdir)
configure_file(
  input: 'xfce-polkit.desktop.in',
  output: 'xfce-polkit.desktop',
  configuration: desktopconf,
  install_dir: autostartdir
)
