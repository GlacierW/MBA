/*
 * The tree node functions
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

#include <common.h>
#include <memory.h>
#include <types.h>

#include "libfdata_definitions.h"
#include "libfdata_libcdata.h"
#include "libfdata_libcerror.h"
#include "libfdata_libfcache.h"
#include "libfdata_range.h"
#include "libfdata_tree_node.h"
#include "libfdata_types.h"

/* Creates a node
 * Make sure the value node is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_node_initialize(
     libfdata_tree_node_t **node,
     libfdata_tree_t *tree,
     libfdata_tree_node_t *parent_node,
     libcerror_error_t **error )
{
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	static char *function                             = "libfdata_tree_node_initialize";

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	if( *node != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid node value already set.",
		 function );

		return( -1 );
	}
	if( tree == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid tree.",
		 function );

		return( -1 );
	}
	internal_tree_node = memory_allocate_structure(
	                      libfdata_internal_tree_node_t );

	if( internal_tree_node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create node.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_tree_node,
	     0,
	     sizeof( libfdata_internal_tree_node_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear node.",
		 function );

		memory_free(
		 internal_tree_node );

		return( -1 );
	}
	if( libfdata_range_initialize(
	     &( internal_tree_node->node_data_range ),
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create node data range.",
		 function );

		goto on_error;
	}
	if( libfdata_range_initialize(
	     &( internal_tree_node->sub_nodes_data_range ),
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create sub nodes data range.",
		 function );

		goto on_error;
	}
	if( libcdata_array_initialize(
	     &( internal_tree_node->sub_nodes ),
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create sub nodes array.",
		 function );

		goto on_error;
	}
	if( libfcache_date_time_get_timestamp(
	     &( internal_tree_node->timestamp ),
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve cache timestamp.",
		 function );

		goto on_error;
	}
	internal_tree_node->tree        = tree;
	internal_tree_node->parent_node = parent_node;
	internal_tree_node->flags       = LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES;

	*node = (libfdata_tree_node_t *) internal_tree_node;

	return( 1 );

on_error:
	if( internal_tree_node != NULL )
	{
		if( internal_tree_node->sub_nodes != NULL )
		{
			libcdata_array_free(
			 &( internal_tree_node->sub_nodes ),
			 NULL,
			 NULL );
		}
		if( internal_tree_node->sub_nodes_data_range != NULL )
		{
			libfdata_range_free(
			 &( internal_tree_node->sub_nodes_data_range ),
			 NULL );
		}
		if( internal_tree_node->node_data_range != NULL )
		{
			libfdata_range_free(
			 &( internal_tree_node->node_data_range ),
			 NULL );
		}
		memory_free(
		 internal_tree_node );
	}
	return( -1 );
}

/* Frees a node
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_node_free(
     libfdata_tree_node_t **node,
     libcerror_error_t **error )
{
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	static char *function                             = "libfdata_tree_node_free";
	int result                                        = 1;

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	if( *node != NULL )
	{
		internal_tree_node = (libfdata_internal_tree_node_t *) *node;
		*node              = NULL;

		if( internal_tree_node->node_data_range != NULL )
		{
			if( libfdata_range_free(
			     &( internal_tree_node->node_data_range ),
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: unable to free node data range.",
				 function );

				result = -1;
			}
		}
		if( internal_tree_node->sub_nodes_data_range != NULL )
		{
			if( libfdata_range_free(
			     &( internal_tree_node->sub_nodes_data_range ),
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: unable to free sub nodes data range.",
				 function );

				result = -1;
			}
		}
		if( libcdata_array_free(
		     &( internal_tree_node->sub_nodes ),
		     (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_tree_node_free,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free the sub nodes array.",
			 function );

			result = -1;
		}
		memory_free(
		 internal_tree_node );
	}
	return( result );
}

/* Frees a node, but not its sub nodes
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_node_free_single(
     libfdata_tree_node_t **node,
     libcerror_error_t **error )
{
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	static char *function                             = "libfdata_tree_node_free_single";
	int result                                        = 1;

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	if( *node != NULL )
	{
		internal_tree_node = (libfdata_internal_tree_node_t *) node;
		*node              = NULL;

		if( libcdata_array_free(
		     &( internal_tree_node->sub_nodes ),
		     NULL,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free the sub nodes array.",
			 function );

			result = -1;
		}
		memory_free(
		 internal_tree_node );
	}
	return( result );
}

/* Sets the parent node
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_node_set_parent_node(
     libfdata_tree_node_t *node,
     libfdata_tree_node_t *parent_node,
     libcerror_error_t **error )
{
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	static char *function                             = "libfdata_tree_node_set_parent_node";

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	internal_tree_node = (libfdata_internal_tree_node_t *) node;

	internal_tree_node->parent_node = parent_node;

	return( 1 );
}

/* Determines if the node is a root node
 * Returns 1 if a root node, 0 if not or -1 on error
 */
int libfdata_tree_node_is_root(
     libfdata_tree_node_t *node,
     libcerror_error_t **error )
{
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	static char *function                             = "libfdata_tree_node_is_root";

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	internal_tree_node = (libfdata_internal_tree_node_t *) node;

	if( internal_tree_node->parent_node == NULL )
	{
		return( 1 );
	}
	return( 0 );
}

/* Retrieves the data offset and size
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_node_get_data_range(
     libfdata_tree_node_t *node,
     int *node_file_index,
     off64_t *node_offset,
     size64_t *node_size,
     uint32_t *node_flags,
     libcerror_error_t **error )
{
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	static char *function                             = "libfdata_tree_node_get_data_range";

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	internal_tree_node = (libfdata_internal_tree_node_t *) node;

	if( libfdata_range_get(
	     internal_tree_node->node_data_range,
	     node_file_index,
	     node_offset,
	     node_size,
	     node_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve node data range.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Sets the data offset and size
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_node_set_data_range(
     libfdata_tree_node_t *node,
     int node_file_index,
     off64_t node_offset,
     size64_t node_size,
     uint32_t node_flags,
     libcerror_error_t **error )
{
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	static char *function                             = "libfdata_tree_node_set_data_range";

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	internal_tree_node = (libfdata_internal_tree_node_t *) node;

	if( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_IS_VIRTUAL ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid node - node is virtual.",
		 function );

		return( -1 );
	}
	if( libfdata_range_set(
	     internal_tree_node->node_data_range,
	     node_file_index,
	     node_offset,
	     node_size,
	     node_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set node data range.",
		 function );

		return( -1 );
	}
	if( libfcache_date_time_get_timestamp(
	     &( internal_tree_node->timestamp ),
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve cache timestamp.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Determines if the sub nodes data range has been set
 * Returns 1 if sub nodes have been set, 0 if not or -1 on error
 */
int libfdata_tree_node_sub_nodes_data_range_is_set(
     libfdata_tree_node_t *node,
     libcerror_error_t **error )
{
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	static char *function                             = "libfdata_tree_node_sub_nodes_data_range_is_set";

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	internal_tree_node = (libfdata_internal_tree_node_t *) node;

	if( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_SUB_NODES_DATA_RANGE_SET ) != 0 )
	{
		return( 1 );
	}
	return( 0 );
}

