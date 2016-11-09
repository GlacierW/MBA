/*
 * The (data) mapped range functions
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

#if !defined( _LIBFDATA_MAPPED_RANGE_H )
#define _LIBFDATA_MAPPED_RANGE_H

#include <common.h>
#include <types.h>

#include "libfdata_libcerror.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libfdata_mapped_range libfdata_mapped_range_t;

struct libfdata_mapped_range
{
	/* The (start) offset
	 */
	off64_t offset;

	/* The size
	 */
	size64_t size;
};

int libfdata_mapped_range_initialize(
     libfdata_mapped_range_t **mapped_range,
     libcerror_error_t **error );

int libfdata_mapped_range_free(
     libfdata_mapped_range_t **mapped_range,
     libcerror_error_t **error );

int libfdata_mapped_range_clone(
     libfdata_mapped_range_t **destination_mapped_range,
     libfdata_mapped_range_t *source_mapped_range,
     libcerror_error_t **error );

int libfdata_mapped_range_get(
     libfdata_mapped_range_t *mapped_range,
     off64_t *offset,
     size64_t *size,
     libcerror_error_t **error );

int libfdata_mapped_range_set(
     libfdata_mapped_range_t *mapped_range,
     off64_t offset,
     size64_t size,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

