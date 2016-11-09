/*
 * The balanced tree functions
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

#if !defined( _LIBFDATA_INTERNAL_BTREE_H )
#define _LIBFDATA_INTERNAL_BTREE_H

#include <common.h>
#include <types.h>

#include "libfdata_btree_node.h"
#include "libfdata_btree_range.h"
#include "libfdata_extern.h"
#include "libfdata_libcerror.h"
#include "libfdata_libfcache.h"
#include "libfdata_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libfdata_internal_btree libfdata_internal_btree_t;

struct libfdata_internal_btree
{
	/* The root node data range
	 */
	libfdata_btree_range_t *root_node_data_range;

	/* The time stamp
	 */
	time_t timestamp;

	/* The flags
	 */
	uint8_t flags;

	/* The calculate node cache entry index value
	 */
	int (*calculate_node_cache_entry_index)(
	       libfdata_btree_t *tree,
	       libfcache_cache_t *cache,
	       int level,
	       int node_data_file_index,
	       off64_t node_data_offset,
	       size64_t node_data_size,
	       uint32_t node_data_flags,
	       int *cache_entry_index,
	       libcerror_error_t **error );

	/* The calculate leaf value cache entry index value
	 */
	int (*calculate_leaf_value_cache_entry_index)(
	       libfdata_btree_t *tree,
	       libfcache_cache_t *cache,
	       int leaf_value_index,
	       int leaf_value_data_file_index,
	       off64_t leaf_value_data_offset,
	       size64_t leaf_value_data_size,
	       uint32_t leaf_value_data_flags,
	       int *cache_entry_index,
	       libcerror_error_t **error );

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

	/* The read node function
	 */
	int (*read_node)(
	       intptr_t *data_handle,
	       intptr_t *file_io_handle,
	       libfdata_btree_node_t *node,
	       int node_data_file_index,
	       off64_t node_data_offset,
	       size64_t node_data_size,
	       uint32_t node_data_flags,
	       intptr_t *key_value,
               uint8_t read_flags,
	       libcerror_error_t **error );

	/* The read leaf value function
	 */
	int (*read_leaf_value)(
	       intptr_t *data_handle,
	       intptr_t *file_io_handle,
	       libfdata_btree_t *tree,
	       libfcache_cache_t *cache,
	       int leaf_value_index,
	       int leaf_value_data_file_index,
	       off64_t leaf_value_data_offset,
	       size64_t leaf_value_data_size,
	       uint32_t leaf_value_data_flags,
	       intptr_t *key_value,
               uint8_t read_flags,
	       libcerror_error_t **error );
};