/* Retrieves the sub nodes data range
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_node_get_sub_nodes_data_range(
     libfdata_tree_node_t *node,
     int *sub_nodes_file_index,
     off64_t *sub_nodes_offset,
     size64_t *sub_nodes_size,
     uint32_t *sub_nodes_flags,
     libcerror_error_t **error )
{
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	static char *function                             = "libfdata_tree_node_get_sub_nodes_data_range";

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	internal_tree_node = (libfdata_internal_tree_node_t *) node;

	if( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_SUB_NODES_DATA_RANGE_SET ) == 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid node - sub nodes range has not been set.",
		 function );

		return( -1 );
	}
	if( libfdata_range_get(
	     internal_tree_node->sub_nodes_data_range,
	     sub_nodes_file_index,
	     sub_nodes_offset,
	     sub_nodes_size,
	     sub_nodes_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve sub nodes data range.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Sets the sub nodes data range
 * The sub nodes data range cannot be set after the sub nodes have been read
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_node_set_sub_nodes_data_range(
     libfdata_tree_node_t *node,
     int sub_nodes_file_index,
     off64_t sub_nodes_offset,
     size64_t sub_nodes_size,
     uint32_t sub_nodes_flags,
     libcerror_error_t **error )
{
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	static char *function                             = "libfdata_tree_node_set_sub_nodes_data_range";

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	internal_tree_node = (libfdata_internal_tree_node_t *) node;

	if( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_IS_VIRTUAL ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid node - node is virtual.",
		 function );

		return( -1 );
	}
	if( ( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_IS_VIRTUAL ) == 0 )
	 && ( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_IS_LEAF ) != 0 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid node - node is a leaf.",
		 function );

		return( -1 );
	}
	if( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_SUB_NODES_READ ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid node - sub nodes range has already been read.",
		 function );

		return( -1 );
	}
	if( libfdata_range_set(
	     internal_tree_node->sub_nodes_data_range,
	     sub_nodes_file_index,
	     sub_nodes_offset,
	     sub_nodes_size,
	     sub_nodes_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set sub nodes data range.",
		 function );

		return( -1 );
	}
	internal_tree_node->flags |= LIBFDATA_TREE_NODE_FLAG_SUB_NODES_DATA_RANGE_SET;

	return( 1 );
}

/* Retrieves the time stamp
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_node_get_timestamp(
     libfdata_tree_node_t *node,
     time_t *timestamp,
     libcerror_error_t **error )
{
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	static char *function                             = "libfdata_tree_node_get_timestamp";

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	internal_tree_node = (libfdata_internal_tree_node_t *) node;

	if( timestamp == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node time stamp.",
		 function );

		return( -1 );
	}
	*timestamp = internal_tree_node->timestamp;

	return( 1 );
}

/* Retrieves the first leaf node index
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_node_get_first_leaf_node_index(
     libfdata_tree_node_t *node,
     int *first_leaf_node_index,
     int *deleted_first_leaf_node_index,
     libcerror_error_t **error )
{
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	static char *function                             = "libfdata_tree_node_get_first_leaf_node_index";

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	internal_tree_node = (libfdata_internal_tree_node_t *) node;

	if( first_leaf_node_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid first leaf node index.",
		 function );

		return( -1 );
	}
	if( deleted_first_leaf_node_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid first deleted leaf node index.",
		 function );

		return( -1 );
	}
	*first_leaf_node_index         = internal_tree_node->first_leaf_node_index;
	*deleted_first_leaf_node_index = internal_tree_node->first_deleted_leaf_node_index;

	return( 1 );
}

/* Sets the first leaf node index
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_node_set_first_leaf_node_index(
     libfdata_tree_node_t *node,
     int first_leaf_node_index,
     int first_deleted_leaf_node_index,
     libcerror_error_t **error )
{
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	static char *function                             = "libfdata_tree_node_set_first_leaf_node_index";

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	internal_tree_node = (libfdata_internal_tree_node_t *) node;

	if( first_leaf_node_index < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_LESS_THAN_ZERO,
		 "%s: invalid first leaf node index value less than zero.",
		 function );

		return( -1 );
	}
	if( first_deleted_leaf_node_index < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_LESS_THAN_ZERO,
		 "%s: invalid first deleted leaf node index value less than zero.",
		 function );

		return( -1 );
	}
	internal_tree_node->first_leaf_node_index         = first_leaf_node_index;
	internal_tree_node->first_deleted_leaf_node_index = first_deleted_leaf_node_index;

	return( 1 );
}

/* Retrieves the node value
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_node_get_node_value(
     libfdata_tree_node_t *node,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     intptr_t **node_value,
     uint8_t read_flags,
     libcerror_error_t **error )
{
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	static char *function                             = "libfdata_tree_node_get_node_value";

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	internal_tree_node = (libfdata_internal_tree_node_t *) node;

	if( libfdata_tree_get_node_value(
	     internal_tree_node->tree,
	     file_io_handle,
	     cache,
	     node,
	     node_value,
	     read_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve node value.",
		 function );

		return( -1 );
	}
	internal_tree_node->flags |= LIBFDATA_TREE_NODE_FLAG_NOTE_DATA_READ;

	return( 1 );
}

/* Sets the node value
 *
 * If the flag LIBFDATA_TREE_NODE_VALUE_FLAG_MANAGED is set the node
 * takes over management of the value and the value is freed when
 * no longer needed.
 *
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_node_set_node_value(
     libfdata_tree_node_t *node,
     libfcache_cache_t *cache,
     intptr_t *node_value,
     int (*free_node_value)(
            intptr_t **node_value,
            libcerror_error_t **error ),
     uint8_t flags,
     libcerror_error_t **error )
{
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	static char *function                             = "libfdata_tree_node_set_node_value";

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	internal_tree_node = (libfdata_internal_tree_node_t *) node;

	if( libfdata_tree_set_node_value(
	     internal_tree_node->tree,
	     cache,
	     node,
	     node_value,
	     free_node_value,
	     flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set node value.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Resizes the sub nodes
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_node_resize_sub_nodes(
     libfdata_tree_node_t *node,
     int number_of_sub_nodes,
     libcerror_error_t **error )
{
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	static char *function                             = "libfdata_tree_node_resize_sub_nodes";

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	internal_tree_node = (libfdata_internal_tree_node_t *) node;

	if( ( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_IS_VIRTUAL ) == 0 )
	 && ( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_IS_LEAF ) != 0 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid node - node is a leaf.",
		 function );

		return( -1 );
	}
	if( libcdata_array_resize(
	     internal_tree_node->sub_nodes,
	     number_of_sub_nodes,
	     (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_tree_node_free_single,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_RESIZE_FAILED,
		 "%s: unable to resize sub nodes array.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the number of sub nodes
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_node_get_number_of_sub_nodes(
     libfdata_tree_node_t *node,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     int *number_of_sub_nodes,
     uint8_t read_flags,
     libcerror_error_t **error )
{
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	intptr_t *node_value                              = NULL;
	static char *function                             = "libfdata_tree_node_get_number_of_sub_nodes";

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	internal_tree_node = (libfdata_internal_tree_node_t *) node;

	if( ( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_IS_VIRTUAL ) == 0 )
	 && ( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_SUB_NODES_READ ) == 0 ) )
	{
		/* Make sure the node data is read before the sub nodes
		 */
		if( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_NOTE_DATA_READ ) == 0 )
		{
			if( libfdata_tree_get_node_value(
			     internal_tree_node->tree,
			     file_io_handle,
			     cache,
			     node,
			     &node_value,
			     read_flags,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve node value.",
				 function );

				return( -1 );
			}
			internal_tree_node->flags |= LIBFDATA_TREE_NODE_FLAG_NOTE_DATA_READ;
		}
		if( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_SUB_NODES_DATA_RANGE_SET ) != 0 )
		{
			if( libfdata_tree_read_sub_nodes(
			     internal_tree_node->tree,
			     file_io_handle,
			     cache,
			     node,
			     read_flags,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_IO,
				 LIBCERROR_IO_ERROR_READ_FAILED,
				 "%s: unable to read sub nodes.",
				 function );

				return( -1 );
			}
		}
		internal_tree_node->flags |= LIBFDATA_TREE_NODE_FLAG_SUB_NODES_READ;
	}
	if( libcdata_array_get_number_of_entries(
	     internal_tree_node->sub_nodes,
	     number_of_sub_nodes,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of entries from sub nodes array.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves a specific sub node
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_node_get_sub_node_by_index(
     libfdata_tree_node_t *node,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     int sub_node_index,
     libfdata_tree_node_t **sub_node,
     uint8_t read_flags,
     libcerror_error_t **error )
{
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	intptr_t *node_value                              = NULL;
	static char *function                             = "libfdata_tree_node_get_sub_node_by_index";

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	internal_tree_node = (libfdata_internal_tree_node_t *) node;

	if( ( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_IS_VIRTUAL ) == 0 )
	 && ( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_IS_LEAF ) != 0 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid node - node is a leaf.",
		 function );

		return( -1 );
	}
	if( ( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_IS_VIRTUAL ) == 0 )
	 && ( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_SUB_NODES_READ ) == 0 ) )
	{
		/* Make sure the node data is read before the sub nodes
		 */
		if( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_NOTE_DATA_READ ) == 0 )
		{
			if( libfdata_tree_get_node_value(
			     internal_tree_node->tree,
			     file_io_handle,
			     cache,
			     node,
			     &node_value,
			     read_flags,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve node value.",
				 function );

				return( -1 );
			}
			internal_tree_node->flags |= LIBFDATA_TREE_NODE_FLAG_NOTE_DATA_READ;
		}
		if( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_SUB_NODES_DATA_RANGE_SET ) != 0 )
		{
			if( libfdata_tree_read_sub_nodes(
			     internal_tree_node->tree,
			     file_io_handle,
			     cache,
			     node,
			     read_flags,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_IO,
				 LIBCERROR_IO_ERROR_READ_FAILED,
				 "%s: unable to read sub nodes.",
				 function );

				return( -1 );
			}
		}
		internal_tree_node->flags |= LIBFDATA_TREE_NODE_FLAG_SUB_NODES_READ;
	}
	if( libcdata_array_get_entry_by_index(
	     internal_tree_node->sub_nodes,
	     sub_node_index,
	     (intptr_t **) sub_node,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry: %d from sub nodes array.",
		 function,
		 sub_node_index );

		return( -1 );
	}
	return( 1 );
}

