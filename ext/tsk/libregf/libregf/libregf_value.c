/*
 * Value functions
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

#include "libregf_definitions.h"
#include "libregf_io_handle.h"
#include "libregf_libbfio.h"
#include "libregf_libcerror.h"
#include "libregf_libfcache.h"
#include "libregf_libfdata.h"
#include "libregf_libuna.h"
#include "libregf_value.h"
#include "libregf_value_item.h"
#include "libregf_value_type.h"

/* Creates a value
 * Make sure the value value is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libregf_value_initialize(
     libregf_value_t **value,
     libregf_io_handle_t *io_handle,
     libbfio_handle_t *file_io_handle,
     libfdata_list_element_t *values_list_element,
     libfcache_cache_t *values_cache,
     libcerror_error_t **error )
{
	libregf_internal_value_t *internal_value = NULL;
	static char *function                    = "libregf_value_initialize";

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
		 "%s: invalid value value already set.",
		 function );

		return( -1 );
	}
	if( values_list_element == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid values list element.",
		 function );

		return( -1 );
	}
	internal_value = memory_allocate_structure(
	                  libregf_internal_value_t );

	if( internal_value == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create internal value.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_value,
	     0,
	     sizeof( libregf_internal_value_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear internal value.",
		 function );

		memory_free(
		 internal_value );

		return( -1 );
	}
	internal_value->file_io_handle      = file_io_handle;
	internal_value->io_handle           = io_handle;
	internal_value->values_list_element = values_list_element;
	internal_value->values_cache        = values_cache;

	*value = (libregf_value_t *) internal_value;

	return( 1 );

on_error:
	if( internal_value != NULL )
	{
		memory_free(
		 internal_value );
	}
	return( -1 );
}

/* Frees a value
 * Returns 1 if successful or -1 on error
 */
int libregf_value_free(
     libregf_value_t **value,
     libcerror_error_t **error )
{
	libregf_internal_value_t *internal_value = NULL;
	static char *function                    = "libregf_value_free";

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
		internal_value = (libregf_internal_value_t *) *value;
		*value         = NULL;

		/* The io_handle, file_io_handle and values_list_element references are freed elsewhere
		 */
		memory_free(
		 internal_value );
	}
	return( 1 );
}

/* Determine if the value corrupted
 * Returns 1 if corrupted, 0 if not or -1 on error
 */
int libregf_value_is_corrupted(
     libregf_value_t *value,
     libcerror_error_t **error )
{
	libregf_internal_value_t *internal_value = NULL;
	libregf_value_item_t *value_item         = NULL;
	static char *function                    = "libregf_value_is_corrupted";

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
	internal_value = (libregf_internal_value_t *) value;

	if( libfdata_list_element_get_element_value(
	     internal_value->values_list_element,
	     (intptr_t *) internal_value->file_io_handle,
	     internal_value->values_cache,
	     (intptr_t **) &value_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value item.",
		 function );

		return( -1 );
	}
	if( value_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing value item.",
		 function );

		return( -1 );
	}
	if( ( value_item->item_flags & LIBREGF_VALUE_ITEM_FLAG_IS_CORRUPTED ) != 0 )
	{
		return( 1 );
	}
	return( 0 );
}

/* Retrieves the offset of the value
 * Returns 1 if successful or -1 on error
 */
int libregf_value_get_offset(
     libregf_value_t *value,
     off64_t *offset,
     libcerror_error_t **error )
{
	libregf_internal_value_t *internal_value = NULL;
	static char *function                    = "libregf_value_get_offset";
	size64_t size                            = 0;
	uint32_t flags                           = 0;
	int file_index                           = 0;

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
	internal_value = (libregf_internal_value_t *) value;

	if( internal_value->io_handle == NULL )
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
	if( libfdata_list_element_get_data_range(
	     internal_value->values_list_element,
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
		 "%s: unable to retrieve value data range.",
		 function );

		return( -1 );
	}
	/* The offset is relative from the start of the hive bins list
	 * and points to the start of the corresponding hive bin cell
	 */
	*offset += internal_value->io_handle->hive_bins_list_offset + 4;

	return( 1 );
}

/* Retrieves the value name size
 * Returns 1 if successful or -1 on error
 */
