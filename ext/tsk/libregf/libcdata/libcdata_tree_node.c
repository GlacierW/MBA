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

#include <common.h>
#include <memory.h>
#include <types.h>

#include "libcdata_definitions.h"
#include "libcdata_libcerror.h"
#include "libcdata_list.h"
#include "libcdata_tree_node.h"
#include "libcdata_types.h"

/* Creates a tree node
 * Make sure the value node is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libcdata_tree_node_initialize(
     libcdata_tree_node_t **node,
     libcerror_error_t **error )
{
	libcdata_internal_tree_node_t *internal_node = NULL;
	static char *function                        = "libcdata_tree_node_initialize";

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
	internal_node = memory_allocate_structure(
	                 libcdata_internal_tree_node_t );

	if( internal_node == NULL )
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
	     internal_node,
	     0,
	     sizeof( libcdata_internal_tree_node_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear node.",
		 function );

		goto on_error;
	}
	*node = (libcdata_tree_node_t *) internal_node;

	return( 1 );

on_error:
	if( internal_node != NULL )
	{
		memory_free(
		 internal_node );
	}
	return( -1 );
}

/* Frees a tree node, its sub nodes
 * Uses the value_free_function to free the value
 * Returns 1 if successful or -1 on error
 */
int libcdata_tree_node_free(
     libcdata_tree_node_t **node,
     int (*value_free_function)(
            intptr_t **value,
            libcerror_error_t **error ),
     libcerror_error_t **error )
{
	libcdata_internal_tree_node_t *internal_node = NULL;
	libcdata_tree_node_t *next_node              = NULL;
	libcdata_tree_node_t *parent_node            = NULL;
	libcdata_tree_node_t *previous_node          = NULL;
	libcdata_tree_node_t *sub_node               = NULL;
	static char *function                        = "libcdata_tree_node_free";
	int number_of_sub_nodes                      = 0;
	int result                                   = 1;
	int sub_node_index                           = 0;

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
		internal_node = (libcdata_internal_tree_node_t *) *node;

		if( ( internal_node->parent_node != NULL )
		 || ( internal_node->previous_node != NULL )
		 || ( internal_node->next_node != NULL ) )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
			 "%s: invalid node - connected to other nodes.",
			 function );

			return( -1 );
		}
		number_of_sub_nodes = internal_node->number_of_sub_nodes;

		sub_node = internal_node->first_sub_node;

		for( sub_node_index = 0;
		     sub_node_index < number_of_sub_nodes;
		     sub_node_index++ )
		{
			if( libcdata_tree_node_get_nodes(
			     sub_node,
			     &parent_node,
			     &previous_node,
			     &next_node,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve nodes of sub node: %d.",
				 function,
				 sub_node_index );

				return( -1 );
			}
			if( previous_node != NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
				 "%s: invalid sub node: %d - previous node is set.",
				 function,
				 sub_node_index );

				return( -1 );
			}
			internal_node->first_sub_node = next_node;

			if( internal_node->last_sub_node == sub_node )
			{
				internal_node->last_sub_node = next_node;
			}
			internal_node->number_of_sub_nodes--;

			if( next_node != NULL )
			{
				if( libcdata_tree_node_set_previous_node(
				     next_node,
				     NULL,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to set previous node of sub node: %d.",
					 function,
					 sub_node_index + 1 );

					return( -1 );
				}
			}
			if( libcdata_tree_node_set_nodes(
			     sub_node,
			     NULL,
			     NULL,
			     NULL,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set nodes of sub node: %d.",
				 function,
				 sub_node_index );

				return( -1 );
			}
			if( libcdata_tree_node_free(
			     &sub_node,
			     value_free_function,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: unable to free sub node: %d.",
				 function,
				 sub_node_index );

				result = -1;
			}
			sub_node = next_node;
		}
		if( internal_node->value != NULL )
		{
			if( value_free_function != NULL )
			{
				if( value_free_function(
				     &( internal_node->value ),
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
					 "%s: unable to free value.",
					function );

					result = -1;
				}
				internal_node->value = NULL;
			}
		}
		memory_free(
		 internal_node );

		*node = NULL;
	}
	return( result );
}

/* Empties a tree node and frees its sub nodes
 * Uses the value_free_function to free the value of the sub nodes
 * Returns 1 if successful or -1 on error
 */
