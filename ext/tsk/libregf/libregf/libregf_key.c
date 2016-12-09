/*
 * Key functions
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
#include <memory.h>
#include <types.h>

#if defined( HAVE_WCTYPE_H )
#include <wctype.h>
#endif

#include "libregf_definitions.h"
#include "libregf_io_handle.h"
#include "libregf_key.h"
#include "libregf_key_item.h"
#include "libregf_key_tree.h"
#include "libregf_libbfio.h"
#include "libregf_libcerror.h"
#include "libregf_libfcache.h"
#include "libregf_libfdata.h"
#include "libregf_libuna.h"
#include "libregf_value.h"
#include "libregf_value_item.h"
#include "libregf_value_type.h"

/* Creates a key
 * Make sure the value key is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libregf_key_initialize(
     libregf_key_t **key,
     libregf_io_handle_t *io_handle,
     libbfio_handle_t *file_io_handle,
     libfdata_tree_node_t *key_tree_node,
     libfcache_cache_t *key_cache,
     libcerror_error_t **error )
{
	libregf_internal_key_t *internal_key = NULL;
	static char *function                = "libregf_key_initialize";

	if( key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key.",
		 function );

		return( -1 );
	}
	if( *key != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid key value already set.",
		 function );

		return( -1 );
	}
	if( key_tree_node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key tree node.",
		 function );

		return( -1 );
	}
	internal_key = memory_allocate_structure(
	                libregf_internal_key_t );

	if( internal_key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create internal key.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_key,
	     0,
	     sizeof( libregf_internal_key_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear internal key.",
		 function );

		memory_free(
		 internal_key );

		return( -1 );
	}
	internal_key->file_io_handle = file_io_handle;
	internal_key->io_handle      = io_handle;
	internal_key->key_tree_node  = key_tree_node;
	internal_key->key_cache      = key_cache;

	*key = (libregf_key_t *) internal_key;

	return( 1 );

on_error:
	if( internal_key != NULL )
	{
		memory_free(
		 internal_key );
	}
	return( -1 );
}

/* Frees a key
 * Returns 1 if successful or -1 on error
 */
int libregf_key_free(
     libregf_key_t **key,
     libcerror_error_t **error )
{
	libregf_internal_key_t *internal_key = NULL;
	static char *function                = "libregf_key_free";
	int result                           = 1;

	if( key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key.",
		 function );

		return( -1 );
	}
	if( *key != NULL )
	{
		internal_key = (libregf_internal_key_t *) *key;
		*key         = NULL;

		/* The io_handle, file_io_handle, key_tree_node and key_cache references are freed elsewhere
		 */
		memory_free(
		 internal_key );
	}
	return( result );
}

/* Determine if the key corrupted
 * Returns 1 if corrupted, 0 if not or -1 on error
 */
int libregf_key_is_corrupted(
     libregf_key_t *key,
     libcerror_error_t **error )
{
	libregf_internal_key_t *internal_key = NULL;
	libregf_key_item_t *key_item         = NULL;
	static char *function                = "libregf_key_is_corrupted";

	if( key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key.",
		 function );

		return( -1 );
	}
	internal_key = (libregf_internal_key_t *) key;

	if( libfdata_tree_node_get_node_value(
	     internal_key->key_tree_node,
	     (intptr_t *) internal_key->file_io_handle,
	     internal_key->key_cache,
	     (intptr_t **) &key_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve key item.",
		 function );

		return( -1 );
	}
	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing key item.",
		 function );

		return( -1 );
	}
	if( ( key_item->item_flags & LIBREGF_KEY_ITEM_FLAG_IS_CORRUPTED ) != 0 )
	{
		return( 1 );
	}
	return( 0 );
}

/* Retrieves the offset of the key
 * Returns 1 if successful or -1 on error
 */
int libregf_key_get_offset(
     libregf_key_t *key,
     off64_t *offset,
     libcerror_error_t **error )
{
	libregf_internal_key_t *internal_key = NULL;
	static char *function                = "libregf_key_get_offset";
	size64_t size                        = 0;
	uint32_t flags                       = 0;
	int file_index                       = 0;

	if( key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key.",
		 function );

		return( -1 );
	}
	internal_key = (libregf_internal_key_t *) key;

	if( internal_key->io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid key - missing IO handle.",
		 function );

		return( -1 );
	}
	if( offset == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid offset.",
		 function );

		return( -1 );
	}
	if( libfdata_tree_node_get_data_range(
	     internal_key->key_tree_node,
	     &file_index,
	     offset,
	     &size,
	     &flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve key data range.",
		 function );

		return( -1 );
	}
	/* The offset is relative from the start of the hive bins list
	 * and points to the start of the corresponding hive bin cell
	 */
	*offset += internal_key->io_handle->hive_bins_list_offset + 4;

	return( 1 );
}

/* Retrieves the key name size
 * Returns 1 if successful or -1 on error
 */
int libregf_key_get_name_size(
     libregf_key_t *key,
     size_t *name_size,
     libcerror_error_t **error )
{
	libregf_internal_key_t *internal_key = NULL;
	libregf_key_item_t *key_item         = NULL;
	static char *function                = "libregf_key_get_name_size";

	if( key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key.",
		 function );

		return( -1 );
	}
	internal_key = (libregf_internal_key_t *) key;

	if( name_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid name size.",
		 function );

		return( -1 );
	}
	if( libfdata_tree_node_get_node_value(
	     internal_key->key_tree_node,
	     (intptr_t *) internal_key->file_io_handle,
	     internal_key->key_cache,
	     (intptr_t **) &key_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve key item.",
		 function );

		return( -1 );
	}
	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing key item.",
		 function );

		return( -1 );
	}
	*name_size = key_item->name_size;

	return( 1 );
}

/* Retrieves the key name data and size
 * Returns 1 if successful or -1 on error
 */
