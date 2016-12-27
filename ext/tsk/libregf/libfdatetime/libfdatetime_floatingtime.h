/*
 * Floatingtime functions
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

#if !defined( _LIBFDATETIME_INTERNAL_FLOATINGTIME_H )
#define _LIBFDATETIME_INTERNAL_FLOATINGTIME_H

#include <common.h>
#include <byte_stream.h>
#include <types.h>

#include "libfdatetime_extern.h"
#include "libfdatetime_libcerror.h"
#include "libfdatetime_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libfdatetime_internal_floatingtime libfdatetime_internal_floatingtime_t;

/* Contains a 64-bit value containing a floating point where the whole number part represents
 * the number of days since December 30, 1899 and the fractional part fraction of a day since midnight
 */
struct libfdatetime_internal_floatingtime
{
	/* The floatingtime floating point
	 */
	byte_stream_float64_t timestamp;
};

LIBFDATETIME_EXTERN \
int libfdatetime_floatingtime_initialize(
     libfdatetime_floatingtime_t **floatingtime,
     libcerror_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_floatingtime_free(
     libfdatetime_floatingtime_t **floatingtime,
     libcerror_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_floatingtime_copy_from_byte_stream(
     libfdatetime_floatingtime_t *floatingtime,
     const uint8_t *byte_stream,
     size_t byte_stream_size,
     int byte_order,
     libcerror_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_floatingtime_copy_from_64bit(
     libfdatetime_floatingtime_t *floatingtime,
     uint64_t value_64bit,
     libcerror_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_floatingtime_copy_to_64bit(
     libfdatetime_floatingtime_t *floatingtime,
     uint64_t *value_64bit,
     libcerror_error_t **error );

int libfdatetime_floatingtime_copy_to_date_time_values(
     libfdatetime_internal_floatingtime_t *internal_floatingtime,
     libfdatetime_date_time_values_t *date_time_values,
     libcerror_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_floatingtime_get_string_size(
     libfdatetime_floatingtime_t *floatingtime,
     size_t *string_size,
     uint32_t string_format_flags,
     libcerror_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_floatingtime_copy_to_utf8_string(
     libfdatetime_floatingtime_t *floatingtime,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     uint32_t string_format_flags,
     libcerror_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_floatingtime_copy_to_utf8_string_with_index(
     libfdatetime_floatingtime_t *floatingtime,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     size_t *utf8_string_index,
     uint32_t string_format_flags,
     libcerror_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_floatingtime_copy_to_utf16_string(
     libfdatetime_floatingtime_t *floatingtime,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     uint32_t string_format_flags,
     libcerror_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_floatingtime_copy_to_utf16_string_with_index(
     libfdatetime_floatingtime_t *floatingtime,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     size_t *utf16_string_index,
     uint32_t string_format_flags,
     libcerror_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_floatingtime_copy_to_utf32_string(
     libfdatetime_floatingtime_t *floatingtime,
     uint32_t *utf32_string,
     size_t utf32_string_size,
     uint32_t string_format_flags,
     libcerror_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_floatingtime_copy_to_utf32_string_with_index(
     libfdatetime_floatingtime_t *floatingtime,
     uint32_t *utf32_string,
     size_t utf32_string_size,
     size_t *utf32_string_index,
     uint32_t string_format_flags,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBFDATETIME_INTERNAL_FLOATINGTIME_H ) */

