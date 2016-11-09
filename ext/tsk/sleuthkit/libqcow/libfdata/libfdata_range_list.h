/*
 * The range list functions
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

#if !defined( _LIBFDATA_INTERNAL_RANGE_LIST_H )
#define _LIBFDATA_INTERNAL_RANGE_LIST_H

#include <common.h>
#include <types.h>

#include "libfdata_extern.h"
#include "libfdata_libcdata.h"
#include "libfdata_libcerror.h"
#include "libfdata_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libfdata_internal_range_list libfdata_internal_range_list_t;

struct libfdata_internal_range_list
{
	/* The (list) elements range list
	 */
	libcdata_range_list_t *elements_range_list;

	/* The flags
	 */
	uint8_t flags;

	/* The data handle
	 */
	intptr_t *data_handle;

	/* The free data handle function
	 */
	int (*free_data_handle)(
	       intptr_t **data_handle,
	       libcerror_error_t **error );

	/* The clone (duplicate) data handle function
	 */
	int (*clone_data_handle)(
	       intptr_t **destination_data_handle,
	       intptr_t *source_data_handle,
	       libcerror_error_t **error );

	/* The read element data function
	 */
	int (*read_element_data)(
	       intptr_t *data_handle,
	       intptr_t *file_io_handle,
	       libfdata_list_element_t *list_element,
	       libfcache_cache_t *cache,
	       int element_file_index,
	       off64_t element_offset,
	       size64_t element_size,
	       uint32_t element_flags,
               uint8_t read_flags,
	       libcerror_error_t **error );

	/* The write element data function
	 */
	int (*write_element_data)(
	       intptr_t *data_handle,
	       intptr_t *file_io_handle,
	       libfdata_list_element_t *list_element,
	       libfcache_cache_t *cache,
	       int element_file_index,
	       off64_t element_offset,
	       size64_t element_size,
	       uint32_t element_flags,
               uint8_t write_flags,
	       libcerror_error_t **error );
};

LIBFDATA_EXTERN \
int libfdata_range_list_initialize(
     libfdata_range_list_t **range_list,
     intptr_t *data_handle,
     int (*free_data_handle)(
            intptr_t **data_handle,
            libcerror_error_t **error ),
     int (*clone_data_handle)(
            intptr_t **destination_data_handle,
            intptr_t *source_data_handle,
            libcerror_error_t **error ),
     int (*read_element_data)(
            intptr_t *data_handle,
            intptr_t *file_io_handle,
            libfdata_list_element_t *list_element,
            libfcache_cache_t *cache,
            int element_file_index,
            off64_t element_offset,
            size64_t element_size,
            uint32_t element_flags,
            uint8_t read_flags,
            libcerror_error_t **error ),
     int (*write_element_data)(
            intptr_t *data_handle,
            intptr_t *file_io_handle,
            libfdata_list_element_t *list_element,
            libfcache_cache_t *cache,
            int element_file_index,
            off64_t element_offset,
            size64_t element_size,
            uint32_t element_flags,
            uint8_t write_flags,
            libcerror_error_t **error ),
     uint8_t flags,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_range_list_free(
     libfdata_range_list_t **range_list,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_range_list_clone(
     libfdata_range_list_t **destination_range_list,
     libfdata_range_list_t *source_range_list,
     libcerror_error_t **error );

/* List elements functions
 */
LIBFDATA_EXTERN \
int libfdata_range_list_empty(
     libfdata_range_list_t *range_list,
     libcerror_error_t **error );

/* Mapped range functions
 */

LIBFDATA_EXTERN \
int libfdata_range_list_get_list_element_at_offset(
     libfdata_range_list_t *range_list,
     off64_t offset,
     off64_t *element_data_offset,
     libfdata_list_element_t **element,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_range_list_get_element_at_offset(
     libfdata_range_list_t *range_list,
     off64_t offset,
     off64_t *element_data_offset,
     int *element_file_index,
     off64_t *element_offset,
     size64_t *element_size,
     uint32_t *element_flags,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_range_list_insert_element(
     libfdata_range_list_t *range_list,
     off64_t offset,
     size64_t size,
     int element_file_index,
     off64_t element_offset,
     size64_t element_size,
     uint32_t element_flags,
     libcerror_error_t **error );

/* List element value functions
 */

LIBFDATA_EXTERN \
int libfdata_range_list_get_element_value_at_offset(
     libfdata_range_list_t *range_list,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     off64_t offset,
     off64_t *element_data_offset,
     intptr_t **element_value,
     uint8_t read_flags,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_range_list_set_element_value_at_offset(
     libfdata_range_list_t *range_list,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     off64_t offset,
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

