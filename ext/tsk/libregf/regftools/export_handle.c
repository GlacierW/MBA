/*
 * Export handle
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
#include <narrow_string.h>
#include <system_string.h>
#include <types.h>
#include <wide_string.h>

#include "export_handle.h"
#include "log_handle.h"
#include "regftools_libcerror.h"
#include "regftools_libclocale.h"
#include "regftools_libregf.h"

#define EXPORT_HANDLE_NOTIFY_STREAM	stdout

/* Creates an export handle
 * Make sure the value export_handle is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int export_handle_initialize(
     export_handle_t **export_handle,
     libcerror_error_t **error )
{
	static char *function = "export_handle_initialize";

	if( export_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid export handle.",
		 function );

		return( -1 );
	}
	if( *export_handle != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid export handle value already set.",
		 function );

		return( -1 );
	}
	*export_handle = memory_allocate_structure(
	                  export_handle_t );

	if( *export_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create export handle.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *export_handle,
	     0,
	     sizeof( export_handle_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear export handle.",
		 function );

		goto on_error;
	}
	if( libregf_file_initialize(
	     &( ( *export_handle )->input_file ),
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to initialize input file.",
		 function );

		goto on_error;
	}
	( *export_handle )->ascii_codepage = LIBREGF_CODEPAGE_WINDOWS_1252;
	( *export_handle )->notify_stream = EXPORT_HANDLE_NOTIFY_STREAM;

	return( 1 );

on_error:
	if( *export_handle != NULL )
	{
		memory_free(
		 *export_handle );

		*export_handle = NULL;
	}
	return( -1 );
}

/* Frees an export handle
 * Returns 1 if successful or -1 on error
 */
int export_handle_free(
     export_handle_t **export_handle,
     libcerror_error_t **error )
{
	static char *function = "export_handle_free";
	int result            = 1;

	if( export_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid export handle.",
		 function );

		return( -1 );
	}
	if( *export_handle != NULL )
	{
		if( libregf_file_free(
		     &( ( *export_handle )->input_file ),
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free input file.",
			 function );

			result = -1;
		}
		memory_free(
		 *export_handle );

		*export_handle = NULL;
	}
	return( result );
}

/* Signals the export handle to abort
 * Returns 1 if successful or -1 on error
 */
int export_handle_signal_abort(
     export_handle_t *export_handle,
     libcerror_error_t **error )
{
	static char *function = "export_handle_signal_abort";

	if( export_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid export handle.",
		 function );

		return( -1 );
	}
	export_handle->abort = 1;

	if( export_handle->input_file != NULL )
	{
		if( libregf_file_signal_abort(
		     export_handle->input_file,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to signal input file to abort.",
			 function );

			return( -1 );
		}
	}
	return( 1 );
}

/* Sets the ascii codepage
 * Returns 1 if successful or -1 on error
 */
int export_handle_set_ascii_codepage(
     export_handle_t *export_handle,
     const system_character_t *string,
     libcerror_error_t **error )
{
	static char *function  = "export_handle_set_ascii_codepage";
	size_t string_length   = 0;
	uint32_t feature_flags = 0;
	int result             = 0;

	if( export_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid export handle.",
		 function );

		return( -1 );
	}
	feature_flags = LIBCLOCALE_CODEPAGE_FEATURE_FLAG_HAVE_KOI8
	              | LIBCLOCALE_CODEPAGE_FEATURE_FLAG_HAVE_WINDOWS;

	string_length = system_string_length(
	                 string );

#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
	result = libclocale_codepage_copy_from_string_wide(
	          &( export_handle->ascii_codepage ),
	          string,
	          string_length,
	          feature_flags,
	          error );
#else
	result = libclocale_codepage_copy_from_string(
	          &( export_handle->ascii_codepage ),
	          string,
	          string_length,
	          feature_flags,
	          error );
#endif
	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to determine ASCII codepage.",
		 function );

		return( -1 );
	}
	return( result );
}

/* Prints the data on the stream
 * Returns the number of printed characters if successful or -1 on error
 */
