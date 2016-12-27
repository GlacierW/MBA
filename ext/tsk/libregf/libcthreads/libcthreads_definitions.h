/*
 * The internal definitions
 *
 * Copyright (C) 2012-2016, Joachim Metz <joachim.metz@gmail.com>
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

#if !defined( LIBCTHREADS_INTERNAL_DEFINITIONS_H )
#define LIBCTHREADS_INTERNAL_DEFINITIONS_H

#include <common.h>
#include <types.h>

/* Define HAVE_LOCAL_LIBCTHREADS for local use of libcthreads
 */
#if !defined( HAVE_LOCAL_LIBCTHREADS )
#include <libcthreads/definitions.h>

/* The definitions in <libcthreads/definitions.h> are copied here
 * for local use of libcthreads
 */
#else

#define LIBCTHREADS_VERSION				20161119

/* The libcthreads version string
 */
#define LIBCTHREADS_VERSION_STRING			"20161119"

/* The comparison function definitions
 */
enum LIBCTHREADS_COMPARE_DEFINITIONS
{
	/* The first value is less than the second value
	 */
        LIBCTHREADS_COMPARE_LESS,

	/* The first and second values are equal
	 */
        LIBCTHREADS_COMPARE_EQUAL,

	/* The first value is greater than the second value
	 */
        LIBCTHREADS_COMPARE_GREATER
};

/* The sort flag definitions
 */
enum LIBCTHREADS_SORT_FLAGS
{
	/* Allow duplicate values
	 */
	LIBCTHREADS_SORT_FLAG_NON_UNIQUE_VALUES		= 0x00,

	/* Only allow unique values, no duplicates
	 */
	LIBCTHREADS_SORT_FLAG_UNIQUE_VALUES		= 0x01
};

#endif /* !defined( HAVE_LOCAL_LIBCTHREADS ) */

/* The status definitions
 */
enum LIBCTHREADS_STATUS
{
	LIBCTHREADS_STATUS_EXIT				= 1
};

#endif /* !defined( LIBCTHREADS_INTERNAL_DEFINITIONS_H ) */

