/*
 * File pool functions
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

#if !defined( _LIBBFIO_FILE_POOL_H )
#define _LIBBFIO_FILE_POOL_H

#include <common.h>
#include <types.h>

#include "libbfio_extern.h"
#include "libbfio_libcerror.h"
#include "libbfio_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

LIBBFIO_EXTERN \
int libbfio_file_pool_append_handles_for_names(
     libbfio_pool_t *pool,
     char * const names[],
     int number_of_names,
     int access_flags,
     libcerror_error_t **error );

#if defined( HAVE_WIDE_CHARACTER_TYPE )

LIBBFIO_EXTERN \
int libbfio_file_pool_append_handles_for_names_wide(
     libbfio_pool_t *pool,
     wchar_t * const names[],
     int number_of_names,
     int access_flags,
     libcerror_error_t **error );

#endif /* defined( HAVE_WIDE_CHARACTER_TYPE ) */

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBBFIO_FILE_POOL_H ) */

