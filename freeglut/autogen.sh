#! /bin/sh
set -e
aclocal
autoheader
libtoolize --copy --force
automake --add-missing
autoconf
