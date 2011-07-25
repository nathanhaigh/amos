#-- CUSTOM AMOS MACROS --##

##-- AMOS_BUILD_CONDITIONAL([NAME], [DEFAULT]) ---------------------------------
##   sets an automake conditional BUILD_NAME with --enable-NAME --disable-NAME
##   --enable-NAME or --enable-NAME=yes will set BUILD_NAME to TRUE
##   anything else will set BUILD_NAME to FALSE
##
AC_DEFUN([AMOS_BUILD_CONDITIONAL],
[
AC_ARG_ENABLE([$1],
              AS_HELP_STRING(--enable-$1,build $1 dependencies @<:@ARG=$2@:>@),
              [ac_cv_enable_$1=${enableval}],
              [ac_cv_enable_$1=$2])
AC_CACHE_CHECK([whether to build $1],
               [ac_cv_enable_$1],
               [ac_cv_enable_$1=$2])
AM_CONDITIONAL([BUILD_$1],
               [test x"${ac_cv_enable_$1}" = x"yes"])
])


##-- AMOS_C_BITFIELDS ----------------------------------------------------------
##   checks whether C compiler packs bits hi to lo or lo to hi. will set the
##   BITFIELDS_HTOL define if the compiler packs hi to lo, or leave undefined
##   if compiler packs lo to hi. HTOL, that is the compiler packs its bitfields
##   beginning with hi-order bits and moves onward to lo-order bits.
##
AC_DEFUN([AMOS_C_BITFIELDS],
[
AC_CACHE_CHECK([whether bitfields are packed htol],
               ac_cv_c_bitfields_htol,
               [AC_RUN_IFELSE([AC_LANG_SOURCE([[

               struct
               {
                 unsigned char a : 4;
                 unsigned char b : 4;
               } bf;

               int
               main()
               {
                 char * p = (char *) &bf;
                 *p = 0x12;
                 if (bf.a == 1)
                   exit(0);
                 else
                   exit(-1);
               }

               ]])],
               [ac_cv_c_bitfields_htol=yes],
               [ac_cv_c_bitfields_htol=no],
               [ac_cv_c_bitfields_htol=no])])

if test x"$ac_cv_c_bitfields_htol" = x"yes"; then
  AC_DEFINE(BITFIELDS_HTOL, 1,
            [Define to 1 if C compiler packs bits hi-order to lo-order])
fi
])


##-- AMOS_QT4 -------------------------------------------------------------------
# @synopsis AMOS_QT4
#  [--with-Qt4-dir=DIR]
#  [--with-Qt4-include-dir=DIR]
#  [--with-Qt4-bin-dir=DIR]
#  [--with-Qt4-lib-dir=DIR]
#  [--with-Qt4-lib=LIB]
#
# @summary Search for Trolltech's Qt GUI framework.
#
# Modified to only search for Qt4
#
# The following variables are set to either "yes" or "no"
#
#   have_qt4                      // if Qt was found somewhere
#   have_qt4_test                 // if Qt worked
#
# Additionally, the following variables are exported:
#
#   QT_CXXFLAGS
#   QT_LIBS
#   QT_MOC
#
# which respectively contain an "-I" flag pointing to the Qt include
# directory (and "-DQT_THREAD_SUPPORT" when LIB is "qt-mt"), link
# flags necessary to link with Qt and X, the name of the meta object
# compiler and the user interface compiler both with full path, and
# finaly the variable QTDIR as Trolltech likes to see it defined (if
# possible).
#
# Example lines for Makefile.in:
#
#   CXXFLAGS = @QT_CXXFLAGS@
#   MOC      = @QT_MOC@
#
# After the variables have been set, a trial compile and link is
# performed to check the correct functioning of the meta object
# compiler. This test may fail when the different detected elements
# stem from different releases of the Qt framework. In that case, an
# error message is emitted and configure stops.
#
# No common variables such as $LIBS or $CFLAGS are polluted.
#
# Options:
#
# --with-Qt4-dir=DIR: DIR is equal to $QTDIR if you have followed the
# installation instructions of Trolltech. Header files are in
# DIR/include, binary utilities are in DIR/bin and the library is in
# DIR/lib.
#
# --with-Qt4-include-dir=DIR
# --with-Qt4-bin-dir=DIR
# --with-Qt4-lib-dir=DIR
# Override the --with-Qt-dir option for specific dirs.
#
# --with-Qt4-lib=LIB: Use -lLIB to link with the Qt library.
#
# If some option "=no" or, equivalently, a --without-Qt-* version is
# given in stead of a --with-Qt-*, "have_qt4" is set to "no" and the
# other variables are set to the empty string.
#
# @category InstalledPackages
# @author Bastiaan Veelo <Bastiaan.N.Veelo@ntnu.no>
# @version 2005-01-24
# @license AllPermissive

