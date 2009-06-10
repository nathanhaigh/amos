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


##-- AMOS_QT -------------------------------------------------------------------
# @synopsis AMOS_QT
#  [--with-Qt-dir=DIR]
#  [--with-Qt-include-dir=DIR]
#  [--with-Qt-bin-dir=DIR]
#  [--with-Qt-lib-dir=DIR]
#  [--with-Qt-lib=LIB]
#
# @summary Search for Trolltech's Qt GUI framework.
#
# Modified to only search for Qt3
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
# --with-Qt-include-dir=DIR
# --with-Qt-bin-dir=DIR
# --with-Qt-lib-dir=DIR
# Override the --with-Qt-dir option for specific dirs.
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

  # Custom edits for AMOS made by Adam Phillippy

  AC_REQUIRE([AC_PROG_CXX])
  AC_REQUIRE([AC_PATH_X])
  AC_REQUIRE([AC_PATH_XTRA])

  AC_MSG_CHECKING([for Qt])

  AC_ARG_WITH([Qt-dir],
    [  --with-Qt-dir=DIR       DIR is equal to QTDIR if you have followed the
                          installation instructions of Trolltech. Header files
                          are in DIR/include, binary utilities are in DIR/bin
                          and the library is in DIR/lib. Use the options below
                          to override these defaults])
  AC_ARG_WITH([Qt-include-dir], [  --with-Qt-include-dir=DIR])
  AC_ARG_WITH([Qt-bin-dir], [  --with-Qt-bin-dir=DIR])
  AC_ARG_WITH([Qt-lib-dir], [  --with-Qt-lib-dir=DIR])
  AC_ARG_WITH([Qt-lib],
    [  --with-Qt-lib=LIB       Use -lLIB to link the Qt library])

  if test x"$with_Qt_dir" = x"no" ||
     test x"$with_Qt_include_dir" = x"no" ||
     test x"$with_Qt_bin_dir" = x"no" ||
     test x"$with_Qt_lib_dir" = x"no" ||
     test x"$with_Qt_lib" = x"no"; then
    # user disabled Qt. Leave cache alone.
    have_qt="disabled"
  else
    # "yes" is a bogus option
    if test x"$with_Qt_dir" = xyes; then
      with_Qt_dir=
    fi
    if test x"$with_Qt_include_dir" = xyes; then
      with_Qt_include_dir=
    fi
    if test x"$with_Qt_bin_dir" = xyes; then
      with_Qt_bin_dir=
    fi
    if test x"$with_Qt_lib_dir" = xyes; then
      with_Qt_lib_dir=
    fi
    if test x"$with_Qt_lib" = xyes; then
      with_Qt_lib=
    fi
    # check for some answers
    if test x"$with_Qt_dir" != x; then
      amos_qt_dir="$with_Qt_dir"
    else
      amos_qt_dir=
    fi
    if test x"$with_Qt_include_dir" != x; then
      amos_qt_include_dir="$with_Qt_include_dir"
    else
      amos_qt_include_dir=
    fi
    if test x"$with_Qt_bin_dir" != x; then
      amos_qt_bin_dir="$with_Qt_bin_dir"
    else
      amos_qt_bin_dir=
    fi
    if test x"$with_Qt_lib_dir" != x; then
      amos_qt_lib_dir="$with_Qt_lib_dir"
    else
      amos_qt_lib_dir=
    fi
    if test x"$with_Qt_lib" != x; then
      amos_qt_lib="$with_Qt_lib"
    else
      amos_qt_lib=
    fi
    # No Qt unless we discover otherwise
    have_qt=no
    # Check whether we were supplied with the whole answer
    if test x"$amos_qt_dir" != x ||
       (
       test x"$amos_qt_include_dir" != x &&
       test x"$amos_qt_bin_dir" != x &&
       test x"$amos_qt_lib_dir" != x
       ); then

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
        have_qt=yes
        # Only search for the lib if the user did not define one already
        if test x"$amos_qt_lib" = x; then
          amos_qt_lib="`ls $amos_qt_lib_dir/libqt* 2>/dev/null | sed -n 1p | sed s@$amos_qt_lib_dir/lib@@ | [sed s@[.].*@@]`"
        fi
        amos_qt_LIBS="-L$amos_qt_lib_dir -l$amos_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
      else
        have_qt=no
      fi
    # Use cached value or do search
    else
      AC_CACHE_VAL(ac_cv_have_qt,
      [
        # We are not given a solution and there is no cached value
        AMOS_PATH_QT_DIRECT
        if test x"$amos_qt_dir" = x ||
           test x"$amos_qt_include_dir" = x ||
           test x"$amos_qt_bin_dir" = x ||
           test x"$amos_qt_lib_dir" = x ||
           test x"$amos_qt_lib" = x; then
          # Problem with finding complete Qt. Cache the known absence of Qt.
          ac_cv_have_qt="have_qt=no"
        else
          # Record where we found Qt for the cache.
          ac_cv_have_qt="have_qt=yes                  \
                       amos_qt_dir=$amos_qt_dir         \
               amos_qt_include_dir=$amos_qt_include_dir \
                   amos_qt_bin_dir=$amos_qt_bin_dir     \
                   amos_qt_lib_dir=$amos_qt_lib_dir     \
                       amos_qt_lib=$amos_qt_lib         \
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


##-- AMOS_PATH_QT_DIRECT -------------------------------------------------------
# Internal subroutine of AMOS_QT
# Sets if unset:
# amos_qt_dir amos_qt_include_dir amos_qt_bin_dir amos_qt_lib_dir amos_qt_lib
AC_DEFUN([AMOS_PATH_QT_DIRECT],
[
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
          ac_cv_have_qt="have_boost=no"
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
