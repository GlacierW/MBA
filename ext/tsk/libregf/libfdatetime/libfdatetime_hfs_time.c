/*
 * HFS time functions
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

#include <common.h>
#include <byte_stream.h>
#include <memory.h>
#include <types.h>

#include "libfdatetime_definitions.h"
#include "libfdatetime_date_time_values.h"
#include "libfdatetime_hfs_time.h"
#include "libfdatetime_libcerror.h"
#include "libfdatetime_types.h"

/* Creates a HFS time
 * Make sure the value hfs_time is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libfdatetime_hfs_time_initialize(
     libfdatetime_hfs_time_t **hfs_time,
     libcerror_error_t **error )
{
	libfdatetime_internal_hfs_time_t *internal_hfs_time = NULL;
	static char *function                               = "libfdatetime_hfs_time_initialize";

	if( hfs_time == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid HFS time.",
		 function );

		return( -1 );
	}
	if( *hfs_time != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid HFS time value already set.",
		 function );

		return( -1 );
	}
	internal_hfs_time = memory_allocate_structure(
	                     libfdatetime_internal_hfs_time_t );

	if( internal_hfs_time == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create HFS time.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_hfs_time,
	     0,
	     sizeof( libfdatetime_internal_hfs_time_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear HFS time.",
		 function );

		goto on_error;
	}
	*hfs_time = (libfdatetime_hfs_time_t *) internal_hfs_time;

	return( 1 );

on_error:
	if( internal_hfs_time != NULL )
	{
		memory_free(
		 internal_hfs_time );
	}
	return( -1 );
}

/* Frees a HFS time
 * Returns 1 if successful or -1 on error
 */
int libfdatetime_hfs_time_free(
     libfdatetime_hfs_time_t **hfs_time,
     libcerror_error_t **error )
{
	libfdatetime_internal_hfs_time_t *internal_hfs_time = NULL;
	static char *function                               = "libfdatetime_hfs_time_free";

	if( hfs_time == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid HFS time.",
		 function );

		return( -1 );
	}
	if( *hfs_time != NULL )
	{
		internal_hfs_time = (libfdatetime_internal_hfs_time_t *) *hfs_time;
		*hfs_time         = NULL;

		memory_free(
		 internal_hfs_time );
	}
	return( 1 );
}

/* Converts a byte stream into a HFS time
 * Returns 1 if successful or -1 on error
 */
int libfdatetime_hfs_time_copy_from_byte_stream(
     libfdatetime_hfs_time_t *hfs_time,
     const uint8_t *byte_stream,
     size_t byte_stream_size,
     int byte_order,
     libcerror_error_t **error )
{
	libfdatetime_internal_hfs_time_t *internal_hfs_time = NULL;
	static char *function                               = "libfdatetime_hfs_time_copy_from_byte_stream";
	uint32_t value_32bit                                = 0;

	if( hfs_time == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid HFS time.",
		 function );

		return( -1 );
	}
	internal_hfs_time = (libfdatetime_internal_hfs_time_t *) hfs_time;

	if( byte_stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid byte stream.",
		 function );

		return( -1 );
	}
	if( byte_stream_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: byte stream size exceeds maximum.",
		 function );

		return( -1 );
	}
	if( ( byte_order != LIBFDATETIME_ENDIAN_BIG )
	 && ( byte_order != LIBFDATETIME_ENDIAN_LITTLE ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported byte order.",
		 function );

		return( -1 );
	}
	if( byte_stream_size < 4 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: byte stream too small.",
		 function );

		return( -1 );
	}
	if( byte_order == LIBFDATETIME_ENDIAN_LITTLE )
	{
		byte_stream_copy_to_uint32_little_endian(
		 byte_stream,
		 value_32bit );
	}
	else if( byte_order == LIBFDATETIME_ENDIAN_BIG )
	{
		byte_stream_copy_to_uint32_big_endian(
		 byte_stream,
		 value_32bit );
	}
	internal_hfs_time->timestamp = value_32bit;

	return( 1 );
}

/* Converts a 32-bit value into a HFS time
 * Returns 1 if successful or -1 on error
 */
