/*
 * Unicode character functions
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
#include <types.h>

#include "libuna_base64_stream.h"
#include "libuna_codepage_iso_8859_2.h"
#include "libuna_codepage_iso_8859_3.h"
#include "libuna_codepage_iso_8859_4.h"
#include "libuna_codepage_iso_8859_5.h"
#include "libuna_codepage_iso_8859_6.h"
#include "libuna_codepage_iso_8859_7.h"
#include "libuna_codepage_iso_8859_8.h"
#include "libuna_codepage_iso_8859_9.h"
#include "libuna_codepage_iso_8859_10.h"
#include "libuna_codepage_iso_8859_13.h"
#include "libuna_codepage_iso_8859_14.h"
#include "libuna_codepage_iso_8859_15.h"
#include "libuna_codepage_iso_8859_16.h"
#include "libuna_codepage_koi8_r.h"
#include "libuna_codepage_koi8_u.h"
#include "libuna_codepage_windows_874.h"
#include "libuna_codepage_windows_932.h"
#include "libuna_codepage_windows_936.h"
#include "libuna_codepage_windows_949.h"
#include "libuna_codepage_windows_950.h"
#include "libuna_codepage_windows_1250.h"
#include "libuna_codepage_windows_1251.h"
#include "libuna_codepage_windows_1252.h"
#include "libuna_codepage_windows_1253.h"
#include "libuna_codepage_windows_1254.h"
#include "libuna_codepage_windows_1255.h"
#include "libuna_codepage_windows_1256.h"
#include "libuna_codepage_windows_1257.h"
#include "libuna_codepage_windows_1258.h"
#include "libuna_definitions.h"
#include "libuna_inline.h"
#include "libuna_libcerror.h"
#include "libuna_types.h"
#include "libuna_unicode_character.h"
#include "libuna_unused.h"

/* Determines the size of a byte stream character from an Unicode character
 * Adds the size to the byte stream character size value
 * Returns 1 if successful or -1 on error
 */
LIBUNA_INLINE \
int libuna_unicode_character_size_to_byte_stream(
     libuna_unicode_character_t unicode_character,
     int codepage,
     size_t *byte_stream_character_size,
     libcerror_error_t **error )
{
	static char *function = "libuna_unicode_character_size_to_byte_stream";
	int result            = 1;

	if( byte_stream_character_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid byte stream character size.",
		 function );

		return( -1 );
	}
	switch( codepage )
	{
		case LIBUNA_CODEPAGE_ASCII:
		case LIBUNA_CODEPAGE_ISO_8859_1:
		case LIBUNA_CODEPAGE_ISO_8859_2:
		case LIBUNA_CODEPAGE_ISO_8859_3:
		case LIBUNA_CODEPAGE_ISO_8859_4:
		case LIBUNA_CODEPAGE_ISO_8859_5:
		case LIBUNA_CODEPAGE_ISO_8859_6:
		case LIBUNA_CODEPAGE_ISO_8859_7:
		case LIBUNA_CODEPAGE_ISO_8859_8:
		case LIBUNA_CODEPAGE_ISO_8859_9:
		case LIBUNA_CODEPAGE_ISO_8859_10:
		case LIBUNA_CODEPAGE_ISO_8859_11:
		case LIBUNA_CODEPAGE_ISO_8859_13:
		case LIBUNA_CODEPAGE_ISO_8859_14:
		case LIBUNA_CODEPAGE_ISO_8859_15:
		case LIBUNA_CODEPAGE_ISO_8859_16:
		case LIBUNA_CODEPAGE_KOI8_R:
		case LIBUNA_CODEPAGE_KOI8_U:
		case LIBUNA_CODEPAGE_WINDOWS_874:
		case LIBUNA_CODEPAGE_WINDOWS_1250:
		case LIBUNA_CODEPAGE_WINDOWS_1251:
		case LIBUNA_CODEPAGE_WINDOWS_1252:
		case LIBUNA_CODEPAGE_WINDOWS_1253:
		case LIBUNA_CODEPAGE_WINDOWS_1254:
		case LIBUNA_CODEPAGE_WINDOWS_1255:
		case LIBUNA_CODEPAGE_WINDOWS_1256:
		case LIBUNA_CODEPAGE_WINDOWS_1257:
		case LIBUNA_CODEPAGE_WINDOWS_1258:
			*byte_stream_character_size += 1;
			break;

		case LIBUNA_CODEPAGE_WINDOWS_932:
			result = libuna_codepage_windows_932_unicode_character_size_to_byte_stream(
			          unicode_character,
			          byte_stream_character_size,
			          error );
			break;

		case LIBUNA_CODEPAGE_WINDOWS_936:
			result = libuna_codepage_windows_936_unicode_character_size_to_byte_stream(
			          unicode_character,
			          byte_stream_character_size,
			          error );
			break;

		case LIBUNA_CODEPAGE_WINDOWS_949:
			result = libuna_codepage_windows_949_unicode_character_size_to_byte_stream(
			          unicode_character,
			          byte_stream_character_size,
			          error );
			break;

		case LIBUNA_CODEPAGE_WINDOWS_950:
			result = libuna_codepage_windows_950_unicode_character_size_to_byte_stream(
			          unicode_character,
			          byte_stream_character_size,
			          error );
			break;

		default:
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: unsupported codepage: %d.",
			 function,
			 codepage );

			return( -1 );
	}
	if( result != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to determine byte stream character size.",
		 function );

		return( -1 );
	}
	return( result );
}

/* Copies a Unicode character from a byte stream
 * Returns 1 if successful or -1 on error
 */