int export_handle_print_data(
     export_handle_t *export_handle,
     FILE *stream,
     const uint8_t *data,
     size_t data_size,
     libcerror_error_t **error )
{
	static char *function = "export_handle_print_data";
	size_t byte_iterator  = 0;
	size_t data_iterator  = 0;
	int print_count       = 0;
	int total_print_count = 0;

	if( export_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid export handle.",
		 function );

		return( -1 );
	}
	if( stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid stream.",
		 function );

		return( -1 );
	}
	if( data == NULL )
	{
		return( 0 );
	}
	while( data_iterator < data_size )
	{
		while( byte_iterator < data_size )
		{
			if( byte_iterator % 16 == 0 )
			{
				print_count = fprintf(
					       stream,
					       "%.8" PRIzx ": ",
					       byte_iterator );

				if( print_count <= -1 )
				{
					return( -1 );
				}
				total_print_count += print_count;
			}
			print_count = fprintf(
				       stream,
				       "%.2" PRIx8 " ",
				       data[ byte_iterator++ ] );

			if( print_count <= -1 )
			{
				return( -1 );
			}
			total_print_count += print_count;

			if( byte_iterator % 16 == 0 )
			{
				break;
			}
			else if( byte_iterator % 8 == 0 )
			{
				print_count = fprintf(
					       stream,
					       " " );

				if( print_count <= -1 )
				{
					return( -1 );
				}
				total_print_count += print_count;
			}
		}
		while( byte_iterator % 16 != 0 )
		{
			byte_iterator++;

			print_count = fprintf(
				       stream,
				       "   " );

			if( print_count <= -1 )
			{
				return( -1 );
			}
			total_print_count += print_count;

			if( ( byte_iterator % 8 == 0 )
			 && ( byte_iterator % 16 != 0 ) )
			{
				print_count = fprintf(
					       stream,
					       " " );

				if( print_count <= -1 )
				{
					return( -1 );
				}
				total_print_count += print_count;
			}
		}
		print_count = fprintf(
			       stream,
			       "  " );

		if( print_count <= -1 )
		{
			return( -1 );
		}
		total_print_count += print_count;

		byte_iterator = data_iterator;

		while( byte_iterator < data_size )
		{
			if( ( data[ byte_iterator ] >= 0x20 )
			 && ( data[ byte_iterator ] <= 0x7e ) )
			{
				print_count = fprintf(
					       stream,
					       "%c",
					       (char) data[ byte_iterator ] );
			}
			else
			{
				print_count = fprintf(
					       stream,
					       "." );
			}
			if( print_count <= -1 )
			{
				return( -1 );
			}
			total_print_count += print_count;

			byte_iterator++;

			if( byte_iterator % 16 == 0 )
			{
				break;
			}
			else if( byte_iterator % 8 == 0 )
			{
				print_count = fprintf(
					       stream,
					       " " );

				if( print_count <= -1 )
				{
					return( -1 );
				}
				total_print_count += print_count;
			}
		}
		print_count = fprintf(
			       stream,
			       "\n" );

		if( print_count <= -1 )
		{
			return( -1 );
		}
		total_print_count += print_count;

		data_iterator = byte_iterator;
	}
	print_count = fprintf(
		       stream,
		       "\n" );

	if( print_count <= -1 )
	{
		return( -1 );
	}
	total_print_count += print_count;

	return( total_print_count );
}

/* Opens the input
 * Returns 1 if successful or -1 on error
 */
int export_handle_open_input(
     export_handle_t *export_handle,
     const system_character_t *filename,
     libcerror_error_t **error )
{
	static char *function = "export_handle_open_input";

	if( export_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid export handle.",
		 function );

		return( -1 );
	}
	if( libregf_file_set_ascii_codepage(
	     export_handle->input_file,
	     export_handle->ascii_codepage,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set ASCII codepage in input file.",
		 function );

		return( -1 );
	}
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
	if( libregf_file_open_wide(
	     export_handle->input_file,
	     filename,
	     LIBREGF_OPEN_READ,
	     error ) != 1 )
#else
	if( libregf_file_open(
	     export_handle->input_file,
	     filename,
	     LIBREGF_OPEN_READ,
	     error ) != 1 )
#endif
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_OPEN_FAILED,
		 "%s: unable to open input file.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Closes the input
 * Returns the 0 if succesful or -1 on error
 */
