/*
 * The internal definitions
 *
 * Copyright (C) 2006-2016, Joachim Metz <joachim.metz@gmail.com>
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

#if !defined( LIBCDATA_INTERNAL_DEFINITIONS_H )
#define LIBCDATA_INTERNAL_DEFINITIONS_H

#include <common.h>
#include <types.h>

/* Define HAVE_LOCAL_LIBCDATA for local use of libcdata
 */
#if !defined( HAVE_LOCAL_LIBCDATA )
#include <libcdata/definitions.h>

/* The definitions in <libcdata/definitions.h> are copied here
 * for local use of libcdata
 */
#else

#define LIBCDATA_VERSION				20161106

/* The libcdata version string
 */
#define LIBCDATA_VERSION_STRING				"20161106"

/* The comparison function definitions
 */
enum LIBCDATA_COMPARE_DEFINITIONS
{
	/* The first value is less than the second value
	 */
        LIBCDATA_COMPARE_LESS,

	/* The first and second values are equal
	 */
        LIBCDATA_COMPARE_EQUAL,

	/* The first value is greater than the second value
	 */
        LIBCDATA_COMPARE_GREATER
};

/* The insert flag definitions
 */
enum LIBCDATA_INSERT_FLAGS
{
	/* Allow duplicate entries
	 */
	LIBCDATA_INSERT_FLAG_NON_UNIQUE_ENTRIES	= 0x00,

	/* Only allow unique entries, no duplicates
	 */
	LIBCDATA_INSERT_FLAG_UNIQUE_ENTRIES	= 0x01
};

#endif /* !defined( HAVE_LOCAL_LIBCDATA ) */

#endif