LIBUNA_INLINE \
int libuna_unicode_character_copy_from_byte_stream(
     libuna_unicode_character_t *unicode_character,
     const uint8_t *byte_stream,
     size_t byte_stream_size,
     size_t *byte_stream_index,
     int codepage,
     libcerror_error_t **error )
{
	static char *function         = "libuna_unicode_character_copy_from_byte_stream";
	uint8_t byte_stream_character = 0;
	int result                    = 1;

	if( unicode_character == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid Unicode character.",
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
	if( byte_stream_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid byte stream index.",
		 function );

		return( -1 );
	}
	if( *byte_stream_index >= byte_stream_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: byte stream too small.",
		 function );

		return( -1 );
	}
	byte_stream_character = byte_stream[ *byte_stream_index ];

	switch( codepage )
	{
		case LIBUNA_CODEPAGE_ASCII:
			if( byte_stream_character < 0x80 )
			{
				*unicode_character = byte_stream_character;
			}
			else
			{
				*unicode_character = 0xfffd;
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_ISO_8859_1:
			*unicode_character = byte_stream_character;

			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_ISO_8859_2:
			if( byte_stream_character < 0xa0 )
			{
				*unicode_character = byte_stream_character;
			}
			else
			{
				byte_stream_character -= 0xa0;

				*unicode_character = libuna_codepage_iso_8859_2_byte_stream_to_unicode_base_0xa0[ byte_stream_character ];
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_ISO_8859_3:
			if( byte_stream_character < 0xa0 )
			{
				*unicode_character = byte_stream_character;
			}
			else
			{
				byte_stream_character -= 0xa0;

				*unicode_character = libuna_codepage_iso_8859_3_byte_stream_to_unicode_base_0xa0[ byte_stream_character ];
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_ISO_8859_4:
			if( byte_stream_character < 0xa0 )
			{
				*unicode_character = byte_stream_character;
			}
			else
			{
				byte_stream_character -= 0xa0;

				*unicode_character = libuna_codepage_iso_8859_4_byte_stream_to_unicode_base_0xa0[ byte_stream_character ];
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_ISO_8859_5:
			if( byte_stream_character < 0xa0 )
			{
				*unicode_character = byte_stream_character;
			}
			else
			{
				byte_stream_character -= 0xa0;

				*unicode_character = libuna_codepage_iso_8859_5_byte_stream_to_unicode_base_0xa0[ byte_stream_character ];
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_ISO_8859_6:
			if( byte_stream_character < 0xa0 )
			{
				*unicode_character = byte_stream_character;
			}
			else
			{
				byte_stream_character -= 0xa0;

				*unicode_character = libuna_codepage_iso_8859_6_byte_stream_to_unicode_base_0xa0[ byte_stream_character ];
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_ISO_8859_7:
			if( byte_stream_character < 0xa0 )
			{
				*unicode_character = byte_stream_character;
			}
			else
			{
				byte_stream_character -= 0xa0;

				*unicode_character = libuna_codepage_iso_8859_7_byte_stream_to_unicode_base_0xa0[ byte_stream_character ];
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_ISO_8859_8:
			if( byte_stream_character < 0xa0 )
			{
				*unicode_character = byte_stream_character;
			}
			else
			{
				byte_stream_character -= 0xa0;

				*unicode_character = libuna_codepage_iso_8859_8_byte_stream_to_unicode_base_0xa0[ byte_stream_character ];
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_ISO_8859_9:
			if( byte_stream_character < 0xd0 )
			{
				*unicode_character = byte_stream_character;
			}
			else
			{
				byte_stream_character -= 0xd0;

				*unicode_character = libuna_codepage_iso_8859_9_byte_stream_to_unicode_base_0xd0[ byte_stream_character ];
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_ISO_8859_10:
			if( byte_stream_character < 0xa0 )
			{
				*unicode_character = byte_stream_character;
			}
			else
			{
				byte_stream_character -= 0xa0;

				*unicode_character = libuna_codepage_iso_8859_10_byte_stream_to_unicode_base_0xa0[ byte_stream_character ];
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_ISO_8859_11:
			if( byte_stream_character < 0xa1 )
			{
				*unicode_character = byte_stream_character;
			}
			else if( byte_stream_character < 0xdb )
			{
				*unicode_character = byte_stream_character + 0x0d60;
			}
			else if( byte_stream_character < 0xdf )
			{
				*unicode_character = 0xfffd;
			}
			else if( byte_stream_character < 0xfc )
			{
				*unicode_character = byte_stream_character + 0x0d60;
			}
			else
			{
				*unicode_character = 0xfffd;
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_ISO_8859_13:
			if( byte_stream_character < 0xa0 )
			{
				*unicode_character = byte_stream_character;
			}
			else
			{
				byte_stream_character -= 0xa0;

				*unicode_character = libuna_codepage_iso_8859_13_byte_stream_to_unicode_base_0xa0[ byte_stream_character ];
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_ISO_8859_14:
			if( byte_stream_character < 0xa0 )
			{
				*unicode_character = byte_stream_character;
			}
			else
			{
				byte_stream_character -= 0xa0;

				*unicode_character = libuna_codepage_iso_8859_14_byte_stream_to_unicode_base_0xa0[ byte_stream_character ];
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_ISO_8859_15:
			if( ( byte_stream_character >= 0xa0 )
			 && ( byte_stream_character < 0xc0 ) )
			{
				byte_stream_character -= 0xa0;

				*unicode_character = libuna_codepage_iso_8859_15_byte_stream_to_unicode_base_0xa0[ byte_stream_character ];
			}
			else
			{
				*unicode_character = byte_stream_character;
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_ISO_8859_16:
			if( byte_stream_character < 0xa0 )
			{
				*unicode_character = byte_stream_character;
			}
			else
			{
				byte_stream_character -= 0xa0;

				*unicode_character = libuna_codepage_iso_8859_16_byte_stream_to_unicode_base_0xa0[ byte_stream_character ];
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_KOI8_R:
			if( byte_stream_character < 0x80 )
			{
				*unicode_character = byte_stream_character;
			}
			else
			{
				byte_stream_character -= 0x80;

				*unicode_character = libuna_codepage_koi8_r_byte_stream_to_unicode_base_0x80[ byte_stream_character ];
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_KOI8_U:
			if( byte_stream_character < 0x80 )
			{
				*unicode_character = byte_stream_character;
			}
			else
			{
				byte_stream_character -= 0x80;

				*unicode_character = libuna_codepage_koi8_u_byte_stream_to_unicode_base_0x80[ byte_stream_character ];
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_WINDOWS_874:
			if( byte_stream_character < 0x80 )
			{
				*unicode_character = byte_stream_character;
			}
			else
			{
				byte_stream_character -= 0x80;

				*unicode_character = libuna_codepage_windows_874_byte_stream_to_unicode_base_0x80[ byte_stream_character ];
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_WINDOWS_932:
			result = libuna_codepage_windows_932_copy_from_byte_stream(
			          unicode_character,
			          byte_stream,
			          byte_stream_size,
			          byte_stream_index,
			          error );
			break;

		case LIBUNA_CODEPAGE_WINDOWS_936:
			result = libuna_codepage_windows_936_copy_from_byte_stream(
			          unicode_character,
			          byte_stream,
			          byte_stream_size,
			          byte_stream_index,
			          error );
			break;

		case LIBUNA_CODEPAGE_WINDOWS_949:
			result = libuna_codepage_windows_949_copy_from_byte_stream(
			          unicode_character,
			          byte_stream,
			          byte_stream_size,
			          byte_stream_index,
			          error );
			break;

		case LIBUNA_CODEPAGE_WINDOWS_950:
			result = libuna_codepage_windows_950_copy_from_byte_stream(
			          unicode_character,
			          byte_stream,
			          byte_stream_size,
			          byte_stream_index,
			          error );
			break;

		case LIBUNA_CODEPAGE_WINDOWS_1250:
			if( byte_stream_character < 0x80 )
			{
				*unicode_character = byte_stream_character;
			}
			else
			{
				byte_stream_character -= 0x80;

				*unicode_character = libuna_codepage_windows_1250_byte_stream_to_unicode_base_0x80[ byte_stream_character ];
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_WINDOWS_1251:
			if( byte_stream_character < 0x80 )
			{
				*unicode_character = byte_stream_character;
			}
			else
			{
				byte_stream_character -= 0x80;

				*unicode_character = libuna_codepage_windows_1251_byte_stream_to_unicode_base_0x80[ byte_stream_character ];
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_WINDOWS_1252:
			if( ( byte_stream_character < 0x80 )
			 || ( byte_stream_character >= 0xa0 ) )
			{
				*unicode_character = byte_stream_character;
			}
			else
			{
				byte_stream_character -= 0x80;

				*unicode_character = libuna_codepage_windows_1252_byte_stream_to_unicode_base_0x80[ byte_stream_character ];
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_WINDOWS_1253:
			if( byte_stream_character < 0x80 )
			{
				*unicode_character = byte_stream_character;
			}
			else
			{
				byte_stream_character -= 0x80;

				*unicode_character = libuna_codepage_windows_1253_byte_stream_to_unicode_base_0x80[ byte_stream_character ];
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_WINDOWS_1254:
			if( byte_stream_character < 0x80 )
			{
				*unicode_character = byte_stream_character;
			}
			else if( byte_stream_character < 0xa0 )
			{
				byte_stream_character -= 0x80;

				*unicode_character = libuna_codepage_windows_1254_byte_stream_to_unicode_base_0x80[ byte_stream_character ];
			}
			else if( byte_stream_character < 0xd0 )
			{
				*unicode_character = byte_stream_character;
			}
			else if( byte_stream_character < 0xe0 )
			{
				byte_stream_character -= 0xd0;

				*unicode_character = libuna_codepage_windows_1254_byte_stream_to_unicode_base_0xd0[ byte_stream_character ];
			}
			else if( byte_stream_character < 0xf0 )
			{
				*unicode_character = byte_stream_character;
			}
			else
			{
				byte_stream_character -= 0xf0;

				*unicode_character = libuna_codepage_windows_1254_byte_stream_to_unicode_base_0xf0[ byte_stream_character ];
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_WINDOWS_1255:
			if( byte_stream_character < 0x80 )
			{
				*unicode_character = byte_stream_character;
			}
			else
			{
				byte_stream_character -= 0x80;

				*unicode_character = libuna_codepage_windows_1255_byte_stream_to_unicode_base_0x80[ byte_stream_character ];
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_WINDOWS_1256:
			if( byte_stream_character < 0x80 )
			{
				*unicode_character = byte_stream_character;
			}
			else
			{
				byte_stream_character -= 0x80;

				*unicode_character = libuna_codepage_windows_1256_byte_stream_to_unicode_base_0x80[ byte_stream_character ];
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_WINDOWS_1257:
			if( byte_stream_character < 0x80 )
			{
				*unicode_character = byte_stream_character;
			}
			else
			{
				byte_stream_character -= 0x80;

				*unicode_character = libuna_codepage_windows_1257_byte_stream_to_unicode_base_0x80[ byte_stream_character ];
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_WINDOWS_1258:
			if( byte_stream_character < 0x80 )
			{
				*unicode_character = byte_stream_character;
			}
			else
			{
				byte_stream_character -= 0x80;

				*unicode_character = libuna_codepage_windows_1258_byte_stream_to_unicode_base_0x80[ byte_stream_character ];
			}
			*byte_stream_index += 1;

			break;

		default:
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			"%s: unsupported codepage: %d.",
			 function,
			 codepage );

			return( -1 );
	}
	if( result != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
		 "%s: unable to copy Unicode character from byte stream.",
		 function );

		return( -1 );
	}
	return( result );
}

/* Copies a Unicode character to a byte stream
 * Returns 1 if successful or -1 on error
 */
LIBUNA_INLINE \
int libuna_unicode_character_copy_to_byte_stream(
     libuna_unicode_character_t unicode_character,
     uint8_t *byte_stream,
     size_t byte_stream_size,
     size_t *byte_stream_index,
     int codepage,
     libcerror_error_t **error )
{
	static char *function  = "libuna_unicode_character_copy_to_byte_stream";
	int result             = 1;

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
	if( byte_stream_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid byte stream index.",
		 function );

		return( -1 );
	}
	if( *byte_stream_index >= byte_stream_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: byte stream too small.",
		 function );

		return( -1 );
	}
	switch( codepage )
	{
		case LIBUNA_CODEPAGE_ASCII:
			if( unicode_character < 0x0080 )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) unicode_character;
			}
			else
			{
				byte_stream[ *byte_stream_index ] = 0x1a;
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_ISO_8859_1:
			if( unicode_character < 0x0100 )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) unicode_character;
			}
			else
			{
				byte_stream[ *byte_stream_index ] = 0x1a;
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_ISO_8859_2:
			if( unicode_character < 0x00a0 )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) unicode_character;
			}
			else if( ( unicode_character >= 0x00a0 )
			      && ( unicode_character < 0x0120 ) )
			{
				unicode_character -= 0x00a0;

				byte_stream[ *byte_stream_index ] = libuna_codepage_iso_8859_2_unicode_to_byte_stream_base_0x00a0[ unicode_character ];
			}
			else if( ( unicode_character >= 0x0138 )
			      && ( unicode_character < 0x0180 ) )
			{
				unicode_character -= 0x0138;

				byte_stream[ *byte_stream_index ] = libuna_codepage_iso_8859_2_unicode_to_byte_stream_base_0x0138[ unicode_character ];
			}
			else if( ( unicode_character >= 0x02d8 )
			      && ( unicode_character < 0x02e0 ) )
			{
				unicode_character -= 0x02d8;

				byte_stream[ *byte_stream_index ] = libuna_codepage_iso_8859_2_unicode_to_byte_stream_base_0x02d8[ unicode_character ];
			}
			else if( unicode_character == 0x02c7 )
			{
				byte_stream[ *byte_stream_index ] = 0xb7;
			}
			else
			{
				byte_stream[ *byte_stream_index ] = 0x1a;
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_ISO_8859_3:
			if( unicode_character < 0x00a0 )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) unicode_character;
			}
			else if( ( unicode_character >= 0x00a0 )
			      && ( unicode_character < 0x0100 ) )
			{
				unicode_character -= 0x00a0;

				byte_stream[ *byte_stream_index ] = libuna_codepage_iso_8859_3_unicode_to_byte_stream_base_0x00a0[ unicode_character ];
			}
			else if( ( unicode_character >= 0x0108 )
			      && ( unicode_character < 0x0110 ) )
			{
				unicode_character -= 0x0108;

				byte_stream[ *byte_stream_index ] = libuna_codepage_iso_8859_3_unicode_to_byte_stream_base_0x0108[ unicode_character ];
			}
			else if( ( unicode_character >= 0x0118 )
			      && ( unicode_character < 0x0128 ) )
			{
				unicode_character -= 0x0118;

				byte_stream[ *byte_stream_index ] = libuna_codepage_iso_8859_3_unicode_to_byte_stream_base_0x0118[ unicode_character ];
			}
			else if( ( unicode_character >= 0x0130 )
			      && ( unicode_character < 0x0138 ) )
			{
				unicode_character -= 0x0130;

				byte_stream[ *byte_stream_index ] = libuna_codepage_iso_8859_3_unicode_to_byte_stream_base_0x0130[ unicode_character ];
			}
			else if( ( unicode_character >= 0x0158 )
			      && ( unicode_character < 0x0160 ) )
			{
				unicode_character -= 0x0158;

				byte_stream[ *byte_stream_index ] = libuna_codepage_iso_8859_3_unicode_to_byte_stream_base_0x0158[ unicode_character ];
			}
			else switch( unicode_character )
			{
				case 0x016c:
					byte_stream[ *byte_stream_index ] = 0xdd;
					break;

				case 0x016d:
					byte_stream[ *byte_stream_index ] = 0xfd;
					break;

				case 0x017b:
					byte_stream[ *byte_stream_index ] = 0xaf;
					break;

				case 0x017c:
					byte_stream[ *byte_stream_index ] = 0xbf;
					break;

				case 0x02d8:
					byte_stream[ *byte_stream_index ] = 0xa2;
					break;

				case 0x02d9:
					byte_stream[ *byte_stream_index ] = 0xff;
					break;

				default:
					byte_stream[ *byte_stream_index ] = 0x1a;
					break;
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_ISO_8859_4:
			if( unicode_character < 0x00a0 )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) unicode_character;
			}
			else if( ( unicode_character >= 0x00a0 )
			      && ( unicode_character < 0x0158 ) )
			{
				unicode_character -= 0x00a0;

				byte_stream[ *byte_stream_index ] = libuna_codepage_iso_8859_4_unicode_to_byte_stream_base_0x00a0[ unicode_character ];
			}
			else if( ( unicode_character >= 0x0160 )
			      && ( unicode_character < 0x0180 ) )
			{
				unicode_character -= 0x0160;

				byte_stream[ *byte_stream_index ] = libuna_codepage_iso_8859_4_unicode_to_byte_stream_base_0x0160[ unicode_character ];
			}
			else switch( unicode_character )
			{
				case 0x02c7:
					byte_stream[ *byte_stream_index ] = 0xb7;
					break;

				case 0x02d9:
					byte_stream[ *byte_stream_index ] = 0xff;
					break;

				case 0x02db:
					byte_stream[ *byte_stream_index ] = 0xb2;
					break;

				default:
					byte_stream[ *byte_stream_index ] = 0x1a;
					break;
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_ISO_8859_5:
			if( unicode_character < 0x00a1 )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) unicode_character;
			}
			else if( ( unicode_character >= 0x0400 )
			      && ( unicode_character < 0x0460 ) )
			{
				unicode_character -= 0x0400;

				byte_stream[ *byte_stream_index ] = libuna_codepage_iso_8859_5_unicode_to_byte_stream_base_0x0400[ unicode_character ];
			}
			else switch( unicode_character )
			{
				case 0x00a7:
					byte_stream[ *byte_stream_index ] = 0xfd;
					break;

				case 0x00ad:
					byte_stream[ *byte_stream_index ] = 0xad;
					break;

				case 0x2116:
					byte_stream[ *byte_stream_index ] = 0xf0;
					break;

				default:
					byte_stream[ *byte_stream_index ] = 0x1a;
					break;
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_ISO_8859_6:
			if( unicode_character < 0x00a1 )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) unicode_character;
			}
			else if( ( unicode_character >= 0x0618 )
			      && ( unicode_character < 0x658 ) )
			{
				unicode_character -= 0x0618;

				byte_stream[ *byte_stream_index ] = libuna_codepage_iso_8859_6_unicode_to_byte_stream_base_0x0618[ unicode_character ];
			}
			else switch( unicode_character )
			{
				case 0x00a4:
					byte_stream[ *byte_stream_index ] = 0xa4;
					break;

				case 0x00ad:
					byte_stream[ *byte_stream_index ] = 0xad;
					break;

				case 0x060c:
					byte_stream[ *byte_stream_index ] = 0xac;
					break;

				default:
					byte_stream[ *byte_stream_index ] = 0x1a;
					break;
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_ISO_8859_7:
			if( unicode_character < 0x00a0 )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) unicode_character;
			}
			else if( ( unicode_character >= 0x00a0 )
			      && ( unicode_character < 0x00b8 ) )
			{
				unicode_character -= 0x00a0;

				byte_stream[ *byte_stream_index ] = libuna_codepage_iso_8859_7_unicode_to_byte_stream_base_0x00a0[ unicode_character ];
			}
			else if( ( unicode_character >= 0x0380 )
			      && ( unicode_character < 0x03d0 ) )
			{
				unicode_character -= 0x0380;

				byte_stream[ *byte_stream_index ] = libuna_codepage_iso_8859_7_unicode_to_byte_stream_base_0x0380[ unicode_character ];
			}
			else switch( unicode_character )
			{
				case 0x00bb:
					byte_stream[ *byte_stream_index ] = 0xbb;
					break;

				case 0x00bd:
					byte_stream[ *byte_stream_index ] = 0xbd;
					break;

				case 0x037a:
					byte_stream[ *byte_stream_index ] = 0xaa;
					break;

				case 0x2015:
					byte_stream[ *byte_stream_index ] = 0xaf;
					break;

				case 0x2018:
					byte_stream[ *byte_stream_index ] = 0xa1;
					break;

				case 0x2019:
					byte_stream[ *byte_stream_index ] = 0xa2;
					break;

				case 0x20ac:
					byte_stream[ *byte_stream_index ] = 0xa4;
					break;

				case 0x20af:
					byte_stream[ *byte_stream_index ] = 0xa5;
					break;

				default:
					byte_stream[ *byte_stream_index ] = 0x1a;
					break;
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_ISO_8859_8:
			if( unicode_character < 0x00a0 )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) unicode_character;
			}
			else if( ( unicode_character >= 0x00a0 )
			      && ( unicode_character < 0x00c0 ) )
			{
				unicode_character -= 0x00a0;

				byte_stream[ *byte_stream_index ] = libuna_codepage_iso_8859_8_unicode_to_byte_stream_base_0x00a0[ unicode_character ];
			}
			else if( ( unicode_character >= 0x05d0 )
			      && ( unicode_character < 0x05f0 ) )
			{
				unicode_character -= 0x05d0;

				byte_stream[ *byte_stream_index ] = libuna_codepage_iso_8859_8_unicode_to_byte_stream_base_0x05d0[ unicode_character ];
			}
			else switch( unicode_character )
			{
				case 0x00d7:
					byte_stream[ *byte_stream_index ] = 0xaa;
					break;

				case 0x00f7:
					byte_stream[ *byte_stream_index ] = 0xba;
					break;

				case 0x200e:
					byte_stream[ *byte_stream_index ] = 0xfd;
					break;

				case 0x200f:
					byte_stream[ *byte_stream_index ] = 0xfe;
					break;

				case 0x2017:
					byte_stream[ *byte_stream_index ] = 0xdf;
					break;

				default:
					byte_stream[ *byte_stream_index ] = 0x1a;
					break;
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_ISO_8859_9:
			if( unicode_character < 0x00d0 )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) unicode_character;
			}
			else if( ( unicode_character >= 0x00d0 )
			      && ( unicode_character < 0x0100 ) )
			{
				unicode_character -= 0x00d0;

				byte_stream[ *byte_stream_index ] = libuna_codepage_iso_8859_9_unicode_to_byte_stream_base_0x00d0[ unicode_character ];
			}
			else switch( unicode_character )
			{
				case 0x011e:
					byte_stream[ *byte_stream_index ] = 0xd0;
					break;

				case 0x011f:
					byte_stream[ *byte_stream_index ] = 0xf0;
					break;

				case 0x0130:
					byte_stream[ *byte_stream_index ] = 0xdd;
					break;

				case 0x0131:
					byte_stream[ *byte_stream_index ] = 0xfd;
					break;

				case 0x015e:
					byte_stream[ *byte_stream_index ] = 0xde;
					break;

				case 0x015f:
					byte_stream[ *byte_stream_index ] = 0xfe;
					break;

				default:
					byte_stream[ *byte_stream_index ] = 0x1a;
					break;
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_ISO_8859_10:
			if( unicode_character < 0x00a1 )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) unicode_character;
			}
			else if( ( unicode_character >= 0x00c0 )
			      && ( unicode_character < 0x0150 ) )
			{
				unicode_character -= 0x00c0;

				byte_stream[ *byte_stream_index ] = libuna_codepage_iso_8859_10_unicode_to_byte_stream_base_0x00c0[ unicode_character ];
			}
			else if( ( unicode_character >= 0x0160 )
			      && ( unicode_character < 0x0170 ) )
			{
				unicode_character -= 0x0160;

				byte_stream[ *byte_stream_index ] = libuna_codepage_iso_8859_10_unicode_to_byte_stream_base_0x0160[ unicode_character ];
			}
			else switch( unicode_character )
			{
				case 0x00a7:
					byte_stream[ *byte_stream_index ] = 0xa7;
					break;

				case 0x00ad:
					byte_stream[ *byte_stream_index ] = 0xad;
					break;

				case 0x00b0:
					byte_stream[ *byte_stream_index ] = 0xb0;
					break;

				case 0x00b7:
					byte_stream[ *byte_stream_index ] = 0xb7;
					break;

				case 0x0172:
					byte_stream[ *byte_stream_index ] = 0xd9;
					break;

				case 0x0173:
					byte_stream[ *byte_stream_index ] = 0xf9;
					break;

				case 0x017d:
					byte_stream[ *byte_stream_index ] = 0xac;
					break;

				case 0x017e:
					byte_stream[ *byte_stream_index ] = 0xbc;
					break;

				case 0x2015:
					byte_stream[ *byte_stream_index ] = 0xbd;
					break;

				default:
					byte_stream[ *byte_stream_index ] = 0x1a;
					break;
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_ISO_8859_11:
			if( unicode_character < 0x00a1 )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) unicode_character;
			}
			else if( ( unicode_character >= 0x0e01 )
			      && ( unicode_character < 0x0e3b ) )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) ( unicode_character - 0x0d60 );
			}
			else if( ( unicode_character >= 0x0e3f )
			      && ( unicode_character < 0x0e5c ) )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) ( unicode_character - 0x0d60 );
			}
			else
			{
				byte_stream[ *byte_stream_index ] = 0x1a;
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_ISO_8859_13:
			if( unicode_character < 0x00a0 )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) unicode_character;
			}
			else if( ( unicode_character >= 0x00a0 )
			      && ( unicode_character < 0x0180 ) )
			{
				unicode_character -= 0x00a0;

				byte_stream[ *byte_stream_index ] = libuna_codepage_iso_8859_13_unicode_to_byte_stream_base_0x00a0[ unicode_character ];
			}
			else if( ( unicode_character >= 0x2018 )
			       && ( unicode_character < 0x2020 ) )
			{
				unicode_character -= 0x2018;

				byte_stream[ *byte_stream_index ] = libuna_codepage_iso_8859_13_unicode_to_byte_stream_base_0x2018[ unicode_character ];
			}
			else
			{
				byte_stream[ *byte_stream_index ] = 0x1a;
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_ISO_8859_14:
			if( unicode_character < 0x00a1 )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) unicode_character;
			}
			else if( ( unicode_character >= 0x00c0 )
			      && ( unicode_character < 0x0100 ) )
			{
				unicode_character -= 0x00c0;

				byte_stream[ *byte_stream_index ] = libuna_codepage_iso_8859_14_unicode_to_byte_stream_base_0x00c0[ unicode_character ];
			}
			else if( ( unicode_character >= 0x0170 )
			      && ( unicode_character < 0x0178 ) )
			{
				unicode_character -= 0x0170;

				byte_stream[ *byte_stream_index ] = libuna_codepage_iso_8859_14_unicode_to_byte_stream_base_0x0170[ unicode_character ];
			}
			else if( ( unicode_character >= 0x1e80 )
			      && ( unicode_character < 0x1e88 ) )
			{
				unicode_character -= 0x1e80;

				byte_stream[ *byte_stream_index ] = libuna_codepage_iso_8859_14_unicode_to_byte_stream_base_0x1e80[ unicode_character ];
			}
			else switch( unicode_character )
			{
				case 0x00a3:
					byte_stream[ *byte_stream_index ] = 0xa3;
					break;

				case 0x00a7:
					byte_stream[ *byte_stream_index ] = 0xa7;
					break;

				case 0x00a9:
					byte_stream[ *byte_stream_index ] = 0xa9;
					break;

				case 0x00ad:
					byte_stream[ *byte_stream_index ] = 0xad;
					break;

				case 0x00ae:
					byte_stream[ *byte_stream_index ] = 0xae;
					break;

				case 0x00b6:
					byte_stream[ *byte_stream_index ] = 0xb6;
					break;

				case 0x010a:
					byte_stream[ *byte_stream_index ] = 0xa4;
					break;

				case 0x010b:
					byte_stream[ *byte_stream_index ] = 0xa5;
					break;

				case 0x0120:
					byte_stream[ *byte_stream_index ] = 0xb2;
					break;

				case 0x0121:
					byte_stream[ *byte_stream_index ] = 0xb3;
					break;

				case 0x0178:
					byte_stream[ *byte_stream_index ] = 0xaf;
					break;

				case 0x1e02:
					byte_stream[ *byte_stream_index ] = 0xa1;
					break;

				case 0x1e03:
					byte_stream[ *byte_stream_index ] = 0xa2;
					break;

				case 0x1e0a:
					byte_stream[ *byte_stream_index ] = 0xa6;
					break;

				case 0x1e0b:
					byte_stream[ *byte_stream_index ] = 0xab;
					break;

				case 0x1e1e:
					byte_stream[ *byte_stream_index ] = 0xb0;
					break;

				case 0x1e1f:
					byte_stream[ *byte_stream_index ] = 0xb1;
					break;

				case 0x1e40:
					byte_stream[ *byte_stream_index ] = 0xb4;
					break;

				case 0x1e41:
					byte_stream[ *byte_stream_index ] = 0xb5;
					break;

				case 0x1e56:
					byte_stream[ *byte_stream_index ] = 0xb7;
					break;

				case 0x1e57:
					byte_stream[ *byte_stream_index ] = 0xb9;
					break;

				case 0x1e60:
					byte_stream[ *byte_stream_index ] = 0xbb;
					break;

				case 0x1e61:
					byte_stream[ *byte_stream_index ] = 0xbf;
					break;

				case 0x1e6a:
					byte_stream[ *byte_stream_index ] = 0xd7;
					break;

				case 0x1e6b:
					byte_stream[ *byte_stream_index ] = 0xf7;
					break;

				case 0x1ef2:
					byte_stream[ *byte_stream_index ] = 0xac;
					break;

				case 0x1ef3:
					byte_stream[ *byte_stream_index ] = 0xbc;
					break;

				default:
					byte_stream[ *byte_stream_index ] = 0x1a;
					break;
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_ISO_8859_15:
			if( unicode_character < 0x00a0 )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) unicode_character;
			}
			else if( ( unicode_character >= 0x00a0 )
			      && ( unicode_character < 0x00c0 ) )
			{
				unicode_character -= 0x00a0;

				byte_stream[ *byte_stream_index ] = libuna_codepage_iso_8859_15_unicode_to_byte_stream_base_0x00a0[ unicode_character ];
			}
			else if( unicode_character < 0x0100 )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) unicode_character;
			}
			else switch( unicode_character )
			{
				case 0x0152:
					byte_stream[ *byte_stream_index ] = 0xbc;
					break;

				case 0x0153:
					byte_stream[ *byte_stream_index ] = 0xbd;
					break;

				case 0x0160:
					byte_stream[ *byte_stream_index ] = 0xa6;
					break;

				case 0x0161:
					byte_stream[ *byte_stream_index ] = 0xa8;
					break;

				case 0x0178:
					byte_stream[ *byte_stream_index ] = 0xbe;
					break;

				case 0x017d:
					byte_stream[ *byte_stream_index ] = 0xb4;
					break;

				case 0x017e:
					byte_stream[ *byte_stream_index ] = 0xb8;
					break;

				case 0x20ac:
					byte_stream[ *byte_stream_index ] = 0xa4;
					break;

				default:
					byte_stream[ *byte_stream_index ] = 0x1a;
					break;
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_ISO_8859_16:
			if( unicode_character < 0x00a1 )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) unicode_character;
			}
			else if( ( unicode_character >= 0x00a8 )
			      && ( unicode_character < 0x0108 ) )
			{
				unicode_character -= 0x00a8;

				byte_stream[ *byte_stream_index ] = libuna_codepage_iso_8859_16_unicode_to_byte_stream_base_0x00a8[ unicode_character ];
			}
			else if( ( unicode_character >= 0x0140 )
			      && ( unicode_character < 0x0148 ) )
			{
				unicode_character -= 0x0140;

				byte_stream[ *byte_stream_index ] = libuna_codepage_iso_8859_16_unicode_to_byte_stream_base_0x0140[ unicode_character ];
			}
			else if( ( unicode_character >= 0x0150 )
			      && ( unicode_character < 0x0158 ) )
			{
				unicode_character -= 0x0150;

				byte_stream[ *byte_stream_index ] = libuna_codepage_iso_8859_16_unicode_to_byte_stream_base_0x0150[ unicode_character ];
			}
			else if( ( unicode_character >= 0x0178 )
			      && ( unicode_character < 0x0180 ) )
			{
				unicode_character -= 0x0178;

				byte_stream[ *byte_stream_index ] = libuna_codepage_iso_8859_16_unicode_to_byte_stream_base_0x0178[ unicode_character ];
			}
			else if( ( unicode_character >= 0x0218 )
			      && ( unicode_character < 0x0220 ) )
			{
				unicode_character -= 0x0218;

				byte_stream[ *byte_stream_index ] = libuna_codepage_iso_8859_16_unicode_to_byte_stream_base_0x0218[ unicode_character ];
			}
			else switch( unicode_character )
			{
				case 0x00a7:
					byte_stream[ *byte_stream_index ] = 0xa7;
					break;

				case 0x010c:
					byte_stream[ *byte_stream_index ] = 0xb2;
					break;

				case 0x010d:
					byte_stream[ *byte_stream_index ] = 0xb9;
					break;

				case 0x0110:
					byte_stream[ *byte_stream_index ] = 0xd0;
					break;

				case 0x0111:
					byte_stream[ *byte_stream_index ] = 0xf0;
					break;

				case 0x0118:
					byte_stream[ *byte_stream_index ] = 0xdd;
					break;

				case 0x0119:
					byte_stream[ *byte_stream_index ] = 0xfd;
					break;

				case 0x015a:
					byte_stream[ *byte_stream_index ] = 0xd7;
					break;

				case 0x015b:
					byte_stream[ *byte_stream_index ] = 0xf7;
					break;

				case 0x0160:
					byte_stream[ *byte_stream_index ] = 0xa6;
					break;

				case 0x0161:
					byte_stream[ *byte_stream_index ] = 0xa8;
					break;

				case 0x0170:
					byte_stream[ *byte_stream_index ] = 0xd8;
					break;

				case 0x0171:
					byte_stream[ *byte_stream_index ] = 0xf8;
					break;

				case 0x201d:
					byte_stream[ *byte_stream_index ] = 0xb5;
					break;

				case 0x201e:
					byte_stream[ *byte_stream_index ] = 0xa5;
					break;

				case 0x20ac:
					byte_stream[ *byte_stream_index ] = 0xa4;
					break;

				default:
					byte_stream[ *byte_stream_index ] = 0x1a;
					break;
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_KOI8_R:
			if( unicode_character < 0x0080 )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) unicode_character;
			}
			else if( ( unicode_character >= 0x0410 )
			      && ( unicode_character < 0x0450 ) )
			{
				unicode_character -= 0x0410;

				byte_stream[ *byte_stream_index ] = libuna_codepage_koi8_r_unicode_to_byte_stream_base_0x0410[ unicode_character ];
			}
			else if( ( unicode_character >= 0x2550 )
			      && ( unicode_character < 0x2570 ) )
			{
				unicode_character -= 0x2550;

				byte_stream[ *byte_stream_index ] = libuna_codepage_koi8_r_unicode_to_byte_stream_base_0x2550[ unicode_character ];
			}
			else switch( unicode_character )
			{
				case 0x00a0:
					byte_stream[ *byte_stream_index ] = 0x9a;
					break;

				case 0x00a9:
					byte_stream[ *byte_stream_index ] = 0xbf;
					break;

				case 0x00b0:
					byte_stream[ *byte_stream_index ] = 0x9c;
					break;

				case 0x00b2:
					byte_stream[ *byte_stream_index ] = 0x9d;
					break;

				case 0x00b7:
					byte_stream[ *byte_stream_index ] = 0x9e;
					break;

				case 0x00f7:
					byte_stream[ *byte_stream_index ] = 0x9f;
					break;

				case 0x0401:
					byte_stream[ *byte_stream_index ] = 0xb3;
					break;

				case 0x0451:
					byte_stream[ *byte_stream_index ] = 0xa3;
					break;

				case 0x2219:
					byte_stream[ *byte_stream_index ] = 0x95;
					break;

				case 0x221a:
					byte_stream[ *byte_stream_index ] = 0x96;
					break;

				case 0x2248:
					byte_stream[ *byte_stream_index ] = 0x97;
					break;

				case 0x2264:
					byte_stream[ *byte_stream_index ] = 0x98;
					break;

				case 0x2265:
					byte_stream[ *byte_stream_index ] = 0x99;
					break;

				case 0x2320:
					byte_stream[ *byte_stream_index ] = 0x93;
					break;

				case 0x2321:
					byte_stream[ *byte_stream_index ] = 0x9b;
					break;

				case 0x2500:
					byte_stream[ *byte_stream_index ] = 0x80;
					break;

				case 0x2502:
					byte_stream[ *byte_stream_index ] = 0x81;
					break;

				case 0x250c:
					byte_stream[ *byte_stream_index ] = 0x82;
					break;

				case 0x2510:
					byte_stream[ *byte_stream_index ] = 0x83;
					break;

				case 0x2514:
					byte_stream[ *byte_stream_index ] = 0x84;
					break;

				case 0x2518:
					byte_stream[ *byte_stream_index ] = 0x85;
					break;

				case 0x251c:
					byte_stream[ *byte_stream_index ] = 0x86;
					break;

				case 0x2524:
					byte_stream[ *byte_stream_index ] = 0x87;
					break;

				case 0x252c:
					byte_stream[ *byte_stream_index ] = 0x88;
					break;

				case 0x2534:
					byte_stream[ *byte_stream_index ] = 0x89;
					break;

				case 0x253c:
					byte_stream[ *byte_stream_index ] = 0x8a;
					break;

				case 0x2580:
					byte_stream[ *byte_stream_index ] = 0x8b;
					break;

				case 0x2584:
					byte_stream[ *byte_stream_index ] = 0x8c;
					break;

				case 0x2588:
					byte_stream[ *byte_stream_index ] = 0x8d;
					break;

				case 0x258c:
					byte_stream[ *byte_stream_index ] = 0x8e;
					break;

				case 0x2590:
					byte_stream[ *byte_stream_index ] = 0x8f;
					break;

				case 0x2591:
					byte_stream[ *byte_stream_index ] = 0x90;
					break;

				case 0x2592:
					byte_stream[ *byte_stream_index ] = 0x91;
					break;

				case 0x2593:
					byte_stream[ *byte_stream_index ] = 0x92;
					break;

				case 0x25a0:
					byte_stream[ *byte_stream_index ] = 0x94;
					break;

				default:
					byte_stream[ *byte_stream_index ] = 0x1a;
					break;
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_KOI8_U:
			if( unicode_character < 0x0080 )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) unicode_character;
			}
			else if( ( unicode_character >= 0x0410 )
			      && ( unicode_character < 0x0450 ) )
			{
				unicode_character -= 0x0410;

				byte_stream[ *byte_stream_index ] = libuna_codepage_koi8_u_unicode_to_byte_stream_base_0x0410[ unicode_character ];
			}
			else if( ( unicode_character >= 0x2550 )
			      && ( unicode_character < 0x2570 ) )
			{
				unicode_character -= 0x2550;

				byte_stream[ *byte_stream_index ] = libuna_codepage_koi8_u_unicode_to_byte_stream_base_0x2550[ unicode_character ];
			}
			else switch( unicode_character )
			{
				case 0x00a0:
					byte_stream[ *byte_stream_index ] = 0x9a;
					break;

				case 0x00a9:
					byte_stream[ *byte_stream_index ] = 0xbf;
					break;

				case 0x00b0:
					byte_stream[ *byte_stream_index ] = 0x9c;
					break;

				case 0x00b2:
					byte_stream[ *byte_stream_index ] = 0x9d;
					break;

				case 0x00b7:
					byte_stream[ *byte_stream_index ] = 0x9e;
					break;

				case 0x00f7:
					byte_stream[ *byte_stream_index ] = 0x9f;
					break;

				case 0x0401:
					byte_stream[ *byte_stream_index ] = 0xb3;
					break;

				case 0x0404:
					byte_stream[ *byte_stream_index ] = 0xb4;
					break;

				case 0x0406:
					byte_stream[ *byte_stream_index ] = 0xb6;
					break;

				case 0x0407:
					byte_stream[ *byte_stream_index ] = 0xb7;
					break;

				case 0x0451:
					byte_stream[ *byte_stream_index ] = 0xa3;
					break;

				case 0x0454:
					byte_stream[ *byte_stream_index ] = 0xa4;
					break;

				case 0x0456:
					byte_stream[ *byte_stream_index ] = 0xa6;
					break;

				case 0x0457:
					byte_stream[ *byte_stream_index ] = 0xa7;
					break;

				case 0x0490:
					byte_stream[ *byte_stream_index ] = 0xbd;
					break;

				case 0x0491:
					byte_stream[ *byte_stream_index ] = 0xad;
					break;

				case 0x2219:
					byte_stream[ *byte_stream_index ] = 0x95;
					break;

				case 0x221a:
					byte_stream[ *byte_stream_index ] = 0x96;
					break;

				case 0x2248:
					byte_stream[ *byte_stream_index ] = 0x97;
					break;

				case 0x2264:
					byte_stream[ *byte_stream_index ] = 0x98;
					break;

				case 0x2265:
					byte_stream[ *byte_stream_index ] = 0x99;
					break;

				case 0x2320:
					byte_stream[ *byte_stream_index ] = 0x93;
					break;

				case 0x2321:
					byte_stream[ *byte_stream_index ] = 0x9b;
					break;

				case 0x2500:
					byte_stream[ *byte_stream_index ] = 0x80;
					break;

				case 0x2502:
					byte_stream[ *byte_stream_index ] = 0x81;
					break;

				case 0x250c:
					byte_stream[ *byte_stream_index ] = 0x82;
					break;

				case 0x2510:
					byte_stream[ *byte_stream_index ] = 0x83;
					break;

				case 0x2514:
					byte_stream[ *byte_stream_index ] = 0x84;
					break;

				case 0x2518:
					byte_stream[ *byte_stream_index ] = 0x85;
					break;

				case 0x251c:
					byte_stream[ *byte_stream_index ] = 0x86;
					break;

				case 0x2524:
					byte_stream[ *byte_stream_index ] = 0x87;
					break;

				case 0x252c:
					byte_stream[ *byte_stream_index ] = 0x88;
					break;

				case 0x2534:
					byte_stream[ *byte_stream_index ] = 0x89;
					break;

				case 0x253c:
					byte_stream[ *byte_stream_index ] = 0x8a;
					break;

				case 0x2580:
					byte_stream[ *byte_stream_index ] = 0x8b;
					break;

				case 0x2584:
					byte_stream[ *byte_stream_index ] = 0x8c;
					break;

				case 0x2588:
					byte_stream[ *byte_stream_index ] = 0x8d;
					break;

				case 0x258c:
					byte_stream[ *byte_stream_index ] = 0x8e;
					break;

				case 0x2590:
					byte_stream[ *byte_stream_index ] = 0x8f;
					break;

				case 0x2591:
					byte_stream[ *byte_stream_index ] = 0x90;
					break;

				case 0x2592:
					byte_stream[ *byte_stream_index ] = 0x91;
					break;

				case 0x2593:
					byte_stream[ *byte_stream_index ] = 0x92;
					break;

				case 0x25a0:
					byte_stream[ *byte_stream_index ] = 0x94;
					break;

				default:
					byte_stream[ *byte_stream_index ] = 0x1a;
					break;
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_WINDOWS_874:
			if( ( unicode_character < 0x0080 )
			 || ( unicode_character == 0x00a0 ) )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) unicode_character;
			}
			else if( ( unicode_character >= 0x0e00 )
			      && ( unicode_character < 0x0e60 ) )
			{
				unicode_character -= 0x0e00;

				byte_stream[ *byte_stream_index ] = libuna_codepage_windows_874_unicode_to_byte_stream_base_0x0e00[ unicode_character ];
			}
			else if( ( unicode_character >= 0x2018 )
			      && ( unicode_character < 0x2020 ) )
			{
				unicode_character -= 0x2018;

				byte_stream[ *byte_stream_index ] = libuna_codepage_windows_874_unicode_to_byte_stream_base_0x2018[ unicode_character ];
			}
			else switch( unicode_character )
			{
				case 0x2013:
					byte_stream[ *byte_stream_index ] = 0x96;
					break;

				case 0x2014:
					byte_stream[ *byte_stream_index ] = 0x97;
					break;

				case 0x2022:
					byte_stream[ *byte_stream_index ] = 0x95;
					break;

				case 0x2026:
					byte_stream[ *byte_stream_index ] = 0x85;
					break;

				case 0x20ac:
					byte_stream[ *byte_stream_index ] = 0x80;
					break;

				default:
					byte_stream[ *byte_stream_index ] = 0x1a;
					break;
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_WINDOWS_932:
			result = libuna_codepage_windows_932_copy_to_byte_stream(
			          unicode_character,
			          byte_stream,
			          byte_stream_size,
			          byte_stream_index,
			          error );
			break;

		case LIBUNA_CODEPAGE_WINDOWS_936:
			result = libuna_codepage_windows_936_copy_to_byte_stream(
			          unicode_character,
			          byte_stream,
			          byte_stream_size,
			          byte_stream_index,
			          error );
			break;

		case LIBUNA_CODEPAGE_WINDOWS_949:
			result = libuna_codepage_windows_949_copy_to_byte_stream(
			          unicode_character,
			          byte_stream,
			          byte_stream_size,
			          byte_stream_index,
			          error );
			break;

		case LIBUNA_CODEPAGE_WINDOWS_950:
			result = libuna_codepage_windows_950_copy_to_byte_stream(
			          unicode_character,
			          byte_stream,
			          byte_stream_size,
			          byte_stream_index,
			          error );
			break;

		case LIBUNA_CODEPAGE_WINDOWS_1250:
			if( unicode_character < 0x0080 )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) unicode_character;
			}
			else if( ( unicode_character >= 0x00a0 )
			      && ( unicode_character < 0x0120 ) )
			{
				unicode_character -= 0x00a0;

				byte_stream[ *byte_stream_index ] = libuna_codepage_windows_1250_unicode_to_byte_stream_base_0x00a0[ unicode_character ];
			}
			else if( ( unicode_character >= 0x0138 )
			      && ( unicode_character < 0x0180 ) )
			{
				unicode_character -= 0x0138;

				byte_stream[ *byte_stream_index ] = libuna_codepage_windows_1250_unicode_to_byte_stream_base_0x0138[ unicode_character ];
			}
			else if( ( unicode_character >= 0x02d8 )
			      && ( unicode_character < 0x02e0 ) )
			{
				unicode_character -= 0x02d8;

				byte_stream[ *byte_stream_index ] = libuna_codepage_windows_1250_unicode_to_byte_stream_base_0x02d8[ unicode_character ];
			}
			else if( ( unicode_character >= 0x2010 )
			      && ( unicode_character < 0x2028 ) )
			{
				unicode_character -= 0x2010;

				byte_stream[ *byte_stream_index ] = libuna_codepage_windows_1250_unicode_to_byte_stream_base_0x2010[ unicode_character ];
			}
			else if( ( unicode_character >= 0x2030 )
			      && ( unicode_character < 0x2040 ) )
			{
				unicode_character -= 0x2030;

				byte_stream[ *byte_stream_index ] = libuna_codepage_windows_1250_unicode_to_byte_stream_base_0x2030[ unicode_character ];
			}
			else switch( unicode_character )
			{
				case 0x02c7:
					byte_stream[ *byte_stream_index ] = 0xa1;
					break;

				case 0x20ac:
					byte_stream[ *byte_stream_index ] = 0x80;
					break;

				case 0x2122:
					byte_stream[ *byte_stream_index ] = 0x99;
					break;

				default:
					byte_stream[ *byte_stream_index ] = 0x1a;
					break;
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_WINDOWS_1251:
			if( unicode_character < 0x0080 )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) unicode_character;
			}
			else if( ( unicode_character >= 0x00a0 )
			      && ( unicode_character < 0x00c0 ) )
			{
				unicode_character -= 0x00a0;

				byte_stream[ *byte_stream_index ] = libuna_codepage_windows_1251_unicode_to_byte_stream_base_0x00a0[ unicode_character ];
			}
			else if( ( unicode_character >= 0x0400 )
			      && ( unicode_character < 0x0460 ) )
			{
				unicode_character -= 0x0400;

				byte_stream[ *byte_stream_index ] = libuna_codepage_windows_1251_unicode_to_byte_stream_base_0x0400[ unicode_character ];
			}
			else if( ( unicode_character >= 0x2010 )
			      && ( unicode_character < 0x2028 ) )
			{
				unicode_character -= 0x2010;

				byte_stream[ *byte_stream_index ] = libuna_codepage_windows_1251_unicode_to_byte_stream_base_0x2010[ unicode_character ];
			}
			else switch( unicode_character )
			{
				case 0x0490:
					byte_stream[ *byte_stream_index ] = 0xa5;
					break;

				case 0x0491:
					byte_stream[ *byte_stream_index ] = 0xb4;
					break;

				case 0x2030:
					byte_stream[ *byte_stream_index ] = 0x89;
					break;

				case 0x2039:
					byte_stream[ *byte_stream_index ] = 0x8b;
					break;

				case 0x203a:
					byte_stream[ *byte_stream_index ] = 0x9b;
					break;

				case 0x20ac:
					byte_stream[ *byte_stream_index ] = 0x88;
					break;

				case 0x2116:
					byte_stream[ *byte_stream_index ] = 0xb9;
					break;

				case 0x2122:
					byte_stream[ *byte_stream_index ] = 0x99;
					break;

				default:
					byte_stream[ *byte_stream_index ] = 0x1a;
					break;
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_WINDOWS_1252:
			if( unicode_character < 0x0080 )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) unicode_character;
			}
			else if( ( unicode_character >= 0x00a0 )
			      && ( unicode_character < 0x0100 ) )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) unicode_character;
			}
			else if( ( unicode_character >= 0x2010 )
			      && ( unicode_character < 0x2028 ) )
			{
				unicode_character -= 0x2010;

				byte_stream[ *byte_stream_index ] = libuna_codepage_windows_1252_unicode_to_byte_stream_base_0x2010[ unicode_character ];
			}
			else switch( unicode_character )
			{
				case 0x0152:
					byte_stream[ *byte_stream_index ] = 0x8c;
					break;

				case 0x0153:
					byte_stream[ *byte_stream_index ] = 0x9c;
					break;

				case 0x0160:
					byte_stream[ *byte_stream_index ] = 0x8a;
					break;

				case 0x0161:
					byte_stream[ *byte_stream_index ] = 0x9a;
					break;

				case 0x0178:
					byte_stream[ *byte_stream_index ] = 0x9f;
					break;

				case 0x017d:
					byte_stream[ *byte_stream_index ] = 0x8e;
					break;

				case 0x017e:
					byte_stream[ *byte_stream_index ] = 0x9e;
					break;

				case 0x0192:
					byte_stream[ *byte_stream_index ] = 0x83;
					break;

				case 0x02c6:
					byte_stream[ *byte_stream_index ] = 0x88;
					break;

				case 0x02dc:
					byte_stream[ *byte_stream_index ] = 0x98;
					break;

				case 0x2030:
					byte_stream[ *byte_stream_index ] = 0x89;
					break;

				case 0x2039:
					byte_stream[ *byte_stream_index ] = 0x8b;
					break;

				case 0x203a:
					byte_stream[ *byte_stream_index ] = 0x9b;
					break;

				case 0x20ac:
					byte_stream[ *byte_stream_index ] = 0x80;
					break;

				case 0x2122:
					byte_stream[ *byte_stream_index ] = 0x99;
					break;

				default:
					byte_stream[ *byte_stream_index ] = 0x1a;
					break;
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_WINDOWS_1253:
			if( unicode_character < 0x0080 )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) unicode_character;
			}
			else if( ( unicode_character >= 0x00a0 )
			      && ( unicode_character < 0x00c0 ) )
			{
				unicode_character -= 0x00a0;

				byte_stream[ *byte_stream_index ] = libuna_codepage_windows_1253_unicode_to_byte_stream_base_0x00a0[ unicode_character ];
			}
			else if( ( unicode_character >= 0x0380 )
			      && ( unicode_character < 0x03d0 ) )
			{
				unicode_character -= 0x0380;

				byte_stream[ *byte_stream_index ] = libuna_codepage_windows_1253_unicode_to_byte_stream_base_0x0380[ unicode_character ];
			}
			else if( ( unicode_character >= 0x2010 )
			      && ( unicode_character < 0x2028 ) )
			{
				unicode_character -= 0x2010;

				byte_stream[ *byte_stream_index ] = libuna_codepage_windows_1253_unicode_to_byte_stream_base_0x2010[ unicode_character ];
			}
			else switch( unicode_character )
			{
				case 0x0192:
					byte_stream[ *byte_stream_index ] = 0x83;
					break;

				case 0x2030:
					byte_stream[ *byte_stream_index ] = 0x89;
					break;

				case 0x2039:
					byte_stream[ *byte_stream_index ] = 0x8b;
					break;

				case 0x203a:
					byte_stream[ *byte_stream_index ] = 0x9b;
					break;

				case 0x20ac:
					byte_stream[ *byte_stream_index ] = 0x80;
					break;

				case 0x2122:
					byte_stream[ *byte_stream_index ] = 0x99;
					break;

				default:
					byte_stream[ *byte_stream_index ] = 0x1a;
					break;
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_WINDOWS_1254:
			if( unicode_character < 0x0080 )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) unicode_character;
			}
			else if( ( unicode_character >= 0x00a0 )
			      && ( unicode_character < 0x00d0 ) )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) unicode_character;
			}
			else if( ( unicode_character >= 0x00d0 )
			      && ( unicode_character < 0x0100 ) )
			{
				unicode_character -= 0x00d0;

				byte_stream[ *byte_stream_index ] = libuna_codepage_windows_1254_unicode_to_byte_stream_base_0x00d0[ unicode_character ];
			}
			else if( ( unicode_character >= 0x2010 )
			      && ( unicode_character < 0x2028 ) )
			{
				unicode_character -= 0x2010;

				byte_stream[ *byte_stream_index ] = libuna_codepage_windows_1254_unicode_to_byte_stream_base_0x2010[ unicode_character ];
			}
			else switch( unicode_character )
			{
				case 0x011e:
					byte_stream[ *byte_stream_index ] = 0xd0;
					break;

				case 0x011f:
					byte_stream[ *byte_stream_index ] = 0xf0;
					break;

				case 0x0130:
					byte_stream[ *byte_stream_index ] = 0xdd;
					break;

				case 0x0131:
					byte_stream[ *byte_stream_index ] = 0xfd;
					break;

				case 0x0152:
					byte_stream[ *byte_stream_index ] = 0x8c;
					break;

				case 0x0153:
					byte_stream[ *byte_stream_index ] = 0x9c;
					break;

				case 0x015e:
					byte_stream[ *byte_stream_index ] = 0xde;
					break;

				case 0x015f:
					byte_stream[ *byte_stream_index ] = 0xfe;
					break;

				case 0x0160:
					byte_stream[ *byte_stream_index ] = 0x8a;
					break;

				case 0x0161:
					byte_stream[ *byte_stream_index ] = 0x9a;
					break;

				case 0x0178:
					byte_stream[ *byte_stream_index ] = 0x9f;
					break;

				case 0x0192:
					byte_stream[ *byte_stream_index ] = 0x83;
					break;

				case 0x02c6:
					byte_stream[ *byte_stream_index ] = 0x88;
					break;

				case 0x02dc:
					byte_stream[ *byte_stream_index ] = 0x98;
					break;

				case 0x2030:
					byte_stream[ *byte_stream_index ] = 0x89;
					break;

				case 0x2039:
					byte_stream[ *byte_stream_index ] = 0x8b;
					break;

				case 0x203a:
					byte_stream[ *byte_stream_index ] = 0x9b;
					break;

				case 0x20ac:
					byte_stream[ *byte_stream_index ] = 0x80;
					break;

				case 0x2122:
					byte_stream[ *byte_stream_index ] = 0x99;
					break;

				default:
					byte_stream[ *byte_stream_index ] = 0x1a;
					break;
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_WINDOWS_1255:
			if( unicode_character < 0x0080 )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) unicode_character;
			}
			else if( ( unicode_character >= 0x00a0 )
			      && ( unicode_character < 0x00c0 ) )
			{
				unicode_character -= 0x00a0;

				byte_stream[ *byte_stream_index ] = libuna_codepage_windows_1255_unicode_to_byte_stream_base_0x00a0[ unicode_character ];
			}
			else if( ( unicode_character >= 0x05b0 )
			      && ( unicode_character < 0x05c8 ) )
			{
				unicode_character -= 0x05b0;

				byte_stream[ *byte_stream_index ] = libuna_codepage_windows_1255_unicode_to_byte_stream_base_0x05b0[ unicode_character ];
			}
			else if( ( unicode_character >= 0x05d0 )
			      && ( unicode_character < 0x05f8 ) )
			{
				unicode_character -= 0x05d0;

				byte_stream[ *byte_stream_index ] = libuna_codepage_windows_1255_unicode_to_byte_stream_base_0x05d0[ unicode_character ];
			}
			else if( ( unicode_character >= 0x2010 )
			      && ( unicode_character < 0x2028 ) )
			{
				unicode_character -= 0x2010;

				byte_stream[ *byte_stream_index ] = libuna_codepage_windows_1255_unicode_to_byte_stream_base_0x2010[ unicode_character ];
			}
			else switch( unicode_character )
			{
				case 0x00d7:
					byte_stream[ *byte_stream_index ] = 0xaa;
					break;

				case 0x00f7:
					byte_stream[ *byte_stream_index ] = 0xba;
					break;

				case 0x0192:
					byte_stream[ *byte_stream_index ] = 0x83;
					break;

				case 0x02c6:
					byte_stream[ *byte_stream_index ] = 0x88;
					break;

				case 0x02dc:
					byte_stream[ *byte_stream_index ] = 0x98;
					break;

				case 0x200e:
					byte_stream[ *byte_stream_index ] = 0xfd;
					break;

				case 0x200f:
					byte_stream[ *byte_stream_index ] = 0xfe;
					break;

				case 0x2030:
					byte_stream[ *byte_stream_index ] = 0x89;
					break;

				case 0x2039:
					byte_stream[ *byte_stream_index ] = 0x8b;
					break;

				case 0x203a:
					byte_stream[ *byte_stream_index ] = 0x9b;
					break;

				case 0x20aa:
					byte_stream[ *byte_stream_index ] = 0xa4;
					break;

				case 0x20ac:
					byte_stream[ *byte_stream_index ] = 0x80;
					break;

				case 0x2122:
					byte_stream[ *byte_stream_index ] = 0x99;
					break;

				default:
					byte_stream[ *byte_stream_index ] = 0x1a;
					break;
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_WINDOWS_1256:
			if( unicode_character < 0x0080 )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) unicode_character;
			}
			else if( ( unicode_character >= 0x00a0 )
			      && ( unicode_character < 0x00c0 ) )
			{
				unicode_character -= 0x00a0;

				byte_stream[ *byte_stream_index ] = libuna_codepage_windows_1256_unicode_to_byte_stream_base_0x00a0[ unicode_character ];
			}
			else if( ( unicode_character >= 0x00e0 )
			      && ( unicode_character < 0x0100 ) )
			{
				unicode_character -= 0x00e0;

				byte_stream[ *byte_stream_index ] = libuna_codepage_windows_1256_unicode_to_byte_stream_base_0x00e0[ unicode_character ];
			}
			else if( ( unicode_character >= 0x0618 )
			      && ( unicode_character < 0x0658 ) )
			{
				unicode_character -= 0x0618;

				byte_stream[ *byte_stream_index ] = libuna_codepage_windows_1256_unicode_to_byte_stream_base_0x0618[ unicode_character ];
			}
			else if( ( unicode_character >= 0x2008 )
			      && ( unicode_character < 0x2028 ) )
			{
				unicode_character -= 0x2008;

				byte_stream[ *byte_stream_index ] = libuna_codepage_windows_1256_unicode_to_byte_stream_base_0x2008[ unicode_character ];
			}
			else switch( unicode_character )
			{
				case 0x00d7:
					byte_stream[ *byte_stream_index ] = 0xd7;
					break;

				case 0x0152:
					byte_stream[ *byte_stream_index ] = 0x8c;
					break;

				case 0x0153:
					byte_stream[ *byte_stream_index ] = 0x9c;
					break;

				case 0x0192:
					byte_stream[ *byte_stream_index ] = 0x83;
					break;

				case 0x02c6:
					byte_stream[ *byte_stream_index ] = 0x88;
					break;

				case 0x060c:
					byte_stream[ *byte_stream_index ] = 0xa1;
					break;

				case 0x0679:
					byte_stream[ *byte_stream_index ] = 0x8a;
					break;

				case 0x067e:
					byte_stream[ *byte_stream_index ] = 0x81;
					break;

				case 0x0686:
					byte_stream[ *byte_stream_index ] = 0x8d;
					break;

				case 0x0688:
					byte_stream[ *byte_stream_index ] = 0x8f;
					break;

				case 0x0691:
					byte_stream[ *byte_stream_index ] = 0x9a;
					break;

				case 0x0698:
					byte_stream[ *byte_stream_index ] = 0x8e;
					break;

				case 0x06a9:
					byte_stream[ *byte_stream_index ] = 0x98;
					break;

				case 0x06af:
					byte_stream[ *byte_stream_index ] = 0x90;
					break;

				case 0x06ba:
					byte_stream[ *byte_stream_index ] = 0x9f;
					break;

				case 0x06be:
					byte_stream[ *byte_stream_index ] = 0xaa;
					break;

				case 0x06c1:
					byte_stream[ *byte_stream_index ] = 0xc0;
					break;

				case 0x06d2:
					byte_stream[ *byte_stream_index ] = 0xff;
					break;

				case 0x2030:
					byte_stream[ *byte_stream_index ] = 0x89;
					break;

				case 0x2039:
					byte_stream[ *byte_stream_index ] = 0x8b;
					break;

				case 0x203a:
					byte_stream[ *byte_stream_index ] = 0x9b;
					break;

				case 0x20ac:
					byte_stream[ *byte_stream_index ] = 0x80;
					break;

				case 0x2122:
					byte_stream[ *byte_stream_index ] = 0x99;
					break;

				default:
					byte_stream[ *byte_stream_index ] = 0x1a;
					break;
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_WINDOWS_1257:
			if( unicode_character < 0x0080 )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) unicode_character;
			}
			else if( ( unicode_character >= 0x00a0 )
			      && ( unicode_character < 0x0180 ) )
			{
				unicode_character -= 0x00a0;

				byte_stream[ *byte_stream_index ] = libuna_codepage_windows_1257_unicode_to_byte_stream_base_0x00a0[ unicode_character ];
			}
			else if( ( unicode_character >= 0x2010 )
			      && ( unicode_character < 0x2028 ) )
			{
				unicode_character -= 0x2010;

				byte_stream[ *byte_stream_index ] = libuna_codepage_windows_1257_unicode_to_byte_stream_base_0x2010[ unicode_character ];
			}
			else switch( unicode_character )
			{
				case 0x02c7:
					byte_stream[ *byte_stream_index ] = 0x8e;
					break;

				case 0x02d9:
					byte_stream[ *byte_stream_index ] = 0xff;
					break;

				case 0x02db:
					byte_stream[ *byte_stream_index ] = 0x9e;
					break;

				case 0x2030:
					byte_stream[ *byte_stream_index ] = 0x89;
					break;

				case 0x2039:
					byte_stream[ *byte_stream_index ] = 0x8b;
					break;

				case 0x203a:
					byte_stream[ *byte_stream_index ] = 0x9b;
					break;

				case 0x20ac:
					byte_stream[ *byte_stream_index ] = 0x80;
					break;

				case 0x2122:
					byte_stream[ *byte_stream_index ] = 0x99;
					break;

				default:
					byte_stream[ *byte_stream_index ] = 0x1a;
					break;
			}
			*byte_stream_index += 1;

			break;

		case LIBUNA_CODEPAGE_WINDOWS_1258:
			if( unicode_character < 0x0080 )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) unicode_character;
			}
			else if( ( unicode_character >= 0x00a0 )
			      && ( unicode_character < 0x00c0 ) )
			{
				byte_stream[ *byte_stream_index ] = (uint8_t) unicode_character;
			}
			else if( ( unicode_character >= 0x00c0 )
			      && ( unicode_character < 0x0108 ) )
			{
				unicode_character -= 0x00c0;

				byte_stream[ *byte_stream_index ] = libuna_codepage_windows_1258_unicode_to_byte_stream_base_0x00c0[ unicode_character ];
			}
			else if( ( unicode_character >= 0x2010 )
			      && ( unicode_character < 0x2028 ) )
			{
				unicode_character -= 0x2010;

				byte_stream[ *byte_stream_index ] = libuna_codepage_windows_1258_unicode_to_byte_stream_base_0x2010[ unicode_character ];
			}
			else switch( unicode_character )
			{
				case 0x0110:
					byte_stream[ *byte_stream_index ] = 0xd0;
					break;

				case 0x0111:
					byte_stream[ *byte_stream_index ] = 0xf0;
					break;

				case 0x0152:
					byte_stream[ *byte_stream_index ] = 0x8c;
					break;

				case 0x0153:
					byte_stream[ *byte_stream_index ] = 0x9c;
					break;

				case 0x0178:
					byte_stream[ *byte_stream_index ] = 0x9f;
					break;

				case 0x0192:
					byte_stream[ *byte_stream_index ] = 0x83;
					break;

				case 0x01a0:
					byte_stream[ *byte_stream_index ] = 0xd5;
					break;

				case 0x01a1:
					byte_stream[ *byte_stream_index ] = 0xf5;
					break;

				case 0x01af:
					byte_stream[ *byte_stream_index ] = 0xdd;
					break;

				case 0x01b0:
					byte_stream[ *byte_stream_index ] = 0xfd;
					break;

				case 0x02c6:
					byte_stream[ *byte_stream_index ] = 0x88;
					break;

				case 0x02dc:
					byte_stream[ *byte_stream_index ] = 0x98;
					break;

				case 0x0300:
					byte_stream[ *byte_stream_index ] = 0xcc;
					break;

				case 0x0301:
					byte_stream[ *byte_stream_index ] = 0xec;
					break;

				case 0x0303:
					byte_stream[ *byte_stream_index ] = 0xde;
					break;

				case 0x0309:
					byte_stream[ *byte_stream_index ] = 0xd2;
					break;

				case 0x0323:
					byte_stream[ *byte_stream_index ] = 0xf2;
					break;

				case 0x2030:
					byte_stream[ *byte_stream_index ] = 0x89;
					break;

				case 0x2039:
					byte_stream[ *byte_stream_index ] = 0x8b;
					break;

				case 0x203a:
					byte_stream[ *byte_stream_index ] = 0x9b;
					break;

				case 0x20ab:
					byte_stream[ *byte_stream_index ] = 0xfe;
					break;

				case 0x20ac:
					byte_stream[ *byte_stream_index ] = 0x80;
					break;

				case 0x2122:
					byte_stream[ *byte_stream_index ] = 0x99;
					break;

				default:
					byte_stream[ *byte_stream_index ] = 0x1a;
					break;
			}
			*byte_stream_index += 1;

			break;

		default:
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			"%s: unsupported codepage: %d.",
			 function,
		         codepage );

			return( -1 );
	}
	if( result != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
		 "%s: unable to copy Unicode character to byte stream.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Determines the size of an UTF-7 stream character from an Unicode character
 * Adds the size to the UTF-7 stream character size value
 * Returns 1 if successful or -1 on error
 */
LIBUNA_INLINE \
int libuna_unicode_character_size_to_utf7_stream(
     libuna_unicode_character_t unicode_character,
     size_t *utf7_stream_character_size,
     uint32_t *utf7_stream_base64_data,
     libcerror_error_t **error )
{
	libuna_utf16_character_t utf16_surrogate = 0;
	static char *function                    = "libuna_unicode_character_size_to_utf7_stream";
	uint8_t base64_encode_character          = 0;
	uint32_t base64_triplet                  = 0;
	uint8_t number_of_bytes                  = 0;
	uint8_t current_byte                     = 0;
	uint8_t byte_bit_shift                   = 0;

	if( utf7_stream_character_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-7 stream character size.",
		 function );

		return( -1 );
	}
	if( utf7_stream_base64_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-7 stream base64 data.",
		 function );

		return( -1 );
	}
	/* Determine if the Unicode character is valid
	 */
	if( unicode_character > LIBUNA_UNICODE_CHARACTER_MAX )
	{
		unicode_character = LIBUNA_UNICODE_REPLACEMENT_CHARACTER;
	}
	/* A-Z is not a continous range on an EBCDIC based system
	 * it consists of the ranges: A-I, J-R, S-Z
	 */
	if( ( unicode_character >= 0x41 )
	 && ( unicode_character <= 0x49 ) )
	{
	}
	else if( ( unicode_character >= 0x4a )
	      && ( unicode_character <= 0x52 ) )
	{
	}
	else if( ( unicode_character >= 0x53 )
	      && ( unicode_character <= 0x5a ) )
	{
	}
	/* a-z is not a continous range on an EBCDIC based system
	 * it consists of the ranges: a-i, j-r, s-z
	 */
	else if( ( unicode_character >= 0x61 )
	      && ( unicode_character <= 0x69 ) )
	{
	}
	else if( ( unicode_character >= 0x6a )
	      && ( unicode_character <= 0x72 ) )
	{
	}
	else if( ( unicode_character >= 0x73 )
	      && ( unicode_character <= 0x7a ) )
	{
	}
	/* 0-9
	 */
	else if( ( unicode_character >= 0x30 )
	      && ( unicode_character <= 0x39 ) )
	{
	}
	/* Valid directly encoded whitespace
	 */
	else if( ( unicode_character == (libuna_unicode_character_t) '\t' )
	      || ( unicode_character == (libuna_unicode_character_t) '\n' )
	      || ( unicode_character == (libuna_unicode_character_t) '\r' )
	      || ( unicode_character == (libuna_unicode_character_t) ' ' ) )
	{
	}
	/* The + character must be escaped
	 */
	else if( unicode_character == (libuna_unicode_character_t) '+' )
	{
	}
	/* Valid directly encoded characters
	 */
	else if( ( unicode_character == (libuna_unicode_character_t) '\'' )
	      || ( unicode_character == (libuna_unicode_character_t) '(' )
	      || ( unicode_character == (libuna_unicode_character_t) ')' )
	      || ( unicode_character == (libuna_unicode_character_t) ',' )
	      || ( unicode_character == (libuna_unicode_character_t) '-' )
	      || ( unicode_character == (libuna_unicode_character_t) '.' )
	      || ( unicode_character == (libuna_unicode_character_t) '/' )
	      || ( unicode_character == (libuna_unicode_character_t) ':' )
	      || ( unicode_character == (libuna_unicode_character_t) '?' ) )
	{
	}
	/* Valid optional directly encoded characters
	 */
	else if( ( unicode_character == (libuna_unicode_character_t) '!' )
	      || ( unicode_character == (libuna_unicode_character_t) '"' )
	      || ( unicode_character == (libuna_unicode_character_t) '#' )
	      || ( unicode_character == (libuna_unicode_character_t) '$' )
	      || ( unicode_character == (libuna_unicode_character_t) '%' )
	      || ( unicode_character == (libuna_unicode_character_t) '&' )
	      || ( unicode_character == (libuna_unicode_character_t) '*' )
	      || ( unicode_character == (libuna_unicode_character_t) ';' )
	      || ( unicode_character == (libuna_unicode_character_t) '<' )
	      || ( unicode_character == (libuna_unicode_character_t) '=' )
	      || ( unicode_character == (libuna_unicode_character_t) '>' )
	      || ( unicode_character == (libuna_unicode_character_t) '@' )
	      || ( unicode_character == (libuna_unicode_character_t) '[' )
	      || ( unicode_character == (libuna_unicode_character_t) ']' )
	      || ( unicode_character == (libuna_unicode_character_t) '^' )
	      || ( unicode_character == (libuna_unicode_character_t) '_' )
	      || ( unicode_character == (libuna_unicode_character_t) '`' )
	      || ( unicode_character == (libuna_unicode_character_t) '{' )
	      || ( unicode_character == (libuna_unicode_character_t) '|' )
	      || ( unicode_character == (libuna_unicode_character_t) '}' ) )
	{
	}
	/* Allow for the end of string character
	 */
	else if( unicode_character == 0 )
	{
	}
	else
	{
		base64_encode_character = 1;
	}
	if( base64_encode_character == 0 )
	{
		if( ( *utf7_stream_base64_data & LIBUNA_UTF7_IS_BASE64_ENCODED ) != 0 )
		{
			*utf7_stream_base64_data = 0;
		}
		*utf7_stream_character_size += 1;

		/* The + character must be escaped
		 */
		if( unicode_character == (libuna_unicode_character_t) '+' )
		{
			*utf7_stream_character_size += 1;
		}
	}
	else
	{
		/* Escape the base64 encoded characters with a +
		 */
		if( ( *utf7_stream_base64_data & LIBUNA_UTF7_IS_BASE64_ENCODED ) == 0 )
		{
			*utf7_stream_character_size += 1;
		}
		/* Otherwise continue the previously base64 encoded characters
		 */
		else
		{
			base64_triplet  = *utf7_stream_base64_data & 0x00ffffff;
			number_of_bytes = ( *utf7_stream_base64_data >> 24 ) & 0x03;
			current_byte    = ( *utf7_stream_base64_data >> 28 ) & 0x03;

			if( number_of_bytes > 0 )
			{
				if( *utf7_stream_character_size < (size_t) ( number_of_bytes + 1 ) )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
					 "%s: invalid UTF-7 stream character size value out of bounds.",
					 function );

					return( -1 );
				}
				/* Correct the size for the last partial base64 stream
				 */
				*utf7_stream_character_size -= number_of_bytes + 1;
			}
			if( *utf7_stream_character_size < 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: invalid UTF-7 stream character size value out of bounds.",
				 function );

				return( -1 );
			}
			/* Correct the size for the base64 stream termination character
			 */
			*utf7_stream_character_size -= 1;
		}
		*utf7_stream_base64_data = LIBUNA_UTF7_IS_BASE64_ENCODED;

		if( unicode_character > LIBUNA_UNICODE_BASIC_MULTILINGUAL_PLANE_MAX )
		{
			unicode_character -= 0x010000;

			utf16_surrogate = (libuna_utf16_character_t) ( ( unicode_character >> 10 ) + LIBUNA_UNICODE_SURROGATE_HIGH_RANGE_START );

			byte_bit_shift   = 16 - ( current_byte * 8 );
			base64_triplet  += (uint32_t) ( ( utf16_surrogate >> 8 ) & 0xff ) << byte_bit_shift;
			current_byte    += 1;
			number_of_bytes += 1;

			if( number_of_bytes == 3 )
			{
				*utf7_stream_character_size += 4;
				number_of_bytes              = 0;
				current_byte                 = 0;
				base64_triplet               = 0;
			}
			byte_bit_shift   = 16 - ( current_byte * 8 );
			base64_triplet  += (uint32_t) ( utf16_surrogate & 0xff ) << byte_bit_shift;
			current_byte    += 1;
			number_of_bytes += 1;

			if( number_of_bytes == 3 )
			{
				*utf7_stream_character_size += 4;
				number_of_bytes              = 0;
				current_byte                 = 0;
				base64_triplet               = 0;
			}
			unicode_character = (libuna_utf16_character_t) ( ( unicode_character & 0x03ff ) + LIBUNA_UNICODE_SURROGATE_LOW_RANGE_START );
		}
		byte_bit_shift   = 16 - ( current_byte * 8 );
		base64_triplet  += (uint32_t) ( ( unicode_character >> 8 ) & 0xff ) << byte_bit_shift;
		current_byte    += 1;
		number_of_bytes += 1;

		if( number_of_bytes == 3 )
		{
			*utf7_stream_character_size += 4;
			number_of_bytes              = 0;
			current_byte                 = 0;
			base64_triplet               = 0;
		}
		byte_bit_shift   = 16 - ( current_byte * 8 );
		base64_triplet  += (uint32_t) ( unicode_character & 0xff ) << byte_bit_shift;
		current_byte    += 1;
		number_of_bytes += 1;

		if( number_of_bytes == 3 )
		{
			*utf7_stream_character_size += 4;
			number_of_bytes              = 0;
			current_byte                 = 0;
			base64_triplet               = 0;
		}
		/* Terminate the base64 encoded characters
		 */
		if( number_of_bytes > 0 )
		{
			*utf7_stream_character_size += number_of_bytes + 1;
		}
		*utf7_stream_character_size += 1;
	}
	if( ( *utf7_stream_base64_data & LIBUNA_UTF7_IS_BASE64_ENCODED ) != 0 )
	{
		*utf7_stream_base64_data  = LIBUNA_UTF7_IS_BASE64_ENCODED;
		*utf7_stream_base64_data |= (uint32_t) current_byte << 28;
		*utf7_stream_base64_data |= (uint32_t) number_of_bytes << 24;
		*utf7_stream_base64_data |= base64_triplet & 0x00ffffff;
	}
	return( 1 );
}

