# initialize
AC_PREREQ(2.59)
AC_INIT([AMOS Assembler],
        [1.2.3],
        [amos@tigr.org],
        [amos])
AC_PREFIX_DEFAULT([`pwd`])
AC_CONFIG_SRCDIR([src/AMOS/foundation_AMOS.hh])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE(1.8)

# declare some precious variables
AC_ARG_VAR(PERL, [perl command])
AC_ARG_VAR(NUCMER, [nucmer command])
AC_SUBST(docdir, [${datadir}/doc/${PACKAGE_TARNAME}-${PACKAGE_VERSION}])
AC_SUBST(amoslibdir, [${libdir}/AMOS])
AC_SUBST(tigrlibdir, [${libdir}/TIGR])
AC_SUBST(amosincludedir, [${includedir}/AMOS])
AC_SUBST(tigrincludedir, [${includedir}/TIGR])

# set automake conditionals
#AM_CONDITIONAL([COND_AMOSCMP], [test "${enable_AMOScmp}" = "yes"])

# check for programs
AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PATH_PROG(PERL, [perl], [:])
if test "${PERL}" = ":" ; then
   AC_MSG_ERROR([perl required to run ${PACKAGE_NAME} scripts])
fi
AC_PATH_PROG(NUCMER, [nucmer], [:])

# check for libraries
AC_CHECK_LIB(m, pow)

# check for headers
AC_HEADER_STDC
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
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_C_BIGENDIAN
AC_SYS_LARGEFILE
AC_SYS_LONG_FILE_NAMES

# check for library functions
AC_CHECK_FUNCS(
   [ \
   floor \
   pow \
   sqrt \
   strtod \
   memmove \
   memset \
   mkdir \
   rmdir \
   strchr \
   strdup \
   strstr \
   strtol \
   ],
   [],
   AC_MSG_ERROR([required function not found please use a supported compiler])
)

# check the platform
AC_CANONICAL_HOST

# makefiles to configure
AC_CONFIG_FILES(
   [ \
   Makefile \
   doc/Makefile \
   src/Makefile \
   src/AMOS/Makefile \
   src/Common/Makefile \
   src/Graph/Makefile \
   src/Slice/Makefile \
   src/CelMsg/Makefile \
   src/Message/Makefile \
   src/Bank/Makefile \
   src/Align/Makefile \
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
fi

echo \
"  Host System type:    ${host}
  Install prefix:      ${prefix}

  See config.h for further configuration information.
  Email <${PACKAGE_BUGREPORT}> with questions and bug reports.
"

case "${gcc_version}" in
     *2.7.* | *3.2.*)
     echo "warning! your GCC version has bugs that may affect ${PACKAGE_NAME}"
     ;;
     *)
     ;;
esac

if test "${NUCMER}" = ":" ; then
   echo "warning! nucmer was not found, but is required to run AMOScmp"
fi


##-- END OF CONFIGURE --##