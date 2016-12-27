/*
 * Value item functions
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

#include "libregf_data_type.h"
#include "libregf_debug.h"
#include "libregf_definitions.h"
#include "libregf_hive_bin.h"
#include "libregf_hive_bin_cell.h"
#include "libregf_hive_bins_list.h"
#include "libregf_libbfio.h"
#include "libregf_libcerror.h"
#include "libregf_libcnotify.h"
#include "libregf_libfcache.h"
#include "libregf_libfdata.h"
#include "libregf_libuna.h"
#include "libregf_unused.h"
#include "libregf_value_item.h"

#include "regf_cell_values.h"

/* Creates value item
 * Make sure the value value_item is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libregf_value_item_initialize(
     libregf_value_item_t **value_item,
     libcerror_error_t **error )
{
	static char *function = "libregf_value_item_initialize";

	if( value_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value item.",
		 function );

		return( -1 );
	}
	if( *value_item != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid value item value already set.",
		 function );

		return( -1 );
	}
	*value_item = memory_allocate_structure(
	               libregf_value_item_t );

	if( *value_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create value item.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *value_item,
	     0,
	     sizeof( libregf_value_item_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear value item.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( *value_item != NULL )
	{
		memory_free(
		 *value_item );

		*value_item = NULL;
	}
	return( -1 );
}

/* Frees value item
 * Returns 1 if successful or -1 on error
 */
int libregf_value_item_free(
     libregf_value_item_t **value_item,
     libcerror_error_t **error )
{
	static char *function = "libregf_value_item_free";
	int result            = 1;

	if( value_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value item.",
		 function );

		return( -1 );
	}
	if( *value_item != NULL )
	{
		if( ( *value_item )->name != NULL )
		{
			memory_free(
			 ( *value_item )->name );
		}
		if( ( *value_item )->data_buffer != NULL )
		{
			memory_free(
			 ( *value_item )->data_buffer );
		}
		if( ( *value_item )->data_stream != NULL )
		{
			if( libfdata_stream_free(
			     &( ( *value_item )->data_stream ),
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: unable to free data stream.",
				 function );

				result = -1;
			}
		}
		memory_free(
		 *value_item );

		*value_item = NULL;
	}
	return( result );
}

/* Retrieves the data size
 * Returns 1 if successful or -1 on error
 */
int libregf_value_item_get_data_size(
     libregf_value_item_t *value_item,
     size_t *data_size,
     libcerror_error_t **error )
{
	static char *function     = "libregf_value_item_get_data_size";
	size64_t stream_data_size = 0;

	if( value_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value item.",
		 function );

		return( -1 );
	}
	if( ( value_item->data_type != LIBREGF_VALUE_ITEM_DATA_TYPE_BUFFER )
	 && ( value_item->data_type != LIBREGF_VALUE_ITEM_DATA_TYPE_BLOCK ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported value item data type: 0x%02" PRIx8 ".",
		 function,
		 value_item->data_type );

		return( -1 );
	}
	if( data_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid data size.",
		 function );

		return( -1 );
	}
	if( ( value_item->data_type == LIBREGF_VALUE_ITEM_DATA_TYPE_BLOCK )
	 && ( value_item->data_buffer == NULL ) )
	{
		if( libfdata_stream_get_size(
		     value_item->data_stream,
		     &stream_data_size,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve size from data stream.",
			 function );

			return( -1 );
		}
		if( stream_data_size > (size64_t) SSIZE_MAX )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: invalid stream data size value out of bounds.",
			 function );

			return( -1 );
		}
		*data_size = (size_t) stream_data_size;
	}
	else
	{
		*data_size = value_item->data_buffer_size;
	}
	return( 1 );
}

/* Retrieves the data
 * Returns 1 if successful or -1 on error
 */
int libregf_value_item_get_data(
     libregf_value_item_t *value_item,
     libbfio_handle_t *file_io_handle,
     uint8_t **data,
     size_t *data_size,
     libcerror_error_t **error )
{
	static char *function     = "libregf_value_item_get_data";
	size64_t stream_data_size = 0;
	ssize_t read_count        = 0;

	if( value_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value item.",
		 function );

		return( -1 );
	}
	if( ( value_item->data_type != LIBREGF_VALUE_ITEM_DATA_TYPE_BUFFER )
	 && ( value_item->data_type != LIBREGF_VALUE_ITEM_DATA_TYPE_BLOCK ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported value item data type: 0x%02" PRIx8 ".",
		 function,
		 value_item->data_type );

		return( -1 );
	}
	if( data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid data.",
		 function );

		return( -1 );
	}
	if( data_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid data size.",
		 function );

		return( -1 );
	}
	if( ( value_item->data_type == LIBREGF_VALUE_ITEM_DATA_TYPE_BLOCK )
	 && ( value_item->data_buffer == NULL ) )
	{
		if( libfdata_stream_get_size(
		     value_item->data_stream,
		     &stream_data_size,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve size from data stream.",
			 function );

			goto on_error;
		}
		if( stream_data_size > (size64_t) SSIZE_MAX )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: invalid stream data size value out of bounds.",
			 function );

			goto on_error;
		}
		/* Cache the whole stream this should not happen very often "normal" files
		 */
		value_item->data_buffer = (uint8_t *) memory_allocate(
		                                       sizeof( uint8_t ) * (size_t) stream_data_size );

		if( value_item->data_buffer == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_MEMORY,
			 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to create data buffer.",
			 function );

			goto on_error;
		}
		value_item->data_buffer_size = (size_t) stream_data_size;

		if( libfdata_stream_seek_offset(
		     value_item->data_stream,
		     0,
		     SEEK_SET,
		     error ) == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_SEEK_FAILED,
			 "%s: unable to seek start of data stream.",
			 function );

			goto on_error;
		}
		read_count = libfdata_stream_read_buffer(
		              value_item->data_stream,
		              (intptr_t *) file_io_handle,
		              value_item->data_buffer,
		              value_item->data_buffer_size,
		              0,
		              error );

		if( read_count != (ssize_t) value_item->data_buffer_size )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_READ_FAILED,
			 "%s: unable to read data from data stream.",
			 function );

			goto on_error;
		}
	}
	*data      = value_item->data_buffer;
	*data_size = value_item->data_buffer_size;

	return( 1 );

