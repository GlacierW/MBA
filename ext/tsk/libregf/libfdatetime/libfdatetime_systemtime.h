/*
 * SYSTEMTIME functions
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

#if !defined( _LIBFDATETIME_INTERNAL_SYSTEMTIME_H )
#define _LIBFDATETIME_INTERNAL_SYSTEMTIME_H

#include <common.h>
#include <types.h>

#include "libfdatetime_extern.h"
#include "libfdatetime_libcerror.h"
#include "libfdatetime_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libfdatetime_internal_systemtime libfdatetime_internal_systemtime_t;

struct libfdatetime_internal_systemtime
{
	/* The year
	 */
	uint16_t year;

	/* The month
	 */
	uint16_t month;

	/* The day of week
	 */
	uint16_t day_of_week;

	/* The day of month
	 */
	uint16_t day_of_month;

	/* The hours
	 */
	uint16_t hours;

	/* The minutes
	 */
	uint16_t minutes;

	/* The seconds
	 */
	uint16_t seconds;

	/* The milli seconds
	 */
	uint16_t milli_seconds;
};

LIBFDATETIME_EXTERN \
int libfdatetime_systemtime_initialize(
     libfdatetime_systemtime_t **systemtime,
     libcerror_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_systemtime_free(
     libfdatetime_systemtime_t **systemtime,
     libcerror_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_systemtime_copy_from_byte_stream(
     libfdatetime_systemtime_t *systemtime,
     const uint8_t *byte_stream,
     size_t byte_stream_size,
     int byte_order,
     libcerror_error_t **error );

int libfdatetime_systemtime_copy_to_date_time_values(
     libfdatetime_internal_systemtime_t *internal_systemtime,
     libfdatetime_date_time_values_t *date_time_values,
     libcerror_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_systemtime_get_string_size(
     libfdatetime_systemtime_t *systemtime,
     size_t *string_size,
     uint32_t string_format_flags,
     libcerror_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_systemtime_copy_to_utf8_string(
     libfdatetime_systemtime_t *systemtime,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     uint32_t string_format_flags,
     libcerror_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_systemtime_copy_to_utf8_string_with_index(
     libfdatetime_systemtime_t *systemtime,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     size_t *utf8_string_index,
     uint32_t string_format_flags,
     libcerror_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_systemtime_copy_to_utf16_string(
     libfdatetime_systemtime_t *systemtime,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     uint32_t string_format_flags,
     libcerror_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_systemtime_copy_to_utf16_string_with_index(
     libfdatetime_systemtime_t *systemtime,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     size_t *utf16_string_index,
     uint32_t string_format_flags,
     libcerror_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_systemtime_copy_to_utf32_string(
     libfdatetime_systemtime_t *systemtime,
     uint32_t *utf32_string,
     size_t utf32_string_size,
     uint32_t string_format_flags,
     libcerror_error_t **error );

LIBFDATETIME_EXTERN \
int libfdatetime_systemtime_copy_to_utf32_string_with_index(
     libfdatetime_systemtime_t *systemtime,
     uint32_t *utf32_string,
     size_t utf32_string_size,
     size_t *utf32_string_index,
     uint32_t string_format_flags,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBFDATETIME_INTERNAL_SYSTEMTIME_H ) */

