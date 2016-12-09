/*
 * The internal definitions
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

#if !defined( LIBFWNT_INTERNAL_DEFINITIONS_H )
#define LIBFWNT_INTERNAL_DEFINITIONS_H

#include <common.h>
#include <types.h>

/* Define HAVE_LOCAL_LIBFWNT for local use of libfwnt
 */
#if !defined( HAVE_LOCAL_LIBFWNT )
#include <libfwnt/definitions.h>

/* The definitions in <libfwnt/definitions.h> are copied here
 * for local use of libfwnt
 */
#else
#include <byte_stream.h>

#define LIBFWNT_VERSION					20161121

/* The version string
 */
#define LIBFWNT_VERSION_STRING				"20161121"


/* The endian definitions
 */
#define LIBFWNT_ENDIAN_BIG				_BYTE_STREAM_ENDIAN_BIG
#define LIBFWNT_ENDIAN_LITTLE				_BYTE_STREAM_ENDIAN_LITTLE

/* The security identifier (SID) authorities
 */
enum LIBFWNT_SECURITY_IDENTIFIER_AUTHORITIES
{
	LIBFWNT_AUTHORITY_NULL				= 0x00,
	LIBFWNT_AUTHORITY_WORLD				= 0x01,
	LIBFWNT_AUTHORITY_LOCAL				= 0x02,
	LIBFWNT_AUTHORITY_CREATOR			= 0x03,
	LIBFWNT_AUTHORITY_NON_UNIQUE			= 0x04,
	LIBFWNT_AUTHORITY_NT_AUTHORITY			= 0x05,

	LIBFWNT_AUTHORITY_EXCHANGE_2007			= 0x09
};

/* The access control types
 */
enum LIBFWNT_ACCESS_CONTROL_TYPES
{
	LIBFWNT_ACCESS_ALLOWED				= 0x00,
	LIBFWNT_ACCESS_DENIED				= 0x01,
	LIBFWNT_SYSTEM_AUDIT				= 0x02,
	LIBFWNT_SYSTEM_ALARM				= 0x03,
	LIBFWNT_ACCESS_ALLOWED_COMPOUND			= 0x04,
	LIBFWNT_ACCESS_ALLOWED_OBJECT			= 0x05,
	LIBFWNT_ACCESS_DENIED_OBJECT			= 0x06,
	LIBFWNT_SYSTEM_AUDIT_OBJECT			= 0x07,
	LIBFWNT_SYSTEM_ALARM_OBJECT			= 0x08,
	LIBFWNT_ACCESS_ALLOWED_CALLBACK			= 0x09,
	LIBFWNT_ACCESS_DENIED_CALLBACK			= 0x0a,
	LIBFWNT_ACCESS_ALLOWED_CALLBACK_OBJECT		= 0x0b,
	LIBFWNT_ACCESS_DENIED_CALLBACK_OBJECT		= 0x0c,
	LIBFWNT_SYSTEM_AUDIT_CALLBACK			= 0x0d,
	LIBFWNT_SYSTEM_ALARM_CALLBACK			= 0x0e,
	LIBFWNT_SYSTEM_AUDIT_CALLBACK_OBJECT		= 0x0f,
	LIBFWNT_SYSTEM_ALARM_CALLBACK_OBJECT		= 0x10,
	LIBFWNT_SYSTEM_MANDATORY_LABEL			= 0x11
};

#endif /* !defined( HAVE_LOCAL_LIBFWNT ) */

/* The security descriptor control flags
 */
enum LIBFWNT_SECURITY_DESCRIPTOR_CONTROL_FLAGS
{
	LIBFWNT_CONTROL_FLAG_OWNER_DEFAULTED		= 0x0001,
	LIBFWNT_CONTROL_FLAG_GROUP_DEFAULTED		= 0x0002,
	LIBFWNT_CONTROL_FLAG_DACL_PRESENT		= 0x0004,
	LIBFWNT_CONTROL_FLAG_DACL_DEFAULTED		= 0x0008,
	LIBFWNT_CONTROL_FLAG_SACL_PRESENT		= 0x0010,

	LIBFWNT_CONTROL_FLAG_DACL_AUTO_INHERIT_REQ	= 0x0100,
	LIBFWNT_CONTROL_FLAG_SACL_AUTO_INHERIT_REQ	= 0x0200,
	LIBFWNT_CONTROL_FLAG_DACL_AUTO_INHERITED	= 0x0400,
	LIBFWNT_CONTROL_FLAG_SACL_AUTO_INHERITED	= 0x0800,
	LIBFWNT_CONTROL_FLAG_DACL_PROTECTED		= 0x1000,
	LIBFWNT_CONTROL_FLAG_SACL_PROTECTED		= 0x2000,
	LIBFWNT_CONTROL_FLAG_RM_CONTROL_VALID		= 0x4000,
	LIBFWNT_CONTROL_FLAG_SELF_RELATIVE		= 0x8000
};

#endif /* !defined( LIBFWNT_INTERNAL_DEFINITIONS_H ) */

