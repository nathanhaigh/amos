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


##-- END OF m4 --##