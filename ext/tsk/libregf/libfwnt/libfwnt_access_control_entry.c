/*
 * Windows NT Access Control Entry (ACE) functions
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
#include <byte_stream.h>
#include <memory.h>
#include <narrow_string.h>
#include <system_string.h>
#include <types.h>
#include <wide_string.h>

#include "libfwnt_access_control_entry.h"
#include "libfwnt_debug.h"
#include "libfwnt_definitions.h"
#include "libfwnt_libcerror.h"
#include "libfwnt_libcnotify.h"
#include "libfwnt_security_identifier.h"
#include "libfwnt_types.h"

/* Creates an access control entry
 * Make sure the value access_control_entry is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libfwnt_access_control_entry_initialize(
     libfwnt_access_control_entry_t **access_control_entry,
     libcerror_error_t **error )
{
	libfwnt_internal_access_control_entry_t *internal_access_control_entry = NULL;
	static char *function                                                  = "libfwnt_access_control_entry_initialize";

	if( access_control_entry == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid access control entry.",
		 function );

		return( -1 );
	}
	if( *access_control_entry != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid access control entry value already set.",
		 function );

		return( -1 );
	}
	internal_access_control_entry = memory_allocate_structure(
	                                 libfwnt_internal_access_control_entry_t );

	if( internal_access_control_entry == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create access control entry.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_access_control_entry,
	     0,
	     sizeof( libfwnt_internal_access_control_entry_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear access control entry.",
		 function );

		goto on_error;
	}
	*access_control_entry = (libfwnt_access_control_entry_t *) internal_access_control_entry;

	return( 1 );

on_error:
	if( internal_access_control_entry != NULL )
	{
		memory_free(
		 internal_access_control_entry );
	}
	return( -1 );
}

/* Frees an access control entry
 * Returns 1 if successful or -1 on error
 */
int libfwnt_access_control_entry_free(
     libfwnt_access_control_entry_t **access_control_entry,
     libcerror_error_t **error )
{
	static char *function = "libfwnt_access_control_entry_free";

	if( access_control_entry == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid access control entry.",
		 function );

		return( -1 );
	}
	if( *access_control_entry != NULL )
	{
		*access_control_entry = NULL;
	}
	return( 1 );
}

/* Frees an access control entry
 * Returns 1 if successful or -1 on error
 */
int libfwnt_internal_access_control_entry_free(
     libfwnt_internal_access_control_entry_t **internal_access_control_entry,
     libcerror_error_t **error )
{
	static char *function = "libfwnt_internal_access_control_entry_free";
	int result            = 1;

	if( internal_access_control_entry == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid access control entry.",
		 function );

		return( -1 );
	}
	if( *internal_access_control_entry != NULL )
	{
		if( ( *internal_access_control_entry )->security_identifier != NULL )
		{
			if( libfwnt_internal_security_identifier_free(
			     (libfwnt_internal_security_identifier_t **) &( ( *internal_access_control_entry )->security_identifier ),
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: unable to free security identifier.",
				 function );

				result = -1;
			}
		}
		memory_free(
		 *internal_access_control_entry );

		*internal_access_control_entry = NULL;
	}
	return( result );
}

/* Converts an access control entry stored in a byte stream into a runtime version
 * Returns 1 if successful or -1 on error
 */