int export_handle_close_input(
     export_handle_t *export_handle,
     libcerror_error_t **error )
{
	static char *function = "export_handle_close_input";

	if( export_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid export handle.",
		 function );

		return( -1 );
	}
	if( libregf_file_close(
	     export_handle->input_file,
	     error ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_CLOSE_FAILED,
		 "%s: unable to close input file.",
		 function );

		return( -1 );
	}
	return( 0 );
}

/* Exports a key
 * Returns the 1 if succesful or -1 on error
 */
int export_handle_export_key(
     export_handle_t *export_handle,
     libregf_key_t *key,
     log_handle_t *log_handle,
     libcerror_error_t **error )
{
	system_character_t *value_string = NULL;
	libregf_key_t *sub_key           = NULL;
	libregf_value_t *value           = NULL;
	uint8_t *data                    = NULL;
	static char *function            = "export_handle_export_key";
	size_t data_size                 = 0;
	size_t expected_data_size        = 0;
	size_t value_string_size         = 0;
	ssize_t print_count              = 0;
	uint64_t value_64bit             = 0;
	uint32_t value_32bit             = 0;
	uint32_t value_type              = 0;
	int number_of_sub_keys           = 0;
	int number_of_values             = 0;
	int result                       = 0;
	int sub_key_index                = 0;
	int value_index                  = 0;

	if( export_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid export handle.",
		 function );

		return( -1 );
	}
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
	result = libregf_key_get_utf16_name_size(
	          key,
	          &value_string_size,
	          error );
#else
	result = libregf_key_get_utf8_name_size(
	          key,
	          &value_string_size,
	          error );
#endif
	if( result != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve key name size.",
		 function );

		goto on_error;
	}
	if( value_string_size > 0 )
	{
		if( ( value_string_size > (size_t) SSIZE_MAX )
		 || ( ( sizeof( system_character_t ) * value_string_size ) > (size_t) SSIZE_MAX ) )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
			 "%s: invalid name size value exceeds maximum.",
			 function );

			goto on_error;
		}
		value_string = system_string_allocate(
		                value_string_size );

		if( value_string == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_MEMORY,
			 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to create name string.",
			 function );

			goto on_error;
		}
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
		result = libregf_key_get_utf16_name(
		          key,
		          (uint16_t *) value_string,
		          value_string_size,
		          error );
#else
		result = libregf_key_get_utf8_name(
		          key,
		          (uint8_t *) value_string,
		          value_string_size,
		          error );
#endif
		if( result != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve key name.",
			 function );

			goto on_error;
		}
/* TODO print full key path */
		fprintf(
		 export_handle->notify_stream,
		 "Key: %" PRIs_SYSTEM "\n",
		 value_string );

		memory_free(
		 value_string );

		value_string = NULL;
	}
#ifdef TODO
/* TODO fix this */
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
	result = libregf_key_get_utf16_class_name_size(
	          key,
	          &value_string_size,
	          error );
#else
	result = libregf_key_get_utf8_class_name_size(
	          key,
	          &value_string_size,
	          error );
#endif
	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve the class name size.",
		 function );

		goto on_error;
	}
	if( ( result != 0 )
	 && ( value_string_size > 0 ) )
	{
		if( ( value_string_size > (size_t) SSIZE_MAX )
		 || ( ( sizeof( system_character_t ) * value_string_size ) > (size_t) SSIZE_MAX ) )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
			 "%s: invalid name size value exceeds maximum.",
			 function );

			goto on_error;
		}
		value_string = system_string_allocate(
		                value_string_size );

		if( value_string == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_MEMORY,
			 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to create name string.",
			 function );

			goto on_error;
		}
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
		result = libregf_key_get_utf16_class_name(
		          key,
		          (uint16_t *) value_string,
		          value_string_size,
		          error );
#else
		result = libregf_key_get_utf8_class_name(
		          key,
		          (uint8_t *) value_string,
		          value_string_size,
		          error );
#endif
		if( result != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve class name.",
			 function );

			goto on_error;
		}
		fprintf(
		 export_handle->notify_stream,
		 "Class name: %" PRIs_SYSTEM "\n",
		 value_string );

		memory_free(
		 value_string );

		value_string = NULL;
	}
