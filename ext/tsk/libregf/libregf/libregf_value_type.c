/*
 * Values type functions
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

#include "libregf_definitions.h"
#include "libregf_libcerror.h"
#include "libregf_libcnotify.h"
#include "libregf_libuna.h"
#include "libregf_value_type.h"

/* Converts the value data into a 32-bit value
 * Returns 1 if successful or -1 on error
 */
int libregf_value_type_copy_to_32bit(
     uint8_t *value_data,
     size_t value_data_size,
     uint8_t byte_order,
     uint32_t *value_32bit,
     libcerror_error_t **error )
{
	static char *function = "libregf_value_type_copy_to_32bit";

	if( value_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value data.",
		 function );

		return( -1 );
	}
	if( value_32bit == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value 32-bit.",
		 function );

		return( -1 );
	}
	/* The value data size of a 32-bit value is 4
	 */
	if( value_data_size != 4 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid value data size value out of bounds.",
		 function );

		return( -1 );
	}
	if( ( byte_order != LIBREGF_ENDIAN_BIG )
	 && ( byte_order != LIBREGF_ENDIAN_LITTLE ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported byte order: 0x%02" PRIx8 ".",
		 function,
		 byte_order );

		return( -1 );
	}
	if( byte_order == LIBREGF_ENDIAN_BIG )
	{
		byte_stream_copy_to_uint32_big_endian(
		 value_data,
		 *value_32bit );
	}
	else
	{
		byte_stream_copy_to_uint32_little_endian(
		 value_data,
		 *value_32bit );
	}
	return( 1 );
}

/* Converts the value data into a 64-bit value
 * Returns 1 if successful or -1 on error
 */
int libregf_value_type_copy_to_64bit(
     uint8_t *value_data,
     size_t value_data_size,
     uint8_t byte_order,
     uint64_t *value_64bit,
     libcerror_error_t **error )
{
	static char *function = "libregf_value_type_copy_to_64bit";

	if( value_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value data.",
		 function );

		return( -1 );
	}
	if( value_64bit == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value 64-bit.",
		 function );

		return( -1 );
	}
	/* The value data size of a 64-bit value is 8
	 */
	if( value_data_size != 8 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid value data size value out of bounds.",
		 function );

		return( -1 );
	}
	if( ( byte_order != LIBREGF_ENDIAN_BIG )
	 && ( byte_order != LIBREGF_ENDIAN_LITTLE ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported byte order: 0x%02" PRIx8 ".",
		 function,
		 byte_order );

		return( -1 );
	}
	if( byte_order == LIBREGF_ENDIAN_BIG )
	{
		byte_stream_copy_to_uint64_big_endian(
		 value_data,
		 *value_64bit );
	}
	else
	{
		byte_stream_copy_to_uint64_little_endian(
		 value_data,
		 *value_64bit );
	}
	return( 1 );
}

/* Determines the UTF-8 string size from the value data
 * Returns 1 if successful or -1 on error
 */
int libregf_value_type_get_utf8_string_size(
     uint8_t *value_data,
     size_t value_data_size,
     size_t *utf8_string_size,
     libcerror_error_t **error )
{
	static char *function = "libregf_value_type_get_utf8_string_size";

	if( utf8_string_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-8 string size.",
		 function );

		return( -1 );
	}
	/* Internally an empty string is represented by a NULL reference
	 */
	if( value_data == NULL )
	{
		*utf8_string_size = 0;
	}
	else
	{
		if( libuna_utf8_string_size_from_utf16_stream(
		     value_data,
		     value_data_size,
		     LIBUNA_ENDIAN_LITTLE,
		     utf8_string_size,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to determine UTF-8 string size.",
			 function );

			return( -1 );
		}
	}
	return( 1 );
}

/* Converts the value data into a UTF-8 string
 * Returns 1 if successful or -1 on error
 */
int libregf_value_type_copy_to_utf8_string(
     uint8_t *value_data,
     size_t value_data_size,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     libcerror_error_t **error )
{
	static char *function = "libregf_value_type_copy_to_utf8_string";

	if( utf8_string == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-8 string.",
		 function );

		return( -1 );
	}
	if( utf8_string_size == 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_ZERO_OR_LESS,
		 "%s: invalid UTF-8 string size.",
		 function );

		return( -1 );
	}
	if( utf8_string_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid UTF-8 string size value exceeds maximum.",
		 function );

		return( -1 );
	}
	/* Internally an empty string is represented by a NULL reference
	 */
	if( value_data == NULL )
	{
		utf8_string[ 0 ] = 0;
	}
	else
	{
		if( libuna_utf8_string_copy_from_utf16_stream(
		     utf8_string,
		     utf8_string_size,
		     value_data,
		     value_data_size,
		     LIBUNA_ENDIAN_LITTLE,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_CONVERSION,
			 LIBCERROR_CONVERSION_ERROR_GENERIC,
			 "%s: unable to set UTF-8 string.",
			 function );

			return( -1 );
		}
	}
	return( 1 );
}

