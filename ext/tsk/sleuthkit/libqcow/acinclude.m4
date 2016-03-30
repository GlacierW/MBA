dnl Function to detect if libqcow dependencies are available
AC_DEFUN([AX_LIBQCOW_CHECK_LOCAL],
 [dnl Check for internationalization functions in libqcow/libqcow_i18n.c 
 AC_CHECK_FUNCS([bindtextdomain])
 ])