int libfdatetime_hfs_time_copy_from_32bit(
     libfdatetime_hfs_time_t *hfs_time,
     uint32_t value_32bit,
     libcerror_error_t **error )
{
	libfdatetime_internal_hfs_time_t *internal_hfs_time = NULL;
	static char *function                               = "libfdatetime_hfs_time_copy_from_32bit";

	if( hfs_time == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid HFS time.",
		 function );

		return( -1 );
	}
	internal_hfs_time = (libfdatetime_internal_hfs_time_t *) hfs_time;

	internal_hfs_time->timestamp = value_32bit;

	return( 1 );
}

/* Converts a HFS time into a 32-bit value
 * Returns 1 if successful or -1 on error
 */
int libfdatetime_hfs_time_copy_to_32bit(
     libfdatetime_hfs_time_t *hfs_time,
     uint32_t *value_32bit,
     libcerror_error_t **error )
{
	libfdatetime_internal_hfs_time_t *internal_hfs_time = NULL;
	static char *function                               = "libfdatetime_hfs_time_copy_to_32bit";

	if( hfs_time == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid HFS time.",
		 function );

		return( -1 );
	}
	internal_hfs_time = (libfdatetime_internal_hfs_time_t *) hfs_time;

	if( value_32bit == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid 32-bit value.",
		 function );

		return( -1 );
	}
	*value_32bit = internal_hfs_time->timestamp;

	return( 1 );
}

/* Converts a HFS time into date time values
 * Returns 1 if successful or -1 on error
 */
int libfdatetime_hfs_time_copy_to_date_time_values(
     libfdatetime_internal_hfs_time_t *internal_hfs_time,
     libfdatetime_date_time_values_t *date_time_values,
     libcerror_error_t **error )
{
	static char *function  = "libfdatetime_hfs_time_copy_to_date_time_values";
	uint32_t hfs_timestamp = 0;
	uint16_t days_in_year  = 0;
	uint8_t days_in_month  = 0;

	if( internal_hfs_time == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid HFS time.",
		 function );

		return( -1 );
	}
	if( date_time_values == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid date time values.",
		 function );

		return( -1 );
	}
	hfs_timestamp = internal_hfs_time->timestamp;

        date_time_values->micro_seconds = 0;
        date_time_values->milli_seconds = 0;
        date_time_values->nano_seconds  = 0;

	/* There are 60 seconds in a minute correct the value to minutes
	 */
	date_time_values->seconds = hfs_timestamp % 60;
	hfs_timestamp            /= 60;

	/* There are 60 minutes in an hour correct the value to hours
	 */
	date_time_values->minutes = hfs_timestamp % 60;
	hfs_timestamp            /= 60;

	/* There are 24 hours in a day correct the value to days
	 */
	date_time_values->hours = hfs_timestamp % 24;
	hfs_timestamp          /= 24;

	/* Determine the number of years starting at 'Jan 1, 1904 00:00:00'
	 * correct the value to days within the year
	 */
	/* Add 1 day to compensate that Jan 1, 1904 is represented as 0
	 */
	hfs_timestamp += 1;

	date_time_values->year = 1904;

	if( hfs_timestamp >= 35064 )
	{
		date_time_values->year = 2000;

		hfs_timestamp -= 35064;
	}
	while( hfs_timestamp > 0 )
	{
		/* Check for a leap year
		 * The year is ( ( dividable by 4 ) and ( not dividable by 100 ) ) or ( dividable by 400 )
		 */
		if( ( ( ( date_time_values->year % 4 ) == 0 )
		  &&  ( ( date_time_values->year % 100 ) != 0 ) )
		 || ( ( date_time_values->year % 400 ) == 0 ) )
		{
			days_in_year = 366;
		}
		else
		{
			days_in_year = 365;
		}
		if( hfs_timestamp <= days_in_year )
		{
			break;
		}
		hfs_timestamp -= days_in_year;

		date_time_values->year += 1;
	}

	/* Determine the month correct the value to days within the month
	 */
	date_time_values->month = 1;

	do
	{
		/* February (2)
		 */
		if( date_time_values->month == 2 )
		{
			if( ( ( ( date_time_values->year % 4 ) == 0 )
			  &&  ( ( date_time_values->year % 100 ) != 0 ) )
			 || ( ( date_time_values->year % 400 ) == 0 ) )
			{
				days_in_month = 29;
			}
			else
			{
				days_in_month = 28;
			}
		}
		/* April (4), June (6), September (9), November (11)
		 */
		else if( ( date_time_values->month == 4 )
		      || ( date_time_values->month == 6 )
		      || ( date_time_values->month == 9 )
		      || ( date_time_values->month == 11 ) )
		{
			days_in_month = 30;
		}
		/* January (1), March (3), May (5), July (7), August (8), October (10), December (12)
		 */
		else if( ( date_time_values->month == 1 )
		      || ( date_time_values->month == 3 )
		      || ( date_time_values->month == 5 )
		      || ( date_time_values->month == 7 )
		      || ( date_time_values->month == 8 )
		      || ( date_time_values->month == 10 )
		      || ( date_time_values->month == 12 ) )
		{
			days_in_month = 31;
		}
		/* This should never happen, but just in case
		 */
		else
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
			 "%s: unsupported month: %d.",
			 function,
			 date_time_values->month );

			return( -1 );
		}
		if( hfs_timestamp <= days_in_month )
		{
			break;
		}
		hfs_timestamp -= days_in_month;

		date_time_values->month += 1;
	}
	while( hfs_timestamp > 0 );

	/* Determine the day
	 */
	date_time_values->day = (uint8_t) hfs_timestamp;

	return( 1 );
}

