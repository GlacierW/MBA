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

#if !defined( _LIBCAES_KEY_H )
#define _LIBCAES_KEY_H

#include <common.h>
#include <types.h>

#include "libcaes_libcerror.h"

#if defined( __cplusplus )
extern "C" {
#endif

#if defined( WINAPI ) && ( WINVER >= 0x0600 )

typedef struct libcaes_key libcaes_key_t;

struct libcaes_key
{
	/* The public key structure (PUBLICKEYSTRUC) aka blob header (BLOBHEADER)
	 */
	PUBLICKEYSTRUC header;

	/* The (key) data size
	 */
	DWORD data_size;

	/* The (key) data
	 */
	BYTE data[ 32 ];
};

int libcaes_key_initialize(
     libcaes_key_t **key,
     libcerror_error_t **error );

int libcaes_key_free(
     libcaes_key_t **key,
     libcerror_error_t **error );

int libcaes_key_set(
     libcaes_key_t *key,
     const uint8_t *key_data,
     size_t bit_size,
     libcerror_error_t **error );

#endif /* defined( WINAPI ) && ( WINVER >= 0x0600 ) */

#ifdef __cplusplus
}
#endif

#endif

