dnl Available from the GNU Autoconf Macro Archive at:
dnl http://www.gnu.org/software/ac-archive/htmldoc/ac_prog_perl_modules.html
dnl
AC_DEFUN([AC_PROG_PYTHON],[dnl
# Make sure we have python
if test -z "$PYTHON"; then
  AC_CHECK_PROG(PYTHON,python,python)
else
  AC_MSG_WARN(could not find python)
fi])dnl
