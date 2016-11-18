/*
 * The area functions
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

#if !defined( _LIBFDATA_INTERNAL_AREA_H )
#define _LIBFDATA_INTERNAL_AREA_H

#include <common.h>
#include <types.h>

#include "libfdata_extern.h"
#include "libfdata_libcdata.h"
#include "libfdata_libcerror.h"
#include "libfdata_libfcache.h"
#include "libfdata_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libfdata_internal_area libfdata_internal_area_t;

struct libfdata_internal_area
{
	/* The element data size
	 */
	size64_t element_data_size;

	/* The (area) size
	 */
	size64_t size;

	/* The segments array
	 */
	libcdata_array_t *segments_array;

	/* The mapped ranges array
	 */
	libcdata_array_t *mapped_ranges_array;

	/* The time stamp
	 */
	time_t timestamp;

	/* The flags
	 */
	uint8_t flags;

	/* The calculate cache entry index value
	 */
	int (*calculate_cache_entry_index)(
	       int element_index,
	       int element_data_file_index,
	       off64_t element_data_offset,
	       size64_t element_data_size,
	       uint32_t element_data_flags,
               int number_of_cache_entries );

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
	       libfdata_area_t *area,
	       libfcache_cache_t *cache,
	       off64_t element_value_offset,
	       int element_data_file_index,
	       off64_t element_data_offset,
	       size64_t element_data_size,
	       uint32_t element_data_flags,
	       uint8_t read_flags,
	       libcerror_error_t **error );

	/* The write element data function
	 */
	int (*write_element_data)(
	       intptr_t *data_handle,
	       intptr_t *file_io_handle,
	       libfdata_area_t *area,
	       libfcache_cache_t *cache,
	       off64_t element_value_offset,
	       int element_data_file_index,
	       off64_t element_data_offset,
	       size64_t element_data_size,
	       uint32_t element_data_flags,
	       uint8_t write_flags,
	       libcerror_error_t **error );
};

LIBFDATA_EXTERN \
int libfdata_area_initialize(
     libfdata_area_t **area,
     size64_t element_data_size,
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
            libfdata_area_t *area,
            libfcache_cache_t *cache,
            off64_t element_value_offset,
            int element_data_file_index,
            off64_t element_data_offset,
            size64_t element_data_size,
            uint32_t element_data_flags,
            uint8_t read_flags,
            libcerror_error_t **error ),
     int (*write_element_data)(
            intptr_t *data_handle,
            intptr_t *file_io_handle,
            libfdata_area_t *area,
            libfcache_cache_t *cache,
            off64_t element_value_offset,
            int element_data_file_index,
            off64_t element_data_offset,
            size64_t element_data_size,
            uint32_t element_data_flags,
            uint8_t write_flags,
            libcerror_error_t **error ),
     uint8_t flags,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_area_free(
     libfdata_area_t **area,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_area_clone(
     libfdata_area_t **destination_area,
     libfdata_area_t *source_area,
     libcerror_error_t **error );

/* Segment functions
 */
LIBFDATA_EXTERN \
int libfdata_area_empty(
     libfdata_area_t *area,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_area_resize(
     libfdata_area_t *area,
     int number_of_segments,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_area_get_number_of_segments(
     libfdata_area_t *area,
     int *number_of_segments,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_area_get_segment_by_index(
     libfdata_area_t *area,
     int segment_index,
     int *segment_file_index,
     off64_t *segment_offset,
     size64_t *segment_size,
     uint32_t *segment_flags,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_area_set_segment_by_index(
     libfdata_area_t *area,
     int segment_index,
     int segment_file_index,
     off64_t segment_offset,
     size64_t segment_size,
     uint32_t segment_flags,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_area_prepend_segment(
     libfdata_area_t *area,
     int segment_file_index,
     off64_t segment_offset,
     size64_t segment_size,
     uint32_t segment_flags,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_area_append_segment(
     libfdata_area_t *area,
     int *segment_index,
     int segment_file_index,
     off64_t segment_offset,
     size64_t segment_size,
     uint32_t segment_flags,
     libcerror_error_t **error );

/* Area element functions
 */
LIBFDATA_EXTERN \
int libfdata_area_get_element_data_size(
     libfdata_area_t *area,
     size64_t *element_data_size,
     libcerror_error_t **error );

/* Area element value functions
 */
LIBFDATA_EXTERN \
int libfdata_area_get_element_value_at_offset(
     libfdata_area_t *area,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     off64_t element_value_offset,
     intptr_t **element_value,
     uint8_t read_flags,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_area_set_element_value_at_offset(
     libfdata_area_t *area,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     off64_t element_value_offset,
     intptr_t *element_value,
     int (*free_element_value)(
            intptr_t **element_value,
     libcerror_error_t **error ),
     uint8_t write_flags,
     libcerror_error_t **error );

/* IO functions
 */
LIBFDATA_EXTERN \
int libfdata_area_get_size(
     libfdata_area_t *area,
     size64_t *size,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

