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

#include <common.h>
#include <memory.h>
#include <types.h>

#include "libfdata_btree_node.h"
#include "libfdata_btree_range.h"
#include "libfdata_definitions.h"
#include "libfdata_libcdata.h"
#include "libfdata_libcerror.h"
#include "libfdata_libcnotify.h"
#include "libfdata_types.h"

/* Creates a node
 * Make sure the value node is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libfdata_btree_node_initialize(
     libfdata_btree_node_t **node,
     int level,
     libcerror_error_t **error )
{
	libfdata_internal_btree_node_t *internal_tree_node = NULL;
	static char *function                              = "libfdata_btree_node_initialize";

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
	if( level < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_LESS_THAN_ZERO,
		 "%s: invalid level value less than zero.",
		 function );

		return( -1 );
	}
	internal_tree_node = memory_allocate_structure(
	                      libfdata_internal_btree_node_t );

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
	     sizeof( libfdata_internal_btree_node_t ) ) == NULL )
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
	internal_tree_node->level = level;
	internal_tree_node->flags = LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES;

	*node = (libfdata_btree_node_t *) internal_tree_node;

	return( 1 );

on_error:
	if( internal_tree_node != NULL )
	{
		memory_free(
		 internal_tree_node );
	}
	return( -1 );
}

/* Frees a node
 * Returns 1 if successful or -1 on error
 */
int libfdata_btree_node_free(
     libfdata_btree_node_t **node,
     libcerror_error_t **error )
{
	libfdata_internal_btree_node_t *internal_tree_node = NULL;
	static char *function                              = "libfdata_btree_node_free";
	int result                                         = 1;

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
		internal_tree_node = (libfdata_internal_btree_node_t *) *node;
		*node              = NULL;

		if( internal_tree_node->sub_node_ranges_array != NULL )
		{
			if( libcdata_array_free(
			     &( internal_tree_node->sub_node_ranges_array ),
			     (int (*)(intptr_t **, libcerror_error_t **error)) &libfdata_btree_range_free,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: unable to free the sub node ranges array.",
				 function );

				result = -1;
			}
		}
		if( internal_tree_node->leaf_value_ranges_array != NULL )
		{
			if( libcdata_array_free(
			     &( internal_tree_node->leaf_value_ranges_array ),
			     (int (*)(intptr_t **, libcerror_error_t **error)) &libfdata_btree_range_free,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: unable to free the leaf value ranges array.",
				 function );

				result = -1;
			}
		}
		memory_free(
		 internal_tree_node );
	}
	return( result );
}

/* Node functions
 */

/* Retrieves the level
 * Returns 1 if successful or -1 on error
 */
int libfdata_btree_node_get_level(
     libfdata_btree_node_t *node,
     int *level,
     libcerror_error_t **error )
{
	libfdata_internal_btree_node_t *internal_tree_node = NULL;
	static char *function                              = "libfdata_btree_node_get_level";

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
	internal_tree_node = (libfdata_internal_btree_node_t *) node;

	if( level == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid level.",
		 function );

		return( -1 );
	}
	*level = internal_tree_node->level;

	return( 1 );
}

/* Determines if the node is a branch node
 * Returns 1 if a branch node, 0 if not or -1 on error
 */
int libfdata_btree_node_is_branch(
     libfdata_btree_node_t *node,
     libcerror_error_t **error )
{
	libfdata_internal_btree_node_t *internal_tree_node = NULL;
	static char *function                              = "libfdata_btree_node_is_branch";

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
	internal_tree_node = (libfdata_internal_btree_node_t *) node;

	if( ( internal_tree_node->flags & LIBFDATA_BTREE_NODE_FLAG_IS_BRANCH ) != 0 )
	{
		return( 1 );
	}
	return( 0 );
}

/* Determines if the node is a leaf node
 * Returns 1 if a leaf node, 0 if not or -1 on error
 */
int libfdata_btree_node_is_leaf(
     libfdata_btree_node_t *node,
     libcerror_error_t **error )
{
	libfdata_internal_btree_node_t *internal_tree_node = NULL;
	static char *function                              = "libfdata_btree_node_is_leaf";

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
	internal_tree_node = (libfdata_internal_btree_node_t *) node;

	if( ( internal_tree_node->flags & LIBFDATA_BTREE_NODE_FLAG_IS_LEAF ) != 0 )
	{
		return( 1 );
	}
	return( 0 );
}