int libregf_key_get_name(
     libregf_key_t *key,
     uint8_t *name,
     size_t name_size,
     libcerror_error_t **error )
{
	libregf_internal_key_t *internal_key = NULL;
	libregf_key_item_t *key_item         = NULL;
	static char *function                = "libregf_key_get_name";

	if( key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key.",
		 function );

		return( -1 );
	}
	internal_key = (libregf_internal_key_t *) key;

	if( name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid name.",
		 function );

		return( -1 );
	}
	if( name_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid name size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( libfdata_tree_node_get_node_value(
	     internal_key->key_tree_node,
	     (intptr_t *) internal_key->file_io_handle,
	     internal_key->key_cache,
	     (intptr_t **) &key_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve key item.",
		 function );

		return( -1 );
	}
	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing key item.",
		 function );

		return( -1 );
	}
	if( name_size < key_item->name_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid name size value out of bounds.",
		 function );

		return( -1 );
	}
	if( memory_copy(
	     name,
	     key_item->name,
	     key_item->name_size ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
		 "%s: unable to copy name.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the UTF-8 string size of the key name
 * The returned size includes the end of string character
 * Returns 1 if successful or -1 on error
 */
int libregf_key_get_utf8_name_size(
     libregf_key_t *key,
     size_t *utf8_name_size,
     libcerror_error_t **error )
{
	libregf_internal_key_t *internal_key = NULL;
	libregf_key_item_t *key_item         = NULL;
	static char *function                = "libregf_key_get_utf8_name_size";
	int result                           = 0;

	if( key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key.",
		 function );

		return( -1 );
	}
	internal_key = (libregf_internal_key_t *) key;

	if( internal_key->io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid key - missing IO handle.",
		 function );

		return( -1 );
	}
	if( libfdata_tree_node_get_node_value(
	     internal_key->key_tree_node,
	     (intptr_t *) internal_key->file_io_handle,
	     internal_key->key_cache,
	     (intptr_t **) &key_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve key item.",
		 function );

		return( -1 );
	}
	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing key item.",
		 function );

		return( -1 );
	}
	if( key_item->name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid key item - missing name.",
		 function );

		return( -1 );
	}
	if( ( key_item->flags & LIBREGF_NAMED_KEY_FLAG_NAME_IS_ASCII ) != 0 )
	{
		result = libuna_utf8_string_size_from_byte_stream(
			  key_item->name,
			  (size_t) key_item->name_size,
			  internal_key->io_handle->ascii_codepage,
			  utf8_name_size,
			  error );
	}
	else
	{
		result = libuna_utf8_string_size_from_utf16_stream(
			  key_item->name,
			  (size_t) key_item->name_size,
			  LIBUNA_ENDIAN_LITTLE,
			  utf8_name_size,
			  error );
	}
	if( result != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve UTF-8 string size.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the UTF-8 string value of the key name
 * The function uses a codepage if necessary, it uses the codepage set for the library
 * The size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libregf_key_get_utf8_name(
     libregf_key_t *key,
     uint8_t *utf8_name,
     size_t utf8_name_size,
     libcerror_error_t **error )
{
	libregf_internal_key_t *internal_key = NULL;
	libregf_key_item_t *key_item         = NULL;
	static char *function                = "libregf_key_get_utf8_name";
	int result                           = 0;

	if( key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key.",
		 function );

		return( -1 );
	}
	internal_key = (libregf_internal_key_t *) key;

	if( internal_key->io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid key - missing IO handle.",
		 function );

		return( -1 );
	}
	if( libfdata_tree_node_get_node_value(
	     internal_key->key_tree_node,
	     (intptr_t *) internal_key->file_io_handle,
	     internal_key->key_cache,
	     (intptr_t **) &key_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve key item.",
		 function );

		return( -1 );
	}
	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing key item.",
		 function );

		return( -1 );
	}
	if( key_item->name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid key item - missing name.",
		 function );

		return( -1 );
	}
	if( ( key_item->flags & LIBREGF_NAMED_KEY_FLAG_NAME_IS_ASCII ) != 0 )
	{
		result = libuna_utf8_string_copy_from_byte_stream(
			  utf8_name,
			  utf8_name_size,
			  key_item->name,
			  (size_t) key_item->name_size,
			  internal_key->io_handle->ascii_codepage,
			  error );
	}
	else
	{
		result = libuna_utf8_string_copy_from_utf16_stream(
			  utf8_name,
			  utf8_name_size,
			  key_item->name,
			  (size_t) key_item->name_size,
			  LIBUNA_ENDIAN_LITTLE,
			  error );
	}
	if( result != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve UTF-8 string.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the UTF-16 string size of the key name
 * The returned size includes the end of string character
 * Returns 1 if successful or -1 on error
 */
int libregf_key_get_utf16_name_size(
     libregf_key_t *key,
     size_t *utf16_name_size,
     libcerror_error_t **error )
{
	libregf_internal_key_t *internal_key = NULL;
	libregf_key_item_t *key_item         = NULL;
	static char *function                = "libregf_key_get_utf16_name_size";
	int result                           = 0;

	if( key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key.",
		 function );

		return( -1 );
	}
	internal_key = (libregf_internal_key_t *) key;

	if( internal_key->io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid key - missing IO handle.",
		 function );

		return( -1 );
	}
	if( libfdata_tree_node_get_node_value(
	     internal_key->key_tree_node,
	     (intptr_t *) internal_key->file_io_handle,
	     internal_key->key_cache,
	     (intptr_t **) &key_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve key item.",
		 function );

		return( -1 );
	}
	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing key item.",
		 function );

		return( -1 );
	}
	if( key_item->name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid key item - missing name.",
		 function );

		return( -1 );
	}
	if( ( key_item->flags & LIBREGF_NAMED_KEY_FLAG_NAME_IS_ASCII ) != 0 )
	{
		result = libuna_utf16_string_size_from_byte_stream(
			  key_item->name,
			  (size_t) key_item->name_size,
			  internal_key->io_handle->ascii_codepage,
			  utf16_name_size,
			  error );
	}
	else
	{
		result = libuna_utf16_string_size_from_utf16_stream(
			  key_item->name,
			  (size_t) key_item->name_size,
			  LIBUNA_ENDIAN_LITTLE,
			  utf16_name_size,
			  error );
	}
	if( result != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve UTF-16 string size.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the UTF-16 string value of the key name
 * The function uses a codepage if necessary, it uses the codepage set for the library
 * The size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libregf_key_get_utf16_name(
     libregf_key_t *key,
     uint16_t *utf16_name,
     size_t utf16_name_size,
     libcerror_error_t **error )
{
	libregf_internal_key_t *internal_key = NULL;
	libregf_key_item_t *key_item         = NULL;
	static char *function                = "libregf_value_get_utf16_name";
	int result                           = 0;

	if( key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key.",
		 function );

		return( -1 );
	}
	internal_key = (libregf_internal_key_t *) key;

	if( internal_key->io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid key - missing IO handle.",
		 function );

		return( -1 );
	}
	if( libfdata_tree_node_get_node_value(
	     internal_key->key_tree_node,
	     (intptr_t *) internal_key->file_io_handle,
	     internal_key->key_cache,
	     (intptr_t **) &key_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve key item.",
		 function );

		return( -1 );
	}
	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing key item.",
		 function );

		return( -1 );
	}
	if( key_item->name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid key item - missing name.",
		 function );

		return( -1 );
	}
	if( ( key_item->flags & LIBREGF_NAMED_KEY_FLAG_NAME_IS_ASCII ) != 0 )
	{
		result = libuna_utf16_string_copy_from_byte_stream(
			  utf16_name,
			  utf16_name_size,
			  key_item->name,
			  (size_t) key_item->name_size,
			  internal_key->io_handle->ascii_codepage,
			  error );
	}
	else
	{
		result = libuna_utf16_string_copy_from_utf16_stream(
			  utf16_name,
			  utf16_name_size,
			  key_item->name,
			  (size_t) key_item->name_size,
			  LIBUNA_ENDIAN_LITTLE,
			  error );
	}
	if( result != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve UTF-16 string.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the class name size
 * Returns 1 if successful, 0 if no such value or -1 on error
 */
int libregf_key_get_class_name_size(
     libregf_key_t *key,
     size_t *class_name_size,
     libcerror_error_t **error )
{
	libregf_internal_key_t *internal_key = NULL;
	libregf_key_item_t *key_item         = NULL;
	static char *function                = "libregf_key_get_class_name_size";

	if( key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key.",
		 function );

		return( -1 );
	}
	internal_key = (libregf_internal_key_t *) key;

	if( class_name_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid class name size.",
		 function );

		return( -1 );
	}
	if( libfdata_tree_node_get_node_value(
	     internal_key->key_tree_node,
	     (intptr_t *) internal_key->file_io_handle,
	     internal_key->key_cache,
	     (intptr_t **) &key_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve key item.",
		 function );

		return( -1 );
	}
	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing key item.",
		 function );

		return( -1 );
	}
	if( key_item->class_name == NULL )
	{
		return( 0 );
	}
	*class_name_size = key_item->class_name_size;

	return( 1 );
}