int libcdata_tree_node_empty(
     libcdata_tree_node_t *node,
     int (*value_free_function)(
            intptr_t **value,
            libcerror_error_t **error ),
     libcerror_error_t **error )
{
	libcdata_internal_tree_node_t *internal_node = NULL;
	libcdata_tree_node_t *next_node              = NULL;
	libcdata_tree_node_t *parent_node            = NULL;
	libcdata_tree_node_t *previous_node          = NULL;
	libcdata_tree_node_t *sub_node               = NULL;
	static char *function                        = "libcdata_tree_node_empty";
	int number_of_sub_nodes                      = 0;
	int result                                   = 1;
	int sub_node_index                           = 0;

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
	internal_node = (libcdata_internal_tree_node_t *) node;

	number_of_sub_nodes = internal_node->number_of_sub_nodes;

	sub_node = internal_node->first_sub_node;

	for( sub_node_index = 0;
	     sub_node_index < number_of_sub_nodes;
	     sub_node_index++ )
	{
		if( libcdata_tree_node_get_nodes(
		     sub_node,
		     &parent_node,
		     &previous_node,
		     &next_node,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve nodes of sub node: %d.",
			 function,
			 sub_node_index );

			return( -1 );
		}
		if( previous_node != NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
			 "%s: invalid sub node: %d - previous node is set.",
			 function,
			 sub_node_index );

			return( -1 );
		}
		internal_node->first_sub_node = next_node;

		if( internal_node->last_sub_node == sub_node )
		{
			internal_node->last_sub_node = next_node;
		}
		internal_node->number_of_sub_nodes--;

		if( next_node != NULL )
		{
			if( libcdata_tree_node_set_previous_node(
			     next_node,
			     NULL,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set previous node of sub node: %d.",
				 function,
				 sub_node_index + 1 );

				return( -1 );
			}
		}
		if( libcdata_tree_node_set_nodes(
		     sub_node,
		     NULL,
		     NULL,
		     NULL,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set nodes of sub node: %d.",
			 function,
			 sub_node_index );

			return( -1 );
		}
		if( libcdata_tree_node_free(
		     &sub_node,
		     value_free_function,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free sub node: %d.",
			 function,
			 sub_node_index );

			result = -1;
		}
		sub_node = next_node;
	}
	return( result );
}

/* Clones the tree node and its sub nodes
 *
 * The values are cloned using the value_clone_function
 * On error the values are freed using the value_free_function
 *
 * Returns 1 if successful or -1 on error
 */
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
     libcerror_error_t **error )
{
	libcdata_internal_tree_node_t *internal_destination_node = NULL;
	libcdata_internal_tree_node_t *internal_source_node      = NULL;
	libcdata_tree_node_t *destination_sub_node               = NULL;
	libcdata_tree_node_t *source_sub_node                    = NULL;
	static char *function                                    = "libcdata_tree_node_clone";
	int sub_node_index                                       = 0;

	if( destination_node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid destination tree node.",
		 function );

		return( -1 );
	}
	if( *destination_node != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid destination tree node already set.",
		 function );

		return( -1 );
	}
	if( value_free_function == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value free function.",
		 function );

		return( -1 );
	}
	if( value_clone_function == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value clone function.",
		 function );

		return( -1 );
	}
	if( source_node == NULL )
	{
		*destination_node = NULL;

		return( 1 );
	}
	internal_source_node = (libcdata_internal_tree_node_t *) source_node;

	if( libcdata_tree_node_initialize(
	     destination_node,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create destination tree node.",
		 function );

		return( -1 );
	}
	if( *destination_node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing destination tree node.",
		 function );

		return( -1 );
	}
	internal_destination_node = (libcdata_internal_tree_node_t *) *destination_node;

	if( value_clone_function(
	     &( internal_destination_node->value ),
	     internal_source_node->value,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create destination tree node value.",
		 function );

		goto on_error;
	}
	/* Clone the sub nodes
	 */
	source_sub_node = internal_source_node->first_sub_node;

	for( sub_node_index = 0;
	     sub_node_index < internal_source_node->number_of_sub_nodes;
	     sub_node_index++ )
	{
		if( source_sub_node == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: corruption detected in source sub node: %d.",
			 function,
			 sub_node_index );

			goto on_error;
		}
		if( libcdata_tree_node_clone(
		     &destination_sub_node,
		     source_sub_node,
		     value_free_function,
		     value_clone_function,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
			 "%s: unable to create destination sub node: %d.",
			 function,
			 sub_node_index );

			goto on_error;
		}
		if( libcdata_tree_node_append_node(
		     *destination_node,
		     destination_sub_node,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
			 "%s: unable to append sub node: %d to destination tree node.",
			 function,
			 sub_node_index );

			goto on_error;
		}
		destination_sub_node = NULL;

		if( libcdata_tree_node_get_next_node(
		     source_sub_node,
		     &source_sub_node,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve next node of source sub node: %d.",
			 function,
			 sub_node_index );

			goto on_error;
		}
	}
	return( 1 );

