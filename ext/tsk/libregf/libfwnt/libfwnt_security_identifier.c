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

#include <common.h>
#include <byte_stream.h>
#include <memory.h>
#include <types.h>

#include "libfwnt_definitions.h"
#include "libfwnt_libcerror.h"
#include "libfwnt_security_identifier.h"
#include "libfwnt_types.h"

/* Creates a security identifier
 * Make sure the value security_identifier is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libfwnt_security_identifier_initialize(
     libfwnt_security_identifier_t **security_identifier,
     libcerror_error_t **error )
{
	libfwnt_internal_security_identifier_t *internal_security_identifier = NULL;
	static char *function                                                = "libfwnt_security_identifier_initialize";

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
	internal_security_identifier = memory_allocate_structure(
	                                libfwnt_internal_security_identifier_t );

	if( internal_security_identifier == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create security identifier.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_security_identifier,
	     0,
	     sizeof( libfwnt_internal_security_identifier_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear security identifier.",
		 function );

		goto on_error;
	}
	*security_identifier = (libfwnt_security_identifier_t *) internal_security_identifier;

	return( 1 );

on_error:
	if( internal_security_identifier != NULL )
	{
		memory_free(
		 internal_security_identifier );
	}
	return( -1 );
}

/* Frees a security identifier
 * Returns 1 if successful or -1 on error
 */
int libfwnt_security_identifier_free(
     libfwnt_security_identifier_t **security_identifier,
     libcerror_error_t **error )
{
	libfwnt_internal_security_identifier_t *internal_security_identifier = NULL;
	static char *function                                                = "libfwnt_security_identifier_free";
	int result                                                           = 1;

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
		internal_security_identifier = (libfwnt_internal_security_identifier_t *) *security_identifier;

		if( internal_security_identifier->is_managed == 0 )
		{
			if( libfwnt_internal_security_identifier_free(
			     &internal_security_identifier,
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
		*security_identifier = NULL;
	}
	return( result );
}

/* Frees a security identifier
 * Returns 1 if successful or -1 on error
 */
int libfwnt_internal_security_identifier_free(
     libfwnt_internal_security_identifier_t **internal_security_identifier,
     libcerror_error_t **error )
{
	static char *function = "libfwnt_internal_security_identifier_free";

	if( internal_security_identifier == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid security identifier.",
		 function );

		return( -1 );
	}
	if( *internal_security_identifier != NULL )
	{
		memory_free(
		 *internal_security_identifier );

		*internal_security_identifier = NULL;
	}
	return( 1 );
}

/* Converts a security identifier stored in a byte stream into a runtime version
 * Returns 1 if successful or -1 on error
 */
int libfwnt_security_identifier_copy_from_byte_stream(
     libfwnt_security_identifier_t *security_identifier,
     const uint8_t *byte_stream,
     size_t byte_stream_size,
     int byte_order,
     libcerror_error_t **error )
{
	libfwnt_internal_security_identifier_t *internal_security_identifier = NULL;
	static char *function                                                = "libfwnt_security_identifier_copy_from_byte_stream";
	size_t security_identifier_size                                      = 0;
	uint8_t sub_authority_index                                          = 0;

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
	internal_security_identifier = (libfwnt_internal_security_identifier_t *) security_identifier;

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
	if( byte_stream_size < 8 )
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
	internal_security_identifier->revision_number           = byte_stream[ 0 ];
	internal_security_identifier->number_of_sub_authorities = byte_stream[ 1 ];

	security_identifier_size = 8 + ( internal_security_identifier->number_of_sub_authorities * 4 );

	if( byte_stream_size < (size_t) security_identifier_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: byte stream too small.",
		 function );

		return( -1 );
	}
	if( internal_security_identifier->number_of_sub_authorities > 15 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported security identifier contains more than 15 sub authoritites.",
		 function );

		return( -1 );
	}
	byte_stream += 2;

	/* The authority is stored as a 48-bit value
	 */
	byte_stream_copy_to_uint48_big_endian(
	 byte_stream,
	 internal_security_identifier->authority );

	byte_stream += 6;

	for( sub_authority_index = 0;
	     sub_authority_index < internal_security_identifier->number_of_sub_authorities;
	     sub_authority_index++ )
	{
		byte_stream_copy_to_uint32_little_endian(
		 byte_stream,
		 internal_security_identifier->sub_authority[ sub_authority_index ] );

		byte_stream += 4;
	}
	return( 1 );
}

