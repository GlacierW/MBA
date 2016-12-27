/*
 * Debug functions
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

#include <common.h>
#include <memory.h>
#include <types.h>

#include "libfwnt_debug.h"
#include "libfwnt_definitions.h"
#include "libfwnt_libcnotify.h"

#if defined( HAVE_DEBUG_OUTPUT )

/* Prints the security descriptor control flags
 */
void libfwnt_debug_print_security_descriptor_control_flags(
      uint16_t control_flags )
{
	if( ( control_flags & 0x0001 ) != 0 )
	{
		libcnotify_printf(
		 "\t(SE_OWNER_DEFAULTED)\n" );
	}
	if( ( control_flags & 0x0002 ) != 0 )
	{
		libcnotify_printf(
		 "\t(SE_GROUP_DEFAULTED)\n" );
	}
	if( ( control_flags & 0x0004 ) != 0 )
	{
		libcnotify_printf(
		 "\t(SE_DACL_PRESENT)\n" );
	}
	if( ( control_flags & 0x0008 ) != 0 )
	{
		libcnotify_printf(
		 "\t(SE_DACL_DEFAULTED)\n" );
	}
	if( ( control_flags & 0x0010 ) != 0 )
	{
		libcnotify_printf(
		 "\t(SE_SACL_PRESENT)\n" );
	}
	if( ( control_flags & 0x0020 ) != 0 )
	{
		libcnotify_printf(
		 "\t(Unknown 0x0020)\n" );
	}
	if( ( control_flags & 0x0040 ) != 0 )
	{
		libcnotify_printf(
		 "\t(Unknown 0x0040)\n" );
	}
	if( ( control_flags & 0x0080 ) != 0 )
	{
		libcnotify_printf(
		 "\t(Unknown 0x0080)\n" );
	}
	if( ( control_flags & 0x0100 ) != 0 )
	{
		libcnotify_printf(
		 "\t(SE_DACL_AUTO_INHERIT_REQ)\n" );
	}
	if( ( control_flags & 0x0200 ) != 0 )
	{
		libcnotify_printf(
		 "\t(SE_SACL_AUTO_INHERIT_REQ)\n" );
	}
	if( ( control_flags & 0x0400 ) != 0 )
	{
		libcnotify_printf(
		 "\t(SE_DACL_AUTO_INHERITED)\n" );
	}
	if( ( control_flags & 0x0800 ) != 0 )
	{
		libcnotify_printf(
		 "\t(SE_SACL_AUTO_INHERITED)\n" );
	}
	if( ( control_flags & 0x1000 ) != 0 )
	{
		libcnotify_printf(
		 "\t(SE_DACL_PROTECTED)\n" );
	}
	if( ( control_flags & 0x2000 ) != 0 )
	{
		libcnotify_printf(
		 "\t(SE_SACL_PROTECTED)\n" );
	}
	if( ( control_flags & 0x4000 ) != 0 )
	{
		libcnotify_printf(
		 "\t(SE_RM_CONTROL_VALID)\n" );
	}
	if( ( control_flags & 0x8000 ) != 0 )
	{
		libcnotify_printf(
		 "\t(SE_SELF_RELATIVE)\n" );
	}
}

/* Retrieves a string containing the access control entry type
 */
