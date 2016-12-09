/*
 * Key item functions
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

#if !defined( _LIBREGF_KEY_ITEM_H )
#define _LIBREGF_KEY_ITEM_H

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

typedef struct libregf_key_item libregf_key_item_t;

struct libregf_key_item
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

	/* The class name
	 */
	uint8_t *class_name;

	/* The class name size
	 */
	uint16_t class_name_size;

	/* The security descriptor
	 */
	uint8_t *security_descriptor;

	/* The security descriptor size
	 */
	size_t security_descriptor_size;

	/* The last written time
	 */
	uint64_t last_written_time;

	/* The values list
	 */
	libfdata_list_t *values_list;

	/* The values cache
	 */
	libfcache_cache_t *values_cache;

	/* Various item flags
	 */
	uint8_t item_flags;
};

int libregf_key_item_initialize(
     libregf_key_item_t **key_item,
     libcerror_error_t **error );

int libregf_key_item_free(
     libregf_key_item_t **key_item,
     libcerror_error_t **error );

int libregf_key_item_get_number_of_values(
     libregf_key_item_t *key_item,
     int *number_of_values,
     libcerror_error_t **error );

int libregf_key_item_read_named_key(
     libregf_key_item_t *key_item,
     libfdata_tree_node_t *key_tree_node,
     libbfio_handle_t *file_io_handle,
     libregf_hive_bins_list_t *hive_bins_list,
     off64_t named_key_offset,
     uint32_t named_key_hash,
     libcerror_error_t **error );

int libregf_key_item_read_class_name(
     libregf_key_item_t *key_item,
     libbfio_handle_t *file_io_handle,
     libregf_hive_bins_list_t *hive_bins_list,
     uint32_t class_name_offset,
     uint16_t class_name_size,
     libcerror_error_t **error );

int libregf_key_item_read_security_key(
     libregf_key_item_t *key_item,
     libbfio_handle_t *file_io_handle,
     libregf_hive_bins_list_t *hive_bins_list,
     uint32_t security_key_offset,
     libcerror_error_t **error );

int libregf_key_item_read_values_list(
     libregf_key_item_t *key_item,
     libbfio_handle_t *file_io_handle,
     libregf_hive_bins_list_t *hive_bins_list,
     uint32_t values_list_offset,
     uint32_t number_of_values_list_elements,
     libcerror_error_t **error );

int libregf_key_item_read_node_data(
     libregf_hive_bins_list_t *hive_bins_list,
     libbfio_handle_t *file_io_handle,
     libfdata_tree_node_t *node,
     libfcache_cache_t *cache,
     int node_data_file_index,
     off64_t node_data_offset,
     size64_t node_data_size,
     uint32_t node_data_flags,
     uint8_t read_flags,
     libcerror_error_t **error );

int libregf_key_item_read_sub_keys_list(
     libfdata_tree_node_t *key_tree_node,
     libbfio_handle_t *file_io_handle,
     libregf_hive_bins_list_t *hive_bins_list,
     off64_t sub_keys_list_offset,
     libcerror_error_t **error );

int libregf_key_item_read_sub_nodes(
     libregf_hive_bins_list_t *hive_bins_list,
     libbfio_handle_t *file_io_handle,
     libfdata_tree_node_t *node,
     libfcache_cache_t *cache,
     int sub_nodes_data_file_index,
     off64_t sub_nodes_data_offset,
     size64_t sub_nodes_data_size,
     uint32_t sub_nodes_data_flags,
     uint8_t read_flags,
     libcerror_error_t **error );

int libregf_key_item_compare_name_with_utf8_string(
     libregf_key_item_t *key_item,
     uint32_t name_hash,
     const uint8_t *utf8_string,
     size_t utf8_string_length,
     int ascii_codepage,
     libcerror_error_t **error );

int libregf_key_item_compare_name_with_utf16_string(
     libregf_key_item_t *key_item,
     uint32_t name_hash,
     const uint16_t *utf16_string,
     size_t utf16_string_length,
     int ascii_codepage,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBREGF_KEY_ITEM_H ) */

