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

#if !defined( _LIBQCOW_INTERNAL_ERROR_H )
#define _LIBQCOW_INTERNAL_ERROR_H

#include <common.h>
#include <file_stream.h>
#include <types.h>

#if !defined( HAVE_LOCAL_LIBQCOW )
#include <libqcow/error.h>
#endif

#include "libqcow_extern.h"

#if defined( __cplusplus )
extern "C" {
#endif

#if !defined( HAVE_LOCAL_LIBQCOW )

LIBQCOW_EXTERN \
void libqcow_error_free(
      libqcow_error_t **error );

LIBQCOW_EXTERN \
int libqcow_error_fprint(
     libqcow_error_t *error,
     FILE *stream );

LIBQCOW_EXTERN \
int libqcow_error_sprint(
     libqcow_error_t *error,
     char *string,
     size_t size );

LIBQCOW_EXTERN \
int libqcow_error_backtrace_fprint(
     libqcow_error_t *error,
     FILE *stream );

LIBQCOW_EXTERN \
int libqcow_error_backtrace_sprint(
     libqcow_error_t *error,
     char *string,
     size_t size );

#endif /* !defined( HAVE_LOCAL_LIBQCOW ) */

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBQCOW_INTERNAL_ERROR_H ) */

