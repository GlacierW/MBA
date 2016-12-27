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

#include <common.h>
#include <memory.h>
#include <types.h>

#include "libfdatetime_definitions.h"
#include "libfdatetime_date_time_values.h"
#include "libfdatetime_libcerror.h"
#include "libfdatetime_types.h"

/* Creates date time values
 * Make sure the value date_time_values is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libfdatetime_date_time_values_initialize(
     libfdatetime_date_time_values_t **date_time_values,
     libcerror_error_t **error )
{
	static char *function = "libfdatetime_date_time_values_initialize";

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
	if( *date_time_values != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid date time values value already set.",
		 function );

		return( -1 );
	}
	*date_time_values = memory_allocate_structure(
	                     libfdatetime_date_time_values_t );

	if( *date_time_values == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create date time values.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *date_time_values,
	     0,
	     sizeof( libfdatetime_date_time_values_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear date time values.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( *date_time_values != NULL )
	{
		memory_free(
		 *date_time_values );

		*date_time_values = NULL;
	}
	return( -1 );
}

/* Frees date time values
 * Returns 1 if successful or -1 on error
 */
int libfdatetime_date_time_values_free(
     libfdatetime_date_time_values_t **date_time_values,
     libcerror_error_t **error )
{
	static char *function = "libfdatetime_date_time_values_free";

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
	if( *date_time_values != NULL )
	{
		memory_free(
		 *date_time_values );

		*date_time_values = NULL;
	}
	return( 1 );
}

/* Deterimes the size of the string for the date and time values
 * The string size includes the end of string character
 * Returns 1 if successful, 0 if the date and time values are not valid or -1 on error
 */
int libfdatetime_date_time_values_get_string_size(
     libfdatetime_date_time_values_t *date_time_values,
     size_t *string_size,
     uint32_t string_format_flags,
     libcerror_error_t **error )
{
	static char *function       = "libfdatetime_date_time_values_get_string_size";
	uint32_t string_format_type = 0;
	uint32_t supported_flags    = 0;
	uint8_t days_in_month       = 0;

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
	supported_flags = 0x000000ffUL
	                | LIBFDATETIME_STRING_FORMAT_FLAG_DATE
	                | LIBFDATETIME_STRING_FORMAT_FLAG_TIME
	                | LIBFDATETIME_STRING_FORMAT_FLAG_DURATION
	                | LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MILLI_SECONDS
	                | LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MICRO_SECONDS
	                | LIBFDATETIME_STRING_FORMAT_FLAG_TIME_NANO_SECONDS
	                | LIBFDATETIME_STRING_FORMAT_FLAG_TIMEZONE_INDICATOR;

	if( ( string_format_flags & supported_flags ) == 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported string format flags: 0x%08" PRIx32 ".",
		 function,
		 string_format_flags );

		return( -1 );
	}
	string_format_type = string_format_flags & 0x000000ffUL;

	if( ( string_format_type != LIBFDATETIME_STRING_FORMAT_TYPE_CTIME )
	 && ( string_format_type != LIBFDATETIME_STRING_FORMAT_TYPE_ISO8601 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported string format type: 0x%08" PRIx32 ".",
		 function,
		 string_format_type );

		return( -1 );
	}
	/* Validate the date and time if necessary
	 */
	if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_DATE ) != 0 )
	{
		if( date_time_values->year > 9999 )
		{
			return( 0 );
		}
		if( ( date_time_values->month == 0 )
		 || ( date_time_values->month > 12 ) )
		{
			return( 0 );
		}
		switch( date_time_values->month )
		{
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
			case 12:
				days_in_month = 31;
				break;
			case 4:
			case 6:
			case 9:
			case 11:
				days_in_month = 30;
				break;
			case 2:
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
				break;
		}
		if( ( date_time_values->day == 0 )
		 || ( date_time_values->day > days_in_month ) )
		{
			return( 0 );
		}
	}
	if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME ) != 0 )
	{
		if( date_time_values->hours > 23 )
		{
			return( 0 );
		}
		if( date_time_values->minutes > 59 )
		{
			return( 0 );
		}
		if( date_time_values->seconds > 59 )
		{
			return( 0 );
		}
		if( ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MILLI_SECONDS ) != 0 )
		 || ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MICRO_SECONDS ) != 0 )
		 || ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_NANO_SECONDS ) != 0 ) )
		{
			if( date_time_values->milli_seconds > 999 )
			{
				return( 0 );
			}
		}
		if( ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MICRO_SECONDS ) != 0 )
		 || ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_NANO_SECONDS ) != 0 ) )
		{
			if( date_time_values->micro_seconds > 999 )
			{
				return( 0 );
			}
		}
		if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_NANO_SECONDS ) != 0 )
		{
			if( date_time_values->nano_seconds > 999 )
			{
				return( 0 );
			}
		}
	}
	/* End of string character
	 */
	*string_size = 1;

	/* Determine the size of the date and time string
	 */
	if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_DATE ) != 0 )
	{
		/* Example: Jan 01, 1970
		 */
		if( string_format_type == LIBFDATETIME_STRING_FORMAT_TYPE_CTIME )
		{
			*string_size += 12;
		}
		/* Example: 1970-01-01
		 */
		else if( string_format_type == LIBFDATETIME_STRING_FORMAT_TYPE_ISO8601 )
		{
			*string_size += 10;
		}
	}
	if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME ) != 0 )
	{
		/* Date and time separator
		 */
		if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_DATE ) != 0 )
		{
			*string_size += 1;
		}
		/* Example: 00:00:00
		 */
		*string_size += 8;

		/* Example: .000
		 */
		if( ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MILLI_SECONDS ) != 0 )
		 || ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MICRO_SECONDS ) != 0 )
		 || ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_NANO_SECONDS ) != 0 ) )
		{
			*string_size += 4;
		}
		/* Example: .000000
		 */
		if( ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MICRO_SECONDS ) != 0 )
		 || ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_NANO_SECONDS ) != 0 ) )
		{
			*string_size += 3;
		}
		/* Example: .000000000
		 */
		if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_NANO_SECONDS ) != 0 )
		{
			*string_size += 3;
		}
	}
	if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIMEZONE_INDICATOR ) != 0 )
	{
		/* Example: UTC
		 */
		if( string_format_type == LIBFDATETIME_STRING_FORMAT_TYPE_CTIME )
		{
			*string_size += 4;
		}
		/* Example: Z
		 */
		else if( string_format_type == LIBFDATETIME_STRING_FORMAT_TYPE_ISO8601 )
		{
			*string_size += 1;
		}
	}
	return( 1 );
}

