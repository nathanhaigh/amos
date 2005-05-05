#-- CUSTOM AMOS MACROS --##

##-- AMOS_BUILD_CONDITIONAL([NAME], [DEFAULT])
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


##-- AMOS_C_BITFIELDS
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


##-- AMOS_QT
# @synopsis AMOS_QT [--with-Qt-dir=DIR] [--with-Qt-lib=LIB]
#
# @summary Search for Trolltech's Qt GUI framework.
#
# Searches common directories for Qt include files, libraries and Qt
# binary utilities. The macro supports several different versions of
# the Qt framework being installed on the same machine. Without
# options, the macro is designed to look for the latest library,
# i.e., the highest definition of QT_VERSION in qglobal.h. By use of
# one or more options a different library may be selected. There are
# two different sets of options. Both sets contain the option
# --with-Qt-lib=LIB which can be used to force the use of a
# particular version of the library file when more than one are
# available. LIB must be in the form as it would appear behind the
# "-l" option to the compiler. Examples for LIB would be "qt-mt" for
# the multi-threaded version and "qt" for the regular version. In
# addition to this, the first set consists of an option
# --with-Qt-dir=DIR which can be used when the installation conforms
# to Trolltech's standard installation, which means that header files
# are in DIR/include, binary utilities are in DIR/bin and the library
# is in DIR/lib. The second set of options can be used to indicate
# individual locations for the header files, the binary utilities and
# the library file, in addition to the specific version of the
# library file.
#
# The following variables are set to either "yes" or "no"
#
#   have_qt                      // if Qt was found somewhere
#   have_qt_test                 // if Qt worked
#
# Additionally, the following variables are exported:
#
#   QT_CXXFLAGS
#   QT_LIBS
#   QT_MOC
#   QT_UIC
#   QT_DIR
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
# --with-Qt-dir=DIR: DIR is equal to $QTDIR if you have followed the
# installation instructions of Trolltech. Header files are in
# DIR/include, binary utilities are in DIR/bin and the library is in
# DIR/lib.
#
# --with-Qt-lib=LIB: Use -lLIB to link with the Qt library.
#
# If some option "=no" or, equivalently, a --without-Qt-* version is
# given in stead of a --with-Qt-*, "have_qt" is set to "no" and the
# other variables are set to the empty string.
#
# @category InstalledPackages
# @author Bastiaan Veelo <Bastiaan.N.Veelo@ntnu.no>
# @version 2005-01-24
# @license AllPermissive

# Copyright (C) 2001, 2002, 2003, 2005, Bastiaan Veelo

