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

#include <common.h>
#include <memory.h>
#include <types.h>

#include "libfdata_btree.h"
#include "libfdata_btree_node.h"
#include "libfdata_btree_range.h"
#include "libfdata_definitions.h"
#include "libfdata_libcerror.h"
#include "libfdata_libcnotify.h"
#include "libfdata_libfcache.h"
#include "libfdata_types.h"
#include "libfdata_unused.h"

/* Creates a tree
 * Make sure the value tree is referencing, is set to NULL
 *
 * If the flag LIBFDATA_DATA_HANDLE_FLAG_MANAGED is set the tree
 * takes over management of the data handle and the data handle is freed when
 * no longer needed
 *
 * Returns 1 if successful or -1 on error
 */
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
     libcerror_error_t **error )
{
	libfdata_internal_btree_t *internal_tree = NULL;
	static char *function                    = "libfdata_btree_initialize";

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
	if( *tree != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid tree value already set.",
		 function );

		return( -1 );
	}
	internal_tree = memory_allocate_structure(
	                 libfdata_internal_btree_t );

	if( internal_tree == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create tree.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_tree,
	     0,
	     sizeof( libfdata_internal_btree_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear tree.",
		 function );

		goto on_error;
	}
	if( libfdata_btree_range_initialize(
	     &( internal_tree->root_node_data_range ),
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create root node data range.",
		 function );

		goto on_error;
	}
	if( libfcache_date_time_get_timestamp(
	     &( internal_tree->timestamp ),
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
	internal_tree->flags                                 |= flags;
	internal_tree->calculate_node_cache_entry_index       = &libfdata_btree_calculate_node_cache_entry_index;
	internal_tree->calculate_leaf_value_cache_entry_index = &libfdata_btree_calculate_leaf_value_cache_entry_index;
	internal_tree->data_handle                            = data_handle;
	internal_tree->free_data_handle                       = free_data_handle;
	internal_tree->clone_data_handle                      = clone_data_handle;
	internal_tree->read_node                              = read_node;
	internal_tree->read_leaf_value                        = read_leaf_value;

	*tree = (libfdata_btree_t *) internal_tree;

	return( 1 );

on_error:
	if( internal_tree != NULL )
	{
		if( internal_tree->root_node_data_range != NULL )
		{
			libfdata_btree_range_free(
			 &( internal_tree->root_node_data_range ),
			 NULL );
		}
		memory_free(
		 internal_tree );
	}
	return( -1 );
}

/* Frees a tree
 * Returns 1 if successful or -1 on error
 */
int libfdata_btree_free(
     libfdata_btree_t **tree,
     libcerror_error_t **error )
{
	libfdata_internal_btree_t *internal_tree = NULL;
	static char *function                    = "libfdata_btree_free";
	int result                               = 1;

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
	if( *tree != NULL )
	{
		internal_tree = (libfdata_internal_btree_t *) *tree;
		*tree         = NULL;

		if( libfdata_btree_range_free(
		     &( internal_tree->root_node_data_range ),
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free root node data range.",
			 function );

			result = -1;
		}
		if( ( internal_tree->flags & LIBFDATA_DATA_HANDLE_FLAG_MANAGED ) != 0 )
		{
			if( internal_tree->data_handle != NULL )
			{
				if( internal_tree->free_data_handle == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: invalid list - missing free data handle function.",
					 function );

					result = -1;
				}
				else if( internal_tree->free_data_handle(
				          &( internal_tree->data_handle ),
				          error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
					 "%s: unable to free data handle.",
					 function );

					result = -1;
				}
			}
		}
		memory_free(
		 internal_tree );
	}
	return( result );
}

/* Clones (duplicates) the tree
 * Returns 1 if successful or -1 on error
 */
int libfdata_btree_clone(
     libfdata_btree_t **destination_tree,
     libfdata_btree_t *source_tree,
     libcerror_error_t **error )
{
	libfdata_internal_btree_t *internal_destination_tree = NULL;
	libfdata_internal_btree_t *internal_source_tree      = NULL;
	intptr_t *destination_data_handle                    = NULL;
	static char *function                                = "libfdata_btree_clone";

	if( destination_tree == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid destination tree.",
		 function );

		return( -1 );
	}
	if( *destination_tree != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid destination tree value already set.",
		 function );

		return( -1 );
	}
	if( source_tree == NULL )
	{
		*destination_tree = NULL;

		return( 1 );
	}
	internal_source_tree = (libfdata_internal_btree_t *) source_tree;

	if( internal_source_tree->data_handle != NULL )
	{
		if( internal_source_tree->free_data_handle == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: invalid source tree - missing free data handle function.",
			 function );

			goto on_error;
		}
		if( internal_source_tree->clone_data_handle == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: invalid source tree - missing clone data handle function.",
			 function );

			goto on_error;
		}
		if( internal_source_tree->clone_data_handle(
		     &destination_data_handle,
		     internal_source_tree->data_handle,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to clone data handle.",
			 function );

			goto on_error;
		}
	}
	internal_destination_tree = memory_allocate_structure(
	                             libfdata_internal_btree_t );

	if( internal_destination_tree == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create destination tree.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_destination_tree,
	     0,
	     sizeof( libfdata_internal_btree_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear destination tree.",
		 function );

		memory_free(
		 internal_destination_tree );

		return( -1 );
	}
	if( internal_source_tree->data_handle != NULL )
	{
		if( internal_source_tree->free_data_handle == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: invalid source tree - missing free data handle function.",
			 function );

			goto on_error;
		}
		if( internal_source_tree->clone_data_handle == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: invalid source tree - missing clone data handle function.",
			 function );

			goto on_error;
		}
		if( internal_source_tree->clone_data_handle(
		     &( internal_destination_tree->data_handle ),
		     internal_source_tree->data_handle,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to clone data handle.",
			 function );

			goto on_error;
		}
	}
	if( libfdata_btree_range_clone(
	     &( internal_destination_tree->root_node_data_range ),
	     internal_source_tree->root_node_data_range,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create destination root node data range.",
		 function );

		goto on_error;
	}
	if( libfcache_date_time_get_timestamp(
	     &( internal_destination_tree->timestamp ),
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve destination timestamp.",
		 function );

		goto on_error;
	}
	*destination_tree = (libfdata_btree_t *) internal_destination_tree;

	return( 1 );

