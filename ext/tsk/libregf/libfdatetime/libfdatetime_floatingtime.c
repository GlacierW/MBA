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

#include <common.h>
#include <byte_stream.h>
#include <memory.h>
#include <types.h>

#include "libfdatetime_definitions.h"
#include "libfdatetime_date_time_values.h"
#include "libfdatetime_floatingtime.h"
#include "libfdatetime_libcerror.h"
#include "libfdatetime_types.h"

/* Creates a floatingtime
 * Make sure the value floatingtime is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libfdatetime_floatingtime_initialize(
     libfdatetime_floatingtime_t **floatingtime,
     libcerror_error_t **error )
{
	libfdatetime_internal_floatingtime_t *internal_floatingtime = NULL;
	static char *function                                       = "libfdatetime_floatingtime_initialize";

	if( floatingtime == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid floatingtime.",
		 function );

		return( -1 );
	}
	if( *floatingtime != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid floatingtime value already set.",
		 function );

		return( -1 );
	}
	internal_floatingtime = memory_allocate_structure(
	                         libfdatetime_internal_floatingtime_t );

	if( internal_floatingtime == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create floatingtime.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_floatingtime,
	     0,
	     sizeof( libfdatetime_internal_floatingtime_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear floatingtime.",
		 function );

		goto on_error;
	}
	*floatingtime = (libfdatetime_floatingtime_t *) internal_floatingtime;

	return( 1 );

on_error:
	if( internal_floatingtime != NULL )
	{
		memory_free(
		 internal_floatingtime );
	}
	return( -1 );
}

/* Frees a floatingtime
 * Returns 1 if successful or -1 on error
 */
int libfdatetime_floatingtime_free(
     libfdatetime_floatingtime_t **floatingtime,
     libcerror_error_t **error )
{
	libfdatetime_internal_floatingtime_t *internal_floatingtime = NULL;
	static char *function                                       = "libfdatetime_floatingtime_free";

	if( floatingtime == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid floatingtime.",
		 function );

		return( -1 );
	}
	if( *floatingtime != NULL )
	{
		internal_floatingtime = (libfdatetime_internal_floatingtime_t *) *floatingtime;
		*floatingtime         = NULL;

		memory_free(
		 internal_floatingtime );
	}
	return( 1 );
}

/* Converts a byte stream into a floatingtime
 * Returns 1 if successful or -1 on error
 */
int libfdatetime_floatingtime_copy_from_byte_stream(
     libfdatetime_floatingtime_t *floatingtime,
     const uint8_t *byte_stream,
     size_t byte_stream_size,
     int byte_order,
     libcerror_error_t **error )
{
	libfdatetime_internal_floatingtime_t *internal_floatingtime = NULL;
	static char *function                                       = "libfdatetime_floatingtime_copy_from_byte_stream";

	if( floatingtime == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid floatingtime.",
		 function );

		return( -1 );
	}
	internal_floatingtime = (libfdatetime_internal_floatingtime_t *) floatingtime;

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
	if( byte_stream_size < 8 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: byte stream too small.",
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
	if( byte_order == LIBFDATETIME_ENDIAN_LITTLE )
	{
		byte_stream_copy_to_uint64_little_endian(
		 byte_stream,
		 internal_floatingtime->timestamp.integer );
	}
	else if( byte_order == LIBFDATETIME_ENDIAN_BIG )
	{
		byte_stream_copy_to_uint64_big_endian(
		 byte_stream,
		 internal_floatingtime->timestamp.integer );
	}
	return( 1 );
}

/* Converts a 64-bit value into a floatingtime
 * Returns 1 if successful or -1 on error
 */
