/*
 * Base16 stream functions
 *
 * Copyright (C) 2008-2016, Joachim Metz <joachim.metz@gmail.com>
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
#include <types.h>

#include "libuna_base16_stream.h"
#include "libuna_definitions.h"
#include "libuna_libcerror.h"
#include "libuna_types.h"

/* Determines the size of a byte stream from a base16 stream
 *
 * LIBUNA_BASE16_FLAG_STRIP_WHITESPACE removes leading space and tab characters,
 * and trailing space, tab and end of line characters
 *
 * Returns 1 if successful or -1 on error
 */
int libuna_base16_stream_size_to_byte_stream(
     const uint8_t *base16_stream,
     size_t base16_stream_size,
     size_t *byte_stream_size,
     uint32_t base16_variant,
     uint8_t flags,
     libcerror_error_t **error )
{
	static char *function        = "libuna_base16_stream_size_to_byte_stream";
	size_t base16_character_size = 0;
	size_t base16_stream_index   = 0;
	size_t number_of_characters  = 0;
	size_t whitespace_size       = 0;
	uint32_t base16_character1   = 0;
	uint32_t base16_character2   = 0;
	uint8_t character_case       = 0;
	uint8_t character_limit      = 0;
	uint8_t strip_mode           = LIBUNA_STRIP_MODE_LEADING_WHITESPACE;

	if( base16_stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid base16 stream.",
		 function );

		return( -1 );
	}
	if( ( base16_stream_size == 0 )
	 || ( base16_stream_size > (size_t) SSIZE_MAX ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid base16 stream size value out of bounds.",
		 function );

		return( -1 );
	}
	if( byte_stream_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid byte stream size.",
		 function );

		return( -1 );
	}
	switch( base16_variant & 0x000000ffUL )
	{
		case LIBUNA_BASE16_VARIANT_CHARACTER_LIMIT_NONE:
			character_limit = 0;
			break;

		case LIBUNA_BASE16_VARIANT_CHARACTER_LIMIT_64:
			character_limit = 64;
			break;

		case LIBUNA_BASE16_VARIANT_CHARACTER_LIMIT_76:
			character_limit = 76;
			break;

		default:
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: unsupported base16 variant.",
			 function );

			return( -1 );
	}
	switch( base16_variant & 0x000f0000UL )
	{
		case LIBUNA_BASE16_VARIANT_CASE_LOWER:
			character_case = LIBUNA_CASE_LOWER;
			break;

		case LIBUNA_BASE16_VARIANT_CASE_MIXED:
			character_case = LIBUNA_CASE_MIXED;
			break;

		case LIBUNA_BASE16_VARIANT_CASE_UPPER:
			character_case = LIBUNA_CASE_UPPER;
			break;

		default:
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: unsupported base16 variant.",
			 function );

			return( -1 );
	}
	switch( base16_variant & 0xf0000000UL )
	{
		case LIBUNA_BASE16_VARIANT_ENCODING_BYTE_STREAM:
			base16_character_size = 1;
			break;

		case LIBUNA_BASE16_VARIANT_ENCODING_UTF16_BIG_ENDIAN:
		case LIBUNA_BASE16_VARIANT_ENCODING_UTF16_LITTLE_ENDIAN:
			base16_character_size = 2;
			break;

		case LIBUNA_BASE16_VARIANT_ENCODING_UTF32_BIG_ENDIAN:
		case LIBUNA_BASE16_VARIANT_ENCODING_UTF32_LITTLE_ENDIAN:
			base16_character_size = 4;
			break;

		default:
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: unsupported base16 variant.",
			 function );

			return( -1 );
	}
	if( base16_stream_size < base16_character_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: invalid base16 stream value too small.",
		 function );

		return( -1 );
	}
	if( ( flags & ~( LIBUNA_BASE16_FLAG_STRIP_WHITESPACE ) ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported flags.",
		 function );

		return( -1 );
	}
	base16_stream_index = base16_stream_size - base16_character_size;
	whitespace_size     = 0;

	while( base16_stream_index > base16_character_size )
	{
		switch( base16_variant & 0xf0000000UL )
		{
			case LIBUNA_BASE16_VARIANT_ENCODING_BYTE_STREAM:
				base16_character1 = base16_stream[ base16_stream_index ];
				break;

			case LIBUNA_BASE16_VARIANT_ENCODING_UTF16_BIG_ENDIAN:
				byte_stream_copy_to_uint16_big_endian(
				 &( base16_stream[ base16_stream_index ] ),
				 base16_character1 );
				break;

			case LIBUNA_BASE16_VARIANT_ENCODING_UTF16_LITTLE_ENDIAN:
				byte_stream_copy_to_uint16_little_endian(
				 &( base16_stream[ base16_stream_index ] ),
				 base16_character1 );
				break;

			case LIBUNA_BASE16_VARIANT_ENCODING_UTF32_BIG_ENDIAN:
				byte_stream_copy_to_uint32_big_endian(
				 &( base16_stream[ base16_stream_index ] ),
				 base16_character1 );
				break;

			case LIBUNA_BASE16_VARIANT_ENCODING_UTF32_LITTLE_ENDIAN:
				byte_stream_copy_to_uint32_little_endian(
				 &( base16_stream[ base16_stream_index ] ),
				 base16_character1 );
				break;
		}
		if( ( base16_character1 & 0xffffff00UL ) != 0 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: invalid base16 character at index: %" PRIzd ".",
			 function,
			 base16_stream_index );

			return( -1 );
		}
		base16_stream_index -= base16_character_size;

		if( ( base16_character1 == (uint32_t) '\n' )
		 || ( base16_character1 == (uint32_t) '\r' ) )
		{
			whitespace_size += base16_character_size;
		}
		else if( ( flags & LIBUNA_BASE16_FLAG_STRIP_WHITESPACE ) == 0 )
		{
			break;
		}
		else if( ( base16_character1 == (uint32_t) ' ' )
		      || ( base16_character1 == (uint32_t) '\t' )
		      || ( base16_character1 == (uint32_t) '\v' ) )
		{
			whitespace_size += base16_character_size;
		}
		else
		{
			break;
		}
	}
	base16_stream_size -= whitespace_size;

	if( base16_stream_size < base16_character_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: invalid base16 stream value too small.",
		 function );

		return( -1 );
	}
	base16_stream_index = 0;
	whitespace_size     = 0;

	while( base16_stream_index < base16_stream_size )
	{
		switch( base16_variant & 0xf0000000UL )
		{
			case LIBUNA_BASE16_VARIANT_ENCODING_BYTE_STREAM:
				base16_character1 = base16_stream[ base16_stream_index ];
				break;

			case LIBUNA_BASE16_VARIANT_ENCODING_UTF16_BIG_ENDIAN:
				byte_stream_copy_to_uint16_big_endian(
				 &( base16_stream[ base16_stream_index ] ),
				 base16_character1 );
				break;

			case LIBUNA_BASE16_VARIANT_ENCODING_UTF16_LITTLE_ENDIAN:
				byte_stream_copy_to_uint16_little_endian(
				 &( base16_stream[ base16_stream_index ] ),
				 base16_character1 );
				break;

			case LIBUNA_BASE16_VARIANT_ENCODING_UTF32_BIG_ENDIAN:
				byte_stream_copy_to_uint32_big_endian(
				 &( base16_stream[ base16_stream_index ] ),
				 base16_character1 );
				break;

			case LIBUNA_BASE16_VARIANT_ENCODING_UTF32_LITTLE_ENDIAN:
				byte_stream_copy_to_uint32_little_endian(
				 &( base16_stream[ base16_stream_index ] ),
				 base16_character1 );
				break;
		}
		if( ( base16_character1 & 0xffffff00UL ) != 0 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: invalid base16 character at index: %" PRIzd ".",
			 function,
			 base16_stream_index );

			return( -1 );
		}
		base16_stream_index += base16_character_size;

		if( ( base16_character1 == (uint32_t) '\n' )
		 || ( base16_character1 == (uint32_t) '\r' ) )
		{
			if( ( strip_mode != LIBUNA_STRIP_MODE_NON_WHITESPACE )
			 && ( strip_mode != LIBUNA_STRIP_MODE_TRAILING_WHITESPACE ) )
			{
				strip_mode = LIBUNA_STRIP_MODE_INVALID_CHARACTER;
			}
			else
			{
				if( ( base16_stream_index + base16_character_size ) < base16_stream_size )
				{
					switch( base16_variant & 0xf0000000UL )
					{
						case LIBUNA_BASE16_VARIANT_ENCODING_BYTE_STREAM:
							base16_character2 = base16_stream[ base16_stream_index ];
							break;

						case LIBUNA_BASE16_VARIANT_ENCODING_UTF16_BIG_ENDIAN:
							byte_stream_copy_to_uint16_big_endian(
							 &( base16_stream[ base16_stream_index ] ),
							 base16_character2 );
							break;

						case LIBUNA_BASE16_VARIANT_ENCODING_UTF16_LITTLE_ENDIAN:
							byte_stream_copy_to_uint16_little_endian(
							 &( base16_stream[ base16_stream_index ] ),
							 base16_character2 );
							break;

						case LIBUNA_BASE16_VARIANT_ENCODING_UTF32_BIG_ENDIAN:
							byte_stream_copy_to_uint32_big_endian(
							 &( base16_stream[ base16_stream_index ] ),
							 base16_character2 );
							break;

						case LIBUNA_BASE16_VARIANT_ENCODING_UTF32_LITTLE_ENDIAN:
							byte_stream_copy_to_uint32_little_endian(
							 &( base16_stream[ base16_stream_index ] ),
							 base16_character2 );
							break;
					}
					if( ( base16_character2 & 0xffffff00UL ) != 0 )
					{
						libcerror_error_set(
						 error,
						 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
						 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
						 "%s: invalid base16 character at index: %" PRIzd ".",
						 function,
						 base16_stream_index );

						return( -1 );
					}
					if( ( base16_character2 == (uint32_t) '\n' )
					 || ( base16_character2 == (uint32_t) '\r' ) )
					{
						base16_stream_index += base16_character_size;
						whitespace_size     += base16_character_size;
					}
				}
				strip_mode = LIBUNA_STRIP_MODE_LEADING_WHITESPACE;
			}
			if( character_limit != 0 )
			{
				if( number_of_characters != (size_t) character_limit )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_CONVERSION,
					 LIBCERROR_CONVERSION_ERROR_INPUT_FAILED,
					 "%s: number of characters in line does not match character limit.",
					 function );

					return( -1 );
				}
				number_of_characters = 0;
			}
			whitespace_size += base16_character_size;
		}
		else if( ( base16_character1 == (uint32_t) ' ' )
		      || ( base16_character1 == (uint32_t) '\t' )
		      || ( base16_character1 == (uint32_t) '\v' ) )
		{
			if( ( flags & LIBUNA_BASE16_FLAG_STRIP_WHITESPACE ) != 0 )
			{
				if( strip_mode == LIBUNA_STRIP_MODE_NON_WHITESPACE )
				{
					strip_mode = LIBUNA_STRIP_MODE_TRAILING_WHITESPACE;
				}
				if( ( strip_mode != LIBUNA_STRIP_MODE_LEADING_WHITESPACE )
				 && ( strip_mode != LIBUNA_STRIP_MODE_TRAILING_WHITESPACE ) )
				{
					strip_mode = LIBUNA_STRIP_MODE_INVALID_CHARACTER;
				}
				else
				{
					whitespace_size += base16_character_size;
				}
			}
			else
			{
				strip_mode = LIBUNA_STRIP_MODE_INVALID_CHARACTER;
			}
		}
		else if( strip_mode == LIBUNA_STRIP_MODE_LEADING_WHITESPACE )
		{
			strip_mode = LIBUNA_STRIP_MODE_NON_WHITESPACE;
		}
		else if( strip_mode == LIBUNA_STRIP_MODE_TRAILING_WHITESPACE )
		{
			strip_mode = LIBUNA_STRIP_MODE_INVALID_CHARACTER;
		}
		if( strip_mode == LIBUNA_STRIP_MODE_NON_WHITESPACE )
		{
			if( ( base16_character1 >= (uint32_t) 'A' )
			 && ( base16_character1 <= (uint32_t) 'F' ) )
			{
				if( ( character_case != LIBUNA_CASE_MIXED )
				 && ( character_case != LIBUNA_CASE_UPPER ) )
				{
					strip_mode = LIBUNA_STRIP_MODE_INVALID_CHARACTER;
				}
				number_of_characters++;
			}
			else if( ( base16_character1 >= (uint32_t) 'a' )
			      && ( base16_character1 <= (uint32_t) 'f' ) )
			{
				if( ( character_case != LIBUNA_CASE_MIXED )
				 && ( character_case != LIBUNA_CASE_LOWER ) )
				{
					strip_mode = LIBUNA_STRIP_MODE_INVALID_CHARACTER;
				}
				number_of_characters++;
			}
			else if( ( base16_character1 >= (uint32_t) '0' )
			      && ( base16_character1 <= (uint32_t) '9' ) )
			{
				number_of_characters++;
			}
			else
			{
				strip_mode = LIBUNA_STRIP_MODE_INVALID_CHARACTER;
			}
		}
		if( strip_mode == LIBUNA_STRIP_MODE_INVALID_CHARACTER )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_CONVERSION,
			 LIBCERROR_CONVERSION_ERROR_INPUT_FAILED,
			 "%s: invalid character in base16 stream at index: %" PRIzd ".",
			 function,
			 base16_stream_index - base16_character_size );

			return( -1 );
		}
	}
	if( character_limit != 0 )
	{
		if( number_of_characters > (size_t) character_limit )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_CONVERSION,
			 LIBCERROR_CONVERSION_ERROR_INPUT_FAILED,
			 "%s: number of characters in last line exceed maximum.",
			 function );

			return( -1 );
		}
	}
	base16_stream_size -= whitespace_size;

	/* Make sure the byte stream is able to hold
	 * at least 1 byte for each 2 base16 characters
	 */
	base16_character_size *= 2;

	if( ( base16_stream_size % base16_character_size ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_CONVERSION,
		 LIBCERROR_CONVERSION_ERROR_INPUT_FAILED,
		 "%s: invalid base16 stream size.",
		 function );

		return( -1 );
	}
	base16_stream_size /= base16_character_size;

	*byte_stream_size = base16_stream_size;

	return( 1 );
}