on_error:
	if( value_item->data_buffer != NULL )
	{
		memory_free(
		 value_item->data_buffer );

		value_item->data_buffer = NULL;
	}
	value_item->data_buffer_size = 0;

	return( -1 );
}

/* Reads a value key
 * Returns 1 if successful or -1 on error
 */
int libregf_value_item_read_value_key(
     libregf_value_item_t *value_item,
     libbfio_handle_t *file_io_handle,
     libregf_hive_bins_list_t *hive_bins_list,
     uint32_t value_key_offset,
     libcerror_error_t **error )
{
	libregf_hive_bin_t *hive_bin                 = NULL;
	libregf_hive_bin_cell_t *hive_bin_cell       = NULL;
	uint8_t *data_offset_data                    = NULL;
	uint8_t *hive_bin_cell_data                  = NULL;
	static char *function                        = "libregf_value_item_read_value_key";
	libuna_unicode_character_t unicode_character = 0;
	off64_t hive_bin_data_offset                 = 0;
	size_t hive_bin_cell_size                    = 0;
	size_t name_index                            = 0;
	uint32_t data_offset                         = 0;
	uint32_t data_size                           = 0;
	int hive_bin_index                           = 0;
	int result                                   = 0;

#if defined( HAVE_DEBUG_OUTPUT )
	uint16_t value_16bit                         = 0;
#endif

	if( value_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value item.",
		 function );

		return( -1 );
	}
	if( hive_bins_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid hive bins list.",
		 function );

		return( -1 );
	}
	if( ( value_key_offset == 0 )
	 || ( value_key_offset == 0xffffffffUL ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: invalid value key offset.",
		 function );

		return( -1 );
	}
	if( libfdata_list_get_element_value_at_offset(
	     hive_bins_list->data_list,
	     (intptr_t *) file_io_handle,
	     hive_bins_list->data_cache,
	     (off64_t) value_key_offset,
	     &hive_bin_index,
	     &hive_bin_data_offset,
	     (intptr_t **) &hive_bin,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve hive bin at offset: %" PRIu32 ".",
		 function,
		 value_key_offset );

		goto on_error;
	}
	if( libregf_hive_bin_get_cell_at_offset(
	     hive_bin,
	     value_key_offset,
	     &hive_bin_cell,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve hive bin cell at offset: %" PRIu32 ".",
		 function,
		 value_key_offset );

		goto on_error;
	}
	hive_bin_cell_data = hive_bin_cell->data;
	hive_bin_cell_size = hive_bin_cell->size;

#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: data:\n",
		 function );
		libcnotify_print_data(
		 hive_bin_cell_data,
		 hive_bin_cell_size,
		 LIBCNOTIFY_PRINT_DATA_FLAG_GROUP_DATA );
	}
#endif
	if( hive_bin_cell_size < sizeof( regf_value_key_t ) )
	{
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: invalid hive bin cell size value too small.",
			 function );
		}
#endif
		value_item->item_flags |= LIBREGF_VALUE_ITEM_FLAG_IS_CORRUPTED;

		return( 1 );
	}
	/* Check if the cell signature matches that of a value key: "vk"
	 */
	if( ( hive_bin_cell_data[ 0 ] != (uint8_t) 'v' )
	 || ( hive_bin_cell_data[ 1 ] != (uint8_t) 'k' ) )
	{
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: unsupported value key signature.",
			 function );
		}
#endif
		value_item->item_flags |= LIBREGF_VALUE_ITEM_FLAG_IS_CORRUPTED;

		return( 1 );
	}
	byte_stream_copy_to_uint16_little_endian(
	 ( (regf_value_key_t *) hive_bin_cell_data )->value_name_size,
	 value_item->name_size );

	byte_stream_copy_to_uint32_little_endian(
	 ( (regf_value_key_t *) hive_bin_cell_data )->data_size,
	 data_size );

	byte_stream_copy_to_uint32_little_endian(
	 ( (regf_value_key_t *) hive_bin_cell_data )->data_offset,
	 data_offset );

	byte_stream_copy_to_uint32_little_endian(
	 ( (regf_value_key_t *) hive_bin_cell_data )->data_type,
	 value_item->type );

	byte_stream_copy_to_uint16_little_endian(
	 ( (regf_value_key_t *) hive_bin_cell_data )->flags,
	 value_item->flags );

