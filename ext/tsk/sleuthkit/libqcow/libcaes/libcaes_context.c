/*
 * AES de/encryption context functions
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

#if defined( WINAPI )
#include <wincrypt.h>

#elif defined( HAVE_LIBCRYPTO ) && defined( HAVE_OPENSSL_AES_H )
#include <openssl/sha.h>

#elif defined( HAVE_LIBCRYPTO ) && defined( HAVE_OPENSSL_EVP_H )
#include <openssl/err.h>
#include <openssl/evp.h>

#endif

#include "libcaes_context.h"
#include "libcaes_definitions.h"
#include "libcaes_key.h"
#include "libcaes_libcerror.h"
#include "libcaes_types.h"

#if !defined( LIBCAES_HAVE_AES_SUPPORT )

/* FIPS-197 compliant AES encryption functions
 *
 * The AES block cipher was designed by Vincent Rijmen and Joan Daemen.
 *
 * http://csrc.nist.gov/encryption/aes/rijndael/Rijndael.pdf
 * http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf
 */

#define libcaes_calculate_forward_substitution_value( value, index0, index1, index2, index3 ) \
	value   = libcaes_forward_substitution_box[ index3 ]; \
	value <<= 8; \
	value  |= libcaes_forward_substitution_box[ index2 ]; \
	value <<= 8; \
	value  |= libcaes_forward_substitution_box[ index1 ]; \
	value <<= 8; \
	value  |= libcaes_forward_substitution_box[ index0 ];

#define libcaes_calculate_forward_table_value( value, index0, index1, index2, index3 ) \
	value  = libcaes_forward_table0[ index0 ]; \
	value ^= libcaes_forward_table1[ index1 ]; \
	value ^= libcaes_forward_table2[ index2 ]; \
	value ^= libcaes_forward_table3[ index3 ];

#define libcaes_calculate_reverse_substitution_value( value, index0, index1, index2, index3 ) \
	value   = libcaes_reverse_substitution_box[ index3 ]; \
	value <<= 8; \
	value  |= libcaes_reverse_substitution_box[ index2 ]; \
	value <<= 8; \
	value  |= libcaes_reverse_substitution_box[ index1 ]; \
	value <<= 8; \
	value  |= libcaes_reverse_substitution_box[ index0 ];

#define libcaes_calculate_reverse_table_value( value, index0, index1, index2, index3 ) \
	value  = libcaes_reverse_table0[ index0 ]; \
	value ^= libcaes_reverse_table1[ index1 ]; \
	value ^= libcaes_reverse_table2[ index2 ]; \
	value ^= libcaes_reverse_table3[ index3 ];

#define libcaes_calculate_forward_substitution_round( round_keys, values_32bit, cipher_values_32bit, substitution_value ) \
	libcaes_calculate_forward_substitution_value( \
	 substitution_value, \
	 ( cipher_values_32bit[ 0 ] & 0xff ), \
	 ( ( cipher_values_32bit[ 1 ] >> 8 ) & 0xff ), \
	 ( ( cipher_values_32bit[ 2 ] >> 16 ) & 0xff ), \
	 ( ( cipher_values_32bit[ 3 ] >> 24 ) & 0xff ) ); \
\
	values_32bit[ 0 ] = round_keys[ 0 ] ^ substitution_value; \
\
	libcaes_calculate_forward_substitution_value( \
	 substitution_value, \
	 ( cipher_values_32bit[ 1 ] & 0xff ), \
	 ( ( cipher_values_32bit[ 2 ] >> 8 ) & 0xff ), \
	 ( ( cipher_values_32bit[ 3 ] >> 16 ) & 0xff ), \
	 ( ( cipher_values_32bit[ 0 ] >> 24 ) & 0xff ) ); \
\
	values_32bit[ 1 ] = round_keys[ 1 ] ^ substitution_value; \
\
	libcaes_calculate_forward_substitution_value( \
	 substitution_value, \
	 ( cipher_values_32bit[ 2 ] & 0xff ), \
	 ( ( cipher_values_32bit[ 3 ] >> 8 ) & 0xff ), \
	 ( ( cipher_values_32bit[ 0 ] >> 16 ) & 0xff ), \
	 ( ( cipher_values_32bit[ 1 ] >> 24 ) & 0xff ) ); \
\
	values_32bit[ 2 ] = round_keys[ 2 ] ^ substitution_value; \
\
	libcaes_calculate_forward_substitution_value( \
	 substitution_value, \
	 ( cipher_values_32bit[ 3 ] & 0xff ), \
	 ( ( cipher_values_32bit[ 0 ] >> 8 ) & 0xff ), \
	 ( ( cipher_values_32bit[ 1 ] >> 16 ) & 0xff ), \
	 ( ( cipher_values_32bit[ 2 ] >> 24 ) & 0xff ) ); \
\
	values_32bit[ 3 ] = round_keys[ 3 ] ^ substitution_value;

#define libcaes_calculate_forward_table_round( round_keys, values_32bit, cipher_values_32bit, table_value ) \
	libcaes_calculate_forward_table_value( \
	 table_value, \
	 ( cipher_values_32bit[ 0 ] & 0xff ), \
	 ( ( cipher_values_32bit[ 1 ] >> 8 ) & 0xff ), \
	 ( ( cipher_values_32bit[ 2 ] >> 16 ) & 0xff ), \
	 ( ( cipher_values_32bit[ 3 ] >> 24 ) & 0xff ) ); \
\
	values_32bit[ 0 ] = round_keys[ 0 ] ^ table_value; \
\
	libcaes_calculate_forward_table_value( \
	 table_value, \
	 ( cipher_values_32bit[ 1 ] & 0xff ), \
	 ( ( cipher_values_32bit[ 2 ] >> 8 ) & 0xff ), \
	 ( ( cipher_values_32bit[ 3 ] >> 16 ) & 0xff ), \
	 ( ( cipher_values_32bit[ 0 ] >> 24 ) & 0xff ) ); \
\
	values_32bit[ 1 ] = round_keys[ 1 ] ^ table_value; \
\
	libcaes_calculate_forward_table_value( \
	 table_value, \
	 ( cipher_values_32bit[ 2 ] & 0xff ), \
	 ( ( cipher_values_32bit[ 3 ] >> 8 ) & 0xff ), \
	 ( ( cipher_values_32bit[ 0 ] >> 16 ) & 0xff ), \
	 ( ( cipher_values_32bit[ 1 ] >> 24 ) & 0xff ) ); \
\
	values_32bit[ 2 ] = round_keys[ 2 ] ^ table_value; \
\
	libcaes_calculate_forward_table_value( \
	 table_value, \
	 ( cipher_values_32bit[ 3 ] & 0xff ), \
	 ( ( cipher_values_32bit[ 0 ] >> 8 ) & 0xff ), \
	 ( ( cipher_values_32bit[ 1 ] >> 16 ) & 0xff ), \
	 ( ( cipher_values_32bit[ 2 ] >> 24 ) & 0xff ) ); \
\
	values_32bit[ 3 ] = round_keys[ 3 ] ^ table_value;

#define libcaes_calculate_reverse_substitution_round( round_keys, values_32bit, cipher_values_32bit, substitution_value ) \
	libcaes_calculate_reverse_substitution_value( \
	 substitution_value, \
	 ( cipher_values_32bit[ 0 ] & 0xff ), \
	 ( ( cipher_values_32bit[ 3 ] >> 8 ) & 0xff ), \
	 ( ( cipher_values_32bit[ 2 ] >> 16 ) & 0xff ), \
	 ( ( cipher_values_32bit[ 1 ] >> 24 ) & 0xff ) ); \
\
	values_32bit[ 0 ] = round_keys[ 0 ] ^ substitution_value; \
\
	libcaes_calculate_reverse_substitution_value( \
	 substitution_value, \
	 ( cipher_values_32bit[ 1 ] & 0xff ), \
	 ( ( cipher_values_32bit[ 0 ] >> 8 ) & 0xff ), \
	 ( ( cipher_values_32bit[ 3 ] >> 16 ) & 0xff ), \
	 ( ( cipher_values_32bit[ 2 ] >> 24 ) & 0xff ) ); \
\
	values_32bit[ 1 ] = round_keys[ 1 ] ^ substitution_value; \
\
	libcaes_calculate_reverse_substitution_value( \
	 substitution_value, \
	 ( cipher_values_32bit[ 2 ] & 0xff ), \
	 ( ( cipher_values_32bit[ 1 ] >> 8 ) & 0xff ), \
	 ( ( cipher_values_32bit[ 0 ] >> 16 ) & 0xff ), \
	 ( ( cipher_values_32bit[ 3 ] >> 24 ) & 0xff ) ); \
\
	values_32bit[ 2 ] = round_keys[ 2 ] ^ substitution_value; \