/* Deterimes the size of the string for the security identifier
 * The string size includes the end of string character
 * Returns 1 if successful or -1 on error
 */
int libfwnt_security_identifier_get_string_size(
     libfwnt_security_identifier_t *security_identifier,
     size_t *string_size,
     uint32_t string_format_flags,
     libcerror_error_t **error )
{
	libfwnt_internal_security_identifier_t *internal_security_identifier = NULL;
	static char *function                                                = "libfwnt_security_identifier_get_string_size";
	size_t value_string_index                                            = 0;
	uint64_t value_64bit                                                 = 0;
	uint8_t sub_authority_index                                          = 0;

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
	internal_security_identifier = (libfwnt_internal_security_identifier_t *) security_identifier;

	if( string_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid string size.",
		 function );

		return( -1 );
	}
	if( string_format_flags != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported string format flags.",
		 function );

		return( -1 );
	}
	value_string_index = 2;

	value_64bit = internal_security_identifier->revision_number;

	do
	{
		value_string_index++;

		value_64bit /= 10;
	}
	while( value_64bit > 0 );

	value_string_index++;

	value_64bit = internal_security_identifier->authority;

	do
	{
		value_string_index++;

		value_64bit /= 10;
	}
	while( value_64bit > 0 );

	for( sub_authority_index = 0;
	     sub_authority_index < internal_security_identifier->number_of_sub_authorities;
	     sub_authority_index++ )
	{
		value_string_index++;

		value_64bit = internal_security_identifier->sub_authority[ sub_authority_index ];

		do
		{
			value_string_index++;

			value_64bit /= 10;
		}
		while( value_64bit > 0 );
	}
	value_string_index++;

	*string_size = value_string_index;

	return( 1 );
}