/* Converts the date and time values into an UTF-8 string
 * The string size should include the end of string character
 * Returns 1 if successful, 0 if the date and time values are not valid or -1 on error
 */
int libfdatetime_date_time_values_copy_to_utf8_string_with_index(
     libfdatetime_date_time_values_t *date_time_values,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     size_t *utf8_string_index,
     uint32_t string_format_flags,
     libcerror_error_t **error )
{
	char *month_string          = NULL;
	static char *function       = "libfdatetime_date_time_values_copy_to_utf8_string_with_index";
	size_t string_index         = 0;
	uint32_t string_format_type = 0;
	uint32_t supported_flags    = 0;
	uint16_t micro_seconds      = 0;
	uint16_t milli_seconds      = 0;
	uint16_t nano_seconds       = 0;
	uint16_t year_value         = 0;
	uint8_t days_in_month       = 0;

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
	supported_flags = 0x000000ffUL
	                | LIBFDATETIME_STRING_FORMAT_FLAG_DATE
	                | LIBFDATETIME_STRING_FORMAT_FLAG_TIME
	                | LIBFDATETIME_STRING_FORMAT_FLAG_DURATION
	                | LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MILLI_SECONDS
	                | LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MICRO_SECONDS
	                | LIBFDATETIME_STRING_FORMAT_FLAG_TIME_NANO_SECONDS
	                | LIBFDATETIME_STRING_FORMAT_FLAG_TIMEZONE_INDICATOR;

	if( ( string_format_flags & supported_flags ) == 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported string format flags: 0x%08" PRIx32 ".",
		 function,
		 string_format_flags );

		return( -1 );
	}
	string_format_type = string_format_flags & 0x000000ffUL;

	if( ( string_format_type != LIBFDATETIME_STRING_FORMAT_TYPE_CTIME )
	 && ( string_format_type != LIBFDATETIME_STRING_FORMAT_TYPE_ISO8601 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported string format type: 0x%08" PRIx32 ".",
		 function,
		 string_format_type );

		return( -1 );
	}
	string_index = *utf8_string_index;

	/* Validate the date and time if necessary
	 */
	if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_DATE ) != 0 )
	{
		if( date_time_values->year > 9999 )
		{
			return( 0 );
		}
		if( ( date_time_values->month == 0 )
		 || ( date_time_values->month > 12 ) )
		{
			return( 0 );
		}
		switch( date_time_values->month )
		{
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
			case 12:
				days_in_month = 31;
				break;
			case 4:
			case 6:
			case 9:
			case 11:
				days_in_month = 30;
				break;
			case 2:
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
				break;
		}
		if( ( date_time_values->day == 0 )
		 || ( date_time_values->day > days_in_month ) )
		{
			return( 0 );
		}
	}
	if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME ) != 0 )
	{
		if( date_time_values->hours > 23 )
		{
			return( 0 );
		}
		if( date_time_values->minutes > 59 )
		{
			return( 0 );
		}
		if( date_time_values->seconds > 59 )
		{
			return( 0 );
		}
		if( ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MILLI_SECONDS ) != 0 )
		 || ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MICRO_SECONDS ) != 0 )
		 || ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_NANO_SECONDS ) != 0 ) )
		{
			if( date_time_values->milli_seconds > 999 )
			{
				return( 0 );
			}
		}
		if( ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MICRO_SECONDS ) != 0 )
		 || ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_NANO_SECONDS ) != 0 ) )
		{
			if( date_time_values->micro_seconds > 999 )
			{
				return( 0 );
			}
		}
		if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_NANO_SECONDS ) != 0 )
		{
			if( date_time_values->nano_seconds > 999 )
			{
				return( 0 );
			}
		}
	}
	/* Create the date and time string
	 */
	if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_DATE ) != 0 )
	{
		if( string_format_type == LIBFDATETIME_STRING_FORMAT_TYPE_CTIME )
		{
			switch( date_time_values->month )
			{
				case 1:
					month_string = "Jan";
					break;
				case 2:
					month_string = "Feb";
					break;
				case 3:
					month_string = "Mar";
					break;
				case 4:
					month_string = "Apr";
					break;
				case 5:
					month_string = "May";
					break;
				case 6:
					month_string = "Jun";
					break;
				case 7:
					month_string = "Jul";
					break;
				case 8:
					month_string = "Aug";
					break;
				case 9:
					month_string = "Sep";
					break;
				case 10:
					month_string = "Oct";
					break;
				case 11:
					month_string = "Nov";
					break;
				case 12:
					month_string = "Dec";
					break;
			}
			if( ( string_index + 12 ) > utf8_string_size )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: UTF-8 string is too small.",
				 function );

				return( -1 );
			}
			if( month_string == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
				 "%s: missing month string.",
				 function );

				return( -1 );
			}
			/* Format: mmm dd, yyyy */
			utf8_string[ string_index++ ] = (uint8_t) month_string[ 0 ];
			utf8_string[ string_index++ ] = (uint8_t) month_string[ 1 ];
			utf8_string[ string_index++ ] = (uint8_t) month_string[ 2 ];

			utf8_string[ string_index++ ] = (uint8_t) ' ';

			utf8_string[ string_index++ ] = (uint8_t) '0' + ( date_time_values->day / 10 );
			utf8_string[ string_index++ ] = (uint8_t) '0' + ( date_time_values->day % 10 );

			utf8_string[ string_index++ ] = (uint8_t) ',';
			utf8_string[ string_index++ ] = (uint8_t) ' ';

			year_value                    = date_time_values->year;
			utf8_string[ string_index++ ] = (uint8_t) '0' + (uint8_t) ( year_value / 1000 );
			year_value                   %= 1000;
			utf8_string[ string_index++ ] = (uint8_t) '0' + (uint8_t) ( year_value / 100 );
			year_value                   %= 100;
			utf8_string[ string_index++ ] = (uint8_t) '0' + (uint8_t) ( year_value / 10 );
			year_value                   %= 10;
			utf8_string[ string_index++ ] = (uint8_t) '0' + (uint8_t) year_value;

			if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME ) != 0 )
			{
				if( ( string_index + 1 ) > utf8_string_size )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
					 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
					 "%s: UTF-8 string is too small.",
					 function );

					return( -1 );
				}
				utf8_string[ string_index++ ] = (uint8_t) ' ';
			}
		}
		else if( string_format_type == LIBFDATETIME_STRING_FORMAT_TYPE_ISO8601 )
		{
			if( ( string_index + 10 ) > utf8_string_size )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: UTF-8 string is too small.",
				 function );

				return( -1 );
			}
			/* Format: yyyy-mm-dd */
			year_value                    = date_time_values->year;
			utf8_string[ string_index++ ] = (uint8_t) '0' + (uint8_t) ( year_value / 1000 );
			year_value                   %= 1000;
			utf8_string[ string_index++ ] = (uint8_t) '0' + (uint8_t) ( year_value / 100 );
			year_value                   %= 100;
			utf8_string[ string_index++ ] = (uint8_t) '0' + (uint8_t) ( year_value / 10 );
			year_value                   %= 10;
			utf8_string[ string_index++ ] = (uint8_t) '0' + (uint8_t) year_value;

			utf8_string[ string_index++ ] = (uint8_t) '-';

			utf8_string[ string_index++ ] = (uint8_t) '0' + ( date_time_values->month / 10 );
			utf8_string[ string_index++ ] = (uint8_t) '0' + ( date_time_values->month % 10 );

			utf8_string[ string_index++ ] = (uint8_t) '-';

			utf8_string[ string_index++ ] = (uint8_t) '0' + ( date_time_values->day / 10 );
			utf8_string[ string_index++ ] = (uint8_t) '0' + ( date_time_values->day % 10 );

			if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME ) != 0 )
			{
				if( ( string_index + 1 ) > utf8_string_size )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
					 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
					 "%s: UTF-8 string is too small.",
					 function );

					return( -1 );
				}
				utf8_string[ string_index++ ] = (uint8_t) 'T';
			}
		}
	}
	if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME ) != 0 )
	{
		if( ( string_index + 8 ) > utf8_string_size )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
			 "%s: UTF-8 string is too small.",
			 function );

			return( -1 );
		}
		/* Format: HH:MM:SS */
		utf8_string[ string_index++ ] = (uint8_t) '0' + ( date_time_values->hours / 10 );
		utf8_string[ string_index++ ] = (uint8_t) '0' + ( date_time_values->hours % 10 );

		utf8_string[ string_index++ ] = (uint8_t) ':';

		utf8_string[ string_index++ ] = (uint8_t) '0' + ( date_time_values->minutes / 10 );
		utf8_string[ string_index++ ] = (uint8_t) '0' + ( date_time_values->minutes % 10 );

		utf8_string[ string_index++ ] = (uint8_t) ':';

		utf8_string[ string_index++ ] = (uint8_t) '0' + ( date_time_values->seconds / 10 );
		utf8_string[ string_index++ ] = (uint8_t) '0' + ( date_time_values->seconds % 10 );

		if( ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MILLI_SECONDS ) != 0 )
		 || ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MICRO_SECONDS ) != 0 )
		 || ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_NANO_SECONDS ) != 0 ) )
		{
			if( ( string_index + 4 ) > utf8_string_size )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: UTF-8 string is too small.",
				 function );

				return( -1 );
			}
			/* Format: .### */
			utf8_string[ string_index++ ] = (uint8_t) '.';

			milli_seconds = date_time_values->milli_seconds;

			utf8_string[ string_index + 2 ] = (uint8_t) '0' + (uint8_t) ( milli_seconds % 10 );
			milli_seconds                  /= 10;

			utf8_string[ string_index + 1 ] = (uint8_t) '0' + (uint8_t) ( milli_seconds % 10 );
			milli_seconds                  /= 10;

			utf8_string[ string_index ] = (uint8_t) '0' + (uint8_t) ( milli_seconds % 10 );

			string_index += 3;
		}
		if( ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MICRO_SECONDS ) != 0 )
		 || ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_NANO_SECONDS ) != 0 ) )
		{
			if( ( string_index + 3 ) > utf8_string_size )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: UTF-8 string is too small.",
				 function );

				return( -1 );
			}
			/* Format: ### */
			micro_seconds = date_time_values->micro_seconds;

			utf8_string[ string_index + 2 ] = (uint8_t) '0' + (uint8_t) ( micro_seconds % 10 );
			micro_seconds                  /= 10;

			utf8_string[ string_index + 1 ] = (uint8_t) '0' + (uint8_t) ( micro_seconds % 10 );
			micro_seconds                  /= 10;

			utf8_string[ string_index ] = (uint8_t) '0' + (uint8_t) ( micro_seconds % 10 );

			string_index += 3;
		}
		if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_NANO_SECONDS ) != 0 )
		{
			if( ( string_index + 3 ) > utf8_string_size )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: UTF-8 string is too small.",
				 function );

				return( -1 );
			}
			/* Format: ### */
			nano_seconds = date_time_values->nano_seconds;

			utf8_string[ string_index + 2 ] = (uint8_t) '0' + (uint8_t) ( nano_seconds % 10 );
			nano_seconds                   /= 10;

			utf8_string[ string_index + 1 ] = (uint8_t) '0' + (uint8_t) ( nano_seconds % 10 );
			nano_seconds                   /= 10;

			utf8_string[ string_index ] = (uint8_t) '0' + (uint8_t) ( nano_seconds % 10 );

			string_index += 3;
		}
	}
	if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIMEZONE_INDICATOR ) != 0 )
	{
		if( string_format_type == LIBFDATETIME_STRING_FORMAT_TYPE_CTIME )
		{
			if( ( string_index + 4 ) > utf8_string_size )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: UTF-8 string is too small.",
				 function );

				return( -1 );
			}
			utf8_string[ string_index++ ] = (uint8_t) ' ';
			utf8_string[ string_index++ ] = (uint8_t) 'U';
			utf8_string[ string_index++ ] = (uint8_t) 'T';
			utf8_string[ string_index++ ] = (uint8_t) 'C';
		}
		else if( string_format_type == LIBFDATETIME_STRING_FORMAT_TYPE_ISO8601 )
		{
			if( ( string_index + 1 ) > utf8_string_size )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: UTF-8 string is too small.",
				 function );

				return( -1 );
			}
			utf8_string[ string_index++ ] = (uint8_t) 'Z';
		}
	}
	if( ( string_index + 1 ) > utf8_string_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-8 string is too small.",
		 function );

		return( -1 );
	}
	utf8_string[ string_index++ ] = 0;

	*utf8_string_index = string_index;

	return( 1 );
}