/* Determines the UTF-16 string size from the value data
 * Returns 1 if successful or -1 on error
 */
int libregf_value_type_get_utf16_string_size(
     uint8_t *value_data,
     size_t value_data_size,
     size_t *utf16_string_size,
     libcerror_error_t **error )
{
	static char *function = "libregf_value_type_get_utf16_string_size";

	if( utf16_string_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-16 string size.",
		 function );

		return( -1 );
	}
	/* Internally an empty string is represented by a NULL reference
	 */
	if( value_data == NULL )
	{
		*utf16_string_size = 0;
	}
	else
	{
		if( libuna_utf16_string_size_from_utf16_stream(
		     value_data,
		     value_data_size,
		     LIBUNA_ENDIAN_LITTLE,
		     utf16_string_size,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to determine UTF-16 string size.",
			 function );

			return( -1 );
		}
	}
	return( 1 );
}

/* Converts the value data into a UTF-16 string
 * Returns 1 if successful or -1 on error
 */
int libregf_value_type_copy_to_utf16_string(
     uint8_t *value_data,
     size_t value_data_size,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     libcerror_error_t **error )
{
	static char *function = "libregf_value_type_copy_to_utf16_string";

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
	if( utf16_string_size == 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_ZERO_OR_LESS,
		 "%s: invalid UTF-16 string size.",
		 function );

		return( -1 );
	}
	if( utf16_string_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid UTF-16 string size value exceeds maximum.",
		 function );

		return( -1 );
	}
	/* Internally an empty string is represented by a NULL reference
	 */
	if( value_data == NULL )
	{
		utf16_string[ 0 ] = 0;
	}
	else
	{
		if( libuna_utf16_string_copy_from_utf16_stream(
		     utf16_string,
		     utf16_string_size,
		     value_data,
		     value_data_size,
		     LIBUNA_ENDIAN_LITTLE,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_CONVERSION,
			 LIBCERROR_CONVERSION_ERROR_GENERIC,
			 "%s: unable to set UTF-16 string.",
			 function );

			return( -1 );
		}
	}
	return( 1 );
}

/* Determines the binary data size from the value data
 * Returns 1 if successful or -1 on error
 */
int libregf_value_type_get_binary_data_size(
     uint8_t *value_data,
     size_t value_data_size,
     size_t *binary_data_size,
     libcerror_error_t **error )
{
	static char *function = "libregf_value_type_get_binary_data_size";

	if( binary_data_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid binary data size.",
		 function );

		return( -1 );
	}
	/* Internally an empty binary data is represented by a NULL reference
	 */
	if( value_data == NULL )
	{
		*binary_data_size = 0;
	}
	else
	{
		*binary_data_size = value_data_size;
	}
	return( 1 );
}

/* Converts the value data into binary data
 * Returns 1 if successful or -1 on error
 */
int libregf_value_type_copy_to_binary_data(
     uint8_t *value_data,
     size_t value_data_size,
     uint8_t *binary_data,
     size_t binary_data_size,
     libcerror_error_t **error )
{
	static char *function = "libregf_value_type_copy_to_binary_data";

	if( binary_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid binary data.",
		 function );

		return( -1 );
	}
	if( binary_data_size == 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_ZERO_OR_LESS,
		 "%s: invalid binary data size.",
		 function );

		return( -1 );
	}
	if( binary_data_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid binary data size value exceeds maximum.",
		 function );

		return( -1 );
	}
	/* Internally empty binary data is represented by a NULL reference
	 */
	if( value_data != NULL )
	{
		if( binary_data_size < value_data_size )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
			 "%s: binary data is too small.",
			 function );

			return( -1 );
		}
		if( memory_copy(
		     binary_data,
		     value_data,
		     value_data_size ) == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_MEMORY,
			 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
			 "%s: unable to set binary data.",
			 function );

			return( -1 );
		}
	}
	return( 1 );
}