/* Retrieves the class name
 * Returns 1 if successful, 0 if no such value or -1 on error
 */
int libregf_key_get_class_name(
     libregf_key_t *key,
     uint8_t *class_name,
     size_t class_name_size,
     libcerror_error_t **error )
{
	libregf_internal_key_t *internal_key = NULL;
	libregf_key_item_t *key_item         = NULL;
	static char *function                = "libregf_key_get_class_name";

	if( key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key.",
		 function );

		return( -1 );
	}
	internal_key = (libregf_internal_key_t *) key;

	if( class_name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid class name.",
		 function );

		return( -1 );
	}
	if( class_name_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid class name size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( libfdata_tree_node_get_node_value(
	     internal_key->key_tree_node,
	     (intptr_t *) internal_key->file_io_handle,
	     internal_key->key_cache,
	     (intptr_t **) &key_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve key item.",
		 function );

		return( -1 );
	}
	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing key item.",
		 function );

		return( -1 );
	}
	if( key_item->class_name == NULL )
	{
		return( 0 );
	}
	if( class_name_size < key_item->class_name_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid class name size value out of bounds.",
		 function );

		return( -1 );
	}
	if( memory_copy(
	     class_name,
	     key_item->class_name,
	     key_item->class_name_size ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
		 "%s: unable to copy class name.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the UTF-8 string size of the class name
 * The returned size includes the end of string character
 * Returns 1 if successful, 0 if no such value or -1 on error
 */
int libregf_key_get_utf8_class_name_size(
     libregf_key_t *key,
     size_t *utf8_class_name_size,
     libcerror_error_t **error )
{
	libregf_internal_key_t *internal_key = NULL;
	libregf_key_item_t *key_item         = NULL;
	static char *function                = "libregf_key_get_utf8_class_name_size";
	int result                           = 0;

	if( key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key.",
		 function );

		return( -1 );
	}
	internal_key = (libregf_internal_key_t *) key;

	if( libfdata_tree_node_get_node_value(
	     internal_key->key_tree_node,
	     (intptr_t *) internal_key->file_io_handle,
	     internal_key->key_cache,
	     (intptr_t **) &key_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve key item.",
		 function );

		return( -1 );
	}
	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing key item.",
		 function );

		return( -1 );
	}
	if( key_item->class_name != NULL )
	{
		result = libuna_utf8_string_size_from_utf16_stream(
			  key_item->name,
			  (size_t) key_item->name_size,
			  LIBUNA_ENDIAN_LITTLE,
			  utf8_class_name_size,
			  error );

		if( result != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve UTF-8 string size.",
			 function );

			return( -1 );
		}
	}
	return( result );
}

/* Retrieves the UTF-8 string value of the class name
 * The function uses a codepage if necessary, it uses the codepage set for the library
 * The size should include the end of string character
 * Returns 1 if successful, 0 if no such value or -1 on error
 */
int libregf_key_get_utf8_class_name(
     libregf_key_t *key,
     uint8_t *utf8_class_name,
     size_t utf8_class_name_size,
     libcerror_error_t **error )
{
	libregf_internal_key_t *internal_key = NULL;
	libregf_key_item_t *key_item         = NULL;
	static char *function                = "libregf_key_get_utf8_class_name";
	int result                           = 0;

	if( key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key.",
		 function );

		return( -1 );
	}
	internal_key = (libregf_internal_key_t *) key;

	if( libfdata_tree_node_get_node_value(
	     internal_key->key_tree_node,
	     (intptr_t *) internal_key->file_io_handle,
	     internal_key->key_cache,
	     (intptr_t **) &key_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve key item.",
		 function );

		return( -1 );
	}
	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing key item.",
		 function );

		return( -1 );
	}
	if( key_item->class_name != NULL )
	{
		result = libuna_utf8_string_copy_from_utf16_stream(
			  utf8_class_name,
			  utf8_class_name_size,
			  key_item->name,
			  (size_t) key_item->name_size,
			  LIBUNA_ENDIAN_LITTLE,
			  error );

		if( result != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve UTF-8 string.",
			 function );

			return( -1 );
		}
	}
	return( result );
}

/* Retrieves the UTF-16 string size of the class name
 * The returned size includes the end of string character
 * Returns 1 if successful, 0 if no such value or -1 on error
 */
