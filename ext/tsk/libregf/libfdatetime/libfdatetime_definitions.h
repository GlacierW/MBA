/*
 * The internal definitions
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

#if !defined( LIBFDATETIME_INTERNAL_DEFINITIONS_H )
#define LIBFDATETIME_INTERNAL_DEFINITIONS_H

#include <common.h>
#include <types.h>

/* Define HAVE_LOCAL_LIBFDATETIME for local use of libfdatetime
 */
#if !defined( HAVE_LOCAL_LIBFDATETIME )
#include <libfdatetime/definitions.h>

/* The definitions in <libfdatetime/definitions.h> are copied here
 * for local use of libfdatetime
 */
#else
#include <byte_stream.h>

#define LIBFDATETIME_VERSION					20161125

/* The version string
 */
#define LIBFDATETIME_VERSION_STRING				"20161125"

/* The byte order definitions
 */
#define LIBFDATETIME_ENDIAN_BIG					_BYTE_STREAM_ENDIAN_BIG
#define LIBFDATETIME_ENDIAN_LITTLE				_BYTE_STREAM_ENDIAN_LITTLE

/* The string format definition flags
 */
enum LIBFDATETIME_STRING_FORMAT_FLAGS
{
	/* Format the date time value in C time
	 */
	LIBFDATETIME_STRING_FORMAT_TYPE_CTIME			= 0x00000001UL,

	/* Format the date time value in ISO 8601
	 */
	LIBFDATETIME_STRING_FORMAT_TYPE_ISO8601			= 0x00000002UL,

	/* Format the date time value as a date only
	 */
	LIBFDATETIME_STRING_FORMAT_FLAG_DATE			= 0x00000100UL,

	/* Format the date time value as a time only
	 */
	LIBFDATETIME_STRING_FORMAT_FLAG_TIME			= 0x00000200UL,

	/* Format the date time value as a duration
	 */
	LIBFDATETIME_STRING_FORMAT_FLAG_DURATION		= 0x00000400UL,

	/* Format the date time value with a time in milli seconds
	 */
	LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MILLI_SECONDS	= 0x00010000UL,

	/* Format the date time value with a time in micro seconds
	 */
	LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MICRO_SECONDS	= 0x00020000UL,

	/* Format the date time value with a time in nano seconds
	 */
	LIBFDATETIME_STRING_FORMAT_FLAG_TIME_NANO_SECONDS	= 0x00040000UL,

	/* Add a timezone indicator
	 */
	LIBFDATETIME_STRING_FORMAT_FLAG_TIMEZONE_INDICATOR	= 0x80000000UL,
};

#define LIBFDATETIME_STRING_FORMAT_FLAG_DATE_TIME \
	( LIBFDATETIME_STRING_FORMAT_FLAG_DATE | LIBFDATETIME_STRING_FORMAT_FLAG_TIME )

#define LIBFDATETIME_STRING_FORMAT_FLAG_DATE_TIME_MILLI_SECONDS \
	( LIBFDATETIME_STRING_FORMAT_FLAG_DATE | LIBFDATETIME_STRING_FORMAT_FLAG_TIME | LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MILLI_SECONDS )

#define LIBFDATETIME_STRING_FORMAT_FLAG_DATE_TIME_MICRO_SECONDS \
	( LIBFDATETIME_STRING_FORMAT_FLAG_DATE | LIBFDATETIME_STRING_FORMAT_FLAG_TIME | LIBFDATETIME_STRING_FORMAT_FLAG_TIME_MICRO_SECONDS )

#define LIBFDATETIME_STRING_FORMAT_FLAG_DATE_TIME_NANO_SECONDS \
	( LIBFDATETIME_STRING_FORMAT_FLAG_DATE | LIBFDATETIME_STRING_FORMAT_FLAG_TIME | LIBFDATETIME_STRING_FORMAT_FLAG_TIME_NANO_SECONDS )

/* The POSIX time value type definitions
 */
enum LIBFDATETIME_POSIX_TIME_VALUE_TYPES
{
        LIBFDATETIME_POSIX_TIME_VALUE_TYPE_UNDEFINED,
        LIBFDATETIME_POSIX_TIME_VALUE_TYPE_SECONDS_32BIT_SIGNED,
        LIBFDATETIME_POSIX_TIME_VALUE_TYPE_SECONDS_32BIT_UNSIGNED,
        LIBFDATETIME_POSIX_TIME_VALUE_TYPE_SECONDS_64BIT_SIGNED,
        LIBFDATETIME_POSIX_TIME_VALUE_TYPE_SECONDS_64BIT_UNSIGNED,
        LIBFDATETIME_POSIX_TIME_VALUE_TYPE_MICRO_SECONDS_64BIT_SIGNED,
        LIBFDATETIME_POSIX_TIME_VALUE_TYPE_MICRO_SECONDS_64BIT_UNSIGNED
};

#endif

#endif