#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: signature\t\t\t\t: %c%c\n",
		 function,
		 ( (regf_value_key_t *) hive_bin_cell_data )->signature[ 0 ],
		 ( (regf_value_key_t *) hive_bin_cell_data )->signature[ 1 ] );

		libcnotify_printf(
		 "%s: value name size\t\t\t: %" PRIu16 "\n",
		 function,
		 value_item->name_size );

		libcnotify_printf(
		 "%s: data size\t\t\t\t: 0x%08" PRIx32 " (%" PRIu32 ")\n",
		 function,
		 data_size,
		 data_size & 0x5fffffffUL );

		libcnotify_printf(
		 "%s: data offset\t\t\t\t: 0x%08" PRIx32 "\n",
		 function,
		 data_offset );

		libcnotify_printf(
		 "%s: data type\t\t\t\t: 0x%08" PRIx32 " (%s) %s\n",
		 function,
		 value_item->type,
		 libregf_data_type_get_identifier(
		  value_item->type ),
		 libregf_data_type_get_description(
		  value_item->type ) );

		libcnotify_printf(
		 "%s: flags\t\t\t\t: 0x%04" PRIx16 "\n",
		 function,
		 value_item->flags );
		libregf_debug_print_value_key_flags(
		 value_item->flags );

		byte_stream_copy_to_uint16_little_endian(
		 ( (regf_value_key_t *) hive_bin_cell_data )->unknown1,
		 value_16bit );
		libcnotify_printf(
		 "%s: unknown1\t\t\t\t: 0x%04" PRIx16 " (%" PRIu16 ")\n",
		 function,
		 value_16bit,
		 value_16bit );
	}
#endif
	data_offset_data = ( (regf_value_key_t *) hive_bin_cell_data )->data_offset;

	hive_bin_cell_data += sizeof( regf_value_key_t );
	hive_bin_cell_size -= sizeof( regf_value_key_t );

	value_item->name_hash = 0;

	if( value_item->name_size > 0 )
	{
		if( value_item->name_size > hive_bin_cell_size )
		{
#if defined( HAVE_DEBUG_OUTPUT )
			if( libcnotify_verbose != 0 )
			{
				libcnotify_printf(
				 "%s: invalid value name size value out of bounds.",
				 function );
			}
#endif
			value_item->item_flags |= LIBREGF_VALUE_ITEM_FLAG_IS_CORRUPTED;

			return( 1 );
		}
		value_item->name = (uint8_t *) memory_allocate(
		                                sizeof( uint8_t ) * (size_t) value_item->name_size );

		if( value_item->name == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_MEMORY,
			 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to create value name.",
			 function );

			goto on_error;
		}
		if( memory_copy(
		     value_item->name,
		     hive_bin_cell_data,
		     (size_t) value_item->name_size ) == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_MEMORY,
			 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
			 "%s: unable to copy value name.",
			 function );

			goto on_error;
		}
		while( name_index < (size_t) value_item->name_size )
		{
			if( ( value_item->flags & LIBREGF_VALUE_KEY_FLAG_NAME_IS_ASCII ) != 0 )
			{
				result = libuna_unicode_character_copy_from_byte_stream(
					  &unicode_character,
					  value_item->name,
					  (size_t) value_item->name_size,
					  &name_index,
					  hive_bins_list->io_handle->ascii_codepage,
					  error );
			}
			else
			{
				result = libuna_unicode_character_copy_from_utf16_stream(
					  &unicode_character,
					  value_item->name,
					  (size_t) value_item->name_size,
					  &name_index,
					  LIBUNA_ENDIAN_LITTLE,
					  error );
			}
			if( result != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy value name to Unicode character.",
				 function );

				goto on_error;
			}
			value_item->name_hash *= 37;
			value_item->name_hash += (uint32_t) towupper( (wint_t) unicode_character );
		}
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			if( ( value_item->flags & LIBREGF_VALUE_KEY_FLAG_NAME_IS_ASCII ) != 0 )
			{
				if( libregf_debug_print_string_value(
				     function,
				     "value name\t\t\t\t",
				     value_item->name,
				     (size_t) value_item->name_size,
				     hive_bins_list->io_handle->ascii_codepage,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_PRINT_FAILED,
					 "%s: unable to print string value.",
					 function );

					goto on_error;
				}
			}
			else
			{
				if( libregf_debug_print_utf16_string_value(
				     function,
				     "value name\t\t\t\t",
				     value_item->name,
				     (size_t) value_item->name_size,
				     LIBUNA_ENDIAN_LITTLE,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_PRINT_FAILED,
					 "%s: unable to print UTF-16 string value.",
					 function );

					goto on_error;
				}
			}
			libcnotify_printf(
			 "%s: value name hash\t\t\t: 0x%08" PRIx32 "\n",
			 function,
			 value_item->name_hash );
		}
		hive_bin_cell_data += value_item->name_size;
		hive_bin_cell_size -= value_item->name_size;
#endif
	}
#if defined( HAVE_DEBUG_OUTPUT )
	else if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: value name\t\t\t\t: (default)\n",
		 function );

		libcnotify_printf(
		 "%s: value name hash\t\t\t: 0x%08" PRIx32 "\n",
		 function,
		 value_item->name_hash );
	}
	if( libcnotify_verbose != 0 )
	{
		if( hive_bin_cell_size > 0 )
		{
			libcnotify_printf(
			 "%s: padding:\n",
			 function );
			libcnotify_print_data(
			 hive_bin_cell_data,
			 hive_bin_cell_size,
			 LIBCNOTIFY_PRINT_DATA_FLAG_GROUP_DATA );
		}
		else
		{
			libcnotify_printf(
			 "\n" );
		}
	}