/* Copies a byte stream from a base16 stream
 *
 * LIBUNA_BASE16_FLAG_STRIP_WHITESPACE removes leading space and tab characters,
 * and trailing space, tab and end of line characters
 *
 * Returns 1 if successful or -1 on error
 */
int libuna_base16_stream_copy_to_byte_stream(
     const uint8_t *base16_stream,
     size_t base16_stream_size,
     uint8_t *byte_stream,
     size_t byte_stream_size,
     uint32_t base16_variant,
     uint8_t flags,
     libcerror_error_t **error )
{
	static char *function        = "libuna_base16_stream_copy_to_byte_stream";
	size_t base16_character_size = 0;
	size_t base16_stream_index   = 0;
	size_t byte_stream_index     = 0;
	size_t number_of_characters  = 0;
	size_t whitespace_size       = 0;
	uint32_t base16_character1   = 0;
	uint32_t base16_character2   = 0;
	uint8_t byte_value           = 0;
	uint8_t character_case       = 0;
	uint8_t character_limit      = 0;
	uint8_t strip_mode           = LIBUNA_STRIP_MODE_LEADING_WHITESPACE;

	if( base16_stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid base16 stream.",
		 function );

		return( -1 );
	}
	if( base16_stream_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid base16 stream size value exceeds maximum.",
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
	if( byte_stream_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid byte stream size value exceeds maximum.",
		 function );

		return( -1 );
	}
	switch( base16_variant & 0x000000ffUL )
	{
		case LIBUNA_BASE16_VARIANT_CHARACTER_LIMIT_NONE:
			character_limit = 0;
			break;

		case LIBUNA_BASE16_VARIANT_CHARACTER_LIMIT_64:
			character_limit = 64;
			break;

		case LIBUNA_BASE16_VARIANT_CHARACTER_LIMIT_76:
			character_limit = 76;
			break;

		default:
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: unsupported base16 variant.",
			 function );

			return( -1 );
	}
	switch( base16_variant & 0x000f0000UL )
	{
		case LIBUNA_BASE16_VARIANT_CASE_LOWER:
			character_case = LIBUNA_CASE_LOWER;
			break;

		case LIBUNA_BASE16_VARIANT_CASE_MIXED:
			character_case = LIBUNA_CASE_MIXED;
			break;

		case LIBUNA_BASE16_VARIANT_CASE_UPPER:
			character_case = LIBUNA_CASE_UPPER;
			break;

		default:
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: unsupported base16 variant.",
			 function );

			return( -1 );
	}
	switch( base16_variant & 0xf0000000UL )
	{
		case LIBUNA_BASE16_VARIANT_ENCODING_BYTE_STREAM:
			base16_character_size = 1;
			break;

		case LIBUNA_BASE16_VARIANT_ENCODING_UTF16_BIG_ENDIAN:
		case LIBUNA_BASE16_VARIANT_ENCODING_UTF16_LITTLE_ENDIAN:
			base16_character_size = 2;
			break;

		case LIBUNA_BASE16_VARIANT_ENCODING_UTF32_BIG_ENDIAN:
		case LIBUNA_BASE16_VARIANT_ENCODING_UTF32_LITTLE_ENDIAN:
			base16_character_size = 4;
			break;

		default:
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: unsupported base16 variant.",
			 function );

			return( -1 );
	}
	if( base16_stream_size < base16_character_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: invalid base16 stream value too small.",
		 function );

		return( -1 );
	}
	if( ( flags & ~( LIBUNA_BASE16_FLAG_STRIP_WHITESPACE ) ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported flags.",
		 function );

		return( -1 );
	}
	if( ( flags & LIBUNA_BASE16_FLAG_STRIP_WHITESPACE ) == 0 )
	{
		strip_mode = LIBUNA_STRIP_MODE_NON_WHITESPACE;
	}
	base16_stream_index = base16_stream_size - base16_character_size;
	whitespace_size     = 0;

	while( base16_stream_index > base16_character_size )
	{
		switch( base16_variant & 0xf0000000UL )
		{
			case LIBUNA_BASE16_VARIANT_ENCODING_BYTE_STREAM:
				base16_character1 = base16_stream[ base16_stream_index ];
				break;

			case LIBUNA_BASE16_VARIANT_ENCODING_UTF16_BIG_ENDIAN:
				byte_stream_copy_to_uint16_big_endian(
				 &( base16_stream[ base16_stream_index ] ),
				 base16_character1 );
				break;

			case LIBUNA_BASE16_VARIANT_ENCODING_UTF16_LITTLE_ENDIAN:
				byte_stream_copy_to_uint16_little_endian(
				 &( base16_stream[ base16_stream_index ] ),
				 base16_character1 );
				break;

			case LIBUNA_BASE16_VARIANT_ENCODING_UTF32_BIG_ENDIAN:
				byte_stream_copy_to_uint32_big_endian(
				 &( base16_stream[ base16_stream_index ] ),
				 base16_character1 );
				break;

			case LIBUNA_BASE16_VARIANT_ENCODING_UTF32_LITTLE_ENDIAN:
				byte_stream_copy_to_uint32_little_endian(
				 &( base16_stream[ base16_stream_index ] ),
				 base16_character1 );
				break;
		}
		if( ( base16_character1 & 0xffffff00UL ) != 0 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: invalid base16 character at index: %" PRIzd ".",
			 function,
			 base16_stream_index );

			return( -1 );
		}
		base16_stream_index -= base16_character_size;

		if( ( base16_character1 == (uint32_t) '\n' )
		 || ( base16_character1 == (uint32_t) '\r' ) )
		{
			whitespace_size += base16_character_size;
		}
		else if( ( flags & LIBUNA_BASE16_FLAG_STRIP_WHITESPACE ) == 0 )
		{
			break;
		}
		else if( ( base16_character1 == (uint32_t) ' ' )
		      || ( base16_character1 == (uint32_t) '\t' )
		      || ( base16_character1 == (uint32_t) '\v' ) )
		{
			whitespace_size += base16_character_size;
		}
		else
		{
			break;
		}
	}
	base16_stream_size -= whitespace_size;

	if( base16_stream_size < base16_character_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: invalid base16 stream value too small.",
		 function );

		return( -1 );
	}
	base16_stream_index = 0;

	while( base16_stream_index < base16_stream_size )
	{
		switch( base16_variant & 0xf0000000UL )
		{
			case LIBUNA_BASE16_VARIANT_ENCODING_BYTE_STREAM:
				base16_character1 = base16_stream[ base16_stream_index ];
				break;

			case LIBUNA_BASE16_VARIANT_ENCODING_UTF16_BIG_ENDIAN:
				byte_stream_copy_to_uint16_big_endian(
				 &( base16_stream[ base16_stream_index ] ),
				 base16_character1 );
				break;

			case LIBUNA_BASE16_VARIANT_ENCODING_UTF16_LITTLE_ENDIAN:
				byte_stream_copy_to_uint16_little_endian(
				 &( base16_stream[ base16_stream_index ] ),
				 base16_character1 );
				break;

			case LIBUNA_BASE16_VARIANT_ENCODING_UTF32_BIG_ENDIAN:
				byte_stream_copy_to_uint32_big_endian(
				 &( base16_stream[ base16_stream_index ] ),
				 base16_character1 );
				break;

			case LIBUNA_BASE16_VARIANT_ENCODING_UTF32_LITTLE_ENDIAN:
				byte_stream_copy_to_uint32_little_endian(
				 &( base16_stream[ base16_stream_index ] ),
				 base16_character1 );
				break;
		}
		if( ( base16_character1 & 0xffffff00UL ) != 0 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: invalid base16 character at index: %" PRIzd ".",
			 base16_stream_index,
			 function );

			return( -1 );
		}
		base16_stream_index += base16_character_size;

		if( ( base16_character1 == (uint32_t) '\n' )
		 || ( base16_character1 == (uint32_t) '\r' ) )
		{
			if( ( strip_mode != LIBUNA_STRIP_MODE_NON_WHITESPACE )
			 && ( strip_mode != LIBUNA_STRIP_MODE_TRAILING_WHITESPACE ) )
			{
				strip_mode = LIBUNA_STRIP_MODE_INVALID_CHARACTER;
			}
			else
			{
				if( ( base16_stream_index + base16_character_size ) < base16_stream_size )
				{
					switch( base16_variant & 0xf0000000UL )
					{
						case LIBUNA_BASE16_VARIANT_ENCODING_BYTE_STREAM:
							base16_character2 = base16_stream[ base16_stream_index ];
							break;

						case LIBUNA_BASE16_VARIANT_ENCODING_UTF16_BIG_ENDIAN:
							byte_stream_copy_to_uint16_big_endian(
							 &( base16_stream[ base16_stream_index ] ),
							 base16_character2 );
							break;

						case LIBUNA_BASE16_VARIANT_ENCODING_UTF16_LITTLE_ENDIAN:
							byte_stream_copy_to_uint16_little_endian(
							 &( base16_stream[ base16_stream_index ] ),
							 base16_character2 );
							break;

						case LIBUNA_BASE16_VARIANT_ENCODING_UTF32_BIG_ENDIAN:
							byte_stream_copy_to_uint32_big_endian(
							 &( base16_stream[ base16_stream_index ] ),
							 base16_character2 );
							break;

						case LIBUNA_BASE16_VARIANT_ENCODING_UTF32_LITTLE_ENDIAN:
							byte_stream_copy_to_uint32_little_endian(
							 &( base16_stream[ base16_stream_index ] ),
							 base16_character2 );
							break;
					}
					if( ( base16_character2 & 0xffffff00UL ) != 0 )
					{
						libcerror_error_set(
						 error,
						 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
						 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
						 "%s: invalid base16 character at index: %" PRIzd ".",
						 function,
						 base16_stream_index );

						return( -1 );
					}
					if( ( base16_character2 == (uint32_t) '\n' )
					 || ( base16_character2 == (uint32_t) '\r' ) )
					{
						base16_stream_index += base16_character_size;
					}
				}
				strip_mode = LIBUNA_STRIP_MODE_LEADING_WHITESPACE;
			}
			if( character_limit != 0 )
			{
				if( number_of_characters != (size_t) character_limit )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_CONVERSION,
					 LIBCERROR_CONVERSION_ERROR_INPUT_FAILED,
					 "%s: number of characters in line does not match character limit.",
					 function );

					return( -1 );
				}
				number_of_characters = 0;
			}
		}
		else if( ( base16_character1 == (uint32_t) ' ' )
		      || ( base16_character1 == (uint32_t) '\t' )
		      || ( base16_character1 == (uint32_t) '\v' ) )
		{
			if( ( flags & LIBUNA_BASE16_FLAG_STRIP_WHITESPACE ) != 0 )
			{
				if( strip_mode == LIBUNA_STRIP_MODE_NON_WHITESPACE )
				{
					strip_mode = LIBUNA_STRIP_MODE_TRAILING_WHITESPACE;
				}
				if( ( strip_mode != LIBUNA_STRIP_MODE_LEADING_WHITESPACE )
				 && ( strip_mode != LIBUNA_STRIP_MODE_TRAILING_WHITESPACE ) )
				{
					strip_mode = LIBUNA_STRIP_MODE_INVALID_CHARACTER;
				}
			}
			else
			{
				strip_mode = LIBUNA_STRIP_MODE_INVALID_CHARACTER;
			}
		}
		else if( strip_mode == LIBUNA_STRIP_MODE_LEADING_WHITESPACE )
		{
			strip_mode = LIBUNA_STRIP_MODE_NON_WHITESPACE;
		}
		else if( strip_mode == LIBUNA_STRIP_MODE_TRAILING_WHITESPACE )
		{
			strip_mode = LIBUNA_STRIP_MODE_INVALID_CHARACTER;
		}
		if( strip_mode == LIBUNA_STRIP_MODE_INVALID_CHARACTER )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_CONVERSION,
			 LIBCERROR_CONVERSION_ERROR_INPUT_FAILED,
			 "%s: invalid character in base16 stream at index: %" PRIzd ".",
			 function,
			 base16_stream_index - base16_character_size );

			return( -1 );
		}
		if( strip_mode == LIBUNA_STRIP_MODE_NON_WHITESPACE )
		{
			byte_value = 0;

			if( ( base16_character1 >= (uint32_t) 'A' )
			 && ( base16_character1 <= (uint32_t) 'F' ) )
			{
				if( ( character_case != LIBUNA_CASE_MIXED )
				 && ( character_case != LIBUNA_CASE_UPPER ) )
				{
					strip_mode = LIBUNA_STRIP_MODE_INVALID_CHARACTER;
				}
				byte_value = (uint8_t) ( base16_character1 - (uint32_t) 'A' + 10 );
			}
			else if( ( base16_character1 >= (uint32_t) 'a' )
			      && ( base16_character1 <= (uint32_t) 'f' ) )
			{
				if( ( character_case != LIBUNA_CASE_MIXED )
				 && ( character_case != LIBUNA_CASE_LOWER ) )
				{
					strip_mode = LIBUNA_STRIP_MODE_INVALID_CHARACTER;
				}
				byte_value = (uint8_t) ( base16_character1 - (uint32_t) 'a' + 10 );
			}
			else if( ( base16_character1 >= (uint32_t) '0' )
			      && ( base16_character1 <= (uint32_t) '9' ) )
			{
				byte_value = (uint8_t) ( base16_character1 - (uint32_t) '0' );
			}
			else
			{
				strip_mode = LIBUNA_STRIP_MODE_INVALID_CHARACTER;
			}
			if( strip_mode == LIBUNA_STRIP_MODE_INVALID_CHARACTER )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
				 "%s: invalid base16 character stream at index: %" PRIzd ".",
				 function,
				 base16_stream_index - base16_character_size );

				return( -1 );
			}
			byte_value <<= 4;

			switch( base16_variant & 0xf0000000UL )
			{
				case LIBUNA_BASE16_VARIANT_ENCODING_BYTE_STREAM:
					base16_character1 = base16_stream[ base16_stream_index ];
					break;

				case LIBUNA_BASE16_VARIANT_ENCODING_UTF16_BIG_ENDIAN:
					byte_stream_copy_to_uint16_big_endian(
					 &( base16_stream[ base16_stream_index ] ),
					 base16_character1 );
					break;

				case LIBUNA_BASE16_VARIANT_ENCODING_UTF16_LITTLE_ENDIAN:
					byte_stream_copy_to_uint16_little_endian(
					 &( base16_stream[ base16_stream_index ] ),
					 base16_character1 );
					break;

				case LIBUNA_BASE16_VARIANT_ENCODING_UTF32_BIG_ENDIAN:
					byte_stream_copy_to_uint32_big_endian(
					 &( base16_stream[ base16_stream_index ] ),
					 base16_character1 );
					break;

				case LIBUNA_BASE16_VARIANT_ENCODING_UTF32_LITTLE_ENDIAN:
					byte_stream_copy_to_uint32_little_endian(
					 &( base16_stream[ base16_stream_index ] ),
					 base16_character1 );
					break;
			}
			if( ( base16_character1 & 0xffffff00UL ) != 0 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
				 "%s: invalid base16 character at index: %" PRIzd ".",
				 base16_stream_index,
				 function );

				return( -1 );
			}
			base16_stream_index += base16_character_size;

			if( ( base16_character1 >= (uint32_t) 'A' )
			 && ( base16_character1 <= (uint32_t) 'F' ) )
			{
				if( ( character_case != LIBUNA_CASE_MIXED )
				 && ( character_case != LIBUNA_CASE_UPPER ) )
				{
					strip_mode = LIBUNA_STRIP_MODE_INVALID_CHARACTER;
				}
				byte_value |= (uint8_t) ( base16_character1 - (uint32_t) 'A' + 10 );
			}
			else if( ( base16_character1 >= (uint32_t) 'a' )
			      && ( base16_character1 <= (uint32_t) 'f' ) )
			{
				if( ( character_case != LIBUNA_CASE_MIXED )
				 && ( character_case != LIBUNA_CASE_LOWER ) )
				{
					strip_mode = LIBUNA_STRIP_MODE_INVALID_CHARACTER;
				}
				byte_value |= (uint8_t) ( base16_character1 - (uint32_t) 'a' + 10 );
			}
			else if( ( base16_character1 >= (uint32_t) '0' )
			      && ( base16_character1 <= (uint32_t) '9' ) )
			{
				byte_value |= (uint8_t) ( base16_character1 - (uint32_t) '0' );
			}
			else
			{
				strip_mode = LIBUNA_STRIP_MODE_INVALID_CHARACTER;
			}
			if( strip_mode == LIBUNA_STRIP_MODE_INVALID_CHARACTER )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
				 "%s: invalid base16 character stream at index: %" PRIzd ".",
				 function,
				 base16_stream_index - base16_character_size );

				return( -1 );
			}
			if( byte_stream_index >= byte_stream_size )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: byte stream is too small.",
				 function );

				return( -1 );
			}
			byte_stream[ byte_stream_index++ ] = byte_value;

			number_of_characters += 2;
		}
	}
	if( character_limit != 0 )
	{
		if( number_of_characters > (size_t) character_limit )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_CONVERSION,
			 LIBCERROR_CONVERSION_ERROR_INPUT_FAILED,
			 "%s: number of characters in last line exceed maximum.",
			 function );

			return( -1 );
		}
	}
	return( 1 );
}

