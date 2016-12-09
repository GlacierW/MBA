/*
 * Windows NT Security Identifier (SID) functions
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

#if !defined( _LIBFWNT_INTERNAL_SECURITY_IDENTIFIER_H )
#define _LIBFWNT_INTERNAL_SECURITY_IDENTIFIER_H

#include <common.h>
#include <types.h>

#include "libfwnt_extern.h"
#include "libfwnt_libcerror.h"
#include "libfwnt_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

/* The Windows NT Security Identifier (SID) is formatted as following:
 * S-1-5-21-7623811015-3361044348-030300820-1013
 *
 * S - The string is a SID.
 * 1 - The revision number (the version of the SID specification).
 * 5 - The identifier authority value.
 * 21-7623811015-3361044348-030300820 - domain or local computer identifier
 * 1013 – a Relative ID (RID). Any group or user that is not created by default will have a Relative ID of 1000 or greater.
 *
 * The SID is stored as
 * 01       - 05      - 00 00 00 00 00 05 - aa aa aa aa - bb bb bb bb - cc cc cc cc - dd dd dd dd - ee ee ee ee
 * uint8_t  - uint8_t - uint48_t          - uint32_t    - uin32_t     - uin32_t     - uin32_t     - uin32_t
 *                    - (big endian)      - (little endian)
 * revision - number  - authority         - sub authorities
 *
 * The second value contains the number of sub authorities in the SID
 *
 * The maximum number of sub authorities seems to be 15.
 */

typedef struct libfwnt_internal_security_identifier libfwnt_internal_security_identifier_t;

struct libfwnt_internal_security_identifier
{
	/* The revision number
	 */
	uint8_t revision_number;

	/* The authority value
	 */
	uint64_t authority;

	/* The number of sub authoritties
	 */
	uint8_t number_of_sub_authorities;

	/* The sub autoritites
	 */
	uint32_t sub_authority[ 15 ];

	/* Value to indicate if the identifier is managed by the descriptor
	 */
	uint8_t is_managed;
};

LIBFWNT_EXTERN \
int libfwnt_security_identifier_initialize(
     libfwnt_security_identifier_t **security_identifier,
     libcerror_error_t **error );

LIBFWNT_EXTERN \
int libfwnt_security_identifier_free(
     libfwnt_security_identifier_t **security_identifier,
     libcerror_error_t **error );

int libfwnt_internal_security_identifier_free(
     libfwnt_internal_security_identifier_t **internal_security_identifier,
     libcerror_error_t **error );

LIBFWNT_EXTERN \
int libfwnt_security_identifier_copy_from_byte_stream(
     libfwnt_security_identifier_t *security_identifier,
     const uint8_t *byte_stream,
     size_t byte_stream_size,
     int byte_order,
     libcerror_error_t **error );

LIBFWNT_EXTERN \
int libfwnt_security_identifier_get_string_size(
     libfwnt_security_identifier_t *security_identifier,
     size_t *string_size,
     uint32_t string_format_flags,
     libcerror_error_t **error );

LIBFWNT_EXTERN \
int libfwnt_security_identifier_copy_to_utf8_string(
     libfwnt_security_identifier_t *security_identifier,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     uint32_t string_format_flags,
     libcerror_error_t **error );

LIBFWNT_EXTERN \
int libfwnt_security_identifier_copy_to_utf8_string_with_index(
     libfwnt_security_identifier_t *security_identifier,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     size_t *utf8_string_index,
     uint32_t string_format_flags,
     libcerror_error_t **error );

LIBFWNT_EXTERN \
int libfwnt_security_identifier_copy_to_utf16_string(
     libfwnt_security_identifier_t *security_identifier,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     uint32_t string_format_flags,
     libcerror_error_t **error );

LIBFWNT_EXTERN \
int libfwnt_security_identifier_copy_to_utf16_string_with_index(
     libfwnt_security_identifier_t *security_identifier,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     size_t *utf16_string_index,
     uint32_t string_format_flags,
     libcerror_error_t **error );

LIBFWNT_EXTERN \
int libfwnt_security_identifier_copy_to_utf32_string(
     libfwnt_security_identifier_t *security_identifier,
     uint32_t *utf32_string,
     size_t utf32_string_size,
     uint32_t string_format_flags,
     libcerror_error_t **error );

LIBFWNT_EXTERN \
int libfwnt_security_identifier_copy_to_utf32_string_with_index(
     libfwnt_security_identifier_t *security_identifier,
     uint32_t *utf32_string,
     size_t utf32_string_size,
     size_t *utf32_string_index,
     uint32_t string_format_flags,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBFWNT_INTERNAL_SECURITY_IDENTIFIER_H ) */