/* Converts the security identifier into an UTF-8 string
 * The string size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libfwnt_security_identifier_copy_to_utf8_string(
     libfwnt_security_identifier_t *security_identifier,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     uint32_t string_format_flags,
     libcerror_error_t **error )
{
	static char *function    = "libfwnt_security_identifier_copy_to_utf8_string";
	size_t utf8_string_index = 0;

	if( libfwnt_security_identifier_copy_to_utf8_string_with_index(
	     security_identifier,
	     utf8_string,
	     utf8_string_size,
	     &utf8_string_index,
	     string_format_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
		 "%s: unable to copy security identifier to UTF-8 string.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Converts the security identifier into an UTF-8 string
 * The string size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libfwnt_security_identifier_copy_to_utf8_string_with_index(
     libfwnt_security_identifier_t *security_identifier,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     size_t *utf8_string_index,
     uint32_t string_format_flags,
     libcerror_error_t **error )
{
	libfwnt_internal_security_identifier_t *internal_security_identifier = NULL;
	static char *function                                                = "libfwnt_security_identifier_copy_to_utf8_string";
	size_t string_index                                                  = 0;
	size_t value_string_index                                            = 0;
	size_t value_string_length                                           = 0;
	uint64_t value_64bit                                                 = 0;
	uint8_t sub_authority_index                                          = 0;

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
	internal_security_identifier = (libfwnt_internal_security_identifier_t *) security_identifier;

	if( utf8_string == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-8 size.",
		 function );

		return( -1 );
	}
	if( utf8_string_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: UTF-8 string size exceeds maximum.",
		 function );

		return( -1 );
	}
	if( utf8_string_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-8 string index.",
		 function );

		return( -1 );
	}
	if( string_format_flags != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported string format flags.",
		 function );

		return( -1 );
	}
	string_index = *utf8_string_index;

	if( ( string_index + 2 ) >= utf8_string_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-8 string is too small.",
		 function );

		return( -1 );
	}
	utf8_string[ string_index++ ] = (uint8_t) 'S';
	utf8_string[ string_index++ ] = (uint8_t) '-';

	value_64bit         = internal_security_identifier->revision_number;
	value_string_length = 0;

	do
	{
		value_string_length++;

		value_64bit /= 10;
	}
	while( value_64bit > 0 );

	if( ( string_index + value_string_length + 1 ) >= utf8_string_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-8 string is too small.",
		 function );

		return( -1 );
	}
	value_64bit        = internal_security_identifier->revision_number;
	value_string_index = value_string_length;

	while( value_string_index > 0 )
	{
		value_string_index--;

		utf8_string[ string_index + value_string_index ] = (uint8_t) '0' + (uint8_t) ( value_64bit % 10 );

		value_64bit /= 10;
	}
	string_index += value_string_length;

	utf8_string[ string_index++ ] = (uint8_t) '-';

	value_64bit         = internal_security_identifier->authority;
	value_string_length = 0;

	do
	{
		value_string_length++;

		value_64bit /= 10;
	}
	while( value_64bit > 0 );

	if( ( string_index + value_string_length ) >= utf8_string_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-8 string is too small.",
		 function );

		return( -1 );
	}
	value_64bit        = internal_security_identifier->authority;
	value_string_index = value_string_length;

	while( value_string_index > 0 )
	{
		value_string_index--;

		utf8_string[ string_index + value_string_index ] = (uint8_t) '0' + (uint8_t) ( value_64bit % 10 );

		value_64bit /= 10;
	}
	string_index += value_string_length;

	for( sub_authority_index = 0;
	     sub_authority_index < internal_security_identifier->number_of_sub_authorities;
	     sub_authority_index++ )
	{
		value_64bit         = internal_security_identifier->sub_authority[ sub_authority_index ];
		value_string_length = 0;

		do
		{
			value_string_length++;

			value_64bit /= 10;
		}
		while( value_64bit > 0 );

		if( ( string_index + value_string_length + 1 ) >= utf8_string_size )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
			 "%s: UTF-8 string is too small.",
			 function );

			return( -1 );
		}
		utf8_string[ string_index++ ] = (uint8_t) '-';

		value_64bit        = internal_security_identifier->sub_authority[ sub_authority_index ];
		value_string_index = value_string_length;

		while( value_string_index > 0 )
		{
			value_string_index--;

			utf8_string[ string_index + value_string_index ] = (uint8_t) '0' + (uint8_t) ( value_64bit % 10 );

			value_64bit /= 10;
		}
		string_index += value_string_length;
	}
	if( string_index >= utf8_string_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-8 string is too small.",
		 function );

		return( -1 );
	}
	utf8_string[ string_index++ ] = 0;

	*utf8_string_index = string_index;

	return( 1 );
}

/* Converts the security identifier into an UTF-16 string
 * The string size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libfwnt_security_identifier_copy_to_utf16_string(
     libfwnt_security_identifier_t *security_identifier,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     uint32_t string_format_flags,
     libcerror_error_t **error )
{
	static char *function    = "libfwnt_security_identifier_copy_to_utf16_string";
	size_t utf16_string_index = 0;

	if( libfwnt_security_identifier_copy_to_utf16_string_with_index(
	     security_identifier,
	     utf16_string,
	     utf16_string_size,
	     &utf16_string_index,
	     string_format_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
		 "%s: unable to copy security identifier to UTF-16 string.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Converts the security identifier into an UTF-16 string
 * The string size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libfwnt_security_identifier_copy_to_utf16_string_with_index(
     libfwnt_security_identifier_t *security_identifier,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     size_t *utf16_string_index,
     uint32_t string_format_flags,
     libcerror_error_t **error )
{
	libfwnt_internal_security_identifier_t *internal_security_identifier = NULL;
	static char *function                                                = "libfwnt_security_identifier_copy_to_utf16_string";
	size_t string_index                                                  = 0;
	size_t value_string_index                                            = 0;
	size_t value_string_length                                           = 0;
	uint64_t value_64bit                                                 = 0;
	uint8_t sub_authority_index                                          = 0;

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
	internal_security_identifier = (libfwnt_internal_security_identifier_t *) security_identifier;

	if( utf16_string == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-16 string.",
		 function );

		return( -1 );
	}
	if( utf16_string_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: UTF-16 string size exceeds maximum.",
		 function );

		return( -1 );
	}
	if( utf16_string_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-16 string index.",
		 function );

		return( -1 );
	}
	if( string_format_flags != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported string format flags.",
		 function );

		return( -1 );
	}
	string_index = *utf16_string_index;

	if( ( string_index + 2 ) >= utf16_string_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-16 string is too small.",
		 function );

		return( -1 );
	}
	utf16_string[ string_index++ ] = (uint16_t) 'S';
	utf16_string[ string_index++ ] = (uint16_t) '-';

	value_64bit         = internal_security_identifier->revision_number;
	value_string_length = 0;

	do
	{
		value_string_length++;

		value_64bit /= 10;
	}
	while( value_64bit > 0 );

	if( ( string_index + value_string_length + 1 ) >= utf16_string_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-16 string is too small.",
		 function );

		return( -1 );
	}
	value_64bit        = internal_security_identifier->revision_number;
	value_string_index = value_string_length;

	while( value_string_index > 0 )
	{
		value_string_index--;

		utf16_string[ string_index + value_string_index ] = (uint16_t) '0' + (uint16_t) ( value_64bit % 10 );

		value_64bit /= 10;
	}
	string_index += value_string_length;

	utf16_string[ string_index++ ] = (uint16_t) '-';

	value_64bit         = internal_security_identifier->authority;
	value_string_length = 0;

	do
	{
		value_string_length++;

		value_64bit /= 10;
	}
	while( value_64bit > 0 );

	if( ( string_index + value_string_length ) >= utf16_string_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-16 string is too small.",
		 function );

		return( -1 );
	}
	value_64bit        = internal_security_identifier->authority;
	value_string_index = value_string_length;

	while( value_string_index > 0 )
	{
		value_string_index--;

		utf16_string[ string_index + value_string_index ] = (uint16_t) '0' + (uint16_t) ( value_64bit % 10 );

		value_64bit /= 10;
	}
	string_index += value_string_length;

	for( sub_authority_index = 0;
	     sub_authority_index < internal_security_identifier->number_of_sub_authorities;
	     sub_authority_index++ )
	{
		value_64bit         = internal_security_identifier->sub_authority[ sub_authority_index ];
		value_string_length = 0;

		do
		{
			value_64bit /= 10;

			value_string_length++;
		}
		while( value_64bit > 0 );

		if( ( string_index + value_string_length + 1 ) >= utf16_string_size )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
			 "%s: UTF-16 string is too small.",
			 function );

			return( -1 );
		}
		utf16_string[ string_index++ ] = (uint16_t) '-';

		value_64bit        = internal_security_identifier->sub_authority[ sub_authority_index ];
		value_string_index = value_string_length;

		while( value_string_index > 0 )
		{
			value_string_index--;

			utf16_string[ string_index + value_string_index ] = (uint16_t) '0' + (uint16_t) ( value_64bit % 10 );

			value_64bit /= 10;
		}
		string_index += value_string_length;
	}
	if( string_index >= utf16_string_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-16 string is too small.",
		 function );

		return( -1 );
	}
	utf16_string[ string_index++ ] = 0;

	*utf16_string_index = string_index;

	return( 1 );
}

/* Converts the security identifier into an UTF-32 string
 * The string size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libfwnt_security_identifier_copy_to_utf32_string(
     libfwnt_security_identifier_t *security_identifier,
     uint32_t *utf32_string,
     size_t utf32_string_size,
     uint32_t string_format_flags,
     libcerror_error_t **error )
{
	static char *function    = "libfwnt_security_identifier_copy_to_utf32_string";
	size_t utf32_string_index = 0;

	if( libfwnt_security_identifier_copy_to_utf32_string_with_index(
	     security_identifier,
	     utf32_string,
	     utf32_string_size,
	     &utf32_string_index,
	     string_format_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
		 "%s: unable to copy security identifier to UTF-32 string.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Converts the security identifier into an UTF-32 string
 * The string size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libfwnt_security_identifier_copy_to_utf32_string_with_index(
     libfwnt_security_identifier_t *security_identifier,
     uint32_t *utf32_string,
     size_t utf32_string_size,
     size_t *utf32_string_index,
     uint32_t string_format_flags,
     libcerror_error_t **error )
{
	libfwnt_internal_security_identifier_t *internal_security_identifier = NULL;
	static char *function                                                = "libfwnt_security_identifier_copy_to_utf32_string_with_index";
	size_t string_index                                                  = 0;
	size_t value_string_index                                            = 0;
	size_t value_string_length                                           = 0;
	uint64_t value_64bit                                                 = 0;
	uint8_t sub_authority_index                                          = 0;

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
	internal_security_identifier = (libfwnt_internal_security_identifier_t *) security_identifier;

	if( utf32_string == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-32 string.",
		 function );

		return( -1 );
	}
	if( utf32_string_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: UTF-32 string size exceeds maximum.",
		 function );

		return( -1 );
	}
	if( utf32_string_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-32 string index.",
		 function );

		return( -1 );
	}
	if( string_format_flags != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported string format flags.",
		 function );

		return( -1 );
	}
	string_index = *utf32_string_index;

	if( ( string_index + 2 ) > utf32_string_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-32 string is too small.",
		 function );

		return( -1 );
	}
	utf32_string[ string_index++ ] = (uint32_t) 'S';
	utf32_string[ string_index++ ] = (uint32_t) '-';

	value_64bit         = internal_security_identifier->revision_number;
	value_string_length = 0;

	do
	{
		value_64bit /= 10;

		value_string_length++;
	}
	while( value_64bit > 0 );

	if( ( string_index + value_string_length + 1 ) >= utf32_string_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-32 string is too small.",
		 function );

		return( -1 );
	}
	value_64bit        = internal_security_identifier->revision_number;
	value_string_index = value_string_length;

	while( value_string_index > 0 )
	{
		value_string_index--;

		utf32_string[ string_index + value_string_index ] = (uint32_t) '0' + (uint32_t) ( value_64bit % 10 );

		value_64bit /= 10;
	}
	string_index += value_string_length;

	utf32_string[ string_index++ ] = (uint32_t) '-';

	value_64bit         = internal_security_identifier->authority;
	value_string_length = 0;

	do
	{
		value_string_length++;

		value_64bit /= 10;
	}
	while( value_64bit > 0 );

	if( ( string_index + value_string_length ) >= utf32_string_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-32 string is too small.",
		 function );

		return( -1 );
	}
	value_64bit        = internal_security_identifier->authority;
	value_string_index = value_string_length;

	while( value_string_index > 0 )
	{
		value_string_index--;

		utf32_string[ string_index + value_string_index ] = (uint32_t) '0' + (uint32_t) ( value_64bit % 10 );

		value_64bit /= 10;
	}
	string_index += value_string_length;

	for( sub_authority_index = 0;
	     sub_authority_index < internal_security_identifier->number_of_sub_authorities;
	     sub_authority_index++ )
	{
		value_64bit         = internal_security_identifier->sub_authority[ sub_authority_index ];
		value_string_length = 0;

		do
		{
			value_string_length++;

			value_64bit /= 10;
		}
		while( value_64bit > 0 );

		if( ( string_index + value_string_length + 1 ) >= utf32_string_size )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
			 "%s: UTF-32 string is too small.",
			 function );

			return( -1 );
		}
		utf32_string[ string_index++ ] = (uint32_t) '-';

		value_64bit        = internal_security_identifier->sub_authority[ sub_authority_index ];
		value_string_index = value_string_length;

		while( value_string_index > 0 )
		{
			value_string_index--;

			utf32_string[ string_index + value_string_index ] = (uint32_t) '0' + (uint32_t) ( value_64bit % 10 );

			value_64bit /= 10;
		}
		string_index += value_string_length;
	}
	if( string_index >= utf32_string_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-32 string is too small.",
		 function );

		return( -1 );
	}
	utf32_string[ string_index++ ] = 0;

	*utf32_string_index = string_index;

	return( 1 );
}