LIBFDATA_EXTERN \
int libfdata_btree_initialize(
     libfdata_btree_t **tree,
     intptr_t *data_handle,
     int (*free_data_handle)(
            intptr_t **data_handle,
            libcerror_error_t **error ),
     int (*clone_data_handle)(
            intptr_t **destination_data_handle,
            intptr_t *source_data_handle,
            libcerror_error_t **error ),
     int (*read_node)(
            intptr_t *data_handle,
            intptr_t *file_io_handle,
            libfdata_btree_node_t *node,
            int node_data_file_index,
            off64_t node_data_offset,
            size64_t node_data_size,
            uint32_t node_data_flags,
            intptr_t *key_value,
            uint8_t read_flags,
            libcerror_error_t **error ),
     int (*read_leaf_value)(
            intptr_t *data_handle,
            intptr_t *file_io_handle,
            libfdata_btree_t *tree,
            libfcache_cache_t *cache,
            int leaf_value_index,
            int leaf_value_data_file_index,
            off64_t leaf_value_data_offset,
            size64_t leaf_value_data_size,
            uint32_t leaf_value_data_flags,
            intptr_t *key_value,
            uint8_t read_flags,
            libcerror_error_t **error ),
     uint8_t flags,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_btree_free(
     libfdata_btree_t **tree,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_btree_clone(
     libfdata_btree_t **destination_tree,
     libfdata_btree_t *source_tree,
     libcerror_error_t **error );

/* Cache entry functions
 */
int libfdata_btree_calculate_node_cache_entry_index(
     libfdata_btree_t *tree,
     libfcache_cache_t *cache,
     int level,
     int node_data_file_index,
     off64_t node_data_offset,
     size64_t node_data_size,
     uint32_t node_data_flags,
     int *cache_entry_index,
     libcerror_error_t **error );

int libfdata_btree_calculate_leaf_value_cache_entry_index(
     libfdata_btree_t *tree,
     libfcache_cache_t *cache,
     int leaf_value_index,
     int leaf_value_data_file_index,
     off64_t leaf_value_data_offset,
     size64_t leaf_value_data_size,
     uint32_t leaf_value_data_flags,
     int *cache_entry_index,
     libcerror_error_t **error );

/* Root node functions
 */
LIBFDATA_EXTERN \
int libfdata_btree_get_root_node(
     libfdata_btree_t *tree,
     int *node_data_file_index,
     off64_t *node_data_offset,
     size64_t *node_data_size,
     uint32_t *node_data_flags,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_btree_set_root_node(
     libfdata_btree_t *tree,
     int node_data_file_index,
     off64_t node_data_offset,
     size64_t node_data_size,
     uint32_t node_data_flags,
     libcerror_error_t **error );

/* Leaf value functions
 */
int libfdata_btree_read_leaf_value(
     libfdata_internal_btree_t *internal_tree,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     libfdata_btree_range_t *leaf_value_data_range,
     intptr_t **leaf_value,
     uint8_t read_flags,
     libcerror_error_t **error );

int libfdata_btree_read_node(
     libfdata_internal_btree_t *internal_tree,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     libfdata_btree_range_t *node_data_range,
     int level,
     libfdata_btree_node_t **node,
     uint8_t read_flags,
     libcerror_error_t **error );

int libfdata_btree_read_sub_tree(
     libfdata_internal_btree_t *internal_tree,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     libfdata_btree_range_t *node_data_range,
     int level,
     int mapped_first_leaf_value_index,
     libfdata_btree_node_t **node,
     uint8_t read_flags,
     libcerror_error_t **error );

int libfdata_btree_get_leaf_node_by_index(
     libfdata_internal_btree_t *internal_tree,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     libfdata_btree_range_t *node_data_range,
     int level,
     int leaf_value_index,
     libfdata_btree_node_t **node,
     uint8_t read_flags,
     libcerror_error_t **error );

int libfdata_btree_get_leaf_node_by_key(
     libfdata_internal_btree_t *internal_tree,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     libfdata_btree_range_t *node_data_range,
     int maximum_node_level,
     int current_node_level,
     intptr_t *key_value,
     int (*key_value_compare_function)(
            intptr_t *first_key_value,
            intptr_t *second_key_value,
            libcerror_error_t **error ),
     int *node_index,
     libfdata_btree_node_t **node,
     uint8_t read_flags,
     libcerror_error_t **error );

int libfdata_btree_get_next_leaf_node_by_key(
     libfdata_internal_btree_t *internal_tree,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     libfdata_btree_range_t *node_data_range,
     int node_index,
     libfdata_btree_node_t *node,
     intptr_t *key_value,
     int (*key_value_compare_function)(
            intptr_t *first_key_value,
            intptr_t *second_key_value,
            libcerror_error_t **error ),
     int *next_node_index,
     libfdata_btree_node_t **next_node,
     uint8_t read_flags,
     libcerror_error_t **error );

int libfdata_btree_get_previous_leaf_node_by_key(
     libfdata_internal_btree_t *internal_tree,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     libfdata_btree_range_t *node_data_range,
     int node_index,
     libfdata_btree_node_t *node,
     intptr_t *key_value,
     int (*key_value_compare_function)(
            intptr_t *first_key_value,
            intptr_t *second_key_value,
            libcerror_error_t **error ),
     int *previous_node_index,
     libfdata_btree_node_t **previous_node,
     uint8_t read_flags,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_btree_get_number_of_leaf_values(
     libfdata_btree_t *tree,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     int *number_of_values,
     uint8_t read_flags,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_btree_get_leaf_value_by_index(
     libfdata_btree_t *tree,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     int leaf_value_index,
     intptr_t **value,
     uint8_t read_flags,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_btree_set_leaf_value_by_index(
     libfdata_btree_t *tree,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     int leaf_value_index,
     intptr_t *leaf_value,
     int (*free_leaf_value)(
            intptr_t **leaf_value,
     libcerror_error_t **error ),
     uint8_t write_flags,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_btree_get_leaf_value_by_key(
     libfdata_btree_t *tree,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     intptr_t *key_value,
     int (*key_value_compare_function)(
            intptr_t *first_key_value,
            intptr_t *second_key_value,
            libcerror_error_t **error ),
     uint8_t search_flags,
     intptr_t **value,
     uint8_t read_flags,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

