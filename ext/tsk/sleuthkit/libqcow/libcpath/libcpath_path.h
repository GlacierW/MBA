/*
 * Path functions
 *
 * Copyright (C) 2008-2016, Joachim Metz <joachim.metz@gmail.com>
 *
 * Refer to AUTHORS for acknowledgements.
 *
 * This software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this software.  If not, see <http://www.gnu.org/licenses/>.
 */

#if !defined( _LIBCPATH_PATH_H )
#define _LIBCPATH_PATH_H

#include <common.h>
#include <types.h>

#include "libcpath_extern.h"
#include "libcpath_libcerror.h"

#if defined( __cplusplus )
extern "C" {
#endif

#if defined( WINAPI ) && ( WINVER <= 0x0500 )
BOOL libcpath_CloseHandle(
      HANDLE file_handle );
#endif

#if defined( WINAPI ) && ( WINVER <= 0x0500 )
BOOL libcpath_SetCurrentDirectoryA(
      LPCSTR path );
#endif

LIBCPATH_EXTERN \
int libcpath_path_change_directory(
     const char *directory_name,
     libcerror_error_t **error );

#if defined( WINAPI ) && ( WINVER <= 0x0500 )
DWORD libcpath_GetCurrentDirectoryA(
       DWORD buffer_size,
       LPCSTR buffer );
#endif

LIBCPATH_EXTERN \
int libcpath_path_get_current_working_directory(
     char **current_working_directory,
     size_t *current_working_directory_size,
     libcerror_error_t **error );

LIBCPATH_EXTERN \
int libcpath_path_get_full_path(
     const char *path,
     size_t path_length,
     char **full_path,
     size_t *full_path_size,
     libcerror_error_t **error );

LIBCPATH_EXTERN \
int libcpath_path_get_sanitized_filename(
     const char *filename,
     size_t filename_size,
     char **sanitized_filename,
     size_t *sanitized_filename_size,
     libcerror_error_t **error );

LIBCPATH_EXTERN \
int libcpath_path_get_sanitized_path(
     const char *path,
     size_t path_size,
     char **sanitized_path,
     size_t *sanitized_path_size,
     libcerror_error_t **error );

LIBCPATH_EXTERN \
int libcpath_path_join(
     char **path,
     size_t *path_size,
     const char *directory_name,
     size_t directory_name_length,
     const char *filename,
     size_t filename_length,
     libcerror_error_t **error );

#if defined( WINAPI ) && ( WINVER <= 0x0500 )
BOOL libcpath_CreateDirectoryA(
      LPCSTR path,
      SECURITY_ATTRIBUTES *security_attributes );
#endif

LIBCPATH_EXTERN \
int libcpath_path_make_directory(
     const char *directory_name,
     libcerror_error_t **error );

/* TODO: deprecated remove */
LIBCPATH_EXTERN \
int libcpath_path_sanitize(
     char *path,
     size_t *path_size,
     libcerror_error_t **error );

/* TODO: deprecated remove */
LIBCPATH_EXTERN \
int libcpath_path_sanitize_filename(
     char *filename,
     size_t *filename_size,
     libcerror_error_t **error );

#if defined( HAVE_WIDE_CHARACTER_TYPE )

#if defined( WINAPI ) && ( WINVER <= 0x0500 )
BOOL libcpath_SetCurrentDirectoryW(
      LPCWSTR path );
#endif

LIBCPATH_EXTERN \
int libcpath_path_change_directory_wide(
     const wchar_t *directory_name,
     libcerror_error_t **error );

#if defined( WINAPI ) && ( WINVER <= 0x0500 )
DWORD libcpath_GetCurrentDirectoryW(
       DWORD buffer_size,
       LPCWSTR buffer );
#endif

LIBCPATH_EXTERN \
int libcpath_path_get_current_working_directory_wide(
     wchar_t **current_working_directory,
     size_t *current_working_directory_size,
     libcerror_error_t **error );

LIBCPATH_EXTERN \
int libcpath_path_get_full_path_wide(
     const wchar_t *path,
     size_t path_length,
     wchar_t **full_path,
     size_t *full_path_size,
     libcerror_error_t **error );

LIBCPATH_EXTERN \
int libcpath_path_get_sanitized_filename_wide(
     const wchar_t *filename,
     size_t filename_size,
     wchar_t **sanitized_filename,
     size_t *sanitized_filename_size,
     libcerror_error_t **error );

LIBCPATH_EXTERN \
int libcpath_path_get_sanitized_path_wide(
     const wchar_t *path,
     size_t path_size,
     wchar_t **sanitized_path,
     size_t *sanitized_path_size,
     libcerror_error_t **error );

LIBCPATH_EXTERN \
int libcpath_path_join_wide(
     wchar_t **path,
     size_t *path_size,
     const wchar_t *directory_name,
     size_t directory_name_length,
     const wchar_t *filename,
     size_t filename_length,
     libcerror_error_t **error );

#if defined( WINAPI ) && ( WINVER <= 0x0500 )
BOOL libcpath_CreateDirectoryW(
      LPCWSTR path,
      SECURITY_ATTRIBUTES *security_attributes );
#endif

LIBCPATH_EXTERN \
int libcpath_path_make_directory_wide(
     const wchar_t *directory_name,
     libcerror_error_t **error );

/* TODO: deprecated remove */
LIBCPATH_EXTERN \
int libcpath_path_sanitize_wide(
     wchar_t *path,
     size_t *path_size,
     libcerror_error_t **error );

/* TODO: deprecated remove */
LIBCPATH_EXTERN \
int libcpath_path_sanitize_filename_wide(
     wchar_t *filename,
     size_t *filename_size,
     libcerror_error_t **error );

#endif /* defined( HAVE_WIDE_CHARACTER_TYPE ) */

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBCPATH_PATH_H ) */

