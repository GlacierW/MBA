/*
 * Balanced tree type functions
 *
 * Copyright (C) 2006-2016, Joachim Metz <joachim.metz@gmail.com>
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

#if !defined( _LIBCDATA_BTREE_H )
#define _LIBCDATA_BTREE_H

#include <common.h>
#include <types.h>

#include "libcdata_extern.h"
#include "libcdata_libcerror.h"
#include "libcdata_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

/* The balanced tree uses the list and tree node
 * The value of a tree node contains a list where each value in the list corresponds with either a sub node or a leaf value
 * The values array is used for sequential access to the leaf values
 */
typedef struct libcdata_internal_btree libcdata_internal_btree_t;

struct libcdata_internal_btree
{
	/* The values array
	 */
	libcdata_array_t *values_array;

	/* The root node
	 */
	libcdata_tree_node_t *root_node;

	/* The maximum number of values
	 */
	int maximum_number_of_values;
};

LIBCDATA_EXTERN \
int libcdata_btree_initialize(
     libcdata_btree_t **tree,
     int maximum_number_of_sub_nodes,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_btree_free(
     libcdata_btree_t **tree,
     int (*value_free_function)(
            intptr_t **value,
            libcerror_error_t **error ),
     libcerror_error_t **error );

int libcdata_btree_free_values_list(
     libcdata_list_t **values_list,
     libcerror_error_t **error );

/* TODO clone function */

int libcdata_btree_node_get_sub_node_by_value(
     libcdata_tree_node_t *node,
     intptr_t *value,
     int (*value_compare_function)(
            intptr_t *first_value,
            intptr_t *second_value,
            libcerror_error_t **error ),
     libcdata_tree_node_t **sub_node,
     libcdata_list_element_t **values_list_element,
     libcerror_error_t **error );

int libcdata_btree_node_get_upper_node_by_value(
     libcdata_tree_node_t *node,
     intptr_t *value,
     int (*value_compare_function)(
            intptr_t *first_value,
            intptr_t *second_value,
            libcerror_error_t **error ),
     libcdata_tree_node_t **upper_node,
     libcdata_list_element_t **values_list_element,
     libcerror_error_t **error );

int libcdata_btree_node_append_value(
     libcdata_tree_node_t *node,
     intptr_t *value,
     libcerror_error_t **error );

int libcdata_btree_node_insert_value(
     libcdata_tree_node_t *node,
     intptr_t *value,
     int (*value_compare_function)(
            intptr_t *first_value,
            intptr_t *second_value,
            libcerror_error_t **error ),
     libcerror_error_t **error );

int libcdata_btree_node_replace_value(
     libcdata_tree_node_t *node,
     intptr_t *value,
     intptr_t *replacement_value,
     libcerror_error_t **error );

int libcdata_btree_node_remove_value(
     libcdata_tree_node_t *node,
     intptr_t *value,
     intptr_t *replacement_value,
     libcerror_error_t **error );

int libcdata_btree_node_split(
     libcdata_tree_node_t *node,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_btree_get_number_of_values(
     libcdata_btree_t *tree,
     int *number_of_values,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_btree_get_value_by_index(
     libcdata_btree_t *tree,
     int value_index,
     intptr_t **value,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_btree_get_value_by_value(
     libcdata_btree_t *tree,
     intptr_t *value,
     int (*value_compare_function)(
            intptr_t *first_value,
            intptr_t *second_value,
            libcerror_error_t **error ),
     libcdata_tree_node_t **upper_node,
     intptr_t **existing_value,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_btree_insert_value(
     libcdata_btree_t *tree,
     int *value_index,
     intptr_t *value,
     int (*value_compare_function)(
            intptr_t *first_value,
            intptr_t *second_value,
            libcerror_error_t **error ),
     libcdata_tree_node_t **upper_node,
     intptr_t **existing_value,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_btree_replace_value(
     libcdata_btree_t *tree,
     libcdata_tree_node_t *upper_node,
     int *value_index,
     intptr_t *value,
     int *replacement_value_index,
     intptr_t *replacement_value,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_btree_remove_value(
     libcdata_btree_t *tree,
     libcdata_tree_node_t *upper_node,
     int *value_index,
     intptr_t *value,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBCDATA_BTREE_H ) */