on_error:
	if( destination_sub_node != NULL )
	{
		libcdata_tree_node_free(
		 &destination_sub_node,
		 value_free_function,
		 NULL );
	}
	if( destination_node != NULL )
	{
		libcdata_tree_node_free(
		 destination_node,
		 value_free_function,
		 NULL );
	}
	return( -1 );
}

/* Retrieves the value from the tree node
 * Returns 1 if successful or -1 on error
 */
int libcdata_tree_node_get_value(
     libcdata_tree_node_t *node,
     intptr_t **value,
     libcerror_error_t **error )
{
	libcdata_internal_tree_node_t *internal_node = NULL;
	static char *function                        = "libcdata_tree_node_get_value";

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
	internal_node = (libcdata_internal_tree_node_t *) node;

	if( value == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	*value = internal_node->value;

	return( 1 );
}

/* Sets the value in the tree node
 * Returns 1 if successful or -1 on error
 */
int libcdata_tree_node_set_value(
     libcdata_tree_node_t *node,
     intptr_t *value,
     libcerror_error_t **error )
{
	libcdata_internal_tree_node_t *internal_node = NULL;
	static char *function                        = "libcdata_tree_node_set_value";

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
	internal_node = (libcdata_internal_tree_node_t *) node;

	internal_node->value = value;

	return( 1 );
}

/* Retrieves the parent node from the tree node
 * Returns 1 if successful or -1 on error
 */
int libcdata_tree_node_get_parent_node(
     libcdata_tree_node_t *node,
     libcdata_tree_node_t **parent_node,
     libcerror_error_t **error )
{
	libcdata_internal_tree_node_t *internal_node = NULL;
	static char *function                        = "libcdata_tree_node_get_parent_node";

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
	internal_node = (libcdata_internal_tree_node_t *) node;

	if( parent_node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid parent node.",
		 function );

		return( -1 );
	}
	*parent_node = internal_node->parent_node;

	return( 1 );
}

/* Sets the parent node in the tree node
 * Returns 1 if successful or -1 on error
 */
int libcdata_tree_node_set_parent_node(
     libcdata_tree_node_t *node,
     libcdata_tree_node_t *parent_node,
     libcerror_error_t **error )
{
	libcdata_internal_tree_node_t *internal_node = NULL;
	static char *function                        = "libcdata_tree_node_set_parent_node";

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
	internal_node = (libcdata_internal_tree_node_t *) node;

	internal_node->parent_node = parent_node;

	return( 1 );
}

/* Retrieves the previous node from the tree node
 * Returns 1 if successful or -1 on error
 */
int libcdata_tree_node_get_previous_node(
     libcdata_tree_node_t *node,
     libcdata_tree_node_t **previous_node,
     libcerror_error_t **error )
{
	libcdata_internal_tree_node_t *internal_node = NULL;
	static char *function                        = "libcdata_tree_node_get_previous_node";

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
	internal_node = (libcdata_internal_tree_node_t *) node;

	if( previous_node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid previous node.",
		 function );

		return( -1 );
	}
	*previous_node = internal_node->previous_node;

	return( 1 );
}

/* Sets the previous node in the tree node
 * Returns 1 if successful or -1 on error
 */
int libcdata_tree_node_set_previous_node(
     libcdata_tree_node_t *node,
     libcdata_tree_node_t *previous_node,
     libcerror_error_t **error )
{
	libcdata_internal_tree_node_t *internal_node = NULL;
	static char *function                        = "libcdata_tree_node_set_previous_node";

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
	internal_node = (libcdata_internal_tree_node_t *) node;

	internal_node->previous_node = previous_node;

	return( 1 );
}

/* Retrieves the next node from the tree node
 * Returns 1 if successful or -1 on error
 */
int libcdata_tree_node_get_next_node(
     libcdata_tree_node_t *node,
     libcdata_tree_node_t **next_node,
     libcerror_error_t **error )
{
	libcdata_internal_tree_node_t *internal_node = NULL;
	static char *function                        = "libcdata_tree_node_get_next_node";

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
	internal_node = (libcdata_internal_tree_node_t *) node;

	if( next_node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid next node.",
		 function );

		return( -1 );
	}
	*next_node = internal_node->next_node;

	return( 1 );
}