/* Determines if the node is a root node
 * Returns 1 if a root node, 0 if not or -1 on error
 */
int libfdata_btree_node_is_root(
     libfdata_btree_node_t *node,
     libcerror_error_t **error )
{
	libfdata_internal_btree_node_t *internal_tree_node = NULL;
	static char *function                              = "libfdata_btree_node_is_root";

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
	internal_tree_node = (libfdata_internal_btree_node_t *) node;

	if( internal_tree_node->level == 0 )
	{
		return( 1 );
	}
	return( 0 );
}

/* Sub node data range functions
 */

/* Retrieves the number of sub nodes
 * Returns 1 if successful or -1 on error
 */
int libfdata_btree_node_get_number_of_sub_nodes(
     libfdata_btree_node_t *node,
     int *number_of_sub_nodes,
     libcerror_error_t **error )
{
	libfdata_internal_btree_node_t *internal_tree_node = NULL;
	static char *function                              = "libfdata_btree_node_get_number_of_sub_nodes";

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
	internal_tree_node = (libfdata_internal_btree_node_t *) node;

	if( internal_tree_node->sub_node_ranges_array == NULL )
	{
		if( number_of_sub_nodes == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
			 "%s: invalid number of sub nodes.",
			 function );

			return( -1 );
		}
		*number_of_sub_nodes = 0;
	}
	else
	{
		if( libcdata_array_get_number_of_entries(
		     internal_tree_node->sub_node_ranges_array,
		     number_of_sub_nodes,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve number of entries from sub node ranges array.",
			 function );

			return( -1 );
		}
	}
	return( 1 );
}

/* Retrieves a specific sub node data range
 * Returns 1 if successful or -1 on error
 */
int libfdata_btree_node_get_sub_node_data_range_by_index(
     libfdata_btree_node_t *node,
     int sub_node_index,
     libfdata_btree_range_t **sub_node_data_range,
     libcerror_error_t **error )
{
	libfdata_internal_btree_node_t *internal_tree_node = NULL;
	static char *function                              = "libfdata_btree_node_get_sub_node_data_range_by_index";

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
	internal_tree_node = (libfdata_internal_btree_node_t *) node;

	if( internal_tree_node->sub_node_ranges_array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid node - missing sub node ranges array.",
		 function );

		return( -1 );
	}
	if( libcdata_array_get_entry_by_index(
	     internal_tree_node->sub_node_ranges_array,
	     sub_node_index,
	     (intptr_t **) sub_node_data_range,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry: %d from sub node ranges array.",
		 function,
		 sub_node_index );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves a sub node data range for a specific mapped index
 * Returns 1 if successful or -1 on error
 */
int libfdata_btree_node_get_sub_node_data_range_by_mapped_index(
     libfdata_btree_node_t *node,
     int mapped_index,
     libfdata_btree_range_t **sub_node_data_range,
     libcerror_error_t **error )
{
	libfdata_internal_btree_node_t *internal_tree_node = NULL;
	static char *function                              = "libfdata_btree_node_get_sub_node_data_range_by_mapped_index";
	int number_of_sub_nodes                            = 0;
	int sub_node_index                                 = 0;

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
	internal_tree_node = (libfdata_internal_btree_node_t *) node;

	if( internal_tree_node->sub_node_ranges_array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid node - missing sub node ranges array.",
		 function );

		return( -1 );
	}
	if( ( internal_tree_node->flags & LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: invalid node - unsupported flags calculate mapped ranges is set.",
		 function );

		return( -1 );
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: requested index: %d\n",
		 function,
		 mapped_index );
	}
#endif
	if( libcdata_array_get_number_of_entries(
	     internal_tree_node->sub_node_ranges_array,
	     &number_of_sub_nodes,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of entries from sub node ranges array.",
		 function );

		return( -1 );
	}
	for( sub_node_index = 0;
	     sub_node_index < number_of_sub_nodes;
	     sub_node_index++ )
	{
		if( libcdata_array_get_entry_by_index(
		     internal_tree_node->sub_node_ranges_array,
		     sub_node_index,
		     (intptr_t **) sub_node_data_range,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve entry: %d from sub node ranges array.",
			 function,
			 sub_node_index );

			return( -1 );
		}
		if( *sub_node_data_range == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing sub node: %d data range.",
			 function,
			 sub_node_index );

			return( -1 );
		}
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: mapped range: %d - %d\n",
			 function,
			 ( *sub_node_data_range )->mapped_first_leaf_value_index,
			 ( *sub_node_data_range )->mapped_last_leaf_value_index );
		}