# Copyright (C) 2001, 2002, 2003, 2005, Bastiaan Veelo

# Calls AMOS_PATH_QT_DIRECT (contained in this file) as a subroutine.
AC_DEFUN([AMOS_QT4],
[
  # THANKS! This code includes bug fixes and contributions made by:
  # Tim McClarren,
  # Dennis R. Weilert,
  # Qingning Huo.

  # Custom edits for AMOS made by Adam Phillippy, Michael Schatz

  AC_REQUIRE([AC_PROG_CXX])
  AC_REQUIRE([AC_PATH_X])
  AC_REQUIRE([AC_PATH_XTRA])

  AC_MSG_CHECKING([for Qt4])

  AC_ARG_WITH([Qt4-dir],
    [  --with-Qt4-dir=DIR       DIR is equal to QTDIR if you have followed the
                          installation instructions of Trolltech. Header files
                          are in DIR/include, binary utilities are in DIR/bin
                          and the library is in DIR/lib. Use the options below
                          to override these defaults. On OS X, use this to set
                          the path to the directory with the Qt frameworks.])
  AC_ARG_WITH([Qt4-include-dir], [  --with-Qt4-include-dir=DIR])
  AC_ARG_WITH([Qt4-bin-dir], [  --with-Qt4-bin-dir=DIR])
  AC_ARG_WITH([Qt4-lib-dir], [  --with-Qt4-lib-dir=DIR])
  AC_ARG_WITH([Qt4-lib],
    [  --with-Qt4-lib=LIB       Use -lLIB to link the Qt library])

  if test x"$with_Qt4_dir" = x"no" ||
     test x"$with_Qt4_include_dir" = x"no" ||
     test x"$with_Qt4_bin_dir" = x"no" ||
     test x"$with_Qt4_lib_dir" = x"no" ||
     test x"$with_Qt4_lib" = x"no"; then
    # user disabled Qt. Leave cache alone.
    have_qt4="disabled"
  else
    # "yes" is a bogus option
    if test x"$with_Qt4_dir" = xyes; then
      with_Qt4_dir=
    fi
    if test x"$with_Qt4_include_dir" = xyes; then
      with_Qt4_include_dir=
    fi
    if test x"$with_Qt4_bin_dir" = xyes; then
      with_Qt4_bin_dir=
    fi
    if test x"$with_Qt4_lib_dir" = xyes; then
      with_Qt4_lib_dir=
    fi
    if test x"$with_Qt4_lib" = xyes; then
      with_Qt4_lib=
    fi

    # check for some answers
    if test x"$with_Qt4_dir" != x; then
      amos_qt_dir="$with_Qt4_dir"
    else
      amos_qt_dir=
    fi
    if test x"$with_Qt4_include_dir" != x; then
      amos_qt_include_dir="$with_Qt4_include_dir"
    else
      amos_qt_include_dir=
    fi
    if test x"$with_Qt4_bin_dir" != x; then
      amos_qt_bin_dir="$with_Qt4_bin_dir"
    else
      amos_qt_bin_dir=
    fi
    if test x"$with_Qt4_lib_dir" != x; then
      amos_qt_lib_dir="$with_Qt4_lib_dir"
    else
      amos_qt_lib_dir=
    fi
    if test x"$with_Qt4_lib" != x; then
      amos_qt_lib="$with_Qt4_lib"
    else
      amos_qt_lib=
    fi

    # No Qt unless we discover otherwise
    have_qt4=no

    # Check whether we were supplied with the whole answer
    if test x"$amos_qt_dir" != x ||
       (
       test x"$amos_qt_include_dir" != x &&
       test x"$amos_qt_bin_dir" != x &&
       test x"$amos_qt_lib_dir" != x
       ); then

      if test `uname -s` = Darwin && 
         test -x $amos_qt_dir &&
         test -x "$amos_qt_dir/QtCore.framework" &&
         test -x "$amos_qt_dir/Qt3Support.framework" &&
         test -x "$amos_qt_dir/QtGui.framework"; then
              have_qt4=yes
              amos_qt_LIBS="-F$amos_qt_dir -framework Qt3Support -framework QtGui -framework QtCore"
      else
        if test x"$amos_qt_dir" = x; then
          amos_qt_dir="$amos_qt_bin_dir"
        fi

        if test x"$amos_qt_include_dir" = x; then
          amos_qt_include_dir="$amos_qt_dir/include"
        fi

        if test x"$amos_qt_bin_dir" = x; then
          amos_qt_bin_dir="$amos_qt_dir/bin"
        fi

        if test x"$amos_qt_lib_dir" = x; then
          amos_qt_lib_dir="$amos_qt_dir/lib"
        fi

        if test -x $amos_qt_include_dir &&
           test -x $amos_qt_bin_dir/moc &&
           test -x $amos_qt_lib_dir; then
                have_qt4=yes
                amos_qt_LIBS="-Wl,-O1 -Wl,-rpath,$amos_qt_lib_dir -L$amos_qt_lib_dir -lQt3Support -lQtNetwork -lQtGui -lQtCore -lpng \
                -lgobject-2.0 -lSM -lICE -lXrender -lfontconfig -lfreetype -lXext -lz -lm -lgthread-2.0"
        fi
    fi

    #echo "MCS 3 with_Qt4_dir: \"$with_Qt4_dir\""
    #echo "MCS 3 amos_qt_dir: \"$amos_qt_dir\""
    #echo "MCS 3 amos_qt_bin_dir: \"$amos_qt_bin_dir\""
    #echo "MCS 3 amos_qt_include_dir: \"$amos_qt_include_dir\""
    #echo "MCS 3 amos_qt_lib_dir: \"$amos_qt_lib_dir\""
    #echo "MCS 3 have_qt4: \"$have_qt4\""
    #echo "MCS 3 amos_qt_LIBS: \"$amos_qt_LIBS\""

    # Use cached value or do search
    else
      AC_CACHE_VAL(ac_cv_have_qt4,
      [
        # We are not given a solution and there is no cached value
        AMOS_PATH_QT_DIRECT
        if test x"$amos_qt_dir" = x ||
           test x"$amos_qt_include_dir" = x ||
           test x"$amos_qt_bin_dir" = x ||
           test x"$amos_qt_lib_dir" = x ||
           test x"$amos_qt_lib" = x; then
          # Problem with finding complete Qt. Cache the known absence of Qt.
          ac_cv_have_qt4="have_qt4=no"
        else
          # Record where we found Qt for the cache.
          ac_cv_have_qt4="have_qt4=yes                  \
                       amos_qt_dir=$amos_qt_dir         \
               amos_qt_include_dir=$amos_qt_include_dir \
                   amos_qt_bin_dir=$amos_qt_bin_dir     \
                   amos_qt_lib_dir=$amos_qt_lib_dir     \
                       amos_qt_lib=$amos_qt_lib         \
                      amos_qt_LIBS=\"$amos_qt_LIBS\""
        fi
      ])dnl
      eval "$ac_cv_have_qt4"
    fi # all $amos_qt_* are set
  fi   # $have_qt4 reflects the system status

  if test x"$have_qt4" = xyes; then
    if test `uname -s` = "Darwin" && test x"$amos_qt_dir" != x && test -x "$amos_qt_dir/QtGui.framework"; then
      QT_CXXFLAGS="-I$amos_qt_dir/QtGui.framework/Headers/ -I$amos_qt_dir/QtCore.framework/Headers/ -I$amos_qt_dir/Qt3Support.framework/Headers/ -DQT_QT3SUPPORT_LIB -DQT3_SUPPORT -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED"
    else
      QT_CXXFLAGS="-I$amos_qt_include_dir -I$amos_qt_include_dir/QtGui -I$amos_qt_include_dir/QtCore -I$amos_qt_include_dir/Qt3Support -DQT_QT3SUPPORT_LIB -DQT3_SUPPORT -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED"
      if test x"$amos_qt_lib" = x"qt-mt"; then
          QT_CXXFLAGS="$QT_CXXFLAGS -DQT_THREAD_SUPPORT"
      fi
    fi

    QT_LIBS="$amos_qt_LIBS"
    
    # If amos_qt_dir is defined, utilities are expected to be in the
    # bin subdirectory, else check the qt_bin dir, else check path
    
    if test x"$amos_qt_dir" != x && test -x $amos_qt_dir/bin/moc; then
      QT_MOC="$amos_qt_dir/bin/moc"
    elif test x"$amos_qt_bin_dir" != x && test -x $amos_qt_bin_dir/moc; then
        QT_MOC="$amos_qt_bin_dir/moc"
    else
        QT_MOC="`which moc`"
    fi

    if test ! -x $QT_MOC; then
       have_qt4=no
    fi

    # All variables are defined, report the result
    AC_MSG_RESULT([$have_qt4])
  else
    # Qt was not found
    QT_CXXFLAGS=
    QT_LIBS=
    QT_MOC=
    AC_MSG_RESULT([$have_qt4])
  fi
  AC_SUBST(QT_CXXFLAGS)
  AC_SUBST(QT_LIBS)
  AC_SUBST(QT_MOC)

  # Check if Qt can actually be used as expected
  AC_MSG_CHECKING(whether Qt works)
  AC_CACHE_VAL(ac_cv_qt_test_result,
  [
    ac_cv_qt_test_result="no"

    if test x"$have_qt4" = xyes; then
      cat > amos_qt_test.h << EOF
#include <QObject>
class Test : public QObject
{
Q_OBJECT
public:
  Test() {}
  ~Test() {}
public slots:
  void receive() {}
signals:
  void send();
};
EOF

      cat > amos_qt_main.$ac_ext << EOF

#include "amos_qt_test.h"
#include <QApplication>
int main( int argc, char **argv )
{
  QApplication app( argc, argv );
  Test t;
  QObject::connect( &t, SIGNAL(send()), &t, SLOT(receive()) );
}
EOF

      if AC_TRY_EVAL("$QT_MOC amos_qt_test.h -o moc_amos_qt_test.$ac_ext"); then
        if AC_TRY_EVAL("$CXX $QT_CXXFLAGS -c $CXXFLAGS -o moc_amos_qt_test.o moc_amos_qt_test.$ac_ext"); then
          if AC_TRY_EVAL("$CXX $QT_CXXFLAGS -c $CXXFLAGS -o amos_qt_main.o amos_qt_main.$ac_ext"); then
            if AC_TRY_EVAL("$CXX $QT_LIBS $LIBS -o amos_qt_main amos_qt_main.o moc_amos_qt_test.o $QT_LIBS"); then
              ac_cv_qt_test_result="yes"
            fi
          fi
        fi
      fi

      if test "$ac_cv_qt_test_result" = "no"; then
        echo "$as_me: failed program was:" >&AS_MESSAGE_LOG_FD()
        sed 's/^/| /' amos_qt_main.$ac_ext >&AS_MESSAGE_LOG_FD()
      fi

    fi # if have_qt4
  ])dnl AC_CACHE_VAL ac_cv_qt_test_result
  AC_MSG_RESULT([$ac_cv_qt_test_result])
  rm -f amos_qt_test.h moc_amos_qt_test.$ac_ext moc_amos_qt_test.o \
        amos_qt_main.$ac_ext amos_qt_main.o amos_qt_main

  have_qt4_test=$ac_cv_qt_test_result
])