/* Sets the offset and size of a specific sub node
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_node_set_sub_node_by_index(
     libfdata_tree_node_t *node,
     int sub_node_index,
     int node_file_index,
     off64_t node_offset,
     size64_t node_size,
     uint32_t node_flags,
     libcerror_error_t **error )
{
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	libfdata_tree_node_t *sub_node                    = NULL;
	static char *function                             = "libfdata_tree_node_set_sub_node_by_index";
	off64_t previous_node_offset                      = 0;
	size64_t previous_node_size                       = 0;
	uint32_t previous_node_flags                      = 0;
	int previous_node_file_index                      = -1;

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	internal_tree_node = (libfdata_internal_tree_node_t *) node;

	if( ( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_IS_VIRTUAL ) == 0 )
	 && ( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_IS_LEAF ) != 0 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid node - node is a leaf.",
		 function );

		return( -1 );
	}
	if( libcdata_array_get_entry_by_index(
	     internal_tree_node->sub_nodes,
	     sub_node_index,
	     (intptr_t **) &sub_node,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry: %d from sub nodes array.",
		 function,
		 sub_node_index );

		return( -1 );
	}
	if( sub_node == NULL )
	{
		if( libfdata_tree_node_initialize(
		     &sub_node,
		     internal_tree_node->tree,
		     node,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create sub node.",
			 function );

			return( -1 );
		}
		if( libcdata_array_set_entry_by_index(
		     internal_tree_node->sub_nodes,
		     sub_node_index,
		     (intptr_t *) sub_node,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set sub node: %d in sub nodes array.",
			 function,
			 sub_node_index );

			libfdata_tree_node_free(
			 &sub_node,
			 NULL );

			return( -1 );
		}
	}
	else
	{
		if( libfdata_tree_node_get_data_range(
		     sub_node,
		     &previous_node_file_index,
		     &previous_node_offset,
		     &previous_node_size,
		     &previous_node_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve sub node: %d data range.",
			 function,
			 sub_node_index );

			return( -1 );
		}
	}
	if( libfdata_tree_node_set_data_range(
	     sub_node,
	     node_file_index,
	     node_offset,
	     node_size,
	     node_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set sub node: %d data range.",
		 function,
		 sub_node_index );

		return( -1 );
	}
	return( 1 );
}

/* Appends a sub node offset and size
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_node_append_sub_node(
     libfdata_tree_node_t *node,
     int *sub_node_index,
     int node_file_index,
     off64_t node_offset,
     size64_t node_size,
     uint32_t node_flags,
     libcerror_error_t **error )
{
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	libfdata_tree_node_t *sub_node                    = NULL;
	static char *function                             = "libfdata_tree_node_append_sub_node";

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	internal_tree_node = (libfdata_internal_tree_node_t *) node;

	if( ( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_IS_VIRTUAL ) == 0 )
	 && ( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_IS_LEAF ) != 0 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid node - node is a leaf.",
		 function );

		return( -1 );
	}
	if( libfdata_tree_node_initialize(
	     &sub_node,
	     internal_tree_node->tree,
	     node,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create sub node.",
		 function );

		goto on_error;
	}
	if( libfdata_tree_node_set_data_range(
	     sub_node,
	     node_file_index,
	     node_offset,
	     node_size,
	     node_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set sub node data range.",
		 function );

		goto on_error;
	}
	if( libcdata_array_append_entry(
	     internal_tree_node->sub_nodes,
	     sub_node_index,
	     (intptr_t *) sub_node,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
		 "%s: unable to append sub node to sub nodes array.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( sub_node != NULL )
	{
		libfdata_tree_node_free(
		 &sub_node,
		 NULL );
	}
	return( -1 );
}

/* Inserts a sub node offset and size
 *
 * Uses the node_value_compare_function to determine the order of the sub nodes
 * The node_value_compare_function should return LIBFDATA_COMPARE_LESS,
 * LIBFDATA_COMPARE_EQUAL, LIBFDATA_COMPARE_GREATER if successful or -1 on error
 *
 * Duplicate entries are allowed by default and inserted after the last duplicate entry.
 * Only allowing unique entries can be enforced by setting the flag LIBFDATA_TREE_NODE_INSERT_FLAG_UNIQUE_ENTRIES
 *
 * Returns 1 if successful, 0 if the node already exists or -1 on error
 */
