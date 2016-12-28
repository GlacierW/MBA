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

#include <common.h>
#include <byte_stream.h>
#include <memory.h>
#include <narrow_string.h>
#include <system_string.h>
#include <types.h>
#include <wide_string.h>

#include "libfwnt_access_control_list.h"
#include "libfwnt_debug.h"
#include "libfwnt_definitions.h"
#include "libfwnt_libcerror.h"
#include "libfwnt_libcnotify.h"
#include "libfwnt_security_descriptor.h"
#include "libfwnt_security_identifier.h"
#include "libfwnt_types.h"

/* Creates a security descriptor
 * Make sure the value security_descriptor is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libfwnt_security_descriptor_initialize(
     libfwnt_security_descriptor_t **security_descriptor,
     libcerror_error_t **error )
{
	libfwnt_internal_security_descriptor_t *internal_security_descriptor = NULL;
	static char *function                                                = "libfwnt_security_descriptor_initialize";

	if( security_descriptor == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid security descriptor.",
		 function );

		return( -1 );
	}
	if( *security_descriptor != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid security descriptor value already set.",
		 function );

		return( -1 );
	}
	internal_security_descriptor = memory_allocate_structure(
	                                libfwnt_internal_security_descriptor_t );

	if( internal_security_descriptor == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create security descriptor.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_security_descriptor,
	     0,
	     sizeof( libfwnt_internal_security_descriptor_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear security descriptor.",
		 function );

		goto on_error;
	}
	*security_descriptor = (libfwnt_security_descriptor_t *) internal_security_descriptor;

	return( 1 );

on_error:
	if( internal_security_descriptor != NULL )
	{
		memory_free(
		 internal_security_descriptor );
	}
	return( -1 );
}

/* Frees a security descriptor
 * Returns 1 if successful or -1 on error
 */
int libfwnt_security_descriptor_free(
     libfwnt_security_descriptor_t **security_descriptor,
     libcerror_error_t **error )
{
	libfwnt_internal_security_descriptor_t *internal_security_descriptor = NULL;
	static char *function                                                = "libfwnt_security_descriptor_free";
	int result                                                           = 1;

	if( security_descriptor == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid security descriptor.",
		 function );

		return( -1 );
	}
	if( *security_descriptor != NULL )
	{
		internal_security_descriptor = (libfwnt_internal_security_descriptor_t *) *security_descriptor;
		*security_descriptor         = NULL;

		if( internal_security_descriptor->owner_sid != NULL )
		{
			if( libfwnt_internal_security_identifier_free(
			     (libfwnt_internal_security_identifier_t **) &( internal_security_descriptor->owner_sid ),
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: unable to free owner security identifier.",
				 function );

				result = -1;
			}
		}
		if( internal_security_descriptor->group_sid != NULL )
		{
			if( libfwnt_internal_security_identifier_free(
			     (libfwnt_internal_security_identifier_t **) &( internal_security_descriptor->group_sid ),
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: unable to free group security identifier.",
				 function );

				result = -1;
			}
		}
		if( internal_security_descriptor->discretionary_acl != NULL )
		{
			if( libfwnt_internal_access_control_list_free(
			     (libfwnt_internal_access_control_list_t ** ) &( internal_security_descriptor->discretionary_acl ),
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: unable to free discretionary ACL.",
				 function );

				result = -1;
			}
		}
		if( internal_security_descriptor->system_acl != NULL )
		{
			if( libfwnt_internal_access_control_list_free(
			     (libfwnt_internal_access_control_list_t ** ) &( internal_security_descriptor->system_acl ),
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: unable to free system ACL.",
				 function );

				result = -1;
			}
		}
		memory_free(
		 internal_security_descriptor );
	}
	return( result );
}

/* Converts a security descriptor stored in a byte stream into a runtime version
 * Returns 1 if successful or -1 on error
 */