##-- AMOS_PATH_QT_DIRECT -------------------------------------------------------
# Internal subroutine of AMOS_QT
# Sets if unset:
# amos_qt_dir amos_qt_include_dir amos_qt_bin_dir amos_qt_lib_dir amos_qt_lib
AC_DEFUN([AMOS_PATH_QT_DIRECT],
[
  if test `uname -s` = Darwin && 
     test -x "/Library/Frameworks/QtCore.framework" &&
     test -x "/Library/Frameworks/Qt3Support.framework" &&
     test -x "/Library/Frameworks/QtGui.framework"; then
          amos_qt_dir=/Library/Frameworks/
          amos_qt_lib_dir=$amos_qt_dir
          amos_qt_lib="QtGui"
          amos_qt_include_dir=$amos_qt_dir
          amos_qt_bin_dir=$amos_qt_dir
          amos_qt_LIBS="-F$amos_qt_dir -framework Qt3Support -framework QtGui -framework QtCore"
  else

  if test x"$amos_qt_include_dir" = x; then
    # The following header file is expected to define QT_VERSION_STR.
    qt_direct_test_header=qglobal.h
    # Look for the header file in a standard set of common directories.
    amos_include_path_list="
      `ls -dr /opt/qt*/include 2>/dev/null`
      `ls -dr /usr/local/qt*/include 2>/dev/null`
      `ls -dr /usr/lib/qt*/include 2>/dev/null`
      `ls -dr /usr/lib64/qt*/include 2>/dev/null`
      `ls -dr /usr/include/qt* 2>/dev/null`
      /usr/include
    "
    if test x"$QTDIR" != x; then
      amos_include_path_list="
        $amos_include_path_list
        $QTDIR/include
      "
    fi
    for amos_dir in $amos_include_path_list; do
      if test -r "$amos_dir/$qt_direct_test_header"; then
        amos_dirs="$amos_dirs $amos_dir"
      fi
    done
    # Now look for version 3
    for amos_dir in $amos_dirs; do
      amos_this_ver=`egrep -w '#define QT_VERSION_STR' $amos_dir/$qt_direct_test_header | cut -f2 -d \" | cut -f1 -d \.`
      if test x"$amos_this_ver" = x"3"; then
        amos_qt_include_dir=$amos_dir
      fi
    done
  fi # test $amos_qt_include_dir

  # Are these headers located in a traditional Trolltech installation?
  # That would be $amos_qt_include_dir stripped from its last element:
  if test x"$amos_qt_include_dir" != x; then
    amos_possible_qt_dir=`dirname $amos_qt_include_dir`
    if test x"$amos_qt_dir" = x; then
      amos_qt_dir=$amos_possible_qt_dir;
    fi
    if test x"$amos_qt_bin_dir" = x; then
      amos_qt_bin_dir=$amos_possible_qt_dir/bin
    fi
    if test x"$amos_qt_lib_dir" = x; then
      amos_qt_lib_dir=$amos_possible_qt_dir/lib
    fi
  fi

  if test -x $amos_qt_include_dir &&
     test -x $amos_qt_bin_dir/moc &&
     test -x $amos_qt_lib_dir; then
    # Only look for lib if the user did not supply it already
    if test x"$amos_qt_lib" = x; then
      amos_qt_lib="`ls $amos_qt_lib_dir/libqt* 2>/dev/null | sed -n 1p | sed s@$amos_qt_lib_dir/lib@@ | [sed s@[.].*@@]`"
    fi
    amos_qt_LIBS="-L$amos_qt_lib_dir -l$amos_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
  fi
  fi
])

