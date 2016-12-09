/*
 * The internal libfcache header
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

#if !defined( _LIBFDATA_LIBFCACHE_H )
#define _LIBFDATA_LIBFCACHE_H

#include <common.h>

/* Define HAVE_LOCAL_LIBFCACHE for local use of libfcache
 */
#if defined( HAVE_LOCAL_LIBFCACHE )

#include <libfcache_cache.h>
#include <libfcache_date_time.h>
#include <libfcache_definitions.h>
#include <libfcache_types.h>

#else

/* If libtool DLL support is enabled set LIBFCACHE_DLL_IMPORT
 * before including libfcache.h
 */
#if defined( _WIN32 ) && defined( DLL_IMPORT )
#define LIBFCACHE_DLL_IMPORT
#endif

#include <libfcache.h>

#endif /* defined( HAVE_LOCAL_LIBFCACHE ) */

#endif /* !defined( _LIBFDATA_LIBFCACHE_H ) */

