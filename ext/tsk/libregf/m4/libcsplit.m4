dnl Functions for libcsplit
dnl
dnl Version: 20130406

dnl Function to detect if libcsplit is available
dnl ac_libcsplit_dummy is used to prevent AC_CHECK_LIB adding unnecessary -l<library> arguments
AC_DEFUN([AX_LIBCSPLIT_CHECK_LIB],
 [dnl Check if parameters were provided
 AS_IF(
  [test "x$ac_cv_with_libcsplit" != x && test "x$ac_cv_with_libcsplit" != xno && test "x$ac_cv_with_libcsplit" != xauto-detect],
  [AS_IF(
   [test -d "$ac_cv_with_libcsplit"],
   [CFLAGS="$CFLAGS -I${ac_cv_with_libcsplit}/include"
   LDFLAGS="$LDFLAGS -L${ac_cv_with_libcsplit}/lib"],
   [AC_MSG_WARN([no such directory: $ac_cv_with_libcsplit])
   ])
  ])

 AS_IF(
  [test "x$ac_cv_with_libcsplit" = xno],
  [ac_cv_libcsplit=no],
  [dnl Check for a pkg-config file
  AS_IF(
   [test "x$cross_compiling" != "xyes" && test "x$PKGCONFIG" != "x"],
   [PKG_CHECK_MODULES(
    [libcsplit],
    [libcsplit >= 20120701],
    [ac_cv_libcsplit=yes],
    [ac_cv_libcsplit=no])
   ])

  AS_IF(
   [test "x$ac_cv_libcsplit" = xyes],
   [ac_cv_libcsplit_CPPFLAGS="$pkg_cv_libcsplit_CFLAGS"
   ac_cv_libcsplit_LIBADD="$pkg_cv_libcsplit_LIBS"],
   [dnl Check for headers
   AC_CHECK_HEADERS([libcsplit.h])

   AS_IF(
    [test "x$ac_cv_header_libcsplit_h" = xno],
    [ac_cv_libcsplit=no],
    [dnl Check for the individual functions
    ac_cv_libcsplit=yes

    AC_CHECK_LIB(
     csplit,
     libcsplit_get_version,
     [ac_cv_libcsplit_dummy=yes],
     [ac_cv_libcsplit=no])

    dnl Narrow string functions
    AC_CHECK_LIB(
     csplit,
     libcsplit_narrow_string_split,
     [ac_cv_libcsplit_dummy=yes],
     [ac_cv_libcsplit=no])

    dnl Narrow split string functions
    AC_CHECK_LIB(
     csplit,
     libcsplit_narrow_split_string_free,
     [ac_cv_libcsplit_dummy=yes],
     [ac_cv_libcsplit=no])
    AC_CHECK_LIB(
     csplit,
     libcsplit_narrow_split_string_get_string,
     [ac_cv_libcsplit_dummy=yes],
     [ac_cv_libcsplit=no])
    AC_CHECK_LIB(
     csplit,
     libcsplit_narrow_split_string_get_number_of_segments,
     [ac_cv_libcsplit_dummy=yes],
     [ac_cv_libcsplit=no])
    AC_CHECK_LIB(
     csplit,
     libcsplit_narrow_split_string_get_segment_by_index,
     [ac_cv_libcsplit_dummy=yes],
     [ac_cv_libcsplit=no])
    AC_CHECK_LIB(
     csplit,
     libcsplit_narrow_split_string_set_segment_by_index,
     [ac_cv_libcsplit_dummy=yes],
     [ac_cv_libcsplit=no])

    dnl Wide string functions
    AS_IF(
     [test "x$ac_cv_enable_wide_character_type" != xno],
     [AC_CHECK_LIB(
     csplit,
      libcsplit_wide_string_split,
      [ac_cv_libcsplit_dummy=yes],
      [ac_cv_libcsplit=no])

    dnl Wide split string functions
     AC_CHECK_LIB(
      csplit,
      libcsplit_wide_split_string_free,
      [ac_cv_libcsplit_dummy=yes],
      [ac_cv_libcsplit=no])
     AC_CHECK_LIB(
      csplit,
      libcsplit_wide_split_string_get_string,
      [ac_cv_libcsplit_dummy=yes],
      [ac_cv_libcsplit=no])
     AC_CHECK_LIB(
      csplit,
      libcsplit_wide_split_string_get_number_of_segments,
      [ac_cv_libcsplit_dummy=yes],
      [ac_cv_libcsplit=no])
     AC_CHECK_LIB(
      csplit,
      libcsplit_wide_split_string_get_segment_by_index,
      [ac_cv_libcsplit_dummy=yes],
      [ac_cv_libcsplit=no])
     AC_CHECK_LIB(
      csplit,
      libcsplit_wide_split_string_set_segment_by_index,
      [ac_cv_libcsplit_dummy=yes],
      [ac_cv_libcsplit=no])
     ])

    ac_cv_libcsplit_LIBADD="-lcsplit"
    ])
   ])
  ])

 AS_IF(
  [test "x$ac_cv_libcsplit" = xyes],
  [AC_DEFINE(
   [HAVE_LIBCSPLIT],
   [1],
   [Define to 1 if you have the `csplit' library (-lcsplit).])
  ])

 AS_IF(
  [test "x$ac_cv_libcsplit" = xyes],
  [AC_SUBST(
   [HAVE_LIBCSPLIT],
   [1]) ],
  [AC_SUBST(
   [HAVE_LIBCSPLIT],
   [0])
  ])
 ])

dnl Function to detect how to enable libcsplit
AC_DEFUN([AX_LIBCSPLIT_CHECK_ENABLE],
 [AX_COMMON_ARG_WITH(
  [libcsplit],
  [libcsplit],
  [search for libcsplit in includedir and libdir or in the specified DIR, or no if to use local version],
  [auto-detect],
  [DIR])

 dnl Check for a shared library version
 AX_LIBCSPLIT_CHECK_LIB

 dnl Check if the dependencies for the local library version
 AS_IF(
  [test "x$ac_cv_libcsplit" != xyes],
  [AC_DEFINE(
   [HAVE_LOCAL_LIBCSPLIT],
   [1],
   [Define to 1 if the local version of libcsplit is used.])
  AC_SUBST(
   [HAVE_LOCAL_LIBCSPLIT],
   [1])

  ac_cv_libcsplit_CPPFLAGS="-I../libcsplit";
  ac_cv_libcsplit_LIBADD="../libcsplit/libcsplit.la";

  ac_cv_libcsplit=local
  ])

 AM_CONDITIONAL(
  [HAVE_LOCAL_LIBCSPLIT],
  [test "x$ac_cv_libcsplit" = xlocal])
 AS_IF(
  [test "x$ac_cv_libcsplit_CPPFLAGS" != "x"],
  [AC_SUBST(
   [LIBCSPLIT_CPPFLAGS],
   [$ac_cv_libcsplit_CPPFLAGS])
  ])
 AS_IF(
  [test "x$ac_cv_libcsplit_LIBADD" != "x"],
  [AC_SUBST(
   [LIBCSPLIT_LIBADD],
   [$ac_cv_libcsplit_LIBADD])
  ])

 AS_IF(
  [test "x$ac_cv_libcsplit" = xyes],
  [AC_SUBST(
   [ax_libcsplit_pc_libs_private],
   [-lcsplit])
  ])

 AS_IF(
  [test "x$ac_cv_libcsplit" = xyes],
  [AC_SUBST(
   [ax_libcsplit_spec_requires],
   [libcsplit])
  AC_SUBST(
   [ax_libcsplit_spec_build_requires],
   [libcsplit-devel])
  ])
 ])