on_error:
	if( internal_destination_tree != NULL )
	{
		if( internal_destination_tree->root_node_data_range != NULL )
		{
			libfdata_btree_range_free(
			 &( internal_destination_tree->root_node_data_range ),
			 NULL );
		}
		if( ( internal_destination_tree->data_handle != NULL )
		 && ( internal_source_tree->free_data_handle != NULL ) )
		{
			internal_source_tree->free_data_handle(
			 &( internal_destination_tree->data_handle ),
			 NULL );
		}
		memory_free(
		 internal_destination_tree );
	}
	return( -1 );
}

/* Cache entry functions
 */

/* Calculated the node cache entry index
 * Returns 1 if successful or -1 on error
 */
int libfdata_btree_calculate_node_cache_entry_index(
     libfdata_btree_t *tree,
     libfcache_cache_t *cache,
     int level LIBFDATA_ATTRIBUTE_UNUSED,
     int node_data_file_index LIBFDATA_ATTRIBUTE_UNUSED,
     off64_t node_data_offset,
     size64_t node_data_size LIBFDATA_ATTRIBUTE_UNUSED,
     uint32_t node_data_flags LIBFDATA_ATTRIBUTE_UNUSED,
     int *cache_entry_index,
     libcerror_error_t **error )
{
	static char *function       = "libfdata_btree_calculate_node_cache_entry_index";
	int number_of_cache_entries = 0;

	LIBFDATA_UNREFERENCED_PARAMETER( level );
	LIBFDATA_UNREFERENCED_PARAMETER( node_data_file_index );
	LIBFDATA_UNREFERENCED_PARAMETER( node_data_size );
	LIBFDATA_UNREFERENCED_PARAMETER( node_data_flags );

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
	if( cache_entry_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid cache entry index.",
		 function );

		return( -1 );
	}
	if( libfcache_cache_get_number_of_entries(
	     cache,
	     &number_of_cache_entries,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of cache entries.",
		 function );

		return( -1 );
	}
	if( number_of_cache_entries <= 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid number of cache entries value out of bounds.",
		 function );

		return( -1 );
	}
/* TODO improve cache entry index calculation */
	if( number_of_cache_entries <= 4 )
	{
		*cache_entry_index = node_data_offset % number_of_cache_entries;
	}
	else if( level == 0 )
	{
		*cache_entry_index = 0;
	}
	else
	{
		*cache_entry_index = 1;

		/* Use 3/4 of the cache for the nodes
		 */
		number_of_cache_entries = ( ( number_of_cache_entries - 1 ) / 4 ) * 3;

		*cache_entry_index += node_data_offset % number_of_cache_entries;
	}
	return( 1 );
}

/* Calculated the leaf value cache entry index
 * Returns 1 if successful or -1 on error
 */
int libfdata_btree_calculate_leaf_value_cache_entry_index(
     libfdata_btree_t *tree,
     libfcache_cache_t *cache,
     int leaf_value_index LIBFDATA_ATTRIBUTE_UNUSED,
     int leaf_value_data_file_index LIBFDATA_ATTRIBUTE_UNUSED,
     off64_t leaf_value_data_offset,
     size64_t leaf_value_data_size LIBFDATA_ATTRIBUTE_UNUSED,
     uint32_t leaf_value_data_flags LIBFDATA_ATTRIBUTE_UNUSED,
     int *cache_entry_index,
     libcerror_error_t **error )
{
	static char *function       = "libfdata_btree_calculate_leaf_value_cache_entry_index";
	int number_of_cache_entries = 0;

	LIBFDATA_UNREFERENCED_PARAMETER( leaf_value_index );
	LIBFDATA_UNREFERENCED_PARAMETER( leaf_value_data_file_index );
	LIBFDATA_UNREFERENCED_PARAMETER( leaf_value_data_size );
	LIBFDATA_UNREFERENCED_PARAMETER( leaf_value_data_flags );

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
	if( cache_entry_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid cache entry index.",
		 function );

		return( -1 );
	}
	if( libfcache_cache_get_number_of_entries(
	     cache,
	     &number_of_cache_entries,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of cache entries.",
		 function );

		return( -1 );
	}
	if( number_of_cache_entries <= 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid number of cache entries value out of bounds.",
		 function );

		return( -1 );
	}
/* TODO improve cache entry index calculation */
	if( number_of_cache_entries <= 4 )
	{
		*cache_entry_index = leaf_value_data_offset % number_of_cache_entries;
	}
	else
	{
		/* Use 1/4 of the cache for the leaf values
		 */
		*cache_entry_index      = ( ( number_of_cache_entries - 1 ) / 4 ) * 3;
		number_of_cache_entries = ( number_of_cache_entries - 1 ) / 4;

		*cache_entry_index += leaf_value_data_offset % number_of_cache_entries;
	}
	return( 1 );
}

/* Root node functions
 */

/* Retrieves the root node
 * Returns 1 if successful or -1 on error
 */
