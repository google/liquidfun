#!/bin/sh
#
echo "Generating build information using aclocal, automake and autoconf"
echo "This may take a while ..."

aclocal
autoheader
libtoolize  --copy  --force
automake  --add-missing
autoconf

echo "Now you are ready to run ./configure"