const char *libfwnt_debug_print_access_control_entry_type(
             uint8_t entry_type )
{
	switch( entry_type )
	{
		case LIBFWNT_ACCESS_ALLOWED:
			return( "Access allowed (ACCESS_ALLOWED_ACE_TYPE)" );

		case LIBFWNT_ACCESS_DENIED:
			return( "Access denied (ACCESS_DENIED_ACE_TYPE)" );

		case LIBFWNT_SYSTEM_AUDIT:
			return( "System-audit (SYSTEM_AUDIT_ACE_TYPE)" );

		case LIBFWNT_SYSTEM_ALARM:
			return( "System-alarm (SYSTEM_ALARM_ACE_TYPE)" );

		case LIBFWNT_ACCESS_ALLOWED_COMPOUND:
			return( "Access allowed (ACCESS_ALLOWED_COMPOUND_ACE_TYPE)" );

		case LIBFWNT_ACCESS_ALLOWED_OBJECT:
			return( "Access allowed (ACCESS_ALLOWED_OBJECT_ACE_TYPE)" );

		case LIBFWNT_ACCESS_DENIED_OBJECT:
			return( "Access denied (ACCESS_DENIED_OBJECT_ACE_TYPE)" );

		case LIBFWNT_SYSTEM_AUDIT_OBJECT:
			return( "System-audit (SYSTEM_AUDIT_OBJECT_ACE_TYPE)" );

		case LIBFWNT_SYSTEM_ALARM_OBJECT:
			return( "System-alarm (SYSTEM_ALARM_OBJECT_ACE_TYPE)" );

		case LIBFWNT_ACCESS_ALLOWED_CALLBACK:
			return( "Access allowed (ACCESS_ALLOWED_CALLBACK_ACE_TYPE)" );

		case LIBFWNT_ACCESS_DENIED_CALLBACK:
			return( "Access denied (ACCESS_DENIED_CALLBACK_ACE_TYPE)" );

		case LIBFWNT_ACCESS_ALLOWED_CALLBACK_OBJECT:
			return( "Access allowed (ACCESS_ALLOWED_CALLBACK_OBJECT_ACE_TYPE)" );

		case LIBFWNT_ACCESS_DENIED_CALLBACK_OBJECT:
			return( "Access denied (ACCESS_DENIED_CALLBACK_OBJECT_ACE_TYPE)" );

		case LIBFWNT_SYSTEM_AUDIT_CALLBACK:
			return( "System-audit (SYSTEM_AUDIT_CALLBACK_ACE_TYPE)" );

		case LIBFWNT_SYSTEM_ALARM_CALLBACK:
			return( "System-alarm (SYSTEM_ALARM_CALLBACK_ACE_TYPE)" );

		case LIBFWNT_SYSTEM_AUDIT_CALLBACK_OBJECT:
			return( "System-audit (SYSTEM_AUDIT_CALLBACK_OBJECT_ACE_TYPE)" );

		case LIBFWNT_SYSTEM_ALARM_CALLBACK_OBJECT:
			return( "System-alarm (SYSTEM_ALARM_CALLBACK_OBJECT_ACE_TYPE)" );

		case LIBFWNT_SYSTEM_MANDATORY_LABEL:
			return( "Mandatory label (SYSTEM_MANDATORY_LABEL_ACE_TYPE)" );
	}
	return( "UNKNOWN" );
}

/* Prints the access control entry flags
 */
void libfwnt_debug_print_access_control_entry_flags(
      uint8_t flags )
{
	if( ( flags & 0x01 ) != 0 )
	{
		libcnotify_printf(
		 "\t(OBJECT_INHERIT_ACE)\n" );
	}
	if( ( flags & 0x02 ) != 0 )
	{
		libcnotify_printf(
		 "\t(CONTAINER_INHERIT_ACE)\n" );
	}
	if( ( flags & 0x04 ) != 0 )
	{
		libcnotify_printf(
		 "\t(NO_PROPAGATE_INHERIT_ACE)\n" );
	}
	if( ( flags & 0x08 ) != 0 )
	{
		libcnotify_printf(
		 "\t(INHERIT_ONLY_ACE)\n" );
	}

	if( ( flags & 0x40 ) != 0 )
	{
		libcnotify_printf(
		 "\t(SUCCESSFUL_ACCESS_ACE_FLAG)\n" );
	}
	if( ( flags & 0x80 ) != 0 )
	{
		libcnotify_printf(
		 "\t(FAILED_ACCESS_ACE_FLAG)\n" );
	}
}

/* Prints the access control entry access mask
 */
