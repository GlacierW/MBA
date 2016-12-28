/*
 * Mount handle
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
#include <system_string.h>
#include <types.h>

#include "mount_handle.h"
#include "regftools_libcerror.h"
#include "regftools_libclocale.h"
#include "regftools_libregf.h"

/* Creates a mount handle
 * Make sure the value mount_handle is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int mount_handle_initialize(
     mount_handle_t **mount_handle,
     libcerror_error_t **error )
{
	static char *function = "mount_handle_initialize";

	if( mount_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid mount handle.",
		 function );

		return( -1 );
	}
	if( *mount_handle != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid mount handle value already set.",
		 function );

		return( -1 );
	}
	*mount_handle = memory_allocate_structure(
	                 mount_handle_t );

	if( *mount_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create mount handle.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *mount_handle,
	     0,
	     sizeof( mount_handle_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear mount handle.",
		 function );

		goto on_error;
	}
	if( libregf_file_initialize(
	     &( ( *mount_handle )->input_file ),
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
	( *mount_handle )->ascii_codepage = LIBREGF_CODEPAGE_WINDOWS_1252;

	return( 1 );

on_error:
	if( *mount_handle != NULL )
	{
		memory_free(
		 *mount_handle );

		*mount_handle = NULL;
	}
	return( -1 );
}

/* Frees a mount handle
 * Returns 1 if successful or -1 on error
 */
int mount_handle_free(
     mount_handle_t **mount_handle,
     libcerror_error_t **error )
{
	static char *function = "mount_handle_free";
	int result            = 1;

	if( mount_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid mount handle.",
		 function );

		return( -1 );
	}
	if( *mount_handle != NULL )
	{
		if( libregf_file_free(
		     &( ( *mount_handle )->input_file ),
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
		 *mount_handle );

		*mount_handle = NULL;
	}
	return( result );
}

/* Signals the mount handle to abort
 * Returns 1 if successful or -1 on error
 */
