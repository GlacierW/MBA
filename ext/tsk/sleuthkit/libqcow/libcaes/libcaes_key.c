/*
 * AES key functions
 *
 * Copyright (C) 2011-2016, Joachim Metz <joachim.metz@gmail.com>
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

#include "libcaes_definitions.h"
#include "libcaes_key.h"
#include "libcaes_libcerror.h"

#if defined( WINAPI ) && ( WINVER >= 0x0600 )

/* Creates an AES key
 * Make sure the value key is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libcaes_key_initialize(
     libcaes_key_t **key,
     libcerror_error_t **error )
{
	static char *function = "libcaes_key_initialize";

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
	*key = memory_allocate_structure(
	        libcaes_key_t );

	if( *key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create key.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *key,
	     0,
	     sizeof( libcaes_key_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear key.",
		 function );

		goto on_error;
	}
	( *key )->header.bType    = PLAINTEXTKEYBLOB;
	( *key )->header.bVersion = CUR_BLOB_VERSION;

	return( 1 );

on_error:
	if( *key != NULL )
	{
		memory_free(
		 *key );

		*key = NULL;
	}
	return( -1 );
}

/* Frees an AES key
 * Returns 1 if successful or -1 on error
 */
int libcaes_key_free(
     libcaes_key_t **key,
     libcerror_error_t **error )
{
	static char *function = "libcaes_key_free";
	int result            = 1;

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
		if( memory_set(
		     *key,
		     0,
		     sizeof( libcaes_key_t ) ) == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_MEMORY,
			 LIBCERROR_MEMORY_ERROR_SET_FAILED,
			 "%s: unable to clear key.",
			 function );

			result = -1;
		}
		memory_free(
		 *key );

		*key = NULL;
	}
	return( result );
}

/* Sets the AES key
 * Returns 1 if successful or -1 on error
 */
int libcaes_key_set(
     libcaes_key_t *key,
     const uint8_t *key_data,
     size_t bit_size,
     libcerror_error_t **error )
{
	static char *function = "libcaes_key_set";

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
	if( key_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key data.",
		 function );

		return( -1 );
	}
	if( ( bit_size != 128 )
	 && ( bit_size != 192 )
	 && ( bit_size != 256 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported key bit size.",
		 function );

		return( -1 );
	}
	if( bit_size == 128 )
	{
		key->header.aiKeyAlg = CALG_AES_128;
	}
	else if( bit_size == 192 )
	{
		key->header.aiKeyAlg = CALG_AES_192;
	}
	else if( bit_size == 256 )
	{
		key->header.aiKeyAlg = CALG_AES_256;
	}
	key->data_size = (DWORD)( bit_size / 8 );

	if( memory_copy(
	     key->data,
	     key_data,
	     key->data_size ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
		 "%s: unable to copy key.",
		 function );

		return( -1 );
	}
	return( 1 );
}

#endif /* defined( WINAPI ) && ( WINVER >= 0x0600 ) */

