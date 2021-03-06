xfce-polkit
===========

A simple PolicyKit authentication agent for XFCE

# Required packages

xfce-polkit depends on the following packages:

- glib
- libxfce4ui
- polkit

# Installation

Run the following commands at the root of the repository:
```
mkdir build
cd build
meson --prefix=/usr ..
ninja
```
to compile, then run
```
ninja install
```
as root to install.

# Usage
Make sure there's no other PolicyKit authentication agent running (if there is, `kill` them.)
If you wish to use this by default, make sure `xfce-polkit` is the only authentication agent that is run at boot.