void libfwnt_debug_print_access_control_entry_access_mask(
      uint32_t access_mask )
{
	if( ( access_mask & 0x00000001UL ) != 0 )
	{
		libcnotify_printf(
		 "\t(fsdrightReadBody or fsdrightListContents)\n" );
	}
	if( ( access_mask & 0x00000002UL ) != 0 )
	{
		libcnotify_printf(
		 "\t(fsdrightWriteBody or fsdrightCreateItem)\n" );
	}
	if( ( access_mask & 0x00000004UL ) != 0 )
	{
		libcnotify_printf(
		 "\t(fsdrightAppendMsg or fsdrightCreateContainer)\n" );
	}
	if( ( access_mask & 0x00000008UL ) != 0 )
	{
		libcnotify_printf(
		 "\t(fsdrightReadProperty or fsdrightReadProperty)\n" );
	}
	if( ( access_mask & 0x00000010UL ) != 0 )
	{
		libcnotify_printf(
		 "\t(fsdrightWriteProperty or fsdrightWriteProperty)\n" );
	}
	if( ( access_mask & 0x00000020UL ) != 0 )
	{
		libcnotify_printf(
		 "\t(fsdrightExecute)\n" );
	}

	if( ( access_mask & 0x00000080UL ) != 0 )
	{
		libcnotify_printf(
		 "\t(fsdrightReadAttributes or fsdrightReadAttributes)\n" );
	}
	if( ( access_mask & 0x00000100UL ) != 0 )
	{
		libcnotify_printf(
		 "\t(fsdrightWriteAttributes or fsdrightWriteAttributes)\n" );
	}
	if( ( access_mask & 0x00000200UL ) != 0 )
	{
		libcnotify_printf(
		 "\t(fsdrightWriteOwnProperty or fsdrightWriteOwnProperty)\n" );
	}
	if( ( access_mask & 0x00000400UL ) != 0 )
	{
		libcnotify_printf(
		 "\t(fsdrightDeleteOwnItem or fsdrightDeleteOwnItem)\n" );
	}
	if( ( access_mask & 0x00000800UL ) != 0 )
	{
		libcnotify_printf(
		 "\t(fsdrightViewItem or fsdrightViewItem)\n" );
	}

	if( ( access_mask & 0x00004000UL ) != 0 )
	{
		libcnotify_printf(
		 "\t(fsdrightOwner)\n" );
	}
	if( ( access_mask & 0x00008000UL ) != 0 )
	{
		libcnotify_printf(
		 "\t(fsdrightContact)\n" );
	}

	/* Standard access rights flags */
	if( ( access_mask & 0x00010000UL ) != 0 )
	{
		libcnotify_printf(
		 "\t(fsdrightDelete)\n" );
	}
	if( ( access_mask & 0x00020000UL ) != 0 )
	{
		libcnotify_printf(
		 "\t(fsdrightReadControl)\n" );
	}
	if( ( access_mask & 0x00040000UL ) != 0 )
	{
		libcnotify_printf(
		 "\t(fsdrightWriteSD)\n" );
	}
	if( ( access_mask & 0x00080000UL ) != 0 )
	{
		libcnotify_printf(
		 "\t(fsdrightWriteOwner)\n" );
	}
	if( ( access_mask & 0x00100000UL ) != 0 )
	{
		libcnotify_printf(
		 "\t(fsdrightSynchronize)\n" );
	}

	if( ( access_mask & 0x01000000UL ) != 0 )
	{
		libcnotify_printf(
		 "\t(ACCESS_SYSTEM_SECURITY)\n" );
	}
	if( ( access_mask & 0x02000000UL ) != 0 )
	{
		libcnotify_printf(
		 "\t(MAXIMUM_ALLOWED)\n" );
	}

	if( ( access_mask & 0x10000000UL ) != 0 )
	{
		libcnotify_printf(
		 "\t(GENERIC_ALL)\n" );
	}
	if( ( access_mask & 0x20000000UL ) != 0 )
	{
		libcnotify_printf(
		 "\t(GENERIC_EXECUTE)\n" );
	}
	if( ( access_mask & 0x40000000UL ) != 0 )
	{
		libcnotify_printf(
		 "\t(GENERIC_WRITE)\n" );
	}
	if( ( access_mask & 0x80000000UL ) != 0 )
	{
		libcnotify_printf(
		 "\t(GENERIC_READ)\n" );
	}
}

#endif /* defined( HAVE_DEBUG_OUTPUT ) */

