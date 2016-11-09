/*
 * The balanced tree node functions
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

#if !defined( _LIBFDATA_INTERNAL_BTREE_NODE_H )
#define _LIBFDATA_INTERNAL_BTREE_NODE_H

#include <common.h>
#include <types.h>

#include "libfdata_btree_range.h"
#include "libfdata_extern.h"
#include "libfdata_libcdata.h"
#include "libfdata_libcerror.h"
#include "libfdata_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libfdata_internal_btree_node libfdata_internal_btree_node_t;

struct libfdata_internal_btree_node
{
	/* The (node) level
	 */
	int level;

	/* The sub node (data) ranges array
	 */
	libcdata_array_t *sub_node_ranges_array;

	/* The number of leaf values in the branch
	 */
	int branch_number_of_leaf_values;

	/* The leaf value (data) ranges array
	 */
	libcdata_array_t *leaf_value_ranges_array;

	/* The flags
	 */
	uint8_t flags;
};

int libfdata_btree_node_initialize(
     libfdata_btree_node_t **node,
     int level,
     libcerror_error_t **error );

int libfdata_btree_node_free(
     libfdata_btree_node_t **node,
     libcerror_error_t **error );

/* Node functions
 */

LIBFDATA_EXTERN \
int libfdata_btree_node_get_level(
     libfdata_btree_node_t *node,
     int *level,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_btree_node_is_branch(
     libfdata_btree_node_t *node,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_btree_node_is_leaf(
     libfdata_btree_node_t *node,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_btree_node_is_root(
     libfdata_btree_node_t *node,
     libcerror_error_t **error );

/* Sub node data range functions
 */

LIBFDATA_EXTERN \
int libfdata_btree_node_get_number_of_sub_nodes(
     libfdata_btree_node_t *node,
     int *number_of_sub_nodes,
     libcerror_error_t **error );

int libfdata_btree_node_get_sub_node_data_range_by_index(
     libfdata_btree_node_t *node,
     int sub_node_index,
     libfdata_btree_range_t **sub_node_data_range,
     libcerror_error_t **error );

int libfdata_btree_node_get_sub_node_data_range_by_mapped_index(
     libfdata_btree_node_t *node,
     int mapped_index,
     libfdata_btree_range_t **sub_node_data_range,
     libcerror_error_t **error );

int libfdata_btree_node_get_sub_node_data_range_by_key(
     libfdata_btree_node_t *node,
     intptr_t *key_value,
     int (*key_value_compare_function)(
            intptr_t *first_key_value,
            intptr_t *second_key_value,
            libcerror_error_t **error ),
     int *sub_node_index,
     libfdata_btree_range_t **sub_node_data_range,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_btree_node_get_sub_node_by_index(
     libfdata_btree_node_t *node,
     int sub_node_index,
     int *sub_node_data_file_index,
     off64_t *sub_node_data_offset,
     size64_t *sub_node_data_size,
     uint32_t *sub_node_data_flags,
     intptr_t **key_value,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_btree_node_set_sub_node_by_index(
     libfdata_btree_node_t *node,
     int sub_node_index,
     int sub_node_data_file_index,
     off64_t sub_node_data_offset,
     size64_t sub_node_data_size,
     uint32_t sub_node_data_flags,
     intptr_t *key_value,
     int (*free_key_value)(
            intptr_t **key_value,
            libcerror_error_t **error ),
     uint8_t key_value_flags,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_btree_node_append_sub_node(
     libfdata_btree_node_t *node,
     int *sub_node_index,
     int sub_node_data_file_index,
     off64_t sub_node_data_offset,
     size64_t sub_node_data_size,
     uint32_t sub_node_data_flags,
     intptr_t *key_value,
     int (*free_key_value)(
            intptr_t **key_value,
            libcerror_error_t **error ),
     uint8_t key_value_flags,
     libcerror_error_t **error );

/* Branch leaf values functions
 */

int libfdata_btree_node_get_number_of_leaf_values_in_branch(
     libfdata_btree_node_t *node,
     int *number_of_leaf_values,
     libcerror_error_t **error );

/* Leaf value data range functions
 */

LIBFDATA_EXTERN \
int libfdata_btree_node_get_number_of_leaf_values(
     libfdata_btree_node_t *node,
     int *number_of_leaf_values,
     libcerror_error_t **error );

int libfdata_btree_node_get_leaf_value_data_range_by_index(
     libfdata_btree_node_t *node,
     int leaf_value_index,
     libfdata_btree_range_t **leaf_value_data_range,
     libcerror_error_t **error );

int libfdata_btree_node_get_leaf_value_data_range_by_mapped_index(
     libfdata_btree_node_t *node,
     int mapped_index,
     libfdata_btree_range_t **leaf_value_data_range,
     libcerror_error_t **error );

int libfdata_btree_node_get_leaf_value_data_range_by_key(
     libfdata_btree_node_t *node,
     intptr_t *key_value,
     int (*key_value_compare_function)(
            intptr_t *first_key_value,
            intptr_t *second_key_value,
            libcerror_error_t **error ),
     libfdata_btree_range_t **leaf_value_data_range,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_btree_node_get_leaf_value_by_index(
     libfdata_btree_node_t *node,
     int leaf_value_index,
     int *leaf_value_data_file_index,
     off64_t *leaf_value_data_offset,
     size64_t *leaf_value_data_size,
     uint32_t *leaf_value_data_flags,
     intptr_t **key_value,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_btree_node_get_leaf_value_by_key(
     libfdata_btree_node_t *node,
     intptr_t *key_value,
     int (*key_value_compare_function)(
            intptr_t *first_key_value,
            intptr_t *second_key_value,
            libcerror_error_t **error ),
     int *leaf_value_data_file_index,
     off64_t *leaf_value_data_offset,
     size64_t *leaf_value_data_size,
     uint32_t *leaf_value_data_flags,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_btree_node_set_leaf_value_by_index(
     libfdata_btree_node_t *node,
     int leaf_value_index,
     int leaf_value_data_file_index,
     off64_t leaf_value_data_offset,
     size64_t leaf_value_data_size,
     uint32_t leaf_value_data_flags,
     intptr_t *key_value,
     int (*free_key_value)(
            intptr_t **key_value,
            libcerror_error_t **error ),
     uint8_t key_value_flags,
     libcerror_error_t **error );

LIBFDATA_EXTERN \
int libfdata_btree_node_append_leaf_value(
     libfdata_btree_node_t *node,
     int *leaf_value_index,
     int leaf_value_data_file_index,
     off64_t leaf_value_data_offset,
     size64_t leaf_value_data_size,
     uint32_t leaf_value_data_flags,
     intptr_t *key_value,
     int (*free_key_value)(
            intptr_t **key_value,
            libcerror_error_t **error ),
     uint8_t key_value_flags,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

