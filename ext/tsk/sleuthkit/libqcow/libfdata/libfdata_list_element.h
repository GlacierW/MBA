/*
 * The list element functions
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

#if !defined( _LIBFDATA_INTERNAL_LIST_ELEMENT_H )
#define _LIBFDATA_INTERNAL_LIST_ELEMENT_H

#include <common.h>
#include <types.h>

#include "libfdata_extern.h"
#include "libfdata_libcerror.h"
#include "libfdata_libfcache.h"
#include "libfdata_range.h"
#include "libfdata_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libfdata_internal_list_element libfdata_internal_list_element_t;

struct libfdata_internal_list_element
{
	/* The list reference
	 */
	libfdata_list_t *list;

	/* The (list) element index
	 */
	int element_index;

	/* The (element) data range
	 */
	libfdata_range_t *data_range;

	/* The (element) mapped size
	 */
	size64_t mapped_size;

	/* The time stamp
	 */
	time_t timestamp;

	/* The flags
	 */
	uint8_t flags;
};

int libfdata_list_element_initialize(
     libfdata_list_element_t **element,
     libfdata_list_t *list,
     int element_index,
     libcerror_error_t **error );

int libfdata_list_element_free(
     libfdata_list_element_t **element,
     libcerror_error_t **error );

int libfdata_list_element_clone(
     libfdata_list_element_t **destination_element,
     libfdata_list_element_t *source_element,
     libfdata_list_t *list,
     int element_index,
     libcerror_error_t **error );

int libfdata_list_element_get_element_index(
     libfdata_list_element_t *element,
     int *element_index,
     libcerror_error_t **error );

int libfdata_list_element_set_element_index(
     libfdata_list_element_t *element,
     int element_index,
     libcerror_error_t **error );

int libfdata_list_element_get_timestamp(
     libfdata_list_element_t *element,
     time_t *timestamp,
     libcerror_error_t **error );

/* Data range functions
 */
int libfdata_list_element_get_data_range(
     libfdata_list_element_t *element,
     int *file_index,
     off64_t *offset,
     size64_t *size,
     uint32_t *flags,
     libcerror_error_t **error );

int libfdata_list_element_set_data_range(
     libfdata_list_element_t *element,
     int file_index,
     off64_t offset,
     size64_t size,
     uint32_t flags,
     libcerror_error_t **error );

/* Mapped range functions
 */
LIBFDATA_EXTERN \
int libfdata_list_element_get_mapped_size(
     libfdata_list_element_t *element,
     size64_t *mapped_size,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_list_element_set_mapped_size(
     libfdata_list_element_t *element,
     size64_t mapped_size,
     libcerror_error_t **error );

/* Element value functions
 */
LIBFDATA_EXTERN \
int libfdata_list_element_get_element_value(
     libfdata_list_element_t *element,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     intptr_t **element_value,
     uint8_t read_flags,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_list_element_set_element_value(
     libfdata_list_element_t *element,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     intptr_t *element_value,
     int (*free_element_value)(
            intptr_t **element_value,
            libcerror_error_t **error ),
     uint8_t write_flags,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