int libfwnt_security_descriptor_copy_from_byte_stream(
     libfwnt_security_descriptor_t *security_descriptor,
     const uint8_t *byte_stream,
     size_t byte_stream_size,
     int byte_order,
     libcerror_error_t **error )
{
	libfwnt_internal_security_descriptor_t *internal_security_descriptor = NULL;
	static char *function                                                = "libfwnt_security_descriptor_copy_from_byte_stream";
	uint32_t discretionary_acl_offset                                    = 0;
	uint32_t group_sid_offset                                            = 0;
	uint32_t owner_sid_offset                                            = 0;
	uint32_t system_acl_offset                                           = 0;
	uint16_t control_flags                                               = 0;

#if defined( HAVE_DEBUG_OUTPUT )
	system_character_t *sid_string                                       = NULL;
	size_t sid_string_size                                               = 0;
	int result                                                           = 0;
#endif

	if( security_descriptor == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid security descriptor.",
		 function );

		return( -1 );
	}
	internal_security_descriptor = (libfwnt_internal_security_descriptor_t *) security_descriptor;

	if( internal_security_descriptor->owner_sid != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid security descriptor - owner SID value already set.",
		 function );

		return( -1 );
	}
	if( internal_security_descriptor->group_sid != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid security descriptor - group SID value already set.",
		 function );

		return( -1 );
	}
	if( internal_security_descriptor->discretionary_acl != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid security descriptor - discretionary ACL value already set.",
		 function );

		return( -1 );
	}
	if( internal_security_descriptor->system_acl != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid security descriptor - system ACL value already set.",
		 function );

		return( -1 );
	}
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
	if( byte_stream_size < 20 )
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
		 20,
		 0 );
	}
#endif
	internal_security_descriptor->revision_number = byte_stream[ 0 ];

	byte_stream_copy_to_uint16_little_endian(
	 &( byte_stream[ 2 ] ),
	 control_flags );

	byte_stream_copy_to_uint32_little_endian(
	 &( byte_stream[ 4 ] ),
	 owner_sid_offset );

	byte_stream_copy_to_uint32_little_endian(
	 &( byte_stream[ 8 ] ),
	 group_sid_offset );

	byte_stream_copy_to_uint32_little_endian(
	 &( byte_stream[ 12 ] ),
	 discretionary_acl_offset );

	byte_stream_copy_to_uint32_little_endian(
	 &( byte_stream[ 16 ] ),
	 system_acl_offset );

#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: revision number\t\t: %" PRIu8 "\n",
		 function,
		 internal_security_descriptor->revision_number );

		libcnotify_printf(
		 "%s: padding\t\t\t: 0x%02" PRIx8 "\n",
		 function,
		 byte_stream[ 1 ] );

		libcnotify_printf(
		 "%s: control flags\t\t: 0x%04" PRIx16 "\n",
		 function,
		 control_flags );
		libfwnt_debug_print_security_descriptor_control_flags(
		 control_flags );
		libcnotify_printf(
		 "\n" );

		libcnotify_printf(
		 "%s: owner SID offset\t\t: 0x%08" PRIx32 "\n",
		 function,
		 owner_sid_offset );

		libcnotify_printf(
		 "%s: group SID offset\t\t: 0x%08" PRIx32 "\n",
		 function,
		 group_sid_offset );

		libcnotify_printf(
		 "%s: discretionary ACL offset\t: 0x%08" PRIx32 "\n",
		 function,
		 discretionary_acl_offset );

		libcnotify_printf(
		 "%s: system ACL offset\t\t: 0x%08" PRIx32 "\n",
		 function,
		 system_acl_offset );

		libcnotify_printf(
		 "\n" );
	}
#endif
	if( owner_sid_offset != 0 )
	{
		if( ( owner_sid_offset < 20 )
		 || ( (size_t) owner_sid_offset > byte_stream_size ) )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: owner SID offset value out of bounds.",
			 function );

			goto on_error;
		}
		if( libfwnt_security_identifier_initialize(
		     &( internal_security_descriptor->owner_sid ),
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create owner security identifier.",
			 function );

			goto on_error;
		}
		if( internal_security_descriptor->owner_sid == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: invalid security descriptor - missing owner security identifier.",
			 function );

			goto on_error;
		}
		( (libfwnt_internal_security_identifier_t *) internal_security_descriptor->owner_sid )->is_managed = 1;

		if( libfwnt_security_identifier_copy_from_byte_stream(
		     internal_security_descriptor->owner_sid,
		     &( byte_stream[ owner_sid_offset ] ),
		     byte_stream_size - owner_sid_offset,
		     LIBFWNT_ENDIAN_LITTLE,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
			 "%s: unable to copy owner security identifier from byte stream.",
			 function );

			goto on_error;
		}
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			if( libfwnt_security_identifier_get_string_size(
			     internal_security_descriptor->owner_sid,
			     &sid_string_size,
			     0,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve owner security identifier string size.",
				 function );

				goto on_error;
			}
			libcnotify_printf(
			 "%s: owner SID\t\t\t: ",
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
					 "%s: unable to create owner security identifier string.",
					 function );

					goto on_error;
				}
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
				result = libfwnt_security_identifier_copy_to_utf16_string(
					  internal_security_descriptor->owner_sid,
					  (uint16_t *) sid_string,
					  sid_string_size,
					  0,
					  error );