## -- AMOS LINK TO CA -------------------------------------------------------------
# @synopsys AMOS_CA
#  [--with-CA-dir=DIR]
#
# @summary Directory to use for linking against CA. Used for parsing CA output.
#
# The following variables are set to either "yes" or "no"
#
# have_ca             // if CA was found
# have_ca_test        // if we found the CA files we need
# 
# Additionally, the following variables are exporteD:
#
# which respectively contain an -l flag for CA libraries and a -I flag for the includes.
#
# Options:
#
# --with-CA-dir=DIR: DIR is equal to $CADIR if you have followed the
# installation instructions. Header files are in
# DIR/../../src.
#
AC_DEFUN([AMOS_CA],
[
  AC_REQUIRE([AC_PROG_CXX])
  AC_REQUIRE([AC_PATH_X])
  AC_REQUIRE([AC_PATH_XTRA])

  AC_MSG_CHECKING([for CA])

  AC_ARG_WITH([CA-dir],
    [  --with-CA-dir=DIR       DIR is equal to CADIR if you have followed the
                          installation instructions. Header files are in DIR/../src])

  if test x"$with_CA_dir" = x"no"; then
    # user disabled Boost. Leave cache alone.
    have_ca="disabled"
  else
    # "yes" is a bogus option
    if test x"$with_CA_dir" = xyes; then
      with_CA_dir=
    fi
    # check for some answers
    if test x"$with_CA_dir" != x; then
      amos_ca_dir="$with_CA_dir"
    else
      amos_ca_dir=
    fi
    have_ca=no
    # Check whether we were supplied with the whole answer
    if test x"$amos_ca_dir" != x; then
	    have_ca=yes
    fi # all $amos_ca_* are set
  fi   # $have_ca reflects the system status
  if test x"$have_ca" = xyes; then
    CA_DIR="$amos_ca_dir"
    CA_LDADD="$amos_ca_dir/lib/libCA.a"
    CA_CXXFLAGS="-I$amos_ca_dir/../src -I$amos_ca_dir/../src/AS_MSG -I$amos_ca_dir/../src/AS_UTL -DAMOS_HAVE_CA"
    # All variables are defined, report the result
    AC_MSG_RESULT([$have_ca])
  else
    # CA was not found
    CA_LDADD=
    CA_CXXFLAGS=
    CA_DIR=
    AC_MSG_RESULT([$have_ca])
  fi
  AC_SUBST(CA_LDADD)
  AC_SUBST(CA_CXXFLAGS)
  AC_SUBST(CA_DIR)

  # Check if CA can actually be used as expected
  AC_MSG_CHECKING(whether CA works)
  AC_CACHE_VAL(ac_cv_ca_test_result,
  [
    ac_cv_ca_test_result="no"

    if test x"$have_ca" = xyes; then
      cat > amos_ca_test.h << EOF
#include <utility>
#include "AS_MSG_pmesg.h" 

EOF

      cat > amos_ca_main.$ac_ext << EOF
#include "amos_ca_test.h"

int main( int argc, char **argv )
{
   AS_MSG_setFormatVersion(1);

   GenericMesg pmesg;
   pmesg.t = MESG_FRG;
   if (pmesg.t == MESG_FRG) {
      return 0;
   } else if (pmesg.t == MESG_CCO) {
      return 1;
   }
}
EOF


    if AC_TRY_EVAL("$CXX $CA_CXXFLAGS -c $CXXFLAGS -o amos_ca_main.o amos_ca_main.$ac_ext"); then
      if AC_TRY_EVAL("$CXX $LIBS -o amos_ca_main amos_ca_main.o $CA_LDADD"); then
        ac_cv_ca_test_result="yes"
      fi
    fi

      if test "$ac_cv_ca_test_result" = "no"; then
        echo "$as_me: failed program was:" >&AS_MESSAGE_LOG_FD()
        sed 's/^/| /' amos_ca_main.$ac_ext >&AS_MESSAGE_LOG_FD()

        # CA  does not work turn it off
	CA_LDADD=
        CA_CXXFLAGS=
        CA_DIR=
      fi

    fi # if have_ca
    
  ])dnl AC_CACHE_VAL ac_cv_ca_test_result
  AC_MSG_RESULT([$ac_cv_ca_test_result])
  rm -f amos_ca_test.h amos_ca_main.$ac_ext amos_ca_main.o amos_ca_main

  have_ca_test=$ac_cv_ca_test_result
])

