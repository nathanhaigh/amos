##-- CUSTOM AMOS MACROS --##

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
               [test "${ac_cv_enable_$1}" = yes])
])


##-- AMOS_C_BITFIELDS
##   checks whether C compiler packs bits hi to lo or lo to hi. will set the
##   BITFIELDS_HTOL if the compiler packs hi to lo, or leave undefined if
##   compiler packs lo to hi. HTOL, that is the compiler packs its bitfields
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

if test $ac_cv_c_bitfields_htol = yes; then
  AC_DEFINE(BITFIELDS_HTOL, 1,
            [Define to 1 if C compiler packs bits hi-order to lo-order])
fi
])


##-- END OF m4 --##