/* Sets the next node in the tree node
 * Returns 1 if successful or -1 on error
 */
int libcdata_tree_node_set_next_node(
     libcdata_tree_node_t *node,
     libcdata_tree_node_t *next_node,
     libcerror_error_t **error )
{
	libcdata_internal_tree_node_t *internal_node = NULL;
	static char *function                        = "libcdata_tree_node_set_next_node";

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
	internal_node = (libcdata_internal_tree_node_t *) node;

	internal_node->next_node = next_node;

	return( 1 );
}

/* Retrieves the nodes from the tree node
 * Returns 1 if successful or -1 on error
 */
int libcdata_tree_node_get_nodes(
     libcdata_tree_node_t *node,
     libcdata_tree_node_t **parent_node,
     libcdata_tree_node_t **previous_node,
     libcdata_tree_node_t **next_node,
     libcerror_error_t **error )
{
	libcdata_internal_tree_node_t *internal_node = NULL;
	static char *function                        = "libcdata_tree_node_get_nodes";

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
	internal_node = (libcdata_internal_tree_node_t *) node;

	if( parent_node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid parent node.",
		 function );

		return( -1 );
	}
	if( previous_node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid previous node.",
		 function );

		return( -1 );
	}
	if( next_node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid next node.",
		 function );

		return( -1 );
	}
	*parent_node   = internal_node->parent_node;
	*previous_node = internal_node->previous_node;
	*next_node     = internal_node->next_node;

	return( 1 );
}

/* Sets the nodes in the tree node
 * Returns 1 if successful or -1 on error
 */
int libcdata_tree_node_set_nodes(
     libcdata_tree_node_t *node,
     libcdata_tree_node_t *parent_node,
     libcdata_tree_node_t *previous_node,
     libcdata_tree_node_t *next_node,
     libcerror_error_t **error )
{
	libcdata_internal_tree_node_t *internal_node = NULL;
	static char *function                        = "libcdata_tree_node_set_nodes";

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
	internal_node = (libcdata_internal_tree_node_t *) node;

	internal_node->parent_node   = parent_node;
	internal_node->previous_node = previous_node;
	internal_node->next_node     = next_node;

	return( 1 );
}

/* Appends a tree node to the parent node
 * Returns 1 if successful or -1 on error
 */
int libcdata_tree_node_append_node(
     libcdata_tree_node_t *parent_node,
     libcdata_tree_node_t *node,
     libcerror_error_t **error )
{
	libcdata_internal_tree_node_t *internal_node        = NULL;
	libcdata_internal_tree_node_t *internal_parent_node = NULL;
	static char *function                               = "libcdata_tree_node_append_node";

	if( parent_node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid parent node.",
		 function );

		return( -1 );
	}
	internal_parent_node = (libcdata_internal_tree_node_t *) parent_node;

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
	internal_node = (libcdata_internal_tree_node_t *) node;

	if( ( internal_node->parent_node != NULL )
	 || ( internal_node->previous_node != NULL )
	 || ( internal_node->next_node != NULL ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid node - node is already part of a tree.",
		 function );

		return( -1 );
	}
	internal_node->parent_node = parent_node;

	if( internal_parent_node->number_of_sub_nodes == 0 )
	{
		if( internal_parent_node->first_sub_node != NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
			 "%s: corruption detected - first sub node already set.",
			 function );

			return( -1 );
		}
		if( internal_parent_node->last_sub_node != NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
			 "%s: corruption detected - last sub node already set.",
			 function );

			return( -1 );
		}
		internal_parent_node->first_sub_node = node;
		internal_parent_node->last_sub_node  = node;
	}
	else
	{
		if( internal_parent_node->first_sub_node == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: corruption detected - missing first sub node.",
			 function );

			return( -1 );
		}
		if( internal_parent_node->last_sub_node == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: corruption detected - missing last sub node.",
			 function );

			return( -1 );
		}
		if( libcdata_tree_node_set_next_node(
		     internal_parent_node->last_sub_node,
		     node,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set next node of last sub node.",
			 function );

			return( -1 );
		}
		internal_node->previous_node        = internal_parent_node->last_sub_node;
		internal_parent_node->last_sub_node = node;
	}
	internal_parent_node->number_of_sub_nodes++;

	return( 1 );
}

/* Appends a value to the parent node
 * Creates a new tree node
 * Returns 1 if successful or -1 on error
 */