#else
				result = libfwnt_security_identifier_copy_to_utf8_string(
					  internal_security_descriptor->owner_sid,
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
					 "%s: unable to copy owner security identifier to string.",
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
	if( group_sid_offset != 0 )
	{
		if( ( group_sid_offset < 20 )
		 || ( (size_t) group_sid_offset > byte_stream_size ) )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: group security identifier offset value out of bounds.",
			 function );

			goto on_error;
		}
		if( libfwnt_security_identifier_initialize(
		     &( internal_security_descriptor->group_sid ),
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create group security identifier.",
			 function );

			goto on_error;
		}
		if( internal_security_descriptor->group_sid == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: invalid security descriptor - missing group security identifier.",
			 function );

			goto on_error;
		}
		( (libfwnt_internal_security_identifier_t *) internal_security_descriptor->group_sid )->is_managed = 1;

		if( libfwnt_security_identifier_copy_from_byte_stream(
		     internal_security_descriptor->group_sid,
		     &( byte_stream[ group_sid_offset ] ),
		     byte_stream_size - group_sid_offset,
		     LIBFWNT_ENDIAN_LITTLE,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
			 "%s: unable to copy group security identifier from byte stream.",
			 function );

			goto on_error;
		}
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			if( libfwnt_security_identifier_get_string_size(
			     internal_security_descriptor->group_sid,
			     &sid_string_size,
			     0,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve group security identifier string size.",
				 function );

				goto on_error;
			}
			libcnotify_printf(
			 "%s: group SID\t\t\t: ",
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
					 "%s: unable to create group security identifier string.",
					 function );

					goto on_error;
				}
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
				result = libfwnt_security_identifier_copy_to_utf16_string(
					  internal_security_descriptor->group_sid,
					  (uint16_t *) sid_string,
					  sid_string_size,
					  0,
					  error );
#else
				result = libfwnt_security_identifier_copy_to_utf8_string(
					  internal_security_descriptor->group_sid,
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
					 "%s: unable to copy group security identifier to string.",
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
	if( discretionary_acl_offset != 0 )
	{
		if( ( discretionary_acl_offset < 20 )
		 || ( (size_t) discretionary_acl_offset > byte_stream_size ) )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: discretionary ACL offset value out of bounds.",
			 function );

			goto on_error;
		}
		if( libfwnt_access_control_list_initialize(
		     &( internal_security_descriptor->discretionary_acl ),
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create discretionary ACL.",
			 function );

			goto on_error;
		}
		if( libfwnt_access_control_list_copy_from_byte_stream(
		     internal_security_descriptor->discretionary_acl,
		     &( byte_stream[ discretionary_acl_offset ] ),
		     byte_stream_size - discretionary_acl_offset,
		     LIBFWNT_ENDIAN_LITTLE,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
			 "%s: unable to copy discretionary ACL from byte stream.",
			 function );

			goto on_error;
		}
	}
	if( system_acl_offset != 0 )
	{
		if( ( system_acl_offset < 20 )
		 || ( (size_t) system_acl_offset > byte_stream_size ) )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: system ACL offset value out of bounds.",
			 function );

			goto on_error;
		}
		if( libfwnt_access_control_list_initialize(
		     &( internal_security_descriptor->system_acl ),
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create system ACL.",
			 function );

			goto on_error;
		}
		if( libfwnt_access_control_list_copy_from_byte_stream(
		     internal_security_descriptor->system_acl,
		     &( byte_stream[ system_acl_offset ] ),
		     byte_stream_size - system_acl_offset,
		     LIBFWNT_ENDIAN_LITTLE,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
			 "%s: unable to copy system ACL from byte stream.",
			 function );

			goto on_error;
		}
	}
	return( 1 );

on_error:
#if defined( HAVE_DEBUG_OUTPUT )
	if( sid_string != NULL )
	{
		memory_free(
		 sid_string );
	}
