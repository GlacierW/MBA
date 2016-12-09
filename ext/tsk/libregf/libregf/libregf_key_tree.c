/*
 * Key tree functions
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

#include <common.h>
#include <types.h>

#include "libregf_definitions.h"
#include "libregf_key_item.h"
#include "libregf_key_tree.h"
#include "libregf_libcerror.h"
#include "libregf_libfcache.h"
#include "libregf_libfdata.h"

/* Retrieves the sub key values (key tree node and key item) for the specific UTF-8 formatted name
 * Returns 1 if successful, 0 if no such sub key item or -1 on error
 */
int libregf_key_tree_get_sub_key_values_by_utf8_name(
     libfdata_tree_node_t *key_tree_node,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *key_cache,
     uint32_t name_hash,
     const uint8_t *utf8_string,
     size_t utf8_string_length,
     int ascii_codepage,
     libfdata_tree_node_t **key_tree_sub_node,
     libregf_key_item_t **sub_key_item,
     libcerror_error_t **error )
{
	static char *function  = "libregf_key_tree_get_sub_key_values_by_utf8_name";
	int number_of_sub_keys = 0;
	int result             = 0;
	int sub_key_index      = 0;

	if( key_tree_sub_node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key tree sub node.",
		 function );

		return( -1 );
	}
	if( sub_key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid sub key item.",
		 function );

		return( -1 );
	}
	if( libfdata_tree_node_get_number_of_sub_nodes(
	     key_tree_node,
	     (intptr_t *) file_io_handle,
	     key_cache,
	     &number_of_sub_keys,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of sub keys.",
		 function );

		return( -1 );
	}
	for( sub_key_index = 0;
	     sub_key_index < number_of_sub_keys;
	     sub_key_index++ )
	{
		if( libfdata_tree_node_get_sub_node_by_index(
		     key_tree_node,
		     (intptr_t *) file_io_handle,
		     key_cache,
		     sub_key_index,
		     key_tree_sub_node,
		     0,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve key tree sub node: %d.",
			 function,
			 sub_key_index );

			return( -1 );
		}
		if( libfdata_tree_node_get_node_value(
		     *key_tree_sub_node,
		     (intptr_t *) file_io_handle,
		     key_cache,
		     (intptr_t **) sub_key_item,
		     0,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve sub key item.",
			 function );

			return( -1 );
		}
		result = libregf_key_item_compare_name_with_utf8_string(
		          *sub_key_item,
		          name_hash,
		          utf8_string,
		          utf8_string_length,
		          ascii_codepage,
		          error );

		if( result == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GENERIC,
			 "%s: unable to compare sub key name with UTF-8 string.",
			 function );

			return( -1 );
		}
		else if( result != 0 )
		{
			break;
		}
	}
	if( sub_key_index >= number_of_sub_keys )
	{
		return( 0 );
	}
	return( 1 );
}

/* Retrieves the sub key values (key tree node and key item) for the specific UTF-16 formatted name
 * Returns 1 if successful, 0 if no such sub key item or -1 on error
 */
int libregf_key_tree_get_sub_key_values_by_utf16_name(
     libfdata_tree_node_t *key_tree_node,
     libbfio_handle_t *file_io_handle,
     libfcache_cache_t *key_cache,
     uint32_t name_hash,
     const uint16_t *utf16_string,
     size_t utf16_string_length,
     int ascii_codepage,
     libfdata_tree_node_t **key_tree_sub_node,
     libregf_key_item_t **sub_key_item,
     libcerror_error_t **error )
{
	static char *function  = "libregf_key_tree_get_sub_key_values_by_utf16_name";
	int number_of_sub_keys = 0;
	int result             = 0;
	int sub_key_index      = 0;

	if( key_tree_sub_node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key tree sub node.",
		 function );

		return( -1 );
	}
	if( sub_key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid sub key item.",
		 function );

		return( -1 );
	}
	if( libfdata_tree_node_get_number_of_sub_nodes(
	     key_tree_node,
	     (intptr_t *) file_io_handle,
	     key_cache,
	     &number_of_sub_keys,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of sub keys.",
		 function );

		return( -1 );
	}
	for( sub_key_index = 0;
	     sub_key_index < number_of_sub_keys;
	     sub_key_index++ )
	{
		if( libfdata_tree_node_get_sub_node_by_index(
		     key_tree_node,
		     (intptr_t *) file_io_handle,
		     key_cache,
		     sub_key_index,
		     key_tree_sub_node,
		     0,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve key tree sub node: %d.",
			 function,
			 sub_key_index );

			return( -1 );
		}
		if( libfdata_tree_node_get_node_value(
		     *key_tree_sub_node,
		     (intptr_t *) file_io_handle,
		     key_cache,
		     (intptr_t **) sub_key_item,
		     0,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve sub key item.",
			 function );

			return( -1 );
		}
		result = libregf_key_item_compare_name_with_utf16_string(
		          *sub_key_item,
		          name_hash,
		          utf16_string,
		          utf16_string_length,
		          ascii_codepage,
		          error );

		if( result == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GENERIC,
			 "%s: unable to compare sub key name with UTF-16 string.",
			 function );

			return( -1 );
		}
		else if( result != 0 )
		{
			break;
		}
	}
	if( sub_key_index >= number_of_sub_keys )
	{
		return( 0 );
	}
	return( 1 );
}