#endif
	if( libregf_key_get_number_of_values(
	     key,
	     &number_of_values,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of values.",
		 function );

		goto on_error;
	}
	for( value_index = 0;
	     value_index < number_of_values;
	     value_index++ )
	{
		if( libregf_key_get_value(
		     key,
		     value_index,
		     &value,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve value: %d.",
			 function,
			 value_index );

			goto on_error;
		}
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
		result = libregf_value_get_utf16_name_size(
		          value,
		          &value_string_size,
		          error );
#else
		result = libregf_value_get_utf8_name_size(
		          value,
		          &value_string_size,
		          error );
#endif
		if( result != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve value name size.",
			 function );

			goto on_error;
		}
		if( value_string_size > 0 )
		{
			if( ( value_string_size > (size_t) SSIZE_MAX )
			 || ( ( sizeof( system_character_t ) * value_string_size ) > (size_t) SSIZE_MAX ) )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
				 "%s: invalid name size value exceeds maximum.",
				 function );

				goto on_error;
			}
			value_string = system_string_allocate(
			                value_string_size );

			if( value_string == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_MEMORY,
				 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
				 "%s: unable to create name string.",
				 function );

				goto on_error;
			}
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
			result = libregf_value_get_utf16_name(
			          value,
			          (uint16_t *) value_string,
			          value_string_size,
			          error );
#else
			result = libregf_value_get_utf8_name(
			          value,
			          (uint8_t *) value_string,
			          value_string_size,
			          error );
