set  -x                      &&
autoheader                   &&
aclocal                      &&
libtoolize  --copy  --force  &&
automake  --add-missing      &&
autoconf