#endif
	if( internal_security_descriptor->system_acl != NULL )
	{
		libfwnt_internal_access_control_list_free(
		 (libfwnt_internal_access_control_list_t ** ) &( internal_security_descriptor->system_acl ),
		 NULL );
	}
	if( internal_security_descriptor->discretionary_acl != NULL )
	{
		libfwnt_internal_access_control_list_free(
		 (libfwnt_internal_access_control_list_t ** ) &( internal_security_descriptor->discretionary_acl ),
		 NULL );
	}
	if( internal_security_descriptor->group_sid != NULL )
	{
		libfwnt_internal_security_identifier_free(
		 (libfwnt_internal_security_identifier_t **) &( internal_security_descriptor->group_sid ),
		 NULL );
	}
	if( internal_security_descriptor->owner_sid != NULL )
	{
		libfwnt_internal_security_identifier_free(
		 (libfwnt_internal_security_identifier_t **) &( internal_security_descriptor->owner_sid ),
		 NULL );
	}
	return( -1 );
}

/* Retrieves the owner security identifier (SID)
 * Returns 1 if successful, 0 if not available or -1 on error
 */
int libfwnt_security_descriptor_get_owner(
     libfwnt_security_descriptor_t *security_descriptor,
     libfwnt_security_identifier_t **security_identifier,
     libcerror_error_t **error )
{
	libfwnt_internal_security_descriptor_t *internal_security_descriptor = NULL;
	static char *function                                                = "libfwnt_security_descriptor_get_owner";

	if( security_descriptor == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid security descriptor.",
		 function );

		return( -1 );
	}
	internal_security_descriptor = (libfwnt_internal_security_descriptor_t *) security_descriptor;

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
	if( internal_security_descriptor->owner_sid == NULL )
	{
		return( 0 );
	}
	*security_identifier = internal_security_descriptor->owner_sid;

	return( 1 );
}

/* Retrieves the group security identifier (SID)
 * Returns 1 if successful, 0 if not available or -1 on error
 */
int libfwnt_security_descriptor_get_group(
     libfwnt_security_descriptor_t *security_descriptor,
     libfwnt_security_identifier_t **security_identifier,
     libcerror_error_t **error )
{
	libfwnt_internal_security_descriptor_t *internal_security_descriptor = NULL;
	static char *function                                                = "libfwnt_security_descriptor_get_group";

	if( security_descriptor == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid security descriptor.",
		 function );

		return( -1 );
	}
	internal_security_descriptor = (libfwnt_internal_security_descriptor_t *) security_descriptor;

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
	if( internal_security_descriptor->group_sid == NULL )
	{
		return( 0 );
	}
	*security_identifier = internal_security_descriptor->group_sid;

	return( 1 );
}

/* Retrieves the discretionary access control list (ACL)
 * Returns 1 if successful, 0 if not available or -1 on error
 */
int libfwnt_security_descriptor_get_discretionary_acl(
     libfwnt_security_descriptor_t *security_descriptor,
     libfwnt_access_control_list_t **access_control_list,
     libcerror_error_t **error )
{
	libfwnt_internal_security_descriptor_t *internal_security_descriptor = NULL;
	static char *function                                                = "libfwnt_security_descriptor_get_discretionary_acl";

	if( security_descriptor == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid security descriptor.",
		 function );

		return( -1 );
	}
	internal_security_descriptor = (libfwnt_internal_security_descriptor_t *) security_descriptor;

	if( access_control_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid access control list.",
		 function );

		return( -1 );
	}
	if( *access_control_list != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid access control list value already set.",
		 function );

		return( -1 );
	}
	if( internal_security_descriptor->discretionary_acl == NULL )
	{
		return( 0 );
	}
	*access_control_list = internal_security_descriptor->discretionary_acl;

	return( 1 );
}

/* Retrieves the system access control list (ACL)
 * Returns 1 if successful, 0 if not available or -1 on error
 */
int libfwnt_security_descriptor_get_system_acl(
     libfwnt_security_descriptor_t *security_descriptor,
     libfwnt_access_control_list_t **access_control_list,
     libcerror_error_t **error )
{
	libfwnt_internal_security_descriptor_t *internal_security_descriptor = NULL;
	static char *function                                                = "libfwnt_security_descriptor_get_system_acl";

	if( security_descriptor == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid security descriptor.",
		 function );

		return( -1 );
	}
	internal_security_descriptor = (libfwnt_internal_security_descriptor_t *) security_descriptor;

	if( access_control_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid access control list.",
		 function );

		return( -1 );
	}
	if( *access_control_list != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid access control list value already set.",
		 function );

		return( -1 );
	}
	if( internal_security_descriptor->system_acl == NULL )
	{
		return( 0 );
	}
	*access_control_list = internal_security_descriptor->system_acl;

	return( 1 );
}