int libfdatetime_floatingtime_copy_from_64bit(
     libfdatetime_floatingtime_t *floatingtime,
     uint64_t value_64bit,
     libcerror_error_t **error )
{
	libfdatetime_internal_floatingtime_t *internal_floatingtime = NULL;
	static char *function                                       = "libfdatetime_floatingtime_copy_from_64bit";

	if( floatingtime == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid floatingtime.",
		 function );

		return( -1 );
	}
	internal_floatingtime = (libfdatetime_internal_floatingtime_t *) floatingtime;

	internal_floatingtime->timestamp.integer = value_64bit;

	return( 1 );
}

/* Converts a floatingtime into a 64-bit value
 * Returns 1 if successful or -1 on error
 */
int libfdatetime_floatingtime_copy_to_64bit(
     libfdatetime_floatingtime_t *floatingtime,
     uint64_t *value_64bit,
     libcerror_error_t **error )
{
	libfdatetime_internal_floatingtime_t *internal_floatingtime = NULL;
	static char *function                                       = "libfdatetime_floatingtime_copy_from_64bit";

	if( floatingtime == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid floatingtime.",
		 function );

		return( -1 );
	}
	internal_floatingtime = (libfdatetime_internal_floatingtime_t *) floatingtime;

	if( value_64bit == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid 64-bit value.",
		 function );

		return( -1 );
	}
	*value_64bit = internal_floatingtime->timestamp.integer;

	return( 1 );
}

/* Converts a floatingtime into date time values
 * Returns 1 if successful or -1 on error
 */
int libfdatetime_floatingtime_copy_to_date_time_values(
     libfdatetime_internal_floatingtime_t *internal_floatingtime,
     libfdatetime_date_time_values_t *date_time_values,
     libcerror_error_t **error )
{
	static char *function    = "libfdatetime_floatingtime_copy_to_date_time_values";
	double floatingtimestamp = 0;
	uint32_t days_in_century = 0;
	uint16_t days_in_year    = 0;
	uint8_t days_in_month    = 0;

	if( internal_floatingtime == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid floatingtime.",
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
	floatingtimestamp = internal_floatingtime->timestamp.floating_point;

	/* Determine the number of years starting at '30 Dec 1899 00:00:00'
	 * correct the value to days within the year
	 */
	date_time_values->year = 1899;

	if( floatingtimestamp >= 2 )
	{
		date_time_values->year = 1900;

		floatingtimestamp -= 2;
	}
	while( floatingtimestamp > 0 )
	{
		if( ( date_time_values->year % 400 ) == 0 )
		{
			days_in_century = 36525;
		}
		else
		{
			days_in_century = 36524;
		}
		if( floatingtimestamp <= days_in_century )
		{
			break;
		}
		floatingtimestamp -= days_in_century;

		date_time_values->year += 100;
	}
	while( floatingtimestamp > 0 )
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
		if( floatingtimestamp <= days_in_year )
		{
			break;
		}
		floatingtimestamp -= days_in_year;

		date_time_values->year += 1;
	}
	/* Determine the month correct the value to days within the month
	 */
	date_time_values->month = 1;

	while( floatingtimestamp > 0 )
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
		if( floatingtimestamp <= days_in_month )
		{
			break;
		}
		floatingtimestamp -= days_in_month;

		date_time_values->month += 1;
	}
	/* Determine the day
	 */
	date_time_values->day = (uint8_t) floatingtimestamp;
	floatingtimestamp    -= date_time_values->day;

	/* There are 24 hours in a day correct the value to hours
	 */
	floatingtimestamp      *= 24;
	date_time_values->hours = (uint8_t) floatingtimestamp;
	floatingtimestamp      -= date_time_values->hours;

	/* There are 60 minutes in an hour correct the value to minutes
	 */
	floatingtimestamp        *= 60;
	date_time_values->minutes = (uint8_t) floatingtimestamp;
	floatingtimestamp        -= date_time_values->minutes;

	/* There are 60 seconds in a minute correct the value to seconds
	 */
	floatingtimestamp        *= 60;
	date_time_values->seconds = (uint8_t) floatingtimestamp;
	floatingtimestamp        -= date_time_values->seconds;

	/* There are 1000 milli seconds in a seconds correct the value to milli seconds
	 */
	floatingtimestamp              *= 1000;
	date_time_values->milli_seconds = (uint8_t) floatingtimestamp;
	floatingtimestamp              -= date_time_values->milli_seconds;

	/* There are 1000 micro seconds in a seconds correct the value to micro seconds
	 */
	floatingtimestamp              *= 1000;
	date_time_values->micro_seconds = (uint8_t) floatingtimestamp;
	floatingtimestamp              -= date_time_values->micro_seconds;

	/* There are 1000 nano seconds in a seconds correct the value to nano seconds
	 */
	floatingtimestamp             *= 1000;
	date_time_values->nano_seconds = (uint8_t) floatingtimestamp;

	return( 1 );
}