int libfdata_tree_node_insert_sub_node(
     libfdata_tree_node_t *node,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     int *sub_node_index,
     int node_file_index,
     off64_t node_offset,
     size64_t node_size,
     uint32_t node_flags,
     int (*node_value_compare_function)(
            intptr_t *first_node_value,
            intptr_t *second_node_value,
            libcerror_error_t **error ),
     uint8_t insert_flags,
     uint8_t read_flags,
     libcerror_error_t **error )
{
	libfcache_cache_t *insert_cache                   = NULL;
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	libfdata_tree_node_t *insert_sub_node             = NULL;
	libfdata_tree_node_t *sub_node                    = NULL;
	intptr_t *insert_sub_node_value                   = NULL;
	intptr_t *sub_node_value                          = NULL;
	static char *function                             = "libfdata_tree_node_insert_sub_node";
	int number_of_sub_nodes                           = 0;
	int result                                        = 0;
	int sub_node_iterator                             = 0;

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	internal_tree_node = (libfdata_internal_tree_node_t *) node;

	if( sub_node_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid sub node index.",
		 function );

		return( -1 );
	}
	if( ( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_IS_VIRTUAL ) == 0 )
	 && ( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_IS_LEAF ) != 0 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid node - node is a leaf.",
		 function );

		return( -1 );
	}
	if( node_value_compare_function == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node value compare function.",
		 function );

		return( -1 );
	}
	if( ( insert_flags & ~( LIBFDATA_TREE_NODE_INSERT_FLAG_UNIQUE_SUB_NODE_VALUES ) ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported insert flags: 0x%02" PRIx8 ".",
		 function,
		 insert_flags );

		return( -1 );
	}
	if( libfdata_tree_node_initialize(
	     &insert_sub_node,
	     internal_tree_node->tree,
	     node,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create insert sub node.",
		 function );

		goto on_error;
	}
	if( libfdata_tree_node_set_data_range(
	     insert_sub_node,
	     node_file_index,
	     node_offset,
	     node_size,
	     node_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set insert sub node data range.",
		 function );

		goto on_error;
	}
	if( libcdata_array_get_number_of_entries(
	     internal_tree_node->sub_nodes,
	     &number_of_sub_nodes,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of entries from sub nodes array.",
		 function );

		goto on_error;
	}
	if( libfcache_cache_initialize(
	     &insert_cache,
	     1,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create insert cache.",
		 function );

		goto on_error;
	}
	if( libfdata_tree_get_node_value(
	     internal_tree_node->tree,
	     file_io_handle,
	     insert_cache,
	     insert_sub_node,
	     &insert_sub_node_value,
	     read_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve insert sub node value.",
		 function );

		goto on_error;
	}
	for( sub_node_iterator = 0;
	     sub_node_iterator < number_of_sub_nodes;
	     sub_node_iterator++ )
	{
		if( libcdata_array_get_entry_by_index(
		     internal_tree_node->sub_nodes,
		     sub_node_iterator,
		     (intptr_t **) &sub_node,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve entry: %d from sub nodes array.",
			 function,
			 sub_node_iterator );

			goto on_error;
		}
		if( libfdata_tree_get_node_value(
		     internal_tree_node->tree,
		     file_io_handle,
		     cache,
		     sub_node,
		     &sub_node_value,
		     read_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve sub node value: %d.",
			 function,
			 sub_node_iterator );

			goto on_error;
		}
		result = node_value_compare_function(
			  insert_sub_node_value,
			  sub_node_value,
			  error );

		if( result == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to compare sub node value: %d.",
			 function,
			 sub_node_iterator );

			goto on_error;
		}
		else if( result == LIBFDATA_COMPARE_EQUAL )
		{
			if( ( insert_flags & LIBFDATA_TREE_NODE_INSERT_FLAG_UNIQUE_SUB_NODE_VALUES ) != 0 )
			{
				if( libfcache_cache_free(
				     &insert_cache,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
					 "%s: unable to free the insert cache.",
					 function );

					goto on_error;
				}
				if( libfdata_tree_node_free(
				     &insert_sub_node,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
					 "%s: unable to free the insert sub node.",
					 function );

					goto on_error;
				}
				return( 0 );
			}
		}
		else if( result == LIBFDATA_COMPARE_LESS )
		{
			break;
		}
		else if( result != LIBFDATA_COMPARE_GREATER )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: unsupported node value compare function return value: %d.",
			 function,
			 result );

			goto on_error;
		}
	}
	if( libfcache_cache_free(
	     &insert_cache,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
		 "%s: unable to free the insert cache.",
		 function );

		goto on_error;
	}
	if( ( number_of_sub_nodes > 0 )
	 && ( result == LIBCDATA_COMPARE_LESS ) )
	{
		*sub_node_index = sub_node_iterator;

		if( libcdata_array_resize(
		     internal_tree_node->sub_nodes,
		     number_of_sub_nodes + 1,
		     (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_tree_node_free_single,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_RESIZE_FAILED,
			 "%s: unable to resize sub nodes array.",
			 function );

			goto on_error;
		}
		for( sub_node_iterator = number_of_sub_nodes;
		     sub_node_iterator > *sub_node_index;
		     sub_node_iterator-- )
		{
			if( libcdata_array_get_entry_by_index(
			     internal_tree_node->sub_nodes,
			     sub_node_iterator - 1,
			     (intptr_t **) &sub_node,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve sub node: %d from sub nodes array.",
				 function,
				 sub_node_iterator - 1 );

				goto on_error;
			}
			if( libcdata_array_set_entry_by_index(
			     internal_tree_node->sub_nodes,
			     sub_node_iterator,
			     (intptr_t *) sub_node,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set sub node: %d in sub nodes array.",
				 function,
				 sub_node_iterator );

				goto on_error;
			}
		}
		if( libcdata_array_set_entry_by_index(
		     internal_tree_node->sub_nodes,
		     *sub_node_index,
		     (intptr_t *) insert_sub_node,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set insert sub node: %d in sub nodes array.",
			 function,
			 *sub_node_index );

			goto on_error;
		}
	}
	else
	{
		*sub_node_index = number_of_sub_nodes;

		if( libcdata_array_resize(
		     internal_tree_node->sub_nodes,
		     number_of_sub_nodes + 1,
		     (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_tree_node_free_single,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_RESIZE_FAILED,
			 "%s: unable to resize sub nodes array.",
			 function );

			goto on_error;
		}
		if( libcdata_array_set_entry_by_index(
		     internal_tree_node->sub_nodes,
		     *sub_node_index,
		     (intptr_t *) insert_sub_node,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set insert sub node: %d in sub nodes array.",
			 function,
			 *sub_node_index );

			goto on_error;
		}
	}
	return( 1 );

on_error:
	if( insert_cache != NULL )
	{
		libfcache_cache_free(
		 &insert_cache,
		 NULL );
	}
	if( insert_sub_node != NULL )
	{
		libfdata_tree_node_free(
		 &insert_sub_node,
		 NULL );
	}
	return( -1 );
}

/* Splits the sub nodes
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_node_split_sub_nodes(
     libfdata_tree_node_t *node,
     int number_of_splits,
     libcerror_error_t **error )
{
	libcdata_array_t *virtual_sub_nodes_array         = NULL;
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	libfdata_tree_node_t *sub_node                    = NULL;
	libfdata_tree_node_t *virtual_sub_node            = NULL;
	static char *function                             = "libfdata_tree_node_split_sub_nodes";
	int number_of_split_sub_nodes                     = 0;
	int number_of_sub_nodes                           = 0;
	int result                                        = 1;
	int last_sub_node_index                           = 0;
	int split_sub_node_index                          = 0;
	int sub_node_index                                = 0;
	int virtual_sub_node_index                        = 0;

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	internal_tree_node = (libfdata_internal_tree_node_t *) node;

	if( ( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_IS_VIRTUAL ) == 0 )
	 && ( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_IS_LEAF ) != 0 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid node - node is a leaf.",
		 function );

		return( -1 );
	}
	if( number_of_splits < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_LESS_THAN_ZERO,
		 "%s: invalid number of splits value less than zero.",
		 function );

		return( -1 );
	}
	if( number_of_splits == 0 )
	{
		return( 1 );
	}
	if( libcdata_array_get_number_of_entries(
	     internal_tree_node->sub_nodes,
	     &number_of_sub_nodes,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of entries from sub nodes array.",
		 function );

		return( -1 );
	}
	number_of_split_sub_nodes = number_of_sub_nodes / number_of_splits;

	if( number_of_split_sub_nodes < 2 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: number of sub nodes too small to split in: %d.",
		 function,
		 number_of_splits );

		return( -1 );
	}
	if( libcdata_array_initialize(
	     &virtual_sub_nodes_array,
	     number_of_splits,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create virtual sub nodes array.",
		 function );

		return( -1 );
	}
	for( virtual_sub_node_index = 0;
	     virtual_sub_node_index < number_of_splits;
	     virtual_sub_node_index++ )
	{
		if( libfdata_tree_node_initialize(
		     &virtual_sub_node,
		     internal_tree_node->tree,
		     node,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create virtual sub node: %d.",
			 function,
			 virtual_sub_node_index );

			libcdata_array_free(
			 &virtual_sub_nodes_array,
			 (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_tree_node_free_single,
			 NULL );

			result = -1;

			break;
		}
		( (libfdata_internal_tree_node_t *) virtual_sub_node )->flags |= LIBFDATA_TREE_NODE_FLAG_IS_VIRTUAL;

		if( libcdata_array_set_entry_by_index(
		     virtual_sub_nodes_array,
		     virtual_sub_node_index,
		     (intptr_t *) virtual_sub_node,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set virtual sub node: %d in virtual sub nodes array.",
			 function,
			 virtual_sub_node_index );

			libfdata_tree_node_free(
			 &virtual_sub_node,
			 NULL );
			libcdata_array_free(
			 &virtual_sub_nodes_array,
			 (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_tree_node_free_single,
			 NULL );

			result = -1;

			break;
		}
		last_sub_node_index = ( number_of_split_sub_nodes * ( virtual_sub_node_index + 1 ) ) - 1;

		if( virtual_sub_node_index == ( number_of_splits - 1 ) )
		{
			last_sub_node_index += number_of_sub_nodes % number_of_splits;
		}
		if( last_sub_node_index > number_of_sub_nodes )
		{
			last_sub_node_index = number_of_sub_nodes - 1;
		}
		if( libcdata_array_resize(
		     ( (libfdata_internal_tree_node_t *) virtual_sub_node )->sub_nodes,
		     last_sub_node_index + 1 - sub_node_index,
		     (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_tree_node_free_single,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_RESIZE_FAILED,
			 "%s: unable to resize sub nodes array of virtual sub node: %d.",
			 function,
			 virtual_sub_node_index );

			libcdata_array_free(
			 &virtual_sub_nodes_array,
			 (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_tree_node_free_single,
			 NULL );

			result = -1;

			break;
		}
		split_sub_node_index = 0;

		while( sub_node_index <= last_sub_node_index )
		{
			if( libcdata_array_get_entry_by_index(
			     internal_tree_node->sub_nodes,
			     sub_node_index,
			     (intptr_t **) &sub_node,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve entry: %d from sub nodes array.",
				 function,
				 sub_node_index );

				libcdata_array_free(
				 &virtual_sub_nodes_array,
				 (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_tree_node_free_single,
				 NULL );

				result = -1;

				break;
			}
			if( split_sub_node_index == 0 )
			{
				if( sub_node == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: missing sub node: %d.",
					 function,
					 sub_node_index );

					libcdata_array_free(
					 &virtual_sub_nodes_array,
					 (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_tree_node_free_single,
					 NULL );

					result = -1;

					break;
				}
				if( ( (libfdata_internal_tree_node_t *) sub_node )->node_data_range == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: invalid sub node: %d - missing node data range.",
					 function,
					 sub_node_index );

					libcdata_array_free(
					 &virtual_sub_nodes_array,
					 (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_tree_node_free_single,
					 NULL );

					result = -1;

					break;
				}
				/* The node data range of a virtual node cannot be set by the set_data_range function
				 */
				if( libfdata_tree_node_get_data_range(
			             sub_node,
			             &( ( (libfdata_internal_tree_node_t *) virtual_sub_node )->node_data_range->file_index ),
			             &( ( (libfdata_internal_tree_node_t *) virtual_sub_node )->node_data_range->offset ),
			             &( ( (libfdata_internal_tree_node_t *) virtual_sub_node )->node_data_range->size ),
			             &( ( (libfdata_internal_tree_node_t *) virtual_sub_node )->node_data_range->flags ),
			             error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to set virtual sub node data range.",
					 function );

					libcdata_array_free(
					 &virtual_sub_nodes_array,
					 (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_tree_node_free_single,
					 NULL );

					result = -1;

					break;
				}
			}
			if( libcdata_array_set_entry_by_index(
			     ( (libfdata_internal_tree_node_t *) virtual_sub_node )->sub_nodes,
			     split_sub_node_index,
			     (intptr_t *) sub_node,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set entry: %d in sub nodes array of virtual sub node: %d.",
				 function,
				 split_sub_node_index,
				 virtual_sub_node_index );

				libcdata_array_free(
				 &virtual_sub_nodes_array,
				 (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_tree_node_free_single,
				 NULL );

				result = -1;

				break;
			}
			if( libfdata_tree_node_set_parent_node(
			     sub_node,
			     virtual_sub_node,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set parent node in sub node: %d.",
				 function,
				 split_sub_node_index );

				libcdata_array_free(
				 &virtual_sub_nodes_array,
				 (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_tree_node_free_single,
				 NULL );

				result = -1;

				break;
			}
			sub_node_index++;
			split_sub_node_index++;
		}
		virtual_sub_node = NULL;
	}
	if( result != 1 )
	{
		/* Set the parent values in the sub nodes back to their orginal value
		 */
		for( sub_node_index = 0;
		     sub_node_index < number_of_sub_nodes;
		     sub_node_index++ )
		{
			if( libcdata_array_get_entry_by_index(
			     internal_tree_node->sub_nodes,
			     sub_node_index,
			     (intptr_t **) &sub_node,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve entry: %d from sub nodes array.",
				 function,
				 sub_node_index );

				continue;
			}
			if( libfdata_tree_node_set_parent_node(
			     sub_node,
			     node,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set parent node in sub node: %d.",
				 function,
				 sub_node_index );

				continue;
			}
		}
		return( -1 );
	}
	if( libcdata_array_free(
	     &( internal_tree_node->sub_nodes ),
	     NULL,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
		 "%s: unable to free the sub nodes array.",
		 function );

		result = -1;
	}
	internal_tree_node->sub_nodes = virtual_sub_nodes_array;

	return( result );
}