\
	libcaes_calculate_reverse_substitution_value( \
	 substitution_value, \
	 ( cipher_values_32bit[ 3 ] & 0xff ), \
	 ( ( cipher_values_32bit[ 2 ] >> 8 ) & 0xff ), \
	 ( ( cipher_values_32bit[ 1 ] >> 16 ) & 0xff ), \
	 ( ( cipher_values_32bit[ 0 ] >> 24 ) & 0xff ) ); \
\
	values_32bit[ 3 ] = round_keys[ 3 ] ^ substitution_value;

#define libcaes_calculate_reverse_table_round( round_keys, values_32bit, cipher_values_32bit, table_value ) \
	libcaes_calculate_reverse_table_value( \
	 table_value, \
	 ( cipher_values_32bit[ 0 ] & 0xff ), \
	 ( ( cipher_values_32bit[ 3 ] >> 8 ) & 0xff ), \
	 ( ( cipher_values_32bit[ 2 ] >> 16 ) & 0xff ), \
	 ( ( cipher_values_32bit[ 1 ] >> 24 ) & 0xff ) ); \
\
	values_32bit[ 0 ] = round_keys[ 0 ] ^ table_value; \
\
	libcaes_calculate_reverse_table_value( \
	 table_value, \
	 ( cipher_values_32bit[ 1 ] & 0xff ), \
	 ( ( cipher_values_32bit[ 0 ] >> 8 ) & 0xff ), \
	 ( ( cipher_values_32bit[ 3 ] >> 16 ) & 0xff ), \
	 ( ( cipher_values_32bit[ 2 ] >> 24 ) & 0xff ) ); \
\
	values_32bit[ 1 ] = round_keys[ 1 ] ^ table_value; \
\
	libcaes_calculate_reverse_table_value( \
	 table_value, \
	 ( cipher_values_32bit[ 2 ] & 0xff ), \
	 ( ( cipher_values_32bit[ 1 ] >> 8 ) & 0xff ), \
	 ( ( cipher_values_32bit[ 0 ] >> 16 ) & 0xff ), \
	 ( ( cipher_values_32bit[ 3 ] >> 24 ) & 0xff ) ); \
\
	values_32bit[ 2 ] = round_keys[ 2 ] ^ table_value; \
\
	libcaes_calculate_reverse_table_value( \
	 table_value, \
	 ( cipher_values_32bit[ 3 ] & 0xff ), \
	 ( ( cipher_values_32bit[ 2 ] >> 8 ) & 0xff ), \
	 ( ( cipher_values_32bit[ 1 ] >> 16 ) & 0xff ), \
	 ( ( cipher_values_32bit[ 0 ] >> 24 ) & 0xff ) ); \
\
	values_32bit[ 3 ] = round_keys[ 3 ] ^ table_value;

/* Forward S-box & tables
 */
static uint8_t libcaes_forward_substitution_box[ 256 ];

static uint32_t libcaes_forward_table0[ 256 ];
static uint32_t libcaes_forward_table1[ 256 ];
static uint32_t libcaes_forward_table2[ 256 ];
static uint32_t libcaes_forward_table3[ 256 ];

/* Reverse S-box & tables
 */
static uint8_t libcaes_reverse_substitution_box[ 256 ];

static uint32_t libcaes_reverse_table0[ 256 ];
static uint32_t libcaes_reverse_table1[ 256 ];
static uint32_t libcaes_reverse_table2[ 256 ];
static uint32_t libcaes_reverse_table3[ 256 ];

/* Round constants
 */
static uint32_t libcaes_round_constants[ 10 ];

static int libcaes_tables_initialized = 0;

#endif /* !defined( LIBCAES_HAVE_AES_SUPPORT ) */

/* Creates a context
 * Make sure the value context is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libcaes_context_initialize(
     libcaes_context_t **context,
     libcerror_error_t **error )
{
	libcaes_internal_context_t *internal_context = NULL;
	static char *function                        = "libcaes_context_initialize";

#if defined( WINAPI ) && ( WINVER >= 0x0600 )
	DWORD error_code                             = 0;
#endif

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
	                    libcaes_internal_context_t );

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
	     sizeof( libcaes_internal_context_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear context.",
		 function );

		goto on_error;
	}
#if defined( WINAPI ) && ( WINVER >= 0x0600 )
	/* Request the AES crypt provider, fail back to the RSA crypt provider
	*/
	if( CryptAcquireContext(
	     &( internal_context->crypt_provider ),
	     NULL,
	     MS_ENH_RSA_AES_PROV,
	     PROV_RSA_AES,
	     0 ) == 0 )
	{
/* TODO fallback for XP
		if( CryptAcquireContext(
		     &( internal_context->crypt_provider ),
		     NULL,
		     MS_ENH_RSA_AES_PROV_XP,
		     PROV_RSA_AES,
		     CRYPT_NEWKEYSET ) == 0 )
*/
		error_code = GetLastError();

		libcerror_system_set_error(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 error_code,
		 "%s: unable to create AES crypt provider.",
		 function );

		return( -1 );
	}
	if( internal_context->crypt_provider == 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: unable to create AES crypt provider.",
		 function );

		return( -1 );
	}

#elif defined( HAVE_LIBCRYPTO ) && defined( HAVE_OPENSSL_AES_H )
	/* No additional initialization necessary */

#elif defined( HAVE_LIBCRYPTO ) && defined( HAVE_OPENSSL_EVP_H )
	EVP_CIPHER_CTX_init(
	 &( internal_context->evp_context ) );

	if( EVP_CIPHER_CTX_set_padding(
	     &( internal_context->evp_context ),
	     1 ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set padding in context.",
		 function );

		goto on_error;
	}

#else
	if( libcaes_tables_initialized == 0 )
	{
		if( libcaes_initialize_tables(
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to initialize tables.",
			 function );

			goto on_error;
		}
		libcaes_tables_initialized = 1;
	}
#endif
	*context = (libcaes_context_t *) internal_context;

	return( 1 );

on_error:
	if( internal_context != NULL )
	{
		memory_free(
		 internal_context );
	}
	return( -1 );
}

/* Frees a context
 * Returns 1 if successful or -1 on error
 */
int libcaes_context_free(
     libcaes_context_t **context,
     libcerror_error_t **error )
{
	libcaes_internal_context_t *internal_context = NULL;
	static char *function                        = "libcaes_context_free";

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
		internal_context = (libcaes_internal_context_t *) *context;
		*context         = NULL;

#if defined( WINAPI ) && ( WINVER >= 0x0600 )
		if( internal_context->key != 0 )
		{
			CryptDestroyKey(
			 internal_context->key );
		}
		if( internal_context->crypt_provider != 0 )
		{
			CryptReleaseContext(
			 internal_context->crypt_provider,
			 0 );
		}

#elif defined( HAVE_LIBCRYPTO ) && defined( HAVE_OPENSSL_AES_H )
		/* No additional clean up necessary */

#elif defined( HAVE_LIBCRYPTO ) && defined( HAVE_OPENSSL_EVP_H )
		EVP_CIPHER_CTX_cleanup(
		 &( internal_context->evp_context ) );

		/* Make sure the error state is removed otherwise openssl will leak memory
		 */
		ERR_remove_thread_state(
		 NULL );

#else
		/* No additional clean up necessary */
#endif
		memory_free(
		 internal_context );
	}
	return( 1 );
}

/* Sets the key
 * Returns 1 if successful or -1 on error
 */
int libcaes_context_set_key(
     libcaes_context_t *context,
     int mode,
     const uint8_t *key,
     size_t key_bit_size,
     libcerror_error_t **error )
{
	libcaes_internal_context_t *internal_context = NULL;
	static char *function                        = "libcaes_context_set_key";

#if defined( WINAPI ) && ( WINVER >= 0x0600 )
	libcaes_key_t *wincrypt_key                  = NULL;
	DWORD error_code                             = 0;
	DWORD wincrypt_key_size                      = 0;
#endif

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
	internal_context = (libcaes_internal_context_t *) context;

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
#if defined( WINAPI ) && ( WINVER >= 0x0600 )
	if( libcaes_key_initialize(
	     &wincrypt_key,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create wincrypt key.",
		 function );

		return( -1 );
	}
	if( libcaes_key_set(
	     wincrypt_key,
	     key,
	     key_bit_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set wincrypt key.",
		 function );

		libcaes_key_free(
		 &wincrypt_key,
		 NULL );

		return( -1 );
	}
	wincrypt_key_size = (DWORD) ( sizeof( libcaes_key_t ) - ( ( 256 - key_bit_size ) / 8 ) );

	if( CryptImportKey(
	     internal_context->crypt_provider,
	     (CONST BYTE *) wincrypt_key,
	     wincrypt_key_size,
	     (HCRYPTKEY) NULL,
	     0,
	     &( internal_context->key ) ) == 0 )
	{
		error_code = GetLastError();

		libcerror_system_set_error(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 error_code,
		 "%s: unable to create key object.",
		 function );

		libcaes_key_free(
		 &wincrypt_key,
		 NULL );

		return( -1 );
	}
	if( libcaes_key_free(
	     &wincrypt_key,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
		 "%s: unable to free wincrypt key.",
		 function );

		return( -1 );
	}

#elif defined( HAVE_LIBCRYPTO ) && defined( HAVE_OPENSSL_AES_H )
	if( mode == LIBCAES_CRYPT_MODE_ENCRYPT )
	{
		if( AES_set_encrypt_key(
		     (unsigned char *) key,
		     (int) key_bit_size,
		     &( internal_context->key ) ) != 0 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set encryption key.",
			 function );

			return( -1 );
		}
	}
	else
	{
		if( AES_set_decrypt_key(
		     (unsigned char *) key,
		     (int) key_bit_size,
		     &( internal_context->key ) ) != 0 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set decryption key.",
			 function );

			return( -1 );
		}
	}