#endif
	/* Check if the value data is stored in the cell value
	 */
	if( ( data_size & 0x80000000UL ) != 0 )
	{
		data_size &= 0x7fffffffUL;

		value_item->data_type = LIBREGF_VALUE_ITEM_DATA_TYPE_BUFFER;

		if( data_size > 4 )
		{
#if defined( HAVE_DEBUG_OUTPUT )
			if( libcnotify_verbose != 0 )
			{
				libcnotify_printf(
				 "%s: invalid data size value out of bounds.",
				 function );
			}
#endif
			value_item->item_flags |= LIBREGF_VALUE_ITEM_FLAG_IS_CORRUPTED;

			return( 1 );
		}
		else if( data_size > 0 )
		{
			if( ( value_item->type == LIBREGF_VALUE_TYPE_STRING )
			 || ( value_item->type == LIBREGF_VALUE_TYPE_EXPANDABLE_STRING ) )
			{
				if( ( data_size == 1 )
				 || ( data_size == 3 ) )
				{
					data_size += 1;

					data_offset_data[ 4 - data_size ] = 0;
				}
			}
			value_item->data_buffer = (uint8_t *) memory_allocate(
			                                       sizeof( uint8_t ) * data_size );

			if( value_item->data_buffer == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_MEMORY,
				 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
				 "%s: unable to create data buffer.",
				 function );

				goto on_error;
			}
			value_item->data_buffer_size = (size_t) data_size;

			if( memory_copy(
			     value_item->data_buffer,
			     &( data_offset_data[ 4 - data_size ] ),
			     data_size ) == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_MEMORY,
				 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
				 "%s: unable to copy data buffer.",
				 function );

				goto on_error;
			}
		}
	}
	else
	{
		result = libregf_hive_bins_list_get_index_at_offset(
		          hive_bins_list,
		          (off64_t) data_offset,
		          &hive_bin_index,
		          error );

		if( result == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to determine if data offset is valid.",
			 function );

			goto on_error;
		}
		else if( result == 0 )
		{
			value_item->item_flags |= LIBREGF_VALUE_ITEM_FLAG_IS_CORRUPTED;
		}
		else if( libregf_value_item_read_value_data(
		          value_item,
		          file_io_handle,
		          hive_bins_list,
		          data_offset,
		          data_size,
		          error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_READ_FAILED,
			 "%s: unable to read value data at offset: %" PRIu32 ".",
			 function,
			 data_offset );

			goto on_error;
		}
	}
	return( 1 );

on_error:
	if( value_item->data_buffer != NULL )
	{
		memory_free(
		 value_item->data_buffer );

		value_item->data_buffer = NULL;
	}
	value_item->data_buffer_size = 0;

	if( value_item->name != NULL )
	{
		memory_free(
		 value_item->name );

		value_item->name = NULL;
	}
	value_item->name_size = 0;

	return( -1 );
}

/* Reads value data
 * Returns 1 if successful or -1 on error
 */
int libregf_value_item_read_value_data(
     libregf_value_item_t *value_item,
     libbfio_handle_t *file_io_handle,
     libregf_hive_bins_list_t *hive_bins_list,
     uint32_t value_data_offset,
     uint32_t value_data_size,
     libcerror_error_t **error )
{
	libregf_hive_bin_t *hive_bin           = NULL;
	libregf_hive_bin_cell_t *hive_bin_cell = NULL;
	uint8_t *hive_bin_cell_data            = NULL;
	static char *function                  = "libregf_value_item_read_value_data";
	off64_t hive_bin_data_offset           = 0;
	size_t hive_bin_cell_size              = 0;
	size_t utf16_string_size               = 0;
	uint32_t calculated_value_data_size    = 0;
	uint32_t data_block_list_offset        = 0;
	uint16_t number_of_segments            = 0;
	int hive_bin_index                     = 0;
	int result                             = 0;

	if( value_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value item.",
		 function );

		return( -1 );
	}
	if( hive_bins_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid hive bins list.",
		 function );

		return( -1 );
	}
	if( ( value_data_offset == 0 )
	 || ( value_data_offset == 0xffffffffUL ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: invalid value data offset.",
		 function );

		return( -1 );
	}
	if( libfdata_list_get_element_value_at_offset(
	     hive_bins_list->data_list,
	     (intptr_t *) file_io_handle,
	     hive_bins_list->data_cache,
	     (off64_t) value_data_offset,
	     &hive_bin_index,
	     &hive_bin_data_offset,
	     (intptr_t **) &hive_bin,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve hive bin at offset: %" PRIu32 ".",
		 function,
		 value_data_offset );

		goto on_error;
	}
	if( libregf_hive_bin_get_cell_at_offset(
	     hive_bin,
	     value_data_offset,
	     &hive_bin_cell,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve hive bin cell at offset: %" PRIu32 ".",
		 function,
		 value_data_offset );

		goto on_error;
	}
	hive_bin_cell_data = hive_bin_cell->data;
	hive_bin_cell_size = hive_bin_cell->size;

#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: data:\n",
		 function );
		libcnotify_print_data(
		 hive_bin_cell_data,
		 hive_bin_cell_size,
		 LIBCNOTIFY_PRINT_DATA_FLAG_GROUP_DATA );
	}