int libregf_key_get_utf16_class_name_size(
     libregf_key_t *key,
     size_t *utf16_class_name_size,
     libcerror_error_t **error )
{
	libregf_internal_key_t *internal_key = NULL;
	libregf_key_item_t *key_item         = NULL;
	static char *function                = "libregf_key_get_utf16_class_name_size";
	int result                           = 0;

	if( key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key.",
		 function );

		return( -1 );
	}
	internal_key = (libregf_internal_key_t *) key;

	if( libfdata_tree_node_get_node_value(
	     internal_key->key_tree_node,
	     (intptr_t *) internal_key->file_io_handle,
	     internal_key->key_cache,
	     (intptr_t **) &key_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve key item.",
		 function );

		return( -1 );
	}
	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing key item.",
		 function );

		return( -1 );
	}
	if( key_item->class_name != NULL )
	{
		result = libuna_utf16_string_size_from_utf16_stream(
			  key_item->name,
			  (size_t) key_item->name_size,
			  LIBUNA_ENDIAN_LITTLE,
			  utf16_class_name_size,
			  error );

		if( result != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve UTF-16 string size.",
			 function );

			return( -1 );
		}
	}
	return( result );
}

/* Retrieves the UTF-16 string value of the class name
 * The function uses a codepage if necessary, it uses the codepage set for the library
 * The size should include the end of string character
 * Returns 1 if successful, 0 if no such value or -1 on error
 */
int libregf_key_get_utf16_class_name(
     libregf_key_t *key,
     uint16_t *utf16_class_name,
     size_t utf16_class_name_size,
     libcerror_error_t **error )
{
	libregf_internal_key_t *internal_key = NULL;
	libregf_key_item_t *key_item         = NULL;
	static char *function                = "libregf_value_get_utf16_class_name";
	int result                           = 0;

	if( key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key.",
		 function );

		return( -1 );
	}
	internal_key = (libregf_internal_key_t *) key;

	if( libfdata_tree_node_get_node_value(
	     internal_key->key_tree_node,
	     (intptr_t *) internal_key->file_io_handle,
	     internal_key->key_cache,
	     (intptr_t **) &key_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve key item.",
		 function );

		return( -1 );
	}
	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing key item.",
		 function );

		return( -1 );
	}
	if( key_item->class_name != NULL )
	{
		result = libuna_utf16_string_copy_from_utf16_stream(
			  utf16_class_name,
			  utf16_class_name_size,
			  key_item->name,
			  (size_t) key_item->name_size,
			  LIBUNA_ENDIAN_LITTLE,
			  error );

		if( result != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve UTF-16 string.",
			 function );

			return( -1 );
		}
	}
	return( result );
}

/* Retrieves the last written date and time
 * The returned time is a 64-bit version of a filetime value
 * Returns 1 if successful or -1 on error
 */
int libregf_key_get_last_written_time(
     libregf_key_t *key,
     uint64_t *last_written_time,
     libcerror_error_t **error )
{
	libregf_internal_key_t *internal_key = NULL;
	libregf_key_item_t *key_item         = NULL;
	static char *function                = "libregf_key_get_last_written_time";

	if( key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key.",
		 function );

		return( -1 );
	}
	internal_key = (libregf_internal_key_t *) key;

	if( libfdata_tree_node_get_node_value(
	     internal_key->key_tree_node,
	     (intptr_t *) internal_key->file_io_handle,
	     internal_key->key_cache,
	     (intptr_t **) &key_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve key item.",
		 function );

		return( -1 );
	}
	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing key item.",
		 function );

		return( -1 );
	}
	if( last_written_time == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid last written time.",
		 function );

		return( -1 );
	}
	*last_written_time = key_item->last_written_time;

	return( 1 );
}

/* Retrieves the security descriptor size
 * Returns 1 if successful, 0 if no such value or -1 on error
 */
int libregf_key_get_security_descriptor_size(
     libregf_key_t *key,
     size_t *security_descriptor_size,
     libcerror_error_t **error )
{
	libregf_internal_key_t *internal_key = NULL;
	libregf_key_item_t *key_item         = NULL;
	static char *function                = "libregf_key_get_security_descriptor_size";

	if( key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key.",
		 function );

		return( -1 );
	}
	internal_key = (libregf_internal_key_t *) key;

	if( security_descriptor_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid security descriptor size.",
		 function );

		return( -1 );
	}
	if( libfdata_tree_node_get_node_value(
	     internal_key->key_tree_node,
	     (intptr_t *) internal_key->file_io_handle,
	     internal_key->key_cache,
	     (intptr_t **) &key_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve key item.",
		 function );

		return( -1 );
	}
	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing key item.",
		 function );

		return( -1 );
	}
	if( ( key_item->security_descriptor == NULL )
	 || ( key_item->security_descriptor_size == 0 ) )
	{
		return( 0 );
	}
	*security_descriptor_size = key_item->security_descriptor_size;

	return( 1 );
}

/* Retrieves the security descriptor
 * Returns 1 if successful, 0 if no such value or -1 on error
 */
