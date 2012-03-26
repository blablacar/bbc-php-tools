dnl $Id$
dnl config.m4 for extension comuto

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(comuto, for comuto support,
dnl Make sure that the comment is aligned:
dnl [  --with-comuto             Include comuto support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(comuto, whether to enable comuto support,
dnl Make sure that the comment is aligned:
[  --enable-comuto           Enable comuto support])

if test "$PHP_COMUTO" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-comuto -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/comuto.h"  # you most likely want to change this
  dnl if test -r $PHP_COMUTO/$SEARCH_FOR; then # path given as parameter
  dnl   COMUTO_DIR=$PHP_COMUTO
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for comuto files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       COMUTO_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$COMUTO_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the comuto distribution])
  dnl fi

  dnl # --with-comuto -> add include path
  dnl PHP_ADD_INCLUDE($COMUTO_DIR/include)

  dnl # --with-comuto -> check for lib and symbol presence
  dnl LIBNAME=comuto # you may want to change this
  dnl LIBSYMBOL=comuto # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $COMUTO_DIR/lib, COMUTO_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_COMUTOLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong comuto lib version or lib not found])
  dnl ],[
  dnl   -L$COMUTO_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(COMUTO_SHARED_LIBADD)

  PHP_NEW_EXTENSION(comuto, comuto.c, $ext_shared)
fi