#endif
	/* As of version 1.5 large value data is stored in a data block
	 */
	if( ( value_data_size > 16344 )
	 && ( hive_bins_list->io_handle->major_version >= 1 )
	 && ( hive_bins_list->io_handle->minor_version >= 5 ) )
	{
		/* Check if the cell signature matches that of a data block key: "db"
		 */
		if( ( hive_bin_cell_data[ 0 ] != (uint8_t) 'd' )
		 || ( hive_bin_cell_data[ 1 ] != (uint8_t) 'b' ) )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: unsupported data block key signature.",
			 function );

			goto on_error;
		}
		byte_stream_copy_to_uint16_little_endian(
		 ( (regf_data_block_key_t *) hive_bin_cell_data )->number_of_segments,
		 number_of_segments );

		byte_stream_copy_to_uint32_little_endian(
		 ( (regf_data_block_key_t *) hive_bin_cell_data )->data_block_list_offset,
		 data_block_list_offset );

#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: signature\t\t\t\t\t: %c%c\n",
			 function,
			 ( (regf_data_block_key_t *) hive_bin_cell_data )->signature[ 0 ],
			 ( (regf_data_block_key_t *) hive_bin_cell_data )->signature[ 1 ] );

			libcnotify_printf(
			 "%s: number of segments\t\t\t\t: %" PRIu16 "\n",
			 function,
			 number_of_segments );

			libcnotify_printf(
			 "%s: data block list offset\t\t\t: 0x%08" PRIx32 "\n",
			 function,
			 data_block_list_offset );
		}
		hive_bin_cell_data += sizeof( regf_data_block_key_t );
		hive_bin_cell_size -= sizeof( regf_data_block_key_t );
#endif
/* TODO: Check if stored number of segments matches the calculated */

		result = libregf_hive_bins_list_get_index_at_offset(
		          hive_bins_list,
		          (off64_t) data_block_list_offset,
		          &hive_bin_index,
		          error );

		if( result == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to determine if data block list offset is valid.",
			 function );

			goto on_error;
		}
		else if( result == 0 )
		{
			value_item->item_flags |= LIBREGF_VALUE_ITEM_FLAG_IS_CORRUPTED;
		}
		else if( libregf_value_item_read_data_block_list(
		          value_item,
		          file_io_handle,
		          hive_bins_list,
		          data_block_list_offset,
		          number_of_segments,
		          value_data_size,
		          error ) == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_READ_FAILED,
			 "%s: unable to read data block list at offset: %" PRIu32 ".",
			 function,
			 data_block_list_offset );

			goto on_error;
		}
	}
	else
	{
		if( value_data_size > hive_bin_cell_size )
		{
#if defined( HAVE_DEBUG_OUTPUT )
			if( libcnotify_verbose != 0 )
			{
				libcnotify_printf(
				 "%s: value data size: %" PRIu32 " exceeds hive bin cell size: %" PRIzd ".\n",
				 function,
				 value_data_size,
				 hive_bin_cell_size );
			}
#endif
			value_item->item_flags |= LIBREGF_VALUE_ITEM_FLAG_IS_CORRUPTED;

			if( hive_bin_cell_size > (size_t) UINT32_MAX )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: invalid hive bin cell size value out of bounds.",
				 function );

				goto on_error;
			}
			value_data_size = (uint32_t) hive_bin_cell_size;
		}
		if( ( value_item->type == LIBREGF_VALUE_TYPE_STRING )
		 || ( value_item->type == LIBREGF_VALUE_TYPE_EXPANDABLE_STRING ) )
		{
			calculated_value_data_size = 0;

			while( ( calculated_value_data_size + 2 ) <= (uint32_t) hive_bin_cell_size )
			{
				if( ( hive_bin_cell_data[ calculated_value_data_size ] == 0 )
				 && ( hive_bin_cell_data[ calculated_value_data_size + 1 ] == 0 ) )
				{
					calculated_value_data_size += 2;

					break;
				}
				calculated_value_data_size += 2;
			}
			if( calculated_value_data_size != value_data_size )
			{
				/* Check if the calculated string is sane
				 */
				result = libuna_utf16_string_size_from_utf16_stream(
					  hive_bin_cell_data,
					  calculated_value_data_size,
					  LIBUNA_ENDIAN_LITTLE,
					  &utf16_string_size,
					  NULL );

				if( result != 1 )
				{
					calculated_value_data_size = value_data_size;
				}
			}
			if( calculated_value_data_size != value_data_size )
			{
#if defined( HAVE_DEBUG_OUTPUT )
				if( libcnotify_verbose != 0 )
				{
					libcnotify_printf(
					 "%s: mismatch in value data size (calculated: %" PRIu32 ", stored: %" PRIu32 ").\n",
					 function,
					 calculated_value_data_size,
					 value_data_size );
				}
#endif
				value_data_size = calculated_value_data_size;
			}
		}
/* TODO what about a maximum of 1 MiB ? */

		value_item->data_type = LIBREGF_VALUE_ITEM_DATA_TYPE_BUFFER;

		if( value_data_size > 0 )
		{
			value_item->data_buffer = (uint8_t *) memory_allocate(
			                                       sizeof( uint8_t ) * (size_t) value_data_size );

			if( value_item->data_buffer == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_MEMORY,
				 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
				 "%s: unable to create data buffer.",
				 function );

				goto on_error;
			}
			value_item->data_buffer_size = (size_t) value_data_size;

			if( memory_copy(
			     value_item->data_buffer,
			     hive_bin_cell_data,
			     value_data_size  ) == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_MEMORY,
				 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
				 "%s: unable to copy data buffer.",
				 function );

				goto on_error;
			}
		}