int libfdata_btree_get_root_node(
     libfdata_btree_t *tree,
     int *node_data_file_index,
     off64_t *node_data_offset,
     size64_t *node_data_size,
     uint32_t *node_data_flags,
     libcerror_error_t **error )
{
	libfdata_internal_btree_t *internal_tree = NULL;
	static char *function                    = "libfdata_btree_get_root_node";
	intptr_t *key_value                      = NULL;

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
	internal_tree = (libfdata_internal_btree_t *) tree;

	if( libfdata_btree_range_get(
	     internal_tree->root_node_data_range,
	     node_data_file_index,
	     node_data_offset,
	     node_data_size,
	     node_data_flags,
	     &key_value,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve root node data range.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Sets the root node
 * Returns 1 if successful or -1 on error
 */
int libfdata_btree_set_root_node(
     libfdata_btree_t *tree,
     int node_data_file_index,
     off64_t node_data_offset,
     size64_t node_data_size,
     uint32_t node_data_flags,
     libcerror_error_t **error )
{
	libfdata_internal_btree_t *internal_tree = NULL;
	intptr_t *key_value                      = NULL;
	static char *function                    = "libfdata_btree_set_root_node";

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
	internal_tree = (libfdata_internal_btree_t *) tree;

	if( node_data_file_index < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_LESS_THAN_ZERO,
		 "%s: invalid node data file index less than zero.",
		 function );

		return( -1 );
	}
	if( node_data_offset < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_LESS_THAN_ZERO,
		 "%s: invalid node data offset value less than zero.",
		 function );

		return( -1 );
	}
	if( node_data_size > (size64_t) INT64_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid node data size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( libfdata_btree_range_set(
	     internal_tree->root_node_data_range,
	     node_data_file_index,
	     node_data_offset,
	     node_data_size,
	     node_data_flags,
	     key_value,
	     NULL,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set root node data range.",
		 function );

		return( -1 );
	}
	if( libfcache_date_time_get_timestamp(
	     &( internal_tree->timestamp ),
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

/* Leaf value functions
 */

/* Reads the leaf value identified by the data range
 * Returns 1 if successful or -1 on error
 */
int libfdata_btree_read_leaf_value(
     libfdata_internal_btree_t *internal_tree,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     libfdata_btree_range_t *leaf_value_data_range,
     intptr_t **leaf_value,
     uint8_t read_flags,
     libcerror_error_t **error )
{
	libfcache_cache_value_t *cache_value = NULL;
	intptr_t *key_value                  = NULL;
	static char *function                = "libfdata_btree_read_leaf_value";
	off64_t cache_value_offset           = 0;
	off64_t leaf_value_data_offset       = 0;
	size64_t leaf_value_data_size        = 0;
	time_t cache_value_timestamp         = 0;
	uint32_t leaf_value_data_flags       = 0;
	int cache_entry_index                = -1;
	int cache_value_file_index           = -1;
	int leaf_value_data_file_index       = -1;
	int leaf_value_index                 = -1;
	int result                           = 0;

	if( internal_tree == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid tree.",
		 function );

		return( -1 );
	}
	if( internal_tree->read_leaf_value == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid tree - missing read leaf value function.",
		 function );

		return( -1 );
	}
	if( leaf_value_data_range == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid tree leaf value data range.",
		 function );

		return( -1 );
	}
	leaf_value_index = leaf_value_data_range->mapped_first_leaf_value_index;

	if( libfdata_btree_range_get(
	     leaf_value_data_range,
	     &leaf_value_data_file_index,
	     &leaf_value_data_offset,
	     &leaf_value_data_size,
	     &leaf_value_data_flags,
	     &key_value,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve leaf value data range values.",
		 function );

		return( -1 );
	}
	if( ( read_flags & LIBFDATA_READ_FLAG_IGNORE_CACHE ) == 0 )
	{
		if( internal_tree->calculate_leaf_value_cache_entry_index(
		     (libfdata_btree_t *) internal_tree,
		     cache,
		     leaf_value_index,
		     leaf_value_data_file_index,
		     leaf_value_data_offset,
		     leaf_value_data_size,
		     leaf_value_data_flags,
		     &cache_entry_index,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to calculate cache entry index.",
			 function );

			return( -1 );
		}
		if( libfcache_cache_get_value_by_index(
		     cache,
		     cache_entry_index,
		     &cache_value,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve cache entry: %d from cache.",
			 function,
			 cache_entry_index );

			return( -1 );
		}
		if( cache_value != NULL )
		{
			if( libfcache_cache_value_get_identifier(
			     cache_value,
			     &cache_value_file_index,
			     &cache_value_offset,
			     &cache_value_timestamp,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve cache value identifier.",
				 function );

				return( -1 );
			}
		}
		if( ( leaf_value_data_file_index == cache_value_file_index )
		 && ( leaf_value_data_offset == cache_value_offset )
		 && ( internal_tree->timestamp == cache_value_timestamp ) )
		{
			result = 1;
		}
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			if( result == 0 )
			{
				libcnotify_printf(
				 "%s: cache: 0x%08" PRIjx " miss (entry: %d, want: %" PRIi64 ", got: %" PRIi64 ")\n",
				 function,
				 (intptr_t) cache,
				 cache_entry_index,
				 leaf_value_data_offset,
				 cache_value_offset );
			}
			else
			{
				libcnotify_printf(
				 "%s: cache: 0x%08" PRIjx " hit (entry: %d)\n",
				 function,
				 (intptr_t) cache,
				 cache_entry_index );
			}
		}
#endif
	}
	if( result == 0 )
	{
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: reading leaf value at offset: %" PRIi64 " (0x%08" PRIx64 ") of size: %" PRIu64 "\n",
			 function,
			 leaf_value_data_offset,
			 leaf_value_data_offset,
			 leaf_value_data_size );
		}
#endif
		if( internal_tree->read_leaf_value(
		     internal_tree->data_handle,
		     file_io_handle,
		     (libfdata_btree_t *) internal_tree,
		     cache,
		     leaf_value_index,
		     leaf_value_data_file_index,
		     leaf_value_data_offset,
		     leaf_value_data_size,
		     leaf_value_data_flags,
		     key_value,
		     read_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_READ_FAILED,
			 "%s: unable to read leaf value at offset: %" PRIi64 ".",
			 function,
			 leaf_value_data_offset );

			return( -1 );
		}
		if( libfcache_cache_get_value_by_index(
		     cache,
		     cache_entry_index,
		     &cache_value,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve cache entry: %d from cache.",
			 function,
			 cache_entry_index );

			return( -1 );
		}
		if( cache_value != NULL )
		{
			if( libfcache_cache_value_get_identifier(
			     cache_value,
			     &cache_value_file_index,
			     &cache_value_offset,
			     &cache_value_timestamp,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve cache value identifier.",
				 function );

				return( -1 );
			}
		}
		if( ( leaf_value_data_file_index != cache_value_file_index )
		 || ( leaf_value_data_offset != cache_value_offset )
		 || ( internal_tree->timestamp != cache_value_timestamp ) )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing cache value.",
			 function );

			return( -1 );
		}
	}
	if( libfcache_cache_value_get_value(
	     cache_value,
	     leaf_value,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve leaf value.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Reads the node identified by the data range
 * Returns 1 if successful or -1 on error
 */
int libfdata_btree_read_node(
     libfdata_internal_btree_t *internal_tree,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     libfdata_btree_range_t *node_data_range,
     int level,
     libfdata_btree_node_t **node,
     uint8_t read_flags,
     libcerror_error_t **error )
{
	libfcache_cache_value_t *cache_value = NULL;
	intptr_t *key_value                  = NULL;
	static char *function                = "libfdata_btree_read_node";
	off64_t cache_value_offset           = 0;
	off64_t node_data_offset             = 0;
	size64_t node_data_size              = 0;
	time_t cache_value_timestamp         = 0;
	uint32_t node_data_flags             = 0;
	int cache_entry_index                = -1;
	int cache_value_file_index           = -1;
	int node_data_file_index             = -1;
	int result                           = 0;

	if( internal_tree == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid tree.",
		 function );

		return( -1 );
	}
	if( internal_tree->read_node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid tree - missing read node function.",
		 function );

		return( -1 );
	}
	if( node_data_range == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid tree node data range.",
		 function );

		return( -1 );
	}
	if( level < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid level value out of bounds.",
		 function );

		return( -1 );
	}
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
	*node = NULL;

	if( libfdata_btree_range_get(
	     node_data_range,
	     &node_data_file_index,
	     &node_data_offset,
	     &node_data_size,
	     &node_data_flags,
	     &key_value,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve node data range values.",
		 function );

		goto on_error;
	}
	if( ( read_flags & LIBFDATA_READ_FLAG_IGNORE_CACHE ) == 0 )
	{
		if( internal_tree->calculate_node_cache_entry_index(
		     (libfdata_btree_t *) internal_tree,
		     cache,
		     level,
		     node_data_file_index,
		     node_data_offset,
		     node_data_size,
		     node_data_flags,
		     &cache_entry_index,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to calculate cache entry index.",
			 function );

			goto on_error;
		}
		if( libfcache_cache_get_value_by_index(
		     cache,
		     cache_entry_index,
		     &cache_value,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve cache entry: %d from cache.",
			 function,
			 cache_entry_index );

			goto on_error;
		}
		if( cache_value != NULL )
		{
			if( libfcache_cache_value_get_identifier(
			     cache_value,
			     &cache_value_file_index,
			     &cache_value_offset,
			     &cache_value_timestamp,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve cache value identifier.",
				 function );

				goto on_error;
			}
		}
		if( ( node_data_file_index == cache_value_file_index )
		 && ( node_data_offset == cache_value_offset )
		 && ( internal_tree->timestamp == cache_value_timestamp ) )
		{
			result = 1;
		}
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			if( result == 0 )
			{
				libcnotify_printf(
				 "%s: cache: 0x%08" PRIjx " miss (entry: %d, want: %" PRIi64 ", got: %" PRIi64 ")\n",
				 function,
				 (intptr_t) cache,
				 cache_entry_index,
				 node_data_offset,
				 cache_value_offset );
			}
			else
			{
				libcnotify_printf(
				 "%s: cache: 0x%08" PRIjx " hit (entry: %d)\n",
				 function,
				 (intptr_t) cache,
				 cache_entry_index );
			}
		}
