##-- AMOS CONFIGURE --##

# initialize
AC_PREREQ([2.59])
AC_INIT([AMOS Assembler],[3.1.0],[amos-help@lists.sourceforge.net],[amos])
AC_PREFIX_DEFAULT([`pwd`])
AC_CONFIG_SRCDIR([src/AMOS/foundation_AMOS.hh])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_LIBOBJ_DIR([src/GNU])
AM_INIT_AUTOMAKE(1.8)

# allow patters relating to CA
m4_pattern_allow([^AS_UTL])
m4_pattern_allow([^AS_MSG])

# declare some precious variables
AC_ARG_VAR(PERL, [perl command])
AC_ARG_VAR(PYTHON, [python command])
AC_ARG_VAR(NUCMER, [nucmer command])
AC_ARG_VAR(DELTAFILTER, [delta-filter command])
AC_ARG_VAR(SHOWCOORDS, [show-coords command])
AC_ARG_VAR(BLAT, [blat command])
AC_SUBST(docdir, [${datadir}/doc/${PACKAGE_TARNAME}-${PACKAGE_VERSION}])
AC_SUBST(amoslibdir, [${libdir}/AMOS])
AC_SUBST(tigrlibdir, [${libdir}/TIGR])
AC_SUBST(amosincludedir, [${includedir}/AMOS])
AC_SUBST(tigrincludedir, [${includedir}/TIGR])

# check for programs
AC_PROG_CXX
AC_PROG_CC
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_RANLIB
AC_PROG_CPP
AC_PATH_PROG(PERL, [perl], [:])
AC_PATH_PROG(PYTHON, [python], [:])
AC_PATH_PROG(NUCMER, [nucmer], [:])
AC_PATH_PROG(DELTAFILTER, [delta-filter], [:])
AC_PATH_PROG(SHOWCOORDS, [show-coords], [:])
AC_PATH_PROG(BLAT, [blat], [:])

# initialize Python
# AC_PROG_PYTHON()

# check for Perl modules
AC_PROG_PERL_MODULES(Statistics::Descriptive, LIBSTATISTICS_DESCRIPTIVE_PERL=1, LIBSTATISTICS_DESCRIPTIVE_PERL=0)
AC_PROG_PERL_MODULES(XML::Parser, LIBXML_PARSER_PERL=1, LIBXML_PARSER_PERL=0)
AC_PROG_PERL_MODULES(DBI, LIBDBI_PERL=1, LIBDBI_PERL=0)

# check for libraries
AC_CHECK_LIB(m, pow)
AC_CHECK_LIB(c, fopen)
AC_CHECK_LIB(z, main)
case ${LIBS} in
  *"-lz "*)
  have_libz=yes
  ;;
esac

# check for headers
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS(
   [ \
   fcntl.h \
   inttypes.h \
   limits.h \
   stdlib.h \
   unistd.h \
   ],
   [],
   AC_MSG_ERROR([required header not found please use a supported compiler])
)

# check for typedefs, structures, and compiler characteristics
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_MODE_T
AC_STRUCT_TM
AC_C_BIGENDIAN
AC_SYS_LARGEFILE
AMOS_C_BITFIELDS
AMOS_QT4
AMOS_BOOST
AMOS_CA

# check for functions which can be replaced with libGNU modules:
# getopt strtod strtol
gl_GETOPT
gl_FUNC_STRTOD
gl_FUNC_STRTOL

# check for functions not in libGNU
AC_FUNC_FORK
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_STRFTIME
AC_CHECK_FUNCS(
   [ \
   dup2 \
   floor \
   localtime \
   lstat \
   memmove \
   memset \
   mkdir \
   pow \
   rmdir \
   sqrt \
   stat \
   strchr \
   strdup \
   strstr \
   ],
   [],
   AC_MSG_ERROR([required function not found please use a supported compiler])
)

# check the platform
AC_CANONICAL_HOST

# set the automake build conditionals, e.g. BUILD_AMOScmp
AMOS_BUILD_CONDITIONAL([AMOScmp], [no])
AMOS_BUILD_CONDITIONAL([minimus], [no])
AMOS_BUILD_CONDITIONAL([Simulator], [no])
AMOS_BUILD_CONDITIONAL([Experimental], [no])
AMOS_BUILD_CONDITIONAL([all], [yes])

AM_CONDITIONAL(BUILD_LIBZ, [test x"${have_libz}" = x"yes"])
AM_CONDITIONAL(BUILD_QT4, [test x"${have_qt4_test}" = x"yes"])
AM_CONDITIONAL(BUILD_BOOST, [test x"${have_boost_test}" = x"yes"])

# makefiles to configure
AC_CONFIG_FILES(
   [ \
   Makefile \
   doc/Makefile \
   src/Makefile \
   src/GNU/Makefile \
   src/AMOS/Makefile \
   src/Bambus/Makefile \
   src/Bambus/Bundler/Makefile \
   src/Bambus/Output/Makefile \
   src/Bambus/Untangler/Makefile \
   src/Common/Makefile \
   src/Graph/Makefile \
   src/Slice/Makefile \
   src/CelMsg/Makefile \
   src/Message/Makefile \
   src/Bank/Makefile \
   src/Align/Makefile \
   src/Tigger/Makefile \
   src/Compare/Makefile \
   src/Casm/Makefile \
   src/CtgTrim/Makefile \
   src/Sim/Makefile \
   src/Validation/Makefile \
   src/Converters/Makefile \
   src/Utils/Makefile \
   src/PerlModules/Makefile \
   src/PythonModules/Makefile \
   src/Pipeline/Makefile \
   src/Staden/Makefile \
   src/Staden/read/Makefile \
   src/Staden/progs/Makefile \
   src/Contig/Makefile \
   src/Foundation/Makefile \
   src/hawkeye/Makefile \
   src/Experimental/Makefile \
   ]
)