#if defined( HAVE_DEBUG_OUTPUT )
		hive_bin_cell_data += value_data_size;
		hive_bin_cell_size -= value_data_size;
#endif
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		if( hive_bin_cell_size > 0 )
		{
			libcnotify_printf(
			 "%s: padding:\n",
			 function );
			libcnotify_print_data(
			 hive_bin_cell_data,
			 hive_bin_cell_size,
			 LIBCNOTIFY_PRINT_DATA_FLAG_GROUP_DATA );
		}
	}
#endif
	return( 1 );

on_error:
	if( value_item->data_buffer != NULL )
	{
		memory_free(
		 value_item->data_buffer );

		value_item->data_buffer = NULL;
	}
	return( -1 );
}

/* Reads a data block (segments) list cell value
 * Returns 1 if successful, 0 if not or -1 on error
 */
int libregf_value_item_read_data_block_list(
     libregf_value_item_t *value_item,
     libbfio_handle_t *file_io_handle,
     libregf_hive_bins_list_t *hive_bins_list,
     uint32_t data_block_list_offset,
     uint16_t number_of_segments,
     uint32_t value_data_size,
     libcerror_error_t **error )
{
	libregf_hive_bin_t *hive_bin           = NULL;
	libregf_hive_bin_cell_t *hive_bin_cell = NULL;
	uint8_t *hive_bin_cell_data            = NULL;
	static char *function                  = "libregf_value_item_read_data_block_list";
	off64_t hive_bin_data_offset           = 0;
	size_t hive_bin_cell_size              = 0;
	uint32_t calculated_value_data_size    = 0;
	uint32_t element_offset                = 0;
	uint32_t segment_offset                = 0;
	uint32_t segment_size                  = 0;
	uint16_t element_iterator              = 0;
	int hive_bin_index                     = 0;

	if( value_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value item.",
		 function );

		return( -1 );
	}
	if( hive_bins_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid hive bins list.",
		 function );

		return( -1 );
	}
	if( hive_bins_list->io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid hive bins list - missing IO handle.",
		 function );

		return( -1 );
	}
	if( ( data_block_list_offset == 0 )
	 || ( data_block_list_offset == 0xffffffffUL ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: invalid data block list offset.",
		 function );

		return( -1 );
	}
	if( libfdata_list_get_element_value_at_offset(
	     hive_bins_list->data_list,
	     (intptr_t *) file_io_handle,
	     hive_bins_list->data_cache,
	     (off64_t) data_block_list_offset,
	     &hive_bin_index,
	     &hive_bin_data_offset,
	     (intptr_t **) &hive_bin,
	     0,
	     error ) != 1 )
	{
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: unable to retrieve hive bin at offset: %" PRIu32 ".",
			 function,
			 data_block_list_offset );
		}
#endif
		value_item->item_flags |= LIBREGF_VALUE_ITEM_FLAG_IS_CORRUPTED;

		return( 0 );
	}
	if( libregf_hive_bin_get_cell_at_offset(
	     hive_bin,
	     data_block_list_offset,
	     &hive_bin_cell,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve hive bin cell at offset: %" PRIu32 ".",
		 function,
		 data_block_list_offset );

		goto on_error;
	}
	hive_bin_cell_data = hive_bin_cell->data;
	hive_bin_cell_size = hive_bin_cell->size;

#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: data:\n",
		 function );
		libcnotify_print_data(
		 hive_bin_cell_data,
		 hive_bin_cell_size,
		 LIBCNOTIFY_PRINT_DATA_FLAG_GROUP_DATA );
	}