/* Copies a Unicode character from an UTF-7 stream
 * The bits of the base64 data contain:
 *   0 - 23 the base64 triplet
 *  24 - 25 the number of bytes in the triplet
 *  26 - 27 unused
 *  28 - 29 the current byte
 *       30 unused
 *       31 flag to indicate the current UTF-7 characters are (modified) base64 encoded
 *
 * Returns 1 if successful or -1 on error
 */
LIBUNA_INLINE \
int libuna_unicode_character_copy_from_utf7_stream(
     libuna_unicode_character_t *unicode_character,
     const uint8_t *utf7_stream,
     size_t utf7_stream_size,
     size_t *utf7_stream_index,
     uint32_t *utf7_stream_base64_data,
     libcerror_error_t **error )
{
	static char *function                    = "libuna_unicode_character_copy_from_utf7_stream";
	libuna_utf16_character_t utf16_surrogate = 0;
	uint32_t base64_triplet                  = 0;
	uint8_t number_of_bytes                  = 0;
	uint8_t current_byte                     = 0;
	uint8_t padding_size                     = 0;
	uint8_t byte_bit_shift                   = 0;

	if( unicode_character == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid Unicode character.",
		 function );

		return( -1 );
	}
	if( utf7_stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-7 stream.",
		 function );

		return( -1 );
	}
	if( utf7_stream_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid UTF-7 stream size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( utf7_stream_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-7 stream index.",
		 function );

		return( -1 );
	}
	if( *utf7_stream_index >= utf7_stream_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-7 stream too small.",
		 function );

		return( -1 );
	}
	if( utf7_stream_base64_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-7 base64 data.",
		 function );

		return( -1 );
	}
	if( ( *utf7_stream_base64_data & LIBUNA_UTF7_IS_BASE64_ENCODED ) != 0 )
	{
		base64_triplet  = *utf7_stream_base64_data & 0x00ffffff;
		number_of_bytes = ( *utf7_stream_base64_data >> 24 ) & 0x03;
		current_byte    = ( *utf7_stream_base64_data >> 28 ) & 0x03;

		if( current_byte >= number_of_bytes )
		{
			/* A-Z is not a continous range on an EBCDIC based system
			 * it consists of the ranges: A-I, J-R, S-Z
			 */
			if( ( utf7_stream[ *utf7_stream_index ] >= (uint8_t) 'A' )
			 && ( utf7_stream[ *utf7_stream_index ] <= (uint8_t) 'I' ) )
			{
			}
			else if( ( utf7_stream[ *utf7_stream_index ] >= (uint8_t) 'J' )
			      && ( utf7_stream[ *utf7_stream_index ] <= (uint8_t) 'R' ) )
			{
			}
			else if( ( utf7_stream[ *utf7_stream_index ] >= (uint8_t) 'S' )
			      && ( utf7_stream[ *utf7_stream_index ] <= (uint8_t) 'Z' ) )
			{
			}
			/* a-z is not a continous range on an EBCDIC based system
			 * it consists of the ranges: a-i, j-r, s-z
			 */
			else if( ( utf7_stream[ *utf7_stream_index ] >= (uint8_t) 'a' )
			      && ( utf7_stream[ *utf7_stream_index ] <= (uint8_t) 'i' ) )
			{
			}
			else if( ( utf7_stream[ *utf7_stream_index ] >= (uint8_t) 'j' )
			      && ( utf7_stream[ *utf7_stream_index ] <= (uint8_t) 'r' ) )
			{
			}
			else if( ( utf7_stream[ *utf7_stream_index ] >= (uint8_t) 's' )
			      && ( utf7_stream[ *utf7_stream_index ] <= (uint8_t) 'z' ) )
			{
			}
			/* 0-9
			 */
			else if( ( utf7_stream[ *utf7_stream_index ] >= (uint8_t) '0' )
			      && ( utf7_stream[ *utf7_stream_index ] <= (uint8_t) '9' ) )
			{
			}
			/* + or /
			 */
			else if( ( utf7_stream[ *utf7_stream_index ] == (uint8_t) '+' )
			      || ( utf7_stream[ *utf7_stream_index ] == (uint8_t) '/' ) )
			{
			}
			/* Any character not in the modified base64 alphabet
			 * terminates the base64 encoded sequence
			 */
			else
			{
				*utf7_stream_base64_data = 0;
			}
		}
	}
	if( ( *utf7_stream_base64_data & LIBUNA_UTF7_IS_BASE64_ENCODED ) == 0 )
	{
		/* Determine if the character is modified base64 encoded
		 * or a + character
		 */
		if( utf7_stream[ *utf7_stream_index ] == (uint8_t) '+' )
		{
			if( utf7_stream[ *utf7_stream_index + 1 ] != (uint8_t) '-' )
			{
				*utf7_stream_base64_data = LIBUNA_UTF7_IS_BASE64_ENCODED;

				*utf7_stream_index += 1;
			}
		}
		/* A-Z is not a continous range on an EBCDIC based system
		 * it consists of the ranges: A-I, J-R, S-Z
		 */
		else if( ( utf7_stream[ *utf7_stream_index ] >= (uint8_t) 'A' )
		      && ( utf7_stream[ *utf7_stream_index ] <= (uint8_t) 'I' ) )
		{
		}
		else if( ( utf7_stream[ *utf7_stream_index ] >= (uint8_t) 'J' )
		      && ( utf7_stream[ *utf7_stream_index ] <= (uint8_t) 'R' ) )
		{
		}
		else if( ( utf7_stream[ *utf7_stream_index ] >= (uint8_t) 'S' )
		      && ( utf7_stream[ *utf7_stream_index ] <= (uint8_t) 'Z' ) )
		{
		}
		/* a-z is not a continous range on an EBCDIC based system
		 * it consists of the ranges: a-i, j-r, s-z
		 */
		else if( ( utf7_stream[ *utf7_stream_index ] >= (uint8_t) 'a' )
		      && ( utf7_stream[ *utf7_stream_index ] <= (uint8_t) 'i' ) )
		{
		}
		else if( ( utf7_stream[ *utf7_stream_index ] >= (uint8_t) 'j' )
		      && ( utf7_stream[ *utf7_stream_index ] <= (uint8_t) 'r' ) )
		{
		}
		else if( ( utf7_stream[ *utf7_stream_index ] >= (uint8_t) 's' )
		      && ( utf7_stream[ *utf7_stream_index ] <= (uint8_t) 'z' ) )
		{
		}
		/* 0-9
		 */
		else if( ( utf7_stream[ *utf7_stream_index ] >= (uint8_t) '0' )
		      && ( utf7_stream[ *utf7_stream_index ] <= (uint8_t) '9' ) )
		{
		}
		/* Valid directly encoded whitespace
		 */
		else if( ( utf7_stream[ *utf7_stream_index ] == (uint8_t) '\t' )
		      || ( utf7_stream[ *utf7_stream_index ] == (uint8_t) '\n' )
		      || ( utf7_stream[ *utf7_stream_index ] == (uint8_t) '\r' )
		      || ( utf7_stream[ *utf7_stream_index ] == (uint8_t) ' ' ) )
		{
		}
		/* Valid directly encoded characters
		 */
		else if( ( utf7_stream[ *utf7_stream_index ] == (uint8_t) '\'' )
		      || ( utf7_stream[ *utf7_stream_index ] == (uint8_t) '(' )
		      || ( utf7_stream[ *utf7_stream_index ] == (uint8_t) ')' )
		      || ( utf7_stream[ *utf7_stream_index ] == (uint8_t) ',' )
		      || ( utf7_stream[ *utf7_stream_index ] == (uint8_t) '-' )
		      || ( utf7_stream[ *utf7_stream_index ] == (uint8_t) '.' )
		      || ( utf7_stream[ *utf7_stream_index ] == (uint8_t) '/' )
		      || ( utf7_stream[ *utf7_stream_index ] == (uint8_t) ':' )
		      || ( utf7_stream[ *utf7_stream_index ] == (uint8_t) '?' ) )
		{
		}
		/* Valid optional directly encoded characters
		 */
		else if( ( utf7_stream[ *utf7_stream_index ] == (uint8_t) '!' )
		      || ( utf7_stream[ *utf7_stream_index ] == (uint8_t) '"' )
		      || ( utf7_stream[ *utf7_stream_index ] == (uint8_t) '#' )
		      || ( utf7_stream[ *utf7_stream_index ] == (uint8_t) '$' )
		      || ( utf7_stream[ *utf7_stream_index ] == (uint8_t) '%' )
		      || ( utf7_stream[ *utf7_stream_index ] == (uint8_t) '&' )
		      || ( utf7_stream[ *utf7_stream_index ] == (uint8_t) '*' )
		      || ( utf7_stream[ *utf7_stream_index ] == (uint8_t) ';' )
		      || ( utf7_stream[ *utf7_stream_index ] == (uint8_t) '<' )
		      || ( utf7_stream[ *utf7_stream_index ] == (uint8_t) '=' )
		      || ( utf7_stream[ *utf7_stream_index ] == (uint8_t) '>' )
		      || ( utf7_stream[ *utf7_stream_index ] == (uint8_t) '@' )
		      || ( utf7_stream[ *utf7_stream_index ] == (uint8_t) '[' )
		      || ( utf7_stream[ *utf7_stream_index ] == (uint8_t) ']' )
		      || ( utf7_stream[ *utf7_stream_index ] == (uint8_t) '^' )
		      || ( utf7_stream[ *utf7_stream_index ] == (uint8_t) '_' )
		      || ( utf7_stream[ *utf7_stream_index ] == (uint8_t) '`' )
		      || ( utf7_stream[ *utf7_stream_index ] == (uint8_t) '{' )
		      || ( utf7_stream[ *utf7_stream_index ] == (uint8_t) '|' )
		      || ( utf7_stream[ *utf7_stream_index ] == (uint8_t) '}' ) )
		{
		}
		/* Allow for the end of string character
		 */
		else if( utf7_stream[ *utf7_stream_index ] == 0 )
		{
		}
		else
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: invalid directly encoded UTF-7 character byte: 0x%02" PRIx8 ".",
			 function,
			 utf7_stream[ *utf7_stream_index ] );

			return( -1 );
		}
	}
	if( ( *utf7_stream_base64_data & LIBUNA_UTF7_IS_BASE64_ENCODED ) == 0 )
	{
		*unicode_character  = utf7_stream[ *utf7_stream_index ];
		*utf7_stream_index += 1;

		if( ( *unicode_character == (libuna_unicode_character_t) '+' )
		 && ( utf7_stream[ *utf7_stream_index ] == (uint8_t) '-' ) )
		{
			*utf7_stream_index += 1;
		}
	}
	else if( ( number_of_bytes == 0 )
	      || ( current_byte >= number_of_bytes ) )
	{
		if( libuna_base64_triplet_copy_from_base64_stream(
		     &base64_triplet,
		     utf7_stream,
		     utf7_stream_size,
		     utf7_stream_index,
		     &padding_size,
		     LIBUNA_BASE64_VARIANT_UTF7,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_CONVERSION,
			 LIBCERROR_CONVERSION_ERROR_OUTPUT_FAILED,
			 "%s: unable to copy base64 encoded UTF-7 characters.",
			 function );

			return( -1 );
		}
		if( padding_size > 2 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: unsupported padding in base64 encoded UTF-7 characters.",
			 function );

			return( -1 );
		}
		number_of_bytes = 3 - padding_size;
		current_byte    = 0;
	}
	if( ( *utf7_stream_base64_data & LIBUNA_UTF7_IS_BASE64_ENCODED ) != 0 )
	{
		byte_bit_shift     = 16 - ( current_byte * 8 );
		*unicode_character = ( ( base64_triplet >> byte_bit_shift ) & 0x000000ffUL ) << 8;
		current_byte      += 1;

		if( current_byte >= number_of_bytes )
		{
			if( libuna_base64_triplet_copy_from_base64_stream(
			     &base64_triplet,
			     utf7_stream,
			     utf7_stream_size,
			     utf7_stream_index,
			     &padding_size,
			     LIBUNA_BASE64_VARIANT_UTF7,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_CONVERSION,
				 LIBCERROR_CONVERSION_ERROR_OUTPUT_FAILED,
				 "%s: unable to copy base64 encoded UTF-7 characters.",
				 function );

				return( -1 );
			}
			if( padding_size > 2 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
				 "%s: unsupported padding in base64 encoded UTF-7 characters.",
				 function );

				return( -1 );
			}
			number_of_bytes = 3 - padding_size;
			current_byte    = 0;
		}
		byte_bit_shift      = 16 - ( current_byte * 8 );
		*unicode_character += ( base64_triplet >> byte_bit_shift ) & 0x000000ffUL;
		current_byte       += 1;

		if( ( *unicode_character >= LIBUNA_UNICODE_SURROGATE_HIGH_RANGE_START )
		 && ( *unicode_character <= LIBUNA_UNICODE_SURROGATE_HIGH_RANGE_END ) )
		{
			if( current_byte >= number_of_bytes )
			{
				if( libuna_base64_triplet_copy_from_base64_stream(
				     &base64_triplet,
				     utf7_stream,
				     utf7_stream_size,
				     utf7_stream_index,
				     &padding_size,
				     LIBUNA_BASE64_VARIANT_UTF7,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_CONVERSION,
					 LIBCERROR_CONVERSION_ERROR_OUTPUT_FAILED,
					 "%s: unable to copy base64 encoded UTF-7 characters.",
					 function );

					return( -1 );
				}
				if( padding_size > 2 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
					 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
					 "%s: unsupported padding in base64 encoded UTF-7 characters.",
					 function );

					return( -1 );
				}
				number_of_bytes = 3 - padding_size;
				current_byte    = 0;
			}
			byte_bit_shift  = 16 - ( current_byte * 8 );
			utf16_surrogate = ( ( base64_triplet >> byte_bit_shift ) & 0x000000ffUL ) << 8;
			current_byte   += 1;

			if( current_byte >= number_of_bytes )
			{
				if( libuna_base64_triplet_copy_from_base64_stream(
				     &base64_triplet,
				     utf7_stream,
				     utf7_stream_size,
				     utf7_stream_index,
				     &padding_size,
				     LIBUNA_BASE64_VARIANT_UTF7,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve base64 encoded UTF-7 characters.",
					 function );

					return( -1 );
				}
				if( padding_size > 2 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
					 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
					 "%s: unsupported padding in base64 encoded UTF-7 characters.",
					 function );

					return( -1 );
				}
				number_of_bytes = 3 - padding_size;
				current_byte    = 0;
			}
			byte_bit_shift   = 16 - ( current_byte * 8 );
			utf16_surrogate += ( base64_triplet >> byte_bit_shift ) & 0x000000ffUL;
			current_byte    += 1;

			/* Determine if the UTF-16 character is within the low surrogate range
			 */
			if( ( utf16_surrogate >= LIBUNA_UNICODE_SURROGATE_LOW_RANGE_START )
			 && ( utf16_surrogate <= LIBUNA_UNICODE_SURROGATE_LOW_RANGE_END ) )
			{
				*unicode_character  -= LIBUNA_UNICODE_SURROGATE_HIGH_RANGE_START;
				*unicode_character <<= 10;
				*unicode_character  += utf16_surrogate - LIBUNA_UNICODE_SURROGATE_LOW_RANGE_END;
				*unicode_character  += 0x010000;
			}
			else
			{
				*unicode_character = LIBUNA_UNICODE_REPLACEMENT_CHARACTER;
			}
		}
		if( *utf7_stream_index >= utf7_stream_size )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
			 "%s: UTF-7 stream too small.",
			 function );

			return( -1 );
		}
		if( ( current_byte >= number_of_bytes )
		 && ( utf7_stream[ *utf7_stream_index ] == (uint8_t) '-' ) )
		{
			*utf7_stream_base64_data = 0;
			*utf7_stream_index      += 1;
		}
	}
	if( ( *utf7_stream_base64_data & LIBUNA_UTF7_IS_BASE64_ENCODED ) != 0 )
	{
		*utf7_stream_base64_data  = LIBUNA_UTF7_IS_BASE64_ENCODED;
		*utf7_stream_base64_data |= (uint32_t) current_byte << 28;
		*utf7_stream_base64_data |= (uint32_t) number_of_bytes << 24;
		*utf7_stream_base64_data |= base64_triplet & 0x00ffffff;
	}
	return( 1 );
}