/* Determines if the node is deleted
 * Returns 1 if deleted, 0 if not or -1 on error
 */
int libfdata_tree_node_is_deleted(
     libfdata_tree_node_t *node,
     libcerror_error_t **error )
{
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	static char *function                             = "libfdata_tree_node_is_deleted";

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	internal_tree_node = (libfdata_internal_tree_node_t *) node;

	if( ( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_IS_VIRTUAL ) == 0 )
	 && ( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_IS_DELETED ) != 0 ) )
	{
		return( 1 );
	}
	return( 0 );
}

/* Marks a node as deleted
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_node_set_deleted(
     libfdata_tree_node_t *node,
     libcerror_error_t **error )
{
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	static char *function                             = "libfdata_tree_node_set_deleted";

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	internal_tree_node = (libfdata_internal_tree_node_t *) node;

/* TODO add deleted branch node support */
	if( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_SUB_NODES_DATA_RANGE_SET ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: invalid node - currently only deleted leaf nodes are supported.",
		 function );

		return( -1 );
	}
	internal_tree_node->flags |= LIBFDATA_TREE_NODE_FLAG_IS_DELETED;

	if( libfdata_tree_node_set_calculate_leaf_node_values(
	     node,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set calculate leaf node values.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Makes a deleted node of a specific sub node
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_node_set_deleted_sub_node(
     libfdata_tree_node_t *node,
     int sub_node_index,
     libcerror_error_t **error )
{
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	libfdata_tree_node_t *sub_node                    = NULL;
	static char *function                             = "libfdata_tree_node_set_deleted_sub_node";

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	internal_tree_node = (libfdata_internal_tree_node_t *) node;

	if( libcdata_array_get_entry_by_index(
	     internal_tree_node->sub_nodes,
	     sub_node_index,
	     (intptr_t **) &sub_node,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry: %d from sub nodes array.",
		 function,
		 sub_node_index );

		return( -1 );
	}
	if( sub_node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing sub node: %d.",
		 function,
		 sub_node_index );

		return( -1 );
	}
	if( libfdata_tree_node_set_deleted(
	     sub_node,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to set deleted in sub node: %d.",
		 function,
		 sub_node_index );

		return( -1 );
	}
	return( 1 );
}

/* Sets the flag to calculate the leaf node values and propagates this up to its parent node
 * Returns 1 if the successful or -1 on error
 */
int libfdata_tree_node_set_calculate_leaf_node_values(
     libfdata_tree_node_t *node,
     libcerror_error_t **error )
{
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	static char *function                             = "libfdata_tree_node_set_calculate_leaf_node_values";

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	internal_tree_node = (libfdata_internal_tree_node_t *) node;

	if( internal_tree_node->parent_node != NULL )
	{
		if( libfdata_tree_node_set_calculate_leaf_node_values(
		     internal_tree_node->parent_node,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set calculate leaf node values in parent node.",
			 function );

			return( -1 );
		}
	}
	internal_tree_node->flags |= LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES;

	return( 1 );
}

/* Reads the leaf node values
 * Returns 1 if the successful or -1 on error
 */
int libfdata_tree_node_read_leaf_node_values(
     libfdata_tree_node_t *node,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     uint8_t read_flags,
     libcerror_error_t **error )
{
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	libfdata_tree_node_t *sub_node                    = NULL;
	intptr_t *node_value                              = NULL;
	static char *function                             = "libfdata_tree_node_read_leaf_node_values";
	int number_of_deleted_leaf_nodes                  = 0;
	int number_of_leaf_nodes                          = 0;
	int number_of_sub_nodes                           = 0;
	int result                                        = 0;
	int sub_node_index                                = 0;

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	internal_tree_node = (libfdata_internal_tree_node_t *) node;

	if( ( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_IS_VIRTUAL ) == 0 )
	 && ( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_SUB_NODES_READ ) == 0 ) )
	{
		/* Make sure the node data is read before the sub nodes
		 */
		if( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_NOTE_DATA_READ ) == 0 )
		{
			if( libfdata_tree_get_node_value(
			     internal_tree_node->tree,
			     file_io_handle,
			     cache,
			     node,
			     &node_value,
			     read_flags,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve node value.",
				 function );

				return( -1 );
			}
			internal_tree_node->flags |= LIBFDATA_TREE_NODE_FLAG_NOTE_DATA_READ;
		}
		if( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_SUB_NODES_DATA_RANGE_SET ) != 0 )
		{
			if( libfdata_tree_read_sub_nodes(
			     internal_tree_node->tree,
			     file_io_handle,
			     cache,
			     node,
			     read_flags,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_IO,
				 LIBCERROR_IO_ERROR_READ_FAILED,
				 "%s: unable to read sub nodes.",
				 function );

				return( -1 );
			}
		}
		internal_tree_node->flags |= LIBFDATA_TREE_NODE_FLAG_SUB_NODES_READ;
	}
	internal_tree_node->number_of_leaf_nodes         = 0;
	internal_tree_node->number_of_deleted_leaf_nodes = 0;

	if( libcdata_array_get_number_of_entries(
	     internal_tree_node->sub_nodes,
	     &number_of_sub_nodes,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of entries from sub nodes array.",
		 function );

		return( -1 );
	}
	for( sub_node_index = 0;
	     sub_node_index < number_of_sub_nodes;
	     sub_node_index++ )
	{
		if( libcdata_array_get_entry_by_index(
		     internal_tree_node->sub_nodes,
		     sub_node_index,
		     (intptr_t **) &sub_node,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve entry: %d from sub nodes array.",
			 function,
			 sub_node_index );

			return( -1 );
		}
		if( libfdata_tree_node_get_number_of_leaf_nodes(
		     sub_node,
		     file_io_handle,
		     cache,
		     &number_of_leaf_nodes,
		     read_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve number of leaf nodes from sub node: %d.",
			 function,
			 sub_node_index );

			return( -1 );
		}
		if( libfdata_tree_node_get_number_of_deleted_leaf_nodes(
		     sub_node,
		     file_io_handle,
		     cache,
		     &number_of_deleted_leaf_nodes,
		     read_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve number of deleted leaf nodes from sub node: %d.",
			 function,
			 sub_node_index );

			return( -1 );
		}
		if( libfdata_tree_node_set_first_leaf_node_index(
		     sub_node,
		     internal_tree_node->number_of_leaf_nodes,
		     internal_tree_node->number_of_deleted_leaf_nodes,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set first leaf node index in sub node: %d.",
			 function,
			 sub_node_index );

			return( -1 );
		}
		if( ( number_of_leaf_nodes == 0 )
		 && ( number_of_deleted_leaf_nodes == 0 ) )
		{
			/* It is possible to have branch nodes without leaf nodes
			 */
			result = libfdata_tree_node_is_leaf(
				  sub_node,
				  file_io_handle,
				  cache,
				  read_flags,
				  error );

			if( result == -1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to determine if sub node: %d is a leaf.",
				 function,
				 sub_node_index );

				return( -1 );
			}
			else if( result != 0 )
			{
				result = libfdata_tree_node_is_deleted(
					  sub_node,
					  error );

				if( result == -1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to determine if sub node: %d is deleted.",
					 function,
					 sub_node_index );

					return( -1 );
				}
				else if( result == 0 )
				{
					internal_tree_node->number_of_leaf_nodes++;
				}
				else
				{
					internal_tree_node->number_of_deleted_leaf_nodes++;
				}
			}
		}
		else
		{
			internal_tree_node->number_of_leaf_nodes         += number_of_leaf_nodes;
			internal_tree_node->number_of_deleted_leaf_nodes += number_of_deleted_leaf_nodes;
		}
	}
	return( 1 );
}