int libregf_value_get_name_size(
     libregf_value_t *value,
     size_t *name_size,
     libcerror_error_t **error )
{
	libregf_internal_value_t *internal_value = NULL;
	libregf_value_item_t *value_item         = NULL;
	static char *function                    = "libregf_value_get_name_size";

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
	internal_value = (libregf_internal_value_t *) value;

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
	if( libfdata_list_element_get_element_value(
	     internal_value->values_list_element,
	     (intptr_t *) internal_value->file_io_handle,
	     internal_value->values_cache,
	     (intptr_t **) &value_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value item.",
		 function );

		return( -1 );
	}
	if( value_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing value item.",
		 function );

		return( -1 );
	}
	*name_size = value_item->name_size;

	return( 1 );
}

/* Retrieves the value name
 * Returns 1 if successful or -1 on error
 */
int libregf_value_get_name(
     libregf_value_t *value,
     uint8_t *name,
     size_t name_size,
     libcerror_error_t **error )
{
	libregf_internal_value_t *internal_value = NULL;
	libregf_value_item_t *value_item         = NULL;
	static char *function                    = "libregf_value_get_name";

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
	internal_value = (libregf_internal_value_t *) value;

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
	if( libfdata_list_element_get_element_value(
	     internal_value->values_list_element,
	     (intptr_t *) internal_value->file_io_handle,
	     internal_value->values_cache,
	     (intptr_t **) &value_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value item.",
		 function );

		return( -1 );
	}
	if( value_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing value item.",
		 function );

		return( -1 );
	}
	if( name_size < value_item->name_size )
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
	     value_item->name,
	     value_item->name_size ) == NULL )
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

/* Retrieves the UTF-8 string size of the value name
 * The returned size includes the end of string character
 * Returns 1 if successful or -1 on error
 */
