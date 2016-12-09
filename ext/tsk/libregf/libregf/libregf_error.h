/*
 * Error functions
 *
 * Copyright (C) 2009-2016, Joachim Metz <joachim.metz@gmail.com>
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

#if !defined( _LIBREGF_INTERNAL_ERROR_H )
#define _LIBREGF_INTERNAL_ERROR_H

#include <common.h>
#include <file_stream.h>
#include <types.h>

#if !defined( HAVE_LOCAL_LIBREGF )
#include <libregf/error.h>
#endif

#include "libregf_extern.h"

#if defined( __cplusplus )
extern "C" {
#endif

#if !defined( HAVE_LOCAL_LIBREGF )

LIBREGF_EXTERN \
void libregf_error_free(
      libregf_error_t **error );

LIBREGF_EXTERN \
int libregf_error_fprint(
     libregf_error_t *error,
     FILE *stream );

LIBREGF_EXTERN \
int libregf_error_sprint(
     libregf_error_t *error,
     char *string,
     size_t size );

LIBREGF_EXTERN \
int libregf_error_backtrace_fprint(
     libregf_error_t *error,
     FILE *stream );

LIBREGF_EXTERN \
int libregf_error_backtrace_sprint(
     libregf_error_t *error,
     char *string,
     size_t size );

#endif /* !defined( HAVE_LOCAL_LIBREGF ) */

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBREGF_INTERNAL_ERROR_H ) */