/* Determines if the node is a leaf node
 * Returns 1 if a leaf node, 0 if not or -1 on error
 */
int libfdata_tree_node_is_leaf(
     libfdata_tree_node_t *node,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     uint8_t read_flags,
     libcerror_error_t **error )
{
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	intptr_t *node_value                              = NULL;
	static char *function                             = "libfdata_tree_node_is_leaf";

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	internal_tree_node = (libfdata_internal_tree_node_t *) node;

	if( ( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_IS_VIRTUAL ) == 0 )
	 && ( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_SUB_NODES_READ ) == 0 ) )
	{
		/* Make sure the node data is read before the sub nodes
		 */
		if( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_NOTE_DATA_READ ) == 0 )
		{
			if( libfdata_tree_get_node_value(
			     internal_tree_node->tree,
			     file_io_handle,
			     cache,
			     node,
			     &node_value,
			     read_flags,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve node value.",
				 function );

				return( -1 );
			}
			internal_tree_node->flags |= LIBFDATA_TREE_NODE_FLAG_NOTE_DATA_READ;
		}
		if( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_SUB_NODES_DATA_RANGE_SET ) != 0 )
		{
			if( libfdata_tree_read_sub_nodes(
			     internal_tree_node->tree,
			     file_io_handle,
			     cache,
			     node,
			     read_flags,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_IO,
				 LIBCERROR_IO_ERROR_READ_FAILED,
				 "%s: unable to read sub nodes.",
				 function );

				return( -1 );
			}
		}
		internal_tree_node->flags |= LIBFDATA_TREE_NODE_FLAG_SUB_NODES_READ;
	}
	if( ( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_IS_VIRTUAL ) == 0 )
	 && ( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_IS_LEAF ) != 0 ) )
	{
		return( 1 );
	}
	return( 0 );
}