# Calls AMOS_PATH_QT_DIRECT (contained in this file) as a subroutine.
AC_DEFUN([AMOS_QT],
[
  # THANKS! This code includes bug fixes and contributions made by:
  # Tim McClarren,
  # Dennis R. Weilert,
  # Qingning Huo.

  AC_REQUIRE([AC_PROG_CXX])
  AC_REQUIRE([AC_PATH_X])
  AC_REQUIRE([AC_PATH_XTRA])

  AC_MSG_CHECKING([for Qt])

  AC_ARG_WITH([Qt-dir],
    [  --with-Qt-dir=DIR       DIR is equal to \$QTDIR if you have followed the
                          installation instructions of Trolltech. Header
                          files are in DIR/include, binary utilities are
                          in DIR/bin and the library is in DIR/lib])
  AC_ARG_WITH([Qt-lib],
    [  --with-Qt-lib=LIB       Use -lLIB to link with the Qt library])
  if test x"$with_Qt_dir" = x"no" ||
     test x"$with_Qt_lib" = x"no"; then
    # user disabled Qt. Leave cache alone.
    have_qt="User disabled Qt."
  else
    # "yes" is a bogus option
    if test x"$with_Qt_dir" = xyes; then
      with_Qt_dir=
    fi
    if test x"$with_Qt_lib" = xyes; then
      with_Qt_lib=
    fi
    # No Qt unless we discover otherwise
    have_qt=no
    # Check whether we are requested to link with a specific version
    if test x"$with_Qt_lib" != x; then
      amos_qt_lib="$with_Qt_lib"
    fi
    # Check whether we were supplied with an answer already
    if test x"$with_Qt_dir" != x; then
      have_qt=yes
      amos_qt_dir="$with_Qt_dir"
      amos_qt_include_dir="$with_Qt_dir/include"
      amos_qt_bin_dir="$with_Qt_dir/bin"
      amos_qt_lib_dir="$with_Qt_dir/lib"
      # Only search for the lib if the user did not define one already
      if test x"$amos_qt_lib" = x; then
        amos_qt_lib="`ls $amos_qt_lib_dir/libqt* | sed -n 1p |
                     sed s@$amos_qt_lib_dir/lib@@ | [sed s@[.].*@@]`"
      fi
      amos_qt_LIBS="-L$amos_qt_lib_dir -l$amos_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
    else
      # Use cached value or do search, starting with suggestions from
      # the command line
      AC_CACHE_VAL(ac_cv_have_qt,
      [
        # We are not given a solution and there is no cached value.
        amos_qt_dir=NO
        amos_qt_include_dir=NO
        amos_qt_lib_dir=NO
        if test x"$amos_qt_lib" = x; then
          amos_qt_lib=NO
        fi
        AMOS_PATH_QT_DIRECT
        if test "$amos_qt_dir" = NO ||
           test "$amos_qt_include_dir" = NO ||
           test "$amos_qt_lib_dir" = NO ||
           test "$amos_qt_lib" = NO; then
          # Problem with finding complete Qt.  Cache the known absence of Qt.
          ac_cv_have_qt="have_qt=no"
        else
          # Record where we found Qt for the cache.
          ac_cv_have_qt="have_qt=yes                  \
                       amos_qt_dir=$amos_qt_dir         \
               amos_qt_include_dir=$amos_qt_include_dir \
                   amos_qt_bin_dir=$amos_qt_bin_dir     \
                      amos_qt_LIBS=\"$amos_qt_LIBS\""
        fi
      ])dnl
      eval "$ac_cv_have_qt"
    fi # all $amos_qt_* are set
  fi   # $have_qt reflects the system status
  if test x"$have_qt" = xyes; then
    QT_CXXFLAGS="-I$amos_qt_include_dir"
    if test x"$amos_qt_lib" = x"qt-mt"; then
        QT_CXXFLAGS="$QT_CXXFLAGS -DQT_THREAD_SUPPORT"
    fi
    QT_DIR="$amos_qt_dir"
    QT_LIBS="$amos_qt_LIBS"
    # If amos_qt_dir is defined, utilities are expected to be in the
    # bin subdirectory
    if test x"$amos_qt_dir" != x; then
        if test -x "$amos_qt_dir/bin/uic"; then
          QT_UIC="$amos_qt_dir/bin/uic"
        else
          # Old versions of Qt don't have uic
          QT_UIC=
        fi
      QT_MOC="$amos_qt_dir/bin/moc"
    else
      # Or maybe we are told where to look for the utilities
      if test x"$amos_qt_bin_dir" != x; then
        if test -x "$amos_qt_bin_dir/uic"; then
          QT_UIC="$amos_qt_bin_dir/uic"
        else
          # Old versions of Qt don't have uic
          QT_UIC=
        fi
        QT_MOC="$amos_qt_bin_dir/moc"
      else
      # Last possibility is that they are in $PATH
        QT_UIC="`which uic`"
        QT_MOC="`which moc`"
      fi
    fi
    # All variables are defined, report the result
    AC_MSG_RESULT([$have_qt])
  else
    # Qt was not found
    QT_CXXFLAGS=
    QT_DIR=
    QT_LIBS=
    QT_UIC=
    QT_MOC=
    AC_MSG_RESULT([$have_qt])
  fi
  AC_SUBST(QT_CXXFLAGS)
  AC_SUBST(QT_DIR)
  AC_SUBST(QT_LIBS)
  AC_SUBST(QT_UIC)
  AC_SUBST(QT_MOC)

  # Check if Qt can actually be used as expected
  AC_MSG_CHECKING(whether Qt works)
  AC_CACHE_VAL(ac_cv_qt_test_result,
  [
    ac_cv_qt_test_result="no"

    if test x"$have_qt" = xyes; then
      cat > amos_qt_test.h << EOF
#include <qobject.h>
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
#include <qapplication.h>
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
            if AC_TRY_EVAL("$CXX $QT_LIBS $LIBS -o amos_qt_main amos_qt_main.o moc_amos_qt_test.o"); then
              ac_cv_qt_test_result="yes"
            fi
          fi
        fi
      fi

      if test "$ac_cv_qt_test_result" = "no"; then
        echo "$as_me: failed program was:" >&AS_MESSAGE_LOG_FD()
        sed 's/^/| /' amos_qt_main.$ac_ext >&AS_MESSAGE_LOG_FD()
      fi

    fi # if have_qt
  ])dnl AC_CACHE_VAL ac_cv_qt_test_result
  AC_MSG_RESULT([$ac_cv_qt_test_result])
  rm -f amos_qt_test.h moc_amos_qt_test.$ac_ext moc_amos_qt_test.o \
        amos_qt_main.$ac_ext amos_qt_main.o amos_qt_main

  have_qt_test=$ac_cv_qt_test_result
])



# Internal subroutine of AMOS_QT
# Set amos_qt_dir amos_qt_lib
AC_DEFUN([AMOS_PATH_QT_DIRECT],
[
  # The following header file is expected to define QT_VERSION.
  qt_direct_test_header=qglobal.h
  # Look for the header file in a standard set of common directories.
  amos_include_path_list="
    /usr/include
    `ls -dr /usr/include/qt* 2>/dev/null`
    `ls -dr /usr/lib/qt*/include 2>/dev/null`
    `ls -dr /usr/local/qt*/include 2>/dev/null`
    `ls -dr /opt/qt*/include 2>/dev/null`
  "
  for amos_dir in $amos_include_path_list; do
    if test -r "$amos_dir/$qt_direct_test_header"; then
      amos_dirs="$amos_dirs $amos_dir"
    fi
  done
  # Now look for the newest in this list
  amos_prev_ver=0
  for amos_dir in $amos_dirs; do
    amos_this_ver=`egrep -w '#define QT_VERSION' $amos_dir/$qt_direct_test_header | sed s/'#define QT_VERSION'//`
    if expr $amos_this_ver '>' $amos_prev_ver > /dev/null; then
      amos_qt_include_dir=$amos_dir
      amos_prev_ver=$amos_this_ver
    fi
  done

  # Are these headers located in a traditional Trolltech installation?
  # That would be $amos_qt_include_dir stripped from its last element:
  amos_possible_qt_dir=`dirname $amos_qt_include_dir`
  if test -x $amos_possible_qt_dir/bin/moc &&
     ls $amos_possible_qt_dir/lib/libqt* > /dev/null; then
    # Then the rest is a piece of cake
    amos_qt_dir=$amos_possible_qt_dir
    amos_qt_bin_dir="$amos_qt_dir/bin"
    amos_qt_lib_dir="$amos_qt_dir/lib"
    # Only look for lib if the user did not supply it already
    if test x"$amos_qt_lib" = xNO; then
      amos_qt_lib="`ls $amos_qt_lib_dir/libqt* | sed -n 1p |
                   sed s@$amos_qt_lib_dir/lib@@ | [sed s@[.].*@@]`"
    fi
    amos_qt_LIBS="-L$amos_qt_lib_dir -l$amos_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
  else
    # There is no valid definition for $QTDIR as Trolltech likes to see it
    amos_qt_dir=
    ## Look for Qt library ##
    # Normally, when there is no traditional Trolltech installation,
    # the library is installed in a place where the linker finds it
    # automatically.
    # If the user did not define the library name, try with qt
    if test x"$amos_qt_lib" = xNO; then
      amos_qt_lib=qt
    fi
    qt_direct_test_header=qapplication.h
    qt_direct_test_main="
      int argc;
      char ** argv;
      QApplication app(argc,argv);
    "
    # See if we find the library without any special options.
    # Don't add top $LIBS permanently yet
    amos_save_LIBS="$LIBS"
    LIBS="-l$amos_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
    amos_qt_LIBS="$LIBS"
    amos_save_CXXFLAGS="$CXXFLAGS"
    CXXFLAGS="-I$amos_qt_include_dir"
    AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <$qt_direct_test_header>]],
                                    [[$qt_direct_test_main]])],
    [
      # Success.
      # We can link with no special library directory.
      amos_qt_lib_dir=
    ], [
      # That did not work. Try the multi-threaded version
      echo "Non-critical error, please neglect the above." >&AS_MESSAGE_LOG_FD()
      amos_qt_lib=qt-mt
      LIBS="-l$amos_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
      AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <$qt_direct_test_header>]],
                                      [[$qt_direct_test_main]])],
      [
        # Success.
        # We can link with no special library directory.
        amos_qt_lib_dir=
      ], [
        # That did not work. Try the OpenGL version
        echo "Non-critical error, please neglect the above." >&AS_MESSAGE_LOG_FD()
        amos_qt_lib=qt-gl
        LIBS="-l$amos_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
        AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <$qt_direct_test_header>]],
                                        [[$qt_direct_test_main]])],
        [
          # Success.
          # We can link with no special library directory.
          amos_qt_lib_dir=
        ], [
          # That did not work. Maybe a library version I don't know about?
          echo "Non-critical error, please neglect the above." >&AS_MESSAGE_LOG_FD()
          # Look for some Qt lib in a standard set of common directories.
          amos_dir_list="
            `echo $amos_qt_includes | sed ss/includess`
            /lib
            /usr/lib
            /usr/local/lib
            /opt/lib
            `ls -dr /usr/lib/qt* 2>/dev/null`
            `ls -dr /usr/local/qt* 2>/dev/null`
            `ls -dr /opt/qt* 2>/dev/null`
          "
          for amos_dir in $amos_dir_list; do
            if ls $amos_dir/libqt*; then
              # Gamble that it's the first one...
              amos_qt_lib="`ls $amos_dir/libqt* | sed -n 1p |
                          sed s@$amos_dir/lib@@ | sed s/[.].*//`"
              amos_qt_lib_dir="$amos_dir"
              break
            fi
          done
          # Try with that one
          LIBS="-l$amos_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
          AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <$qt_direct_test_header>]],
                                          [[$qt_direct_test_main]])],
          [
            # Success.
            # We can link with no special library directory.
            amos_qt_lib_dir=
          ], [
            # Leave amos_qt_lib_dir defined
          ])
        ])
      ])
    ])
    if test x"$amos_qt_lib_dir" != x; then
      amos_qt_LIBS="-l$amos_qt_lib_dir $LIBS"
    else
      amos_qt_LIBS="$LIBS"
    fi
    LIBS="$amos_save_LIBS"
    CXXFLAGS="$amos_save_CXXFLAGS"
  fi # Done setting up for non-traditional Trolltech installation
])

##-- END OF m4 --##