#endif
		if( ( mapped_index >= ( *sub_node_data_range )->mapped_first_leaf_value_index )
		 && ( mapped_index <= ( *sub_node_data_range )->mapped_last_leaf_value_index ) )
		{
			break;
		}
	}
	if( sub_node_index >= number_of_sub_nodes )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve sub node data range.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves a sub node data range for a specific key
 *
 * Uses the key_value_compare_function to determine the similarity of the key values
 * The key_value_compare_function should return LIBFDATA_COMPARE_LESS,
 * LIBFDATA_COMPARE_LESS_EQUAL, LIBFDATA_COMPARE_EQUAL, LIBFDATA_COMPARE_GREATER,
 * LIBFDATA_COMPARE_GREATER_EQUAL if successful or -1 on error
 *
 * Returns 1 if successful, 0 if no such value or -1 on error
 */
int libfdata_btree_node_get_sub_node_data_range_by_key(
     libfdata_btree_node_t *node,
     intptr_t *key_value,
     int (*key_value_compare_function)(
            intptr_t *first_key_value,
            intptr_t *second_key_value,
            libcerror_error_t **error ),
     int *sub_node_index,
     libfdata_btree_range_t **sub_node_data_range,
     libcerror_error_t **error )
{
	libfdata_internal_btree_node_t *internal_tree_node = NULL;
	static char *function                              = "libfdata_btree_node_get_sub_node_data_range_by_key";
	int number_of_sub_nodes                            = 0;
	int result                                         = 0;
	int search_node_index                              = 0;

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
	internal_tree_node = (libfdata_internal_btree_node_t *) node;

	if( internal_tree_node->sub_node_ranges_array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid node - missing sub node ranges array.",
		 function );

		return( -1 );
	}
	if( ( internal_tree_node->flags & LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: invalid node - unsupported flags calculate mapped ranges is set.",
		 function );

		return( -1 );
	}
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
	if( libcdata_array_get_number_of_entries(
	     internal_tree_node->sub_node_ranges_array,
	     &number_of_sub_nodes,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of entries from sub node ranges array.",
		 function );

		return( -1 );
	}
	for( search_node_index = 0;
	     search_node_index < number_of_sub_nodes;
	     search_node_index++ )
	{
		if( libcdata_array_get_entry_by_index(
		     internal_tree_node->sub_node_ranges_array,
		     search_node_index,
		     (intptr_t **) sub_node_data_range,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve entry: %d from sub node ranges array.",
			 function,
			 search_node_index );

			return( -1 );
		}
		if( *sub_node_data_range == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing sub node: %d data range.",
			 function,
			 search_node_index );

			return( -1 );
		}
		if( key_value_compare_function == NULL )
		{
			if( key_value == ( *sub_node_data_range )->key_value )
			{
				*sub_node_index = search_node_index;

				return( 1 );
			}
		}
		else
		{
			result = key_value_compare_function(
				  key_value,
				  ( *sub_node_data_range )->key_value,
				  error );

			if( result == -1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to compare key value with sub node data range key value: %d.",
				 function,
				 search_node_index );

				return( -1 );
			}
			else if( ( result == LIBFDATA_COMPARE_EQUAL )
			      || ( result == LIBFDATA_COMPARE_GREATER_EQUAL )
			      || ( result == LIBFDATA_COMPARE_LESS_EQUAL ) )
			{
				*sub_node_index = search_node_index;

				return( 1 );
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
				 "%s: unsupported value compare function return value: %d.",
				 function,
				 result );

				return( -1 );
			}
		}
	}
	*sub_node_data_range = NULL;

	return( 0 );
}

/* Retrieves a specific sub node
 * Returns 1 if successful or -1 on error
 */