/* Copies a Unicode character into a UTF-7 stream
 * The bits of the base64 data contain:
 *   0 - 23 the base64 triplet
 *  24 - 25 the number of bytes in the triplet
 *  26 - 27 unused
 *  28 - 29 the current byte
 *       30 unused
 *       31 flag to indicate the current UTF-7 characters are (modified) base64 encoded
 *
 * Returns 1 if successful or -1 on error
 */
LIBUNA_INLINE \
int libuna_unicode_character_copy_to_utf7_stream(
     libuna_unicode_character_t unicode_character,
     uint8_t *utf7_stream,
     size_t utf7_stream_size,
     size_t *utf7_stream_index,
     uint32_t *utf7_stream_base64_data,
     libcerror_error_t **error )
{
	static char *function                    = "libuna_unicode_character_copy_to_utf7_stream";
	libuna_utf16_character_t utf16_surrogate = 0;
	uint32_t base64_triplet                  = 0;
	uint8_t number_of_bytes                  = 0;
	uint8_t base64_encode_character          = 0;
	uint8_t current_byte                     = 0;
	uint8_t byte_bit_shift                   = 0;

	if( utf7_stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-7 stream.",
		 function );

		return( -1 );
	}
	if( utf7_stream_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid UTF-7 stream size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( utf7_stream_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-7 stream index.",
		 function );

		return( -1 );
	}
	if( utf7_stream_base64_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-7 stream base64 data.",
		 function );

		return( -1 );
	}
	/* Determine if the Unicode character is valid
	 */
	if( unicode_character > LIBUNA_UNICODE_CHARACTER_MAX )
	{
		unicode_character = LIBUNA_UNICODE_REPLACEMENT_CHARACTER;
	}
	/* A-Z is not a continous range on an EBCDIC based system
	 * it consists of the ranges: A-I, J-R, S-Z
	 */
	if( ( unicode_character >= 0x41 )
	 && ( unicode_character <= 0x49 ) )
	{
		unicode_character = ( unicode_character - 0x41 ) + (libuna_unicode_character_t) 'A';
	}
	else if( ( unicode_character >= 0x4a )
	      && ( unicode_character <= 0x52 ) )
	{
		unicode_character = ( unicode_character - 0x4a ) + (libuna_unicode_character_t) 'J';
	}
	else if( ( unicode_character >= 0x53 )
	      && ( unicode_character <= 0x5a ) )
	{
		unicode_character = ( unicode_character - 0x53 ) + (libuna_unicode_character_t) 'S';
	}
	/* a-z is not a continous range on an EBCDIC based system
	 * it consists of the ranges: a-i, j-r, s-z
	 */
	else if( ( unicode_character >= 0x61 )
	      && ( unicode_character <= 0x69 ) )
	{
		unicode_character = ( unicode_character - 0x61 ) + (libuna_unicode_character_t) 'a';
	}
	else if( ( unicode_character >= 0x6a )
	      && ( unicode_character <= 0x72 ) )
	{
		unicode_character = ( unicode_character - 0x6a ) + (libuna_unicode_character_t) 'j';
	}
	else if( ( unicode_character >= 0x73 )
	      && ( unicode_character <= 0x7a ) )
	{
		unicode_character = ( unicode_character - 0x73 ) + (libuna_unicode_character_t) 's';
	}
	/* 0-9
	 */
	else if( ( unicode_character >= 0x30 )
	      && ( unicode_character <= 0x39 ) )
	{
		unicode_character = ( unicode_character - 0x30 ) + (libuna_unicode_character_t) '0';
	}
	/* Valid directly encoded whitespace
	 */
	else if( ( unicode_character == (libuna_unicode_character_t) '\t' )
	      || ( unicode_character == (libuna_unicode_character_t) '\n' )
	      || ( unicode_character == (libuna_unicode_character_t) '\r' )
	      || ( unicode_character == (libuna_unicode_character_t) ' ' ) )
	{
	}
	/* The + character must be escaped
	 */
	else if( unicode_character == (libuna_unicode_character_t) '+' )
	{
	}
	/* Valid directly encoded characters
	 */
	else if( ( unicode_character == (libuna_unicode_character_t) '\'' )
	      || ( unicode_character == (libuna_unicode_character_t) '(' )
	      || ( unicode_character == (libuna_unicode_character_t) ')' )
	      || ( unicode_character == (libuna_unicode_character_t) ',' )
	      || ( unicode_character == (libuna_unicode_character_t) '-' )
	      || ( unicode_character == (libuna_unicode_character_t) '.' )
	      || ( unicode_character == (libuna_unicode_character_t) '/' )
	      || ( unicode_character == (libuna_unicode_character_t) ':' )
	      || ( unicode_character == (libuna_unicode_character_t) '?' ) )
	{
	}
	/* Valid optional directly encoded characters
	 */
	else if( ( unicode_character == (libuna_unicode_character_t) '!' )
	      || ( unicode_character == (libuna_unicode_character_t) '"' )
	      || ( unicode_character == (libuna_unicode_character_t) '#' )
	      || ( unicode_character == (libuna_unicode_character_t) '$' )
	      || ( unicode_character == (libuna_unicode_character_t) '%' )
	      || ( unicode_character == (libuna_unicode_character_t) '&' )
	      || ( unicode_character == (libuna_unicode_character_t) '*' )
	      || ( unicode_character == (libuna_unicode_character_t) ';' )
	      || ( unicode_character == (libuna_unicode_character_t) '<' )
	      || ( unicode_character == (libuna_unicode_character_t) '=' )
	      || ( unicode_character == (libuna_unicode_character_t) '>' )
	      || ( unicode_character == (libuna_unicode_character_t) '@' )
	      || ( unicode_character == (libuna_unicode_character_t) '[' )
	      || ( unicode_character == (libuna_unicode_character_t) ']' )
	      || ( unicode_character == (libuna_unicode_character_t) '^' )
	      || ( unicode_character == (libuna_unicode_character_t) '_' )
	      || ( unicode_character == (libuna_unicode_character_t) '`' )
	      || ( unicode_character == (libuna_unicode_character_t) '{' )
	      || ( unicode_character == (libuna_unicode_character_t) '|' )
	      || ( unicode_character == (libuna_unicode_character_t) '}' ) )
	{
	}
	/* Allow for the end of string character
	 */
	else if( unicode_character == 0 )
	{
	}
	else
	{
		base64_encode_character = 1;
	}
	if( base64_encode_character == 0 )
	{
		if( ( *utf7_stream_base64_data & LIBUNA_UTF7_IS_BASE64_ENCODED ) != 0 )
		{
			*utf7_stream_base64_data = 0;
		}
		if( *utf7_stream_index >= utf7_stream_size )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
			 "%s: UTF-7 stream too small.",
			 function );

			return( -1 );
		}
		utf7_stream[ *utf7_stream_index ] = (uint8_t) unicode_character;

		*utf7_stream_index += 1;

		/* The + character must be escaped
		 */
		if( unicode_character == (libuna_unicode_character_t) '+' )
		{
			if( *utf7_stream_index >= utf7_stream_size )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: UTF-7 stream too small.",
				 function );

				return( -1 );
			}
			utf7_stream[ *utf7_stream_index ] = (uint8_t) '-';

			*utf7_stream_index += 1;
		}
	}
	else
	{
		/* Escape the base64 encoded chracters with a +
		 */
		if( ( *utf7_stream_base64_data & LIBUNA_UTF7_IS_BASE64_ENCODED ) == 0 )
		{
			if( *utf7_stream_index >= utf7_stream_size )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: UTF-7 stream too small.",
				 function );

				return( -1 );
			}
			utf7_stream[ *utf7_stream_index ] = (uint8_t) '+';

			*utf7_stream_index += 1;
		}
		/* Otherwise continue the previously base64 encoded characters
		 */
		else
		{
			base64_triplet  = *utf7_stream_base64_data & 0x00ffffff;
			number_of_bytes = ( *utf7_stream_base64_data >> 24 ) & 0x03;
			current_byte    = ( *utf7_stream_base64_data >> 28 ) & 0x03;

			if( number_of_bytes > 0 )
			{
				/* Correct the index for the last partial base64 stream
				 */
				*utf7_stream_index -= number_of_bytes + 1;
			}
			/* Correct the index for the base64 stream termination character
			 */
			*utf7_stream_index -= 1;
		}
		*utf7_stream_base64_data = LIBUNA_UTF7_IS_BASE64_ENCODED;

		if( unicode_character > LIBUNA_UNICODE_BASIC_MULTILINGUAL_PLANE_MAX )
		{
			unicode_character -= 0x010000;

			utf16_surrogate = (libuna_utf16_character_t) ( ( unicode_character >> 10 )
			                + LIBUNA_UNICODE_SURROGATE_HIGH_RANGE_START );

			byte_bit_shift   = 16 - ( current_byte * 8 );
			base64_triplet  += (uint32_t) ( ( utf16_surrogate >> 8 ) & 0xff ) << byte_bit_shift;
			current_byte    += 1;
			number_of_bytes += 1;

			if( number_of_bytes == 3 )
			{
				if( libuna_base64_triplet_copy_to_base64_stream(
				     base64_triplet,
				     utf7_stream,
				     utf7_stream_size,
				     utf7_stream_index,
				     0,
				     LIBUNA_BASE64_VARIANT_UTF7,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to set base64 encoded UTF-7 characters.",
					 function );

					return( -1 );
				}
				number_of_bytes = 0;
				current_byte    = 0;
				base64_triplet  = 0;
			}
			byte_bit_shift   = 16 - ( current_byte * 8 );
			base64_triplet  += (uint32_t) ( utf16_surrogate & 0xff ) << byte_bit_shift;
			current_byte    += 1;
			number_of_bytes += 1;

			if( number_of_bytes == 3 )
			{
				if( libuna_base64_triplet_copy_to_base64_stream(
				     base64_triplet,
				     utf7_stream,
				     utf7_stream_size,
				     utf7_stream_index,
				     0,
				     LIBUNA_BASE64_VARIANT_UTF7,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to set base64 encoded UTF-7 characters.",
					 function );

					return( -1 );
				}
				number_of_bytes = 0;
				current_byte    = 0;
				base64_triplet  = 0;
			}
			unicode_character = (libuna_utf16_character_t) ( ( unicode_character & 0x03ff )
			                  + LIBUNA_UNICODE_SURROGATE_LOW_RANGE_START );
		}
		byte_bit_shift   = 16 - ( current_byte * 8 );
		base64_triplet  += (uint32_t) ( ( unicode_character >> 8 ) & 0xff ) << byte_bit_shift;
		current_byte    += 1;
		number_of_bytes += 1;

		if( number_of_bytes == 3 )
		{
			if( libuna_base64_triplet_copy_to_base64_stream(
			     base64_triplet,
			     utf7_stream,
			     utf7_stream_size,
			     utf7_stream_index,
			     0,
			     LIBUNA_BASE64_VARIANT_UTF7,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set base64 encoded UTF-7 characters.",
				 function );

				return( -1 );
			}
			number_of_bytes = 0;
			current_byte    = 0;
			base64_triplet  = 0;
		}
		byte_bit_shift   = 16 - ( current_byte * 8 );
		base64_triplet  += (uint32_t) ( unicode_character & 0xff ) << byte_bit_shift;
		current_byte    += 1;
		number_of_bytes += 1;

		if( number_of_bytes == 3 )
		{
			if( libuna_base64_triplet_copy_to_base64_stream(
			     base64_triplet,
			     utf7_stream,
			     utf7_stream_size,
			     utf7_stream_index,
			     0,
			     LIBUNA_BASE64_VARIANT_UTF7,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set base64 encoded UTF-7 characters.",
				 function );

				return( -1 );
			}
			number_of_bytes = 0;
			current_byte    = 0;
			base64_triplet  = 0;
		}
		/* Terminate the base64 encoded characters
		 */
		if( number_of_bytes > 0 )
		{
			if( libuna_base64_triplet_copy_to_base64_stream(
			     base64_triplet,
			     utf7_stream,
			     utf7_stream_size,
			     utf7_stream_index,
			     3 - number_of_bytes,
			     LIBUNA_BASE64_VARIANT_UTF7,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set base64 encoded UTF-7 characters.",
				 function );

				return( -1 );
			}
		}
		if( *utf7_stream_index >= utf7_stream_size )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
			 "%s: UTF-7 stream too small.",
			 function );

			return( -1 );
		}
		utf7_stream[ *utf7_stream_index ] = (uint8_t) '-';

		*utf7_stream_index += 1;
	}
	if( ( *utf7_stream_base64_data & LIBUNA_UTF7_IS_BASE64_ENCODED ) != 0 )
	{
		*utf7_stream_base64_data  = LIBUNA_UTF7_IS_BASE64_ENCODED;
		*utf7_stream_base64_data |= (uint32_t) current_byte << 28;
		*utf7_stream_base64_data |= (uint32_t) number_of_bytes << 24;
		*utf7_stream_base64_data |= base64_triplet & 0x00ffffff;
	}
	return( 1 );
}