int libregf_key_get_security_descriptor(
     libregf_key_t *key,
     uint8_t *security_descriptor,
     size_t security_descriptor_size,
     libcerror_error_t **error )
{
	libregf_internal_key_t *internal_key = NULL;
	libregf_key_item_t *key_item         = NULL;
	static char *function                = "libregf_key_get_security_descriptor";

	if( key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key.",
		 function );

		return( -1 );
	}
	internal_key = (libregf_internal_key_t *) key;

	if( security_descriptor == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid security descriptor.",
		 function );

		return( -1 );
	}
	if( security_descriptor_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid security descriptor size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( libfdata_tree_node_get_node_value(
	     internal_key->key_tree_node,
	     (intptr_t *) internal_key->file_io_handle,
	     internal_key->key_cache,
	     (intptr_t **) &key_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve key item.",
		 function );

		return( -1 );
	}
	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing key item.",
		 function );

		return( -1 );
	}
	if( ( key_item->security_descriptor == NULL )
	 || ( key_item->security_descriptor_size == 0 ) )
	{
		return( 0 );
	}
	if( security_descriptor_size < key_item->security_descriptor_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid security descriptor size value out of bounds.",
		 function );

		return( -1 );
	}
	if( memory_copy(
	     security_descriptor,
	     key_item->security_descriptor,
	     key_item->security_descriptor_size ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
		 "%s: unable to copy security descriptor.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the number of values of the referenced key
 * All sets in a key contain the same number of values
 * Returns 1 if successful or -1 on error
 */
int libregf_key_get_number_of_values(
     libregf_key_t *key,
     int *number_of_values,
     libcerror_error_t **error )
{
	libregf_internal_key_t *internal_key = NULL;
	libregf_key_item_t *key_item         = NULL;
	static char *function                = "libregf_key_get_number_of_values";

	if( key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key.",
		 function );

		return( -1 );
	}
	internal_key = (libregf_internal_key_t *) key;

	if( libfdata_tree_node_get_node_value(
	     internal_key->key_tree_node,
	     (intptr_t *) internal_key->file_io_handle,
	     internal_key->key_cache,
	     (intptr_t **) &key_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve key item.",
		 function );

		return( -1 );
	}
	if( libregf_key_item_get_number_of_values(
	     key_item,
	     number_of_values,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of values.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the value
 * Returns 1 if successful or -1 on error
 */
int libregf_key_get_value(
     libregf_key_t *key,
     int value_index,
     libregf_value_t **value,
     libcerror_error_t **error )
{
	libfdata_list_element_t *values_list_element = NULL;
	libregf_internal_key_t *internal_key         = NULL;
	libregf_key_item_t *key_item                 = NULL;
	static char *function                        = "libregf_key_get_value";

	if( key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key.",
		 function );

		return( -1 );
	}
	internal_key = (libregf_internal_key_t *) key;

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
	if( *value != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: value already set.",
		 function );

		return( -1 );
	}
	if( libfdata_tree_node_get_node_value(
	     internal_key->key_tree_node,
	     (intptr_t *) internal_key->file_io_handle,
	     internal_key->key_cache,
	     (intptr_t **) &key_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve key item.",
		 function );

		return( -1 );
	}
	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing key item.",
		 function );

		return( -1 );
	}
	if( libfdata_list_get_list_element_by_index(
	     key_item->values_list,
	     value_index,
	     &values_list_element,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value: %d.",
		 function,
		 value_index );

		return( -1 );
	}
	if( libregf_value_initialize(
	     value,
	     internal_key->io_handle,
	     internal_key->file_io_handle,
	     values_list_element,
	     key_item->values_cache,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to initialize value.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the value for the specific UTF-8 encoded name
 * To retrieve the default value specify value name as NULL
 * and its length as 0
 * Returns 1 if successful, 0 if no such value or -1 on error
 */
int libregf_key_get_value_by_utf8_name(
     libregf_key_t *key,
     const uint8_t *utf8_string,
     size_t utf8_string_length,
     libregf_value_t **value,
     libcerror_error_t **error )
{
	libfdata_list_element_t *values_list_element = NULL;
	libregf_internal_key_t *internal_key         = NULL;
	libregf_key_item_t *key_item                 = NULL;
	libregf_value_item_t *value_item             = NULL;
	static char *function                        = "libregf_key_get_value_by_utf8_name";
	libuna_unicode_character_t unicode_character = 0;
	size_t utf8_string_index                     = 0;
	uint32_t name_hash                           = 0;
	int number_of_values                         = 0;
	int result                                   = 0;
	int value_index                              = 0;

	if( key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key.",
		 function );

		return( -1 );
	}
	internal_key = (libregf_internal_key_t *) key;

	if( internal_key->io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid key - missing IO handle.",
		 function );

		return( -1 );
	}
	if( ( utf8_string == NULL )
	 && ( utf8_string_length != 0 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-8 string.",
		 function );

		return( -1 );
	}
	if( utf8_string_length > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid UTF-8 string length value exceeds maximum.",
		 function );

		return( -1 );
	}
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
	if( *value != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: value already set.",
		 function );

		return( -1 );
	}
	if( libfdata_tree_node_get_node_value(
	     internal_key->key_tree_node,
	     (intptr_t *) internal_key->file_io_handle,
	     internal_key->key_cache,
	     (intptr_t **) &key_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve key item.",
		 function );

		return( -1 );
	}
	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing key item.",
		 function );

		return( -1 );
	}
	if( libregf_key_item_get_number_of_values(
	     key_item,
	     &number_of_values,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of values.",
		 function );

		return( -1 );
	}
	if( number_of_values == 0 )
	{
		return( 0 );
	}
	while( utf8_string_index < utf8_string_length )
	{
		if( libuna_unicode_character_copy_from_utf8(
		     &unicode_character,
		     utf8_string,
		     utf8_string_length,
		     &utf8_string_index,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
			 "%s: unable to copy UTF-8 string to Unicode character.",
			 function );

			return( -1 );
		}
		name_hash *= 37;
		name_hash += (uint32_t) towupper( (wint_t) unicode_character );
	}
	for( value_index = 0;
	     value_index < number_of_values;
	     value_index++ )
	{
		if( libfdata_list_get_list_element_by_index(
		     key_item->values_list,
		     value_index,
		     &values_list_element,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve value item: %d.",
			 function,
			 value_index );

			return( -1 );
		}
		if( libfdata_list_element_get_element_value(
		     values_list_element,
		     (intptr_t *) internal_key->file_io_handle,
		     key_item->values_cache,
		     (intptr_t **) &value_item,
		     0,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve value item: %d values.",
			 function,
			 value_index );

			return( -1 );
		}
		if( value_item == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing value item: %d values.",
			 function,
			 value_index );

			return( -1 );
		}
		if( value_item->name == NULL )
		{
			if( ( utf8_string == NULL )
			 && ( utf8_string_length == 0 ) )
			{
				result = 1;
			}
		}
		else if( utf8_string == NULL )
		{
			continue;
		}
		else
		{
			result = libregf_value_item_compare_name_with_utf8_string(
				  value_item,
				  name_hash,
				  utf8_string,
				  utf8_string_length,
				  internal_key->io_handle->ascii_codepage,
				  error );

			if( result == -1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GENERIC,
				 "%s: unable to compare value name with UTF-8 string.",
				 function );

				return( -1 );
			}
		}
		if( result != 0 )
		{
			break;
		}
	}
	if( value_index >= number_of_values )
	{
		return( 0 );
	}
	if( libregf_value_initialize(
	     value,
	     internal_key->io_handle,
	     internal_key->file_io_handle,
	     values_list_element,
	     key_item->values_cache,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to initialize value.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the value for the specific UTF-16 encoded name
 * To retrieve the default value specify string as NULL
 * and its length as 0
 * Returns 1 if successful, 0 if no such value or -1 on error
 */
int libregf_key_get_value_by_utf16_name(
     libregf_key_t *key,
     const uint16_t *utf16_string,
     size_t utf16_string_length,
     libregf_value_t **value,
     libcerror_error_t **error )
{
	libfdata_list_element_t *values_list_element = NULL;
	libregf_internal_key_t *internal_key         = NULL;
	libregf_key_item_t *key_item                 = NULL;
	libregf_value_item_t *value_item             = NULL;
	static char *function                        = "libregf_key_get_value_by_utf16_name";
	libuna_unicode_character_t unicode_character = 0;
	size_t utf16_string_index                    = 0;
	uint32_t name_hash                           = 0;
	int number_of_values                         = 0;
	int result                                   = 0;
	int value_index                              = 0;

	if( key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key.",
		 function );

		return( -1 );
	}
	internal_key = (libregf_internal_key_t *) key;

	if( internal_key->io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid key - missing IO handle.",
		 function );

		return( -1 );
	}
	if( ( utf16_string == NULL )
	 && ( utf16_string_length != 0 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-16 string.",
		 function );

		return( -1 );
	}
	if( utf16_string_length > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid UTF-16 string length value exceeds maximum.",
		 function );

		return( -1 );
	}
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
	if( *value != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: value already set.",
		 function );

		return( -1 );
	}
	if( libfdata_tree_node_get_node_value(
	     internal_key->key_tree_node,
	     (intptr_t *) internal_key->file_io_handle,
	     internal_key->key_cache,
	     (intptr_t **) &key_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve key item.",
		 function );

		return( -1 );
	}
	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing key item.",
		 function );

		return( -1 );
	}
	if( libregf_key_item_get_number_of_values(
	     key_item,
	     &number_of_values,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of values.",
		 function );

		return( -1 );
	}
	if( number_of_values == 0 )
	{
		return( 0 );
	}
	while( utf16_string_index < utf16_string_length )
	{
		if( libuna_unicode_character_copy_from_utf16(
		     &unicode_character,
		     utf16_string,
		     utf16_string_length,
		     &utf16_string_index,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
			 "%s: unable to copy UTF-16 string to Unicode character.",
			 function );

			return( -1 );
		}
		name_hash *= 37;
		name_hash += (uint32_t) towupper( (wint_t) unicode_character );
	}
	for( value_index = 0;
	     value_index < number_of_values;
	     value_index++ )
	{
		if( libfdata_list_get_list_element_by_index(
		     key_item->values_list,
		     value_index,
		     &values_list_element,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve value item: %d.",
			 function,
			 value_index );

			return( -1 );
		}
		if( libfdata_list_element_get_element_value(
		     values_list_element,
		     (intptr_t *) internal_key->file_io_handle,
		     key_item->values_cache,
		     (intptr_t **) &value_item,
		     0,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve value item: %d values.",
			 function,
			 value_index );

			return( -1 );
		}
		if( value_item == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing value item: %d values.",
			 function,
			 value_index );

			return( -1 );
		}
		if( value_item->name == NULL )
		{
			if( ( utf16_string == NULL )
			 && ( utf16_string_length == 0 ) )
			{
				result = 1;
			}
		}
		else if( utf16_string == NULL )
		{
			continue;
		}
		else
		{
			result = libregf_value_item_compare_name_with_utf16_string(
				  value_item,
				  name_hash,
				  utf16_string,
				  utf16_string_length,
				  internal_key->io_handle->ascii_codepage,
				  error );

			if( result == -1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GENERIC,
				 "%s: unable to compare value name with UTF-16 string.",
				 function );

				return( -1 );
			}
		}
		if( result != 0 )
		{
			break;
		}
	}
	if( value_index >= number_of_values )
	{
		return( 0 );
	}
	if( libregf_value_initialize(
	     value,
	     internal_key->io_handle,
	     internal_key->file_io_handle,
	     values_list_element,
	     key_item->values_cache,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to initialize value.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the number of sub keys
 * Returns 1 if successful or -1 on error
 */
int libregf_key_get_number_of_sub_keys(
     libregf_key_t *key,
     int *number_of_sub_keys,
     libcerror_error_t **error )
{
	libregf_internal_key_t *internal_key = NULL;
	static char *function                = "libregf_key_get_number_of_sub_keys";

	if( key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key.",
		 function );

		return( -1 );
	}
	internal_key = (libregf_internal_key_t *) key;

	if( libfdata_tree_node_get_number_of_sub_nodes(
	     internal_key->key_tree_node,
	     (intptr_t *) internal_key->file_io_handle,
	     internal_key->key_cache,
	     number_of_sub_keys,
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
	return( 1 );
}

/* Retrieves the sub key for the specific index
 * Returns 1 if successful or -1 on error
 */
int libregf_key_get_sub_key(
     libregf_key_t *key,
     int sub_key_index,
     libregf_key_t **sub_key,
     libcerror_error_t **error )
{
	libfdata_tree_node_t *key_tree_sub_node = NULL;
	libregf_internal_key_t *internal_key    = NULL;
	static char *function                   = "libregf_key_get_sub_key";

	if( key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key.",
		 function );

		return( -1 );
	}
	internal_key = (libregf_internal_key_t *) key;

	if( sub_key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid sub key.",
		 function );

		return( -1 );
	}
	if( *sub_key != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: sub key already set.",
		 function );

		return( -1 );
	}
	if( libfdata_tree_node_get_sub_node_by_index(
	     internal_key->key_tree_node,
	     (intptr_t *) internal_key->file_io_handle,
	     internal_key->key_cache,
             sub_key_index,
	     &key_tree_sub_node,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve sub node: %d.",
		 function,
		 sub_key_index );

		return( -1 );
	}
	if( key_tree_sub_node == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid key tree sub node.",
		 function );

		return( -1 );
	}
	if( libregf_key_initialize(
	     sub_key,
	     internal_key->io_handle,
	     internal_key->file_io_handle,
	     key_tree_sub_node,
	     internal_key->key_cache,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to initialize sub key.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the sub key for the specific UTF-8 encoded name
 * Returns 1 if successful, 0 if no such sub key or -1 on error
 */
int libregf_key_get_sub_key_by_utf8_name(
     libregf_key_t *key,
     const uint8_t *utf8_string,
     size_t utf8_string_length,
     libregf_key_t **sub_key,
     libcerror_error_t **error )
{
	libfdata_tree_node_t *key_tree_sub_node      = NULL;
	libregf_internal_key_t *internal_key         = NULL;
	libregf_key_item_t *sub_key_item             = NULL;
	static char *function                        = "libregf_key_get_sub_key_by_utf8_name";
	libuna_unicode_character_t unicode_character = 0;
	size_t utf8_string_index                     = 0;
	uint32_t name_hash                           = 0;
	int result                                   = 0;

	if( key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key.",
		 function );

		return( -1 );
	}
	internal_key = (libregf_internal_key_t *) key;

	if( internal_key->io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid key - missing IO handle.",
		 function );

		return( -1 );
	}
	if( utf8_string == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-8 string.",
		 function );

		return( -1 );
	}
	if( utf8_string_length > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid UTF-8 string length value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( sub_key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid sub key.",
		 function );

		return( -1 );
	}
	if( *sub_key != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: sub key already set.",
		 function );

		return( -1 );
	}
	while( utf8_string_index < utf8_string_length )
	{
		if( libuna_unicode_character_copy_from_utf8(
		     &unicode_character,
		     utf8_string,
		     utf8_string_length,
		     &utf8_string_index,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
			 "%s: unable to copy UTF-8 string to Unicode character.",
			 function );

			return( -1 );
		}
		name_hash *= 37;
		name_hash += (uint32_t) towupper( (wint_t) unicode_character );
	}
	result = libregf_key_tree_get_sub_key_values_by_utf8_name(
	          internal_key->key_tree_node,
	          internal_key->file_io_handle,
	          internal_key->key_cache,
	          name_hash,
	          utf8_string,
	          utf8_string_length,
	          internal_key->io_handle->ascii_codepage,
	          &key_tree_sub_node,
	          &sub_key_item,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve sub key values by UTF-8 name.",
		 function );

		return( -1 );
	}
	else if( result != 0 )
	{
		if( libregf_key_initialize(
		     sub_key,
		     internal_key->io_handle,
		     internal_key->file_io_handle,
		     key_tree_sub_node,
		     internal_key->key_cache,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to initialize sub key.",
			 function );

			return( -1 );
		}
	}
	return( result );
}

