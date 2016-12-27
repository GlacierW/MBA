/*
 * Tree functions
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

#if !defined( _LIBCDATA_TREE_NODE_H )
#define _LIBCDATA_TREE_NODE_H

#include <common.h>
#include <types.h>

#include "libcdata_extern.h"
#include "libcdata_libcerror.h"
#include "libcdata_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libcdata_internal_tree_node libcdata_internal_tree_node_t;

struct libcdata_internal_tree_node
{
	/* The parent node
	 */
	libcdata_tree_node_t *parent_node;

	/* The previous node
	 */
	libcdata_tree_node_t *previous_node;

	/* The next node
	 */
	libcdata_tree_node_t *next_node;

	/* The first sub node
	 */
	libcdata_tree_node_t *first_sub_node;

	/* The last sub node
	 */
	libcdata_tree_node_t *last_sub_node;

	/* The number of sub nodes
	 */
	int number_of_sub_nodes;

	/* The node value
	 */
	intptr_t *value;
};

LIBCDATA_EXTERN \
int libcdata_tree_node_initialize(
     libcdata_tree_node_t **node,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_tree_node_free(
     libcdata_tree_node_t **node,
     int (*value_free_function)(
            intptr_t **value,
            libcerror_error_t **error ),
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_tree_node_empty(
     libcdata_tree_node_t *node,
     int (*value_free_function)(
            intptr_t **value,
            libcerror_error_t **error ),
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_tree_node_clone(
     libcdata_tree_node_t **destination_node,
     libcdata_tree_node_t *source_node,
     int (*value_free_function)(
            intptr_t **value,
            libcerror_error_t **error ),
     int (*value_clone_function)(
            intptr_t **destination_value,
            intptr_t *source_value,
            libcerror_error_t **error ),
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_tree_node_get_value(
     libcdata_tree_node_t *node,
     intptr_t **value,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_tree_node_set_value(
     libcdata_tree_node_t *node,
     intptr_t *value,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_tree_node_get_parent_node(
     libcdata_tree_node_t *node,
     libcdata_tree_node_t **parent_node,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_tree_node_set_parent_node(
     libcdata_tree_node_t *node,
     libcdata_tree_node_t *parent_node,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_tree_node_get_previous_node(
     libcdata_tree_node_t *node,
     libcdata_tree_node_t **previous_node,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_tree_node_set_previous_node(
     libcdata_tree_node_t *node,
     libcdata_tree_node_t *previous_node,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_tree_node_get_next_node(
     libcdata_tree_node_t *node,
     libcdata_tree_node_t **next_node,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_tree_node_set_next_node(
     libcdata_tree_node_t *node,
     libcdata_tree_node_t *next_node,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_tree_node_get_nodes(
     libcdata_tree_node_t *node,
     libcdata_tree_node_t **parent_node,
     libcdata_tree_node_t **previous_node,
     libcdata_tree_node_t **next_node,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_tree_node_set_nodes(
     libcdata_tree_node_t *node,
     libcdata_tree_node_t *parent_node,
     libcdata_tree_node_t *previous_node,
     libcdata_tree_node_t *next_node,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_tree_node_append_node(
     libcdata_tree_node_t *parent_node,
     libcdata_tree_node_t *node,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_tree_node_append_value(
     libcdata_tree_node_t *parent_node,
     intptr_t *value,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_tree_node_insert_node(
     libcdata_tree_node_t *parent_node,
     libcdata_tree_node_t *node,
     int (*value_compare_function)(
            intptr_t *first_value,
            intptr_t *second_value,
            libcerror_error_t **error ),
     uint8_t insert_flags,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_tree_node_insert_value(
     libcdata_tree_node_t *parent_node,
     intptr_t *value,
     int (*value_compare_function)(
            intptr_t *first_value,
            intptr_t *second_value,
            libcerror_error_t **error ),
     uint8_t insert_flags,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_tree_node_replace_node(
     libcdata_tree_node_t *node,
     libcdata_tree_node_t *replacement_node,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_tree_node_remove_node(
     libcdata_tree_node_t *parent_node,
     libcdata_tree_node_t *node,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_tree_node_get_number_of_sub_nodes(
     libcdata_tree_node_t *node,
     int *number_of_sub_nodes,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_tree_node_get_sub_node_by_index(
     libcdata_tree_node_t *node,
     int sub_node_index,
     libcdata_tree_node_t **sub_node,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_tree_node_get_leaf_node_list(
     libcdata_tree_node_t *node,
     libcdata_list_t **leaf_node_list,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBCDATA_TREE_NODE_H ) */

