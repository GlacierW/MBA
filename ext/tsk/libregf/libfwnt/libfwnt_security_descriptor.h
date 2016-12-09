/*
 * Windows NT Security Descriptor functions
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

#if !defined( _LIBFWNT_INTERNAL_SECURITY_DESCRIPTOR_H )
#define _LIBFWNT_INTERNAL_SECURITY_DESCRIPTOR_H

#include <common.h>
#include <types.h>

#include "libfwnt_extern.h"
#include "libfwnt_libcerror.h"
#include "libfwnt_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

/* The Windows NT Security Descriptor is formatted as following:
 * header: revision number, padding, control flags
 * owner (SID)
 * group (SID)
 * discretionary access control list (DACL)
 * system access control list (SACL)
 */

typedef struct libfwnt_internal_security_descriptor libfwnt_internal_security_descriptor_t;

struct libfwnt_internal_security_descriptor
{
	/* The revision number
	 */
	uint8_t revision_number;

	/* The owner SID
	 */
	libfwnt_security_identifier_t *owner_sid;

	/* The group SID
	 */
	libfwnt_security_identifier_t *group_sid;

	/* The discretionary ACL
	 */
	libfwnt_access_control_list_t *discretionary_acl;

	/* The system ACL
	 */
	libfwnt_access_control_list_t *system_acl;
};

LIBFWNT_EXTERN \
int libfwnt_security_descriptor_initialize(
     libfwnt_security_descriptor_t **security_descriptor,
     libcerror_error_t **error );

LIBFWNT_EXTERN \
int libfwnt_security_descriptor_free(
     libfwnt_security_descriptor_t **security_descriptor,
     libcerror_error_t **error );

LIBFWNT_EXTERN \
int libfwnt_security_descriptor_copy_from_byte_stream(
     libfwnt_security_descriptor_t *security_descriptor,
     const uint8_t *byte_stream,
     size_t byte_stream_size,
     int byte_order,
     libcerror_error_t **error );

LIBFWNT_EXTERN \
int libfwnt_security_descriptor_get_owner(
     libfwnt_security_descriptor_t *security_descriptor,
     libfwnt_security_identifier_t **security_identifier,
     libcerror_error_t **error );

LIBFWNT_EXTERN \
int libfwnt_security_descriptor_get_group(
     libfwnt_security_descriptor_t *security_descriptor,
     libfwnt_security_identifier_t **security_identifier,
     libcerror_error_t **error );

LIBFWNT_EXTERN \
int libfwnt_security_descriptor_get_discretionary_acl(
     libfwnt_security_descriptor_t *security_descriptor,
     libfwnt_access_control_list_t **access_control_list,
     libcerror_error_t **error );

LIBFWNT_EXTERN \
int libfwnt_security_descriptor_get_system_acl(
     libfwnt_security_descriptor_t *security_descriptor,
     libfwnt_access_control_list_t **access_control_list,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBFWNT_INTERNAL_SECURITY_DESCRIPTOR_H ) */