int mount_handle_signal_abort(
     mount_handle_t *mount_handle,
     libcerror_error_t **error )
{
	static char *function = "mount_handle_signal_abort";

	if( mount_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid mount handle.",
		 function );

		return( -1 );
	}
	if( mount_handle->input_file != NULL )
	{
		if( libregf_file_signal_abort(
		     mount_handle->input_file,
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
int mount_handle_set_ascii_codepage(
     mount_handle_t *mount_handle,
     const system_character_t *string,
     libcerror_error_t **error )
{
	static char *function  = "mount_handle_set_ascii_codepage";
	size_t string_length   = 0;
	uint32_t feature_flags = 0;
	int result             = 0;

	if( mount_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid mount handle.",
		 function );

		return( -1 );
	}
	feature_flags = LIBCLOCALE_CODEPAGE_FEATURE_FLAG_HAVE_KOI8
	              | LIBCLOCALE_CODEPAGE_FEATURE_FLAG_HAVE_WINDOWS;

	string_length = system_string_length(
	                 string );

#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
	result = libclocale_codepage_copy_from_string_wide(
	          &( mount_handle->ascii_codepage ),
	          string,
	          string_length,
	          feature_flags,
	          error );
#else
	result = libclocale_codepage_copy_from_string(
	          &( mount_handle->ascii_codepage ),
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

/* Opens the input of the mount handle
 * Returns 1 if successful or -1 on error
 */
int mount_handle_open_input(
     mount_handle_t *mount_handle,
     const system_character_t *filename,
     libcerror_error_t **error )
{
	static char *function = "mount_handle_open_input";

	if( mount_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid mount handle.",
		 function );

		return( -1 );
	}
	if( libregf_file_set_ascii_codepage(
	     mount_handle->input_file,
	     mount_handle->ascii_codepage,
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
	     mount_handle->input_file,
	     filename,
	     LIBREGF_OPEN_READ,
	     error ) != 1 )
#else
	if( libregf_file_open(
	     mount_handle->input_file,
	     filename,
	     LIBREGF_OPEN_READ,
	     error ) != 1 )
#endif
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_OPEN_FAILED,
		 "%s: unable to open file.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Closes the mount handle
 * Returns the 0 if succesful or -1 on error
 */
int mount_handle_close(
     mount_handle_t *mount_handle,
     libcerror_error_t **error )
{
	static char *function = "mount_handle_close";

	if( mount_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid mount handle.",
		 function );

		return( -1 );
	}
	if( libregf_file_close(
	     mount_handle->input_file,
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

/* Retrieves the key of a specific path
 * Returns 1 if successful, 0 if no such key or -1 on error
 */
int mount_handle_get_key_by_path(
     mount_handle_t *mount_handle,
     const system_character_t *path,
     size_t path_length,
     system_character_t path_separator,
     libregf_key_t **key,
     libcerror_error_t **error )
{
	system_character_t *regf_path      = NULL;
	static char *function              = "mount_handle_get_key_by_path";
	size_t last_path_seperator_index   = 0;
	size_t normalized_regf_path_length = 0;
	size_t regf_path_length            = 0;
	size_t regf_path_size              = 0;
	int result                         = 0;

	if( mount_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid mount handle.",
		 function );

		return( -1 );
	}
	if( path == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid path.",
		 function );

		return( -1 );
	}
	if( path_length == 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid path length.",
		 function );

		return( -1 );
	}
	if( path_length > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid path length value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( path[ 0 ] != path_separator )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported path.",
		 function );

		return( -1 );
	}
	if( mount_handle_get_filename(
	     mount_handle,
	     path,
	     path_length + 1,
	     path_separator,
	     &regf_path,
	     &regf_path_size,
	     &last_path_seperator_index,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve filename.",
		 function );

		goto on_error;
	}
	if( regf_path == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing REGF path.",
		 function );

		goto on_error;
	}
	regf_path_length = system_string_length(
	                    regf_path );

	/* Strip off (classname) or (values)
	 */
	normalized_regf_path_length = regf_path_length;

	if( ( normalized_regf_path_length >= 1 )
	 && ( regf_path[ normalized_regf_path_length - 1 ] == (system_character_t) '\\' ) )
	{
		normalized_regf_path_length -= 1;
	}
	if( ( normalized_regf_path_length >= 12 )
	 && ( regf_path[ normalized_regf_path_length - 12 ] == (system_character_t) '\\' )
	 && ( system_string_compare(
	       &( regf_path[ normalized_regf_path_length - 11 ] ),
	       _SYSTEM_STRING( "(classname)" ),
	       11 ) == 0 ) )
	{
		regf_path_length = normalized_regf_path_length - 12;
	}
	else if( ( normalized_regf_path_length >= 9 )
	      && ( regf_path[ normalized_regf_path_length - 9 ] == (system_character_t) '\\' )
	      && ( system_string_compare(
	            &( regf_path[ normalized_regf_path_length - 8 ] ),
	            _SYSTEM_STRING( "(values)" ),
	            8 ) == 0 ) )
	{
		regf_path_length = normalized_regf_path_length - 9;
	}
	regf_path[ regf_path_length ] = 0;

#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
	result = libregf_file_get_key_by_utf16_path(
		  mount_handle->input_file,
		  (uint16_t *) regf_path,
		  regf_path_length,
		  key,
		  error );
#else
	result = libregf_file_get_key_by_utf8_path(
		  mount_handle->input_file,
		  (uint8_t *) regf_path,
		  regf_path_length,
		  key,
		  error );
#endif
	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve key.",
		 function );

		goto on_error;
	}
	memory_free(
	 regf_path );

	return( result );

on_error:
	if( regf_path != NULL )
	{
		memory_free(
		 regf_path );
	}
	return( -1 );
}

