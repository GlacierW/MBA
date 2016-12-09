/*
 * Info handle
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
#include <file_stream.h>
#include <memory.h>
#include <narrow_string.h>
#include <system_string.h>
#include <types.h>
#include <wide_string.h>

#include "info_handle.h"
#include "regftools_libcerror.h"
#include "regftools_libclocale.h"
#include "regftools_libregf.h"

#define INFO_HANDLE_NOTIFY_STREAM	stdout

/* Creates an info handle
 * Make sure the value info_handle is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int info_handle_initialize(
     info_handle_t **info_handle,
     libcerror_error_t **error )
{
	static char *function = "info_handle_initialize";

	if( info_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid info handle.",
		 function );

		return( -1 );
	}
	if( *info_handle != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid info handle value already set.",
		 function );

		return( -1 );
	}
	*info_handle = memory_allocate_structure(
	                info_handle_t );

	if( *info_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create info handle.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *info_handle,
	     0,
	     sizeof( info_handle_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear info handle.",
		 function );

		goto on_error;
	}
	if( libregf_file_initialize(
	     &( ( *info_handle )->input_file ),
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
	( *info_handle )->ascii_codepage = LIBREGF_CODEPAGE_WINDOWS_1252;
	( *info_handle )->notify_stream  = INFO_HANDLE_NOTIFY_STREAM;

	return( 1 );

on_error:
	if( *info_handle != NULL )
	{
		memory_free(
		 *info_handle );

		*info_handle = NULL;
	}
	return( -1 );
}

/* Frees an info handle
 * Returns 1 if successful or -1 on error
 */
int info_handle_free(
     info_handle_t **info_handle,
     libcerror_error_t **error )
{
	static char *function = "info_handle_free";
	int result            = 1;

	if( info_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid info handle.",
		 function );

		return( -1 );
	}
	if( *info_handle != NULL )
	{
		if( libregf_file_free(
		     &( ( *info_handle )->input_file ),
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
		 *info_handle );

		*info_handle = NULL;
	}
	return( result );
}

/* Signals the info handle to abort
 * Returns 1 if successful or -1 on error
 */
