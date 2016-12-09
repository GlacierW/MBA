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

#if !defined( _LIBFCACHE_INTERNAL_ERROR_H )
#define _LIBFCACHE_INTERNAL_ERROR_H

#include <common.h>
#include <file_stream.h>
#include <types.h>

#if !defined( HAVE_LOCAL_LIBFCACHE )
#include <libfcache/error.h>
#endif

#include "libfcache_extern.h"

#if defined( __cplusplus )
extern "C" {
#endif

#if !defined( HAVE_LOCAL_LIBFCACHE )

LIBFCACHE_EXTERN \
void libfcache_error_free(
      libfcache_error_t **error );

LIBFCACHE_EXTERN \
int libfcache_error_fprint(
     libfcache_error_t *error,
     FILE *stream );

LIBFCACHE_EXTERN \
int libfcache_error_sprint(
     libfcache_error_t *error,
     char *string,
     size_t size );

LIBFCACHE_EXTERN \
int libfcache_error_backtrace_fprint(
     libfcache_error_t *error,
     FILE *stream );

LIBFCACHE_EXTERN \
int libfcache_error_backtrace_sprint(
     libfcache_error_t *error,
     char *string,
     size_t size );

#endif /* !defined( HAVE_LOCAL_LIBFCACHE ) */

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBFCACHE_INTERNAL_ERROR_H ) */