/* Determines the size of a base16 stream from a byte stream
 * Returns 1 if successful or -1 on error
 */
int libuna_base16_stream_size_from_byte_stream(
     const uint8_t *byte_stream,
     size_t byte_stream_size,
     size_t *base16_stream_size,
     uint32_t base16_variant,
     libcerror_error_t **error )
{
	static char *function        = "libuna_base16_stream_size_from_byte_stream";
	size_t base16_character_size = 0;
	size_t whitespace_size       = 0;
	uint8_t character_limit      = 0;

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
	if( byte_stream_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid byte stream size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( base16_stream_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid base16 stream size.",
		 function );

		return( -1 );
	}
	switch( base16_variant & 0x000000ffUL )
	{
		case LIBUNA_BASE16_VARIANT_CHARACTER_LIMIT_NONE:
			character_limit = 0;
			break;

		case LIBUNA_BASE16_VARIANT_CHARACTER_LIMIT_64:
			character_limit = 64;
			break;

		case LIBUNA_BASE16_VARIANT_CHARACTER_LIMIT_76:
			character_limit = 76;
			break;

		default:
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: unsupported base16 variant.",
			 function );

			return( -1 );
	}
	switch( base16_variant & 0xf0000000UL )
	{
		case LIBUNA_BASE16_VARIANT_ENCODING_BYTE_STREAM:
			base16_character_size = 1;
			break;

		case LIBUNA_BASE16_VARIANT_ENCODING_UTF16_BIG_ENDIAN:
		case LIBUNA_BASE16_VARIANT_ENCODING_UTF16_LITTLE_ENDIAN:
			base16_character_size = 2;
			break;

		case LIBUNA_BASE16_VARIANT_ENCODING_UTF32_BIG_ENDIAN:
		case LIBUNA_BASE16_VARIANT_ENCODING_UTF32_LITTLE_ENDIAN:
			base16_character_size = 4;
			break;

		default:
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: unsupported base16 variant.",
			 function );

			return( -1 );
	}
	/* The base16 stream contains 2 characters for every byte
	 */
	*base16_stream_size = byte_stream_size * 2;

	if( character_limit != 0 )
	{
		whitespace_size = *base16_stream_size / character_limit;

		if( ( *base16_stream_size % character_limit ) != 0 )
		{
			whitespace_size += 1;
		}
		*base16_stream_size += whitespace_size;
	}
	*base16_stream_size *= base16_character_size;

	return( 1 );
}