/* Retrieves the sub key for the specific UTF-8 encoded path
 * The path separator is the \ character
 * Returns 1 if successful, 0 if no such key or -1 on error
 */
int libregf_key_get_sub_key_by_utf8_path(
     libregf_key_t *key,
     const uint8_t *utf8_string,
     size_t utf8_string_length,
     libregf_key_t **sub_key,
     libcerror_error_t **error )
{
	libfdata_tree_node_t *key_tree_node          = NULL;
	libfdata_tree_node_t *key_tree_sub_node      = NULL;
	libregf_internal_key_t *internal_key         = NULL;
	libregf_key_item_t *key_item                 = NULL;
	libregf_key_item_t *sub_key_item             = NULL;
	uint8_t *utf8_string_segment                 = NULL;
	static char *function                        = "libregf_key_get_sub_key_by_utf8_path";
	libuna_unicode_character_t unicode_character = 0;
	size_t utf8_string_index                     = 0;
	size_t utf8_string_segment_length            = 0;
	uint32_t name_hash                           = 0;
	int result                                   = 0;

	if( key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key.",
		 function );

		return( -1 );
	}
	internal_key = (libregf_internal_key_t *) key;

	if( internal_key->io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid key - missing IO handle.",
		 function );

		return( -1 );
	}
	if( utf8_string == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-8 string.",
		 function );

		return( -1 );
	}
	if( utf8_string_length > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid UTF-8 string length value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( sub_key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid sub key.",
		 function );

		return( -1 );
	}
	if( *sub_key != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: sub key already set.",
		 function );

		return( -1 );
	}
	if( utf8_string_length > 0 )
	{
		/* Ignore a leading separator
		 */
		if( utf8_string[ utf8_string_index ] == (uint8_t) LIBREGF_SEPARATOR )
		{
			utf8_string_index++;
		}
	}
	key_tree_node = internal_key->key_tree_node;

	if( libfdata_tree_node_get_node_value(
	     key_tree_node,
	     (intptr_t *) internal_key->file_io_handle,
	     internal_key->key_cache,
	     (intptr_t **) &key_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve key item.",
		 function );

		return( -1 );
	}
	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing key item.",
		 function );

		return( -1 );
	}
	/* If the string is empty return the current key
	 */
	if( utf8_string_length == utf8_string_index )
	{
		result = 1;
	}
	else while( utf8_string_index < utf8_string_length )
	{
		utf8_string_segment        = (uint8_t *) &( utf8_string[ utf8_string_index ] );
		utf8_string_segment_length = utf8_string_index;
		name_hash                  = 0;

		while( utf8_string_index < utf8_string_length )
		{
			if( libuna_unicode_character_copy_from_utf8(
			     &unicode_character,
			     utf8_string,
			     utf8_string_length,
			     &utf8_string_index,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy UTF-8 string to Unicode character.",
				 function );

				return( -1 );
			}
			if( ( unicode_character == (libuna_unicode_character_t) LIBREGF_SEPARATOR )
			 || ( unicode_character == 0 ) )
			{
				utf8_string_segment_length += 1;

				break;
			}
			name_hash *= 37;
			name_hash += (uint32_t) towupper( (wint_t) unicode_character );
		}
		utf8_string_segment_length = utf8_string_index - utf8_string_segment_length;

		if( utf8_string_segment_length == 0 )
		{
			result = 0;
		}
		else
		{
			result = libregf_key_tree_get_sub_key_values_by_utf8_name(
				  key_tree_node,
				  internal_key->file_io_handle,
				  internal_key->key_cache,
				  name_hash,
				  utf8_string_segment,
				  utf8_string_segment_length,
				  internal_key->io_handle->ascii_codepage,
				  &key_tree_sub_node,
				  &sub_key_item,
				  error );
		}
		if( result == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve sub key values by name.",
			 function );

			return( -1 );
		}
		else if( result == 0 )
		{
			break;
		}
		key_tree_node = key_tree_sub_node;
	}
	if( result != 0 )
	{
		if( libregf_key_initialize(
		     sub_key,
		     internal_key->io_handle,
		     internal_key->file_io_handle,
		     key_tree_node,
		     internal_key->key_cache,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create key.",
			 function );

			return( -1 );
		}
	}
	return( result );
}

