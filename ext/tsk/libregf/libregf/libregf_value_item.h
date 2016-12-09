/*
 * Value item functions
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

#if !defined( _LIBREGF_VALUE_ITEM_H )
#define _LIBREGF_VALUE_ITEM_H

#include <common.h>
#include <types.h>

#include "libregf_hive_bins_list.h"
#include "libregf_libbfio.h"
#include "libregf_libcerror.h"
#include "libregf_libfcache.h"
#include "libregf_libfdata.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libregf_value_item libregf_value_item_t;

struct libregf_value_item
{
	/* The flags
	 */
	uint16_t flags;

	/* The name hash
	 */
	uint32_t name_hash;

	/* The name
	 */
	uint8_t *name;

	/* The name size
	 */
	uint16_t name_size;

	/* The value type
	 */
	uint32_t type;

	/* The value data type
	 */
	uint8_t data_type;

	/* The value data buffer
	 */
	uint8_t *data_buffer;

	/* The value data buffer size
	 */
	size_t data_buffer_size;

	/* The large value data stream
	 */
	libfdata_stream_t *data_stream;

	/* Various item flags
	 */
	uint8_t item_flags;
};

int libregf_value_item_initialize(
     libregf_value_item_t **value_item,
     libcerror_error_t **error );

int libregf_value_item_free(
     libregf_value_item_t **value_item,
     libcerror_error_t **error );

int libregf_value_item_get_data_size(
     libregf_value_item_t *value_item,
     size_t *data_size,
     libcerror_error_t **error );

int libregf_value_item_get_data(
     libregf_value_item_t *value_item,
     libbfio_handle_t *file_io_handle,
     uint8_t **data,
     size_t *data_size,
     libcerror_error_t **error );

int libregf_value_item_read_value_key(
     libregf_value_item_t *value_item,
     libbfio_handle_t *file_io_handle,
     libregf_hive_bins_list_t *hive_bins_list,
     uint32_t value_key_offset,
     libcerror_error_t **error );

int libregf_value_item_read_value_data(
     libregf_value_item_t *value_item,
     libbfio_handle_t *file_io_handle,
     libregf_hive_bins_list_t *hive_bins_list,
     uint32_t value_data_offset,
     uint32_t value_data_size,
     libcerror_error_t **error );

int libregf_value_item_read_data_block_list(
     libregf_value_item_t *value_item,
     libbfio_handle_t *file_io_handle,
     libregf_hive_bins_list_t *hive_bins_list,
     uint32_t data_block_list_offset,
     uint16_t number_of_segments,
     uint32_t value_data_size,
     libcerror_error_t **error );

int libregf_value_item_read_element_data(
     libregf_hive_bins_list_t *hive_bins_list,
     libbfio_handle_t *file_io_handle,
     libfdata_list_element_t *element,
     libfcache_cache_t *cache,
     int data_range_file_index,
     off64_t data_range_offset,
     size64_t data_range_size,
     uint32_t data_range_flags,
     uint8_t read_flags,
     libcerror_error_t **error );

int libregf_value_item_compare_name_with_utf8_string(
     libregf_value_item_t *value_item,
     uint32_t name_hash,
     const uint8_t *utf8_string,
     size_t utf8_string_length,
     int ascii_codepage,
     libcerror_error_t **error );

int libregf_value_item_compare_name_with_utf16_string(
     libregf_value_item_t *value_item,
     uint32_t name_hash,
     const uint16_t *utf16_string,
     size_t utf16_string_length,
     int ascii_codepage,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBREGF_VALUE_ITEM_H ) */

