dnl Function to detect if libregf dependencies are available
AC_DEFUN([AX_LIBREGF_CHECK_LOCAL],
 [dnl Headers included in libregf/libregf_file.h, libregf/libregf_key.h
 dnl libregf/libregf_key_item_values.h and libregf/libregf_value_item_values.h
 AC_CHECK_HEADERS([wctype.h])

 dnl Functions used in libregf/libregf_file.h, libregf/libregf_key.h
 dnl libregf/libregf_key_item_values.h and libregf/libregf_value_item_values.h
 AC_CHECK_FUNCS([towupper])

 AS_IF(
  [test "x$ac_cv_func_towupper" != xyes],
  [AC_MSG_FAILURE(
   [Missing function: towupper],
   [1])
  ])

 dnl Check for internationalization functions in libregf/libregf_i18n.c 
 AC_CHECK_FUNCS([bindtextdomain])
 ])

