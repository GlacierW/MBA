/*
 * Error functions
 *
 * Copyright (C) 2010-2016, Joachim Metz <joachim.metz@gmail.com>
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

#if !defined( _LIBFDATA_INTERNAL_ERROR_H )
#define _LIBFDATA_INTERNAL_ERROR_H

#include <common.h>
#include <file_stream.h>
#include <types.h>

#if !defined( HAVE_LOCAL_LIBFDATA )
#include <libfdata/error.h>
#endif

#include "libfdata_extern.h"

#if defined( __cplusplus )
extern "C" {
#endif

#if !defined( HAVE_LOCAL_LIBFDATA )

LIBFDATA_EXTERN \
void libfdata_error_free(
      libfdata_error_t **error );

LIBFDATA_EXTERN \
int libfdata_error_fprint(
     libfdata_error_t *error,
     FILE *stream );

LIBFDATA_EXTERN \
int libfdata_error_sprint(
     libfdata_error_t *error,
     char *string,
     size_t size );

LIBFDATA_EXTERN \
int libfdata_error_backtrace_fprint(
     libfdata_error_t *error,
     FILE *stream );

LIBFDATA_EXTERN \
int libfdata_error_backtrace_sprint(
     libfdata_error_t *error,
     char *string,
     size_t size );

#endif /* !defined( HAVE_LOCAL_LIBFDATA ) */

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBFDATA_INTERNAL_ERROR_H ) */

