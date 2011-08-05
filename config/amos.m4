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
#  [--with-qmake-qt4=PATH]
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

# Calls AMOS_PATH_QT_DIRECT (contained in this file) as a subroutine.
AC_DEFUN([AMOS_QT4],
[
  AC_REQUIRE([AC_PROG_CXX])
  AC_REQUIRE([AC_PATH_X])
  AC_REQUIRE([AC_PATH_XTRA])

  AC_MSG_CHECKING([for Qt4])

  AC_ARG_WITH([qmake-qt4], [  --with-qmake-qt4=CMD    path to qmake])
  
  if test x"$with_qmake_qt4" = x"no"; then
    have_qt4="disabled"
  else
    have_qt4=no

    if test x"$with_qmake_qt4" = x"yes"; then
      with_qmake_qt4=
    fi

    if test x"$with_qmake_qt4" = x; then
      with_qmake_qt4="`which qmake`"
    fi

    if test x"$with_qmake_qt4" != x; then
       if test -x $with_qmake_qt4; then
         if test `uname` == "Darwin"; then
           with_qmake_qt4="$with_qmake_qt4 -spec macx-g++"
         fi
         QT_QMAKE=$with_qmake_qt4
         have_qt4=yes
       fi
    fi
  fi

  AC_MSG_RESULT([$have_qt4])
  AC_SUBST(QT_QMAKE)


  # Check if Qt can actually be used as expected
  AC_MSG_CHECKING(whether Qt4 works)
  AC_CACHE_VAL(ac_cv_qt_test_result,
  [
    ac_cv_qt_test_result="no"

    if test x"$have_qt4" = xyes; then
      cat > amos_qt.h << EOF
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

      cat > amos_qt.$ac_ext << EOF

#include "amos_qt.h"
#include <QApplication>
int main( int argc, char **argv )
{
  QApplication app( argc, argv );
  Test t;
  QObject::connect( &t, SIGNAL(send()), &t, SLOT(receive()) );
}
EOF

      cat > amos_qt.pro << EOF

CONFIG += thread
SOURCES += amos_qt.$ac_ext
HEADERS += amos_qt.h
QT += qt3support
EOF
     
     if AC_TRY_EVAL("$QT_QMAKE amos_qt.pro -o amos_qt.qmake"); then
       ## get the make flags from the qmake derived Makefile
       QT_CXXFLAGS=`grep ^DEFINES amos_qt.qmake | cut -f2 -d'='`

       ## rewrite relative paths to become absolute
       CWD=`pwd`
       QT_INC=`grep ^INCPATH amos_qt.qmake | cut -f2 -d'=' | sed "s^-I../^-I$CWD/../^g"`
       QT_CXXFLAGS="$QT_CXXFLAGS $QT_INC"

       QT_LIBS=`grep ^LIBS amos_qt.qmake | cut -f2 -d'='`
       QT_LIBS=`echo $QT_LIBS | tr ' ' '\n' | grep -v SUBLIBS | tr '\n' ' '`

       ## now get the moc command from the debug output
       $QT_QMAKE -d amos_qt.pro -o amos_qt.qmake 2> amos_qt.qmake.err
       QT_MOC=`grep 'QMAKE_MOC ===' amos_qt.qmake.err | cut -f4 -d'='` 
       
       #echo "MCS: QT_MOC: \"$QT_MOC\""
       #echo "MCS: QT_CXXFLAGS: \"$QT_CXXFLAGS\""
       #echo "MCS: QT_LIBS: \"$QT_LIBS\""

       if AC_TRY_EVAL("$QT_MOC amos_qt.h -o moc_amos_qt.$ac_ext"); then
         if AC_TRY_EVAL("$CXX $QT_CXXFLAGS -c $CXXFLAGS -o moc_amos_qt.o moc_amos_qt.$ac_ext"); then
           if AC_TRY_EVAL("$CXX $QT_CXXFLAGS -c $CXXFLAGS -o amos_qt.o amos_qt.$ac_ext"); then
             if AC_TRY_EVAL("$CXX $LIBS -o amos_qt amos_qt.o moc_amos_qt.o $QT_LIBS"); then
               ac_cv_qt_test_result="yes"
             fi
           fi
         fi
       fi
     fi

     if test "$ac_cv_qt_test_result" = "no"; then
       echo "$as_me: failed program was:" >&AS_MESSAGE_LOG_FD()
       sed 's/^/| /' amos_qt.$ac_ext >&AS_MESSAGE_LOG_FD()
     fi

    fi # if have_qt4
  ])dnl AC_CACHE_VAL ac_cv_qt_test_result
  AC_MSG_RESULT([$ac_cv_qt_test_result])

  AC_SUBST(QT_CXXFLAGS)
  AC_SUBST(QT_LIBS)
  AC_SUBST(QT_MOC)

  rm -f amos_qt.h moc_amos_qt.$ac_ext moc_amos_qt.o \
        amos_qt.$ac_ext amos_qt.o amos_qt \
        amos_qt.pro amos_qt.qmake amos_qt.qmake.err

  have_qt4_test=$ac_cv_qt_test_result
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
    [  --with-Boost-dir=DIR    DIR is equal to BoostDIR if you have followed the
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