/* Determines the size of an UTF-8 character from an Unicode character
 * Adds the size to the UTF-8 character size value
 * Returns 1 if successful or -1 on error
 */
LIBUNA_INLINE \
int libuna_unicode_character_size_to_utf8(
     libuna_unicode_character_t unicode_character,
     size_t *utf8_character_size,
     libcerror_error_t **error )
{
	static char *function = "libuna_unicode_character_size_to_utf8";

	if( utf8_character_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-8 character size.",
		 function );

		return( -1 );
	}
	if( unicode_character < 0x080 )
	{
		*utf8_character_size += 1;
	}
	else if( unicode_character < 0x0800 )
	{
		*utf8_character_size += 2;
	}
	else if( unicode_character < 0x010000 )
	{
		*utf8_character_size += 3;
	}
	else if( unicode_character > LIBUNA_UNICODE_CHARACTER_MAX )
	{
		*utf8_character_size += 3;
	}
	else
	{
		*utf8_character_size += 4;
	}

/* If UTF-8 USC support is needed it should be implemented in
 * utf8_usc or something, but for now leave this here as a reminder

	else if( unicode_character < 0x010000 )
	{
		*utf8_character_size += 3;
	}
	else if( unicode_character > LIBUNA_UNICODE_CHARACTER_MAX )
	{
		*utf8_character_size += 2;
	}
	else if( unicode_character < 0x0200000 )
	{
		*utf8_character_size += 4;
	}
	else if( unicode_character < 0x0400000 )
	{
		*utf8_character_size += 5;
	}
	else
	{
		*utf8_character_size += 6;
	}
*/
	return( 1 );
}

