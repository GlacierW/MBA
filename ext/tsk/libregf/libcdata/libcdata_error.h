/*
 * Error functions
 *
 * Copyright (C) 2006-2016, Joachim Metz <joachim.metz@gmail.com>
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

#if !defined( _LIBCDATA_INTERNAL_ERROR_H )
#define _LIBCDATA_INTERNAL_ERROR_H

#include <common.h>
#include <file_stream.h>
#include <types.h>

#if !defined( HAVE_LOCAL_LIBCDATA )
#include <libcdata/error.h>
#endif

#include "libcdata_extern.h"

#if defined( __cplusplus )
extern "C" {
#endif

#if !defined( HAVE_LOCAL_LIBCDATA )

LIBCDATA_EXTERN \
void libcdata_error_free(
      libcdata_error_t **error );

LIBCDATA_EXTERN \
int libcdata_error_fprint(
     libcdata_error_t *error,
     FILE *stream );

LIBCDATA_EXTERN \
int libcdata_error_sprint(
     libcdata_error_t *error,
     char *string,
     size_t size );

LIBCDATA_EXTERN \
int libcdata_error_backtrace_fprint(
     libcdata_error_t *error,
     FILE *stream );

LIBCDATA_EXTERN \
int libcdata_error_backtrace_sprint(
     libcdata_error_t *error,
     char *string,
     size_t size );

#endif /* !defined( HAVE_LOCAL_LIBCDATA ) */

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBCDATA_INTERNAL_ERROR_H ) */

