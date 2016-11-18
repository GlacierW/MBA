/*
 * AES encryption functions
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
#include <byte_stream.h>
#include <memory.h>
#include <types.h>

#include "libcaes_context.h"
#include "libcaes_definitions.h"
#include "libcaes_libcerror.h"
#include "libcaes_tweaked_context.h"
#include "libcaes_types.h"

/* Creates a tweaked context
 * Make sure the value context is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libcaes_tweaked_context_initialize(
     libcaes_tweaked_context_t **context,
     libcerror_error_t **error )
{
	libcaes_internal_tweaked_context_t *internal_context = NULL;
	static char *function                                = "libcaes_tweaked_context_initialize";

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
	internal_context = memory_allocate_structure(
	                    libcaes_internal_tweaked_context_t );

	if( internal_context == NULL )
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
	     internal_context,
	     0,
	     sizeof( libcaes_internal_tweaked_context_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear context.",
		 function );

		memory_free(
		 internal_context );

		return( -1 );
	}
	if( libcaes_context_initialize(
	     &( internal_context->main_context ),
	     error ) != 1)
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to initialize main context.",
		 function );

		goto on_error;
	}
	if( libcaes_context_initialize(
	     &( internal_context->tweak_context ),
	     error ) != 1)
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to initialize tweak context.",
		 function );

		goto on_error;
	}
	*context = (libcaes_tweaked_context_t *) internal_context;

	return( 1 );

on_error:
	if( internal_context != NULL )
	{
		if( internal_context->main_context != NULL )
		{
			libcaes_context_free(
			 &( internal_context->main_context ),
			 NULL );
		}
		memory_free(
		 internal_context );
	}
	return( -1 );
}

/* Frees a tweaked context
 * Returns 1 if successful or -1 on error
 */
int libcaes_tweaked_context_free(
     libcaes_tweaked_context_t **context,
     libcerror_error_t **error )
{
	libcaes_internal_tweaked_context_t *internal_context = NULL;
	static char *function                                = "libcaes_tweaked_context_free";
	int result                                           = 1;

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
		internal_context = (libcaes_internal_tweaked_context_t *) *context;
		*context         = NULL;

		if( libcaes_context_free(
		     &( internal_context->tweak_context ),
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free tweak context.",
			 function );

			result = -1;
		}
		if( libcaes_context_free(
		     &( internal_context->main_context ),
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free main context.",
			 function );

			result = -1;
		}
		memory_free(
		 internal_context );
	}
	return( result );
}

/* Sets the keys
 * Returns 1 if successful or -1 on error
 */