##-- AMOS_BOOST -------------------------------------------------------------------
# @synopsis AMOS_BOOST
#  [--with-Boost-dir=DIR]
#
# @summary Search for Boost Graph Library.
#
# Modified to only search for Boost 1.3*
#
# The following variables are set to either "yes" or "no"
#
#   have_boost                      // if Boost was found somewhere
#   have_boost_test                 // if Boost worked
#
# Additionally, the following variables are exported:
#
#   BOOST_CXXFLAGS
#   BOOST_DIR
#
# which respectively contain an "-I" flag pointing to the Qt include
# directory (and "-DAMOS_HAVE_BOOST").
#
# Example lines for Makefile.in:
#
#   CXXFLAGS = @BOOST_CXXFLAGS@
#
# After the variables have been set, a trial compile and link is
# performed to check the correct functioning of the meta object
# compiler. This test may fail when the different detected elements
# stem from different releases of the Boost framework. In that case, an
# error message is emitted and configure stops.
#
# No common variables such as $LIBS or $CFLAGS are polluted.
#
# Options:
#
# --with-Boost-dir=DIR: DIR is equal to $BOOSTDIR if you have followed the
# installation instructions. Header files are in
# DIR/include/boost-version.
#
# --with-Boost-include-dir=DIR

# If some option "=no" or, equivalently, a --without-Boost-* version is
# given in stead of a --with-Boost-*, "have_boost" is set to "no" and the
# other variables are set to the empty string.