/* Retrieves the value of a specific path
 * Returns 1 if successful, 0 if no such value or -1 on error
 */
int mount_handle_get_value_by_path(
     mount_handle_t *mount_handle,
     const system_character_t *path,
     size_t path_length,
     system_character_t path_separator,
     libregf_key_t **key,
     libregf_value_t **value,
     libcerror_error_t **error )
{
	system_character_t *regf_path      = NULL;
	system_character_t *value_name     = NULL;
	static char *function              = "mount_handle_get_value_by_path";
	size_t last_path_seperator_index   = 0;
	size_t normalized_regf_path_length = 0;
	size_t regf_path_size              = 0;
	size_t value_name_length           = 0;
	int result                         = 0;

	if( mount_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid mount handle.",
		 function );

		return( -1 );
	}
	if( path == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid path.",
		 function );

		return( -1 );
	}
	if( path_length == 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid path length.",
		 function );

		return( -1 );
	}
	if( path_length > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid path length value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( path[ 0 ] != path_separator )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported path.",
		 function );

		return( -1 );
	}
	if( mount_handle_get_filename(
	     mount_handle,
	     path,
	     path_length + 1,
	     path_separator,
	     &regf_path,
	     &regf_path_size,
	     &last_path_seperator_index,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve filename.",
		 function );

		goto on_error;
	}
	if( regf_path == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing REGF path.",
		 function );

		goto on_error;
	}
	/* Strip off (classname) or (values)
	 */
	normalized_regf_path_length = last_path_seperator_index;

	if( ( normalized_regf_path_length >= 12 )
	 && ( regf_path[ normalized_regf_path_length - 12 ] == (system_character_t) '\\' )
	 && ( system_string_compare(
	       &( regf_path[ normalized_regf_path_length - 11 ] ),
	       _SYSTEM_STRING( "(classname)" ),
	       11 ) == 0 ) )
	{
		normalized_regf_path_length -= 12;
	}
	else if( ( normalized_regf_path_length >= 9 )
	      && ( regf_path[ normalized_regf_path_length - 9 ] == (system_character_t) '\\' )
	      && ( system_string_compare(
	            &( regf_path[ normalized_regf_path_length - 8 ] ),
	            _SYSTEM_STRING( "(values)" ),
	            8 ) == 0 ) )
	{
		normalized_regf_path_length -= 9;
	}
	regf_path[ normalized_regf_path_length ] = 0;

	value_name = &( regf_path[ last_path_seperator_index + 1 ] );

	value_name_length = system_string_length(
	                     value_name );

	if( value_name_length == 0 )
	{
		memory_free(
		 regf_path );

		return( 0 );
	}
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
	result = libregf_file_get_key_by_utf16_path(
		  mount_handle->input_file,
		  (uint16_t *) regf_path,
		  normalized_regf_path_length,
		  key,
		  error );
#else
	result = libregf_file_get_key_by_utf8_path(
		  mount_handle->input_file,
		  (uint8_t *) regf_path,
		  normalized_regf_path_length,
		  key,
		  error );
#endif
	if( result != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve key.",
		 function );

		goto on_error;
	}
	if( value_name_length == 9 )
	{
		if( system_string_compare(
		     value_name,
		     _SYSTEM_STRING( "(default)" ),
		     9 ) == 0 )
		{
			value_name        = NULL;
			value_name_length = 0;
		}
	}
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
	result = libregf_key_get_value_by_utf16_name(
		  *key,
		  (uint16_t *) value_name,
		  value_name_length,
		  value,
		  error );
#else
	result = libregf_key_get_value_by_utf8_name(
		  *key,
		  (uint8_t *) value_name,
		  value_name_length,
		  value,
		  error );
#endif
	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value from key.",
		 function );

		goto on_error;
	}
	memory_free(
	 regf_path );

	return( result );