int libfwnt_access_control_entry_copy_from_byte_stream(
     libfwnt_access_control_entry_t *access_control_entry,
     const uint8_t *byte_stream,
     size_t byte_stream_size,
     int byte_order,
     libcerror_error_t **error )
{
	libfwnt_internal_access_control_entry_t *internal_access_control_entry = NULL;
	static char *function                                                  = "libfwnt_access_control_entry_copy_from_byte_stream";
	size_t access_mask_offset                                              = 0;
	size_t sid_offset                                                      = 0;

#if defined( HAVE_DEBUG_OUTPUT )
	system_character_t *sid_string                                         = NULL;
	size_t sid_string_size                                                 = 0;
	int result                                                             = 0;
#endif

	if( access_control_entry == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid access control entry.",
		 function );

		return( -1 );
	}
	internal_access_control_entry = (libfwnt_internal_access_control_entry_t *) access_control_entry;

	if( byte_stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid byte stream.",
		 function );

		return( -1 );
	}
	if( byte_stream_size < 4 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: byte stream too small.",
		 function );

		return( -1 );
	}
	if( byte_stream_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: byte stream size exceeds maximum.",
		 function );

		return( -1 );
	}
	if( byte_order != LIBFWNT_ENDIAN_LITTLE )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported byte order.",
		 function );

		return( -1 );
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: header data:\n",
		 function );
		libcnotify_print_data(
		 byte_stream,
		 4,
		 0 );
	}
#endif
	internal_access_control_entry->type  = byte_stream[ 0 ];
	internal_access_control_entry->flags = byte_stream[ 1 ];

	byte_stream_copy_to_uint16_little_endian(
	 &( byte_stream[ 2 ] ),
	 internal_access_control_entry->size );

#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: type\t\t: %" PRIu8 " (%s)\n",
		 function,
		 internal_access_control_entry->type,
		 libfwnt_debug_print_access_control_entry_type(
		  internal_access_control_entry->type ) );

		libcnotify_printf(
		 "%s: flags\t\t: 0x%02" PRIx8 "\n",
		 function,
		 internal_access_control_entry->flags );
		libfwnt_debug_print_access_control_entry_flags(
		 internal_access_control_entry->flags );
		libcnotify_printf(
		 "\n" );

		libcnotify_printf(
		 "%s: size\t\t: %" PRIu16 "\n",
		 function,
		 internal_access_control_entry->size );

		libcnotify_printf(
		 "\n" );
	}
#endif
	if( ( internal_access_control_entry->size < 4 )
	 || ( (size_t) internal_access_control_entry->size > byte_stream_size ) )

	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: access control entry size value out of bounds.",
		 function );

		goto on_error;
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: access control entry data:\n",
		 function );
		libcnotify_print_data(
		 &( byte_stream[ 4 ] ),
		 internal_access_control_entry->size - 4,
		 0 );
	}
#endif
	switch( internal_access_control_entry->type )
	{
		/* Basic types */
		case LIBFWNT_ACCESS_ALLOWED:
		case LIBFWNT_ACCESS_DENIED:
		case LIBFWNT_SYSTEM_AUDIT:
		case LIBFWNT_SYSTEM_ALARM:
		case LIBFWNT_ACCESS_ALLOWED_CALLBACK:
		case LIBFWNT_ACCESS_DENIED_CALLBACK:
		case LIBFWNT_SYSTEM_AUDIT_CALLBACK:
		case LIBFWNT_SYSTEM_ALARM_CALLBACK:
		case LIBFWNT_SYSTEM_MANDATORY_LABEL:
			access_mask_offset = 4;
			sid_offset         = 8;
			break;

		/* Object types */
		case LIBFWNT_ACCESS_ALLOWED_OBJECT:
		case LIBFWNT_ACCESS_DENIED_OBJECT:
		case LIBFWNT_SYSTEM_AUDIT_OBJECT:
		case LIBFWNT_SYSTEM_ALARM_OBJECT:
		case LIBFWNT_ACCESS_ALLOWED_CALLBACK_OBJECT:
		case LIBFWNT_ACCESS_DENIED_CALLBACK_OBJECT:
		case LIBFWNT_SYSTEM_AUDIT_CALLBACK_OBJECT:
		case LIBFWNT_SYSTEM_ALARM_CALLBACK_OBJECT:
			access_mask_offset = 4;
			sid_offset         = 40;
			break;

		/* Unknown types */
		case LIBFWNT_ACCESS_ALLOWED_COMPOUND:
		default:
			break;
	}
	if( access_mask_offset > 0 )
	{
		byte_stream_copy_to_uint32_little_endian(
		 &( byte_stream[ access_mask_offset ] ),
		 internal_access_control_entry->access_mask );

#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: access mask\t\t: 0x%08" PRIx32 "\n",
			 function,
			 internal_access_control_entry->access_mask );
			libfwnt_debug_print_access_control_entry_access_mask(
			 internal_access_control_entry->access_mask );
			libcnotify_printf(
			 "\n" );
		}
