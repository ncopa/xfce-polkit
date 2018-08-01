xfce-polkit
===========

A simple PolicyKit authentication agent for XFCE

# Installation
Run the following commands at the root of the repository:
```
aclocal
autoconf
automake --add-missing --foreign
./configure
make
```
to compile, then run
```
make install
```
as root to install.

# Usage
Make sure there's no other PolicyKit authentication agent running (if there is, `kill` them.)
If you wish to use this by default, make sure `xfce-polkit` is the only authentication agent that is run at boot.
