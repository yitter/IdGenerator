dnl $Id$
dnl config.m4 for extension snowdrift

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(snowdrift, for snowdrift support,
dnl Make sure that the comment is aligned:
dnl [  --with-snowdrift             Include snowdrift support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(snowdrift, whether to enable snowdrift support,
dnl Make sure that the comment is aligned:
[  --enable-snowdrift           Enable snowdrift support])

if test "$PHP_SNOWDRIFT" != "no"; then
  dnl Write more examples of tests here...

  dnl # get library FOO build options from pkg-config output
  dnl AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
  dnl AC_MSG_CHECKING(for libfoo)
  dnl if test -x "$PKG_CONFIG" && $PKG_CONFIG --exists foo; then
  dnl   if $PKG_CONFIG foo --atleast-version 1.2.3; then
  dnl     LIBFOO_CFLAGS=`$PKG_CONFIG foo --cflags`
  dnl     LIBFOO_LIBDIR=`$PKG_CONFIG foo --libs`
  dnl     LIBFOO_VERSON=`$PKG_CONFIG foo --modversion`
  dnl     AC_MSG_RESULT(from pkgconfig: version $LIBFOO_VERSON)
  dnl   else
  dnl     AC_MSG_ERROR(system libfoo is too old: version 1.2.3 required)
  dnl   fi
  dnl else
  dnl   AC_MSG_ERROR(pkg-config not found)
  dnl fi
  dnl PHP_EVAL_LIBLINE($LIBFOO_LIBDIR, SNOWDRIFT_SHARED_LIBADD)
  dnl PHP_EVAL_INCLINE($LIBFOO_CFLAGS)

  dnl # --with-snowdrift -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/snowdrift.h"  # you most likely want to change this
  dnl if test -r $PHP_SNOWDRIFT/$SEARCH_FOR; then # path given as parameter
  dnl   SNOWDRIFT_DIR=$PHP_SNOWDRIFT
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for snowdrift files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       SNOWDRIFT_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$SNOWDRIFT_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the snowdrift distribution])
  dnl fi

  dnl # --with-snowdrift -> add include path
  dnl PHP_ADD_INCLUDE($SNOWDRIFT_DIR/include)

  dnl # --with-snowdrift -> check for lib and symbol presence
  dnl LIBNAME=snowdrift # you may want to change this
  dnl LIBSYMBOL=snowdrift # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $SNOWDRIFT_DIR/$PHP_LIBDIR, SNOWDRIFT_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_SNOWDRIFTLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong snowdrift lib version or lib not found])
  dnl ],[
  dnl   -L$SNOWDRIFT_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(SNOWDRIFT_SHARED_LIBADD)
  snowdrift_source_file="snowdrift.c\
  src/snowflake/snowflake.c\
  src/snowflake/shm.c\
  src/snowflake/spinlock.c
  "
  PHP_NEW_EXTENSION(snowdrift, $snowdrift_source_file, $ext_shared)
fi
