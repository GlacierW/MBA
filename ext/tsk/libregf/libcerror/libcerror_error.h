/*
 * Error functions
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

#if !defined( _LIBCERROR_INTERNAL_ERROR_H )
#define _LIBCERROR_INTERNAL_ERROR_H

#include <common.h>
#include <types.h>

#include <stdio.h>

#include "libcerror_extern.h"
#include "libcerror_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

#define LIBCERROR_MESSAGE_INCREMENT_SIZE	64
#define LIBCERROR_MESSAGE_MAXIMUM_SIZE		4096

typedef struct libcerror_internal_error libcerror_internal_error_t;

struct libcerror_internal_error
{
	/* The error domain
	 */
	int domain;

	/* The error code
	 */
	int code;

	/* The number of messages
	 */
	int number_of_messages;

	/* A dynamic array containing the message strings
	 */
	system_character_t **messages;

	/* A dynamic array containing the message string sizes
	 * without the end-of-string character
	 */
	size_t *sizes;
};

LIBCERROR_EXTERN \
void libcerror_error_free(
      libcerror_error_t **error );

LIBCERROR_EXTERN \
void libcerror_error_set(
      libcerror_error_t **error,
      int error_domain,
      int error_code,
      const char *format_string,
      ... );

LIBCERROR_EXTERN \
int libcerror_error_matches(
     libcerror_error_t *error,
     int error_domain,
     int error_code );

LIBCERROR_EXTERN \
int libcerror_error_fprint(
     libcerror_error_t *error,
     FILE *stream );

LIBCERROR_EXTERN \
int libcerror_error_sprint(
     libcerror_error_t *error,
     char *string,
     size_t size );

LIBCERROR_EXTERN \
int libcerror_error_backtrace_fprint(
     libcerror_error_t *error,
     FILE *stream );

LIBCERROR_EXTERN \
int libcerror_error_backtrace_sprint(
     libcerror_error_t *error,
     char *string,
     size_t size );

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBCERROR_INTERNAL_ERROR_H ) */