/* Converts the date and time values into an UTF-16 string
 * The string size should include the end of string character
 * Returns 1 if successful, 0 if the date and time values are not valid or -1 on error
 */
int libfdatetime_date_time_values_copy_to_utf16_string_with_index(
     libfdatetime_date_time_values_t *date_time_values,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     size_t *utf16_string_index,
     uint32_t string_format_flags,
     libcerror_error_t **error )
{
	char *month_string          = NULL;
	static char *function       = "libfdatetime_date_time_values_copy_to_utf16_string_with_index";
	size_t string_index         = 0;
	uint32_t string_format_type = 0;
	uint32_t supported_flags    = 0;
	uint16_t micro_seconds      = 0;
	uint16_t milli_seconds      = 0;
	uint16_t nano_seconds       = 0;
	uint16_t year_value         = 0;
	uint8_t days_in_month       = 0;

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
	supported_flags = 0x000000ffUL
	                | LIBFDATETIME_STRING_FORMAT_FLAG_DATE
	                | LIBFDATETIME_STRING_FORMAT_FLAG_TIME
	                | LIBFDATETIME_STRING_FORMAT_FLAG_DURATION
	                | LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MILLI_SECONDS
	                | LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MICRO_SECONDS
	                | LIBFDATETIME_STRING_FORMAT_FLAG_TIME_NANO_SECONDS
	                | LIBFDATETIME_STRING_FORMAT_FLAG_TIMEZONE_INDICATOR;

	if( ( string_format_flags & supported_flags ) == 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported string format flags: 0x%08" PRIx32 ".",
		 function,
		 string_format_flags );

		return( -1 );
	}
	string_format_type = string_format_flags & 0x000000ffUL;

	if( ( string_format_type != LIBFDATETIME_STRING_FORMAT_TYPE_CTIME )
	 && ( string_format_type != LIBFDATETIME_STRING_FORMAT_TYPE_ISO8601 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported string format type: 0x%08" PRIx32 ".",
		 function,
		 string_format_type );

		return( -1 );
	}
	string_index = *utf16_string_index;

	/* Validate the date and time if necessary
	 */
	if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_DATE ) != 0 )
	{
		if( date_time_values->year > 9999 )
		{
			return( 0 );
		}
		if( ( date_time_values->month == 0 )
		 || ( date_time_values->month > 12 ) )
		{
			return( 0 );
		}
		switch( date_time_values->month )
		{
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
			case 12:
				days_in_month = 31;
				break;
			case 4:
			case 6:
			case 9:
			case 11:
				days_in_month = 30;
				break;
			case 2:
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
				break;
		}
		if( ( date_time_values->day == 0 )
		 || ( date_time_values->day > days_in_month ) )
		{
			return( 0 );
		}
	}
	if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME ) != 0 )
	{
		if( date_time_values->hours > 23 )
		{
			return( 0 );
		}
		if( date_time_values->minutes > 59 )
		{
			return( 0 );
		}
		if( date_time_values->seconds > 59 )
		{
			return( 0 );
		}
		if( ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MILLI_SECONDS ) != 0 )
		 || ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MICRO_SECONDS ) != 0 )
		 || ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_NANO_SECONDS ) != 0 ) )
		{
			if( date_time_values->milli_seconds > 999 )
			{
				return( 0 );
			}
		}
		if( ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MICRO_SECONDS ) != 0 )
		 || ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_NANO_SECONDS ) != 0 ) )
		{
			if( date_time_values->micro_seconds > 999 )
			{
				return( 0 );
			}
		}
		if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_NANO_SECONDS ) != 0 )
		{
			if( date_time_values->nano_seconds > 999 )
			{
				return( 0 );
			}
		}
	}
	/* Create the date and time string
	 */
	if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_DATE ) != 0 )
	{
		if( string_format_type == LIBFDATETIME_STRING_FORMAT_TYPE_CTIME )
		{
			switch( date_time_values->month )
			{
				case 1:
					month_string = "Jan";
					break;
				case 2:
					month_string = "Feb";
					break;
				case 3:
					month_string = "Mar";
					break;
				case 4:
					month_string = "Apr";
					break;
				case 5:
					month_string = "May";
					break;
				case 6:
					month_string = "Jun";
					break;
				case 7:
					month_string = "Jul";
					break;
				case 8:
					month_string = "Aug";
					break;
				case 9:
					month_string = "Sep";
					break;
				case 10:
					month_string = "Oct";
					break;
				case 11:
					month_string = "Nov";
					break;
				case 12:
					month_string = "Dec";
					break;
			}
			if( ( string_index + 12 ) > utf16_string_size )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: UTF-16 string is too small.",
				 function );

				return( -1 );
			}
			if( month_string == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
				 "%s: missing month string.",
				 function );

				return( -1 );
			}
			/* Format: mmm dd, yyyy */
			utf16_string[ string_index++ ] = (uint16_t) month_string[ 0 ];
			utf16_string[ string_index++ ] = (uint16_t) month_string[ 1 ];
			utf16_string[ string_index++ ] = (uint16_t) month_string[ 2 ];

			utf16_string[ string_index++ ] = (uint16_t) ' ';

			utf16_string[ string_index++ ] = (uint16_t) '0' + ( date_time_values->day / 10 );
			utf16_string[ string_index++ ] = (uint16_t) '0' + ( date_time_values->day % 10 );

			utf16_string[ string_index++ ] = (uint16_t) ',';
			utf16_string[ string_index++ ] = (uint16_t) ' ';

			year_value                     = date_time_values->year;
			utf16_string[ string_index++ ] = (uint16_t) '0' + ( year_value / 1000 );
			year_value                    %= 1000;
			utf16_string[ string_index++ ] = (uint16_t) '0' + ( year_value / 100 );
			year_value                    %= 100;
			utf16_string[ string_index++ ] = (uint16_t) '0' + ( year_value / 10 );
			year_value                    %= 10;
			utf16_string[ string_index++ ] = (uint16_t) '0' + year_value;

			if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME ) != 0 )
			{
				if( ( string_index + 1 ) > utf16_string_size )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
					 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
					 "%s: UTF-16 string is too small.",
					 function );

					return( -1 );
				}
				utf16_string[ string_index++ ] = (uint16_t) ' ';
			}
		}
		else if( string_format_type == LIBFDATETIME_STRING_FORMAT_TYPE_ISO8601 )
		{
			if( ( string_index + 10 ) > utf16_string_size )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: UTF-16 string is too small.",
				 function );

				return( -1 );
			}
			/* Format: yyyy-mm-dd */
			year_value                     = date_time_values->year;
			utf16_string[ string_index++ ] = (uint16_t) '0' + ( year_value / 1000 );
			year_value                    %= 1000;
			utf16_string[ string_index++ ] = (uint16_t) '0' + ( year_value / 100 );
			year_value                    %= 100;
			utf16_string[ string_index++ ] = (uint16_t) '0' + ( year_value / 10 );
			year_value                    %= 10;
			utf16_string[ string_index++ ] = (uint16_t) '0' + year_value;

			utf16_string[ string_index++ ] = (uint16_t) '-';

			utf16_string[ string_index++ ] = (uint16_t) '0' + ( date_time_values->month / 10 );
			utf16_string[ string_index++ ] = (uint16_t) '0' + ( date_time_values->month % 10 );

			utf16_string[ string_index++ ] = (uint16_t) '-';

			utf16_string[ string_index++ ] = (uint16_t) '0' + ( date_time_values->day / 10 );
			utf16_string[ string_index++ ] = (uint16_t) '0' + ( date_time_values->day % 10 );

			if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME ) != 0 )
			{
				if( ( string_index + 1 ) > utf16_string_size )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
					 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
					 "%s: UTF-16 string is too small.",
					 function );

					return( -1 );
				}
				utf16_string[ string_index++ ] = (uint16_t) 'T';
			}
		}
	}
	if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME ) != 0 )
	{
		if( ( string_index + 8 ) > utf16_string_size )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
			 "%s: UTF-16 string is too small.",
			 function );

			return( -1 );
		}
		/* Format: HH:MM:SS */
		utf16_string[ string_index++ ] = (uint16_t) '0' + ( date_time_values->hours / 10 );
		utf16_string[ string_index++ ] = (uint16_t) '0' + ( date_time_values->hours % 10 );

		utf16_string[ string_index++ ] = (uint16_t) ':';

		utf16_string[ string_index++ ] = (uint16_t) '0' + ( date_time_values->minutes / 10 );
		utf16_string[ string_index++ ] = (uint16_t) '0' + ( date_time_values->minutes % 10 );

		utf16_string[ string_index++ ] = (uint16_t) ':';

		utf16_string[ string_index++ ] = (uint16_t) '0' + ( date_time_values->seconds / 10 );
		utf16_string[ string_index++ ] = (uint16_t) '0' + ( date_time_values->seconds % 10 );

		if( ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MILLI_SECONDS ) != 0 )
		 || ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MICRO_SECONDS ) != 0 )
		 || ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_NANO_SECONDS ) != 0 ) )
		{
			if( ( string_index + 4 ) > utf16_string_size )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: UTF-16 string is too small.",
				 function );

				return( -1 );
			}
			/* Format: .### */
			utf16_string[ string_index++ ] = (uint16_t) '.';

			milli_seconds = date_time_values->milli_seconds;

			utf16_string[ string_index + 2 ] = (uint16_t) '0' + (uint16_t) ( milli_seconds % 10 );
			milli_seconds                   /= 10;

			utf16_string[ string_index + 1 ] = (uint16_t) '0' + (uint16_t) ( milli_seconds % 10 );
			milli_seconds                   /= 10;

			utf16_string[ string_index ] = (uint16_t) '0' + (uint16_t) ( milli_seconds % 10 );

			string_index += 3;
		}
		if( ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MICRO_SECONDS ) != 0 )
		 || ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_NANO_SECONDS ) != 0 ) )
		{
			if( ( string_index + 3 ) > utf16_string_size )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: UTF-16 string is too small.",
				 function );

				return( -1 );
			}
			/* Format: ### */
			micro_seconds = date_time_values->micro_seconds;

			utf16_string[ string_index + 2 ] = (uint16_t) '0' + (uint16_t) ( micro_seconds % 10 );
			micro_seconds                   /= 10;

			utf16_string[ string_index + 1 ] = (uint16_t) '0' + (uint16_t) ( micro_seconds % 10 );
			micro_seconds                   /= 10;

			utf16_string[ string_index ] = (uint16_t) '0' + (uint16_t) ( micro_seconds % 10 );

			string_index += 3;
		}
		if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_NANO_SECONDS ) != 0 )
		{
			if( ( string_index + 3 ) > utf16_string_size )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: UTF-16 string is too small.",
				 function );

				return( -1 );
			}
			/* Format: ### */
			nano_seconds = date_time_values->nano_seconds;

			utf16_string[ string_index + 2 ] = (uint16_t) '0' + (uint16_t) ( nano_seconds % 10 );
			nano_seconds                    /= 10;

			utf16_string[ string_index + 1 ] = (uint16_t) '0' + (uint16_t) ( nano_seconds % 10 );
			nano_seconds                    /= 10;

			utf16_string[ string_index ] = (uint16_t) '0' + (uint16_t) ( nano_seconds % 10 );

			string_index += 3;
		}
	}
	if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIMEZONE_INDICATOR ) != 0 )
	{
		if( string_format_type == LIBFDATETIME_STRING_FORMAT_TYPE_CTIME )
		{
			if( ( string_index + 4 ) > utf16_string_size )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: UTF-16 string is too small.",
				 function );

				return( -1 );
			}
			utf16_string[ string_index++ ] = (uint16_t) ' ';
			utf16_string[ string_index++ ] = (uint16_t) 'U';
			utf16_string[ string_index++ ] = (uint16_t) 'T';
			utf16_string[ string_index++ ] = (uint16_t) 'C';
		}
		else if( string_format_type == LIBFDATETIME_STRING_FORMAT_TYPE_ISO8601 )
		{
			if( ( string_index + 1 ) > utf16_string_size )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: UTF-16 string is too small.",
				 function );

				return( -1 );
			}
			utf16_string[ string_index++ ] = (uint16_t) 'Z';
		}
	}
	if( ( string_index + 1 ) > utf16_string_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-16 string is too small.",
		 function );

		return( -1 );
	}
	utf16_string[ string_index++ ] = 0;

	*utf16_string_index = string_index;

	return( 1 );
}