/* Deterimes the size of the string for the HFS time
 * The string size includes the end of string character
 * Returns 1 if successful or -1 on error
 */
int libfdatetime_hfs_time_get_string_size(
     libfdatetime_hfs_time_t *hfs_time,
     size_t *string_size,
     uint32_t string_format_flags,
     libcerror_error_t **error )
{
	libfdatetime_date_time_values_t date_time_values;

	static char *function = "libfdatetime_hfs_time_get_string_size";
	int result            = 0;

	if( hfs_time == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid HFS time.",
		 function );

		return( -1 );
	}
	if( string_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid string size.",
		 function );

		return( -1 );
	}
	if( libfdatetime_hfs_time_copy_to_date_time_values(
	     (libfdatetime_internal_hfs_time_t *) hfs_time,
	     &date_time_values,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set date time values.",
		 function );

		return( -1 );
	}
	result = libfdatetime_date_time_values_get_string_size(
	          &date_time_values,
	          string_size,
	          string_format_flags,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to get string size.",
		 function );

		return( -1 );
	}
	else if( result == 0 )
	{
		/* Make sure the string can hold the hexadecimal representation of the HFS time
		 */
		*string_size = 13;
	}
	return( 1 );
}

/* Converts the HFS time into an UTF-8 string
 * The string size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libfdatetime_hfs_time_copy_to_utf8_string(
     libfdatetime_hfs_time_t *hfs_time,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     uint32_t string_format_flags,
     libcerror_error_t **error )
{
	static char *function    = "libfdatetime_hfs_time_copy_to_utf8_string";
	size_t utf8_string_index = 0;

	if( libfdatetime_hfs_time_copy_to_utf8_string_with_index(
	     hfs_time,
	     utf8_string,
	     utf8_string_size,
	     &utf8_string_index,
	     string_format_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
		 "%s: unable to copy HFS time to UTF-8 string.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Converts the HFS time into an UTF-8 string
 * The string size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libfdatetime_hfs_time_copy_to_utf8_string_with_index(
     libfdatetime_hfs_time_t *hfs_time,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     size_t *utf8_string_index,
     uint32_t string_format_flags,
     libcerror_error_t **error )
{
	libfdatetime_date_time_values_t date_time_values;

	libfdatetime_internal_hfs_time_t *internal_hfs_time = NULL;
	static char *function                               = "libfdatetime_hfs_time_copy_to_utf8_string_with_index";
	size_t string_index                                 = 0;
	uint8_t byte_value                                  = 0;
	int8_t byte_shift                                   = 0;
	int result                                          = 0;

	if( hfs_time == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid HFS time.",
		 function );

		return( -1 );
	}
	internal_hfs_time = (libfdatetime_internal_hfs_time_t *) hfs_time;

	if( libfdatetime_hfs_time_copy_to_date_time_values(
	     internal_hfs_time,
	     &date_time_values,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set date time values.",
		 function );

		return( -1 );
	}
	/* Create the date and time string
	 */
	result = libfdatetime_date_time_values_copy_to_utf8_string_with_index(
	          &date_time_values,
	          utf8_string,
	          utf8_string_size,
	          utf8_string_index,
	          string_format_flags,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set string.",
		 function );

		return( -1 );
	}
	else if( result == 0 )
	{
		if( utf8_string == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
			 "%s: invalid UTF-8 string.",
			 function );

			return( -1 );
		}
		if( utf8_string_size > (size_t) SSIZE_MAX )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
			 "%s: invalid UTF-8 string size value exceeds maximum.",
			 function );

			return( -1 );
		}
		if( utf8_string_index == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
			 "%s: invalid UTF-8 string index.",
			 function );

			return( -1 );
		}
		if( ( *utf8_string_index + 13 ) > utf8_string_size )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
			 "%s: UTF-8 string is too small.",
			 function );

			return( -1 );
		}
		string_index = *utf8_string_index;

		utf8_string[ string_index++ ] = (uint8_t) '(';
		utf8_string[ string_index++ ] = (uint8_t) '0';
		utf8_string[ string_index++ ] = (uint8_t) 'x';

		byte_shift = 28;

		do
		{
			byte_value = ( internal_hfs_time->timestamp >> byte_shift ) & 0x0f;

			if( byte_value <= 9 )
			{
				utf8_string[ string_index++ ] = (uint8_t) '0' + byte_value;
			}
			else
			{
				utf8_string[ string_index++ ] = (uint8_t) 'a' + byte_value - 10;
			}
			byte_shift -= 4;
		}
		while( byte_shift >= 0 );

		utf8_string[ string_index++ ] = (uint8_t) ')';

		utf8_string[ string_index++ ] = 0;

		*utf8_string_index = string_index;
	}
	return( 1 );
}