#endif
			if( result != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve value name.",
				 function );

				goto on_error;
			}
			fprintf(
			 export_handle->notify_stream,
			 "Value: %d %" PRIs_SYSTEM "\n",
			 value_index,
			 value_string );

			memory_free(
			 value_string );

			value_string = NULL;
		}
		else
		{
			fprintf(
			 export_handle->notify_stream,
			 "Value: %d (default)\n",
			 value_index );
		}
		if( libregf_value_get_value_type(
		     value,
		     &value_type,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve value type.",
			 function );

			goto on_error;
		}
		switch( value_type )
		{
			case LIBREGF_VALUE_TYPE_UNDEFINED:
				fprintf(
				 export_handle->notify_stream,
				 "Type: undefined (none)\n" );
				break;

			case LIBREGF_VALUE_TYPE_STRING:
				fprintf(
				 export_handle->notify_stream,
				 "Type: string\n" );
				break;

			case LIBREGF_VALUE_TYPE_EXPANDABLE_STRING:
				fprintf(
				 export_handle->notify_stream,
				 "Type: expandable string\n" );
				break;

			case LIBREGF_VALUE_TYPE_BINARY_DATA:
				fprintf(
				 export_handle->notify_stream,
				 "Type: binary data\n" );
				break;

			case LIBREGF_VALUE_TYPE_INTEGER_32BIT_LITTLE_ENDIAN:
				fprintf(
				 export_handle->notify_stream,
				 "Type: 32-bit integer little-endian\n" );
				break;

			case LIBREGF_VALUE_TYPE_INTEGER_32BIT_BIG_ENDIAN:
				fprintf(
				 export_handle->notify_stream,
				 "Type: 32-bit integer big-endian\n" );
				break;

			case LIBREGF_VALUE_TYPE_SYMBOLIC_LINK:
				fprintf(
				 export_handle->notify_stream,
				 "Type: symbolic link\n" );
				break;

			case LIBREGF_VALUE_TYPE_MULTI_VALUE_STRING:
				fprintf(
				 export_handle->notify_stream,
				 "Type: multi-value string\n" );
				break;

			case LIBREGF_VALUE_TYPE_RESOURCE_LIST:
				fprintf(
				 export_handle->notify_stream,
				 "Type: resource list\n" );
				break;

			case LIBREGF_VALUE_TYPE_FULL_RESOURCE_DESCRIPTOR:
				fprintf(
				 export_handle->notify_stream,
				 "Type: full resource descriptor\n" );
				break;

			case LIBREGF_VALUE_TYPE_RESOURCE_REQUIREMENTS_LIST:
				fprintf(
				 export_handle->notify_stream,
				 "Type: resource requirements list\n" );
				break;

			case LIBREGF_VALUE_TYPE_INTEGER_64BIT_LITTLE_ENDIAN:
				fprintf(
				 export_handle->notify_stream,
				 "Type: 64-bit integer little-endian\n" );
				break;

			default:
				fprintf(
				 export_handle->notify_stream,
				 "Type: unknown: 0x%08" PRIx32 "\n",
				 value_type );
				break;
		}
		if( libregf_value_get_value_data_size(
		     value,
		     &data_size,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve value data size.",
			 function );

			goto on_error;
		}
		fprintf(
		 export_handle->notify_stream,
		 "Data size: %" PRIzd "\n",
		 data_size );

		switch( value_type )
		{
			case LIBREGF_VALUE_TYPE_STRING:
			case LIBREGF_VALUE_TYPE_EXPANDABLE_STRING:
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
				result = libregf_value_get_value_utf16_string_size(
					  value,
					  &value_string_size,
					  error );
#else
				result = libregf_value_get_value_utf8_string_size(
					  value,
					  &value_string_size,
					  error );
#endif
				if( result != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve value string size.",
					 function );

					goto on_error;
				}
				if( value_string_size > 0 )
				{
					if( ( value_string_size > (size_t) SSIZE_MAX )
					 || ( ( sizeof( system_character_t ) * value_string_size ) > (size_t) SSIZE_MAX ) )
					{
						libcerror_error_set(
						 error,
						 LIBCERROR_ERROR_DOMAIN_RUNTIME,
						 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
						 "%s: invalid value string size value exceeds maximum.",
						 function );

						goto on_error;
					}
					value_string = system_string_allocate(
							value_string_size );

					if( value_string == NULL )
					{
						libcerror_error_set(
						 error,
						 LIBCERROR_ERROR_DOMAIN_MEMORY,
						 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
						 "%s: unable to create value string.",
						 function );

						goto on_error;
					}
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
					result = libregf_value_get_value_utf16_string(
						  value,
						  (uint16_t *) value_string,
						  value_string_size,
						  error );
#else
					result = libregf_value_get_value_utf8_string(
						  value,
						  (uint8_t *) value_string,
						  value_string_size,
						  error );
#endif
					if( result != 1 )
					{
						libcerror_error_set(
						 error,
						 LIBCERROR_ERROR_DOMAIN_RUNTIME,
						 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
						 "%s: unable to retrieve value string.",
						 function );

						goto on_error;
					}
					fprintf(
					 export_handle->notify_stream,
					 "Data: %" PRIs_SYSTEM "\n",
					 value_string );

					memory_free(
					 value_string );

					value_string = NULL;
				}
				else
				{
					fprintf(
					 export_handle->notify_stream,
					 "Data:\n" );
				}
				result = libregf_value_get_value_utf16_string_size(
					  value,
					  &expected_data_size,
					  error );

				if( result != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve value UTF-16 string size.",
					 function );

					goto on_error;
				}
				expected_data_size *= 2;

				if( expected_data_size == ( data_size + 2 ) )
				{
					expected_data_size -= 2;
				}
				break;

			case LIBREGF_VALUE_TYPE_INTEGER_32BIT_LITTLE_ENDIAN:
			case LIBREGF_VALUE_TYPE_INTEGER_32BIT_BIG_ENDIAN:
				if( data_size == 4 )
				{
					if( libregf_value_get_value_32bit(
					     value,
					     &value_32bit,
					     error ) != 1 )
					{
						libcerror_error_set(
						 error,
						 LIBCERROR_ERROR_DOMAIN_RUNTIME,
						 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
						 "%s: unable to retrieve 32-bit value.",
						 function );

						goto on_error;
					}
					fprintf(
					 export_handle->notify_stream,
					 "Data: %" PRIu32 "\n",
					 value_32bit );
				}
				expected_data_size = 4;

				break;

			case LIBREGF_VALUE_TYPE_INTEGER_64BIT_LITTLE_ENDIAN:
				if( data_size == 8 )
				{
					if( libregf_value_get_value_64bit(
					     value,
					     &value_64bit,
					     error ) != 1 )
					{
						libcerror_error_set(
						 error,
						 LIBCERROR_ERROR_DOMAIN_RUNTIME,
						 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
						 "%s: unable to retrieve 64-bit value.",
						 function );

						goto on_error;
					}
					fprintf(
					 export_handle->notify_stream,
					 "Data: %" PRIu64 "\n",
					 value_64bit );
				}
				expected_data_size = 8;

				break;

			default:
				expected_data_size = 0;

				break;
		}
		if( data_size != expected_data_size )
		{
			if( expected_data_size != 0 )
			{
				fprintf(
				 export_handle->notify_stream,
				 "Mismatch in data size and that required for data type.\n" );
			}
			if( data_size > 0 )
			{
				data = (uint8_t *) memory_allocate(
						    sizeof( uint8_t ) * data_size );

				if( data == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_MEMORY,
					 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
					 "%s: unable to create value data.",
					 function );

					goto on_error;
				}
				if( libregf_value_get_value_data(
				     value,
				     data,
				     data_size,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve value data.",
					 function );

					goto on_error;
				}
				fprintf(
				 export_handle->notify_stream,
				 "Data:\n" );

				print_count = export_handle_print_data(
					       export_handle,
					       export_handle->notify_stream,
					       data,
					       data_size,
					       error );

				if( print_count == -1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_PRINT_FAILED,
					 "%s: unable to print value data.",
					 function );

					goto on_error;
				}
				memory_free(
				 data );

				data = NULL;
			}
			else
			{
				fprintf(
				 export_handle->notify_stream,
				 "\n" );
			}
		}
		else
		{
			fprintf(
			 export_handle->notify_stream,
			 "\n" );
		}
		if( libregf_value_free(
		     &value,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free value: %d.",
			 function,
			 value_index );

			goto on_error;
		}
	}
	if( number_of_values == 0 )
	{
		fprintf(
		 export_handle->notify_stream,
		 "\n" );
	}
	if( libregf_key_get_number_of_sub_keys(
	     key,
	     &number_of_sub_keys,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of sub keys.",
		 function );

		goto on_error;
	}
	for( sub_key_index = 0;
	     sub_key_index < number_of_sub_keys;
	     sub_key_index++ )
	{
		if( export_handle->abort != 0 )
		{
			goto on_error;
		}
		if( libregf_key_get_sub_key(
		     key,
		     sub_key_index,
		     &sub_key,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve sub key: %d.",
			 function,
			 sub_key_index );

			goto on_error;
		}
		if( export_handle_export_key(
		     export_handle,
		     sub_key,
		     log_handle,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GENERIC,
			 "%s: unable to export sub key: %d.",
			 function,
			 sub_key_index );

			goto on_error;
		}
		if( libregf_key_free(
		     &sub_key,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free sub key: %d.",
			 function,
			 sub_key_index );

			goto on_error;
		}
	}
	return( 1 );