int libcdata_tree_node_append_value(
     libcdata_tree_node_t *parent_node,
     intptr_t *value,
     libcerror_error_t **error )
{
	libcdata_tree_node_t *tree_node = NULL;
	static char *function           = "libcdata_tree_node_append_value";

	if( libcdata_tree_node_initialize(
	     &tree_node,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create tree node.",
		 function );

		goto on_error;
	}
	if( libcdata_tree_node_append_node(
	     parent_node,
	     tree_node,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
		 "%s: unable to append tree node.",
		 function );

		goto on_error;
	}
	if( libcdata_tree_node_set_value(
	     tree_node,
	     value,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to set value in tree node.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( tree_node != NULL )
	{
		libcdata_tree_node_free(
		 &tree_node,
		 NULL,
		 NULL );
	}
	return( -1 );
}

/* Inserts a tree node in the parent node
 *
 * Uses the entry_compare_function to determine the order of the entries
 * The entry_compare_function should return LIBCDATA_COMPARE_LESS,
 * LIBCDATA_COMPARE_EQUAL, LIBCDATA_COMPARE_GREATER if successful or -1 on error
 *
 * Duplicate entries are allowed by default and inserted after the last duplicate entry.
 * Only allowing unique entries can be enforced by setting the flag LIBCDATA_INSERT_FLAG_UNIQUE_ENTRIES
 *
 * Returns 1 if successful, 0 if the node already exists or -1 on error
 */
int libcdata_tree_node_insert_node(
     libcdata_tree_node_t *parent_node,
     libcdata_tree_node_t *node,
     int (*value_compare_function)(
            intptr_t *first_value,
            intptr_t *second_value,
            libcerror_error_t **error ),
     uint8_t insert_flags,
     libcerror_error_t **error )
{
	libcdata_internal_tree_node_t *internal_node        = NULL;
	libcdata_internal_tree_node_t *internal_parent_node = NULL;
	libcdata_tree_node_t *previous_node                 = NULL;
	libcdata_tree_node_t *sub_node                      = NULL;
	intptr_t *sub_node_value                            = NULL;
	static char *function                               = "libcdata_tree_node_insert_node";
	int result                                          = -1;
	int sub_node_index                                  = 0;

	if( parent_node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid parent node.",
		 function );

		return( -1 );
	}
	internal_parent_node = (libcdata_internal_tree_node_t *) parent_node;

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
	internal_node = (libcdata_internal_tree_node_t *) node;

	if( ( internal_node->parent_node != NULL )
	 || ( internal_node->previous_node != NULL )
	 || ( internal_node->next_node != NULL ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid node - node is already part of a tree.",
		 function );

		return( -1 );
	}
	if( value_compare_function == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value compare function.",
		 function );

		return( -1 );
	}
	if( ( insert_flags & ~( LIBCDATA_INSERT_FLAG_UNIQUE_ENTRIES ) ) != 0 )
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
	if( internal_parent_node->number_of_sub_nodes == 0 )
	{
		if( internal_parent_node->first_sub_node != NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
			 "%s: corruption detected - first sub node already set.",
			 function );

			return( -1 );
		}
		if( internal_parent_node->last_sub_node != NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
			 "%s: corruption detected - last sub node already set.",
			 function );

			return( -1 );
		}
		internal_parent_node->first_sub_node = node;
		internal_parent_node->last_sub_node  = node;
	}
	else
	{
		if( internal_parent_node->first_sub_node == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: corruption detected - missing first sub node.",
			 function );

			return( -1 );
		}
		if( internal_parent_node->last_sub_node == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: corruption detected - missing last sub node.",
			 function );

			return( -1 );
		}
		sub_node = internal_parent_node->first_sub_node;

		for( sub_node_index = 0;
		     sub_node_index < internal_parent_node->number_of_sub_nodes;
		     sub_node_index++ )
		{
			if( libcdata_tree_node_get_value(
			     sub_node,
			     &sub_node_value,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve value of sub node: %d.",
				 function,
				 sub_node_index );

				return( -1 );
			}
			result = value_compare_function(
			          internal_node->value,
			          sub_node_value,
			          error );

			if( result == -1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to compare sub node: %d.",
				 function,
				 sub_node_index );

				return( -1 );
			}
			else if( result == LIBCDATA_COMPARE_EQUAL )
			{
				if( ( insert_flags & LIBCDATA_INSERT_FLAG_UNIQUE_ENTRIES ) != 0 )
				{
					return( 0 );
				}
			}
			else if( result == LIBCDATA_COMPARE_LESS )
			{
				break;
			}
			else if( result != LIBCDATA_COMPARE_GREATER )
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
			if( libcdata_tree_node_get_next_node(
			     sub_node,
			     &sub_node,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve next node of sub node: %d.",
				 function,
				 sub_node_index );

				return( -1 );
			}
		}
		if( result == LIBCDATA_COMPARE_LESS )
		{
			if( libcdata_tree_node_get_previous_node(
			     sub_node,
			     &previous_node,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve previous node of sub node: %d.",
				 function,
				 sub_node_index );

				return( -1 );
			}
			internal_node->previous_node = previous_node;
			internal_node->next_node     = sub_node;

			if( sub_node == internal_parent_node->first_sub_node )
			{
				internal_parent_node->first_sub_node = node;
			}
			else
			{
				if( previous_node == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: corruption detected - missing previous node in sub node: %d.",
					 function,
					 sub_node_index );

					return( -1 );
				}
				if( libcdata_tree_node_set_next_node(
				     previous_node,
				     node,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to set next node of sub node: %d.",
					 function,
					 sub_node_index - 1 );

					return( -1 );
				}
			}
			if( libcdata_tree_node_set_previous_node(
			     sub_node,
			     node,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set previous node of sub node: %d.",
				 function,
				 sub_node_index );

				return( -1 );
			}
		}
		else
		{
			if( libcdata_tree_node_set_next_node(
			     internal_parent_node->last_sub_node,
			     node,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set next node of last sub node.",
				 function );

				return( -1 );
			}
			internal_node->previous_node        = internal_parent_node->last_sub_node;
			internal_parent_node->last_sub_node = node;
		}
	}
	internal_node->parent_node = parent_node;

	internal_parent_node->number_of_sub_nodes++;

	return( 1 );
}