/* Converts the HFS time into an UTF-16 string
 * The string size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libfdatetime_hfs_time_copy_to_utf16_string(
     libfdatetime_hfs_time_t *hfs_time,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     uint32_t string_format_flags,
     libcerror_error_t **error )
{
	static char *function     = "libfdatetime_hfs_time_copy_to_utf16_string";
	size_t utf16_string_index = 0;

	if( libfdatetime_hfs_time_copy_to_utf16_string_with_index(
	     hfs_time,
	     utf16_string,
	     utf16_string_size,
	     &utf16_string_index,
	     string_format_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
		 "%s: unable to copy HFS time to UTF-16 string.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Converts the HFS time into an UTF-16 string
 * The string size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libfdatetime_hfs_time_copy_to_utf16_string_with_index(
     libfdatetime_hfs_time_t *hfs_time,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     size_t *utf16_string_index,
     uint32_t string_format_flags,
     libcerror_error_t **error )
{
	libfdatetime_date_time_values_t date_time_values;

	libfdatetime_internal_hfs_time_t *internal_hfs_time = NULL;
	static char *function                               = "libfdatetime_hfs_time_copy_to_utf16_string_with_index";
	size_t string_index                                 = 0;
	uint8_t byte_value                                  = 0;
	int8_t byte_shift                                   = 0;
	int result                                          = 0;

	if( hfs_time == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid HFS time.",
		 function );

		return( -1 );
	}
	internal_hfs_time = (libfdatetime_internal_hfs_time_t *) hfs_time;

	if( libfdatetime_hfs_time_copy_to_date_time_values(
	     internal_hfs_time,
	     &date_time_values,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set date time values.",
		 function );

		return( -1 );
	}
	/* Create the date and time string
	 */
	result = libfdatetime_date_time_values_copy_to_utf16_string_with_index(
	          &date_time_values,
	          utf16_string,
	          utf16_string_size,
	          utf16_string_index,
	          string_format_flags,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set string.",
		 function );

		return( -1 );
	}
	else if( result == 0 )
	{
		if( utf16_string == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
			 "%s: invalid UTF-16 string.",
			 function );

			return( -1 );
		}
		if( utf16_string_size > (size_t) SSIZE_MAX )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
			 "%s: invalid UTF-16 string size value exceeds maximum.",
			 function );

			return( -1 );
		}
		if( utf16_string_index == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
			 "%s: invalid UTF-16 string index.",
			 function );

			return( -1 );
		}
		if( ( *utf16_string_index + 13 ) > utf16_string_size )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
			 "%s: UTF-16 string is too small.",
			 function );

			return( -1 );
		}
		string_index = *utf16_string_index;

		utf16_string[ string_index++ ] = (uint16_t) '(';
		utf16_string[ string_index++ ] = (uint16_t) '0';
		utf16_string[ string_index++ ] = (uint16_t) 'x';

		byte_shift = 28;

		do
		{
			byte_value = ( internal_hfs_time->timestamp >> byte_shift ) & 0x0f;

			if( byte_value <= 9 )
			{
				utf16_string[ string_index++ ] = (uint16_t) '0' + byte_value;
			}
			else
			{
				utf16_string[ string_index++ ] = (uint16_t) 'a' + byte_value - 10;
			}
			byte_shift -= 4;
		}
		while( byte_shift >= 0 );

		utf16_string[ string_index++ ] = (uint16_t) ')';

		utf16_string[ string_index++ ] = 0;

		*utf16_string_index = string_index;
	}
	return( 1 );
}

