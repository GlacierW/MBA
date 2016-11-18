/*
 * The internal definitions
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

#if !defined( LIBCAES_INTERNAL_DEFINITIONS_H )
#define LIBCAES_INTERNAL_DEFINITIONS_H

#include <common.h>
#include <types.h>

/* Define HAVE_LOCAL_LIBCAES for local use of libcaes
 */
#if !defined( HAVE_LOCAL_LIBCAES )
#include <libcaes/definitions.h>

/* The definitions in <libcaes/definitions.h> are copied here
 * for local use of libcaes
 */
#else
#define LIBCAES_VERSION				20161106

/* The libcaes version string
 */
#define LIBCAES_VERSION_STRING			"20161106"

/* The crypt modes
 */
enum LIBCAES_CRYPT_MODES
{
	LIBCAES_CRYPT_MODE_DECRYPT		= 0,
	LIBCAES_CRYPT_MODE_ENCRYPT		= 1
};

#endif

#if defined( WINAPI ) && ( WINVER >= 0x0600 )
#define LIBCAES_HAVE_AES_SUPPORT

#elif defined( HAVE_LIBCRYPTO ) && defined( HAVE_OPENSSL_AES_H )
#define LIBCAES_HAVE_AES_SUPPORT

#elif defined( HAVE_LIBCRYPTO ) && defined( HAVE_OPENSSL_EVP_H )
#define LIBCAES_HAVE_AES_SUPPORT

#endif

#if defined( WINAPI )

#if !defined( PROV_RSA_AES )
#define PROV_RSA_AES				24
#endif

#endif /* defined( WINAPI ) */

#define LIBCAES_UNFOLLED_LOOPS

#endif