/* Retrieves the sub key for the specific UTF-16 encoded name
 * Returns 1 if successful, 0 if no such sub key or -1 on error
 */
int libregf_key_get_sub_key_by_utf16_name(
     libregf_key_t *key,
     const uint16_t *utf16_string,
     size_t utf16_string_length,
     libregf_key_t **sub_key,
     libcerror_error_t **error )
{
	libfdata_tree_node_t *key_tree_sub_node      = NULL;
	libregf_internal_key_t *internal_key         = NULL;
	libregf_key_item_t *sub_key_item             = NULL;
	static char *function                        = "libregf_key_get_value_by_utf16_name";
	libuna_unicode_character_t unicode_character = 0;
	size_t utf16_string_index                    = 0;
	uint32_t name_hash                           = 0;
	int result                                   = 0;

	if( key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key.",
		 function );

		return( -1 );
	}
	internal_key = (libregf_internal_key_t *) key;

	if( internal_key->io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid key - missing IO handle.",
		 function );

		return( -1 );
	}
	if( utf16_string == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-16 string.",
		 function );

		return( -1 );
	}
	if( utf16_string_length > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid UTF-16 string length value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( sub_key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid sub key.",
		 function );

		return( -1 );
	}
	if( *sub_key != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: sub key already set.",
		 function );

		return( -1 );
	}
	while( utf16_string_index < utf16_string_length )
	{
		if( libuna_unicode_character_copy_from_utf16(
		     &unicode_character,
		     utf16_string,
		     utf16_string_length,
		     &utf16_string_index,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
			 "%s: unable to copy UTF-16 string to Unicode character.",
			 function );

			return( -1 );
		}
		name_hash *= 37;
		name_hash += (uint32_t) towupper( (wint_t) unicode_character );
	}
	result = libregf_key_tree_get_sub_key_values_by_utf16_name(
	          internal_key->key_tree_node,
	          internal_key->file_io_handle,
	          internal_key->key_cache,
	          name_hash,
	          utf16_string,
	          utf16_string_length,
	          internal_key->io_handle->ascii_codepage,
	          &key_tree_sub_node,
	          &sub_key_item,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve sub key values by UTF-16 name.",
		 function );

		return( -1 );
	}
	else if( result != 0 )
	{
		if( libregf_key_initialize(
		     sub_key,
		     internal_key->io_handle,
		     internal_key->file_io_handle,
		     key_tree_sub_node,
		     internal_key->key_cache,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to initialize sub key.",
			 function );

			return( -1 );
		}
	}
	return( result );
}