/* Converts the HFS time into an UTF-32 string
 * The string size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libfdatetime_hfs_time_copy_to_utf32_string(
     libfdatetime_hfs_time_t *hfs_time,
     uint32_t *utf32_string,
     size_t utf32_string_size,
     uint32_t string_format_flags,
     libcerror_error_t **error )
{
	static char *function     = "libfdatetime_hfs_time_copy_to_utf32_string";
	size_t utf32_string_index = 0;

	if( libfdatetime_hfs_time_copy_to_utf32_string_with_index(
	     hfs_time,
	     utf32_string,
	     utf32_string_size,
	     &utf32_string_index,
	     string_format_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
		 "%s: unable to copy HFS time to UTF-32 string.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Converts the HFS time into an UTF-32 string
 * The string size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libfdatetime_hfs_time_copy_to_utf32_string_with_index(
     libfdatetime_hfs_time_t *hfs_time,
     uint32_t *utf32_string,
     size_t utf32_string_size,
     size_t *utf32_string_index,
     uint32_t string_format_flags,
     libcerror_error_t **error )
{
	libfdatetime_date_time_values_t date_time_values;

	libfdatetime_internal_hfs_time_t *internal_hfs_time = NULL;
	static char *function                               = "libfdatetime_hfs_time_copy_to_utf32_string_with_index";
	size_t string_index                                 = 0;
	uint8_t byte_value                                  = 0;
	int8_t byte_shift                                   = 0;
	int result                                          = 0;

	if( hfs_time == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid HFS time.",
		 function );

		return( -1 );
	}
	internal_hfs_time = (libfdatetime_internal_hfs_time_t *) hfs_time;

	if( libfdatetime_hfs_time_copy_to_date_time_values(
	     internal_hfs_time,
	     &date_time_values,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set date time values.",
		 function );

		return( -1 );
	}
	/* Create the date and time string
	 */
	result = libfdatetime_date_time_values_copy_to_utf32_string_with_index(
	          &date_time_values,
	          utf32_string,
	          utf32_string_size,
	          utf32_string_index,
	          string_format_flags,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set string.",
		 function );

		return( -1 );
	}
	else if( result == 0 )
	{
		if( utf32_string == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
			 "%s: invalid UTF-32 string.",
			 function );

			return( -1 );
		}
		if( utf32_string_size > (size_t) SSIZE_MAX )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
			 "%s: invalid UTF-32 string size value exceeds maximum.",
			 function );

			return( -1 );
		}
		if( utf32_string_index == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
			 "%s: invalid UTF-32 string index.",
			 function );

			return( -1 );
		}
		if( ( *utf32_string_index + 13 ) > utf32_string_size )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
			 "%s: UTF-32 string is too small.",
			 function );

			return( -1 );
		}
		string_index = *utf32_string_index;

		utf32_string[ string_index++ ] = (uint32_t) '(';
		utf32_string[ string_index++ ] = (uint32_t) '0';
		utf32_string[ string_index++ ] = (uint32_t) 'x';

		byte_shift = 28;

		do
		{
			byte_value = ( internal_hfs_time->timestamp >> byte_shift ) & 0x0f;

			if( byte_value <= 9 )
			{
				utf32_string[ string_index++ ] = (uint32_t) '0' + byte_value;
			}
			else
			{
				utf32_string[ string_index++ ] = (uint32_t) 'a' + byte_value - 10;
			}
			byte_shift -= 4;
		}
		while( byte_shift >= 0 );

		utf32_string[ string_index++ ] = (uint32_t) ')';

		utf32_string[ string_index++ ] = 0;

		*utf32_string_index = string_index;
	}
	return( 1 );
}