# make it happen
AC_OUTPUT

# dump some configuration confirmations
echo \
"
-- ${PACKAGE_STRING} Configuration Results --
  C compiler:          ${CC} ${CFLAGS}
  C++ compiler:        ${CXX} ${CXXFLAGS}"

if test x"${GCC}" = x"yes" ; then
   gcc_version=`${CC} --version | head -n 1`
   echo "  GCC version:         ${gcc_version}"
else
   gcc_version=''
fi

echo \
"  Host System type:    ${host}
  Install prefix:      ${prefix}
  Install eprefix:     ${exec_prefix}

  See config.h for further configuration information.
  Email <${PACKAGE_BUGREPORT}> with questions and bug reports.
"

# alert the user of possible problems
case "${host}" in
  *alpha*osf*)
    case "${CFLAGS} ${CXXFLAGS}" in
      *-g*)
        echo "WARNING! TIGR alphas may bomb when -g flag is turned on"
        echo "   use \`configure CFLAGS= CXXFLAGS=' to avoid"
      ;;
    esac
    ;;

  *sparc*solaris*)
    case "${gcc_version}" in
      *2.*.*)
        if test x"${enable_largefile}" != x"no"; then
        echo "WARNING! TIGR suns using GCC 2.x.x will bomb on largefile support"
        echo "   use \`configure --disable-largefile' to avoid"
        fi
      ;;
    esac
    ;;

  *x86_64*)
    case "${gcc_version}" in
      *3.3.*)
        echo "WARNING! GCC ${gcc_version} has bugs that affect x86_64 AMOS"
        echo "   please install a newer version of GCC and reconfigure"
        ;;
    esac
    ;;
esac

case "${gcc_version}" in
  *2.96.*)
    echo "WARNING! GCC ${gcc_version} has bugs that affect AMOS"
    echo "   please install a newer version of GCC and reconfigure"
  ;;
esac

if test x"${PERL}" = x":"  ||  ! test -x "${PERL}"; then
  echo "WARNING! perl was not found but is required to run AMOS scripts"
  echo "   install perl if planning on using AMOS scripts"
fi

if test x"${PYTHON}" = x":"  ||  ! test -x "${PYTHON}"; then
  echo "WARNING! python was not found but is required to run goBambus2"
  echo "   install python if planning on using goBambus2"
fi

if test x"${NUCMER}" = x":"  ||  ! test -x "${NUCMER}"; then
  echo "WARNING! nucmer was not found but is required to run AMOScmp and minimus2"
  echo "   install nucmer if planning on using these programs"
fi

if test x"${DELTAFILTER}" = x":"  ||  ! test -x "${DELTAFILTER}"; then
  echo "WARNING! delta-filter was not found but is required to run AMOScmp-shortReads-alignmentTrimmed"
  echo "   install delta-filter if planning on using AMOScmp-shortReads-alignmentTrimmed"
fi

if test x"${SHOWCOORDS}" = x":"  ||  ! test -x "${DELTAFILTER}"; then
  echo "WARNING! show-coords was not found but is required to run minimus2"
  echo "   install show-coords if planning on using minimus2"
fi

if test x"${BLAT}" = x":"  ||  ! test -x "${BLAT}"; then
  echo "WARNING! BLAT was not found but is required to run minimus2-blat"
  echo "   install BLAT if planning on using minimus2-blat"
fi

if test x"${have_qt4}" != x"yes"; then
  echo "WARNING! Qt4 toolkit was not found but is required to run AMOS GUIs"
  echo "   install Qt4 or locate Qt4 with configure to build GUIs"
  echo "   see config.log for more information on what went wrong"
elif test x"${have_qt4_test}" != x"yes"; then
  echo "WARNING! Qt4 toolkit malfunctioning but is required to run AMOS GUIs"
  echo "   try compiling GUIs manually or reinstall Qt4 toolkit to build GUIs"
  echo "   see config.log for more information on what went wrong"
fi

if test x"${have_boost}" != x"yes"; then
  echo "WARNING! Boost graph toolkit was not found but is required to run parts of the AMOS Scaffolder (Bambus 2)"
  echo "   install Boost or locate Boost with configure to build Scaffolder"
  echo "   see config.log for more information on what went wrong"
elif test x"${have_boost_test}" != x"yes"; then
  echo "WARNING! Boost graph toolkit malfunctioning but is required to run parts of the AMOS Scaffolder (Bambus 2)"
  echo "   try compiling manually or reinstall Boost toolkit to build AMOS Scaffolder"
  echo "   see config.log for more information on what went wrong"
fi

if test x"${have_libz}" != x"yes"; then
  echo "WARNING! libz was not found but is required for certain read trace IO"
  echo "   install libz if planning on using compressed trace formats"
fi

if test x"${LIBSTATISTICS_DESCRIPTIVE_PERL}" = x"0"; then
  echo "WARNING! Statistics::Descriptive Perl module was not found but is required to run some AMOS scripts"
fi

if test x"${LIBXML_PARSER_PERL}" = x"0"; then
  echo "WARNING! XML::Parser Perl module was not found but is required to run some AMOS scripts"
fi

if test x"${LIBDBI_PERL}" = x"0"; then
  echo "WARNING! DBI Perl module was not found but is required to run some AMOS scripts"
fi
##-- END OF CONFIGURE --##
