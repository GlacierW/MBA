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

#if !defined( _LIBQCOW_ENCRYPTION_H )
#define _LIBQCOW_ENCRYPTION_H

#include <common.h>
#include <types.h>

#include "libqcow_libcaes.h"
#include "libqcow_libcerror.h"

#if defined( __cplusplus )
extern "C" {
#endif

enum LIBQCOW_ENCYPTION_CRYPT_MODES
{
	LIBQCOW_ENCYPTION_CRYPT_MODE_DECRYPT   = 0,
	LIBQCOW_ENCYPTION_CRYPT_MODE_ENCRYPT   = 1
};

typedef struct libqcow_encryption_context libqcow_encryption_context_t;

struct libqcow_encryption_context
{
	/* The encryption method
	 */
	uint32_t method;

	/* The (AES) decryption context
	 */
	libcaes_context_t *decryption_context;

	/* The (AES) encryption context
	 */
	libcaes_context_t *encryption_context;
};

int libqcow_encryption_initialize(
     libqcow_encryption_context_t **context,
     uint32_t method,
     libcerror_error_t **error );

int libqcow_encryption_free(
     libqcow_encryption_context_t **context,
     libcerror_error_t **error );

int libqcow_encryption_set_keys(
     libqcow_encryption_context_t *context,
     const uint8_t *key,
     size_t key_size,
     libcerror_error_t **error );

int libqcow_encryption_crypt(
     libqcow_encryption_context_t *context,
     int mode,
     const uint8_t *input_data,
     size_t input_data_size,
     uint8_t *output_data,
     size_t output_data_size,
     uint64_t block_key,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