/* Retrieves the sub key for the specific UTF-16 encoded path
 * The path separator is the \ character
 * Returns 1 if successful, 0 if no such key or -1 on error
 */
int libregf_key_get_sub_key_by_utf16_path(
     libregf_key_t *key,
     const uint16_t *utf16_string,
     size_t utf16_string_length,
     libregf_key_t **sub_key,
     libcerror_error_t **error )
{
	libfdata_tree_node_t *key_tree_node          = NULL;
	libfdata_tree_node_t *key_tree_sub_node      = NULL;
	libregf_internal_key_t *internal_key         = NULL;
	libregf_key_item_t *key_item                 = NULL;
	libregf_key_item_t *sub_key_item             = NULL;
	uint16_t *utf16_string_segment               = NULL;
	static char *function                        = "libregf_key_get_sub_key_by_utf16_path";
	libuna_unicode_character_t unicode_character = 0;
	size_t utf16_string_index                    = 0;
	size_t utf16_string_segment_length           = 0;
	uint32_t name_hash                           = 0;
	int result                                   = 0;

	if( key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key.",
		 function );

		return( -1 );
	}
	internal_key = (libregf_internal_key_t *) key;

	if( internal_key->io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid key - missing IO handle.",
		 function );

		return( -1 );
	}
	if( utf16_string == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-16 string.",
		 function );

		return( -1 );
	}
	if( utf16_string_length > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid UTF-16 string length value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( sub_key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid sub key.",
		 function );

		return( -1 );
	}
	if( *sub_key != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: sub key already set.",
		 function );

		return( -1 );
	}
	if( utf16_string_length > 0 )
	{
		/* Ignore a leading separator
		 */
		if( utf16_string[ utf16_string_index ] == (uint16_t) LIBREGF_SEPARATOR )
		{
			utf16_string_index++;
		}
	}
	key_tree_node = internal_key->key_tree_node;

	if( libfdata_tree_node_get_node_value(
	     key_tree_node,
	     (intptr_t *) internal_key->file_io_handle,
	     internal_key->key_cache,
	     (intptr_t **) &key_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve key item.",
		 function );

		return( -1 );
	}
	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing key item.",
		 function );

		return( -1 );
	}
	/* If the string is empty return the current key
	 */
	if( utf16_string_length == utf16_string_index )
	{
		result = 1;
	}
	else while( utf16_string_index < utf16_string_length )
	{
		utf16_string_segment        = (uint16_t *) &( utf16_string[ utf16_string_index ] );
		utf16_string_segment_length = utf16_string_index;
		name_hash                   = 0;

		while( utf16_string_index < utf16_string_length )
		{
			if( libuna_unicode_character_copy_from_utf16(
			     &unicode_character,
			     utf16_string,
			     utf16_string_length,
			     &utf16_string_index,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy UTF-16 string to Unicode character.",
				 function );

				return( -1 );
			}
			if( ( unicode_character == (libuna_unicode_character_t) LIBREGF_SEPARATOR )
			 || ( unicode_character == 0 ) )
			{
				utf16_string_segment_length += 1;

				break;
			}
			name_hash *= 37;
			name_hash += (uint32_t) towupper( (wint_t) unicode_character );
		}
		utf16_string_segment_length = utf16_string_index - utf16_string_segment_length;

		if( utf16_string_segment_length == 0 )
		{
			result = 0;
		}
		else
		{
			result = libregf_key_tree_get_sub_key_values_by_utf16_name(
				  key_tree_node,
				  internal_key->file_io_handle,
				  internal_key->key_cache,
				  name_hash,
				  utf16_string_segment,
				  utf16_string_segment_length,
				  internal_key->io_handle->ascii_codepage,
				  &key_tree_sub_node,
				  &sub_key_item,
				  error );
		}
		if( result == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve sub key values by name.",
			 function );

			return( -1 );
		}
		else if( result == 0 )
		{
			break;
		}
		key_tree_node = key_tree_sub_node;
	}
	if( result != 0 )
	{
		if( libregf_key_initialize(
		     sub_key,
		     internal_key->io_handle,
		     internal_key->file_io_handle,
		     key_tree_node,
		     internal_key->key_cache,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create key.",
			 function );

			return( -1 );
		}
	}
	return( result );
}