/* Converts the date and time values into an UTF-32 string
 * The string size should include the end of string character
 * Returns 1 if successful, 0 if the date and time values are not valid or -1 on error
 */
int libfdatetime_date_time_values_copy_to_utf32_string_with_index(
     libfdatetime_date_time_values_t *date_time_values,
     uint32_t *utf32_string,
     size_t utf32_string_size,
     size_t *utf32_string_index,
     uint32_t string_format_flags,
     libcerror_error_t **error )
{
	char *month_string          = NULL;
	static char *function       = "libfdatetime_date_time_values_copy_to_utf32_string_with_index";
	size_t string_index         = 0;
	uint32_t string_format_type = 0;
	uint32_t supported_flags    = 0;
	uint16_t micro_seconds      = 0;
	uint16_t milli_seconds      = 0;
	uint16_t nano_seconds       = 0;
	uint16_t year_value         = 0;
	uint8_t days_in_month       = 0;

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
	supported_flags = 0x000000ffUL
	                | LIBFDATETIME_STRING_FORMAT_FLAG_DATE
	                | LIBFDATETIME_STRING_FORMAT_FLAG_TIME
	                | LIBFDATETIME_STRING_FORMAT_FLAG_DURATION
	                | LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MILLI_SECONDS
	                | LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MICRO_SECONDS
	                | LIBFDATETIME_STRING_FORMAT_FLAG_TIME_NANO_SECONDS
	                | LIBFDATETIME_STRING_FORMAT_FLAG_TIMEZONE_INDICATOR;

	if( ( string_format_flags & supported_flags ) == 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported string format flags: 0x%08" PRIx32 ".",
		 function,
		 string_format_flags );

		return( -1 );
	}
	string_format_type = string_format_flags & 0x000000ffUL;

	if( ( string_format_type != LIBFDATETIME_STRING_FORMAT_TYPE_CTIME )
	 && ( string_format_type != LIBFDATETIME_STRING_FORMAT_TYPE_ISO8601 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported string format type: 0x%08" PRIx32 ".",
		 function,
		 string_format_type );

		return( -1 );
	}
	string_index = *utf32_string_index;

	/* Validate the date and time if necessary
	 */
	if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_DATE ) != 0 )
	{
		if( date_time_values->year > 9999 )
		{
			return( 0 );
		}
		if( ( date_time_values->month == 0 )
		 || ( date_time_values->month > 12 ) )
		{
			return( 0 );
		}
		switch( date_time_values->month )
		{
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
			case 12:
				days_in_month = 31;
				break;
			case 4:
			case 6:
			case 9:
			case 11:
				days_in_month = 30;
				break;
			case 2:
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
				break;
		}
		if( ( date_time_values->day == 0 )
		 || ( date_time_values->day > days_in_month ) )
		{
			return( 0 );
		}
	}
	if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME ) != 0 )
	{
		if( date_time_values->hours > 23 )
		{
			return( 0 );
		}
		if( date_time_values->minutes > 59 )
		{
			return( 0 );
		}
		if( date_time_values->seconds > 59 )
		{
			return( 0 );
		}
		if( ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MILLI_SECONDS ) != 0 )
		 || ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MICRO_SECONDS ) != 0 )
		 || ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_NANO_SECONDS ) != 0 ) )
		{
			if( date_time_values->milli_seconds > 999 )
			{
				return( 0 );
			}
		}
		if( ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MICRO_SECONDS ) != 0 )
		 || ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_NANO_SECONDS ) != 0 ) )
		{
			if( date_time_values->micro_seconds > 999 )
			{
				return( 0 );
			}
		}
		if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_NANO_SECONDS ) != 0 )
		{
			if( date_time_values->nano_seconds > 999 )
			{
				return( 0 );
			}
		}
	}
	/* Create the date and time string
	 */
	if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_DATE ) != 0 )
	{
		if( string_format_type == LIBFDATETIME_STRING_FORMAT_TYPE_CTIME )
		{
			switch( date_time_values->month )
			{
				case 1:
					month_string = "Jan";
					break;
				case 2:
					month_string = "Feb";
					break;
				case 3:
					month_string = "Mar";
					break;
				case 4:
					month_string = "Apr";
					break;
				case 5:
					month_string = "May";
					break;
				case 6:
					month_string = "Jun";
					break;
				case 7:
					month_string = "Jul";
					break;
				case 8:
					month_string = "Aug";
					break;
				case 9:
					month_string = "Sep";
					break;
				case 10:
					month_string = "Oct";
					break;
				case 11:
					month_string = "Nov";
					break;
				case 12:
					month_string = "Dec";
					break;
			}
			if( ( string_index + 12 ) > utf32_string_size )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: UTF-32 string is too small.",
				 function );

				return( -1 );
			}
			if( month_string == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
				 "%s: missing month string.",
				 function );

				return( -1 );
			}
			/* Format: mmm dd, yyyy */
			utf32_string[ string_index++ ] = (uint32_t) month_string[ 0 ];
			utf32_string[ string_index++ ] = (uint32_t) month_string[ 1 ];
			utf32_string[ string_index++ ] = (uint32_t) month_string[ 2 ];

			utf32_string[ string_index++ ] = (uint32_t) ' ';

			utf32_string[ string_index++ ] = (uint32_t) '0' + ( date_time_values->day / 10 );
			utf32_string[ string_index++ ] = (uint32_t) '0' + ( date_time_values->day % 10 );

			utf32_string[ string_index++ ] = (uint32_t) ',';
			utf32_string[ string_index++ ] = (uint32_t) ' ';

			year_value                     = date_time_values->year;
			utf32_string[ string_index++ ] = (uint32_t) '0' + ( year_value / 1000 );
			year_value                    %= 1000;
			utf32_string[ string_index++ ] = (uint32_t) '0' + ( year_value / 100 );
			year_value                    %= 100;
			utf32_string[ string_index++ ] = (uint32_t) '0' + ( year_value / 10 );
			year_value                    %= 10;
			utf32_string[ string_index++ ] = (uint32_t) '0' + year_value;

			if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME ) != 0 )
			{
				if( ( string_index + 1 ) > utf32_string_size )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
					 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
					 "%s: UTF-32 string is too small.",
					 function );

					return( -1 );
				}
				utf32_string[ string_index++ ] = (uint32_t) ' ';
			}
		}
		else if( string_format_type == LIBFDATETIME_STRING_FORMAT_TYPE_ISO8601 )
		{
			if( ( string_index + 10 ) > utf32_string_size )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: UTF-32 string is too small.",
				 function );

				return( -1 );
			}
			/* Format: yyyy-mm-dd */
			year_value                     = date_time_values->year;
			utf32_string[ string_index++ ] = (uint32_t) '0' + ( year_value / 1000 );
			year_value                    %= 1000;
			utf32_string[ string_index++ ] = (uint32_t) '0' + ( year_value / 100 );
			year_value                    %= 100;
			utf32_string[ string_index++ ] = (uint32_t) '0' + ( year_value / 10 );
			year_value                    %= 10;
			utf32_string[ string_index++ ] = (uint32_t) '0' + year_value;

			utf32_string[ string_index++ ] = (uint32_t) '-';

			utf32_string[ string_index++ ] = (uint32_t) '0' + ( date_time_values->month / 10 );
			utf32_string[ string_index++ ] = (uint32_t) '0' + ( date_time_values->month % 10 );

			utf32_string[ string_index++ ] = (uint32_t) '-';

			utf32_string[ string_index++ ] = (uint32_t) '0' + ( date_time_values->day / 10 );
			utf32_string[ string_index++ ] = (uint32_t) '0' + ( date_time_values->day % 10 );

			if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME ) != 0 )
			{
				if( ( string_index + 1 ) > utf32_string_size )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
					 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
					 "%s: UTF-32 string is too small.",
					 function );

					return( -1 );
				}
				utf32_string[ string_index++ ] = (uint32_t) 'T';
			}
		}
	}
	if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME ) != 0 )
	{
		if( ( string_index + 8 ) > utf32_string_size )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
			 "%s: UTF-32 string is too small.",
			 function );

			return( -1 );
		}
		/* Format: HH:MM:SS */
		utf32_string[ string_index++ ] = (uint32_t) '0' + ( date_time_values->hours / 10 );
		utf32_string[ string_index++ ] = (uint32_t) '0' + ( date_time_values->hours % 10 );

		utf32_string[ string_index++ ] = (uint32_t) ':';

		utf32_string[ string_index++ ] = (uint32_t) '0' + ( date_time_values->minutes / 10 );
		utf32_string[ string_index++ ] = (uint32_t) '0' + ( date_time_values->minutes % 10 );

		utf32_string[ string_index++ ] = (uint32_t) ':';

		utf32_string[ string_index++ ] = (uint32_t) '0' + ( date_time_values->seconds / 10 );
		utf32_string[ string_index++ ] = (uint32_t) '0' + ( date_time_values->seconds % 10 );

		if( ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MILLI_SECONDS ) != 0 )
		 || ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MICRO_SECONDS ) != 0 )
		 || ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_NANO_SECONDS ) != 0 ) )
		{
			if( ( string_index + 4 ) > utf32_string_size )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: UTF-32 string is too small.",
				 function );

				return( -1 );
			}
			/* Format: .### */
			utf32_string[ string_index++ ] = (uint32_t) '.';

			milli_seconds = date_time_values->milli_seconds;

			utf32_string[ string_index + 2 ] = (uint32_t) '0' + (uint32_t) ( milli_seconds % 10 );
			milli_seconds                   /= 10;

			utf32_string[ string_index + 1 ] = (uint32_t) '0' + (uint32_t) ( milli_seconds % 10 );
			milli_seconds                   /= 10;

			utf32_string[ string_index ] = (uint32_t) '0' + (uint32_t) ( milli_seconds % 10 );

			string_index += 3;
		}
		if( ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MICRO_SECONDS ) != 0 )
		 || ( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_NANO_SECONDS ) != 0 ) )
		{
			if( ( string_index + 3 ) > utf32_string_size )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: UTF-32 string is too small.",
				 function );

				return( -1 );
			}
			/* Format: ### */
			micro_seconds = date_time_values->micro_seconds;

			utf32_string[ string_index + 2 ] = (uint32_t) '0' + (uint32_t) ( micro_seconds % 10 );
			micro_seconds                   /= 10;

			utf32_string[ string_index + 1 ] = (uint32_t) '0' + (uint32_t) ( micro_seconds % 10 );
			micro_seconds                   /= 10;

			utf32_string[ string_index ] = (uint32_t) '0' + (uint32_t) ( micro_seconds % 10 );

			string_index += 3;
		}
		if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIME_NANO_SECONDS ) != 0 )
		{
			if( ( string_index + 3 ) > utf32_string_size )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: UTF-32 string is too small.",
				 function );

				return( -1 );
			}
			/* Format: ### */
			nano_seconds = date_time_values->nano_seconds;

			utf32_string[ string_index + 2 ] = (uint32_t) '0' + (uint32_t) ( nano_seconds % 10 );
			nano_seconds                    /= 10;

			utf32_string[ string_index + 1 ] = (uint32_t) '0' + (uint32_t) ( nano_seconds % 10 );
			nano_seconds                    /= 10;

			utf32_string[ string_index ] = (uint32_t) '0' + (uint32_t) ( nano_seconds % 10 );

			string_index += 3;
		}
	}
	if( ( string_format_flags & LIBFDATETIME_STRING_FORMAT_FLAG_TIMEZONE_INDICATOR ) != 0 )
	{
		if( string_format_type == LIBFDATETIME_STRING_FORMAT_TYPE_CTIME )
		{
			if( ( string_index + 4 ) > utf32_string_size )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: UTF-32 string is too small.",
				 function );

				return( -1 );
			}
			utf32_string[ string_index++ ] = (uint32_t) ' ';
			utf32_string[ string_index++ ] = (uint32_t) 'U';
			utf32_string[ string_index++ ] = (uint32_t) 'T';
			utf32_string[ string_index++ ] = (uint32_t) 'C';
		}
		else if( string_format_type == LIBFDATETIME_STRING_FORMAT_TYPE_ISO8601 )
		{
			if( ( string_index + 1 ) > utf32_string_size )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: UTF-32 string is too small.",
				 function );

				return( -1 );
			}
			utf32_string[ string_index++ ] = (uint32_t) 'Z';
		}
	}
	if( ( string_index + 1 ) > utf32_string_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-32 string is too small.",
		 function );

		return( -1 );
	}
	utf32_string[ string_index++ ] = 0;

	*utf32_string_index = string_index;

	return( 1 );
}

