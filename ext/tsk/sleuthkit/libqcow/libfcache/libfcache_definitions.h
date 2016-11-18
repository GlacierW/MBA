/*
 * The internal definitions
 *
 * Copyright (C) 2010-2016, Joachim Metz <joachim.metz@gmail.com>
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

#if !defined( _LIBFCACHE_INTERNAL_DEFINITIONS_H )
#define _LIBFCACHE_INTERNAL_DEFINITIONS_H

#include <common.h>

/* Define HAVE_LOCAL_LIBFCACHE for local use of libfcache
 */
#if !defined( HAVE_LOCAL_LIBFCACHE )
#include <libfcache/definitions.h>

/* The definitions in <libfcache/definitions.h> are copied here
 * for local use of libfcache
 */
#else
#define LIBFCACHE_VERSION					20161107

/* The libfcache version string
 */
#define LIBFCACHE_VERSION_STRING				"20161107"

/* The cache value flags definitions
 */
enum LIBFCACHE_CACHE_VALUE_FLAGS
{
	/* The cache value is not managed by the library
	 */
	LIBFCACHE_CACHE_VALUE_FLAG_NON_MANAGED			= 0x00,

	/* The cache value is managed by the library
	 */
	LIBFCACHE_CACHE_VALUE_FLAG_MANAGED			= 0x01
};

#endif

#endif