int libcaes_tweaked_context_set_keys(
     libcaes_tweaked_context_t *context,
     int mode,
     const uint8_t *key,
     size_t key_bit_size,
     const uint8_t *tweak_key,
     size_t tweak_key_bit_size,
     libcerror_error_t **error )
{
	libcaes_internal_tweaked_context_t *internal_context = NULL;
	static char *function                                = "libcaes_tweaked_context_set_key";

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
	internal_context = (libcaes_internal_tweaked_context_t *) context;

	if( ( mode != LIBCAES_CRYPT_MODE_DECRYPT )
	 && ( mode != LIBCAES_CRYPT_MODE_ENCRYPT ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported mode.",
		 function );

		return( -1 );
	}
	if( ( key_bit_size != 128 )
	 && ( key_bit_size != 192 )
	 && ( key_bit_size != 256 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported key bit size.",
		 function );

		return( -1 );
	}
	if( libcaes_context_set_key(
	     internal_context->main_context,
	     mode,
	     key,
	     key_bit_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set key in main context.",
		 function );

		return( -1 );
	}
	if( libcaes_context_set_key(
	     internal_context->tweak_context,
	     LIBCAES_CRYPT_MODE_ENCRYPT,
	     tweak_key,
	     tweak_key_bit_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set tweak key in tweak context.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* De- or encrypts a block of data using AES-XTS (XEX-based tweaked-codebook mode with ciphertext stealing)
 * Returns 1 if successful or -1 on error
 */
int libcaes_crypt_xts(
     libcaes_tweaked_context_t *context,
     int mode,
     const uint8_t *tweak_value,
     size_t tweak_value_size,
     const uint8_t *input_data,
     size_t input_data_size,
     uint8_t *output_data,
     size_t output_data_size,
     libcerror_error_t **error )
{
	uint8_t encrypted_tweak_value[ 16 ];
	uint8_t encrypted_tweak_value_copy[ 16 ];

	libcaes_internal_tweaked_context_t *internal_context = NULL;
	static char *function                                = "libcaes_crypt_xts";
	size_t data_offset                                   = 0;
	size_t remaining_data_size                           = 0;
	uint8_t block_index                                  = 0;
	uint8_t byte_value                                   = 0;
	uint8_t carry_bit                                    = 0;

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
	internal_context = (libcaes_internal_tweaked_context_t *) context;

	if( ( mode != LIBCAES_CRYPT_MODE_DECRYPT )
	 && ( mode != LIBCAES_CRYPT_MODE_ENCRYPT ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported mode.",
		 function );

		return( -1 );
	}
	if( tweak_value == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid tweak value.",
		 function );

		return( -1 );
	}
	if( tweak_value_size != 16 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid tweak value size value out of bounds.",
		 function );

		return( -1 );
	}
	if( input_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid input data.",
		 function );

		return( -1 );
	}
	if( input_data_size < 16 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: invalid input data size value too small.",
		 function );

		return( -1 );
	}
	if( input_data_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid input data size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( output_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid output data.",
		 function );

		return( -1 );
	}
	if( output_data_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid output data size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( output_data_size < input_data_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid ouput data size smaller than input data size.",
		 function );

		return( -1 );
	}
	if( libcaes_crypt_ecb(
	     internal_context->tweak_context,
	     LIBCAES_CRYPT_MODE_ENCRYPT,
	     tweak_value,
	     16,
	     encrypted_tweak_value,
	     16,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ENCRYPTION,
		 LIBCERROR_ENCRYPTION_ERROR_GENERIC,
		 "%s: unable to encrypt tweak value.",
		 function );

		goto on_error;
	}
	if( memory_copy(
	     output_data,
	     input_data,
	     input_data_size ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
		 "%s: unable to copy input data to output data.",
		 function );

		return( -1 );
	}
	remaining_data_size = input_data_size;

	while( ( data_offset + 16 ) <= input_data_size )
	{
		if( ( remaining_data_size < 32 )
		 && ( remaining_data_size != 16 ) )
		{
			/* If the input data size is not a multitude of 16 the remaining data needs to be handled differently
			 */
			if( mode == LIBCAES_CRYPT_MODE_DECRYPT )
			{
				if( memory_copy(
				     encrypted_tweak_value_copy,
				     encrypted_tweak_value,
				     16 ) == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_MEMORY,
					 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
					 "%s: unable to copy encrypted tweak value.",
					 function );
				
					goto on_error;
				}
				/* Update the encrypted tweak value for the next 16-byte block
				 */
				carry_bit = 0;

				for( block_index = 0;
				     block_index < 16;
				     block_index++ )
				{
					byte_value = ( encrypted_tweak_value[ block_index ] << 1 ) | carry_bit;
					carry_bit  = encrypted_tweak_value[ block_index ] >> 7;

					encrypted_tweak_value[ block_index ] = byte_value;
				}
				if( carry_bit > 0 )
				{
					encrypted_tweak_value[ 0 ] ^= 0x87;
				}
			}
		}
#if defined( LIBCAES_UNFOLLED_LOOPS )
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 0 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 1 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 2 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 3 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 4 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 5 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 6 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 7 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 8 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 9 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 10 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 11 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 12 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 13 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 14 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 15 ];
#else
		for( block_index = 0;
		     block_index < 16;
		     block_index++ )
		{
			output_data[ data_offset++ ] ^= encrypted_tweak_value[ block_index ];
		}
#endif
		data_offset -= 16;

		if( libcaes_crypt_ecb(
		     internal_context->main_context,
		     mode,
		     &( output_data[ data_offset ] ),
		     16,
		     &( output_data[ data_offset ] ),
		     16,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ENCRYPTION,
			 LIBCERROR_ENCRYPTION_ERROR_GENERIC,
			 "%s: unable to de/encrypt data.",
			 function );

			goto on_error;
		}