/* Inserts a value in the parent node
 *
 * Creates a new tree node
 *
 * Uses the entry_compare_function to determine the order of the entries
 * The entry_compare_function should return LIBCDATA_COMPARE_LESS,
 * LIBCDATA_COMPARE_EQUAL, LIBCDATA_COMPARE_GREATER if successful or -1 on error
 *
 * Duplicate entries are allowed by default and inserted after the last duplicate entry.
 * Only allowing unique entries can be enforced by setting the flag LIBCDATA_INSERT_FLAG_UNIQUE_ENTRIES
 *
 * Returns 1 if successful, 0 if the node already exists or -1 on error
 */
int libcdata_tree_node_insert_value(
     libcdata_tree_node_t *parent_node,
     intptr_t *value,
     int (*value_compare_function)(
            intptr_t *first_value,
            intptr_t *second_value,
            libcerror_error_t **error ),
     uint8_t insert_flags,
     libcerror_error_t **error )
{
	libcdata_tree_node_t *tree_node = NULL;
	static char *function           = "libcdata_tree_node_insert_value";
	int result                      = 0;

	if( libcdata_tree_node_initialize(
	     &tree_node,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create tree node.",
		 function );

		goto on_error;
	}
	if( libcdata_tree_node_set_value(
	     tree_node,
	     value,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to set value in tree node.",
		 function );

		goto on_error;
	}
	result = libcdata_tree_node_insert_node(
	          parent_node,
	          tree_node,
	          value_compare_function,
	          insert_flags,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
		 "%s: unable to insert node.",
		 function );

		goto on_error;
	}
	else if( result == 0 )
	{
		if( libcdata_tree_node_free(
		     &tree_node,
		     NULL,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free tree node.",
			 function );

			goto on_error;
		}
	}
	return( result );

on_error:
	if( tree_node != NULL )
	{
		libcdata_tree_node_free(
		 &tree_node,
		 NULL,
		 NULL );
	}
	return( -1 );
}

/* Replaces a tree node with another tree node
 * Returns 1 if successful or -1 on error
 */