int libfdata_btree_node_get_sub_node_by_index(
     libfdata_btree_node_t *node,
     int sub_node_index,
     int *sub_node_data_file_index,
     off64_t *sub_node_data_offset,
     size64_t *sub_node_data_size,
     uint32_t *sub_node_data_flags,
     intptr_t **key_value,
     libcerror_error_t **error )
{
	libfdata_btree_range_t *data_range = NULL;
	static char *function              = "libfdata_btree_node_get_sub_node_by_index";

	if( libfdata_btree_node_get_sub_node_data_range_by_index(
	     node,
	     sub_node_index,
	     &data_range,
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
	if( libfdata_btree_range_get(
	     data_range,
	     sub_node_data_file_index,
	     sub_node_data_offset,
	     sub_node_data_size,
	     sub_node_data_flags,
	     key_value,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve values from sub node: %d data range.",
		 function,
		 sub_node_index );

		return( -1 );
	}
	return( 1 );
}

/* Sets the offset and size of a specific sub node
 * Returns 1 if successful or -1 on error
 */
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
     libcerror_error_t **error )
{
	libfdata_internal_btree_node_t *internal_tree_node = NULL;
	libfdata_btree_range_t *data_range                 = NULL;
	static char *function                              = "libfdata_btree_node_set_sub_node_by_index";

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
	internal_tree_node = (libfdata_internal_btree_node_t *) node;

	if( internal_tree_node->sub_node_ranges_array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid sub node index value out of bounds.",
		 function );

		return( -1 );
	}
	if( libcdata_array_get_entry_by_index(
	     internal_tree_node->sub_node_ranges_array,
	     sub_node_index,
	     (intptr_t **) &data_range,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry: %d from sub node ranges array.",
		 function,
		 sub_node_index );

		return( -1 );
	}
	if( libfdata_btree_range_set(
	     data_range,
	     sub_node_data_file_index,
	     sub_node_data_offset,
	     sub_node_data_size,
	     sub_node_data_flags,
	     key_value,
	     free_key_value,
	     key_value_flags,
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
     libcerror_error_t **error )
{
	libfdata_internal_btree_node_t *internal_tree_node = NULL;
	libfdata_btree_range_t *data_range                 = NULL;
	static char *function                              = "libfdata_btree_node_append_sub_node";

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
	internal_tree_node = (libfdata_internal_btree_node_t *) node;

	if( ( internal_tree_node->flags & LIBFDATA_BTREE_NODE_FLAG_IS_LEAF ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: invalid node - unsupported flags is leaf is set.",
		 function );

		return( -1 );
	}
	if( internal_tree_node->sub_node_ranges_array == NULL )
	{
		if( libcdata_array_initialize(
		     &( internal_tree_node->sub_node_ranges_array ),
		     0,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create sub node ranges array.",
			 function );

			goto on_error;
		}
	}
	if( libfdata_btree_range_initialize(
	     &data_range,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create data range.",
		 function );

		goto on_error;
	}
	if( libfdata_btree_range_set(
	     data_range,
	     sub_node_data_file_index,
	     sub_node_data_offset,
	     sub_node_data_size,
	     sub_node_data_flags,
	     key_value,
	     free_key_value,
	     key_value_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set data range.",
		 function );

		goto on_error;
	}
	if( libcdata_array_append_entry(
	     internal_tree_node->sub_node_ranges_array,
	     sub_node_index,
	     (intptr_t *) data_range,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
		 "%s: unable to append data range to sub node ranges array.",
		 function );

		goto on_error;
	}
	internal_tree_node->flags |= LIBFDATA_BTREE_NODE_FLAG_IS_BRANCH;

	return( 1 );

on_error:
	if( data_range != NULL )
	{
		libfdata_btree_range_free(
		 &data_range,
		 NULL );
	}
	return( -1 );
}

/* Branch leaf values functions
 */

/* Retrieves the number of leaf values in the branch
 * Returns 1 if successful or -1 on error
 */
int libfdata_btree_node_get_number_of_leaf_values_in_branch(
     libfdata_btree_node_t *node,
     int *number_of_leaf_values,
     libcerror_error_t **error )
{
	libfdata_internal_btree_node_t *internal_tree_node = NULL;
	static char *function                              = "libfdata_btree_node_get_number_of_leaf_values_in_branch";

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
	internal_tree_node = (libfdata_internal_btree_node_t *) node;

	if( number_of_leaf_values == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid number of leaf values.",
		 function );

		return( -1 );
	}
	*number_of_leaf_values = internal_tree_node->branch_number_of_leaf_values;

	return( 1 );
}

/* Leaf value data range functions
 */

/* Retrieves the number of leaf values
 * Returns 1 if successful or -1 on error
 */
int libfdata_btree_node_get_number_of_leaf_values(
     libfdata_btree_node_t *node,
     int *number_of_leaf_values,
     libcerror_error_t **error )
{
	libfdata_internal_btree_node_t *internal_tree_node = NULL;
	static char *function                              = "libfdata_btree_node_get_number_of_leaf_values";

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
	internal_tree_node = (libfdata_internal_btree_node_t *) node;

	if( internal_tree_node->leaf_value_ranges_array == NULL )
	{
		if( number_of_leaf_values == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
			 "%s: invalid number of leaf values.",
			 function );

			return( -1 );
		}
		*number_of_leaf_values = 0;
	}
	else
	{
		if( libcdata_array_get_number_of_entries(
		     internal_tree_node->leaf_value_ranges_array,
		     number_of_leaf_values,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve number of entries from leaf value ranges array.",
			 function );

			return( -1 );
		}
	}
	return( 1 );
}