on_error:
	if( key != NULL )
	{
		libregf_key_free(
		 key,
		 NULL );
	}
	if( regf_path != NULL )
	{
		memory_free(
		 regf_path );
	}
	return( -1 );
}

/* Retrieves the filename
 * Returns 1 if successful or -1 on error
 */
int mount_handle_get_filename(
     mount_handle_t *mount_handle,
     const system_character_t *sanitized_name,
     size_t sanitized_name_size,
     system_character_t path_separator,
     system_character_t **name,
     size_t *name_size,
     size_t *last_path_seperator_index,
     libcerror_error_t **error )
{
	static char *function              = "mount_handle_get_filename";
	size_t name_index                  = 0;
	size_t sanitized_name_index        = 0;
	system_character_t character_value = 0;

	if( mount_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid mount handle.",
		 function );

		return( -1 );
	}
	if( sanitized_name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid sanitized name.",
		 function );

		return( -1 );
	}
	if( sanitized_name_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid sanitized name size value exceeds maximum.",
		 function );

		return( -1 );
	}
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
	if( last_path_seperator_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid last path seperator index.",
		 function );

		return( -1 );
	}
	*name_size = sanitized_name_size;

	if( *name_size > (size_t) ( SSIZE_MAX / sizeof( system_character_t ) ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid name size value exceeds maximum.",
		 function );

		goto on_error;
	}
	*name = system_string_allocate(
	         *name_size );

	if( *name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create name.",
		 function );

		goto on_error;
	}
	name_index           = 0;
	sanitized_name_index = 0;
	sanitized_name_size -= 1;

	while( sanitized_name_index < sanitized_name_size )
	{
		character_value = sanitized_name[ sanitized_name_index ];

		if( character_value == 0x00 )
		{
			break;
		}
		if( ( name_index + 1 ) > *name_size )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: invalid name index value out of bounds.",
			 function );

			goto on_error;
		}
		if( path_separator == (system_character_t) '/' )
		{
			/* Replace:
			 *   / by \
			 *   \\ by \
			 *   \x2f by /
			 *   \x## by values <= 0x1f and 0x7f
			 */
			if( character_value == (system_character_t) '/' )
			{
				*last_path_seperator_index = name_index;

				( *name )[ name_index++ ] = (system_character_t) '\\';
			}
			else if( character_value == (system_character_t) '\\' )
			{
				if( ( ( sanitized_name_index + 1 ) <= sanitized_name_size )
				 && ( sanitized_name[ sanitized_name_index + 1 ] == (system_character_t) '\\' ) )
				{
					( *name )[ name_index++ ] = (system_character_t) '\\';

					sanitized_name_index += 1;
				}
				else if( ( ( sanitized_name_index + 3 ) <= sanitized_name_size )
				      && ( sanitized_name[ sanitized_name_index + 1 ] == (system_character_t) 'x' ) )
				{
					if( ( sanitized_name[ sanitized_name_index + 2 ] >= (system_character_t) '0' )
					 && ( sanitized_name[ sanitized_name_index + 2 ] <= (system_character_t) '9' ) )
					{
						character_value = sanitized_name[ sanitized_name_index + 2 ] - (system_character_t) '0';
					}
					else if( ( sanitized_name[ sanitized_name_index + 2 ] >= (system_character_t) 'a' )
					      && ( sanitized_name[ sanitized_name_index + 2 ] <= (system_character_t) 'f' ) )
					{
						character_value = sanitized_name[ sanitized_name_index + 2 ] - (system_character_t) 'a' + 10;
					}
					else
					{
						libcerror_error_set(
						 error,
						 LIBCERROR_ERROR_DOMAIN_RUNTIME,
						 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
						 "%s: invalid escaped character value out of bounds.",
						 function );

						goto on_error;
					}
					character_value <<= 4;

					if( ( sanitized_name[ sanitized_name_index + 3 ] >= (system_character_t) '0' )
					 && ( sanitized_name[ sanitized_name_index + 3 ] <= (system_character_t) '9' ) )
					{
						character_value |= sanitized_name[ sanitized_name_index + 3 ] - (system_character_t) '0';
					}
					else if( ( sanitized_name[ sanitized_name_index + 3 ] >= (system_character_t) 'a' )
					      && ( sanitized_name[ sanitized_name_index + 3 ] <= (system_character_t) 'f' ) )
					{
						character_value |= sanitized_name[ sanitized_name_index + 3 ] - (system_character_t) 'a' + 10;
					}
					else
					{
						libcerror_error_set(
						 error,
						 LIBCERROR_ERROR_DOMAIN_RUNTIME,
						 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
						 "%s: invalid escaped character value out of bounds.",
						 function );

						goto on_error;
					}
					if( ( character_value == 0 )
					 || ( ( character_value > 0x1f )
					  &&  ( character_value != 0x2f )
					  &&  ( character_value != 0x7f ) ) )
					{
						libcerror_error_set(
						 error,
						 LIBCERROR_ERROR_DOMAIN_RUNTIME,
						 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
						 "%s: invalid escaped character value out of bounds.",
						 function );

						goto on_error;
					}
					( *name )[ name_index++ ] = character_value;

					sanitized_name_index += 3;
				}
				else
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
					 "%s: invalid escaped character value out of bounds.",
					 function );

					goto on_error;
				}
			}
			else
			{
				( *name )[ name_index++ ] = character_value;
			}
		}
		else if( path_separator == (system_character_t) '\\' )
		{
			/* Replace:
			 *   ^^ by ^
			 *   ^x5c by \
			 *   ^x## by values <= 0x1f and 0x7f
			 */
			if( character_value == (system_character_t) '\\' )
			{
				*last_path_seperator_index = name_index;

				( *name )[ name_index++ ] = (system_character_t) '\\';
			}
			else if( character_value == (system_character_t) '^' )
			{
				if( ( ( sanitized_name_index + 1 ) <= sanitized_name_size )
				 && ( sanitized_name[ sanitized_name_index + 1 ] == (system_character_t) '^' ) )
				{
					( *name )[ name_index++ ] = (system_character_t) '^';

					sanitized_name_index += 1;
				}
				else if( ( ( sanitized_name_index + 3 ) <= sanitized_name_size )
				      && ( sanitized_name[ sanitized_name_index + 1 ] == (system_character_t) 'x' ) )
				{
					if( ( sanitized_name[ sanitized_name_index + 2 ] >= (system_character_t) '0' )
					 && ( sanitized_name[ sanitized_name_index + 2 ] <= (system_character_t) '9' ) )
					{
						character_value = sanitized_name[ sanitized_name_index + 2 ] - (system_character_t) '0';
					}
					else if( ( sanitized_name[ sanitized_name_index + 2 ] >= (system_character_t) 'a' )
					      && ( sanitized_name[ sanitized_name_index + 2 ] <= (system_character_t) 'f' ) )
					{
						character_value = sanitized_name[ sanitized_name_index + 2 ] - (system_character_t) 'a' + 10;
					}
					else
					{
						libcerror_error_set(
						 error,
						 LIBCERROR_ERROR_DOMAIN_RUNTIME,
						 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
						 "%s: invalid escaped character value out of bounds.",
						 function );

						goto on_error;
					}
					character_value <<= 4;

					if( ( sanitized_name[ sanitized_name_index + 3 ] >= (system_character_t) '0' )
					 && ( sanitized_name[ sanitized_name_index + 3 ] <= (system_character_t) '9' ) )
					{
						character_value |= sanitized_name[ sanitized_name_index + 3 ] - (system_character_t) '0';
					}
					else if( ( sanitized_name[ sanitized_name_index + 3 ] >= (system_character_t) 'a' )
					      && ( sanitized_name[ sanitized_name_index + 3 ] <= (system_character_t) 'f' ) )
					{
						character_value |= sanitized_name[ sanitized_name_index + 3 ] - (system_character_t) 'a' + 10;
					}
					else
					{
						libcerror_error_set(
						 error,
						 LIBCERROR_ERROR_DOMAIN_RUNTIME,
						 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
						 "%s: invalid escaped character value out of bounds.",
						 function );

						goto on_error;
					}
					if( ( character_value == 0 )
					 || ( ( character_value > 0x1f )
					  &&  ( character_value != 0x5c )
					  &&  ( character_value != 0x7f ) ) )
					{
						libcerror_error_set(
						 error,
						 LIBCERROR_ERROR_DOMAIN_RUNTIME,
						 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
						 "%s: invalid escaped character value out of bounds.",
						 function );

						goto on_error;
					}
					( *name )[ name_index++ ] = character_value;

					sanitized_name_index += 3;
				}
				else
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
					 "%s: invalid escaped character value out of bounds.",
					 function );

					goto on_error;
				}
			}
			else
			{
				( *name )[ name_index++ ] = character_value;
			}
		}
		sanitized_name_index++;
	}
	if( name_index >= *name_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid name index value out of bounds.",
		 function );

		goto on_error;
	}
	( *name )[ name_index ] = 0;

	return( 1 );