#endif
	}
	if( result == 0 )
	{
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: reading node at offset: %" PRIi64 " (0x%08" PRIx64 ") of size: %" PRIu64 "\n",
			 function,
			 node_data_offset,
			 node_data_offset,
			 node_data_size );
		}
#endif
		if( libfdata_btree_node_initialize(
		     node,
		     level,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create node at level: %d.",
			 function,
			 level );

			goto on_error;
		}
		if( internal_tree->read_node(
		     internal_tree->data_handle,
		     file_io_handle,
		     *node,
		     node_data_file_index,
		     node_data_offset,
		     node_data_size,
		     node_data_flags,
		     key_value,
		     read_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_READ_FAILED,
			 "%s: unable to read node at offset: %" PRIi64 ".",
			 function,
			 node_data_offset );

			goto on_error;
		}
		if( ( read_flags & LIBFDATA_READ_FLAG_NO_CACHE ) == 0 )
		{
			if( internal_tree->calculate_node_cache_entry_index(
			     (libfdata_btree_t *) internal_tree,
			     cache,
			     level,
			     node_data_file_index,
			     node_data_offset,
			     node_data_size,
			     node_data_flags,
			     &cache_entry_index,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to calculate cache entry index.",
				 function );

				goto on_error;
			}
			if( libfcache_cache_set_value_by_index(
			     cache,
			     cache_entry_index,
			     node_data_file_index,
			     node_data_offset,
			     internal_tree->timestamp,
			     (intptr_t *) *node,
			     (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_btree_node_free,
			     LIBFCACHE_CACHE_VALUE_FLAG_MANAGED,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set node in cache entry: %d.",
				 function,
				 cache_entry_index );

				goto on_error;
			}
		}
	}
	else
	{
		if( libfcache_cache_value_get_value(
		     cache_value,
		     (intptr_t **) node,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve node value from cache.",
			 function );

			*node = NULL;

			goto on_error;
		}
		if( ( read_flags & LIBFDATA_READ_FLAG_NO_CACHE ) != 0 )
		{
			/* Remove the node from the cache
			 */
			if( libfcache_cache_value_clear(
			     cache_value,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to clear cache entry.",
				 function );

				*node = NULL;

				goto on_error;
			}
		}
	}
	return( 1 );

on_error:
	if( *node != NULL )
	{
		libfdata_btree_node_free(
		 node,
		 NULL );
	}
	return( -1 );
}

/* Reads the sub tree
 * Returns 1 if successful or -1 on error
 */
int libfdata_btree_read_sub_tree(
     libfdata_internal_btree_t *internal_tree,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     libfdata_btree_range_t *node_data_range,
     int level,
     int mapped_first_leaf_value_index,
     libfdata_btree_node_t **node,
     uint8_t read_flags,
     libcerror_error_t **error )
{
	libfdata_btree_range_t *leaf_value_data_range = NULL;
	libfdata_btree_range_t *sub_node_data_range   = NULL;
	libfdata_btree_node_t *sub_node               = NULL;
	intptr_t *key_value                           = NULL;
	static char *function                         = "libfdata_btree_read_sub_tree";
	off64_t node_data_offset                      = 0;
	size64_t node_data_size                       = 0;
	uint32_t node_data_flags                      = 0;
	int branch_number_of_leaf_values              = 0;
	int cache_entry_index                         = -1;
	int leaf_value_index                          = 0;
	int node_data_file_index                      = -1;
	int number_of_leaf_values                     = 0;
	int number_of_sub_nodes                       = 0;
	int result                                    = 0;
	int sub_node_index                            = 0;

	if( internal_tree == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid tree.",
		 function );

		return( -1 );
	}
	if( level < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid level value out of bounds.",
		 function );

		return( -1 );
	}
	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing node.",
		 function );

		return( -1 );
	}
	/* Note that the node data range can be cached out later in the function.
	 */
	if( libfdata_btree_range_get(
	     node_data_range,
	     &node_data_file_index,
	     &node_data_offset,
	     &node_data_size,
	     &node_data_flags,
	     &key_value,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve data range from tree node.",
		 function );

		goto on_error;
	}
	/* Make sure to not have the node cached here to prevent it from
	 * being freed during recursing the sub tree.
	 */
	if( libfdata_btree_read_node(
	     internal_tree,
	     file_io_handle,
	     cache,
	     node_data_range,
	     level,
	     node,
	     read_flags | LIBFDATA_READ_FLAG_NO_CACHE,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read node.",
		 function );

		goto on_error;
	}
	if( *node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing node.",
		 function );

		goto on_error;
	}
	if( ( ( (libfdata_internal_btree_node_t *) *node )->flags & LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES ) != 0 )
	{
		result = libfdata_btree_node_is_leaf(
			  *node,
			  error );

		if( result == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to determine if node is a leaf node.",
			 function,
			 sub_node_index );

			goto on_error;
		}
		else if( result != 0 )
		{
			if( libfdata_btree_node_get_number_of_leaf_values(
			     *node,
			     &number_of_leaf_values,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve number of leaf values.",
				 function );

				goto on_error;
			}
			for( leaf_value_index = 0;
			     leaf_value_index < number_of_leaf_values;
			     leaf_value_index++ )
			{
				if( libfdata_btree_node_get_leaf_value_data_range_by_index(
				     *node,
				     leaf_value_index,
				     &leaf_value_data_range,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve leaf value: %d data range.",
					 function,
					 leaf_value_index );

					goto on_error;
				}
				if( leaf_value_data_range == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: missing leaf value: %d data range.",
					 function,
					 leaf_value_index );

					goto on_error;
				}
				leaf_value_data_range->mapped_first_leaf_value_index = mapped_first_leaf_value_index;
				leaf_value_data_range->mapped_last_leaf_value_index  = -1;
				leaf_value_data_range->mapped_number_of_leaf_values  = 1;

				mapped_first_leaf_value_index++;
			}
			branch_number_of_leaf_values = number_of_leaf_values;