#endif
	if( hive_bin_cell_size < (size_t) ( number_of_segments * 4 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid cell size value too small to contain number of values.",
		 function );

		goto on_error;
	}
	value_item->data_type = LIBREGF_VALUE_ITEM_DATA_TYPE_BLOCK;

	if( libfdata_stream_initialize(
	     &( value_item->data_stream ),
	     NULL,
	     NULL,
	     NULL,
	     NULL,
	     (ssize_t (*)(intptr_t *, intptr_t *, int, int, uint8_t *, size_t, uint32_t, uint8_t, libcerror_error_t **)) &libregf_io_handle_read_segment_data,
	     NULL,
	     (off64_t (*)(intptr_t *, intptr_t *, int, int, off64_t, libcerror_error_t **)) &libregf_io_handle_seek_segment_offset,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create value data stream.",
		 function );

		goto on_error;
	}
	if( number_of_segments > 0 )
	{
		if( libfdata_stream_resize(
		     value_item->data_stream,
		     (int) number_of_segments,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_RESIZE_FAILED,
			 "%s: unable to resize value data stream.",
			 function );

			goto on_error;
		}
		for( element_iterator = 0;
		     element_iterator < number_of_segments;
		     element_iterator++ )
		{
			byte_stream_copy_to_uint32_little_endian(
			 &( hive_bin_cell_data[ element_iterator * 4 ] ),
			 element_offset );

			hive_bin_cell_size -= 4;

#if defined( HAVE_DEBUG_OUTPUT )
			if( libcnotify_verbose != 0 )
			{
				libcnotify_printf(
				 "%s: element: %03" PRIu16 " offset\t\t\t: 0x%08" PRIx32 "\n",
				 function,
				 element_iterator,
				 element_offset );
			}
#endif
			/* Determine the size of the segment by retrieving the corresponding hive bin cell
			 */
			if( libfdata_list_get_element_value_at_offset(
			     hive_bins_list->data_list,
			     (intptr_t *) file_io_handle,
			     hive_bins_list->data_cache,
			     (off64_t) element_offset,
			     &hive_bin_index,
			     &hive_bin_data_offset,
			     (intptr_t **) &hive_bin,
			     0,
			     error ) != 1 )
			{
#if defined( HAVE_DEBUG_OUTPUT )
				if( libcnotify_verbose != 0 )
				{
					libcnotify_printf(
					 "%s: unable to retrieve hive bin at offset: %" PRIu32 ".",
					 function,
					 element_offset );
				}
#endif
				value_item->item_flags |= LIBREGF_VALUE_ITEM_FLAG_IS_CORRUPTED;

/* TODO return partial stream or 0-byte the missing data ? */
				libfdata_stream_free(
				 &( value_item->data_stream ),
				 NULL );

				return( 0 );
			}
			if( libregf_hive_bin_get_cell_at_offset(
			     hive_bin,
			     element_offset,
			     &hive_bin_cell,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve hive bin cell at offset: %" PRIu32 ".",
				 function,
				 element_offset );

				goto on_error;
			}
			segment_offset = (uint32_t) ( hive_bins_list->io_handle->hive_bins_list_offset + 4 + element_offset );
			segment_size   = hive_bin_cell->size - 4;

			if( ( calculated_value_data_size + segment_size ) > value_data_size )
			{
#if defined( HAVE_DEBUG_OUTPUT )
				if( libcnotify_verbose != 0 )
				{
					libcnotify_printf(
					 "%s: mismatch in value data size (calculated: %" PRIu32 ", stored: %" PRIu32 ").\n",
					 function,
					 calculated_value_data_size + segment_size,
					 value_data_size );
				}
#endif
				segment_size = value_data_size - calculated_value_data_size;
			}
			/* Point the stream directly to the data
			 */
			if( libfdata_stream_set_segment_by_index(
			     value_item->data_stream,
			     (int) element_iterator,
			     0,
			     (off64_t) segment_offset,
			     (size64_t) segment_size,
			     0,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_RESIZE_FAILED,
				 "%s: unable to set value data stream segment: %" PRIu16 ".",
				 function,
				 element_iterator );

				goto on_error;
			}
			calculated_value_data_size += segment_size;

			/* Make sure hive_bin_cell_data points to a valid data
			 */
			if( libfdata_list_get_element_value_at_offset(
			     hive_bins_list->data_list,
			     (intptr_t *) file_io_handle,
			     hive_bins_list->data_cache,
			     (off64_t) data_block_list_offset,
			     &hive_bin_index,
			     &hive_bin_data_offset,
			     (intptr_t **) &hive_bin,
			     0,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve hive bin at offset: %" PRIu32 ".",
				 function,
				 data_block_list_offset );

				goto on_error;
			}
			if( libregf_hive_bin_get_cell_at_offset(
			     hive_bin,
			     data_block_list_offset,
			     &hive_bin_cell,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve hive bin cell at offset: %" PRIu32 ".",
				 function,
				 data_block_list_offset );

				goto on_error;
			}
			hive_bin_cell_data = hive_bin_cell->data;
		}
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		if( hive_bin_cell_size > 0 )
		{
			libcnotify_printf(
			 "%s: padding:\n",
			 function );
			libcnotify_print_data(
			 hive_bin_cell_data,
			 hive_bin_cell_size,
			 LIBCNOTIFY_PRINT_DATA_FLAG_GROUP_DATA );
		}
		else
		{
			libcnotify_printf(
			 "\n" );
		}
	}
#endif
	return( 1 );

on_error:
	if( value_item->data_stream != NULL )
	{
		libfdata_stream_free(
		 &( value_item->data_stream ),
		 NULL );
	}
	return( -1 );
}

/* Reads a value
 * Returns 1 if successful or -1 on error
 */
