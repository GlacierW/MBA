/*
 * FILETIME functions
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

#if !defined( _LIBFDATETIME_INTERNAL_FILETIME_H )
#define _LIBFDATETIME_INTERNAL_FILETIME_H

#include <common.h>
#include <types.h>

#include "libfdatetime_extern.h"
#include "libfdatetime_libcerror.h"
#include "libfdatetime_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libfdatetime_internal_filetime libfdatetime_internal_filetime_t;

/* Contains a 64-bit value representing the number of 100-nanosecond intervals since January 1, 1601 (UTC)
 * The upper and lower part are separated due to byte alignment
 */
struct libfdatetime_internal_filetime
{
	/* The lower filetime part
	 */
	uint32_t lower;

	/* The upper filetime part
	 */
	uint32_t upper;
};

LIBFDATETIME_EXTERN \
int libfdatetime_filetime_initialize(
     libfdatetime_filetime_t **filetime,
     libcerror_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_filetime_free(
     libfdatetime_filetime_t **filetime,
     libcerror_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_filetime_add(
     libfdatetime_filetime_t *filetime,
     libfdatetime_filetime_t *additional_filetime,
     libcerror_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_filetime_copy_from_byte_stream(
     libfdatetime_filetime_t *filetime,
     const uint8_t *byte_stream,
     size_t byte_stream_size,
     int byte_order,
     libcerror_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_filetime_copy_from_64bit(
     libfdatetime_filetime_t *filetime,
     uint64_t value_64bit,
     libcerror_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_filetime_copy_to_64bit(
     libfdatetime_filetime_t *filetime,
     uint64_t *value_64bit,
     libcerror_error_t **error );

int libfdatetime_filetime_copy_to_date_time_values(
     libfdatetime_internal_filetime_t *internal_filetime,
     libfdatetime_date_time_values_t *date_time_values,
     libcerror_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_filetime_get_string_size(
     libfdatetime_filetime_t *filetime,
     size_t *string_size,
     uint32_t string_format_flags,
     libcerror_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_filetime_copy_to_utf8_string(
     libfdatetime_filetime_t *filetime,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     uint32_t string_format_flags,
     libcerror_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_filetime_copy_to_utf8_string_with_index(
     libfdatetime_filetime_t *filetime,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     size_t *utf8_string_index,
     uint32_t string_format_flags,
     libcerror_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_filetime_copy_to_utf16_string(
     libfdatetime_filetime_t *filetime,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     uint32_t string_format_flags,
     libcerror_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_filetime_copy_to_utf16_string_with_index(
     libfdatetime_filetime_t *filetime,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     size_t *utf16_string_index,
     uint32_t string_format_flags,
     libcerror_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_filetime_copy_to_utf32_string(
     libfdatetime_filetime_t *filetime,
     uint32_t *utf32_string,
     size_t utf32_string_size,
     uint32_t string_format_flags,
     libcerror_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_filetime_copy_to_utf32_string_with_index(
     libfdatetime_filetime_t *filetime,
     uint32_t *utf32_string,
     size_t utf32_string_size,
     size_t *utf32_string_index,
     uint32_t string_format_flags,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBFDATETIME_INTERNAL_FILETIME_H ) */