/* TODO*/
			level++;
		}
		else
		{
			if( libfdata_btree_node_get_number_of_sub_nodes(
			     *node,
			     &number_of_sub_nodes,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve number of sub nodes.",
				 function );

				goto on_error;
			}
			level++;

			for( sub_node_index = 0;
			     sub_node_index < number_of_sub_nodes;
			     sub_node_index++ )
			{
				if( libfdata_btree_node_get_sub_node_data_range_by_index(
				     *node,
				     sub_node_index,
				     &sub_node_data_range,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve sub node: %d data range.",
					 function,
					 sub_node_index );

					goto on_error;
				}
				if( sub_node_data_range == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: missing sub node: %d data range.",
					 function,
					 sub_node_index );

					goto on_error;
				}
				if( libfdata_btree_read_sub_tree(
				     internal_tree,
				     file_io_handle,
				     cache,
				     sub_node_data_range,
				     level,
				     mapped_first_leaf_value_index,
				     &sub_node,
				     read_flags,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_IO,
					 LIBCERROR_IO_ERROR_READ_FAILED,
					 "%s: unable to read sub node: %d sub tree.",
					 function,
					 sub_node_index );

					goto on_error;
				}
				if( libfdata_btree_node_get_number_of_leaf_values_in_branch(
				     sub_node,
				     &number_of_leaf_values,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve number of leaf values in branch.",
					 function );

					goto on_error;
				}
				sub_node_data_range->mapped_first_leaf_value_index = mapped_first_leaf_value_index;
				mapped_first_leaf_value_index                     += number_of_leaf_values;
				sub_node_data_range->mapped_last_leaf_value_index  = mapped_first_leaf_value_index - 1;
				sub_node_data_range->mapped_number_of_leaf_values  = number_of_leaf_values;
				branch_number_of_leaf_values                      += number_of_leaf_values;
			}
		}
		( (libfdata_internal_btree_node_t *) *node )->branch_number_of_leaf_values = branch_number_of_leaf_values;
		( (libfdata_internal_btree_node_t *) *node )->flags                       &= ~( LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES );
	}
	if( internal_tree->calculate_node_cache_entry_index(
	     (libfdata_btree_t *) internal_tree,
	     cache,
	     level,
	     node_data_file_index,
	     node_data_offset,
	     node_data_size,
	     node_data_flags,
	     &cache_entry_index,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to calculate cache entry index.",
		 function );

		goto on_error;
	}
	if( libfcache_cache_set_value_by_index(
	     cache,
	     cache_entry_index,
	     node_data_file_index,
	     node_data_offset,
	     internal_tree->timestamp,
	     (intptr_t *) *node,
	     (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_btree_node_free,
	     LIBFCACHE_CACHE_VALUE_FLAG_MANAGED,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set node in cache entry: %d.",
		 function,
		 cache_entry_index );

		goto on_error;
	}
	return( 1 );

on_error:
	if( *node != NULL )
	{
		libfdata_btree_node_free(
		 node,
		 NULL );
	}
	return( -1 );
}

/* Retrieves a leaf node the contains a specific leaf value index
 * Returns 1 if successful or -1 on error
 */