/* Retrieves a specific leaf value data range
 * Returns 1 if successful or -1 on error
 */
int libfdata_btree_node_get_leaf_value_data_range_by_index(
     libfdata_btree_node_t *node,
     int leaf_value_index,
     libfdata_btree_range_t **leaf_value_data_range,
     libcerror_error_t **error )
{
	libfdata_internal_btree_node_t *internal_tree_node = NULL;
	static char *function                              = "libfdata_btree_node_get_leaf_value_data_range_by_index";

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
	internal_tree_node = (libfdata_internal_btree_node_t *) node;

	if( internal_tree_node->leaf_value_ranges_array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid node - missing leaf value ranges array.",
		 function );

		return( -1 );
	}
	if( libcdata_array_get_entry_by_index(
	     internal_tree_node->leaf_value_ranges_array,
	     leaf_value_index,
	     (intptr_t **) leaf_value_data_range,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry: %d from leaf value ranges array.",
		 function,
		 leaf_value_index );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves a leaf value data range for a specific mapped index
 * Returns 1 if successful or -1 on error
 */
int libfdata_btree_node_get_leaf_value_data_range_by_mapped_index(
     libfdata_btree_node_t *node,
     int mapped_index,
     libfdata_btree_range_t **leaf_value_data_range,
     libcerror_error_t **error )
{
	libfdata_internal_btree_node_t *internal_tree_node = NULL;
	static char *function                              = "libfdata_btree_node_get_leaf_value_data_range_by_mapped_index";
	int number_of_leaf_values                          = 0;
	int leaf_value_index                               = 0;

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
	internal_tree_node = (libfdata_internal_btree_node_t *) node;

	if( internal_tree_node->leaf_value_ranges_array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid node - missing leaf value ranges array.",
		 function );

		return( -1 );
	}
	if( ( internal_tree_node->flags & LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: invalid node - unsupported flags calculate mapped ranges is set.",
		 function );

		return( -1 );
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: requested index: %d\n",
		 function,
		 mapped_index );
	}
#endif
	if( libcdata_array_get_number_of_entries(
	     internal_tree_node->leaf_value_ranges_array,
	     &number_of_leaf_values,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of entries from leaf value ranges array.",
		 function );

		return( -1 );
	}
	for( leaf_value_index = 0;
	     leaf_value_index < number_of_leaf_values;
	     leaf_value_index++ )
	{
		if( libcdata_array_get_entry_by_index(
		     internal_tree_node->leaf_value_ranges_array,
		     leaf_value_index,
		     (intptr_t **) leaf_value_data_range,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve entry: %d from leaf value ranges array.",
			 function,
			 leaf_value_index );

			return( -1 );
		}
		if( *leaf_value_data_range == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing leaf value: %d data range.",
			 function,
			 leaf_value_index );

			return( -1 );
		}
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: mapped range: %d\n",
			 function,
			 ( *leaf_value_data_range )->mapped_first_leaf_value_index );
		}