int info_handle_signal_abort(
     info_handle_t *info_handle,
     libcerror_error_t **error )
{
	static char *function = "info_handle_signal_abort";

	if( info_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid info handle.",
		 function );

		return( -1 );
	}
	info_handle->abort = 1;

	if( info_handle->input_file != NULL )
	{
		if( libregf_file_signal_abort(
		     info_handle->input_file,
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
int info_handle_set_ascii_codepage(
     info_handle_t *info_handle,
     const system_character_t *string,
     libcerror_error_t **error )
{
	static char *function  = "info_handle_set_ascii_codepage";
	size_t string_length   = 0;
	uint32_t feature_flags = 0;
	int result             = 0;

	if( info_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid info handle.",
		 function );

		return( -1 );
	}
	feature_flags = LIBCLOCALE_CODEPAGE_FEATURE_FLAG_HAVE_KOI8
	              | LIBCLOCALE_CODEPAGE_FEATURE_FLAG_HAVE_WINDOWS;

	string_length = system_string_length(
	                 string );

#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
	result = libclocale_codepage_copy_from_string_wide(
	          &( info_handle->ascii_codepage ),
	          string,
	          string_length,
	          feature_flags,
	          error );
#else
	result = libclocale_codepage_copy_from_string(
	          &( info_handle->ascii_codepage ),
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

/* Opens the input
 * Returns 1 if successful or -1 on error
 */
int info_handle_open_input(
     info_handle_t *info_handle,
     const system_character_t *filename,
     libcerror_error_t **error )
{
	static char *function = "info_handle_open_input";

	if( info_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid info handle.",
		 function );

		return( -1 );
	}
	if( libregf_file_set_ascii_codepage(
	     info_handle->input_file,
	     info_handle->ascii_codepage,
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
	     info_handle->input_file,
	     filename,
	     LIBREGF_OPEN_READ,
	     error ) != 1 )
#else
	if( libregf_file_open(
	     info_handle->input_file,
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
int info_handle_close_input(
     info_handle_t *info_handle,
     libcerror_error_t **error )
{
	static char *function = "info_handle_close_input";

	if( info_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid info handle.",
		 function );

		return( -1 );
	}
	if( libregf_file_close(
	     info_handle->input_file,
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

/* Prints key information
 * Returns 1 if successful or -1 on error
 */
int info_handle_key_fprint(
     info_handle_t *info_handle,
     libregf_key_t *key,
     int indentation_level,
     libregf_error_t **error )
{
	system_character_t *name       = NULL;
	libregf_key_t *sub_key         = NULL;
	libregf_value_t *value         = NULL;
	static char *function          = "info_handle_key_fprint";
	size_t name_size               = 0;
	int indentation_level_iterator = 0;
	int number_of_sub_keys         = 0;
	int number_of_values           = 0;
	int result                     = 0;
	int sub_key_index              = 0;
	int value_index                = 0;

	if( info_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid info handle.",
		 function );

		return( -1 );
	}
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
	result = libregf_key_get_utf16_name_size(
	          key,
	          &name_size,
	          error );
#else
	result = libregf_key_get_utf8_name_size(
	          key,
	          &name_size,
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
	if( name_size > 0 )
	{
		if( ( name_size > (size_t) SSIZE_MAX )
		 || ( ( sizeof( system_character_t ) * name_size ) > (size_t) SSIZE_MAX ) )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
			 "%s: invalid name size value exceeds maximum.",
			 function );

			goto on_error;
		}
		name = system_string_allocate(
		        name_size );

		if( name == NULL )
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
		          (uint16_t *) name,
		          name_size,
		          error );
#else
		result = libregf_key_get_utf8_name(
		          key,
		          (uint8_t *) name,
		          name_size,
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
		for( indentation_level_iterator = 0;
		     indentation_level_iterator < indentation_level;
		     indentation_level_iterator++ )
		{
			fprintf(
			 info_handle->notify_stream,
			 " " );
		}
		fprintf(
		 info_handle->notify_stream,
		 "(key:) %" PRIs_SYSTEM "\n",
		 name );

		memory_free(
		 name );

		name = NULL;
	}
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
		          &name_size,
		          error );
#else
		result = libregf_value_get_utf8_name_size(
		          value,
		          &name_size,
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
		for( indentation_level_iterator = 0;
		     indentation_level_iterator < indentation_level + 1;
		     indentation_level_iterator++ )
		{
			fprintf(
			 info_handle->notify_stream,
			 " " );
		}
		if( name_size > 0 )
		{
			if( ( name_size > (size_t) SSIZE_MAX )
			 || ( ( sizeof( system_character_t ) * name_size ) > (size_t) SSIZE_MAX ) )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
				 "%s: invalid name size value exceeds maximum.",
				 function );

				goto on_error;
			}
			name = system_string_allocate(
			        name_size );

			if( name == NULL )
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
			          (uint16_t *) name,
			          name_size,
			          error );
#else
			result = libregf_value_get_utf8_name(
			          value,
			          (uint8_t *) name,
			          name_size,
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
			 info_handle->notify_stream,
			 "(value: %d) %" PRIs_SYSTEM "\n",
			 value_index,
			 name );

			memory_free(
			 name );

			name = NULL;
		}
		else
		{
			fprintf(
			 info_handle->notify_stream,
			 "(value: %d) (default)\n",
			 value_index );
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
		if( info_handle_key_fprint(
		     info_handle,
		     sub_key,
		     indentation_level + 1,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_PRINT_FAILED,
			 "%s: unable to print sub key: %d info.",
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
	if( value != NULL )
	{
		libregf_value_free(
		 &value,
		 NULL );
	}
	if( name != NULL )
	{
		memory_free(
		 name );
	}
	return( -1 );
}

/* Prints the file information
 * Returns 1 if successful or -1 on error
 */
int info_handle_file_fprint(
     info_handle_t *info_handle,
     libcerror_error_t **error )
{
	libregf_key_t *root_key = NULL;
	static char *function   = "info_handle_file_fprint";
	uint32_t file_type      = 0;
	uint32_t major_version  = 0;
	uint32_t minor_version  = 0;
	int is_corrupted        = 0;
	int result              = 0;

	if( info_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid info handle.",
		 function );

		return( -1 );
	}
	if( libregf_file_get_format_version(
	     info_handle->input_file,
	     &major_version,
	     &minor_version,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve format version.",
		 function );

		goto on_error;
	}
	if( libregf_file_get_type(
	     info_handle->input_file,
	     &file_type,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve file type.",
		 function );

		goto on_error;
	}
	fprintf(
	 info_handle->notify_stream,
	 "Windows NT Registry File information:\n" );

	fprintf(
	 info_handle->notify_stream,
	 "\tVersion:\t%" PRIu32 ".%" PRIu32 "\n",
	 major_version,
	 minor_version );

	fprintf(
	 info_handle->notify_stream,
	 "\tFile type:\t" );

	if( file_type == LIBREGF_FILE_TYPE_REGISTRY )
	{
		fprintf(
		 info_handle->notify_stream,
		 "Registry" );
	}
	else if( file_type == LIBREGF_FILE_TYPE_TRANSACTION_LOG )
	{
		fprintf(
		 info_handle->notify_stream,
		 "Transaction log" );
	}
	else
	{
		fprintf(
		 info_handle->notify_stream,
		 "Unknown: 0x%08" PRIx32 "",
		 file_type );
	}
	fprintf(
	 info_handle->notify_stream,
	 "\n" );

/* TODO calculate number of keys and values */

	fprintf(
	 info_handle->notify_stream,
	 "\n" );

	result = libregf_file_get_root_key(
	          info_handle->input_file,
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
		fprintf(
		 info_handle->notify_stream,
		 "Key hierarchy\n" );

		if( info_handle_key_fprint(
		     info_handle,
		     root_key,
		     0,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_PRINT_FAILED,
			 "%s: unable to print root key info.",
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
		 info_handle->notify_stream,
		 "\n" );
	}
	is_corrupted = libregf_file_is_corrupted(
	                info_handle->input_file,
	                error );

	if( is_corrupted == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to determine if file is corrupted.",
		 function );

		return( -1 );
	}
	if( is_corrupted != 0 )
	{
		fprintf(
		 info_handle->notify_stream,
		 "File is corrupted\n\n" );
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










/* Prints key information
 * Returns 1 if successful or -1 on error
 */
int info_handle_key_fprint_search_key(
		info_handle_t *info_handle,
		libregf_key_t *key,
		int indentation_level,
		system_character_t *keyName,
        system_character_t *findKeyPath,
		int hasKey,
		libregf_error_t **error )
{
	system_character_t *name = NULL;
    system_character_t *nextName = NULL;
	libregf_key_t *sub_key              = NULL;
	libregf_value_t *value              = NULL;
	static char *function               = "info_handle_key_fprint";
	size_t name_size                    = 0;
	int indentation_level_iterator      = 0;
	int number_of_sub_keys              = 0;
	int number_of_values                = 0;
	int result                          = 0;
	int sub_key_index                   = 0;
	int value_index                     = 0;

	if( info_handle == NULL )
	{
		libcerror_error_set(
				error,
				LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
				"%s: invalid info handle.",
				function );

		return( -1 );
	}
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
	result = libregf_key_get_utf16_name_size(
	          key,
	          &name_size,
	          error );
#else
	result = libregf_key_get_utf8_name_size(
			key,
			&name_size,
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
	if( name_size > 0 )
	{
		if( ( name_size > (size_t) SSIZE_MAX )
			|| ( ( sizeof( system_character_t ) * name_size ) > (size_t) SSIZE_MAX ) )
		{
			libcerror_error_set(
					error,
					LIBCERROR_ERROR_DOMAIN_RUNTIME,
					LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
					"%s: invalid name size value exceeds maximum.",
					function );

			goto on_error;
		}
		name = system_string_allocate(
				name_size );
        nextName = system_string_allocate(
                name_size );
		if( name == NULL )
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
		          (uint16_t *) name,
		          name_size,
		          error );
#else
		result = libregf_key_get_utf8_name(
				key,
				(uint8_t *) name,
				name_size,
				error );
        strcpy( nextName, name );
        // printf( "nextName:%s\n", nextName );
		if ( strcmp( keyName, name ) == 0 ) {
			hasKey = 1;
		}
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
		if ( hasKey == 1 ) {
			for (indentation_level_iterator = 0;
				 indentation_level_iterator < indentation_level;
				 indentation_level_iterator++) {
				fprintf(
						info_handle->notify_stream,
						" ");
			}

			fprintf(
					info_handle->notify_stream,
					"(key:) %s\\%"PRIs_SYSTEM"\n"

			        , findKeyPath, name );

		} // if

		memory_free(
				name );

		name = NULL;
	}
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
	if ( hasKey == 1 ) {
		for (value_index = 0;
			 value_index < number_of_values;
			 value_index++) {
			if (libregf_key_get_value(
					key,
					value_index,
					&value,
					error) != 1) {
				libcerror_error_set(
						error,
						LIBCERROR_ERROR_DOMAIN_RUNTIME,
						LIBCERROR_RUNTIME_ERROR_GET_FAILED,
						"%s: unable to retrieve value: %d.",
						function,
						value_index);

				goto on_error;
			}
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
            result = libregf_value_get_utf16_name_size(
                      value,
                      &name_size,
                      error );
#else
			result = libregf_value_get_utf8_name_size(
					value,
					&name_size,
					error);
#endif
			if (result != 1) {
				libcerror_error_set(
						error,
						LIBCERROR_ERROR_DOMAIN_RUNTIME,
						LIBCERROR_RUNTIME_ERROR_GET_FAILED,
						"%s: unable to retrieve value name size.",
						function);

				goto on_error;
			}
			for (indentation_level_iterator = 0;
				 indentation_level_iterator < indentation_level + 1;
				 indentation_level_iterator++) {
				fprintf(
						info_handle->notify_stream,
						" ");
			}
			if (name_size > 0) {
				if ((name_size > (size_t) SSIZE_MAX)
					|| ((sizeof(system_character_t) * name_size) > (size_t) SSIZE_MAX)) {
					libcerror_error_set(
							error,
							LIBCERROR_ERROR_DOMAIN_RUNTIME,
							LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
							"%s: invalid name size value exceeds maximum.",
							function);

					goto on_error;
				}
				name = system_string_allocate(
						name_size);

				if (name == NULL) {
					libcerror_error_set(
							error,
							LIBCERROR_ERROR_DOMAIN_MEMORY,
							LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
							"%s: unable to create name string.",
							function);

					goto on_error;
				}
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
                result = libregf_value_get_utf16_name(
                          value,
                          (uint16_t *) name,
                          name_size,
                          error );
#else
				result = libregf_value_get_utf8_name(
						value,
						(uint8_t *) name,
						name_size,
						error);
#endif
				if (result != 1) {
					libcerror_error_set(
							error,
							LIBCERROR_ERROR_DOMAIN_RUNTIME,
							LIBCERROR_RUNTIME_ERROR_GET_FAILED,
							"%s: unable to retrieve value name.",
							function);

					goto on_error;
				}

				fprintf(
						info_handle->notify_stream,
						"(value: %d) %"PRIs_SYSTEM"\n",
						value_index, name );

				memory_free(
						name);

				name = NULL;
			}

			else
			{
				if ( hasKey == 1 ) {
					fprintf(
							info_handle->notify_stream,
							"(value: %d) (default)\n",
							value_index);
				} // if
			}

			if (libregf_value_free(
					&value,
					error) != 1) {
				libcerror_error_set(
						error,
						LIBCERROR_ERROR_DOMAIN_RUNTIME,
						LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
						"%s: unable to free value: %d.",
						function,
						value_index);

				goto on_error;
			}
		}
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
		if( libregf_key_get_sub_key(
				key,
				sub_key_index,
				&sub_key,
				error ) != 1 ) {
            libcerror_error_set(
                    error,
                    LIBCERROR_ERROR_DOMAIN_RUNTIME,
                    LIBCERROR_RUNTIME_ERROR_GET_FAILED,
                    "%s: unable to retrieve sub key: %d.",
                    function,
                    sub_key_index);

            goto on_error;
        }

        char * tempBuffer ;
        tempBuffer = calloc(1000, sizeof(char));
        strcpy(tempBuffer, findKeyPath );
        strcat(tempBuffer, "\\" );
        strcat(tempBuffer, nextName );
        //findKeyPath = tempBuffer;
        //printf("strcat Boom\n%s %s\n", findKeyPath, nextName);
        // findKeyPath = nextName ;
        //printf("strcat Save\n%s %s\n", findKeyPath, nextName);
		if( info_handle_key_fprint_search_key(
				info_handle,
				sub_key,
				indentation_level + 1,
				keyName,
                tempBuffer,
				hasKey,
				error ) != 1 )
		{
			libcerror_error_set(
					error,
					LIBCERROR_ERROR_DOMAIN_RUNTIME,
					LIBCERROR_RUNTIME_ERROR_PRINT_FAILED,
					"%s: unable to print sub key: %d info.",
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
	if( value != NULL )
	{
		libregf_value_free(
				&value,
				NULL );
	}
	if( name != NULL )
	{
		memory_free(
				name );
	}
	return( -1 );
}


int info_handle_file_fprint_search_key(
		info_handle_t *info_handle,
		system_character_t *keyName,
        system_character_t *path,
		libcerror_error_t **error )
{
	libregf_key_t *root_key = NULL;
	static char *function   = "info_handle_file_fprint";
	uint32_t file_type      = 0;
	uint32_t major_version  = 0;
	uint32_t minor_version  = 0;
	int is_corrupted        = 0;
	int result              = 0;

	if( info_handle == NULL )
	{
		libcerror_error_set(
				error,
				LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
				"%s: invalid info handle.",
				function );

		return( -1 );
	}
	if( libregf_file_get_format_version(
			info_handle->input_file,
			&major_version,
			&minor_version,
			error ) != 1 )
	{
		libcerror_error_set(
				error,
				LIBCERROR_ERROR_DOMAIN_RUNTIME,
				LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				"%s: unable to retrieve format version.",
				function );

		goto on_error;
	}
	if( libregf_file_get_type(
			info_handle->input_file,
			&file_type,
			error ) != 1 )
	{
		libcerror_error_set(
				error,
				LIBCERROR_ERROR_DOMAIN_RUNTIME,
				LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				"%s: unable to retrieve file type.",
				function );

		goto on_error;
	}


/* TODO calculate number of keys and values */

	result = libregf_file_get_root_key(
			info_handle->input_file,
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
		if( info_handle_key_fprint_search_key(
				info_handle,
				root_key,
				0,
				keyName,
                path,
				0,
				error ) != 1 )
		{
			libcerror_error_set(
					error,
					LIBCERROR_ERROR_DOMAIN_RUNTIME,
					LIBCERROR_RUNTIME_ERROR_PRINT_FAILED,
					"%s: unable to print root key info.",
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
				info_handle->notify_stream,
				"\n" );
	}
	is_corrupted = libregf_file_is_corrupted(
			info_handle->input_file,
			error );

	if( is_corrupted == -1 )
	{
		libcerror_error_set(
				error,
				LIBCERROR_ERROR_DOMAIN_RUNTIME,
				LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				"%s: unable to determine if file is corrupted.",
				function );

		return( -1 );
	}
	if( is_corrupted != 0 )
	{
		fprintf(
				info_handle->notify_stream,
				"File is corrupted\n\n" );
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



















int info_handle_key_fprint_registry_path(
     info_handle_t *info_handle,
     libregf_key_t *key,
     int indentation_level,
     StrArray keyPath,
     int keyPathLen,
     libregf_error_t **error )
{
	system_character_t *name = NULL;
        system_character_t *nametemp = NULL;
	libregf_key_t *sub_key              = NULL;
	libregf_value_t *value              = NULL;
	static char *function               = "info_handle_key_fprint";
	size_t name_size                    = 0;
	int indentation_level_iterator      = 0;
	int number_of_sub_keys              = 0;
	int number_of_values                = 0;
	int result                          = 0;
	int sub_key_index                   = 0;
	int value_index                     = 0;

	if( info_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid info handle.",
		 function );

		return( -1 );
	}
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
	result = libregf_key_get_utf16_name_size(
	          key,
	          &name_size,
	          error );
#else
	result = libregf_key_get_utf8_name_size(
	          key,
	          &name_size,
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
	if( name_size > 0 )
	{
		if( ( name_size > (size_t) SSIZE_MAX )
		 || ( ( sizeof( system_character_t ) * name_size ) > (size_t) SSIZE_MAX ) )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
			 "%s: invalid name size value exceeds maximum.",
			 function );

			goto on_error;
		}
		name = system_string_allocate(
		        name_size );              
		if( name == NULL )
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
		          (uint16_t *) name,
		          name_size,
		          error );
#else
		result = libregf_key_get_utf8_name(
		          key,
		          (uint8_t *) name,
		          name_size,
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
		for( indentation_level_iterator = 0;
		     indentation_level_iterator < indentation_level;
		     indentation_level_iterator++ )
		{
                    if ( indentation_level != 0 && indentation_level >= keyPathLen ) {
                        if ( indentation_level == keyPathLen)
                            fprintf(info_handle->notify_stream, "\n" );

			fprintf(
			 info_handle->notify_stream,
			 " " );
                    } // if                    
		}
                if ( indentation_level != 0 && indentation_level >= keyPathLen ) {
		    fprintf(
		     info_handle->notify_stream,
		     "(key:) %" PRIs_SYSTEM "\n",
		     name );
                }
               
		memory_free(
		 name );

		name = NULL;
	}
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
		          &name_size,
		          error );
#else
		result = libregf_value_get_utf8_name_size(
		          value,
		          &name_size,
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
		for( indentation_level_iterator = 0;
		     indentation_level_iterator < indentation_level + 1;
		     indentation_level_iterator++ )
		{
			fprintf(
			 info_handle->notify_stream,
			 " " );
		}
		if( name_size > 0 )
		{
			if( ( name_size > (size_t) SSIZE_MAX )
			 || ( ( sizeof( system_character_t ) * name_size ) > (size_t) SSIZE_MAX ) )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
				 "%s: invalid name size value exceeds maximum.",
				 function );

				goto on_error;
			}
			name = system_string_allocate(
			        name_size );

			if( name == NULL )
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
			          (uint16_t *) name,
			          name_size,
			          error );
#else
			result = libregf_value_get_utf8_name(
			          value,
			          (uint8_t *) name,
			          name_size,
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

                        if ( indentation_level != 0 && indentation_level >= keyPathLen ) {
			fprintf(
			     info_handle->notify_stream,
			     "(value: %d) %" PRIs_SYSTEM "\n",
			     value_index,
			     name );
                         } // if

			memory_free(
			 name );

			name = NULL;
		}
		else
		{   
                    if ( indentation_level != 0 && indentation_level >= keyPathLen ) {                   
		        fprintf(
		        info_handle->notify_stream,
		        "(value: %d) (default)\n",
		        value_index );
                    } // if
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

/*
 *  Getting subkey namesize and name
 */
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
        result = libregf_key_get_utf16_name_size(
	              sub_key,
	              &name_size,
	              error );
#else
        result = libregf_key_get_utf8_name_size(
	              sub_key,
	              &name_size,
	              error );
#endif
                nametemp = system_string_allocate(
		            name_size );
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
		result = libregf_key_get_utf16_name(
		          sub_key,
		          (uint16_t *) nametemp,
		          name_size,
		          error );
#else
		result = libregf_key_get_utf8_name(
		          sub_key,
		          (uint8_t *) nametemp,
		          name_size,
		          error );
#endif
                // printf("get nametemp:%s\n", nametemp );
        if ( ( indentation_level < keyPathLen && strcmp( keyPath[indentation_level], nametemp ) == 0 ) || indentation_level >= keyPathLen ) {
		    if( info_handle_key_fprint_registry_path(
		         info_handle,
		         sub_key,
		         indentation_level + 1,
                         keyPath,
                         keyPathLen,
		         error ) != 1 ) {
		 	    libcerror_error_set(
			     error,
			     LIBCERROR_ERROR_DOMAIN_RUNTIME,
			     LIBCERROR_RUNTIME_ERROR_PRINT_FAILED,
			     "%s: unable to print sub key: %d info.",
			     function,
			     sub_key_index );

			    goto on_error;
		    } // if
        } // if
/*
        else {
            printf( "keypath:%s  name:%s  indentation_level:%d\n", keyPath[indentation_level+1], nametemp, indentation_level );
        } // else
*/
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
	if( value != NULL )
	{
		libregf_value_free(
		 &value,
		 NULL );
	}
	if( name != NULL )
	{
		memory_free(
		 name );
	}
	return( -1 );
}

/* Prints the file information
 * Returns 1 if successful or -1 on error
 */
int info_handle_file_fprint_registry_path(
     info_handle_t *info_handle,
     StrArray keyPath,
     int keyPathLen,
     libcerror_error_t **error )
{
	libregf_key_t *root_key = NULL;
	static char *function   = "info_handle_file_fprint";
	uint32_t file_type      = 0;
	uint32_t major_version  = 0;
	uint32_t minor_version  = 0;
	int is_corrupted        = 0;
	int result              = 0;

	if( info_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid info handle.",
		 function );

		return( -1 );
	}
	if( libregf_file_get_format_version(
	     info_handle->input_file,
	     &major_version,
	     &minor_version,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve format version.",
		 function );

		goto on_error;
	}
	if( libregf_file_get_type(
	     info_handle->input_file,
	     &file_type,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve file type.",
		 function );

		goto on_error;
	}

/* TODO calculate number of keys and values */


	result = libregf_file_get_root_key(
	          info_handle->input_file,
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
		if( info_handle_key_fprint_registry_path(
		     info_handle,
		     root_key,
		     0,
                     keyPath,
                     keyPathLen,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_PRINT_FAILED,
			 "%s: unable to print root key info.",
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
		 info_handle->notify_stream,
		 "\n" );
	}
	is_corrupted = libregf_file_is_corrupted(
	                info_handle->input_file,
	                error );

	if( is_corrupted == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to determine if file is corrupted.",
		 function );

		return( -1 );
	}
	if( is_corrupted != 0 )
	{
		fprintf(
		 info_handle->notify_stream,
		 "File is corrupted\n\n" );
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