int libcdata_tree_node_replace_node(
     libcdata_tree_node_t *node,
     libcdata_tree_node_t *replacement_node,
     libcerror_error_t **error )
{
	libcdata_internal_tree_node_t *internal_node             = NULL;
	libcdata_internal_tree_node_t *internal_replacement_node = NULL;
	libcdata_tree_node_t *first_sub_node                     = NULL;
	libcdata_tree_node_t *last_sub_node                      = NULL;
	libcdata_tree_node_t *sub_node                           = NULL;
	intptr_t *value                                          = NULL;
	static char *function                                    = "libcdata_tree_node_replace_node";
	int number_of_sub_nodes                                  = 0;
	int sub_node_index                                       = 0;

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
	internal_node = (libcdata_internal_tree_node_t *) node;

	if( replacement_node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid replacement node.",
		 function );

		return( -1 );
	}
	internal_replacement_node = (libcdata_internal_tree_node_t *) replacement_node;

	if( replacement_node == node )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: cannot replace node with itself.",
		 function );

		return( -1 );
	}
	if( ( internal_replacement_node->parent_node != NULL )
	 || ( internal_replacement_node->previous_node != NULL )
	 || ( internal_replacement_node->next_node != NULL ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid replacement node - node is already part of a tree.",
		 function );

		return( -1 );
	}
	value               = internal_node->value;
	first_sub_node      = internal_node->first_sub_node;
	last_sub_node       = internal_node->last_sub_node;
	number_of_sub_nodes = internal_node->number_of_sub_nodes;

	internal_node->value               = internal_replacement_node->value;
	internal_node->first_sub_node      = internal_replacement_node->first_sub_node;
	internal_node->last_sub_node       = internal_replacement_node->last_sub_node;
	internal_node->number_of_sub_nodes = internal_replacement_node->number_of_sub_nodes;

	internal_replacement_node->value               = value;
	internal_replacement_node->first_sub_node      = first_sub_node;
	internal_replacement_node->last_sub_node       = last_sub_node;
	internal_replacement_node->number_of_sub_nodes = number_of_sub_nodes;

	sub_node = internal_node->first_sub_node;

	for( sub_node_index = 0;
	     sub_node_index < internal_node->number_of_sub_nodes;
	     sub_node_index++ )
	{
		if( libcdata_tree_node_set_parent_node(
		     sub_node,
		     node,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set parent node value of sub node: %d.",
			 function,
			 sub_node_index );

			return( -1 );
		}
		if( libcdata_tree_node_get_next_node(
		     sub_node,
		     &sub_node,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve next node of sub node: %d.",
			 function,
			 sub_node_index );

			return( -1 );
		}
	}
	return( 1 );
}

/* Removes a tree node from the parent node
 * Returns 1 if successful or -1 on error
 */
int libcdata_tree_node_remove_node(
     libcdata_tree_node_t *parent_node,
     libcdata_tree_node_t *node,
     libcerror_error_t **error )
{
	libcdata_internal_tree_node_t *internal_node        = NULL;
	libcdata_internal_tree_node_t *internal_parent_node = NULL;
	static char *function                               = "libcdata_tree_node_remove_node";

	if( parent_node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid parent node.",
		 function );

		return( -1 );
	}
	internal_parent_node = (libcdata_internal_tree_node_t *) parent_node;

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
	internal_node = (libcdata_internal_tree_node_t *) node;

	if( parent_node != internal_node->parent_node )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid node - parent node mismatch.",
		 function );

		return( -1 );
	}
	if( internal_parent_node->number_of_sub_nodes == 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid parent node - missing number of sub nodes.",
		 function );

		return( -1 );
	}
	if( internal_parent_node->first_sub_node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid parent node - missing first sub node.",
		 function );

		return( -1 );
	}
	if( internal_parent_node->last_sub_node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid parent node - missing last sub node.",
		 function );

		return( -1 );
	}
	if( internal_parent_node->first_sub_node == node )
	{
		internal_parent_node->first_sub_node = internal_node->next_node;
	}
	if( internal_parent_node->last_sub_node == node )
	{
		internal_parent_node->last_sub_node = internal_node->previous_node;
	}
	if( internal_node->next_node != NULL )
	{
		if( libcdata_tree_node_set_previous_node(
		     internal_node->next_node,
		     internal_node->previous_node,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set previous node of next node.",
			 function );

			return( -1 );
		}
	}
	if( internal_node->previous_node != NULL )
	{
		if( libcdata_tree_node_set_next_node(
		     internal_node->previous_node,
		     internal_node->next_node,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set next node of previous node.",
			 function );

			return( -1 );
		}
	}
	internal_node->parent_node   = NULL;
	internal_node->previous_node = NULL;
	internal_node->next_node     = NULL;

	internal_parent_node->number_of_sub_nodes--;

	return( 1 );
}

/* Retrieves the number of sub nodes in the tree node
 * Returns 1 if successful or -1 on error
 */