int libfdata_btree_get_leaf_node_by_index(
     libfdata_internal_btree_t *internal_tree,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     libfdata_btree_range_t *node_data_range,
     int level,
     int leaf_value_index,
     libfdata_btree_node_t **node,
     uint8_t read_flags,
     libcerror_error_t **error )
{
	libfdata_btree_range_t *sub_node_data_range = NULL;
	static char *function                       = "libfdata_btree_get_leaf_node_by_index";
	int result                                  = 0;

	if( internal_tree == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid tree.",
		 function );

		return( -1 );
	}
	if( node_data_range == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing node data range.",
		 function );

		return( -1 );
	}
	if( level < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid level value out of bounds.",
		 function );

		return( -1 );
	}
	if( leaf_value_index < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid leaf value index value out of bounds.",
		 function );

		return( -1 );
	}
	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing node.",
		 function );

		return( -1 );
	}
	if( libfdata_btree_read_sub_tree(
	     internal_tree,
	     file_io_handle,
	     cache,
	     node_data_range,
	     level,
	     node_data_range->mapped_first_leaf_value_index,
	     node,
	     read_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read node sub tree.",
		 function );

		return( -1 );
	}
	if( *node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing node.",
		 function );

		return( -1 );
	}
	result = libfdata_btree_node_is_leaf(
		  *node,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to determine if node is a leaf node.",
		 function );

		return( -1 );
	}
	else if( result != 0 )
	{
		return( 1 );
	}
	if( libfdata_btree_node_get_sub_node_data_range_by_mapped_index(
	     *node,
	     leaf_value_index,
	     &sub_node_data_range,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve sub node data range for leaf value: %d..",
		 function,
		 leaf_value_index );

		return( -1 );
	}
	*node = NULL;

	if( libfdata_btree_get_leaf_node_by_index(
	     internal_tree,
	     file_io_handle,
	     cache,
	     sub_node_data_range,
	     level + 1,
	     leaf_value_index,
	     node,
	     read_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve leaf node for leaf value: %d.",
		 function,
		 leaf_value_index );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves a leaf node for a specific key value
 *
 * Uses the key_value_compare_function to determine the similarity of the key values
 * The key_value_compare_function should return LIBFDATA_COMPARE_LESS,
 * LIBFDATA_COMPARE_LESS_EQUAL, LIBFDATA_COMPARE_EQUAL, LIBFDATA_COMPARE_GREATER,
 * LIBFDATA_COMPARE_GREATER_EQUAL if successful or -1 on error
 *
 * Returns 1 if successful, 0 if no such value or -1 on error
 */
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
     libcerror_error_t **error )
{
	libfdata_btree_range_t *sub_node_data_range = NULL;
	static char *function                       = "libfdata_btree_get_leaf_node_by_key";
	int result                                  = 0;
	int sub_node_index                          = 0;

	if( internal_tree == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid tree.",
		 function );

		return( -1 );
	}
	if( node_data_range == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing node data range.",
		 function );

		return( -1 );
	}
	if( maximum_node_level < -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid maximum node level value out of bounds.",
		 function );

		return( -1 );
	}
	if( current_node_level < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid current node level value out of bounds.",
		 function );

		return( -1 );
	}
	if( node_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing node index.",
		 function );

		return( -1 );
	}
	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing node.",
		 function );

		return( -1 );
	}
	if( libfdata_btree_read_sub_tree(
	     internal_tree,
	     file_io_handle,
	     cache,
	     node_data_range,
	     current_node_level,
	     node_data_range->mapped_first_leaf_value_index,
	     node,
	     read_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read node sub tree.",
		 function );

		return( -1 );
	}
	if( *node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing node.",
		 function );

		return( -1 );
	}
	result = libfdata_btree_node_is_leaf(
		  *node,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to determine if node is a leaf node.",
		 function );

		return( -1 );
	}
	else if( result != 0 )
	{
		return( 1 );
	}
	if( ( maximum_node_level != LIBFDATA_BTREE_NODE_LEVEL_UNLIMITED )
	 && ( current_node_level >= maximum_node_level ) )
	{
		return( 1 );
	}
	result = libfdata_btree_node_get_sub_node_data_range_by_key(
		  *node,
		  key_value,
		  key_value_compare_function,
		  &sub_node_index,
		  &sub_node_data_range,
		  error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve sub node data range by key.",
		 function );

		return( -1 );
	}
	else if( result == 0 )
	{
		return( 0 );
	}
	*node_index = sub_node_index;
	*node       = NULL;

	result = libfdata_btree_get_leaf_node_by_key(
		  internal_tree,
		  file_io_handle,
		  cache,
		  sub_node_data_range,
		  maximum_node_level,
		  current_node_level + 1,
		  key_value,
		  key_value_compare_function,
		  node_index,
		  node,
		  read_flags,
		  error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve leaf node by key.",
		 function );

		return( -1 );
	}
	return( result );
}

/* Retrieves the next leaf node for a specific key value
 *
 * This function will try to determine the next node based
 * on the node_index and node values.
 *
 * Uses the key_value_compare_function to determine the similarity of the key values
 * The key_value_compare_function should return LIBFDATA_COMPARE_LESS,
 * LIBFDATA_COMPARE_LESS_EQUAL, LIBFDATA_COMPARE_EQUAL, LIBFDATA_COMPARE_GREATER,
 * LIBFDATA_COMPARE_GREATER_EQUAL if successful or -1 on error
 *
 * Returns 1 if successful, 0 if no such value or -1 on error
 */
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
     libcerror_error_t **error )
{
	libfdata_btree_node_t *branch_node          = NULL;
	libfdata_btree_range_t *sub_node_data_range = NULL;
	static char *function                       = "libfdata_btree_get_next_leaf_node_by_key";
	int branch_node_index                       = 0;
	int branch_node_level                       = 0;
	int node_level                              = 0;
	int number_of_sub_nodes                     = 0;
	int result                                  = 0;

	if( internal_tree == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid tree.",
		 function );

		return( -1 );
	}
	if( node_index < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid node index value out of bounds.",
		 function );

		return( -1 );
	}
	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing node.",
		 function );

		return( -1 );
	}
	if( libfdata_btree_node_get_level(
	     node,
	     &node_level,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve node level.",
		 function );

		return( -1 );
	}
	if( node_level == 0 )
	{
		return( 0 );
	}
	branch_node_level = node_level;

	while( branch_node_level > 0 )
	{
		branch_node_level--;

		result = libfdata_btree_get_leaf_node_by_key(
		          internal_tree,
		          file_io_handle,
		          cache,
		          node_data_range,
		          branch_node_level,
		          0,
		          key_value,
		          key_value_compare_function,
		          &branch_node_index,
		          &branch_node,
		          read_flags,
		          error );

		if( result == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve branch node at level: %d.",
			 function,
			 branch_node_level );

			return( -1 );
		}
		else if( result == 0 )
		{
			return( 0 );
		}
		if( libfdata_btree_node_get_number_of_sub_nodes(
		     branch_node,
		     &number_of_sub_nodes,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve number of sub nodes.",
			 function );

			return( -1 );
		}
		if( ( node_index < 0 )
		 || ( node_index >= number_of_sub_nodes ) )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: invalid node index value out of bounds.",
			 function );

			return( -1 );
		}
		node_index++;

		if( node_index < number_of_sub_nodes )
		{
			if( libfdata_btree_node_get_sub_node_data_range_by_index(
			     branch_node,
			     node_index,
			     &sub_node_data_range,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve next sub node: %d data range.",
				 function,
				 node_index );

				return( -1 );
			}
			result = libfdata_btree_get_leaf_node_by_key(
			          internal_tree,
			          file_io_handle,
			          cache,
			          sub_node_data_range,
			          LIBFDATA_BTREE_NODE_LEVEL_UNLIMITED,
			          branch_node_level + 1,
			          key_value,
			          key_value_compare_function,
			          next_node_index,
			          next_node,
			          read_flags,
			          error );

			if( result == -1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve branch node at level: %d.",
				 function,
				 branch_node_level );

				return( -1 );
			}
			return( result );
		}
		/* If no next leaf node is available try the next branch
		 */
		node_index = branch_node_index;
	}
	return( 0 );
}