/* Deterimes the size of the string for the floatingtime
 * The string size includes the end of string character
 * Returns 1 if successful or -1 on error
 */
int libfdatetime_floatingtime_get_string_size(
     libfdatetime_floatingtime_t *floatingtime,
     size_t *string_size,
     uint32_t string_format_flags,
     libcerror_error_t **error )
{
	libfdatetime_date_time_values_t date_time_values;

	static char *function = "libfdatetime_floatingtime_get_string_size";
	int result            = 0;

	if( floatingtime == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid floatingtime.",
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
	if( libfdatetime_floatingtime_copy_to_date_time_values(
	     (libfdatetime_internal_floatingtime_t *) floatingtime,
	     &date_time_values,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
		 "%s: unable to copy floatingtime to date time values.",
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
		/* Make sure the string can hold the hexadecimal representation of the floatingtime
		 */
		*string_size = 21;
	}
	return( 1 );
}

/* Converts the floatingtime into an UTF-8 string
 * The string size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libfdatetime_floatingtime_copy_to_utf8_string(
     libfdatetime_floatingtime_t *floatingtime,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     uint32_t string_format_flags,
     libcerror_error_t **error )
{
	static char *function    = "libfdatetime_floatingtime_copy_to_utf8_string";
	size_t utf8_string_index = 0;

	if( libfdatetime_floatingtime_copy_to_utf8_string_with_index(
	     floatingtime,
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
		 "%s: unable to copy floatingtime to UTF-8 string.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Converts the floatingtime into an UTF-8 string
 * The string size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libfdatetime_floatingtime_copy_to_utf8_string_with_index(
     libfdatetime_floatingtime_t *floatingtime,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     size_t *utf8_string_index,
     uint32_t string_format_flags,
     libcerror_error_t **error )
{
	libfdatetime_date_time_values_t date_time_values;

	libfdatetime_internal_floatingtime_t *internal_floatingtime = NULL;
	static char *function                                       = "libfdatetime_floatingtime_copy_to_utf8_string_with_index";
	size_t string_index                                         = 0;
	uint8_t byte_value                                          = 0;
	int8_t byte_shift                                           = 0;
	int result                                                  = 0;

	if( floatingtime == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid floatingtime.",
		 function );

		return( -1 );
	}
	internal_floatingtime = (libfdatetime_internal_floatingtime_t *) floatingtime;

	if( libfdatetime_floatingtime_copy_to_date_time_values(
	     internal_floatingtime,
	     &date_time_values,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
		 "%s: unable to copy floatingtime to date time values.",
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
		if( ( *utf8_string_index + 24 ) > utf8_string_size )
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

		byte_shift = 60;

		do
		{
			byte_value = ( internal_floatingtime->timestamp.integer >> byte_shift ) & 0x0f;

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

/* Converts the floatingtime into an UTF-16 string
 * The string size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libfdatetime_floatingtime_copy_to_utf16_string(
     libfdatetime_floatingtime_t *floatingtime,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     uint32_t string_format_flags,
     libcerror_error_t **error )
{
	static char *function     = "libfdatetime_floatingtime_copy_to_utf16_string";
	size_t utf16_string_index = 0;

	if( libfdatetime_floatingtime_copy_to_utf16_string_with_index(
	     floatingtime,
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
		 "%s: unable to copy floatingtime to UTF-16 string.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Converts the floatingtime into an UTF-16 string
 * The string size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libfdatetime_floatingtime_copy_to_utf16_string_with_index(
     libfdatetime_floatingtime_t *floatingtime,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     size_t *utf16_string_index,
     uint32_t string_format_flags,
     libcerror_error_t **error )
{
	libfdatetime_date_time_values_t date_time_values;

	libfdatetime_internal_floatingtime_t *internal_floatingtime = NULL;
	static char *function                                       = "libfdatetime_floatingtime_copy_to_utf16_string_with_index";
	size_t string_index                                         = 0;
	uint8_t byte_value                                          = 0;
	int8_t byte_shift                                           = 0;
	int result                                                  = 0;

	if( floatingtime == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid floatingtime.",
		 function );

		return( -1 );
	}
	internal_floatingtime = (libfdatetime_internal_floatingtime_t *) floatingtime;

	if( libfdatetime_floatingtime_copy_to_date_time_values(
	     internal_floatingtime,
	     &date_time_values,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
		 "%s: unable to copy floatingtime to date time values.",
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
		if( ( *utf16_string_index + 24 ) > utf16_string_size )
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

		byte_shift = 60;

		do
		{
			byte_value = ( internal_floatingtime->timestamp.integer >> byte_shift ) & 0x0f;

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

/* Converts the floatingtime into an UTF-32 string
 * The string size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libfdatetime_floatingtime_copy_to_utf32_string(
     libfdatetime_floatingtime_t *floatingtime,
     uint32_t *utf32_string,
     size_t utf32_string_size,
     uint32_t string_format_flags,
     libcerror_error_t **error )
{
	static char *function     = "libfdatetime_floatingtime_copy_to_utf32_string";
	size_t utf32_string_index = 0;

	if( libfdatetime_floatingtime_copy_to_utf32_string_with_index(
	     floatingtime,
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
		 "%s: unable to copy floatingtime to UTF-32 string.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Converts the floatingtime into an UTF-32 string
 * The string size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libfdatetime_floatingtime_copy_to_utf32_string_with_index(
     libfdatetime_floatingtime_t *floatingtime,
     uint32_t *utf32_string,
     size_t utf32_string_size,
     size_t *utf32_string_index,
     uint32_t string_format_flags,
     libcerror_error_t **error )
{
	libfdatetime_date_time_values_t date_time_values;

	libfdatetime_internal_floatingtime_t *internal_floatingtime = NULL;
	static char *function                                       = "libfdatetime_floatingtime_copy_to_utf32_string_with_index";
	size_t string_index                                         = 0;
	uint8_t byte_value                                          = 0;
	int8_t byte_shift                                           = 0;
	int result                                                  = 0;

	if( floatingtime == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid floatingtime.",
		 function );

		return( -1 );
	}
	internal_floatingtime = (libfdatetime_internal_floatingtime_t *) floatingtime;

	if( libfdatetime_floatingtime_copy_to_date_time_values(
	     internal_floatingtime,
	     &date_time_values,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
		 "%s: unable to copy floatingtime to date time values.",
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
		if( ( *utf32_string_index + 24 ) > utf32_string_size )
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

		byte_shift = 60;

		do
		{
			byte_value = ( internal_floatingtime->timestamp.integer >> byte_shift ) & 0x0f;

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

		utf32_string[ string_index++ ] = (uint32_t) ' ';
		utf32_string[ string_index++ ] = (uint32_t) '0';
		utf32_string[ string_index++ ] = (uint32_t) 'x';

		utf32_string[ string_index++ ] = (uint32_t) ')';

		utf32_string[ string_index++ ] = 0;

		*utf32_string_index = string_index;
	}
	return( 1 );
}