#endif
		if( ( mapped_index == ( *leaf_value_data_range )->mapped_first_leaf_value_index )
		 && ( ( *leaf_value_data_range )->mapped_number_of_leaf_values == 1 ) )
		{
			break;
		}
	}
	if( leaf_value_index >= number_of_leaf_values )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve leaf value data range.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves a leaf value data range for a specific key
 *
 * Uses the key_value_compare_function to determine the similarity of the key values
 * The key_value_compare_function should return LIBFDATA_COMPARE_LESS,
 * LIBFDATA_COMPARE_EQUAL, LIBFDATA_COMPARE_GREATER if successful or -1 on error
 *
 * Returns 1 if successful, 0 if no such value or -1 on error
 */
int libfdata_btree_node_get_leaf_value_data_range_by_key(
     libfdata_btree_node_t *node,
     intptr_t *key_value,
     int (*key_value_compare_function)(
            intptr_t *first_key_value,
            intptr_t *second_key_value,
            libcerror_error_t **error ),
     libfdata_btree_range_t **leaf_value_data_range,
     libcerror_error_t **error )
{
	libfdata_internal_btree_node_t *internal_tree_node = NULL;
	static char *function                              = "libfdata_btree_node_get_leaf_value_data_range_by_key";
	int number_of_leaf_values                          = 0;
	int leaf_value_index                               = 0;
	int result                                         = 0;

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
	internal_tree_node = (libfdata_internal_btree_node_t *) node;

	if( internal_tree_node->leaf_value_ranges_array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid node - missing leaf value ranges array.",
		 function );

		return( -1 );
	}
	if( ( internal_tree_node->flags & LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: invalid node - unsupported flags calculate mapped ranges is set.",
		 function );

		return( -1 );
	}
	if( libcdata_array_get_number_of_entries(
	     internal_tree_node->leaf_value_ranges_array,
	     &number_of_leaf_values,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of entries from leaf value ranges array.",
		 function );

		return( -1 );
	}
	for( leaf_value_index = 0;
	     leaf_value_index < number_of_leaf_values;
	     leaf_value_index++ )
	{
		if( libcdata_array_get_entry_by_index(
		     internal_tree_node->leaf_value_ranges_array,
		     leaf_value_index,
		     (intptr_t **) leaf_value_data_range,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve entry: %d from leaf value ranges array.",
			 function,
			 leaf_value_index );

			return( -1 );
		}
		if( *leaf_value_data_range == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing leaf value: %d data range.",
			 function,
			 leaf_value_index );

			return( -1 );
		}
		if( key_value_compare_function == NULL )
		{
			if( key_value == ( *leaf_value_data_range )->key_value )
			{
				return( 1 );
			}
		}
		else
		{
			result = key_value_compare_function(
				  key_value,
				  ( *leaf_value_data_range )->key_value,
				  error );

			if( result == -1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to compare key value with leaf value data range key value: %d.",
				 function,
				 leaf_value_index );

				return( -1 );
			}
			else if( result == LIBFDATA_COMPARE_EQUAL )
			{
				return( 1 );
			}
			else if( result == LIBFDATA_COMPARE_LESS )
			{
				return( 0 );
			}
			else if( result != LIBFDATA_COMPARE_GREATER )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
				 "%s: unsupported value compare function return value: %d.",
				 function,
				 result );

				return( -1 );
			}
		}
	}
	return( 0 );
}

/* Retrieves a specific leaf value
 * Returns 1 if successful or -1 on error
 */