/* Copies a Unicode character from an UTF-8 string
 * Returns 1 if successful or -1 on error
 */
LIBUNA_INLINE \
int libuna_unicode_character_copy_from_utf8(
     libuna_unicode_character_t *unicode_character,
     const libuna_utf8_character_t *utf8_string,
     size_t utf8_string_size,
     size_t *utf8_string_index,
     libcerror_error_t **error )
{
	static char *function                   = "libuna_unicode_character_copy_from_utf8";
	uint8_t utf8_character_additional_bytes = 0;

	if( unicode_character == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid Unicode character.",
		 function );

		return( -1 );
	}
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
	if( *utf8_string_index >= utf8_string_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-8 string too small.",
		 function );

		return( -1 );
	}
	/* Determine the number of additional bytes of the UTF-8 character
	 */
	if( utf8_string[ *utf8_string_index ] < 0xc0 )
	{
		utf8_character_additional_bytes = 0;
	}
	else if( utf8_string[ *utf8_string_index ] < 0xe0 )
	{
		utf8_character_additional_bytes = 1;
	}
	else if( utf8_string[ *utf8_string_index ] < 0xf0 )
	{
		utf8_character_additional_bytes = 2;
	}
	else if( utf8_string[ *utf8_string_index ] < 0xf8 )
	{
		utf8_character_additional_bytes = 3;
	}
	else if( utf8_string[ *utf8_string_index ] < 0xfc )
	{
		utf8_character_additional_bytes = 4;
	}
	else
	{
		utf8_character_additional_bytes = 5;
	}
	if( ( *utf8_string_index + utf8_character_additional_bytes + 1 ) > utf8_string_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: missing UTF-8 character bytes.",
		 function );

		return( -1 );
	}
	/* Determine the UTF-8 character and make sure it is valid
	 * Unicode limits the UTF-8 character to consist of a maximum of 4 bytes
	 * while ISO 10646 Universal Character Set (UCS) allows up to 6 bytes
	 */
	if( utf8_string[ *utf8_string_index ] > 0xf4 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: invalid 1st UTF-8 character byte: 0x%02" PRIx8 ".",
		 function,
		 utf8_string[ *utf8_string_index ] );

		return( -1 );
	}
	*unicode_character = utf8_string[ *utf8_string_index ];

	if( utf8_character_additional_bytes == 0 )
	{
		if( ( utf8_string[ *utf8_string_index ] >= 0x80 )
		 && ( utf8_string[ *utf8_string_index ] < 0xc2 ) )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: invalid 1st UTF-8 character byte: 0x%02" PRIx8 ".",
			 function,
			 utf8_string[ *utf8_string_index ] );

			return( -1 );
		}
	}
	if( utf8_character_additional_bytes >= 1 )
	{
		if( utf8_string[ *utf8_string_index + 1 ] > 0xbf )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: invalid 2nd UTF-8 character byte: 0x%02" PRIx8 ".",
			 function,
			 utf8_string[ *utf8_string_index + 1 ] );

			return( -1 );
		}
		if( ( utf8_string[ *utf8_string_index ] == 0xe0 )
		 && ( utf8_string[ *utf8_string_index + 1 ] < 0xa0 ) )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: invalid 2nd UTF-8 character byte: 0x%02" PRIx8 ".",
			 function,
			 utf8_string[ *utf8_string_index + 1 ] );

			return( -1 );
		}
		else if( ( utf8_string[ *utf8_string_index ] == 0xed )
		      && ( utf8_string[ *utf8_string_index + 1 ] > 0x9f ) )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: invalid 2nd UTF-8 character byte: 0x%02" PRIx8 ".",
			 function,
			 utf8_string[ *utf8_string_index + 1 ] );

			return( -1 );
		}
		else if( ( utf8_string[ *utf8_string_index ] == 0xf0 )
		      && ( utf8_string[ *utf8_string_index + 1 ] < 0x90 ) )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: invalid 2nd UTF-8 character byte: 0x%02" PRIx8 ".",
			 function,
			 utf8_string[ *utf8_string_index + 1 ] );

			return( -1 );
		}
		else if( ( utf8_string[ *utf8_string_index ] == 0xf4 )
		      && ( utf8_string[ *utf8_string_index + 1 ] > 0x8f ) )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: invalid 2nd UTF-8 character byte: 0x%02" PRIx8 ".",
			 function,
			 utf8_string[ *utf8_string_index + 1 ] );

			return( -1 );
		}
		else if( utf8_string[ *utf8_string_index + 1 ] < 0x80 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: invalid 2nd UTF-8 character byte: 0x%02" PRIx8 ".",
			 function,
			 utf8_string[ *utf8_string_index + 1 ] );

			return( -1 );
		}
		*unicode_character <<= 6;
		*unicode_character += utf8_string[ *utf8_string_index + 1 ];

		if( utf8_character_additional_bytes == 1 )
		{
			*unicode_character -= 0x03080;
		}
	}
	if( utf8_character_additional_bytes >= 2 )
	{
		if( ( utf8_string[ *utf8_string_index + 2 ] < 0x80 )
		  || ( utf8_string[ *utf8_string_index + 2 ] > 0xbf ) )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: invalid 3rd UTF-8 character byte: 0x%02" PRIx8 ".",
			 function,
			 utf8_string[ *utf8_string_index + 2 ] );

			return( -1 );
		}
		*unicode_character <<= 6;
		*unicode_character += utf8_string[ *utf8_string_index + 2 ];

		if( utf8_character_additional_bytes == 2 )
		{
			*unicode_character -= 0x0e2080;
		}
	}
	if( utf8_character_additional_bytes >= 3 )
	{
		if( ( utf8_string[ *utf8_string_index + 3 ] < 0x80 )
		  || ( utf8_string[ *utf8_string_index + 3 ] > 0xbf ) )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: invalid 4th UTF-8 character byte: 0x%02" PRIx8 ".",
			 function,
			 utf8_string[ *utf8_string_index + 3 ] );

			return( -1 );
		}
		*unicode_character <<= 6;
		*unicode_character += utf8_string[ *utf8_string_index + 3 ];

		if( utf8_character_additional_bytes == 3 )
		{
			*unicode_character -= 0x03c82080;
		}
	}
	if( utf8_character_additional_bytes >= 4 )
	{
		if( ( utf8_string[ *utf8_string_index + 4 ] < 0x80 )
		  || ( utf8_string[ *utf8_string_index + 4 ] > 0xbf ) )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: invalid 5th UTF-8 character byte: 0x%02" PRIx8 ".",
			 function,
			 utf8_string[ *utf8_string_index + 4 ] );

			return( -1 );
		}
		*unicode_character <<= 6;
		*unicode_character += utf8_string[ *utf8_string_index + 4 ];

		if( utf8_character_additional_bytes == 4 )
		{
			*unicode_character -= 0x0fa082080;
		}
	}
	if( utf8_character_additional_bytes == 5 )
	{
		if( ( utf8_string[ *utf8_string_index + 5 ] < 0x80 )
		  || ( utf8_string[ *utf8_string_index + 5 ] > 0xbf ) )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: invalid 6th UTF-8 character byte: 0x%02" PRIx8 ".",
			 function,
			 utf8_string[ *utf8_string_index + 5 ] );

			return( -1 );
		}
		*unicode_character <<= 6;
		*unicode_character += utf8_string[ *utf8_string_index + 5 ];
		*unicode_character -= 0x082082080;
	}
	/* Determine if the Unicode character is valid
	 */
	if( *unicode_character > LIBUNA_UNICODE_CHARACTER_MAX )
	{
		*unicode_character = LIBUNA_UNICODE_REPLACEMENT_CHARACTER;
	}
	*utf8_string_index += 1 + utf8_character_additional_bytes;

	return( 1 );
}