/* Retrieves the previous leaf node for a specific key value
 *
 * This function will try to determine the previous node based
 * on the node_index and node values.
 *
 * Uses the key_value_compare_function to determine the similarity of the key values
 * The key_value_compare_function should return LIBFDATA_COMPARE_LESS,
 * LIBFDATA_COMPARE_LESS_EQUAL, LIBFDATA_COMPARE_EQUAL, LIBFDATA_COMPARE_GREATER,
 * LIBFDATA_COMPARE_GREATER_EQUAL if successful or -1 on error
 *
 * Returns 1 if successful, 0 if no such value or -1 on error
 */
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
     libcerror_error_t **error )
{
	libfdata_btree_node_t *branch_node          = NULL;
	libfdata_btree_range_t *sub_node_data_range = NULL;
	static char *function                       = "libfdata_btree_get_previous_leaf_node_by_key";
	int branch_node_index                       = 0;
	int branch_node_level                       = 0;
	int node_level                              = 0;
	int number_of_sub_nodes                     = 0;
	int result                                  = 0;

	if( internal_tree == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid tree.",
		 function );

		return( -1 );
	}
	if( node_index < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid node index value out of bounds.",
		 function );

		return( -1 );
	}
	if( node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing node.",
		 function );

		return( -1 );
	}
	if( libfdata_btree_node_get_level(
	     node,
	     &node_level,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve node level.",
		 function );

		return( -1 );
	}
	if( node_level == 0 )
	{
		return( 0 );
	}
	branch_node_level = node_level;

	while( branch_node_level > 0 )
	{
		branch_node_level--;

		result = libfdata_btree_get_leaf_node_by_key(
		          internal_tree,
		          file_io_handle,
		          cache,
		          node_data_range,
		          branch_node_level,
		          0,
		          key_value,
		          key_value_compare_function,
		          &branch_node_index,
		          &branch_node,
		          read_flags,
		          error );

		if( result == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve branch node at level: %d.",
			 function,
			 branch_node_level );

			return( -1 );
		}
		else if( result == 0 )
		{
			return( 0 );
		}
		if( libfdata_btree_node_get_number_of_sub_nodes(
		     branch_node,
		     &number_of_sub_nodes,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve number of sub nodes.",
			 function );

			return( -1 );
		}
		if( ( node_index < 0 )
		 || ( node_index >= number_of_sub_nodes ) )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: invalid node index value out of bounds.",
			 function );

			return( -1 );
		}
		node_index--;

		if( node_index >= 0 )
		{
			if( libfdata_btree_node_get_sub_node_data_range_by_index(
			     branch_node,
			     node_index,
			     &sub_node_data_range,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve previous sub node: %d data range.",
				 function,
				 node_index );

				return( -1 );
			}
			result = libfdata_btree_get_leaf_node_by_key(
			          internal_tree,
			          file_io_handle,
			          cache,
			          sub_node_data_range,
			          LIBFDATA_BTREE_NODE_LEVEL_UNLIMITED,
			          branch_node_level + 1,
			          key_value,
			          key_value_compare_function,
			          previous_node_index,
			          previous_node,
			          read_flags,
			          error );

			if( result == -1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve branch node at level: %d.",
				 function,
				 branch_node_level );

				return( -1 );
			}
			return( result );
		}
		/* If no previous leaf node is available try the previous branch
		 */
		node_index = branch_node_index;
	}
	return( 0 );
}

/* Retrieves the number of leaf values in the tree
 * Returns 1 if successful or -1 on error
 */