int libregf_value_get_utf8_name_size(
     libregf_value_t *value,
     size_t *utf8_name_size,
     libcerror_error_t **error )
{
	libregf_internal_value_t *internal_value = NULL;
	libregf_value_item_t *value_item         = NULL;
	static char *function                    = "libregf_value_get_utf8_name_size";
	int result                               = 0;

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
	internal_value = (libregf_internal_value_t *) value;

	if( internal_value->io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid value - missing IO handle.",
		 function );

		return( -1 );
	}
	if( libfdata_list_element_get_element_value(
	     internal_value->values_list_element,
	     (intptr_t *) internal_value->file_io_handle,
	     internal_value->values_cache,
	     (intptr_t **) &value_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value item.",
		 function );

		return( -1 );
	}
	if( value_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing value item.",
		 function );

		return( -1 );
	}
	if( value_item->name == NULL )
	{
		if( utf8_name_size == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
			 "%s: invalid UTF-8 name size.",
			 function );

			return( -1 );
		}
		*utf8_name_size = 0;
	}
	else
	{
		if( ( value_item->flags & LIBREGF_VALUE_KEY_FLAG_NAME_IS_ASCII ) != 0 )
		{
			result = libuna_utf8_string_size_from_byte_stream(
				  value_item->name,
				  (size_t) value_item->name_size,
				  internal_value->io_handle->ascii_codepage,
				  utf8_name_size,
				  error );
		}
		else
		{
			result = libuna_utf8_string_size_from_utf16_stream(
				  value_item->name,
				  (size_t) value_item->name_size,
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
	}
	return( 1 );
}

/* Retrieves the UTF-8 string value of the value name
 * The function uses a codepage if necessary, it uses the codepage set for the library
 * The size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libregf_value_get_utf8_name(
     libregf_value_t *value,
     uint8_t *utf8_name,
     size_t utf8_name_size,
     libcerror_error_t **error )
{
	libregf_internal_value_t *internal_value = NULL;
	libregf_value_item_t *value_item         = NULL;
	static char *function                    = "libregf_value_get_utf8_name";
	int result                               = 0;

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
	internal_value = (libregf_internal_value_t *) value;

	if( internal_value->io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid value - missing IO handle.",
		 function );

		return( -1 );
	}
	if( libfdata_list_element_get_element_value(
	     internal_value->values_list_element,
	     (intptr_t *) internal_value->file_io_handle,
	     internal_value->values_cache,
	     (intptr_t **) &value_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value item.",
		 function );

		return( -1 );
	}
	if( value_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing value item.",
		 function );

		return( -1 );
	}
	if( value_item->name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid value item - missing name.",
		 function );

		return( -1 );
	}
	if( ( value_item->flags & LIBREGF_VALUE_KEY_FLAG_NAME_IS_ASCII ) != 0 )
	{
		result = libuna_utf8_string_copy_from_byte_stream(
			  utf8_name,
			  utf8_name_size,
			  value_item->name,
			  (size_t) value_item->name_size,
			  internal_value->io_handle->ascii_codepage,
			  error );
	}
	else
	{
		result = libuna_utf8_string_copy_from_utf16_stream(
			  utf8_name,
			  utf8_name_size,
			  value_item->name,
			  (size_t) value_item->name_size,
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

/* Retrieves the UTF-16 string size of the value name
 * The returned size includes the end of string character
 * Returns 1 if successful or -1 on error
 */
int libregf_value_get_utf16_name_size(
     libregf_value_t *value,
     size_t *utf16_name_size,
     libcerror_error_t **error )
{
	libregf_internal_value_t *internal_value = NULL;
	libregf_value_item_t *value_item         = NULL;
	static char *function                    = "libregf_value_get_utf16_name_size";
	int result                               = 0;

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
	internal_value = (libregf_internal_value_t *) value;

	if( internal_value->io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid value - missing IO handle.",
		 function );

		return( -1 );
	}
	if( libfdata_list_element_get_element_value(
	     internal_value->values_list_element,
	     (intptr_t *) internal_value->file_io_handle,
	     internal_value->values_cache,
	     (intptr_t **) &value_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value item.",
		 function );

		return( -1 );
	}
	if( value_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing value item.",
		 function );

		return( -1 );
	}
	if( value_item->name == NULL )
	{
		if( utf16_name_size == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
			 "%s: invalid UTF-16 name size.",
			 function );

			return( -1 );
		}
		*utf16_name_size = 0;
	}
	else
	{
		if( ( value_item->flags & LIBREGF_VALUE_KEY_FLAG_NAME_IS_ASCII ) != 0 )
		{
			result = libuna_utf16_string_size_from_byte_stream(
				  value_item->name,
				  (size_t) value_item->name_size,
				  internal_value->io_handle->ascii_codepage,
				  utf16_name_size,
				  error );
		}
		else
		{
			result = libuna_utf16_string_size_from_utf16_stream(
				  value_item->name,
				  (size_t) value_item->name_size,
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
	}
	return( 1 );
}

/* Retrieves the UTF-16 string value of the value name
 * The function uses a codepage if necessary, it uses the codepage set for the library
 * The size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libregf_value_get_utf16_name(
     libregf_value_t *value,
     uint16_t *utf16_name,
     size_t utf16_name_size,
     libcerror_error_t **error )
{
	libregf_internal_value_t *internal_value = NULL;
	libregf_value_item_t *value_item         = NULL;
	static char *function                    = "libregf_value_get_utf16_name";
	int result                               = 0;

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
	internal_value = (libregf_internal_value_t *) value;

	if( internal_value->io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid value - missing IO handle.",
		 function );

		return( -1 );
	}
	if( libfdata_list_element_get_element_value(
	     internal_value->values_list_element,
	     (intptr_t *) internal_value->file_io_handle,
	     internal_value->values_cache,
	     (intptr_t **) &value_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value item.",
		 function );

		return( -1 );
	}
	if( value_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing value item.",
		 function );

		return( -1 );
	}
	if( value_item->name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid value item - missing name.",
		 function );

		return( -1 );
	}
	if( ( value_item->flags & LIBREGF_VALUE_KEY_FLAG_NAME_IS_ASCII ) != 0 )
	{
		result = libuna_utf16_string_copy_from_byte_stream(
			  utf16_name,
			  utf16_name_size,
			  value_item->name,
			  (size_t) value_item->name_size,
			  internal_value->io_handle->ascii_codepage,
			  error );
	}
	else
	{
		result = libuna_utf16_string_copy_from_utf16_stream(
			  utf16_name,
			  utf16_name_size,
			  value_item->name,
			  (size_t) value_item->name_size,
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

/* Retrieves the value type
 * Returns 1 if successful or -1 on error
 */
int libregf_value_get_value_type(
     libregf_value_t *value,
     uint32_t *value_type,
     libcerror_error_t **error )
{
	libregf_internal_value_t *internal_value = NULL;
	libregf_value_item_t *value_item         = NULL;
	static char *function                    = "libregf_value_get_value_type";

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
	internal_value = (libregf_internal_value_t *) value;

	if( value_type == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value type.",
		 function );

		return( -1 );
	}
	if( libfdata_list_element_get_element_value(
	     internal_value->values_list_element,
	     (intptr_t *) internal_value->file_io_handle,
	     internal_value->values_cache,
	     (intptr_t **) &value_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value item.",
		 function );

		return( -1 );
	}
	if( value_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing value item.",
		 function );

		return( -1 );
	}
	*value_type = value_item->type;

	return( 1 );
}