on_error:
	if( *name != NULL )
	{
		memory_free(
		 *name );

		*name = NULL;
	}
	*name_size = 0;

	return( -1 );
}

/* Retrieves the sanitized filename
 * Returns 1 if successful or -1 on error
 */
int mount_handle_get_sanitized_filename(
     mount_handle_t *mount_handle,
     const system_character_t *name,
     size_t name_size,
     system_character_t path_separator,
     system_character_t **sanitized_name,
     size_t *sanitized_name_size,
     libcerror_error_t **error )
{
	static char *function              = "mount_handle_get_sanitized_filename";
	size_t name_index                  = 0;
	size_t sanitized_name_index        = 0;
	system_character_t character_value = 0;
	system_character_t hex_digit       = 0;

	if( mount_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid mount handle.",
		 function );

		return( -1 );
	}
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
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid name size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( sanitized_name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid sanitized name.",
		 function );

		return( -1 );
	}
	if( sanitized_name_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid sanitized name size.",
		 function );

		return( -1 );
	}
	name_size -= 1;

	*sanitized_name_size = ( name_size * 4 ) + 1;

	if( *sanitized_name_size > (size_t) ( SSIZE_MAX / sizeof( system_character_t ) ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid sanitized name size value exceeds maximum.",
		 function );

		goto on_error;
	}
	*sanitized_name = system_string_allocate(
	                   *sanitized_name_size );

	if( *sanitized_name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create sanitized name.",
		 function );

		goto on_error;
	}
	sanitized_name_index = 0;

	for( name_index = 0;
	     name_index < name_size;
	     name_index++ )
	{
		character_value = name[ name_index ];

		if( character_value == 0x00 )
		{
			break;
		}
		if( path_separator == (system_character_t) '/' )
		{
			/* Replace:
			 *   values <= 0x1f and 0x7f by \x##
			 *   / by \x2f
			 *   \ by \\
			 */
			if( ( character_value <= 0x1f )
			 || ( character_value == (system_character_t) '/' )
			 || ( character_value == 0x7f ) )
			{
				if( ( sanitized_name_index + 4 ) > *sanitized_name_size )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
					 "%s: invalid sanitized name index value out of bounds.",
					 function );

					goto on_error;
				}
				( *sanitized_name )[ sanitized_name_index++ ] = (system_character_t) '\\';
				( *sanitized_name )[ sanitized_name_index++ ] = (system_character_t) 'x';

				hex_digit = character_value >> 4;

				if( hex_digit <= 0x09 )
				{
					( *sanitized_name )[ sanitized_name_index++ ] = (system_character_t) '0' + hex_digit;
				}
				else
				{
					( *sanitized_name )[ sanitized_name_index++ ] = (system_character_t) 'a' + hex_digit - 10;
				}
				hex_digit = character_value & 0x0f;

				if( hex_digit <= 0x09 )
				{
					( *sanitized_name )[ sanitized_name_index++ ] = (system_character_t) '0' + hex_digit;
				}
				else
				{
					( *sanitized_name )[ sanitized_name_index++ ] = (system_character_t) 'a' + hex_digit - 10;
				}
			}
			else if( character_value == (system_character_t) '\\' )
			{
				if( ( sanitized_name_index + 2 ) > *sanitized_name_size )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
					 "%s: invalid sanitized name index value out of bounds.",
					 function );

					goto on_error;
				}
				( *sanitized_name )[ sanitized_name_index++ ] = (system_character_t) '\\';
				( *sanitized_name )[ sanitized_name_index++ ] = (system_character_t) '\\';
			}
			else
			{
				if( ( sanitized_name_index + 1 ) > *sanitized_name_size )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
					 "%s: invalid sanitized name index value out of bounds.",
					 function );

					goto on_error;
				}
				( *sanitized_name )[ sanitized_name_index++ ] = character_value;
			}
		}
		else if( path_separator == (system_character_t) '\\' )
		{
			/* Replace:
			 *   values <= 0x1f and 0x7f by ^x##
			 *   \ by ^x5c
			 *   ^ by ^^
			 */
			if( ( character_value <= 0x1f )
			 || ( character_value == (system_character_t) '\\' )
			 || ( character_value == 0x7f ) )
			{
				if( ( sanitized_name_index + 4 ) > *sanitized_name_size )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
					 "%s: invalid sanitized name index value out of bounds.",
					 function );

					goto on_error;
				}
				( *sanitized_name )[ sanitized_name_index++ ] = (system_character_t) '^';
				( *sanitized_name )[ sanitized_name_index++ ] = (system_character_t) 'x';

				hex_digit = character_value >> 4;

				if( hex_digit <= 0x09 )
				{
					( *sanitized_name )[ sanitized_name_index++ ] = (system_character_t) '0' + hex_digit;
				}
				else
				{
					( *sanitized_name )[ sanitized_name_index++ ] = (system_character_t) 'a' + hex_digit - 10;
				}
				hex_digit = character_value & 0x0f;

				if( hex_digit <= 0x09 )
				{
					( *sanitized_name )[ sanitized_name_index++ ] = (system_character_t) '0' + hex_digit;
				}
				else
				{
					( *sanitized_name )[ sanitized_name_index++ ] = (system_character_t) 'a' + hex_digit - 10;
				}
			}
			else if( character_value == (system_character_t) '^' )
			{
				if( ( sanitized_name_index + 2 ) > *sanitized_name_size )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
					 "%s: invalid sanitized name index value out of bounds.",
					 function );

					goto on_error;
				}
				( *sanitized_name )[ sanitized_name_index++ ] = (system_character_t) '^';
				( *sanitized_name )[ sanitized_name_index++ ] = (system_character_t) '^';
			}
			else
			{
				if( ( sanitized_name_index + 1 ) > *sanitized_name_size )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
					 "%s: invalid sanitized name index value out of bounds.",
					 function );

					goto on_error;
				}
				( *sanitized_name )[ sanitized_name_index++ ] = character_value;
			}
		}
	}
	if( sanitized_name_index >= *sanitized_name_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid sanitized name index value out of bounds.",
		 function );

		goto on_error;
	}
	( *sanitized_name )[ sanitized_name_index ] = 0;

	return( 1 );

on_error:
	if( *sanitized_name != NULL )
	{
		memory_free(
		 *sanitized_name );

		*sanitized_name = NULL;
	}
	*sanitized_name_size = 0;

	return( -1 );
}

