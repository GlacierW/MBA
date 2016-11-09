/*
 * Support functions
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

#if !defined( _LIBCFILE_SUPPORT_H )
#define _LIBCFILE_SUPPORT_H

#include <common.h>
#include <types.h>

#include "libcfile_extern.h"
#include "libcfile_libcerror.h"

#if defined( __cplusplus )
extern "C" {
#endif

#if !defined( HAVE_LOCAL_LIBCFILE )

LIBCFILE_EXTERN \
const char *libcfile_get_version(
             void );

LIBCFILE_EXTERN \
int libcfile_get_codepage(
     int *codepage,
     libcerror_error_t **error );

LIBCFILE_EXTERN \
int libcfile_set_codepage(
     int codepage,
     libcerror_error_t **error );

#endif /* !defined( HAVE_LOCAL_LIBCFILE ) */

#if defined( WINAPI ) && ( WINVER <= 0x0500 )

DWORD libcfile_GetFileAttributesA(
        LPCSTR filename );

#endif /* defined( WINAPI ) && ( WINVER <= 0x0500 ) */

LIBCFILE_EXTERN \
int libcfile_file_exists(
     const char *filename,
     libcerror_error_t **error );

#if defined( HAVE_WIDE_CHARACTER_TYPE )

#if defined( WINAPI ) && ( WINVER <= 0x0500 )

DWORD libcfile_GetFileAttributesW(
       LPCWSTR filename );

#endif /* defined( WINAPI ) && ( WINVER <= 0x0500 ) */

LIBCFILE_EXTERN \
int libcfile_file_exists_wide(
     const wchar_t *filename,
     libcerror_error_t **error );

#endif /* defined( HAVE_WIDE_CHARACTER_TYPE ) */

#if defined( WINAPI ) && ( WINVER <= 0x0500 )

BOOL libcfile_DeleteFileA(
      LPCSTR filename );

#endif /* defined( WINAPI ) && ( WINVER <= 0x0500 ) */

LIBCFILE_EXTERN \
int libcfile_file_remove(
     const char *filename,
     libcerror_error_t **error );

LIBCFILE_EXTERN \
int libcfile_file_remove_with_error_code(
     const char *filename,
     uint32_t *error_code,
     libcerror_error_t **error );

#if defined( HAVE_WIDE_CHARACTER_TYPE )

#if defined( WINAPI ) && ( WINVER <= 0x0500 )

BOOL libcfile_DeleteFileW(
      LPCWSTR filename );

#endif /* defined( WINAPI ) && ( WINVER <= 0x0500 ) */

LIBCFILE_EXTERN \
int libcfile_file_remove_wide(
     const wchar_t *filename,
     libcerror_error_t **error );

LIBCFILE_EXTERN \
int libcfile_file_remove_wide_with_error_code(
     const wchar_t *filename,
     uint32_t *error_code,
     libcerror_error_t **error );

#endif /* defined( HAVE_WIDE_CHARACTER_TYPE ) */

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBCFILE_SUPPORT_H ) */