/* Retrieves the value data size
 * Returns 1 if successful or -1 on error
 */
int libregf_value_get_value_data_size(
     libregf_value_t *value,
     size_t *value_data_size,
     libcerror_error_t **error )
{
	libregf_internal_value_t *internal_value = NULL;
	libregf_value_item_t *value_item         = NULL;
	static char *function                    = "libregf_value_get_value_data_size";

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
	internal_value = (libregf_internal_value_t *) value;

	if( libfdata_list_element_get_element_value(
	     internal_value->values_list_element,
	     (intptr_t *) internal_value->file_io_handle,
	     internal_value->values_cache,
	     (intptr_t **) &value_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value item.",
		 function );

		return( -1 );
	}
	if( value_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing value item.",
		 function );

		return( -1 );
	}
	if( value_item->data_type == 0 )
	{
		if( value_data_size == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
			 "%s: invalid value data size.",
			 function );

			return( -1 );
		}
		*value_data_size = 0;
	}
	else
	{
		if( libregf_value_item_get_data_size(
		     value_item,
		     value_data_size,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve value data size.",
			 function );

			return( -1 );
		}
	}
	return( 1 );
}

/* Retrieves the value data
 * Returns 1 if successful or -1 on error
 */
int libregf_value_get_value_data(
     libregf_value_t *value,
     uint8_t *value_data,
     size_t value_data_size,
     libcerror_error_t **error )
{
	libregf_internal_value_t *internal_value = NULL;
	libregf_value_item_t *value_item         = NULL;
	uint8_t *data                            = NULL;
	static char *function                    = "libregf_value_get_value_data";
	size_t data_size                         = 0;

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
	internal_value = (libregf_internal_value_t *) value;

	if( value_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value data.",
		 function );

		return( -1 );
	}
	if( value_data_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid value data size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( libfdata_list_element_get_element_value(
	     internal_value->values_list_element,
	     (intptr_t *) internal_value->file_io_handle,
	     internal_value->values_cache,
	     (intptr_t **) &value_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value item.",
		 function );

		return( -1 );
	}
	if( libregf_value_item_get_data(
	     value_item,
	     internal_value->file_io_handle,
	     &data,
	     &data_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value data.",
		 function );

		return( -1 );
	}
	if( value_data_size < data_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid value data size value out of bounds.",
		 function );

		return( -1 );
	}
	if( memory_copy(
	     value_data,
	     data,
	     data_size ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
		 "%s: unable to copy value data.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the 32-bit value
 * Returns 1 if successful or -1 on error
 */
int libregf_value_get_value_32bit(
     libregf_value_t *value,
     uint32_t *value_32bit,
     libcerror_error_t **error )
{
	libregf_internal_value_t *internal_value = NULL;
	libregf_value_item_t *value_item         = NULL;
	uint8_t *value_data                      = NULL;
	static char *function                    = "libregf_value_get_value_32bit";
	size_t value_data_size                   = 0;
	uint8_t byte_order                       = 0;

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
	internal_value = (libregf_internal_value_t *) value;

	if( libfdata_list_element_get_element_value(
	     internal_value->values_list_element,
	     (intptr_t *) internal_value->file_io_handle,
	     internal_value->values_cache,
	     (intptr_t **) &value_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value item.",
		 function );

		return( -1 );
	}
	if( value_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing value item.",
		 function );

		return( -1 );
	}
	if( ( value_item->type != LIBREGF_VALUE_TYPE_INTEGER_32BIT_BIG_ENDIAN )
	 && ( value_item->type != LIBREGF_VALUE_TYPE_INTEGER_32BIT_LITTLE_ENDIAN ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported 32-bit integer value type: 0x%04" PRIx32 ".",
		 function,
		 value_item->type );

		return( -1 );
	}
	if( value_item->type == LIBREGF_VALUE_TYPE_INTEGER_32BIT_BIG_ENDIAN )
	{
		byte_order = LIBREGF_ENDIAN_BIG;
	}
	else
	{
		byte_order = LIBREGF_ENDIAN_LITTLE;
	}
	if( libregf_value_item_get_data(
	     value_item,
	     internal_value->file_io_handle,
	     &value_data,
	     &value_data_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value data.",
		 function );

		return( -1 );
	}
	if( value_data_size != 4 )
	{
		if( value_data_size > 4 )
		{
			value_data_size = 4;
		}
		value_item->item_flags |= LIBREGF_VALUE_ITEM_FLAG_IS_CORRUPTED;
	}
	if( libregf_value_type_copy_to_32bit(
	     value_data,
	     value_data_size,
	     byte_order,
	     value_32bit,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_CONVERSION,
		 LIBCERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to set 32-bit value.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the 64-bit value
 * Returns 1 if successful or -1 on error
 */
int libregf_value_get_value_64bit(
     libregf_value_t *value,
     uint64_t *value_64bit,
     libcerror_error_t **error )
{
	libregf_internal_value_t *internal_value = NULL;
	libregf_value_item_t *value_item         = NULL;
	uint8_t *value_data                      = NULL;
	static char *function                    = "libregf_value_get_value_64bit";
	size_t value_data_size                   = 0;

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
	internal_value = (libregf_internal_value_t *) value;

	if( libfdata_list_element_get_element_value(
	     internal_value->values_list_element,
	     (intptr_t *) internal_value->file_io_handle,
	     internal_value->values_cache,
	     (intptr_t **) &value_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value item.",
		 function );

		return( -1 );
	}
	if( value_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing value item.",
		 function );

		return( -1 );
	}
	if( value_item->type != LIBREGF_VALUE_TYPE_INTEGER_64BIT_LITTLE_ENDIAN )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported 64-bit integer value type: 0x%04" PRIx32 ".",
		 function,
		 value_item->type );

		return( -1 );
	}
	if( libregf_value_item_get_data(
	     value_item,
	     internal_value->file_io_handle,
	     &value_data,
	     &value_data_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value data.",
		 function );

		return( -1 );
	}
	if( value_data_size != 8 )
	{
		if( value_data_size > 8 )
		{
			value_data_size = 8;
		}
		value_item->item_flags |= LIBREGF_VALUE_ITEM_FLAG_IS_CORRUPTED;
	}
	if( libregf_value_type_copy_to_64bit(
	     value_data,
	     value_data_size,
	     LIBREGF_ENDIAN_LITTLE,
	     value_64bit,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_CONVERSION,
		 LIBCERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to set 64-bit value.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the UTF-8 string size
 * The returned size includes the end of string character
 * Returns 1 if successful or -1 on error
 */
int libregf_value_get_value_utf8_string_size(
     libregf_value_t *value,
     size_t *utf8_string_size,
     libcerror_error_t **error )
{
	libregf_internal_value_t *internal_value = NULL;
	libregf_value_item_t *value_item         = NULL;
	uint8_t *value_data                      = NULL;
	static char *function                    = "libregf_value_get_value_utf8_string_size";
	size_t value_data_size                   = 0;

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
	internal_value = (libregf_internal_value_t *) value;

	if( internal_value->io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid value - missing IO handle.",
		 function );

		return( -1 );
	}
	if( libfdata_list_element_get_element_value(
	     internal_value->values_list_element,
	     (intptr_t *) internal_value->file_io_handle,
	     internal_value->values_cache,
	     (intptr_t **) &value_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value item.",
		 function );

		return( -1 );
	}
	if( value_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing value item.",
		 function );

		return( -1 );
	}
	if( ( value_item->type != LIBREGF_VALUE_TYPE_STRING )
	 && ( value_item->type != LIBREGF_VALUE_TYPE_EXPANDABLE_STRING )
	 && ( value_item->type != LIBREGF_VALUE_TYPE_SYMBOLIC_LINK ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported string value type: 0x%04" PRIx32 ".",
		 function,
		 value_item->type );

		return( -1 );
	}
	if( libregf_value_item_get_data(
	     value_item,
	     internal_value->file_io_handle,
	     &value_data,
	     &value_data_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value data.",
		 function );

		return( -1 );
	}
	if( libregf_value_type_get_utf8_string_size(
	     value_data,
	     value_data_size,
	     utf8_string_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_CONVERSION,
		 LIBCERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to set UTF-8 string size value.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the UTF-8 string value
 * The function uses a codepage if necessary, it uses the codepage set for the library
 * The size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libregf_value_get_value_utf8_string(
     libregf_value_t *value,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     libcerror_error_t **error )
{
	libregf_internal_value_t *internal_value = NULL;
	libregf_value_item_t *value_item         = NULL;
	uint8_t *value_data                      = NULL;
	static char *function                    = "libregf_value_get_value_utf8_string";
	size_t value_data_size                   = 0;

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
	internal_value = (libregf_internal_value_t *) value;

	if( internal_value->io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid value - missing IO handle.",
		 function );

		return( -1 );
	}
	if( libfdata_list_element_get_element_value(
	     internal_value->values_list_element,
	     (intptr_t *) internal_value->file_io_handle,
	     internal_value->values_cache,
	     (intptr_t **) &value_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value item.",
		 function );

		return( -1 );
	}
	if( value_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing value item.",
		 function );

		return( -1 );
	}
	if( ( value_item->type != LIBREGF_VALUE_TYPE_STRING )
	 && ( value_item->type != LIBREGF_VALUE_TYPE_EXPANDABLE_STRING )
	 && ( value_item->type != LIBREGF_VALUE_TYPE_SYMBOLIC_LINK ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported string value type: 0x%04" PRIx32 ".",
		 function,
		 value_item->type );

		return( -1 );
	}
	if( libregf_value_item_get_data(
	     value_item,
	     internal_value->file_io_handle,
	     &value_data,
	     &value_data_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value data.",
		 function );

		return( -1 );
	}
	if( libregf_value_type_copy_to_utf8_string(
	     value_data,
	     value_data_size,
	     utf8_string,
	     utf8_string_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_CONVERSION,
		 LIBCERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to set UTF-8 string value.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the UTF-16 string size at a specific value from the referenced value
 * The returned size includes the end of string character
 * Returns 1 if successful or -1 on error
 */
int libregf_value_get_value_utf16_string_size(
     libregf_value_t *value,
     size_t *utf16_string_size,
     libcerror_error_t **error )
{
	libregf_internal_value_t *internal_value = NULL;
	libregf_value_item_t *value_item         = NULL;
	uint8_t *value_data                      = NULL;
	static char *function                    = "libregf_value_get_value_utf16_string_size";
	size_t value_data_size                   = 0;

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
	internal_value = (libregf_internal_value_t *) value;

	if( internal_value->io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid value - missing IO handle.",
		 function );

		return( -1 );
	}
	if( libfdata_list_element_get_element_value(
	     internal_value->values_list_element,
	     (intptr_t *) internal_value->file_io_handle,
	     internal_value->values_cache,
	     (intptr_t **) &value_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value item.",
		 function );

		return( -1 );
	}
	if( value_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing value item.",
		 function );

		return( -1 );
	}
	if( ( value_item->type != LIBREGF_VALUE_TYPE_STRING )
	 && ( value_item->type != LIBREGF_VALUE_TYPE_EXPANDABLE_STRING )
	 && ( value_item->type != LIBREGF_VALUE_TYPE_SYMBOLIC_LINK ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported string value type: 0x%04" PRIx32 ".",
		 function,
		 value_item->type );

		return( -1 );
	}
	if( libregf_value_item_get_data(
	     value_item,
	     internal_value->file_io_handle,
	     &value_data,
	     &value_data_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value data.",
		 function );

		return( -1 );
	}
	if( libregf_value_type_get_utf16_string_size(
	     value_data,
	     value_data_size,
	     utf16_string_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_CONVERSION,
		 LIBCERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to set UTF-16 string size value.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the UTF-16 string value
 * The function uses a codepage if necessary, it uses the codepage set for the library
 * The size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libregf_value_get_value_utf16_string(
     libregf_value_t *value,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     libcerror_error_t **error )
{
	libregf_internal_value_t *internal_value = NULL;
	libregf_value_item_t *value_item         = NULL;
	uint8_t *value_data                      = NULL;
	static char *function                    = "libregf_value_get_value_utf16_string";
	size_t value_data_size                   = 0;

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
	internal_value = (libregf_internal_value_t *) value;

	if( internal_value->io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid value - missing IO handle.",
		 function );

		return( -1 );
	}
	if( libfdata_list_element_get_element_value(
	     internal_value->values_list_element,
	     (intptr_t *) internal_value->file_io_handle,
	     internal_value->values_cache,
	     (intptr_t **) &value_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value item.",
		 function );

		return( -1 );
	}
	if( value_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing value item.",
		 function );

		return( -1 );
	}
	if( ( value_item->type != LIBREGF_VALUE_TYPE_STRING )
	 && ( value_item->type != LIBREGF_VALUE_TYPE_EXPANDABLE_STRING )
	 && ( value_item->type != LIBREGF_VALUE_TYPE_SYMBOLIC_LINK ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported string value type: 0x%04" PRIx32 ".",
		 function,
		 value_item->type );

		return( -1 );
	}
	if( libregf_value_item_get_data(
	     value_item,
	     internal_value->file_io_handle,
	     &value_data,
	     &value_data_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value data.",
		 function );

		return( -1 );
	}
	if( libregf_value_type_copy_to_utf16_string(
	     value_data,
	     value_data_size,
	     utf16_string,
	     utf16_string_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_CONVERSION,
		 LIBCERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to set UTF-16 string value.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the binary data size
 * Returns 1 if successful or -1 on error
 */
int libregf_value_get_value_binary_data_size(
     libregf_value_t *value,
     size_t *size,
     libcerror_error_t **error )
{
	libregf_internal_value_t *internal_value = NULL;
	libregf_value_item_t *value_item         = NULL;
	static char *function                    = "libregf_value_get_value_binary_data_size";

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
	internal_value = (libregf_internal_value_t *) value;

	if( libfdata_list_element_get_element_value(
	     internal_value->values_list_element,
	     (intptr_t *) internal_value->file_io_handle,
	     internal_value->values_cache,
	     (intptr_t **) &value_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value item.",
		 function );

		return( -1 );
	}
	if( value_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing value item.",
		 function );

		return( -1 );
	}
	if( value_item->type != LIBREGF_VALUE_TYPE_BINARY_DATA )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported binary data value type: 0x%04" PRIx32 ".",
		 function,
		 value_item->type );

		return( -1 );
	}
	if( libregf_value_item_get_data_size(
	     value_item,
	     size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value data size.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the binary data value
 * Returns 1 if successful or -1 on error
 */
int libregf_value_get_value_binary_data(
     libregf_value_t *value,
     uint8_t *binary_data,
     size_t size,
     libcerror_error_t **error )
{
	libregf_internal_value_t *internal_value = NULL;
	libregf_value_item_t *value_item         = NULL;
	uint8_t *value_data                      = NULL;
	static char *function                    = "libregf_value_get_value_binary_data";
	size_t value_data_size                   = 0;

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
	internal_value = (libregf_internal_value_t *) value;

	if( libfdata_list_element_get_element_value(
	     internal_value->values_list_element,
	     (intptr_t *) internal_value->file_io_handle,
	     internal_value->values_cache,
	     (intptr_t **) &value_item,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value item.",
		 function );

		return( -1 );
	}
	if( value_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing value item.",
		 function );

		return( -1 );
	}
	if( value_item->type != LIBREGF_VALUE_TYPE_BINARY_DATA )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported binary data value type: 0x%04" PRIx32 ".",
		 function,
		 value_item->type );

		return( -1 );
	}
/* TODO optimize to read directly from block or stream */
	if( libregf_value_item_get_data(
	     value_item,
	     internal_value->file_io_handle,
	     &value_data,
	     &value_data_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value data.",
		 function );

		return( -1 );
	}
	if( libregf_value_type_copy_to_binary_data(
	     value_data,
	     value_data_size,
	     binary_data,
	     size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_CONVERSION,
		 LIBCERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to set binary data.",
		 function );

		return( -1 );
	}
	return( 1 );
}

