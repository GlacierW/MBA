/*
 * Cache functions
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

#if !defined( _LIBFDATA_INTERNAL_CACHE_H )
#define _LIBFDATA_INTERNAL_CACHE_H

#include <common.h>
#include <types.h>

#include "libfdata_extern.h"
#include "libfdata_libcerror.h"
#include "libfdata_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

#if !defined( HAVE_LOCAL_LIBFDATA )

LIBFDATA_EXTERN \
int libfdata_cache_initialize(
     libfdata_cache_t **cache,
     int maximum_cache_entries,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_cache_free(
     libfdata_cache_t **cache,
     libcerror_error_t **error );

#endif /* !defined( HAVE_LOCAL_LIBFDATA ) */

#if defined( __cplusplus )
}
#endif

#endif