#if defined( LIBCAES_UNFOLLED_LOOPS )
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 0 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 1 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 2 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 3 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 4 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 5 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 6 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 7 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 8 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 9 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 10 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 11 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 12 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 13 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 14 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 15 ];
#else
		for( block_index = 0;
		     block_index < 16;
		     block_index++ )
		{
			output_data[ data_offset++ ] ^= encrypted_tweak_value[ block_index ];
		}
#endif
		remaining_data_size -= 16;

		/* Update the encrypted tweak value for the next 16-byte block
		 */
		carry_bit = 0;

		for( block_index = 0;
		     block_index < 16;
		     block_index++ )
		{
			byte_value = ( encrypted_tweak_value[ block_index ] << 1 ) | carry_bit;
			carry_bit  = encrypted_tweak_value[ block_index ] >> 7;

			encrypted_tweak_value[ block_index ] = byte_value;
		}
		if( carry_bit > 0 )
		{
			encrypted_tweak_value[ 0 ] ^= 0x87;
		}
	}
	/* Any remaining data needs to be handled differently
	 */
	if( remaining_data_size > 0 )
	{
		if( mode == LIBCAES_CRYPT_MODE_DECRYPT )
		{
			if( memory_copy(
			     encrypted_tweak_value,
			     encrypted_tweak_value_copy,
			     16 ) == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_MEMORY,
				 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
				 "%s: unable to copy encrypted tweak value.",
				 function );
			
				goto on_error;
			}
			if( memory_set(
			     encrypted_tweak_value_copy,
			     0,
			     16 ) == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_MEMORY,
				 LIBCERROR_MEMORY_ERROR_SET_FAILED,
				 "%s: unable to clear encrypted tweak value copy.",
				 function );

				goto on_error;
			}
		}
		/* Swap the data of the last 16-byte block with the remaining data
		 */
		data_offset -= 16;

		if( memory_copy(
		     &( output_data[ data_offset + 16 ] ),
		     &( output_data[ data_offset ] ),
		     remaining_data_size ) == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_MEMORY,
			 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
			 "%s: unable to copy remaining output data.",
			 function );

			goto on_error;
		}
		if( memory_copy(
		     &( output_data[ data_offset ] ),
		     &( input_data[ data_offset + 16 ] ),
		     remaining_data_size ) == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_MEMORY,
			 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
			 "%s: unable to copy input data to block data.",
			 function );

			goto on_error;
		}
#if defined( LIBCAES_UNFOLLED_LOOPS )
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 0 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 1 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 2 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 3 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 4 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 5 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 6 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 7 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 8 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 9 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 10 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 11 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 12 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 13 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 14 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 15 ];
#else
		for( block_index = 0;
		     block_index < 16;
		     block_index++ )
		{
			output_data[ data_offset++ ] ^= encrypted_tweak_value[ block_index ];
		}
#endif
		data_offset -= 16;

		if( libcaes_crypt_ecb(
		     internal_context->main_context,
		     mode,
		     &( output_data[ data_offset ] ),
		     16,
		     &( output_data[ data_offset ] ),
		     16,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ENCRYPTION,
			 LIBCERROR_ENCRYPTION_ERROR_GENERIC,
			 "%s: unable to de/encrypt data.",
			 function );

			goto on_error;
		}
#if defined( LIBCAES_UNFOLLED_LOOPS )
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 0 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 1 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 2 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 3 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 4 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 5 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 6 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 7 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 8 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 9 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 10 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 11 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 12 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 13 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 14 ];
		output_data[ data_offset++ ] ^= encrypted_tweak_value[ 15 ];
#else
		for( block_index = 0;
		     block_index < 16;
		     block_index++ )
		{
			output_data[ data_offset++ ] ^= encrypted_tweak_value[ block_index ];
		}
#endif
	}
	if( memory_set(
	     encrypted_tweak_value,
	     0,
	     16 ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear encrypted tweak value.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	memory_set(
	 encrypted_tweak_value_copy,
	 0,
	 16 );

	memory_set(
	 encrypted_tweak_value,
	 0,
	 16 );

	return( -1 );
}