#endif
	}
	if( sid_offset > 0 )
	{
		if( sid_offset > byte_stream_size )

		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: security identifier offset value out of bounds.",
			 function );

			goto on_error;
		}
		if( libfwnt_security_identifier_initialize(
		     &( internal_access_control_entry->security_identifier ),
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create security identifier.",
			 function );

			goto on_error;
		}
		if( internal_access_control_entry->security_identifier == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: invalid access control entry - missing owner security identifier.",
			 function );

			goto on_error;
		}
		( (libfwnt_internal_security_identifier_t *) internal_access_control_entry->security_identifier )->is_managed = 1;

		if( libfwnt_security_identifier_copy_from_byte_stream(
		     internal_access_control_entry->security_identifier,
		     &( byte_stream[ sid_offset ] ),
		     byte_stream_size - sid_offset,
		     byte_order,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
			 "%s: unable to copy security identifier from byte stream.",
			 function );

			goto on_error;
		}
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			if( libfwnt_security_identifier_get_string_size(
			     internal_access_control_entry->security_identifier,
			     &sid_string_size,
			     0,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve security identifier string size.",
				 function );

				goto on_error;
			}
			libcnotify_printf(
			 "%s: SID\t\t\t: ",
			 function );

			if( sid_string_size > 0 )
			{
				sid_string = system_string_allocate(
					      sid_string_size );

				if( sid_string == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_MEMORY,
					 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
					 "%s: unable to create security identifier string.",
					 function );

					goto on_error;
				}
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
				result = libfwnt_security_identifier_copy_to_utf16_string(
					  internal_access_control_entry->security_identifier,
					  (uint16_t *) sid_string,
					  sid_string_size,
					  0,
					  error );
#else
				result = libfwnt_security_identifier_copy_to_utf8_string(
					  internal_access_control_entry->security_identifier,
					  (uint8_t *) sid_string,
					  sid_string_size,
					  0,
					  error );
#endif
				if( result != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
					 "%s: unable to copy security identifier to string.",
					 function );

					goto on_error;
				}
				libcnotify_printf(
				 "%" PRIs_SYSTEM "",
				 sid_string );

				memory_free(
				 sid_string );

				sid_string = NULL;
			}
			libcnotify_printf(
			 "\n" );
		}
#endif
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "\n" );
	}
#endif
	return( 1 );

on_error:
#if defined( HAVE_DEBUG_OUTPUT )
	if( sid_string != NULL )
	{
		memory_free(
		 sid_string );
	}
#endif
	if( internal_access_control_entry->security_identifier != NULL )
	{
		libfwnt_internal_security_identifier_free(
		 (libfwnt_internal_security_identifier_t **) &( internal_access_control_entry->security_identifier ),
		 NULL );
	}
	return( -1 );
}

/* Retrieves the type
 * Returns 1 if successful or -1 on error
 */
int libfwnt_access_control_entry_get_type(
     libfwnt_access_control_entry_t *access_control_entry,
     uint8_t *type,
     libcerror_error_t **error )
{
	libfwnt_internal_access_control_entry_t *internal_access_control_entry = NULL;
	static char *function                                                  = "libfwnt_access_control_entry_get_type";

	if( access_control_entry == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid access control entry.",
		 function );

		return( -1 );
	}
	internal_access_control_entry = (libfwnt_internal_access_control_entry_t *) access_control_entry;

	if( type == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid type.",
		 function );

		return( -1 );
	}
	*type = internal_access_control_entry->type;

	return( 1 );
}

/* Retrieves the flags
 * Returns 1 if successful or -1 on error
 */
