/*
 * The segments array functions
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

#if !defined( _LIBFDATA_SEGMENTS_ARRAY_H )
#define _LIBFDATA_SEGMENTS_ARRAY_H

#include <common.h>
#include <types.h>

#include "libfdata_libcdata.h"
#include "libfdata_libcerror.h"

int libfdata_segments_array_get_segment_by_index(
     libcdata_array_t *segments_array,
     int segment_index,
     int *segment_file_index,
     off64_t *segment_offset,
     size64_t *segment_size,
     uint32_t *segment_flags,
     libcerror_error_t **error );

int libfdata_segments_array_set_segment_by_index(
     libcdata_array_t *segments_array,
     libcdata_array_t *mapped_ranges_array,
     size64_t *data_size,
     int segment_index,
     int segment_file_index,
     off64_t segment_offset,
     size64_t segment_size,
     uint32_t segment_flags,
     libcerror_error_t **error );

int libfdata_segments_array_prepend_segment(
     libcdata_array_t *segments_array,
     libcdata_array_t *mapped_ranges_array,
     size64_t *data_size,
     int segment_file_index,
     off64_t segment_offset,
     size64_t segment_size,
     uint32_t segment_flags,
     libcerror_error_t **error );

int libfdata_segments_array_append_segment(
     libcdata_array_t *segments_array,
     libcdata_array_t *mapped_ranges_array,
     size64_t *data_size,
     int *segment_index,
     int segment_file_index,
     off64_t segment_offset,
     size64_t segment_size,
     uint32_t segment_flags,
     libcerror_error_t **error );

int libfdata_segments_array_calculate_mapped_ranges(
     libcdata_array_t *segments_array,
     libcdata_array_t *mapped_ranges_array,
     libcerror_error_t **error );

int libfdata_segments_array_get_data_range_at_offset(
     libcdata_array_t *segments_array,
     off64_t value_offset,
     off64_t *segment_data_offset,
     libfdata_range_t **segment_data_range,
     libcerror_error_t **error );

#if defined( __cplusplus )
extern "C" {
#endif

#if defined( __cplusplus )
}
#endif

#endif