/* Makes a leaf node
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_node_set_leaf(
     libfdata_tree_node_t *node,
     libcerror_error_t **error )
{
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	static char *function                             = "libfdata_tree_node_set_leaf";

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	internal_tree_node = (libfdata_internal_tree_node_t *) node;

	if( ( internal_tree_node->flags & LIBFDATA_TREE_NODE_FLAG_SUB_NODES_DATA_RANGE_SET ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid node - sub nodes range was set.",
		 function );

		return( -1 );
	}
	internal_tree_node->flags |= LIBFDATA_TREE_NODE_FLAG_IS_LEAF;

	if( libfdata_tree_node_set_calculate_leaf_node_values(
	     node,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set calculate leaf node values.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Makes a leaf node of a specific sub node
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_node_set_leaf_sub_node(
     libfdata_tree_node_t *node,
     int sub_node_index,
     libcerror_error_t **error )
{
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	libfdata_tree_node_t *sub_node                    = NULL;
	static char *function                             = "libfdata_tree_node_set_leaf_sub_node";

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	internal_tree_node = (libfdata_internal_tree_node_t *) node;

	if( libcdata_array_get_entry_by_index(
	     internal_tree_node->sub_nodes,
	     sub_node_index,
	     (intptr_t **) &sub_node,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry: %d from sub nodes array.",
		 function,
		 sub_node_index );

		return( -1 );
	}
	if( sub_node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing sub node: %d.",
		 function,
		 sub_node_index );

		return( -1 );
	}
	if( libfdata_tree_node_set_leaf(
	     sub_node,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to set leaf in sub node: %d.",
		 function,
		 sub_node_index );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the number of leaf nodes
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_node_get_number_of_leaf_nodes(
     libfdata_tree_node_t *node,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     int *number_of_leaf_nodes,
     uint8_t read_flags,
     libcerror_error_t **error )
{
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	static char *function                             = "libfdata_tree_node_get_number_of_leaf_nodes";

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	internal_tree_node = (libfdata_internal_tree_node_t *) node;

	if( number_of_leaf_nodes == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid number of leaf nodes.",
		 function );

		return( -1 );
	}
	if( ( internal_tree_node->flags & LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES ) != 0 )
	{
		if( libfdata_tree_node_read_leaf_node_values(
		     node,
		     file_io_handle,
		     cache,
		     read_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve node value.",
			 function );

			return( -1 );
		}
		internal_tree_node->flags &= ~( LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES );
	}
	*number_of_leaf_nodes = internal_tree_node->number_of_leaf_nodes;

	return( 1 );
}

/* Retrieves a specific leaf node
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_node_get_leaf_node_by_index(
     libfdata_tree_node_t *node,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     int leaf_node_index,
     libfdata_tree_node_t **leaf_node,
     uint8_t read_flags,
     libcerror_error_t **error )
{
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	libfdata_tree_node_t *sub_node                    = NULL;
	static char *function                             = "libfdata_tree_node_get_leaf_node_by_index";
	int first_deleted_leaf_node_index                 = 0;
	int first_leaf_node_index                         = 0;
	int number_of_leaf_nodes                          = 0;
	int number_of_sub_nodes                           = 0;
	int result                                        = 0;
	int sub_node_index                                = 0;

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	internal_tree_node = (libfdata_internal_tree_node_t *) node;

	if( leaf_node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid leaf node.",
		 function );

		return( -1 );
	}
	if( ( internal_tree_node->flags & LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES ) != 0 )
	{
		if( libfdata_tree_node_read_leaf_node_values(
		     node,
		     file_io_handle,
		     cache,
		     read_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve node value.",
			 function );

			return( -1 );
		}
		internal_tree_node->flags &= ~( LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES );
	}
	if( libcdata_array_get_number_of_entries(
	     internal_tree_node->sub_nodes,
	     &number_of_sub_nodes,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of entries from sub nodes array.",
		 function );

		return( -1 );
	}
	for( sub_node_index = 0;
	     sub_node_index < number_of_sub_nodes;
	     sub_node_index++ )
	{
		if( libcdata_array_get_entry_by_index(
		     internal_tree_node->sub_nodes,
		     sub_node_index,
		     (intptr_t **) &sub_node,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve entry: %d from sub nodes array.",
			 function,
			 sub_node_index );

			return( -1 );
		}
		if( libfdata_tree_node_get_number_of_leaf_nodes(
		     sub_node,
		     file_io_handle,
		     cache,
		     &number_of_leaf_nodes,
		     read_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve number of leaf nodes from sub node: %d.",
			 function,
			 sub_node_index );

			return( -1 );
		}
		if( libfdata_tree_node_get_first_leaf_node_index(
		     sub_node,
		     &first_leaf_node_index,
		     &first_deleted_leaf_node_index,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve first leaf node index from sub node: %d.",
			 function,
			 sub_node_index );

			return( -1 );
		}
		if( number_of_leaf_nodes == 0 )
		{
			/* It is possible to have branch nodes without leaf nodes
			 */
			result = libfdata_tree_node_is_leaf(
				  sub_node,
				  file_io_handle,
				  cache,
				  read_flags,
				  error );

			if( result == -1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to determine if sub node: %d is a leaf.",
				 function,
				 sub_node_index );

				return( -1 );
			}
			else if( result != 0 )
			{
				result = libfdata_tree_node_is_deleted(
					  sub_node,
					  error );

				if( result == -1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to determine if sub node: %d is deleted.",
					 function,
					 sub_node_index );

					return( -1 );
				}
				else if( result == 0 )
				{
					if( leaf_node_index == first_leaf_node_index )
					{
						*leaf_node = sub_node;

						break;
					}
				}
			}
		}
		else if( ( leaf_node_index >= first_leaf_node_index )
		      && ( leaf_node_index < ( first_leaf_node_index + number_of_leaf_nodes ) ) )
		{
			/* Note that the first_leaf_node_index is relative for a branch
			 */
			if( libfdata_tree_node_get_leaf_node_by_index(
			     sub_node,
			     file_io_handle,
			     cache,
			     leaf_node_index - first_leaf_node_index,
			     leaf_node,
			     read_flags,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve leaf node: %d from sub node: %d.",
				 function,
				 leaf_node_index - first_leaf_node_index,
				 sub_node_index );

				return( -1 );
			}
			break;
		}
	}
	if( sub_node_index >= number_of_sub_nodes )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid sub node index value out of bounds.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the number of deleted leaf nodes
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_node_get_number_of_deleted_leaf_nodes(
     libfdata_tree_node_t *node,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     int *number_of_deleted_leaf_nodes,
     uint8_t read_flags,
     libcerror_error_t **error )
{
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	static char *function                             = "libfdata_tree_node_get_number_of_deleted_leaf_nodes";

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	internal_tree_node = (libfdata_internal_tree_node_t *) node;

	if( number_of_deleted_leaf_nodes == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid number of deleted leaf nodes.",
		 function );

		return( -1 );
	}
	if( ( internal_tree_node->flags & LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES ) != 0 )
	{
		if( libfdata_tree_node_read_leaf_node_values(
		     node,
		     file_io_handle,
		     cache,
		     read_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve node value.",
			 function );

			return( -1 );
		}
		internal_tree_node->flags &= ~( LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES );
	}
	*number_of_deleted_leaf_nodes = internal_tree_node->number_of_deleted_leaf_nodes;

	return( 1 );
}