int libfdata_btree_node_get_leaf_value_by_index(
     libfdata_btree_node_t *node,
     int leaf_value_index,
     int *leaf_value_data_file_index,
     off64_t *leaf_value_data_offset,
     size64_t *leaf_value_data_size,
     uint32_t *leaf_value_data_flags,
     intptr_t **key_value,
     libcerror_error_t **error )
{
	libfdata_btree_range_t *data_range = NULL;
	static char *function              = "libfdata_btree_node_get_leaf_value_by_index";

	if( libfdata_btree_node_get_leaf_value_data_range_by_index(
	     node,
	     leaf_value_index,
	     &data_range,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve leaf value data range.",
		 function );

		return( -1 );
	}
	if( libfdata_btree_range_get(
	     data_range,
	     leaf_value_data_file_index,
	     leaf_value_data_offset,
	     leaf_value_data_size,
	     leaf_value_data_flags,
	     key_value,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve values from leaf value data range.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves a leaf value for a specific key
 *
 * Uses the key_value_compare_function to determine the similarity of the key values
 * The key_value_compare_function should return LIBFDATA_COMPARE_LESS,
 * LIBFDATA_COMPARE_EQUAL, LIBFDATA_COMPARE_GREATER if successful or -1 on error
 *
 * Returns 1 if successful, 0 if no such value or -1 on error
 */
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
     libcerror_error_t **error )
{
	libfdata_btree_range_t *data_range = NULL;
	intptr_t *data_range_key_value     = NULL;
	static char *function              = "libfdata_btree_node_get_leaf_value_by_key";

	if( libfdata_btree_node_get_leaf_value_data_range_by_key(
	     node,
	     key_value,
	     key_value_compare_function,
	     &data_range,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve leaf value data range.",
		 function );

		return( -1 );
	}
	if( libfdata_btree_range_get(
	     data_range,
	     leaf_value_data_file_index,
	     leaf_value_data_offset,
	     leaf_value_data_size,
	     leaf_value_data_flags,
	     &data_range_key_value,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve values from leaf value data range.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Sets the offset and size of a specific leaf value
 * Returns 1 if successful or -1 on error
 */
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
     libcerror_error_t **error )
{
	libfdata_internal_btree_node_t *internal_tree_node = NULL;
	libfdata_btree_range_t *data_range                 = NULL;
	static char *function                              = "libfdata_btree_node_set_leaf_value_by_index";

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
	internal_tree_node = (libfdata_internal_btree_node_t *) node;

	if( internal_tree_node->leaf_value_ranges_array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid leaf value index value out of bounds.",
		 function );

		return( -1 );
	}
	if( libcdata_array_get_entry_by_index(
	     internal_tree_node->leaf_value_ranges_array,
	     leaf_value_index,
	     (intptr_t **) &data_range,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry: %d from leaf value ranges array.",
		 function,
		 leaf_value_index );

		return( -1 );
	}
	if( libfdata_btree_range_set(
	     data_range,
	     leaf_value_data_file_index,
	     leaf_value_data_offset,
	     leaf_value_data_size,
	     leaf_value_data_flags,
	     key_value,
	     free_key_value,
	     key_value_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set leaf value: %d data range.",
		 function,
		 leaf_value_index );

		return( -1 );
	}
	return( 1 );
}

/* Appends a leaf value offset and size
 * Returns 1 if successful or -1 on error
 */
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
     libcerror_error_t **error )
{
	libfdata_internal_btree_node_t *internal_tree_node = NULL;
	libfdata_btree_range_t *data_range                 = NULL;
	static char *function                              = "libfdata_btree_node_append_leaf_value";

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
	internal_tree_node = (libfdata_internal_btree_node_t *) node;

	if( ( internal_tree_node->flags & LIBFDATA_BTREE_NODE_FLAG_IS_BRANCH ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: invalid node - unsupported flags is branch is set.",
		 function );

		return( -1 );
	}
	if( internal_tree_node->leaf_value_ranges_array == NULL )
	{
		if( libcdata_array_initialize(
		     &( internal_tree_node->leaf_value_ranges_array ),
		     0,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create leaf value ranges array.",
			 function );

			goto on_error;
		}
	}
	if( libfdata_btree_range_initialize(
	     &data_range,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create data range.",
		 function );

		goto on_error;
	}
	if( libfdata_btree_range_set(
	     data_range,
	     leaf_value_data_file_index,
	     leaf_value_data_offset,
	     leaf_value_data_size,
	     leaf_value_data_flags,
	     key_value,
	     free_key_value,
	     key_value_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set data range.",
		 function );

		goto on_error;
	}
	if( libcdata_array_append_entry(
	     internal_tree_node->leaf_value_ranges_array,
	     leaf_value_index,
	     (intptr_t *) data_range,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
		 "%s: unable to append data range to leaf value ranges array.",
		 function );

		goto on_error;
	}
	internal_tree_node->flags |= LIBFDATA_BTREE_NODE_FLAG_IS_LEAF;

	return( 1 );

on_error:
	if( data_range != NULL )
	{
		libfdata_btree_range_free(
		 &data_range,
		 NULL );
	}
	return( -1 );
}