on_error:
	if( sub_key != NULL )
	{
		libregf_key_free(
		 &sub_key,
		 NULL );
	}
	if( data != NULL )
	{
		memory_free(
		 data );
	}
	if( value != NULL )
	{
		libregf_value_free(
		 &value,
		 NULL );
	}
	if( value_string != NULL )
	{
		memory_free(
		 value_string );
	}
	return( -1 );
}

/* Exports the keys and values from the file
 * Returns the 1 if succesful or -1 on error
 */
int export_handle_export_file(
     export_handle_t *export_handle,
     log_handle_t *log_handle,
     libcerror_error_t **error )
{
	libregf_key_t *root_key = NULL;
	static char *function   = "export_handle_export_file";
	int result              = 0;

	if( export_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid export handle.",
		 function );

		return( -1 );
	}
	result = libregf_file_get_root_key(
	          export_handle->input_file,
	          &root_key,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve root key.",
		 function );

		goto on_error;
	}
	else if( result != 0 )
	{
		if( export_handle_export_key(
		     export_handle,
		     root_key,
		     log_handle,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GENERIC,
			 "%s: unable to export root key.",
			 function );

			goto on_error;
		}
		if( libregf_key_free(
		     &root_key,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free root key.",
			 function );

			goto on_error;
		}
		fprintf(
		 export_handle->notify_stream,
		 "\n" );
	}
	return( 1 );

on_error:
	if( root_key != NULL )
	{
		libregf_key_free(
		 &root_key,
		 NULL );
	}
	return( -1 );
}