/* Copies a Unicode character into a UTF-8 string
 * Returns 1 if successful or -1 on error
 */
LIBUNA_INLINE \
int libuna_unicode_character_copy_to_utf8(
     libuna_unicode_character_t unicode_character,
     libuna_utf8_character_t *utf8_string,
     size_t utf8_string_size,
     size_t *utf8_string_index,
     libcerror_error_t **error )
{
	static char *function                   = "libuna_unicode_character_copy_to_utf8";
	uint8_t utf8_character_additional_bytes = 0;
	uint8_t utf8_character_iterator         = 0;
	uint8_t utf8_first_character_mark       = 0;

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
	if( *utf8_string_index >= utf8_string_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-8 string too small.",
		 function );

		return( -1 );
	}
	/* Determine if the Unicode character is valid
	 */
	if( unicode_character > LIBUNA_UNICODE_CHARACTER_MAX )
	{
		unicode_character = LIBUNA_UNICODE_REPLACEMENT_CHARACTER;
	}
	/* Determine how many UTF-8 character bytes are required
	 */
	if( unicode_character < 0x080 )
	{
		utf8_character_additional_bytes = 0;
		utf8_first_character_mark       = 0;
	}
	else if( unicode_character < 0x0800 )
	{
		utf8_character_additional_bytes = 1;
		utf8_first_character_mark       = 0x0c0;
	}
	else if( unicode_character < 0x010000 )
	{
		utf8_character_additional_bytes = 2;
		utf8_first_character_mark       = 0x0e0;
	}
	else if( unicode_character < 0x0200000 )
	{
		utf8_character_additional_bytes = 3;
		utf8_first_character_mark       = 0x0f0;
	}
	else if( unicode_character < 0x0400000 )
	{
		utf8_character_additional_bytes = 4;
		utf8_first_character_mark       = 0x0f8;
	}
	else
	{
		utf8_character_additional_bytes = 5;
		utf8_first_character_mark       = 0x0fc;
	}
	/* Convert Unicode character into UTF-8 character bytes
	 */
	if( ( *utf8_string_index + utf8_character_additional_bytes ) >= utf8_string_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-8 string too small.",
		 function );

		return( -1 );
	}
	for( utf8_character_iterator = utf8_character_additional_bytes;
	     utf8_character_iterator > 0;
	     utf8_character_iterator-- )
	{
		utf8_string[ *utf8_string_index + utf8_character_iterator ] = (libuna_utf8_character_t) ( ( unicode_character & 0x0bf ) | 0x080 );

		unicode_character >>= 6;
	}
	utf8_string[ *utf8_string_index ] = (libuna_utf8_character_t) ( unicode_character | utf8_first_character_mark );

	*utf8_string_index += 1 + utf8_character_additional_bytes;

	return( 1 );
}

/* Determines the size of an UTF-16 character from an Unicode character
 * Adds the size to the UTF-16 character size value
 * Returns 1 if successful or -1 on error
 */
LIBUNA_INLINE \
int libuna_unicode_character_size_to_utf16(
     libuna_unicode_character_t unicode_character,
     size_t *utf16_character_size,
     libcerror_error_t **error )
{
	static char *function = "libuna_unicode_character_size_to_utf16";

	if( utf16_character_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-16 character size.",
		 function );

		return( -1 );
	}
	if( ( unicode_character > LIBUNA_UNICODE_BASIC_MULTILINGUAL_PLANE_MAX )
         && ( unicode_character <= LIBUNA_UTF16_CHARACTER_MAX ) )
	{
		*utf16_character_size += 2;
	}
	else
	{
		*utf16_character_size += 1;
	}
	return( 1 );
}

/* Copies a Unicode character from an UTF-16 string
 * Returns 1 if successful or -1 on error
 */
LIBUNA_INLINE \
int libuna_unicode_character_copy_from_utf16(
     libuna_unicode_character_t *unicode_character,
     const libuna_utf16_character_t *utf16_string,
     size_t utf16_string_size,
     size_t *utf16_string_index,
     libcerror_error_t **error )
{
	static char *function                    = "libuna_unicode_character_copy_from_utf16";
	libuna_utf16_character_t utf16_surrogate = 0;

	if( unicode_character == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid Unicode character.",
		 function );

		return( -1 );
	}
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
		 "%s: invalid UTF-16 string size value exceeds maximum.",
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
	if( *utf16_string_index >= utf16_string_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-16 string too small.",
		 function );

		return( -1 );
	}
	*unicode_character   = utf16_string[ *utf16_string_index ];
	*utf16_string_index += 1;

	/* Determine if the UTF-16 character is within the high surrogate range
	 */
	if( ( *unicode_character >= LIBUNA_UNICODE_SURROGATE_HIGH_RANGE_START )
	 && ( *unicode_character <= LIBUNA_UNICODE_SURROGATE_HIGH_RANGE_END ) )
	{
		if( *utf16_string_index >= utf16_string_size )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: missing surrogate UTF-16 character bytes.",
			 function );

			return( -1 );
		}
		utf16_surrogate      = utf16_string[ *utf16_string_index ];
		*utf16_string_index += 1;

		/* Determine if the UTF-16 character is within the low surrogate range
		 */
		if( ( utf16_surrogate >= LIBUNA_UNICODE_SURROGATE_LOW_RANGE_START )
		 && ( utf16_surrogate <= LIBUNA_UNICODE_SURROGATE_LOW_RANGE_END ) )
		{
			*unicode_character  -= LIBUNA_UNICODE_SURROGATE_HIGH_RANGE_START;
			*unicode_character <<= 10;
			*unicode_character  += utf16_surrogate - LIBUNA_UNICODE_SURROGATE_LOW_RANGE_END;
			*unicode_character  += 0x010000;
		}
		else
		{
			*unicode_character = LIBUNA_UNICODE_REPLACEMENT_CHARACTER;
		}
	}
	/* Determine if the Unicode character is valid
	 */
	else if( ( *unicode_character >= LIBUNA_UNICODE_SURROGATE_LOW_RANGE_START )
	      && ( *unicode_character <= LIBUNA_UNICODE_SURROGATE_LOW_RANGE_END ) )
	{
		*unicode_character = LIBUNA_UNICODE_REPLACEMENT_CHARACTER;
	}
	return( 1 );
}

/* Copies a Unicode character into a UTF-16 string
 * Returns 1 if successful or -1 on error
 */