int libcdata_tree_node_get_number_of_sub_nodes(
     libcdata_tree_node_t *node,
     int *number_of_sub_nodes,
     libcerror_error_t **error )
{
	libcdata_internal_tree_node_t *internal_node = NULL;
	static char *function                        = "libcdata_tree_node_get_number_of_sub_nodes";

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
	internal_node = (libcdata_internal_tree_node_t *) node;

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
	*number_of_sub_nodes = internal_node->number_of_sub_nodes;

	return( 1 );
}

/* Retrieves a specific sub node from the tree node
 * Returns 1 if successful or -1 on error
 */
int libcdata_tree_node_get_sub_node_by_index(
     libcdata_tree_node_t *node,
     int sub_node_index,
     libcdata_tree_node_t **sub_node,
     libcerror_error_t **error )
{
	libcdata_internal_tree_node_t *internal_node = NULL;
	static char *function                        = "libcdata_tree_node_get_sub_node_by_index";
	int result                                   = -1;
	int sub_node_iterator                        = 0;

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
	internal_node = (libcdata_internal_tree_node_t *) node;

	if( ( sub_node_index < 0 )
	 || ( sub_node_index >= internal_node->number_of_sub_nodes ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid sub node index value out of bounds.",
		 function );

		return( -1 );
	}
	if( sub_node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid sub node.",
		 function );

		return( -1 );
	}
	/* Check if the sub nodes should be searched front to back
	 * or back to front
	 */
	if( sub_node_index < ( internal_node->number_of_sub_nodes / 2 ) )
	{
		*sub_node = internal_node->first_sub_node;

		for( sub_node_iterator = 0;
		     sub_node_iterator < internal_node->number_of_sub_nodes;
		     sub_node_iterator++ )
		{
			if( sub_node_iterator == sub_node_index )
			{
				result = 1;

				break;
			}	
			if( libcdata_tree_node_get_next_node(
			     *sub_node,
			     sub_node,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve next node of sub node: %d.",
				 function,
				 sub_node_iterator );

				return( -1 );
			}
		}
	}
	else
	{
		*sub_node = internal_node->last_sub_node;

		for( sub_node_iterator = ( internal_node->number_of_sub_nodes - 1 );
		     sub_node_iterator >= 0;
		     sub_node_iterator-- )
		{
			if( sub_node_iterator == sub_node_index )
			{
				result = 1;

				break;
			}	
			if( libcdata_tree_node_get_previous_node(
			     *sub_node,
			     sub_node,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve previous node of sub node: %d.",
				 function,
				 sub_node_iterator );

				return( -1 );
			}
		}
	}
	return( result );
}

/* Retrieves a list of all the leaf nodes
 * Returns 1 if successful or -1 on error
 */
int libcdata_tree_node_get_leaf_node_list(
     libcdata_tree_node_t *node,
     libcdata_list_t **leaf_node_list,
     libcerror_error_t **error )
{
	libcdata_internal_tree_node_t *internal_node = NULL;
	libcdata_tree_node_t *sub_node               = NULL;
	static char *function                        = "libcdata_tree_node_get_leaf_node_list";
	int sub_node_index                           = 0;

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
	internal_node = (libcdata_internal_tree_node_t *) node;

	if( leaf_node_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid leaf node list.",
		 function );

		return( -1 );
	}
	if( *leaf_node_list == NULL )
	{
		if( libcdata_list_initialize(
		     leaf_node_list,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create leaf node list.",
			 function );

			return( -1 );
		}
	}
	/* Traverse the sub nodes
	 */
	if( internal_node->number_of_sub_nodes > 0 )
	{
		sub_node = internal_node->first_sub_node;

		for( sub_node_index = 0;
		     sub_node_index < internal_node->number_of_sub_nodes;
		     sub_node_index++ )
		{
			if( sub_node == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
				 "%s: corruption detected for sub node: %d.",
				 function,
				 sub_node_index );

				return( -1 );
			}
			if( libcdata_tree_node_get_leaf_node_list(
			     sub_node,
			     leaf_node_list,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
				 "%s: unable to traverse sub node: %d.",
				 function,
				 sub_node_index );

				return( -1 );
			}
			if( libcdata_tree_node_get_next_node(
			     sub_node,
			     &sub_node,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve next node of sub node: %d.",
				 function,
				 sub_node_index );

				return( -1 );
			}
		}
	}
	else if( internal_node->value == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid node - missing value.",
		 function );

		return( -1 );
	}
	else if( libcdata_list_append_value(
	          *leaf_node_list,
	          internal_node->value,
	          error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
		 "%s: unable to append tree node to leaf node list.",
		 function );

		return( -1 );
	}
	return( 1 );
}

