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

#if !defined( _LIBFDATETIME_INTERNAL_ERROR_H )
#define _LIBFDATETIME_INTERNAL_ERROR_H

#include <common.h>
#include <file_stream.h>
#include <types.h>

#if !defined( HAVE_LOCAL_LIBFDATETIME )
#include <libfdatetime/error.h>
#endif

#include "libfdatetime_extern.h"

#if defined( __cplusplus )
extern "C" {
#endif

#if !defined( HAVE_LOCAL_LIBFDATETIME )

LIBFDATETIME_EXTERN \
void libfdatetime_error_free(
      libfdatetime_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_error_fprint(
     libfdatetime_error_t *error,
     FILE *stream );

LIBFDATETIME_EXTERN \
int libfdatetime_error_sprint(
     libfdatetime_error_t *error,
     char *string,
     size_t size );

LIBFDATETIME_EXTERN \
int libfdatetime_error_backtrace_fprint(
     libfdatetime_error_t *error,
     FILE *stream );

LIBFDATETIME_EXTERN \
int libfdatetime_error_backtrace_sprint(
     libfdatetime_error_t *error,
     char *string,
     size_t size );

#endif /* !defined( HAVE_LOCAL_LIBFDATETIME ) */

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBFDATETIME_INTERNAL_ERROR_H ) */