#elif defined( HAVE_LIBCRYPTO ) && defined( HAVE_OPENSSL_EVP_H )
	if( memory_copy(
	     internal_context->key,
	     key,
	     key_bit_size / 8 ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
		 "%s: unable to copy key.",
		 function );

		return( -1 );
	}
	internal_context->key_bit_size = key_bit_size;

#else
	if( mode == LIBCAES_CRYPT_MODE_ENCRYPT )
	{
		if( libcaes_internal_context_set_encryption_key(
		     internal_context,
		     key,
		     key_bit_size,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set encryption key.",
			 function );

			return( -1 );
		}
	}
	else
	{
		if( libcaes_internal_context_set_decryption_key(
		     internal_context,
		     key,
		     key_bit_size,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set decryption key.",
			 function );

			return( -1 );
		}
	}
#endif
	return( 1 );
}

#if !defined( LIBCAES_HAVE_AES_SUPPORT )

/* Initializes the AES encryption and decryption tables
 * Returns 1 if successful or -1 on error
 */
int libcaes_initialize_tables(
     libcerror_error_t **error )
{
	uint8_t logs_table[ 256 ];
	uint8_t powers_table[ 256 ];

	static char *function        = "libcaes_initialize_tables";
	uint16_t byte_index          = 0;
	uint16_t table_index         = 0;
	uint8_t byte_value           = 0;
	uint8_t round_constant_index = 0;
	uint8_t substitution_value   = 0;

	if( memory_set(
	     logs_table,
	     0,
	     sizeof( uint8_t ) * 256 ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear logs table.",
		 function );

		return( -1 );
	}
	/* Fill the powers and logs tables over GF( 2^8 )
	 */
	byte_value = 1;

	for( byte_index = 0;
	     byte_index < 256;
	     byte_index++ )
	{
		powers_table[ byte_index ] = byte_value;
		logs_table[ byte_value ]   = (uint8_t) byte_index;

		if( ( byte_value & 0x80 ) == 0 )
		{
			byte_value ^= ( byte_value << 1 ) & 0xff;
		}
		else
		{
			byte_value ^= ( ( byte_value << 1 ) & 0xff ) ^ 0x1b;
		}
	}
	/* Fill the round constants
	 */
	byte_value = 1;

	for( round_constant_index = 0;
	     round_constant_index < 10;
	     round_constant_index++ )
	{
		libcaes_round_constants[ round_constant_index ] = (uint32_t) byte_value;

		if( ( byte_value & 0x80 ) == 0 )
		{
			byte_value = ( byte_value << 1 ) & 0xff;
		}
		else
		{
			byte_value = ( ( byte_value << 1 ) & 0xff ) ^ 0x1b;
		}
	}
	/* Fill the forward and reverse S-boxes
	 */
	libcaes_forward_substitution_box[ 0x00 ] = 0x63;
	libcaes_reverse_substitution_box[ 0x63 ] = 0x00;

	for( byte_index = 1;
	     byte_index < 256;
	     byte_index++ )
	{
		table_index = 255 - logs_table[ byte_index ];
		byte_value  = powers_table[ table_index ];

		substitution_value = ( ( byte_value << 1 ) & 0xff )
		                   | ( byte_value >> 7 );

		byte_value ^= substitution_value;

		substitution_value = ( ( substitution_value << 1 ) & 0xff )
		                   | ( substitution_value >> 7 );

		byte_value ^= substitution_value;

		substitution_value = ( ( substitution_value << 1 ) & 0xff )
		                   | ( substitution_value >> 7 );

		byte_value ^= substitution_value;

		substitution_value = ( ( substitution_value << 1 ) & 0xff )
		                   | ( substitution_value >> 7 );

		substitution_value ^= byte_value ^ 0x63;

		libcaes_forward_substitution_box[ byte_index ]         = substitution_value;
		libcaes_reverse_substitution_box[ substitution_value ] = (uint8_t) byte_index;
	}
	/* Fill the forward and reverse tables
	 */
	for( byte_index = 0;
	     byte_index < 256;
	     byte_index++ )
	{
		byte_value = libcaes_forward_substitution_box[ byte_index ];

		if( ( byte_value & 0x80 ) == 0 )
		{
			substitution_value = ( byte_value << 1 ) & 0xff;
		}
		else
		{
			substitution_value = ( ( byte_value << 1 ) & 0xff ) ^ 0x1b;
		}
		libcaes_forward_table0[ byte_index ]   = byte_value ^ substitution_value;
		libcaes_forward_table0[ byte_index ] <<= 8;
		libcaes_forward_table0[ byte_index ]  |= byte_value;
		libcaes_forward_table0[ byte_index ] <<= 8;
		libcaes_forward_table0[ byte_index ]  |= byte_value;
		libcaes_forward_table0[ byte_index ] <<= 8;
		libcaes_forward_table0[ byte_index ]  |= substitution_value;

		libcaes_forward_table1[ byte_index ] = byte_stream_bit_rotate_left(
		                                           libcaes_forward_table0[ byte_index ],
		                                           8 );

		libcaes_forward_table2[ byte_index ] = byte_stream_bit_rotate_left(
		                                           libcaes_forward_table1[ byte_index ],
		                                           8 );

		libcaes_forward_table3[ byte_index ] = byte_stream_bit_rotate_left(
		                                           libcaes_forward_table2[ byte_index ],
		                                           8 );

		substitution_value = libcaes_reverse_substitution_box[ byte_index ];

		libcaes_reverse_table0[ byte_index ] = 0;

		if( substitution_value != 0 )
		{
			table_index  = logs_table[ 0x0b ];
			table_index += logs_table[ substitution_value ];
			table_index %= 255;

			libcaes_reverse_table0[ byte_index ] ^= powers_table[ table_index ];
			libcaes_reverse_table0[ byte_index ] <<= 8;

			table_index  = logs_table[ 0x0d ];
			table_index += logs_table[ substitution_value ];
			table_index %= 255;

			libcaes_reverse_table0[ byte_index ] ^= powers_table[ table_index ];
			libcaes_reverse_table0[ byte_index ] <<= 8;

			table_index  = logs_table[ 0x09 ];
			table_index += logs_table[ substitution_value ];
			table_index %= 255;

			libcaes_reverse_table0[ byte_index ] ^= powers_table[ table_index ];
			libcaes_reverse_table0[ byte_index ] <<= 8;

			table_index  = logs_table[ 0x0e ];
			table_index += logs_table[ substitution_value ];
			table_index %= 255;

			libcaes_reverse_table0[ byte_index ] ^= powers_table[ table_index ];
		}
		libcaes_reverse_table1[ byte_index ] = byte_stream_bit_rotate_left(
		                                           libcaes_reverse_table0[ byte_index ],
		                                           8 );

		libcaes_reverse_table2[ byte_index ] = byte_stream_bit_rotate_left(
		                                           libcaes_reverse_table1[ byte_index ],
		                                           8 );

		libcaes_reverse_table3[ byte_index ] = byte_stream_bit_rotate_left(
		                                           libcaes_reverse_table2[ byte_index ],
		                                           8 );
	}
	return( 1 );
}

/* Sets the AES decryption key
 * Returns 1 if successful or -1 on error
 */
