/*
 * Date and time values functions
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

#if !defined( _LIBFDATETIME_DATE_TIME_VALUES_H )
#define _LIBFDATETIME_DATE_TIME_VALUES_H

#include <common.h>
#include <types.h>

#include "libfdatetime_libcerror.h"
#include "libfdatetime_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libfdatetime_date_time_values libfdatetime_date_time_values_t;

struct libfdatetime_date_time_values
{
	/* The year
	 */
	uint16_t year;

	/* The month
	 * 1 represents January
	 */
	uint8_t month;

	/* The day of the month
	 */
	uint8_t day;

	/* The number of hours in 24 hour format
	 * E.g. 13 represents 1 PM
	 */
	uint8_t hours;

	/* The number of minutes
	 */
	uint8_t minutes;

	/* The number of seconds
	 */
	uint8_t seconds;

	/* The number of milli seconds
	 */
	uint16_t milli_seconds;

	/* The number of micro seconds
	 */
	uint16_t micro_seconds;

	/* The number of nano seconds
	 */
	uint16_t nano_seconds;
};

int libfdatetime_date_time_values_initialize(
     libfdatetime_date_time_values_t **date_time_values,
     libcerror_error_t **error );

int libfdatetime_date_time_values_free(
     libfdatetime_date_time_values_t **date_time_values,
     libcerror_error_t **error );

int libfdatetime_date_time_values_get_string_size(
     libfdatetime_date_time_values_t *date_time_values,
     size_t *string_size,
     uint32_t string_format_flags,
     libcerror_error_t **error );

int libfdatetime_date_time_values_copy_to_utf8_string_with_index(
     libfdatetime_date_time_values_t *date_time_values,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     size_t *utf8_string_index,
     uint32_t string_format_flags,
     libcerror_error_t **error );

int libfdatetime_date_time_values_copy_to_utf16_string_with_index(
     libfdatetime_date_time_values_t *date_time_values,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     size_t *utf16_string_index,
     uint32_t string_format_flags,
     libcerror_error_t **error );

int libfdatetime_date_time_values_copy_to_utf32_string_with_index(
     libfdatetime_date_time_values_t *date_time_values,
     uint32_t *utf32_string,
     size_t utf32_string_size,
     size_t *utf32_string_index,
     uint32_t string_format_flags,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBFDATETIME_DATE_TIME_VALUES_H ) */