/* Copies a base16 stream from a byte stream
 * Returns 1 if successful or -1 on error
 */
int libuna_base16_stream_copy_from_byte_stream(
     uint8_t *base16_stream,
     size_t base16_stream_size,
     const uint8_t *byte_stream,
     size_t byte_stream_size,
     uint32_t base16_variant,
     libcerror_error_t **error )
{
	static char *function      = "libuna_base16_stream_copy_from_byte_stream";
	size_t base16_stream_index = 0;

	if( libuna_base16_stream_with_index_copy_from_byte_stream(
	     base16_stream,
	     base16_stream_size,
	     &base16_stream_index,
	     byte_stream,
	     byte_stream_size,
	     base16_variant,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
		 "%s: unable to copy base16 stream from byte stream.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Copies a base16 stream from a byte stream
 * Returns 1 if successful or -1 on error
 */
int libuna_base16_stream_with_index_copy_from_byte_stream(
     uint8_t *base16_stream,
     size_t base16_stream_size,
     size_t *base16_stream_index,
     const uint8_t *byte_stream,
     size_t byte_stream_size,
     uint32_t base16_variant,
     libcerror_error_t **error )
{
	static char *function                = "libuna_base16_stream_with_index_copy_from_byte_stream";
	size_t calculated_base16_stream_size = 0;
	size_t base16_character_size         = 0;
	size_t stream_index                  = 0;
	size_t byte_stream_index             = 0;
	size_t number_of_characters          = 0;
	size_t whitespace_size               = 0;
	uint32_t a_character_value           = 0;
	uint32_t base16_character            = 0;
	uint8_t character_limit              = 0;

	if( base16_stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid base16 stream.",
		 function );

		return( -1 );
	}
	if( base16_stream_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid base16 stream size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( base16_stream_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid base16 stream index.",
		 function );

		return( -1 );
	}
	if( *base16_stream_index >= base16_stream_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: base16 stream string too small.",
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
	if( byte_stream_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid byte stream size value exceeds maximum.",
		 function );

		return( -1 );
	}
	switch( base16_variant & 0x000000ffUL )
	{
		case LIBUNA_BASE16_VARIANT_CHARACTER_LIMIT_NONE:
			character_limit = 0;
			break;

		case LIBUNA_BASE16_VARIANT_CHARACTER_LIMIT_64:
			character_limit = 64;
			break;

		case LIBUNA_BASE16_VARIANT_CHARACTER_LIMIT_76:
			character_limit = 76;
			break;

		default:
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: unsupported base16 variant.",
			 function );

			return( -1 );
	}
	switch( base16_variant & 0x000f0000UL )
	{
		case LIBUNA_BASE16_VARIANT_CASE_LOWER:
			a_character_value = (uint32_t) 'a' - 10;
			break;

		case LIBUNA_BASE16_VARIANT_CASE_MIXED:
		case LIBUNA_BASE16_VARIANT_CASE_UPPER:
			a_character_value = (uint32_t) 'A' - 10;
			break;

		default:
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: unsupported base16 variant.",
			 function );

			return( -1 );
	}
	switch( base16_variant & 0xf0000000UL )
	{
		case LIBUNA_BASE16_VARIANT_ENCODING_BYTE_STREAM:
			base16_character_size = 1;
			break;

		case LIBUNA_BASE16_VARIANT_ENCODING_UTF16_BIG_ENDIAN:
		case LIBUNA_BASE16_VARIANT_ENCODING_UTF16_LITTLE_ENDIAN:
			base16_character_size = 2;
			break;

		case LIBUNA_BASE16_VARIANT_ENCODING_UTF32_BIG_ENDIAN:
		case LIBUNA_BASE16_VARIANT_ENCODING_UTF32_LITTLE_ENDIAN:
			base16_character_size = 4;
			break;

		default:
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: unsupported base16 variant.",
			 function );

			return( -1 );
	}
	stream_index = *base16_stream_index;

	/* Make sure the base16 stream is able to hold
	 * at least 2 bytes for each byte
	 */
	calculated_base16_stream_size = byte_stream_size * 2;

	if( character_limit != 0 )
	{
		whitespace_size = calculated_base16_stream_size / character_limit;

		if( ( calculated_base16_stream_size % character_limit ) != 0 )
		{
			whitespace_size += 1;
		}
		calculated_base16_stream_size += whitespace_size;
	}
	calculated_base16_stream_size *= base16_character_size;

	if( base16_stream_size < calculated_base16_stream_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: base16 stream is too small.",
		 function );

		return( -1 );
	}
	while( byte_stream_index < byte_stream_size )
	{
		base16_character = byte_stream[ byte_stream_index ] >> 4;

		if( base16_character <= 9 )
		{
			base16_character += (uint32_t) '0';
		}
		else
		{
			base16_character += a_character_value;
		}
		switch( base16_variant & 0xf0000000UL )
		{
			case LIBUNA_BASE16_VARIANT_ENCODING_BYTE_STREAM:
				base16_stream[ stream_index ] = (uint8_t) base16_character;
				break;

			case LIBUNA_BASE16_VARIANT_ENCODING_UTF16_BIG_ENDIAN:
				byte_stream_copy_from_uint16_big_endian(
				 &( base16_stream[ stream_index ] ),
				 base16_character );
				break;

			case LIBUNA_BASE16_VARIANT_ENCODING_UTF16_LITTLE_ENDIAN:
				byte_stream_copy_from_uint16_little_endian(
				 &( base16_stream[ stream_index ] ),
				 base16_character );
				break;

			case LIBUNA_BASE16_VARIANT_ENCODING_UTF32_BIG_ENDIAN:
				byte_stream_copy_from_uint32_big_endian(
				 &( base16_stream[ stream_index ] ),
				 base16_character );
				break;

			case LIBUNA_BASE16_VARIANT_ENCODING_UTF32_LITTLE_ENDIAN:
				byte_stream_copy_from_uint32_little_endian(
				 &( base16_stream[ stream_index ] ),
				 base16_character );
				break;
		}
		stream_index += base16_character_size;

		base16_character = byte_stream[ byte_stream_index ] & 0x0f;

		if( base16_character <= 9 )
		{
			base16_character += (uint32_t) '0';
		}
		else
		{
			base16_character += a_character_value;
		}
		switch( base16_variant & 0xf0000000UL )
		{
			case LIBUNA_BASE16_VARIANT_ENCODING_BYTE_STREAM:
				base16_stream[ stream_index ] = (uint8_t) base16_character;
				break;

			case LIBUNA_BASE16_VARIANT_ENCODING_UTF16_BIG_ENDIAN:
				byte_stream_copy_from_uint16_big_endian(
				 &( base16_stream[ stream_index ] ),
				 base16_character );
				break;

			case LIBUNA_BASE16_VARIANT_ENCODING_UTF16_LITTLE_ENDIAN:
				byte_stream_copy_from_uint16_little_endian(
				 &( base16_stream[ stream_index ] ),
				 base16_character );
				break;

			case LIBUNA_BASE16_VARIANT_ENCODING_UTF32_BIG_ENDIAN:
				byte_stream_copy_from_uint32_big_endian(
				 &( base16_stream[ stream_index ] ),
				 base16_character );
				break;

			case LIBUNA_BASE16_VARIANT_ENCODING_UTF32_LITTLE_ENDIAN:
				byte_stream_copy_from_uint32_little_endian(
				 &( base16_stream[ stream_index ] ),
				 base16_character );
				break;
		}
		stream_index += base16_character_size;

		if( character_limit != 0 )
		{
			number_of_characters += 2;

			if( number_of_characters >= (size_t) character_limit )
			{
				base16_character = (uint32_t) '\n';

				switch( base16_variant & 0xf0000000UL )
				{
					case LIBUNA_BASE16_VARIANT_ENCODING_BYTE_STREAM:
						base16_stream[ stream_index ] = (uint8_t) base16_character;
						break;

					case LIBUNA_BASE16_VARIANT_ENCODING_UTF16_BIG_ENDIAN:
						byte_stream_copy_from_uint16_big_endian(
						 &( base16_stream[ stream_index ] ),
						 base16_character );
						break;

					case LIBUNA_BASE16_VARIANT_ENCODING_UTF16_LITTLE_ENDIAN:
						byte_stream_copy_from_uint16_little_endian(
						 &( base16_stream[ stream_index ] ),
						 base16_character );
						break;

					case LIBUNA_BASE16_VARIANT_ENCODING_UTF32_BIG_ENDIAN:
						byte_stream_copy_from_uint32_big_endian(
						 &( base16_stream[ stream_index ] ),
						 base16_character );
						break;

					case LIBUNA_BASE16_VARIANT_ENCODING_UTF32_LITTLE_ENDIAN:
						byte_stream_copy_from_uint32_little_endian(
						 &( base16_stream[ stream_index ] ),
						 base16_character );
						break;
				}
				stream_index += base16_character_size;

				number_of_characters = 0;
			}
		}
		byte_stream_index++;
	}
	if( character_limit != 0 )
	{
		if( number_of_characters != 0 )
		{
			base16_character = (uint32_t) '\n';

			switch( base16_variant & 0xf0000000UL )
			{
				case LIBUNA_BASE16_VARIANT_ENCODING_BYTE_STREAM:
					base16_stream[ stream_index ] = (uint8_t) base16_character;
					break;

				case LIBUNA_BASE16_VARIANT_ENCODING_UTF16_BIG_ENDIAN:
					byte_stream_copy_from_uint16_big_endian(
					 &( base16_stream[ stream_index ] ),
					 base16_character );
					break;

				case LIBUNA_BASE16_VARIANT_ENCODING_UTF16_LITTLE_ENDIAN:
					byte_stream_copy_from_uint16_little_endian(
					 &( base16_stream[ stream_index ] ),
					 base16_character );
					break;

				case LIBUNA_BASE16_VARIANT_ENCODING_UTF32_BIG_ENDIAN:
					byte_stream_copy_from_uint32_big_endian(
					 &( base16_stream[ stream_index ] ),
					 base16_character );
					break;

				case LIBUNA_BASE16_VARIANT_ENCODING_UTF32_LITTLE_ENDIAN:
					byte_stream_copy_from_uint32_little_endian(
					 &( base16_stream[ stream_index ] ),
					 base16_character );
					break;
			}
			stream_index += base16_character_size;
		}
	}
	*base16_stream_index = stream_index;

	return( 1 );
}