int libregf_value_item_read_element_data(
     libregf_hive_bins_list_t *hive_bins_list,
     libbfio_handle_t *file_io_handle,
     libfdata_list_element_t *list_element,
     libfcache_cache_t *cache,
     int data_range_file_index LIBREGF_ATTRIBUTE_UNUSED,
     off64_t data_range_offset,
     size64_t data_range_size LIBREGF_ATTRIBUTE_UNUSED,
     uint32_t data_range_flags LIBREGF_ATTRIBUTE_UNUSED,
     uint8_t read_flags LIBREGF_ATTRIBUTE_UNUSED,
     libcerror_error_t **error )
{
	libregf_value_item_t *value_item = NULL;
	static char *function            = "libregf_value_item_read_element_data";

	LIBREGF_UNREFERENCED_PARAMETER( data_range_file_index )
	LIBREGF_UNREFERENCED_PARAMETER( data_range_size )
	LIBREGF_UNREFERENCED_PARAMETER( data_range_flags )
	LIBREGF_UNREFERENCED_PARAMETER( read_flags )

	if( hive_bins_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid hive bins list.",
		 function );

		return( -1 );
	}
	if( hive_bins_list->io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid hive bins list - missing IO handle.",
		 function );

		return( -1 );
	}
	if( data_range_offset > (off64_t) UINT32_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid data range offset value out of bounds.",
		 function );

		goto on_error;
	}
	if( libregf_value_item_initialize(
	     &value_item,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create value item.",
		 function );

		goto on_error;
	}
	if( libregf_value_item_read_value_key(
	     value_item,
	     file_io_handle,
	     hive_bins_list,
	     (uint32_t) data_range_offset,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read value key at offset: %" PRIi64 ".",
		 function,
		 data_range_offset );

		goto on_error;
	}
	if( ( value_item->item_flags & LIBREGF_VALUE_ITEM_FLAG_IS_CORRUPTED ) != 0 )
	{
		hive_bins_list->io_handle->flags |= LIBREGF_IO_HANDLE_FLAG_IS_CORRUPTED;
	}
	if( libfdata_list_element_set_element_value(
	     list_element,
	     (intptr_t *) file_io_handle,
	     cache,
	     (intptr_t *) value_item,
	     (int (*)(intptr_t **, libcerror_error_t **)) &libregf_value_item_free,
	     LIBFDATA_LIST_ELEMENT_VALUE_FLAG_MANAGED,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set value item as element value.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( value_item != NULL )
	{
		libregf_value_item_free(
		 &value_item,
		 NULL );
	}
	return( -1 );
}

/* Compares the value name with UTF-8 string
 * Returns 1 if the names match, 0 if not or -1 on error
 */
int libregf_value_item_compare_name_with_utf8_string(
     libregf_value_item_t *value_item,
     uint32_t name_hash,
     const uint8_t *utf8_string,
     size_t utf8_string_length,
     int ascii_codepage,
     libcerror_error_t **error )
{
	static char *function                       = "libregf_value_item_compare_name_with_utf8_string";
	libuna_unicode_character_t name_character   = 0;
	libuna_unicode_character_t string_character = 0;
	size_t name_index                           = 0;
	size_t utf8_string_index                    = 0;
	int result                                  = 0;

	if( value_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value item.",
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
	/* Do a full compare if there no name hash was provided or the name hash matches
	 */
	if( ( name_hash == 0 )
	 || ( value_item->name_hash == 0 )
	 || ( value_item->name_hash == name_hash ) )
	{
		while( name_index < (size_t) value_item->name_size )
		{
			if( utf8_string_index >= utf8_string_length )
			{
				break;
			}
			if( ( value_item->flags & LIBREGF_VALUE_KEY_FLAG_NAME_IS_ASCII ) != 0 )
			{
				result = libuna_unicode_character_copy_from_byte_stream(
					  &name_character,
					  value_item->name,
					  (size_t) value_item->name_size,
					  &name_index,
					  ascii_codepage,
					  error );
			}
			else
			{
				result = libuna_unicode_character_copy_from_utf16_stream(
					  &name_character,
					  value_item->name,
					  (size_t) value_item->name_size,
					  &name_index,
					  LIBUNA_ENDIAN_LITTLE,
					  error );
			}
			if( result != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy value name to Unicode character.",
				 function );

				return( -1 );
			}
			if( libuna_unicode_character_copy_from_utf8(
			     &string_character,
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
			if( towupper( (wint_t) name_character ) != towupper( (wint_t) string_character ) )
			{
				break;
			}
		}
		if( ( name_index == (size_t) value_item->name_size )
		 && ( utf8_string_index == utf8_string_length ) )
		{
			return( 1 );
		}
	}
	return( 0 );
}

/* Compares the value name with UTF-16 string
 * Returns 1 if the names match, 0 if not or -1 on error
 */
int libregf_value_item_compare_name_with_utf16_string(
     libregf_value_item_t *value_item,
     uint32_t name_hash,
     const uint16_t *utf16_string,
     size_t utf16_string_length,
     int ascii_codepage,
     libcerror_error_t **error )
{
	static char *function                       = "libregf_value_item_compare_name_with_utf16_string";
	libuna_unicode_character_t name_character   = 0;
	libuna_unicode_character_t string_character = 0;
	size_t name_index                           = 0;
	size_t utf16_string_index                   = 0;
	int result                                  = 0;

	if( value_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value item.",
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
	/* Do a full compare if there no name hash was provided or the name hash matches
	 */
	if( ( name_hash == 0 )
	 || ( value_item->name_hash == 0 )
	 || ( value_item->name_hash == name_hash ) )
	{
		while( name_index < (size_t) value_item->name_size )
		{
			if( utf16_string_index >= utf16_string_length )
			{
				break;
			}
			if( ( value_item->flags & LIBREGF_VALUE_KEY_FLAG_NAME_IS_ASCII ) != 0 )
			{
				result = libuna_unicode_character_copy_from_byte_stream(
					  &name_character,
					  value_item->name,
					  (size_t) value_item->name_size,
					  &name_index,
					  ascii_codepage,
					  error );
			}
			else
			{
				result = libuna_unicode_character_copy_from_utf16_stream(
					  &name_character,
					  value_item->name,
					  (size_t) value_item->name_size,
					  &name_index,
					  LIBUNA_ENDIAN_LITTLE,
					  error );
			}
			if( result != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
				 "%s: unable to copy value name to Unicode character.",
				 function );

				return( -1 );
			}
			if( libuna_unicode_character_copy_from_utf16(
			     &string_character,
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
			if( towupper( (wint_t) name_character ) != towupper( (wint_t) string_character ) )
			{
				break;
			}
		}
		if( ( name_index == (size_t) value_item->name_size )
		 && ( utf16_string_index == utf16_string_length ) )
		{
			return( 1 );
		}
	}
	return( 0 );
}