# Calls AMOS_PATH_BOOST_DIRECT (contained in this file) as a subroutine.
AC_DEFUN([AMOS_BOOST],
[
  AC_REQUIRE([AC_PROG_CXX])
  AC_REQUIRE([AC_PATH_X])
  AC_REQUIRE([AC_PATH_XTRA])

  AC_MSG_CHECKING([for Boost])

  AC_ARG_WITH([Boost-dir],
    [  --with-Boost-dir=DIR       DIR is equal to BoostDIR if you have followed the
                          installation instructions. Header files are in DIR/boost])

  if test x"$with_Boost_dir" = x"no"; then
    # user disabled Boost. Leave cache alone.
    have_boost="disabled"
  else
    # "yes" is a bogus option
    if test x"$with_Boost_dir" = xyes; then
      with_Boost_dir=
    fi
    # check for some answers
    if test x"$with_Boost_dir" != x; then
      amos_boost_dir="$with_Boost_dir"
    else
      amos_boost_dir=
    fi
    # No Qt unless we discover otherwise
    have_boost=no
    # Check whether we were supplied with the whole answer
    if test x"$amos_boost_dir" != x; then
	    have_boost=yes
    # Use cached value or do search
    else
      AC_CACHE_VAL(ac_cv_have_boost,
      [
        # We are not given a solution and there is no cached value
        AMOS_PATH_BOOST_DIRECT
        if test x"$amos_boost_dir" = x; then
          # Problem with finding complete Boost. Cache the known absence of Boost.
          ac_cv_have_boost="have_boost=no"
        else
          # Record where we found Boost for the cache.
          ac_cv_have_boost="have_boost=yes                  \
                       		 amos_boost_dir=$amos_boost_dir"
        fi
      ])dnl
      eval "$ac_cv_have_boost"
    fi # all $amos_boost_* are set
  fi   # $have_boostreflects the system status
  if test x"$have_boost" = xyes; then
    BOOST_CXXFLAGS="-I$amos_boost_dir -DAMOS_HAVE_BOOST"
    BOOST_DIR="$amos_boost_dir"
    # All variables are defined, report the result
    AC_MSG_RESULT([$have_boost])
  else
    # Boost was not found
    BOOST_CXXFLAGS=
    BOOST_DIR=
    AC_MSG_RESULT([$have_boost])
  fi
  AC_SUBST(BOOST_CXXFLAGS)
  AC_SUBST(BOOST_DIR)

  # Check if Boost can actually be used as expected
  AC_MSG_CHECKING(whether Boost works)
  AC_CACHE_VAL(ac_cv_boost_test_result,
  [
    ac_cv_boost_test_result="no"

    if test x"$have_boost" = xyes; then
      cat > amos_boost_test.h << EOF
#include <utility>
#include <boost/config.hpp>
#include <boost/utility.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/strong_components.hpp>
#include <boost/graph/graphviz.hpp>

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS> Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
typedef boost::graph_traits<Graph>::edge_descriptor Edge;
typedef boost::graph_traits<Graph>::vertex_iterator VertexIterator;

EOF

      cat > amos_boost_main.$ac_ext << EOF
#include "amos_boost_test.h"

int main( int argc, char **argv )
{	
	int num_vertices = 10;
	int num_edges = num_vertices - 1;
	
	Graph g(num_vertices);
	int i = 0;
	for (i = 0; i < num_edges; i++) {
		boost::add_edge(i, i+1, g);
	}
	
	// access the vertices
	std::pair<VertexIterator, VertexIterator> iter;
   for (iter = boost::vertices(g); iter.first != iter.second; ++iter.first) {
		Vertex source = *iter.first;
	}
}
EOF


    if AC_TRY_EVAL("$CXX $BOOST_CXXFLAGS -c $CXXFLAGS -o amos_boost_main.o amos_boost_main.$ac_ext"); then
      if AC_TRY_EVAL("$CXX $LIBS -o amos_boost_main amos_boost_main.o"); then
        ac_cv_boost_test_result="yes"
      fi
    fi

      if test "$ac_cv_boost_test_result" = "no"; then
        echo "$as_me: failed program was:" >&AS_MESSAGE_LOG_FD()
        sed 's/^/| /' amos_boost_main.$ac_ext >&AS_MESSAGE_LOG_FD()

	     # Boost does not work turn it off
	     BOOST_CXXFLAGS=
        BOOST_DIR=
      fi

    fi # if have_boost
    
  ])dnl AC_CACHE_VAL ac_cv_boost_test_result
  AC_MSG_RESULT([$ac_cv_boost_test_result])
  rm -f amos_boost_test.h amos_boost_main.$ac_ext amos_boost_main.o amos_boost_main

  have_boost_test=$ac_cv_boost_test_result
])


##-- AMOS_PATH_BOOST_DIRECT -------------------------------------------------------
# Internal subroutine of AMOS_BOOST
# Sets if unset:
# amos_boost_dir
AC_DEFUN([AMOS_PATH_BOOST_DIRECT],
[
  if test x"$amos_boost_dir" = x; then
    # The following header file is expected to define BOOST_VERSION.
    boost_direct_test_header=version.hpp
    # Look for the header file in a standard set of common directories.
    amos_include_path_list="
      `ls -dr /opt/boost*/include 2>/dev/null`
      `ls -dr /usr/local/boost*/include 2>/dev/null`
      `ls -dr /usr/local/include/boost* 2>/dev/null`
      `ls -dr /usr/include/boost* 2>/dev/null`
      /usr/include
    "
    if test x"$BOOSTDIR" != x; then
      amos_include_path_list="
        $amos_include_path_list
        $BOOSTDIR/boost
      "
    fi
    for amos_dir in $amos_include_path_list; do
      if test -r "$amos_dir/boost/$boost_direct_test_header"; then
        amos_boost_dir="$amos_dir"
      fi
    done
  fi # test $amos_boost_include_dir
])


##-- END OF m4 --##