int libfwnt_access_control_entry_get_flags(
     libfwnt_access_control_entry_t *access_control_entry,
     uint8_t *flags,
     libcerror_error_t **error )
{
	libfwnt_internal_access_control_entry_t *internal_access_control_entry = NULL;
	static char *function                                                  = "libfwnt_access_control_entry_get_flags";

	if( access_control_entry == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid access control entry.",
		 function );

		return( -1 );
	}
	internal_access_control_entry = (libfwnt_internal_access_control_entry_t *) access_control_entry;

	if( flags == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid flags.",
		 function );

		return( -1 );
	}
	*flags = internal_access_control_entry->flags;

	return( 1 );
}

/* Retrieves the access mask
 * Returns 1 if successful, 0 if not available or -1 on error
 */
int libfwnt_access_control_entry_get_access_mask(
     libfwnt_access_control_entry_t *access_control_entry,
     uint32_t *access_mask,
     libcerror_error_t **error )
{
	libfwnt_internal_access_control_entry_t *internal_access_control_entry = NULL;
	static char *function                                                  = "libfwnt_access_control_entry_get_access_mask";

	if( access_control_entry == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid access control entry.",
		 function );

		return( -1 );
	}
	internal_access_control_entry = (libfwnt_internal_access_control_entry_t *) access_control_entry;

	if( access_mask == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid access mask.",
		 function );

		return( -1 );
	}
	switch( internal_access_control_entry->type )
	{
		/* Basic types */
		case LIBFWNT_ACCESS_ALLOWED:
		case LIBFWNT_ACCESS_DENIED:
		case LIBFWNT_SYSTEM_AUDIT:
		case LIBFWNT_SYSTEM_ALARM:
		case LIBFWNT_ACCESS_ALLOWED_CALLBACK:
		case LIBFWNT_ACCESS_DENIED_CALLBACK:
		case LIBFWNT_SYSTEM_AUDIT_CALLBACK:
		case LIBFWNT_SYSTEM_ALARM_CALLBACK:
		case LIBFWNT_SYSTEM_MANDATORY_LABEL:
		/* Object types */
		case LIBFWNT_ACCESS_ALLOWED_OBJECT:
		case LIBFWNT_ACCESS_DENIED_OBJECT:
		case LIBFWNT_SYSTEM_AUDIT_OBJECT:
		case LIBFWNT_SYSTEM_ALARM_OBJECT:
		case LIBFWNT_ACCESS_ALLOWED_CALLBACK_OBJECT:
		case LIBFWNT_ACCESS_DENIED_CALLBACK_OBJECT:
		case LIBFWNT_SYSTEM_AUDIT_CALLBACK_OBJECT:
		case LIBFWNT_SYSTEM_ALARM_CALLBACK_OBJECT:
			break;

		/* Unknown types */
		case LIBFWNT_ACCESS_ALLOWED_COMPOUND:
		default:
			return( 0 );
	}
	*access_mask = internal_access_control_entry->access_mask;

	return( 1 );
}

/* Retrieves the security identifier
 * Returns 1 if successful, 0 if not available or -1 on error
 */
int libfwnt_access_control_entry_get_security_identifier(
     libfwnt_access_control_entry_t *access_control_entry,
     libfwnt_security_identifier_t **security_identifier,
     libcerror_error_t **error )
{
	libfwnt_internal_access_control_entry_t *internal_access_control_entry = NULL;
	static char *function                                                  = "libfwnt_access_control_entry_get_security_identifier";

	if( access_control_entry == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid access control entry.",
		 function );

		return( -1 );
	}
	internal_access_control_entry = (libfwnt_internal_access_control_entry_t *) access_control_entry;

	if( security_identifier == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid security identifier.",
		 function );

		return( -1 );
	}
	if( *security_identifier != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid security identifier value already set.",
		 function );

		return( -1 );
	}
	if( internal_access_control_entry->security_identifier == NULL )
	{
		return( 0 );
	}
	*security_identifier = internal_access_control_entry->security_identifier;

	return( 1 );
}