LIBUNA_INLINE \
int libuna_unicode_character_copy_to_utf16(
     libuna_unicode_character_t unicode_character,
     libuna_utf16_character_t *utf16_string,
     size_t utf16_string_size,
     size_t *utf16_string_index,
     libcerror_error_t **error )
{
	static char *function = "libuna_unicode_character_copy_to_utf16";

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
		 "%s: invalid UTF-16 string size value exceeds maximum.",
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
	if( *utf16_string_index >= utf16_string_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-16 string too small.",
		 function );

		return( -1 );
	}
	/* Determine if the Unicode character is valid
	 */
	if( ( ( unicode_character >= LIBUNA_UNICODE_SURROGATE_HIGH_RANGE_START )
	  && ( unicode_character <= LIBUNA_UNICODE_SURROGATE_LOW_RANGE_END ) )
	 || ( unicode_character > LIBUNA_UTF16_CHARACTER_MAX ) )
	{
		unicode_character = LIBUNA_UNICODE_REPLACEMENT_CHARACTER;
	}
	if( unicode_character <= LIBUNA_UNICODE_BASIC_MULTILINGUAL_PLANE_MAX )
	{
		utf16_string[ *utf16_string_index ] = (libuna_utf16_character_t) unicode_character;

		*utf16_string_index += 1;
	}
	else
	{
		if( ( *utf16_string_index + 1 ) >= utf16_string_size )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
			 "%s: UTF-16 string too small.",
			 function );

			return( -1 );
		}
		unicode_character                      -= 0x010000;
		utf16_string[ *utf16_string_index     ] = (libuna_utf16_character_t) ( ( unicode_character >> 10 ) + LIBUNA_UNICODE_SURROGATE_HIGH_RANGE_START );
		utf16_string[ *utf16_string_index + 1 ] = (libuna_utf16_character_t) ( ( unicode_character & 0x03ff ) + LIBUNA_UNICODE_SURROGATE_LOW_RANGE_START );

		*utf16_string_index += 2;
	}
	return( 1 );
}

/* Copies a Unicode character from an UTF-16 stream
 * Returns 1 if successful or -1 on error
 */
LIBUNA_INLINE \
int libuna_unicode_character_copy_from_utf16_stream(
     libuna_unicode_character_t *unicode_character,
     const uint8_t *utf16_stream,
     size_t utf16_stream_size,
     size_t *utf16_stream_index,
     int byte_order,
     libcerror_error_t **error )
{
	static char *function                    = "libuna_unicode_character_copy_from_utf16_stream";
	libuna_utf16_character_t utf16_surrogate = 0;

	if( unicode_character == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid Unicode character.",
		 function );

		return( -1 );
	}
	if( utf16_stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-16 stream.",
		 function );

		return( -1 );
	}
	if( utf16_stream_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid UTF-16 stream size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( utf16_stream_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-16 stream index.",
		 function );

		return( -1 );
	}
	if( *utf16_stream_index >= utf16_stream_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-16 stream too small.",
		 function );

		return( -1 );
	}
	if( ( byte_order != LIBUNA_ENDIAN_BIG )
	 && ( byte_order != LIBUNA_ENDIAN_LITTLE ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported byte order.",
		 function );

		return( -1 );
	}
	if( byte_order == LIBUNA_ENDIAN_BIG )
	{
		*unicode_character   = utf16_stream[ *utf16_stream_index ];
		*unicode_character <<= 8;
		*unicode_character  += utf16_stream[ *utf16_stream_index + 1 ];
	}
	else if( byte_order == LIBUNA_ENDIAN_LITTLE )
	{
		*unicode_character   = utf16_stream[ *utf16_stream_index + 1 ];
		*unicode_character <<= 8;
		*unicode_character  += utf16_stream[ *utf16_stream_index ];
	}
	*utf16_stream_index += 2;

	/* Determine if the UTF-16 character is within the high surrogate range
	 */
	if( ( *unicode_character >= LIBUNA_UNICODE_SURROGATE_HIGH_RANGE_START )
	 && ( *unicode_character <= LIBUNA_UNICODE_SURROGATE_HIGH_RANGE_END ) )
	{
		if( *utf16_stream_index >= utf16_stream_size )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: missing surrogate UTF-16 character bytes.",
			 function );

			return( -1 );
		}
		if( byte_order == LIBUNA_ENDIAN_BIG )
		{
			utf16_surrogate   = utf16_stream[ *utf16_stream_index ];
			utf16_surrogate <<= 8;
			utf16_surrogate  += utf16_stream[ *utf16_stream_index + 1 ];
		}
		else if( byte_order == LIBUNA_ENDIAN_LITTLE )
		{
			utf16_surrogate   = utf16_stream[ *utf16_stream_index + 1 ];
			utf16_surrogate <<= 8;
			utf16_surrogate  += utf16_stream[ *utf16_stream_index ];
		}
		*utf16_stream_index += 2;

		/* Determine if the UTF-16 character is within the low surrogate range
		 */
		if( ( utf16_surrogate >= LIBUNA_UNICODE_SURROGATE_LOW_RANGE_START )
		 && ( utf16_surrogate <= LIBUNA_UNICODE_SURROGATE_LOW_RANGE_END ) )
		{
			*unicode_character  -= LIBUNA_UNICODE_SURROGATE_HIGH_RANGE_START;
			*unicode_character <<= 10;
			*unicode_character  += utf16_surrogate - LIBUNA_UNICODE_SURROGATE_LOW_RANGE_END;
			*unicode_character  += 0x010000;
		}
		else
		{
			*unicode_character = LIBUNA_UNICODE_REPLACEMENT_CHARACTER;
		}
	}
	/* Determine if the Unicode character is valid
	 */
	else if( ( *unicode_character >= LIBUNA_UNICODE_SURROGATE_LOW_RANGE_START )
	      && ( *unicode_character <= LIBUNA_UNICODE_SURROGATE_LOW_RANGE_END ) )
	{
		*unicode_character = LIBUNA_UNICODE_REPLACEMENT_CHARACTER;
	}
	return( 1 );
}

/* Copies a Unicode character to an UTF-16 stream
 * Returns 1 if successful or -1 on error
 */
LIBUNA_INLINE \
int libuna_unicode_character_copy_to_utf16_stream(
     libuna_unicode_character_t unicode_character,
     uint8_t *utf16_stream,
     size_t utf16_stream_size,
     size_t *utf16_stream_index,
     int byte_order,
     libcerror_error_t **error )
{
	static char *function                    = "libuna_unicode_character_copy_to_utf16_stream";
	libuna_utf16_character_t utf16_surrogate = 0;

	if( utf16_stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-16 stream.",
		 function );

		return( -1 );
	}
	if( utf16_stream_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid UTF-16 stream size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( utf16_stream_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-16 stream index.",
		 function );

		return( -1 );
	}
	if( ( *utf16_stream_index + 1 ) >= utf16_stream_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-16 stream too small.",
		 function );

		return( -1 );
	}
	if( ( byte_order != LIBUNA_ENDIAN_BIG )
	 && ( byte_order != LIBUNA_ENDIAN_LITTLE ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported byte order.",
		 function );

		return( -1 );
	}
	/* Determine if the Unicode character is valid
	 */
	if( ( ( unicode_character >= LIBUNA_UNICODE_SURROGATE_HIGH_RANGE_START )
	  && ( unicode_character <= LIBUNA_UNICODE_SURROGATE_LOW_RANGE_END ) )
	 || ( unicode_character > LIBUNA_UTF16_CHARACTER_MAX ) )
	{
		unicode_character = LIBUNA_UNICODE_REPLACEMENT_CHARACTER;
	}
	if( unicode_character <= LIBUNA_UNICODE_BASIC_MULTILINGUAL_PLANE_MAX )
	{
		if( byte_order == LIBUNA_ENDIAN_BIG )
		{
			utf16_stream[ *utf16_stream_index + 1 ]   = (uint8_t) ( unicode_character & 0xff );
			unicode_character                       >>= 8;
			utf16_stream[ *utf16_stream_index     ]   = (uint8_t) ( unicode_character & 0xff );
		}
		else if( byte_order == LIBUNA_ENDIAN_LITTLE )
		{
			utf16_stream[ *utf16_stream_index     ]   = (uint8_t) ( unicode_character & 0xff );
			unicode_character                       >>= 8;
			utf16_stream[ *utf16_stream_index + 1 ]   = (uint8_t) ( unicode_character & 0xff );
		}
		*utf16_stream_index += 2;
	}
	else
	{
		if( ( *utf16_stream_index + 3 ) >= utf16_stream_size )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
			 "%s: UTF-16 stream too small.",
			 function );

			return( -1 );
		}
		unicode_character -= 0x010000;

		utf16_surrogate = (libuna_utf16_character_t) ( ( unicode_character >> 10 ) + LIBUNA_UNICODE_SURROGATE_HIGH_RANGE_START );

		if( byte_order == LIBUNA_ENDIAN_BIG )
		{
			utf16_stream[ *utf16_stream_index + 1 ]   = (uint8_t) ( utf16_surrogate & 0xff );
			utf16_surrogate                         >>= 8;
			utf16_stream[ *utf16_stream_index     ]   = (uint8_t) ( utf16_surrogate & 0xff );
		}
		else if( byte_order == LIBUNA_ENDIAN_LITTLE )
		{
			utf16_stream[ *utf16_stream_index     ]   = (uint8_t) ( utf16_surrogate & 0xff );
			utf16_surrogate                         >>= 8;
			utf16_stream[ *utf16_stream_index + 1 ]   = (uint8_t) ( utf16_surrogate & 0xff );
		}
		*utf16_stream_index += 2;

		utf16_surrogate = (libuna_utf16_character_t) ( ( unicode_character & 0x03ff ) + LIBUNA_UNICODE_SURROGATE_LOW_RANGE_START );

		if( byte_order == LIBUNA_ENDIAN_BIG )
		{
			utf16_stream[ *utf16_stream_index + 1 ]   = (uint8_t) ( utf16_surrogate & 0xff );
			utf16_surrogate                         >>= 8;
			utf16_stream[ *utf16_stream_index     ]   = (uint8_t) ( utf16_surrogate & 0xff );
		}
		else if( byte_order == LIBUNA_ENDIAN_LITTLE )
		{
			utf16_stream[ *utf16_stream_index     ]   = (uint8_t) ( utf16_surrogate & 0xff );
			utf16_surrogate                         >>= 8;
			utf16_stream[ *utf16_stream_index + 1 ]   = (uint8_t) ( utf16_surrogate & 0xff );
		}
		*utf16_stream_index += 2;
	}
	return( 1 );
}

/* Determines the size of an UTF-32 character from an Unicode character
 * Adds the size to the UTF-32 character size value
 * Returns 1 if successful or -1 on error
 */
LIBUNA_INLINE \
int libuna_unicode_character_size_to_utf32(
     libuna_unicode_character_t unicode_character LIBUNA_ATTRIBUTE_UNUSED,
     size_t *utf32_character_size,
     libcerror_error_t **error )
{
	static char *function = "libuna_unicode_character_size_to_utf32";

	LIBUNA_UNREFERENCED_PARAMETER( unicode_character )

	if( utf32_character_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-32 character size.",
		 function );

		return( -1 );
	}
	*utf32_character_size += 1;

	return( 1 );
}

/* Copies a Unicode character from an UTF-32 string
 * Returns 1 if successful or -1 on error
 */
LIBUNA_INLINE \
int libuna_unicode_character_copy_from_utf32(
     libuna_unicode_character_t *unicode_character,
     const libuna_utf32_character_t *utf32_string,
     size_t utf32_string_size,
     size_t *utf32_string_index,
     libcerror_error_t **error )
{
	static char *function = "libuna_unicode_character_copy_from_utf32";

	if( unicode_character == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid Unicode character.",
		 function );

		return( -1 );
	}
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
		 "%s: invalid UTF-32 string size value exceeds maximum.",
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
	if( *utf32_string_index >= utf32_string_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-32 string too small.",
		 function );

		return( -1 );
	}
	/* Determine if the Unicode character is valid
	 */
	if( ( utf32_string[ *utf32_string_index ] >= LIBUNA_UNICODE_SURROGATE_LOW_RANGE_START )
	 && ( utf32_string[ *utf32_string_index ] <= LIBUNA_UNICODE_SURROGATE_LOW_RANGE_END ) )
	{
		*unicode_character = LIBUNA_UNICODE_REPLACEMENT_CHARACTER;
	}
	else
	{
		*unicode_character = utf32_string[ *utf32_string_index ];
	}
	*utf32_string_index += 1;

	return( 1 );
}

/* Copies a Unicode character into a UTF-32 string
 * Returns 1 if successful or -1 on error
 */
LIBUNA_INLINE \
int libuna_unicode_character_copy_to_utf32(
     libuna_unicode_character_t unicode_character,
     libuna_utf32_character_t *utf32_string,
     size_t utf32_string_size,
     size_t *utf32_string_index,
     libcerror_error_t **error )
{
	static char *function = "libuna_unicode_character_copy_to_utf32";

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
		 "%s: invalid UTF-32 string size value exceeds maximum.",
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
	if( *utf32_string_index >= utf32_string_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-32 string too small.",
		 function );

		return( -1 );
	}
	/* Determine if the Unicode character is valid
	 */
	if( ( ( unicode_character >= LIBUNA_UNICODE_SURROGATE_HIGH_RANGE_START )
	  && ( unicode_character <= LIBUNA_UNICODE_SURROGATE_LOW_RANGE_END ) )
	 || ( unicode_character > LIBUNA_UTF32_CHARACTER_MAX ) )
	{
		utf32_string[ *utf32_string_index ] = (libuna_utf32_character_t) LIBUNA_UNICODE_REPLACEMENT_CHARACTER;
	}
	else
	{
		utf32_string[ *utf32_string_index ] = (libuna_utf32_character_t) unicode_character;
	}
	*utf32_string_index += 1;

	return( 1 );
}

/* Copies a Unicode character from an UTF-32 stream
 * Returns 1 if successful or -1 on error
 */
LIBUNA_INLINE \
int libuna_unicode_character_copy_from_utf32_stream(
     libuna_unicode_character_t *unicode_character,
     const uint8_t *utf32_stream,
     size_t utf32_stream_size,
     size_t *utf32_stream_index,
     int byte_order,
     libcerror_error_t **error )
{
	static char *function = "libuna_unicode_character_copy_from_utf32_stream";

	if( unicode_character == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid Unicode character.",
		 function );

		return( -1 );
	}
	if( utf32_stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-32 stream.",
		 function );

		return( -1 );
	}
	if( utf32_stream_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid UTF-32 stream size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( utf32_stream_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-32 stream index.",
		 function );

		return( -1 );
	}
	if( *utf32_stream_index >= utf32_stream_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-32 stream too small.",
		 function );

		return( -1 );
	}
	if( ( byte_order != LIBUNA_ENDIAN_BIG )
	 && ( byte_order != LIBUNA_ENDIAN_LITTLE ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported byte order.",
		 function );

		return( -1 );
	}
	if( byte_order == LIBUNA_ENDIAN_BIG )
	{
		*unicode_character   = utf32_stream[ *utf32_stream_index ];
		*unicode_character <<= 8;
		*unicode_character  += utf32_stream[ *utf32_stream_index + 1 ];
		*unicode_character <<= 8;
		*unicode_character  += utf32_stream[ *utf32_stream_index + 2 ];
		*unicode_character <<= 8;
		*unicode_character  += utf32_stream[ *utf32_stream_index + 3 ];
	}
	else if( byte_order == LIBUNA_ENDIAN_LITTLE )
	{
		*unicode_character   = utf32_stream[ *utf32_stream_index + 3 ];
		*unicode_character <<= 8;
		*unicode_character   = utf32_stream[ *utf32_stream_index + 2 ];
		*unicode_character <<= 8;
		*unicode_character   = utf32_stream[ *utf32_stream_index + 1 ];
		*unicode_character <<= 8;
		*unicode_character  += utf32_stream[ *utf32_stream_index ];
	}
	/* Determine if the Unicode character is valid
	 */
	if( ( *unicode_character >= LIBUNA_UNICODE_SURROGATE_LOW_RANGE_START )
	 && ( *unicode_character <= LIBUNA_UNICODE_SURROGATE_LOW_RANGE_END ) )
	{
		*unicode_character = LIBUNA_UNICODE_REPLACEMENT_CHARACTER;
	}
	*utf32_stream_index += 4;

	return( 1 );
}

/* Copies a Unicode character to an UTF-32 stream
 * Returns 1 if successful or -1 on error
 */
LIBUNA_INLINE \
int libuna_unicode_character_copy_to_utf32_stream(
     libuna_unicode_character_t unicode_character,
     uint8_t *utf32_stream,
     size_t utf32_stream_size,
     size_t *utf32_stream_index,
     int byte_order,
     libcerror_error_t **error )
{
	static char *function = "libuna_unicode_character_copy_to_utf32_stream";

	if( utf32_stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-32 stream.",
		 function );

		return( -1 );
	}
	if( utf32_stream_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid UTF-32 stream size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( utf32_stream_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid UTF-32 stream index.",
		 function );

		return( -1 );
	}
	if( ( *utf32_stream_index + 1 ) >= utf32_stream_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: UTF-32 stream too small.",
		 function );

		return( -1 );
	}
	if( ( byte_order != LIBUNA_ENDIAN_BIG )
	 && ( byte_order != LIBUNA_ENDIAN_LITTLE ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported byte order.",
		 function );

		return( -1 );
	}
	/* Determine if the Unicode character is valid
	 */
	if( ( ( unicode_character >= LIBUNA_UNICODE_SURROGATE_HIGH_RANGE_START )
	  && ( unicode_character <= LIBUNA_UNICODE_SURROGATE_LOW_RANGE_END ) )
	 || ( unicode_character > LIBUNA_UTF32_CHARACTER_MAX ) )
	{
		unicode_character = LIBUNA_UNICODE_REPLACEMENT_CHARACTER;
	}
	if( byte_order == LIBUNA_ENDIAN_BIG )
	{
		utf32_stream[ *utf32_stream_index + 3 ]   = (uint8_t) ( unicode_character & 0xff );
		unicode_character                       >>= 8;
		utf32_stream[ *utf32_stream_index + 2 ]   = (uint8_t) ( unicode_character & 0xff );
		unicode_character                       >>= 8;
		utf32_stream[ *utf32_stream_index + 1 ]   = (uint8_t) ( unicode_character & 0xff );
		unicode_character                       >>= 8;
		utf32_stream[ *utf32_stream_index     ]   = (uint8_t) ( unicode_character & 0xff );
	}
	else if( byte_order == LIBUNA_ENDIAN_LITTLE )
	{
		utf32_stream[ *utf32_stream_index     ]   = (uint8_t) ( unicode_character & 0xff );
		unicode_character                       >>= 8;
		utf32_stream[ *utf32_stream_index + 1 ]   = (uint8_t) ( unicode_character & 0xff );
		unicode_character                       >>= 8;
		utf32_stream[ *utf32_stream_index + 2 ]   = (uint8_t) ( unicode_character & 0xff );
		unicode_character                       >>= 8;
		utf32_stream[ *utf32_stream_index + 3 ]   = (uint8_t) ( unicode_character & 0xff );
	}
	*utf32_stream_index += 4;

	return( 1 );
}

