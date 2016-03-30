/*
 * Encryption functions
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
#include <byte_stream.h>
#include <memory.h>
#include <types.h>

#include "libqcow_encryption.h"
#include "libqcow_libcaes.h"
#include "libqcow_libcerror.h"

/* Creates an encryption context
 * Make sure the value context is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libqcow_encryption_initialize(
     libqcow_encryption_context_t **context,
     uint32_t method,
     libcerror_error_t **error )
{
	static char *function = "libqcow_encryption_initialize";

	if( context == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid context.",
		 function );

		return( -1 );
	}
	if( *context != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid context value already set.",
		 function );

		return( -1 );
	}
	*context = memory_allocate_structure(
	            libqcow_encryption_context_t );

	if( *context == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create context.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *context,
	     0,
	     sizeof( libqcow_encryption_context_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear context.",
		 function );

		memory_free(
		 *context );

		*context = NULL;

		return( -1 );
	}
	if( libcaes_context_initialize(
	     &( ( *context )->decryption_context ),
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable initialize decryption context.",
		 function );

		goto on_error;
	}
	if( libcaes_context_initialize(
	     &( ( *context )->encryption_context ),
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable initialize encryption context.",
		 function );

		goto on_error;
	}
	( *context )->method = method;

	return( 1 );

on_error:
	if( *context != NULL )
	{
		if( ( *context )->decryption_context != NULL )
		{
			libcaes_context_free(
			 &( ( *context )->decryption_context ),
			 NULL );
		}
		memory_free(
		 *context );

		*context = NULL;
	}
	return( -1 );
}

/* Frees an encryption context
 * Returns 1 if successful or -1 on error
 */
int libqcow_encryption_free(
     libqcow_encryption_context_t **context,
     libcerror_error_t **error )
{
	static char *function = "libqcow_encryption_free";
	int result            = 1;

	if( context == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid context.",
		 function );

		return( -1 );
	}
	if( *context != NULL )
	{
		if( libcaes_context_free(
		     &( ( *context )->decryption_context ),
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable free decryption context.",
			 function );

			result = -1;
		}
		if( libcaes_context_free(
		     &( ( *context )->encryption_context ),
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable free encryption context.",
			 function );

			result = -1;
		}
		memory_free(
		 *context );

		*context = NULL;
	}
	return( result );
}

/* Sets the de- and encryption keys
 * Returns 1 if successful or -1 on error
 */
int libqcow_encryption_set_keys(
     libqcow_encryption_context_t *context,
     const uint8_t *key,
     size_t key_size,
     libcerror_error_t **error )
{
	static char *function = "libqcow_encryption_set_keys";

	if( context == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid context.",
		 function );

		return( -1 );
	}
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
	if( key_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid key size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( key_size < 16 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: invalid key value too small.",
		 function );

		return( -1 );
	}
	if( libcaes_crypt_set_key(
	     context->decryption_context,
	     LIBCAES_CRYPT_MODE_DECRYPT,
	     key,
	     128,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set key in decryption context.",
		 function );

		return( -1 );
	}
	if( libcaes_crypt_set_key(
	     context->encryption_context,
	     LIBCAES_CRYPT_MODE_ENCRYPT,
	     key,
	     128,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set key in encryption context.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* De- or encrypts a block of data
 * Returns 1 if successful or -1 on error
 */
int libqcow_encryption_crypt(
     libqcow_encryption_context_t *context,
     int mode,
     const uint8_t *input_data,
     size_t input_data_size,
     uint8_t *output_data,
     size_t output_data_size,
     uint64_t block_key,
     libcerror_error_t **error )
{
	uint8_t initialization_vector[ 16 ];

	static char *function = "libqcow_encryption_crypt";
	size_t data_index     = 0;

	if( context == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid context.",
		 function );

		return( -1 );
	}
	if( ( mode != LIBQCOW_ENCYPTION_CRYPT_MODE_DECRYPT )
	 && ( mode != LIBQCOW_ENCYPTION_CRYPT_MODE_ENCRYPT ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported mode.",
		 function );

		return( -1 );
	}
	if( ( input_data_size % 512 ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid input data size value of bounds.",
		 function );

		return( -1 );
	}
	if( output_data_size < input_data_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid output data size value of bounds.",
		 function );

		return( -1 );
	}
	while( data_index < input_data_size )
	{
		if( memory_set(
		     initialization_vector,
		     0,
		     16 ) == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_MEMORY,
			 LIBCERROR_MEMORY_ERROR_SET_FAILED,
			 "%s: unable to clear initialization vector.",
			 function );

			return( -1 );
		}
		byte_stream_copy_from_uint64_little_endian(
		 initialization_vector,
		 block_key );

		if( mode == LIBQCOW_ENCYPTION_CRYPT_MODE_ENCRYPT )
		{
			if( libcaes_crypt_cbc(
			     context->encryption_context,
			     LIBCAES_CRYPT_MODE_ENCRYPT,
			     initialization_vector,
			     16,
			     &( input_data[ data_index ] ),
			     512,
			     &( output_data[ data_index ] ),
			     512,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ENCRYPTION,
				 LIBCERROR_ENCRYPTION_ERROR_GENERIC,
				 "%s: unable to AES-CBC encrypt output data.",
				 function );

				return( -1 );
			}
		}
		else
		{
			if( libcaes_crypt_cbc(
			     context->decryption_context,
			     LIBCAES_CRYPT_MODE_DECRYPT,
			     initialization_vector,
			     16,
			     &( input_data[ data_index ] ),
			     512,
			     &( output_data[ data_index ] ),
			     512,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ENCRYPTION,
				 LIBCERROR_ENCRYPTION_ERROR_GENERIC,
				 "%s: unable to AES-CBC decrypt output data.",
				 function );

				return( -1 );
			}
		}
		data_index += 512;
		block_key  += 1;
	}
	return( 1 );
}