int libfdata_btree_get_number_of_leaf_values(
     libfdata_btree_t *tree,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     int *number_of_values,
     uint8_t read_flags,
     libcerror_error_t **error )
{
	libfdata_btree_node_t *root_node         = NULL;
	libfdata_internal_btree_t *internal_tree = NULL;
	static char *function                    = "libfdata_btree_get_number_of_leaf_values";

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
	internal_tree = (libfdata_internal_btree_t *) tree;

	if( internal_tree->root_node_data_range == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid tree - missing root node data range.",
		 function );

		return( -1 );
	}
	if( libfdata_btree_read_sub_tree(
	     internal_tree,
	     file_io_handle,
	     cache,
	     internal_tree->root_node_data_range,
	     0,
	     0,
	     &root_node,
	     read_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read root node sub tree.",
		 function );

		return( -1 );
	}
	if( libfdata_btree_node_get_number_of_leaf_values_in_branch(
	     root_node,
	     number_of_values,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of leaf values in branch.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves a leaf value at a specific index
 * Returns 1 if successful or -1 on error
 */
int libfdata_btree_get_leaf_value_by_index(
     libfdata_btree_t *tree,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     int leaf_value_index,
     intptr_t **value,
     uint8_t read_flags,
     libcerror_error_t **error )
{
	libfdata_btree_node_t *node                   = NULL;
	libfdata_btree_range_t *leaf_value_data_range = NULL;
	libfdata_internal_btree_t *internal_tree      = NULL;
	static char *function                         = "libfdata_btree_get_leaf_value_by_index";

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
	internal_tree = (libfdata_internal_btree_t *) tree;

	if( internal_tree->root_node_data_range == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid tree - missing root node data range.",
		 function );

		return( -1 );
	}
	if( libfdata_btree_get_leaf_node_by_index(
	     internal_tree,
	     file_io_handle,
	     cache,
	     internal_tree->root_node_data_range,
	     0,
	     leaf_value_index,
	     &node,
	     read_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve leaf node.",
		 function );

		return( -1 );
	}
	if( libfdata_btree_node_get_leaf_value_data_range_by_mapped_index(
	     node,
	     leaf_value_index,
	     &leaf_value_data_range,
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
	if( libfdata_btree_read_leaf_value(
	     internal_tree,
	     file_io_handle,
	     cache,
	     leaf_value_data_range,
	     value,
	     read_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read leaf value.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Sets a specific leaf value
 *
 * If the flag LIBFDATA_BTREE_LEAF_VALUE_FLAG_MANAGED is set the vector
 * takes over management of the value and the value is freed when
 * no longer needed.
 *
 * Returns 1 if successful or -1 on error
 */
int libfdata_btree_set_leaf_value_by_index(
     libfdata_btree_t *tree,
     intptr_t *file_io_handle LIBFDATA_ATTRIBUTE_UNUSED,
     libfcache_cache_t *cache,
     int leaf_value_index,
     intptr_t *leaf_value,
     int (*free_leaf_value)(
            intptr_t **leaf_value,
     libcerror_error_t **error ),
     uint8_t write_flags,
     libcerror_error_t **error )
{
	libfdata_btree_node_t *node                   = NULL;
	libfdata_btree_range_t *leaf_value_data_range = NULL;
	libfdata_internal_btree_t *internal_tree      = NULL;
	intptr_t *key_value                           = NULL;
	static char *function                         = "libfdata_btree_set_leaf_value_by_index";
	off64_t leaf_value_data_offset                = 0;
	size64_t leaf_value_data_size                 = 0;
	uint32_t leaf_value_data_flags                = 0;
	int cache_entry_index                         = -1;
	int leaf_value_data_file_index                = -1;

	LIBFDATA_UNREFERENCED_PARAMETER( file_io_handle )

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
	internal_tree = (libfdata_internal_btree_t *) tree;

	if( internal_tree->root_node_data_range == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid tree - missing root node data range.",
		 function );

		return( -1 );
	}
	if( libfdata_btree_get_leaf_node_by_index(
	     internal_tree,
	     file_io_handle,
	     cache,
	     internal_tree->root_node_data_range,
	     0,
	     leaf_value_index,
	     &node,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve leaf node.",
		 function );

		return( -1 );
	}
	if( libfdata_btree_node_get_leaf_value_data_range_by_mapped_index(
	     node,
	     leaf_value_index,
	     &leaf_value_data_range,
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
	     leaf_value_data_range,
	     &leaf_value_data_file_index,
	     &leaf_value_data_offset,
	     &leaf_value_data_size,
	     &leaf_value_data_flags,
	     &key_value,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve leaf value data range values.",
		 function );

		return( -1 );
	}
	if( internal_tree->calculate_leaf_value_cache_entry_index(
	     (libfdata_btree_t *) internal_tree,
	     cache,
	     leaf_value_index,
	     leaf_value_data_file_index,
	     leaf_value_data_offset,
	     leaf_value_data_size,
	     leaf_value_data_flags,
	     &cache_entry_index,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to calculate cache entry index.",
		 function );

		return( -1 );
	}
	if( libfcache_cache_set_value_by_index(
	     cache,
	     cache_entry_index,
	     leaf_value_data_file_index,
	     leaf_value_data_offset,
	     internal_tree->timestamp,
	     leaf_value,
	     free_leaf_value,
	     write_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set leaf value in cache entry: %d.",
		 function,
		 cache_entry_index );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves a leaf value for a specific key value
 *
 * Uses the key_value_compare_function to determine the similarity of the key values
 * The key_value_compare_function should return LIBFDATA_COMPARE_LESS,
 * LIBFDATA_COMPARE_LESS_EQUAL, LIBFDATA_COMPARE_EQUAL, LIBFDATA_COMPARE_GREATER,
 * LIBFDATA_COMPARE_GREATER_EQUAL if successful or -1 on error
 *
 * Returns 1 if successful, 0 if no such value or -1 on error
 */
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
     libcerror_error_t **error )
{
	libfdata_btree_node_t *leaf_node              = NULL;
	libfdata_btree_range_t *leaf_value_data_range = NULL;
	libfdata_internal_btree_t *internal_tree      = NULL;
	static char *function                         = "libfdata_btree_get_leaf_value_by_key";
	int leaf_node_index                           = 0;
	int leaf_node_level                           = 0;
	int node_index                                = 0;
	int result                                    = 0;

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
	internal_tree = (libfdata_internal_btree_t *) tree;

	if( internal_tree->root_node_data_range == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid tree - missing root node data range.",
		 function );

		return( -1 );
	}
	if( ( search_flags & ~( LIBFDATA_BTREE_SEARCH_FLAG_SCAN_PREVIOUS_NODE | LIBFDATA_BTREE_SEARCH_FLAG_SCAN_NEXT_NODE ) ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported search flags.",
		 function );

		return( -1 );
	}
	result = libfdata_btree_get_leaf_node_by_key(
	          internal_tree,
	          file_io_handle,
	          cache,
	          internal_tree->root_node_data_range,
	          LIBFDATA_BTREE_NODE_LEVEL_UNLIMITED,
	          0,
	          key_value,
	          key_value_compare_function,
	          &node_index,
	          &leaf_node,
	          read_flags,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve leaf node.",
		 function );

		return( -1 );
	}
	else if( result == 0 )
	{
		return( 0 );
	}
	if( libfdata_btree_node_get_level(
	     leaf_node,
	     &leaf_node_level,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve leaf node level.",
		 function );

		return( -1 );
	}
	result = libfdata_btree_node_get_leaf_value_data_range_by_key(
	          leaf_node,
	          key_value,
	          key_value_compare_function,
	          &leaf_value_data_range,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve leaf value data range.",
		 function );

		return( -1 );
	}
	if( ( result == 0 )
	 && ( ( search_flags & LIBFDATA_BTREE_SEARCH_FLAG_SCAN_PREVIOUS_NODE ) != 0 ) )
	{
		result = libfdata_btree_get_previous_leaf_node_by_key(
		          internal_tree,
		          file_io_handle,
		          cache,
		          internal_tree->root_node_data_range,
		          node_index,
		          leaf_node,
		          key_value,
		          key_value_compare_function,
		          &leaf_node_index,
		          &leaf_node,
		          read_flags,
		          error );

		if( result == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve previous leaf node.",
			 function );

			return( -1 );
		}
		else if( result != 0 )
		{
			result = libfdata_btree_node_get_leaf_value_data_range_by_key(
				  leaf_node,
				  key_value,
				  key_value_compare_function,
				  &leaf_value_data_range,
				  error );

			if( result == -1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve previous leaf value data range.",
				 function );

				return( -1 );
			}
		}
	}
	if( ( result == 0 )
	 && ( ( search_flags & LIBFDATA_BTREE_SEARCH_FLAG_SCAN_NEXT_NODE ) != 0 ) )
	{
		result = libfdata_btree_get_next_leaf_node_by_key(
		          internal_tree,
		          file_io_handle,
		          cache,
		          internal_tree->root_node_data_range,
		          node_index,
		          leaf_node,
		          key_value,
		          key_value_compare_function,
		          &leaf_node_index,
		          &leaf_node,
		          read_flags,
		          error );

		if( result == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve next leaf node.",
			 function );

			return( -1 );
		}
		else if( result != 0 )
		{
			result = libfdata_btree_node_get_leaf_value_data_range_by_key(
				  leaf_node,
				  key_value,
				  key_value_compare_function,
				  &leaf_value_data_range,
				  error );

			if( result == -1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve next leaf value data range.",
				 function );

				return( -1 );
			}
		}
	}
	if( result != 0 )
	{
		if( libfdata_btree_read_leaf_value(
		     internal_tree,
		     file_io_handle,
		     cache,
		     leaf_value_data_range,
		     value,
		     read_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_READ_FAILED,
			 "%s: unable to read leaf value.",
			 function );

			return( -1 );
		}
	}
	return( result );
}