/* Retrieves a specific deleted leaf node
 * Returns 1 if successful or -1 on error
 */
int libfdata_tree_node_get_deleted_leaf_node_by_index(
     libfdata_tree_node_t *node,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     int deleted_leaf_node_index,
     libfdata_tree_node_t **deleted_leaf_node,
     uint8_t read_flags,
     libcerror_error_t **error )
{
	libfdata_internal_tree_node_t *internal_tree_node = NULL;
	libfdata_tree_node_t *sub_node                    = NULL;
	static char *function                             = "libfdata_tree_node_get_deleted_leaf_node_by_index";
	int first_deleted_leaf_node_index                 = 0;
	int first_leaf_node_index                         = 0;
	int number_of_deleted_leaf_nodes                  = 0;
	int number_of_sub_nodes                           = 0;
	int result                                        = 0;
	int sub_node_index                                = 0;

	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid node.",
		 function );

		return( -1 );
	}
	internal_tree_node = (libfdata_internal_tree_node_t *) node;

	if( deleted_leaf_node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid deleted leaf node.",
		 function );

		return( -1 );
	}
	if( ( internal_tree_node->flags & LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES ) != 0 )
	{
		if( libfdata_tree_node_read_leaf_node_values(
		     node,
		     file_io_handle,
		     cache,
		     read_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve node value.",
			 function );

			return( -1 );
		}
		internal_tree_node->flags &= ~( LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES );
	}
	if( libcdata_array_get_number_of_entries(
	     internal_tree_node->sub_nodes,
	     &number_of_sub_nodes,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of entries from sub nodes array.",
		 function );

		return( -1 );
	}
	for( sub_node_index = 0;
	     sub_node_index < number_of_sub_nodes;
	     sub_node_index++ )
	{
		if( libcdata_array_get_entry_by_index(
		     internal_tree_node->sub_nodes,
		     sub_node_index,
		     (intptr_t **) &sub_node,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve entry: %d from sub nodes array.",
			 function,
			 sub_node_index );

			return( -1 );
		}
		if( libfdata_tree_node_get_number_of_deleted_leaf_nodes(
		     sub_node,
		     file_io_handle,
		     cache,
		     &number_of_deleted_leaf_nodes,
		     read_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve number of leaf nodes from sub node: %d.",
			 function,
			 sub_node_index );

			return( -1 );
		}
		if( libfdata_tree_node_get_first_leaf_node_index(
		     sub_node,
		     &first_leaf_node_index,
		     &first_deleted_leaf_node_index,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve first leaf node index from sub node: %d.",
			 function,
			 sub_node_index );

			return( -1 );
		}
		if( number_of_deleted_leaf_nodes == 0 )
		{
			/* It is possible to have branch nodes without leaf nodes
			 */
			result = libfdata_tree_node_is_leaf(
				  sub_node,
				  file_io_handle,
				  cache,
				  read_flags,
				  error );

			if( result == -1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to determine if sub node: %d is a leaf.",
				 function,
				 sub_node_index );

				return( -1 );
			}
			else if( result != 0 )
			{
				result = libfdata_tree_node_is_deleted(
					  sub_node,
					  error );

				if( result == -1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to determine if sub node: %d is deleted.",
					 function,
					 sub_node_index );

					return( -1 );
				}
				else if( result != 0 )
				{
					if( deleted_leaf_node_index == first_deleted_leaf_node_index )
					{
						*deleted_leaf_node = sub_node;

						break;
					}
				}
			}
		}
		else if( ( deleted_leaf_node_index >= first_deleted_leaf_node_index )
		      && ( deleted_leaf_node_index < ( first_deleted_leaf_node_index + number_of_deleted_leaf_nodes ) ) )
		{
			/* Note that the first_deleted_leaf_node_index is relative for a branch
			 */
			if( libfdata_tree_node_get_deleted_leaf_node_by_index(
			     sub_node,
			     file_io_handle,
			     cache,
			     deleted_leaf_node_index - first_deleted_leaf_node_index,
			     deleted_leaf_node,
			     read_flags,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve deleted leaf node: %d from sub node: %d.",
				 function,
				 deleted_leaf_node_index - first_deleted_leaf_node_index,
				 sub_node_index );

				return( -1 );
			}
			break;
		}
	}
	if( sub_node_index >= number_of_sub_nodes )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid sub node index value out of bounds.",
		 function );

		return( -1 );
	}
	return( 1 );
}