int libcaes_internal_context_set_decryption_key(
     libcaes_internal_context_t *internal_context,
     const uint8_t *key,
     size_t key_bit_size,
     libcerror_error_t **error )
{
	libcaes_internal_context_t *encryption_context = NULL;
	static char *function                          = "libcaes_internal_context_set_decryption_key";
	uint32_t *encryption_round_keys                = NULL;
	uint32_t *round_keys                           = NULL;
	size_t round_keys_byte_offset                  = 0;
	uint8_t byte_value0                            = 0;
	uint8_t byte_value1                            = 0;
	uint8_t byte_value2                            = 0;
	uint8_t byte_value3                            = 0;
	int round_key_iterator                         = 0;

	if( internal_context == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid context.",
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
	if( key_bit_size == 128 )
	{
		internal_context->number_of_round_keys = 10;
	}
	else if( key_bit_size == 192 )
	{
		internal_context->number_of_round_keys = 12;
	}
	else if( key_bit_size == 256 )
	{
		internal_context->number_of_round_keys = 14;
	}
	/* Align the buffer to next 16-byte blocks
	 */
	internal_context->round_keys = (uint32_t *) ( 16 + ( (intptr_t) internal_context->round_keys_data & ~( 15 ) ) );

	round_keys = internal_context->round_keys;

	if( libcaes_context_initialize(
	     (libcaes_context_t **) &encryption_context,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create encryption context.",
		 function );

		goto on_error;
	}
	if( libcaes_internal_context_set_encryption_key(
	     encryption_context,
	     key,
	     key_bit_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set encryption key.",
		 function );

		goto on_error;
	}
	/* Point to the end of the round keys
	 */
	round_keys_byte_offset = encryption_context->number_of_round_keys * sizeof( uint32_t );

	encryption_round_keys = &( encryption_context->round_keys[ round_keys_byte_offset ] );

	round_keys[ 0 ] = encryption_round_keys[ 0 ];
	round_keys[ 1 ] = encryption_round_keys[ 1 ];
	round_keys[ 2 ] = encryption_round_keys[ 2 ];
	round_keys[ 3 ] = encryption_round_keys[ 3 ];

	round_keys            += 4;
	encryption_round_keys -= 4;

	for( round_key_iterator = internal_context->number_of_round_keys;
	     round_key_iterator > 1;
	     round_key_iterator-- )
	{
		byte_value0 = ( encryption_round_keys[ 0 ] & 0xff );
		byte_value1 = ( ( encryption_round_keys[ 0 ] >> 8 ) & 0xff );
		byte_value2 = ( ( encryption_round_keys[ 0 ] >> 16 ) & 0xff );
		byte_value3 = ( ( encryption_round_keys[ 0 ] >> 24 ) & 0xff );

		byte_value0 = libcaes_forward_substitution_box[ byte_value0 ];
		byte_value1 = libcaes_forward_substitution_box[ byte_value1 ];
		byte_value2 = libcaes_forward_substitution_box[ byte_value2 ];
		byte_value3 = libcaes_forward_substitution_box[ byte_value3 ];

		round_keys[ 0 ] = libcaes_reverse_table0[ byte_value0 ]
			        ^ libcaes_reverse_table1[ byte_value1 ]
			        ^ libcaes_reverse_table2[ byte_value2 ]
			        ^ libcaes_reverse_table3[ byte_value3 ];

		byte_value0 = ( encryption_round_keys[ 1 ] & 0xff );
		byte_value1 = ( ( encryption_round_keys[ 1 ] >> 8 ) & 0xff );
		byte_value2 = ( ( encryption_round_keys[ 1 ] >> 16 ) & 0xff );
		byte_value3 = ( ( encryption_round_keys[ 1 ] >> 24 ) & 0xff );

		byte_value0 = libcaes_forward_substitution_box[ byte_value0 ];
		byte_value1 = libcaes_forward_substitution_box[ byte_value1 ];
		byte_value2 = libcaes_forward_substitution_box[ byte_value2 ];
		byte_value3 = libcaes_forward_substitution_box[ byte_value3 ];

		round_keys[ 1 ] = libcaes_reverse_table0[ byte_value0 ]
			        ^ libcaes_reverse_table1[ byte_value1 ]
			        ^ libcaes_reverse_table2[ byte_value2 ]
			        ^ libcaes_reverse_table3[ byte_value3 ];

		byte_value0 = ( encryption_round_keys[ 2 ] & 0xff );
		byte_value1 = ( ( encryption_round_keys[ 2 ] >> 8 ) & 0xff );
		byte_value2 = ( ( encryption_round_keys[ 2 ] >> 16 ) & 0xff );
		byte_value3 = ( ( encryption_round_keys[ 2 ] >> 24 ) & 0xff );

		byte_value0 = libcaes_forward_substitution_box[ byte_value0 ];
		byte_value1 = libcaes_forward_substitution_box[ byte_value1 ];
		byte_value2 = libcaes_forward_substitution_box[ byte_value2 ];
		byte_value3 = libcaes_forward_substitution_box[ byte_value3 ];

		round_keys[ 2 ] = libcaes_reverse_table0[ byte_value0 ]
			        ^ libcaes_reverse_table1[ byte_value1 ]
			        ^ libcaes_reverse_table2[ byte_value2 ]
			        ^ libcaes_reverse_table3[ byte_value3 ];

		byte_value0 = ( encryption_round_keys[ 3 ] & 0xff );
		byte_value1 = ( ( encryption_round_keys[ 3 ] >> 8 ) & 0xff );
		byte_value2 = ( ( encryption_round_keys[ 3 ] >> 16 ) & 0xff );
		byte_value3 = ( ( encryption_round_keys[ 3 ] >> 24 ) & 0xff );

		byte_value0 = libcaes_forward_substitution_box[ byte_value0 ];
		byte_value1 = libcaes_forward_substitution_box[ byte_value1 ];
		byte_value2 = libcaes_forward_substitution_box[ byte_value2 ];
		byte_value3 = libcaes_forward_substitution_box[ byte_value3 ];

		round_keys[ 3 ] = libcaes_reverse_table0[ byte_value0 ]
			        ^ libcaes_reverse_table1[ byte_value1 ]
			        ^ libcaes_reverse_table2[ byte_value2 ]
			        ^ libcaes_reverse_table3[ byte_value3 ];

		round_keys            += 4;
		encryption_round_keys -= 4;
	}
	round_keys[ 0 ] = encryption_round_keys[ 0 ];
	round_keys[ 1 ] = encryption_round_keys[ 1 ];
	round_keys[ 2 ] = encryption_round_keys[ 2 ];
	round_keys[ 3 ] = encryption_round_keys[ 3 ];

	if( libcaes_context_free(
	     (libcaes_context_t **) &encryption_context,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
		 "%s: unable to free encryption context.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( encryption_context != NULL )
	{
		libcaes_context_free(
		 (libcaes_context_t **) &encryption_context,
		 NULL );
	}
	return( -1 );
}

/* Sets the AES encryption key
 * Returns 1 if successful or -1 on error
 */
int libcaes_internal_context_set_encryption_key(
     libcaes_internal_context_t *internal_context,
     const uint8_t *key,
     size_t key_bit_size,
     libcerror_error_t **error )
{
	static char *function    = "libcaes_internal_context_set_encryption_key";
	uint32_t *round_keys     = NULL;
	size_t key_index         = 0;
	int round_constant_index = 0;

	if( internal_context == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid context.",
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
	/* Align the buffer to next 16-byte blocks
	 */
	internal_context->round_keys = (uint32_t *) ( 16 + ( (intptr_t) internal_context->round_keys_data & ~( 15 ) ) );

	round_keys = internal_context->round_keys;

	for( key_index = 0;
	     key_index < key_bit_size / 8;
	     key_index += 4 )
	{
		byte_stream_copy_to_uint32_little_endian(
	         &( key[ key_index ] ),
	         round_keys[ round_constant_index ] );

		round_constant_index++;
	}
	if( key_bit_size == 128 )
	{
		internal_context->number_of_round_keys = 10;

		for( round_constant_index = 0;
		     round_constant_index < 10;
		     round_constant_index++ )
		{
			round_keys[ 4 ]  = libcaes_round_constants[ round_constant_index ];
			round_keys[ 4 ] ^= round_keys[ 0 ];
			round_keys[ 4 ] ^= (uint32_t) libcaes_forward_substitution_box[ ( round_keys[ 3 ] >> 8 ) & 0xff ];
			round_keys[ 4 ] ^= ( (uint32_t) libcaes_forward_substitution_box[ ( round_keys[ 3 ] >> 16 ) & 0xff ] ) << 8;
			round_keys[ 4 ] ^= ( (uint32_t) libcaes_forward_substitution_box[ ( round_keys[ 3 ] >> 24 ) & 0xff ] ) << 16;
			round_keys[ 4 ] ^= ( (uint32_t) libcaes_forward_substitution_box[ ( round_keys[ 3 ] ) & 0xff ] ) << 24;

			round_keys[ 5 ] = round_keys[ 1 ] ^ round_keys[ 4 ];
			round_keys[ 6 ] = round_keys[ 2 ] ^ round_keys[ 5 ];
			round_keys[ 7 ] = round_keys[ 3 ] ^ round_keys[ 6 ];

			round_keys += 4;
		}
	}
	else if( key_bit_size == 192 )
	{
		internal_context->number_of_round_keys = 12;

		for( round_constant_index = 0;
		     round_constant_index < 8;
		     round_constant_index++ )
		{
			round_keys[ 6 ]  = libcaes_round_constants[ round_constant_index ];
			round_keys[ 6 ] ^= round_keys[ 0 ];
			round_keys[ 6 ] ^= (uint32_t) libcaes_forward_substitution_box[ ( round_keys[ 5 ] >> 8 ) & 0xff ];
			round_keys[ 6 ] ^= ( (uint32_t) libcaes_forward_substitution_box[ ( round_keys[ 5 ] >> 16 ) & 0xff ] ) << 8;
			round_keys[ 6 ] ^= ( (uint32_t) libcaes_forward_substitution_box[ ( round_keys[ 5 ] >> 24 ) & 0xff ] ) << 16;
			round_keys[ 6 ] ^= ( (uint32_t) libcaes_forward_substitution_box[ ( round_keys[ 5 ] ) & 0xff ] ) << 24;

			round_keys[ 7 ]  = round_keys[ 1 ] ^ round_keys[ 6 ];
			round_keys[ 8 ]  = round_keys[ 2 ] ^ round_keys[ 7 ];
			round_keys[ 9 ]  = round_keys[ 3 ] ^ round_keys[ 8 ];
			round_keys[ 10 ] = round_keys[ 4 ] ^ round_keys[ 9 ];
			round_keys[ 11 ] = round_keys[ 5 ] ^ round_keys[ 10 ];

			round_keys += 6;
		}
	}
	else if( key_bit_size == 256 )
	{
		internal_context->number_of_round_keys = 14;

		for( round_constant_index = 0;
		     round_constant_index < 7;
		     round_constant_index++ )
		{
			round_keys[ 8 ]  = libcaes_round_constants[ round_constant_index ];
			round_keys[ 8 ] ^= round_keys[ 0 ];
			round_keys[ 8 ] ^= (uint32_t) libcaes_forward_substitution_box[ ( round_keys[ 7 ] >> 8 ) & 0xff ];
			round_keys[ 8 ] ^= ( (uint32_t) libcaes_forward_substitution_box[ ( round_keys[ 7 ] >> 16 ) & 0xff ] ) << 8;
			round_keys[ 8 ] ^= ( (uint32_t) libcaes_forward_substitution_box[ ( round_keys[ 7 ] >> 24 ) & 0xff ] ) << 16;
			round_keys[ 8 ] ^= ( (uint32_t) libcaes_forward_substitution_box[ ( round_keys[ 7 ] ) & 0xff ] ) << 24;

			round_keys[ 9 ]  = round_keys[ 1 ] ^ round_keys[ 8 ];
			round_keys[ 10 ] = round_keys[ 2 ] ^ round_keys[ 9 ];
			round_keys[ 11 ] = round_keys[ 3 ] ^ round_keys[ 10 ];

			round_keys[ 12 ]  = round_keys[ 4 ];
			round_keys[ 12 ] ^= (uint32_t) libcaes_forward_substitution_box[ ( round_keys[ 11 ] ) & 0xff ];
			round_keys[ 12 ] ^= ( (uint32_t) libcaes_forward_substitution_box[ ( round_keys[ 11 ] >> 8 ) & 0xff ] ) << 8;
			round_keys[ 12 ] ^= ( (uint32_t) libcaes_forward_substitution_box[ ( round_keys[ 11 ] >> 16 ) & 0xff ] ) << 16;
			round_keys[ 12 ] ^= ( (uint32_t) libcaes_forward_substitution_box[ ( round_keys[ 11 ] >> 24 ) & 0xff ] ) << 24;

			round_keys[ 13 ] = round_keys[ 5 ] ^ round_keys[ 12 ];
			round_keys[ 14 ] = round_keys[ 6 ] ^ round_keys[ 13 ];
			round_keys[ 15 ] = round_keys[ 7 ] ^ round_keys[ 14 ];

			round_keys += 8;
		}
	}
	return( 1 );
}

#endif /* !defined( LIBCAES_HAVE_AES_SUPPORT ) */

/* De- or encrypts a block of data using AES-CBC (Cipher Block Chaining)
 * The size must be a multitude of the AES block size (16 byte)
 * Returns 1 if successful or -1 on error
 */
int libcaes_crypt_cbc(
     libcaes_context_t *context,
     int mode,
     const uint8_t *initialization_vector,
     size_t initialization_vector_size,
     const uint8_t *input_data,
     size_t input_data_size,
     uint8_t *output_data,
     size_t output_data_size,
     libcerror_error_t **error )
{
#if !defined( LIBCAES_HAVE_AES_SUPPORT )
	uint8_t internal_initialization_vector[ 16 ];
#endif

#if defined( LIBCAES_HAVE_AES_SUPPORT )
	libcaes_internal_context_t *internal_context = NULL;
#endif
	static char *function                        = "libcaes_crypt_cbc";

#if defined( WINAPI ) && ( WINVER >= 0x0600 )
	uint8_t block_data[ 128 ];

	DWORD block_length                           = 0;
	DWORD cipher_mode                            = CRYPT_MODE_CBC;
	DWORD error_code                             = 0;
	DWORD parameter_data_size                    = 0;
	DWORD safe_output_data_size                  = 0;

#elif defined( HAVE_LIBCRYPTO ) && defined( HAVE_OPENSSL_AES_H )
	uint8_t safe_initialization_vector[ 16 ];

	int safe_mode                                = 0;

#elif defined( HAVE_LIBCRYPTO ) && defined( HAVE_OPENSSL_EVP_H )
	uint8_t block_data[ EVP_MAX_BLOCK_LENGTH ];

	const EVP_CIPHER *cipher                     = NULL;
	int safe_output_data_size                    = 0;

#elif !defined( LIBCAES_HAVE_AES_SUPPORT )
	size_t data_offset                           = 0;

#if !defined( LIBCAES_UNFOLLED_LOOPS )
	uint8_t block_index                          = 0;
#endif
#endif

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
#if defined( LIBCAES_HAVE_AES_SUPPORT )
	internal_context = (libcaes_internal_context_t *) context;
#endif

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
	if( initialization_vector == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid initialization vector.",
		 function );

		return( -1 );
	}
	if( initialization_vector_size != 16 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid initialization vector size value out of bounds.",
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
	/* Check if the input data size is a multitude of 16-byte
	 */
	if( ( input_data_size & (size_t) 0x0f ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid input data size value out of bounds.",
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
#if defined( WINAPI ) && ( WINVER >= 0x0600 )
	if( input_data_size > (size_t) UINT32_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid input data size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( CryptSetKeyParam(
	     internal_context->key,
	     KP_MODE,
	     (BYTE *) &cipher_mode,
	     0 ) == 0 )
	{
		error_code = GetLastError();

		libcerror_system_set_error(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 error_code,
		 "%s: unable to set cipher mode key parameter.",
		 function );

		return( -1 );
	}
	if( CryptSetKeyParam(
	     internal_context->key,
	     KP_IV,
	     (BYTE *) initialization_vector,
	     0 ) == 0 )
	{
		error_code = GetLastError();

		libcerror_system_set_error(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 error_code,
		 "%s: unable to set initialization vector key parameter.",
		 function );

		return( -1 );
	}
	parameter_data_size = sizeof( DWORD );

	if( CryptGetKeyParam(
	     internal_context->key,
	     KP_BLOCKLEN,
	     (BYTE *) &block_length,
	     (DWORD *) &parameter_data_size,
	     0 ) == 0 )
	{
		error_code = GetLastError();

		libcerror_system_set_error(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 error_code,
		 "%s: unable to retrieve block length key parameter.",
		 function );

		return( -1 );
	}
	if( block_length > 128 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported block length.",
		 function );

		return( -1 );
	}
	if( memory_set(
	     block_data,
	     0,
	     32 ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear input block data.",
		 function );

		return( -1 );
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
	safe_output_data_size = (DWORD) input_data_size;

	if( mode == LIBCAES_CRYPT_MODE_ENCRYPT )
	{
		if( CryptEncrypt(
		     internal_context->key,
		     (HCRYPTHASH) NULL,
		     FALSE,
		     0,
		     output_data,
		     &safe_output_data_size,
		     (DWORD) input_data_size ) == 0 )
		{
			error_code = GetLastError();

			libcerror_system_set_error(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ENCRYPTION,
			 LIBCERROR_ENCRYPTION_ERROR_ENCRYPT_FAILED,
			 error_code,
			 "%s: unable to encrypt output data.",
			 function );

			return( -1 );
		}
		if( safe_output_data_size < input_data_size )
		{
			input_data_size      -= safe_output_data_size;
			safe_output_data_size = 128;

			/* Just ignore the output of this function
			 */
			CryptEncrypt(
			 internal_context->key,
			 (HCRYPTHASH) NULL,
			 TRUE,
			 0,
			 block_data,
			 &safe_output_data_size,
			 (DWORD) input_data_size );
		}
	}
	else
	{
		if( CryptDecrypt(
		     internal_context->key,
		     (HCRYPTHASH) NULL,
		     FALSE,
		     0,
		     output_data,
		     &safe_output_data_size ) == 0 )
		{
			error_code = GetLastError();

			libcerror_system_set_error(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ENCRYPTION,
			 LIBCERROR_ENCRYPTION_ERROR_ENCRYPT_FAILED,
			 error_code,
			 "%s: unable to decrypt output data.",
			 function );

			return( -1 );
		}
		if( safe_output_data_size < input_data_size )
		{
			safe_output_data_size = (DWORD) ( input_data_size - safe_output_data_size );

			/* Just ignore the output of this function
			 */
			CryptDecrypt(
			 internal_context->key,
			 (HCRYPTHASH) NULL,
			 TRUE,
			 0,
			 block_data,
			 &safe_output_data_size );
		}
	}

#elif defined( HAVE_LIBCRYPTO ) && defined( HAVE_OPENSSL_AES_H )
	if( mode == LIBCAES_CRYPT_MODE_ENCRYPT )
	{
		safe_mode = AES_ENCRYPT;
	}
	else
	{
		safe_mode = AES_DECRYPT;
	}
	/* AES_cbc_encrypt overwrites the data in the initialization vector
	 */
	if( memory_copy(
	     safe_initialization_vector,
	     initialization_vector,
	     16 ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
		 "%s: unable to copy initialization vector.",
		 function );

		return( -1 );
	}
	AES_cbc_encrypt(
	 (unsigned char *) input_data,
	 (unsigned char *) output_data,
	 input_data_size,
	 &( internal_context->key ),
	 (unsigned char *) safe_initialization_vector,
	 safe_mode );

	if( memory_set(
	     safe_initialization_vector,
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

#elif defined( HAVE_LIBCRYPTO ) && defined( HAVE_OPENSSL_EVP_H )
	if( input_data_size > (size_t) INT_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid input data size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( output_data_size > (size_t) INT_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid output data size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( memory_set(
	     block_data,
	     0,
	     EVP_MAX_BLOCK_LENGTH ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear input block data.",
		 function );

		return( -1 );
	}
	if( internal_context->key_bit_size == 128 )
	{
		cipher = EVP_aes_128_cbc();
	}
	else if( internal_context->key_bit_size == 192 )
	{
		cipher = EVP_aes_192_cbc();
	}
	else if( internal_context->key_bit_size == 256 )
	{
		cipher = EVP_aes_256_cbc();
	}
	if( EVP_CipherInit_ex(
	     &( internal_context->evp_context ),
	     cipher,
	     NULL,
	     (unsigned char *) internal_context->key,
	     (unsigned char *) initialization_vector,
	     mode ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to initialize cipher.",
		 function );

		return( -1 );
	}
	if( EVP_CipherUpdate(
	     &( internal_context->evp_context ),
	     (unsigned char *) output_data,
	     &safe_output_data_size,
	     (unsigned char *) input_data,
	     input_data_size ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to update cipher.",
		 function );

		return( -1 );
	}
	/* Just ignore the output of this function
	 */
	EVP_CipherFinal_ex(
	 &( internal_context->evp_context ),
	 (unsigned char *) block_data,
	 &safe_output_data_size );

#else
	if( memory_copy(
	     internal_initialization_vector,
	     initialization_vector,
	     16 ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
		 "%s: unable to copy initialization vector.",
		 function );

		goto on_error;
	}
	if( mode == LIBCAES_CRYPT_MODE_ENCRYPT )
	{
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

			goto on_error;
		}
	}
	while( data_offset < input_data_size )
	{
		if( mode == LIBCAES_CRYPT_MODE_ENCRYPT )
		{
#if defined( LIBCAES_UNFOLLED_LOOPS )
			output_data[ data_offset++ ] ^= internal_initialization_vector[ 0 ];
			output_data[ data_offset++ ] ^= internal_initialization_vector[ 1 ];
			output_data[ data_offset++ ] ^= internal_initialization_vector[ 2 ];
			output_data[ data_offset++ ] ^= internal_initialization_vector[ 3 ];
			output_data[ data_offset++ ] ^= internal_initialization_vector[ 4 ];
			output_data[ data_offset++ ] ^= internal_initialization_vector[ 5 ];
			output_data[ data_offset++ ] ^= internal_initialization_vector[ 6 ];
			output_data[ data_offset++ ] ^= internal_initialization_vector[ 7 ];
			output_data[ data_offset++ ] ^= internal_initialization_vector[ 8 ];
			output_data[ data_offset++ ] ^= internal_initialization_vector[ 9 ];
			output_data[ data_offset++ ] ^= internal_initialization_vector[ 10 ];
			output_data[ data_offset++ ] ^= internal_initialization_vector[ 11 ];
			output_data[ data_offset++ ] ^= internal_initialization_vector[ 12 ];
			output_data[ data_offset++ ] ^= internal_initialization_vector[ 13 ];
			output_data[ data_offset++ ] ^= internal_initialization_vector[ 14 ];
			output_data[ data_offset++ ] ^= internal_initialization_vector[ 15 ];
#else
			for( block_index = 0;
			     block_index < 16;
			     block_index++ )
			{
				output_data[ data_offset++ ] ^= internal_initialization_vector[ block_index ];
			}
#endif
			data_offset -= 16;

			if( libcaes_crypt_ecb(
			     context,
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
				 LIBCERROR_ENCRYPTION_ERROR_ENCRYPT_FAILED,
				 "%s: unable to encrypt output data.",
				 function );

				goto on_error;
			}
			if( memory_copy(
			     internal_initialization_vector,
			     &( output_data[ data_offset ] ),
			     16 ) == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_MEMORY,
				 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
				 "%s: unable to copy enrypted output data to initialization vector.",
				 function );

				goto on_error;
			}
		}
		else
		{
			if( libcaes_crypt_ecb(
			     context,
			     mode,
			     &( input_data[ data_offset ] ),
			     16,
			     &( output_data[ data_offset ] ),
			     16,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ENCRYPTION,
				 LIBCERROR_ENCRYPTION_ERROR_GENERIC,
				 "%s: unable to decrypt output data.",
				 function );

				goto on_error;
			}
#if defined( LIBCAES_UNFOLLED_LOOPS )
			output_data[ data_offset++ ] ^= internal_initialization_vector[ 0 ];
			output_data[ data_offset++ ] ^= internal_initialization_vector[ 1 ];
			output_data[ data_offset++ ] ^= internal_initialization_vector[ 2 ];
			output_data[ data_offset++ ] ^= internal_initialization_vector[ 3 ];
			output_data[ data_offset++ ] ^= internal_initialization_vector[ 4 ];
			output_data[ data_offset++ ] ^= internal_initialization_vector[ 5 ];
			output_data[ data_offset++ ] ^= internal_initialization_vector[ 6 ];
			output_data[ data_offset++ ] ^= internal_initialization_vector[ 7 ];
			output_data[ data_offset++ ] ^= internal_initialization_vector[ 8 ];
			output_data[ data_offset++ ] ^= internal_initialization_vector[ 9 ];
			output_data[ data_offset++ ] ^= internal_initialization_vector[ 10 ];
			output_data[ data_offset++ ] ^= internal_initialization_vector[ 11 ];
			output_data[ data_offset++ ] ^= internal_initialization_vector[ 12 ];
			output_data[ data_offset++ ] ^= internal_initialization_vector[ 13 ];
			output_data[ data_offset++ ] ^= internal_initialization_vector[ 14 ];
			output_data[ data_offset++ ] ^= internal_initialization_vector[ 15 ];
#else
			for( block_index = 0;
			     block_index < 16;
			     block_index++ )
			{
				output_data[ data_offset++ ] ^= internal_initialization_vector[ block_index ];
			}
#endif
			data_offset -= 16;

			if( memory_copy(
			     internal_initialization_vector,
			     &( input_data[ data_offset ] ),
			     16 ) == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_MEMORY,
				 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
				 "%s: unable to copy enrypted input data to initialization vector.",
				 function );

				goto on_error;
			}
		}
		data_offset += 16;
  	}
	if( memory_set(
	     internal_initialization_vector,
	     0,
	     16 ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear initialization vector.",
		 function );

		goto on_error;
	}
#endif
	return( 1 );

#if !defined( LIBCAES_HAVE_AES_SUPPORT )
on_error:
	memory_set(
	 internal_initialization_vector,
	 0,
	 16 );

	return( -1 );
#endif
}

/* De- or encrypts a block of data using AES-CCM (Counter with CBC-MAC)
 * Note that the key must be set in encryption mode (LIBCAES_CRYPT_MODE_ENCRYPT) for both de- and encryption.
 * Returns 1 if successful or -1 on error
 */
int libcaes_crypt_ccm(
     libcaes_context_t *context,
     int mode,
     const uint8_t *nonce,
     size_t nonce_size,
     const uint8_t *input_data,
     size_t input_data_size,
     uint8_t *output_data,
     size_t output_data_size,
     libcerror_error_t **error )
{
	uint8_t block_data[ 16 ];
	uint8_t internal_initialization_vector[ 16 ];

	static char *function      = "libcaes_crypt_ccm";
	size_t data_offset         = 0;
	size_t remaining_data_size = 0;
	uint8_t block_index        = 0;

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
	if( nonce == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid nonce.",
		 function );

		return( -1 );
	}
	if( nonce_size >= (size_t) 15 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid nonce size value out of bounds.",
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
	/* The IV consists of:
	 * 1 byte size value formatted as: 15 - nonce size - 1
	 * a maximum of 14 bytes containing nonce bytes
	 * 1 byte counter
	 */
	if( memory_set(
	     internal_initialization_vector,
	     0,
	     16 ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear initialization vector.",
		 function );

		goto on_error;
	}
	if( memory_copy(
	     &( internal_initialization_vector[ 1 ] ),
	     nonce,
	     nonce_size ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
		 "%s: unable to copy nonce to initialization vector.",
		 function );

		goto on_error;
	}
	internal_initialization_vector[ 0 ] = 15 - (uint8_t) nonce_size - 1;

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

		goto on_error;
	}
	while( ( data_offset + 16 ) < input_data_size )
	{
		if( libcaes_crypt_ecb(
		     context,
		     LIBCAES_CRYPT_MODE_ENCRYPT,
		     internal_initialization_vector,
		     16,
		     block_data,
		     16,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ENCRYPTION,
			 LIBCERROR_ENCRYPTION_ERROR_ENCRYPT_FAILED,
			 "%s: unable to encrypt initialization vector.",
			 function );

			goto on_error;
		}
#if defined( LIBCAES_UNFOLLED_LOOPS )
		output_data[ data_offset++ ] ^= block_data[ 0 ];
		output_data[ data_offset++ ] ^= block_data[ 1 ];
		output_data[ data_offset++ ] ^= block_data[ 2 ];
		output_data[ data_offset++ ] ^= block_data[ 3 ];
		output_data[ data_offset++ ] ^= block_data[ 4 ];
		output_data[ data_offset++ ] ^= block_data[ 5 ];
		output_data[ data_offset++ ] ^= block_data[ 6 ];
		output_data[ data_offset++ ] ^= block_data[ 7 ];
		output_data[ data_offset++ ] ^= block_data[ 8 ];
		output_data[ data_offset++ ] ^= block_data[ 9 ];
		output_data[ data_offset++ ] ^= block_data[ 10 ];
		output_data[ data_offset++ ] ^= block_data[ 11 ];
		output_data[ data_offset++ ] ^= block_data[ 12 ];
		output_data[ data_offset++ ] ^= block_data[ 13 ];
		output_data[ data_offset++ ] ^= block_data[ 14 ];
		output_data[ data_offset++ ] ^= block_data[ 15 ];
#else
		for( block_index = 0;
		     block_index < 16;
		     block_index++ )
		{
			output_data[ data_offset++ ] ^= block_data[ block_index ];
		}
#endif
		internal_initialization_vector[ 15 ] += 1;
	}
	if( data_offset < input_data_size )
	{
		remaining_data_size = input_data_size - data_offset;

		if( libcaes_crypt_ecb(
		     context,
		     LIBCAES_CRYPT_MODE_ENCRYPT,
		     internal_initialization_vector,
		     16,
		     block_data,
		     16,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ENCRYPTION,
			 LIBCERROR_ENCRYPTION_ERROR_ENCRYPT_FAILED,
			 "%s: unable to encrypt initialization vector.",
			 function );

			goto on_error;
		}
		for( block_index = 0;
		     block_index < (uint8_t) remaining_data_size;
		     block_index++ )
		{
			output_data[ data_offset++ ] ^= block_data[ block_index ];
		}
	}
	if( memory_set(
	     block_data,
	     0,
	     16 ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear block data.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_initialization_vector,
	     0,
	     16 ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear initialization vector.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	memory_set(
	 block_data,
	 0,
	 16 );

	memory_set(
	 internal_initialization_vector,
	 0,
	 16 );

	return( -1 );
}

#ifdef TODO

/* De- or encrypts a block of data using AES-CFB (Cipher Feedback Mode)
 * Returns 1 if successful or -1 on error
 */
int libcaes_crypt_cfb(
     libcaes_context_t *context,
     int mode,
     const uint8_t *initialization_vector,
     size_t initialization_vector_size,
     size_t *initialization_vector_index,
     const uint8_t *input_data,
     size_t input_data_size,
     uint8_t *output_data,
     size_t output_data_size,
     libcerror_error_t **error )
{
	static char *function = "libcaes_crypt_cfb";
	size_t data_offset    = 0;

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
	if( initialization_vector == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid initialization vector.",
		 function );

		return( -1 );
	}
	if( initialization_vector_size != 16 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid initialization vector size value out of bounds.",
		 function );

		return( -1 );
	}
	if( initialization_vector_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid initialization vector index.",
		 function );

		return( -1 );
	}
	if( *initialization_vector_index >= initialization_vector_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid initialization vector index value out of bounds.",
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
	/* Check if the input data size is a multitude of 16-byte
	 */
	if( ( input_data_size & (size_t) 0x0f ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid input data size value out of bounds.",
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
	if( output_data_size > input_data_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid ouput data size value exceeds input data size.",
		 function );

		return( -1 );
	}
	for( data_offset = 0;
	     data_offset < input_data_size;
	     data_offset++ )
	{
		if( *initialization_vector_index == 0 )
		{
			if( libcaes_crypt_ecb(
			     context,
			     mode,
			     initialization_vector,
			     initialization_vector_size,
			     initialization_vector,
			     initialization_vector_size,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ENCRYPTION,
				 LIBCERROR_ENCRYPTION_ERROR_GENERIC,
				 "%s: unable to de/encrypt initialization vector.",
				 function );

				return( -1 );
			}
		}
		output_data[ data_offset ] = input_data[ data_offset ]
		                           ^ initialization_vector[ *initialization_vector_index ];

		if( mode == LIBCAES_CRYPT_MODE_ENCRYPT )
		{
			initialization_vector[ *initialization_vector_index ] = output_data[ data_offset ];
		}
		else
		{
			initialization_vector[ *initialization_vector_index ] = input_data[ data_offset ];
		}
		*initialization_vector_index = ( *initialization_vector_index + 1 ) & 0x0f;
	}
	return( 1 );
}

#endif /* TODO */

/* De- or encrypts a block of data using AES-ECB (Electronic CodeBook)
 * The size must be a multitude of the AES block size (16 byte)
 * Returns 1 if successful or -1 on error
 */
int libcaes_crypt_ecb(
     libcaes_context_t *context,
     int mode,
     const uint8_t *input_data,
     size_t input_data_size,
     uint8_t *output_data,
     size_t output_data_size,
     libcerror_error_t **error )
{
	libcaes_internal_context_t *internal_context = NULL;
	static char *function                        = "libcaes_crypt_ecb";
	int result                                   = 1;

#if defined( WINAPI ) && ( WINVER >= 0x0600 )
	uint8_t block_data[ 128 ];

	DWORD block_length                           = 0;
	DWORD cipher_mode                            = CRYPT_MODE_ECB;
	DWORD error_code                             = 0;
	DWORD parameter_data_size                    = 0;
	DWORD safe_output_data_size                  = 0;

#elif defined( HAVE_LIBCRYPTO ) && defined( HAVE_OPENSSL_AES_H )
	int safe_mode                                = 0;

#elif defined( HAVE_LIBCRYPTO ) && defined( HAVE_OPENSSL_EVP_H )
	uint8_t block_data[ EVP_MAX_BLOCK_LENGTH ];

	const EVP_CIPHER *cipher                     = NULL;
	int safe_output_data_size                    = 0;

#elif !defined( LIBCAES_HAVE_AES_SUPPORT )
	uint32_t cipher_values_32bit[ 4 ];
	uint32_t values_32bit[ 4 ];

	uint32_t *round_keys                         = NULL;
	size_t data_offset                           = 0;
	uint32_t substitution_value                  = 0;
	uint32_t table_value                         = 0;
	int round_key_iterator                       = 0;
#endif

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
	internal_context = (libcaes_internal_context_t *) context;

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
	if( output_data_size < 16 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: invalid output data size value too small.",
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
#if defined( WINAPI ) && ( WINVER >= 0x0600 )
	if( input_data_size > (size_t) UINT32_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid input data size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( CryptSetKeyParam(
	     internal_context->key,
	     KP_MODE,
	     (BYTE *) &cipher_mode,
	     0 ) == 0 )
	{
		error_code = GetLastError();

		libcerror_system_set_error(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 error_code,
		 "%s: unable to set cipher mode key parameter.",
		 function );

		return( -1 );
	}
	parameter_data_size = sizeof( DWORD );

	if( CryptGetKeyParam(
	     internal_context->key,
	     KP_BLOCKLEN,
	     (BYTE *) &block_length,
	     (DWORD *) &parameter_data_size,
	     0 ) == 0 )
	{
		error_code = GetLastError();

		libcerror_system_set_error(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 error_code,
		 "%s: unable to retrieve block length key parameter.",
		 function );

		return( -1 );
	}
	if( block_length > 128 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported block length.",
		 function );

		return( -1 );
	}
	if( memory_set(
	     block_data,
	     0,
	     32 ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear input block data.",
		 function );

		return( -1 );
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
	safe_output_data_size = (DWORD) input_data_size;

	if( mode == LIBCAES_CRYPT_MODE_ENCRYPT )
	{
		if( CryptEncrypt(
		     internal_context->key,
		     (HCRYPTHASH) NULL,
		     FALSE,
		     0,
		     output_data,
		     &safe_output_data_size,
		     (DWORD) input_data_size ) == 0 )
		{
			error_code = GetLastError();

			libcerror_system_set_error(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ENCRYPTION,
			 LIBCERROR_ENCRYPTION_ERROR_ENCRYPT_FAILED,
			 error_code,
			 "%s: unable to encrypt output data.",
			 function );

			return( -1 );
		}
		if( safe_output_data_size < input_data_size )
		{
			input_data_size      -= safe_output_data_size;
			safe_output_data_size = 128;

			/* Just ignore the output of this function
			 */
			CryptEncrypt(
			 internal_context->key,
			 (HCRYPTHASH) NULL,
			 TRUE,
			 0,
			 block_data,
			 &safe_output_data_size,
			 (DWORD) input_data_size );
		}
	}
	else
	{
		if( CryptDecrypt(
		     internal_context->key,
		     (HCRYPTHASH) NULL,
		     FALSE,
		     0,
		     output_data,
		     &safe_output_data_size ) == 0 )
		{
			error_code = GetLastError();

			libcerror_system_set_error(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ENCRYPTION,
			 LIBCERROR_ENCRYPTION_ERROR_ENCRYPT_FAILED,
			 error_code,
			 "%s: unable to decrypt output data.",
			 function );

			return( -1 );
		}
		if( safe_output_data_size < input_data_size )
		{
			safe_output_data_size = (DWORD) ( input_data_size - safe_output_data_size );

			/* Just ignore the output of this function
			 */
			CryptDecrypt(
			 internal_context->key,
			 (HCRYPTHASH) NULL,
			 TRUE,
			 0,
			 block_data,
			 &safe_output_data_size );
		}
	}

#elif defined( HAVE_LIBCRYPTO ) && defined( HAVE_OPENSSL_AES_H )
	if( mode == LIBCAES_CRYPT_MODE_ENCRYPT )
	{
		safe_mode = AES_ENCRYPT;
	}
	else
	{
		safe_mode = AES_DECRYPT;
	}
	AES_ecb_encrypt(
	 (unsigned char *) input_data,
	 (unsigned char *) output_data,
	 &( internal_context->key ),
	 safe_mode );

#elif defined( HAVE_LIBCRYPTO ) && defined( HAVE_OPENSSL_EVP_H )
	if( input_data_size > (size_t) INT_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid input data size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( output_data_size > (size_t) INT_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid output data size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( memory_set(
	     block_data,
	     0,
	     EVP_MAX_BLOCK_LENGTH ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear block data.",
		 function );

		return( -1 );
	}
	if( internal_context->key_bit_size == 128 )
	{
		cipher = EVP_aes_128_ecb();
	}
	else if( internal_context->key_bit_size == 192 )
	{
		cipher = EVP_aes_192_ecb();
	}
	else if( internal_context->key_bit_size == 256 )
	{
		cipher = EVP_aes_256_ecb();
	}
	if( EVP_CipherInit_ex(
	     &( internal_context->evp_context ),
	     cipher,
	     NULL,
	     (unsigned char *) internal_context->key,
	     NULL,
	     mode ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to initialize cipher.",
		 function );

		return( -1 );
	}
	if( EVP_CipherUpdate(
	     &( internal_context->evp_context ),
	     (unsigned char *) output_data,
	     &safe_output_data_size,
	     (unsigned char *) input_data,
	     16 ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to update cipher.",
		 function );

		return( -1 );
	}
	/* Just ignore the output of this function
	 */
	EVP_CipherFinal_ex(
	 &( internal_context->evp_context ),
	 (unsigned char *) block_data,
	 &safe_output_data_size );

#else
	while( data_offset < input_data_size )
	{
		byte_stream_copy_to_uint32_little_endian(
		 &( input_data[ data_offset ] ),
		 values_32bit[ 0 ] );

		byte_stream_copy_to_uint32_little_endian(
		 &( input_data[ data_offset + 4 ] ),
		 values_32bit[ 1 ] );

		byte_stream_copy_to_uint32_little_endian(
		 &( input_data[ data_offset + 8 ] ),
		 values_32bit[ 2 ] );

		byte_stream_copy_to_uint32_little_endian(
		 &( input_data[ data_offset + 12 ] ),
		 values_32bit[ 3 ] );

		round_keys = internal_context->round_keys;

		values_32bit[ 0 ] ^= round_keys[ 0 ];
		values_32bit[ 1 ] ^= round_keys[ 1 ];
		values_32bit[ 2 ] ^= round_keys[ 2 ];
		values_32bit[ 3 ] ^= round_keys[ 3 ];

		round_keys += 4;

		if( mode == LIBCAES_CRYPT_MODE_ENCRYPT )
		{
			for( round_key_iterator = ( internal_context->number_of_round_keys / 2 );
			     round_key_iterator > 1;
			     round_key_iterator-- )
			{
				libcaes_calculate_forward_table_round(
				 round_keys,
				 cipher_values_32bit,
				 values_32bit,
				 table_value );

				round_keys += 4;

				libcaes_calculate_forward_table_round(
				 round_keys,
				 values_32bit,
				 cipher_values_32bit,
				 table_value );

				round_keys += 4;
			}
			libcaes_calculate_forward_table_round(
			 round_keys,
			 cipher_values_32bit,
			 values_32bit,
			 table_value );

			round_keys += 4;

			libcaes_calculate_forward_substitution_round(
			 round_keys,
			 values_32bit,
			 cipher_values_32bit,
			 substitution_value );
		}
		else
		{
			for( round_key_iterator = ( internal_context->number_of_round_keys / 2 );
			     round_key_iterator > 1;
			     round_key_iterator-- )
			{
				libcaes_calculate_reverse_table_round(
				 round_keys,
				 cipher_values_32bit,
				 values_32bit,
				 table_value );

				round_keys += 4;

				libcaes_calculate_reverse_table_round(
				 round_keys,
				 values_32bit,
				 cipher_values_32bit,
				 table_value );

				round_keys += 4;
			}
			libcaes_calculate_reverse_table_round(
			 round_keys,
			 cipher_values_32bit,
			 values_32bit,
			 table_value );

			round_keys += 4;

			libcaes_calculate_reverse_substitution_round(
			 round_keys,
			 values_32bit,
			 cipher_values_32bit,
			 table_value );
		}
		byte_stream_copy_from_uint32_little_endian(
		 &( output_data[ data_offset ] ),
		 values_32bit[ 0 ] );

		byte_stream_copy_from_uint32_little_endian(
		 &( output_data[ data_offset + 4 ] ),
		 values_32bit[ 1 ] );

		byte_stream_copy_from_uint32_little_endian(
		 &( output_data[ data_offset + 8 ] ),
		 values_32bit[ 2 ] );

		byte_stream_copy_from_uint32_little_endian(
		 &( output_data[ data_offset + 12 ] ),
		 values_32bit[ 3 ] );

		if( memory_set(
		     values_32bit,
		     0,
		     sizeof( uint32_t ) * 4 ) == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_MEMORY,
			 LIBCERROR_MEMORY_ERROR_SET_FAILED,
			 "%s: unable to clear values 32-bit.",
			 function );

			result = -1;
		}
		if( memory_set(
		     cipher_values_32bit,
		     0,
		     sizeof( uint32_t ) * 4 ) == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_MEMORY,
			 LIBCERROR_MEMORY_ERROR_SET_FAILED,
			 "%s: unable to clear cipher values 32-bit.",
			 function );

			result = -1;
		}
		data_offset += 16;
	}
#endif
	return( result );
}

