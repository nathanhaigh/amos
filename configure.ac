##-- AMOS CONFIGURE --##

# initialize
AC_PREREQ(2.59)
AC_INIT([AMOS Assembler],
        [1.2.7],
        [amos@tigr.org],
        [amos])
AC_PREFIX_DEFAULT([`pwd`])
AC_CONFIG_SRCDIR([src/AMOS/foundation_AMOS.hh])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_LIBOBJ_DIR([src/GNU])
AM_INIT_AUTOMAKE(1.8)

# declare some precious variables
AC_ARG_VAR(PERL, [perl command])
AC_ARG_VAR(NUCMER, [nucmer command])
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
if test "${PERL}" = ":" ; then
   AC_MSG_ERROR([perl required to run ${PACKAGE_NAME} scripts])
fi
AC_PATH_PROG(NUCMER, [nucmer], [:])

# check for libraries
AC_CHECK_LIB(m, pow)
AC_CHECK_LIB(c, fopen)

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
AC_STRUCT_TM
AC_C_BIGENDIAN
AC_SYS_LARGEFILE
AMOS_C_BITFIELDS

# check for functions which can be replaced with libGNU modules:
#   getopt strtod strtol
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
AMOS_BUILD_CONDITIONAL([all], [yes])

# makefiles to configure
AC_CONFIG_FILES(
   [ \
   Makefile \
   doc/Makefile \
   src/Makefile \
   src/GNU/Makefile \
   src/AMOS/Makefile \
   src/Common/Makefile \
   src/Graph/Makefile \
   src/Slice/Makefile \
   src/CelMsg/Makefile \
   src/Message/Makefile \
   src/Bank/Makefile \
   src/Align/Makefile \
   src/Tigger/Makefile \
   src/Compare/Makefile \
   src/Bundler/Makefile \
   src/Casm/Makefile \
   src/CtgTrim/Makefile \
   src/Sim/Makefile \
   src/Validation/Makefile \
   src/Converters/Makefile \
   src/Utils/Makefile \
   src/PerlModules/Makefile \
   src/Pipeline/Makefile \
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

if test "${GCC}" = "yes" ; then
   gcc_version=`${CC} --version | head -n 1`
   echo "  GCC version:         ${gcc_version}"
else
   gcc_version=''
fi

echo \
"  Host System type:    ${host}
  Install prefix:      ${prefix}

  See config.h for further configuration information.
  Email <${PACKAGE_BUGREPORT}> with questions and bug reports.
"

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
	echo "WARNING! TIGR suns using GCC 2.x.x will bomb on largefile support"
	echo "   use \`configure --disable-largefile' to avoid"
          ;;
     esac
     ;;
esac

case "${gcc_version}" in
     *2.96.*)
   echo "WARNING! GCC ${gcc_version} has bugs that affect AMOS"
   echo "   please install an newer version of GCC and reconfigure"
esac

if test "${NUCMER}" = ":"  ||  ! test -x "${NUCMER}"; then
   echo "WARNING! nucmer was not found, but is required to run AMOScmp"
   echo "   install nucmer if planning on using AMOScmp"
fi


##-- END OF CONFIGURE --##