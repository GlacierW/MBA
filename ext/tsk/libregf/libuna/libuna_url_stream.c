/*
 * Percent or URL encoded stream functions
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

#include "libuna_definitions.h"
#include "libuna_libcerror.h"
#include "libuna_types.h"
#include "libuna_url_stream.h"

/* Determines the size of a url stream from a byte stream
 * Returns 1 if successful or -1 on error
 */
int libuna_url_stream_size_from_byte_stream(
     uint8_t *byte_stream,
     size_t byte_stream_size,
     size_t *url_stream_size,
     libcerror_error_t **error )
{
	static char *function    = "libuna_url_stream_size_from_byte_stream";
	size_t byte_stream_index = 0;

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
	if( url_stream_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid url stream size.",
		 function );

		return( -1 );
	}
	/* TODO what about end of string character */

	*url_stream_size = 0;

	/* Check if the byte stream contains
	 * reserved or non-allowed characters that need to be decoded
	 */
	while( byte_stream_index < byte_stream_size )
	{
		/* A-Z is not a continous range on an EBCDIC based system
		 * it consists of the ranges: A-I, J-R, S-Z
		 */
		if( ( byte_stream[ byte_stream_index ] >= (uint8_t) 'A' )
		 && ( byte_stream[ byte_stream_index ] <= (uint8_t) 'I' ) )
		{
			*url_stream_size += 1;
		}
		else if( ( byte_stream[ byte_stream_index ] >= (uint8_t) 'J' )
		      && ( byte_stream[ byte_stream_index ] <= (uint8_t) 'R' ) )
		{
			*url_stream_size += 1;
		}
		else if( ( byte_stream[ byte_stream_index ] >= (uint8_t) 'S' )
		      && ( byte_stream[ byte_stream_index ] <= (uint8_t) 'Z' ) )
		{
			*url_stream_size += 1;
		}
		/* a-z is not a continous range on an EBCDIC based system
		 * it consists of the ranges: a-i, j-r, s-z
		 */
		else if( ( byte_stream[ byte_stream_index ] >= (uint8_t) 'a' )
		      && ( byte_stream[ byte_stream_index ] <= (uint8_t) 'i' ) )
		{
			*url_stream_size += 1;
		}
		else if( ( byte_stream[ byte_stream_index ] >= (uint8_t) 'j' )
		      && ( byte_stream[ byte_stream_index ] <= (uint8_t) 'r' ) )
		{
			*url_stream_size += 1;
		}
		else if( ( byte_stream[ byte_stream_index ] >= (uint8_t) 's' )
		      && ( byte_stream[ byte_stream_index ] <= (uint8_t) 'z' ) )
		{
			*url_stream_size += 1;
		}
		else if( ( byte_stream[ byte_stream_index ] >= (uint8_t) '0' )
		      && ( byte_stream[ byte_stream_index ] <= (uint8_t) '9' ) )
		{
			*url_stream_size += 1;
		}
		else if( ( byte_stream[ byte_stream_index ] == (uint8_t) '-' )
		      || ( byte_stream[ byte_stream_index ] == (uint8_t) '_' )
		      || ( byte_stream[ byte_stream_index ] == (uint8_t) '.' )
		      || ( byte_stream[ byte_stream_index ] == (uint8_t) '`' ) )
		{
			*url_stream_size += 1;
		}
		else
		{
			*url_stream_size += 3;
		}
		byte_stream_index++;
	}
	return( 1 );
}

/* Copies a url stream from a byte stream
 * Returns 1 if successful or -1 on error
 */
int libuna_url_stream_copy_from_byte_stream(
     uint8_t *url_stream,
     size_t url_stream_size,
     uint8_t *byte_stream,
     size_t byte_stream_size,
     libcerror_error_t **error )
{
	static char *function    = "libuna_url_stream_copy_from_byte_stream";
	size_t url_stream_index  = 0;
	size_t byte_stream_index = 0;
	uint8_t byte_value       = 0;

	if( url_stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid url stream.",
		 function );

		return( -1 );
	}
	if( url_stream_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid url stream size value exceeds maximum.",
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
	/* TODO what about end of string character */

	/* Check if the byte stream contains
	 * reserved or non-allowed characters that need to be decoded
	 */
	while( byte_stream_index < byte_stream_size )
	{
		if( url_stream_index >= url_stream_size )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
			 "%s: url stream is too small.",
			 function );

			return( -1 );
		}
		/* A-Z is not a continous range on an EBCDIC based system
		 * it consists of the ranges: A-I, J-R, S-Z
		 */
		if( ( byte_stream[ byte_stream_index ] >= (uint8_t) 'A' )
		 && ( byte_stream[ byte_stream_index ] <= (uint8_t) 'I' ) )
		{
			url_stream[ url_stream_index++ ] = byte_stream[ byte_stream_index ];
		}
		else if( ( byte_stream[ byte_stream_index ] >= (uint8_t) 'J' )
		      && ( byte_stream[ byte_stream_index ] <= (uint8_t) 'R' ) )
		{
			url_stream[ url_stream_index++ ] = byte_stream[ byte_stream_index ];
		}
		else if( ( byte_stream[ byte_stream_index ] >= (uint8_t) 'S' )
		      && ( byte_stream[ byte_stream_index ] <= (uint8_t) 'Z' ) )
		{
			url_stream[ url_stream_index++ ] = byte_stream[ byte_stream_index ];
		}
		/* a-z is not a continous range on an EBCDIC based system
		 * it consists of the ranges: a-i, j-r, s-z
		 */
		else if( ( byte_stream[ byte_stream_index ] >= (uint8_t) 'a' )
		      && ( byte_stream[ byte_stream_index ] <= (uint8_t) 'i' ) )
		{
			url_stream[ url_stream_index++ ] = byte_stream[ byte_stream_index ];
		}
		else if( ( byte_stream[ byte_stream_index ] >= (uint8_t) 'j' )
		      && ( byte_stream[ byte_stream_index ] <= (uint8_t) 'r' ) )
		{
			url_stream[ url_stream_index++ ] = byte_stream[ byte_stream_index ];
		}
		else if( ( byte_stream[ byte_stream_index ] >= (uint8_t) 's' )
		      && ( byte_stream[ byte_stream_index ] <= (uint8_t) 'z' ) )
		{
			url_stream[ url_stream_index++ ] = byte_stream[ byte_stream_index ];
		}
		else if( ( byte_stream[ byte_stream_index ] >= (uint8_t) '0' )
		      && ( byte_stream[ byte_stream_index ] <= (uint8_t) '9' ) )
		{
			url_stream[ url_stream_index++ ] = byte_stream[ byte_stream_index ];
		}
		else if( ( byte_stream[ byte_stream_index ] == (uint8_t) '-' )
		      || ( byte_stream[ byte_stream_index ] == (uint8_t) '_' )
		      || ( byte_stream[ byte_stream_index ] == (uint8_t) '.' )
		      || ( byte_stream[ byte_stream_index ] == (uint8_t) '`' ) )
		{
			url_stream[ url_stream_index++ ] = byte_stream[ byte_stream_index ];
		}
		else
		{
			if( ( url_stream_index + 3 ) > url_stream_size )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: url stream is too small.",
				 function );

				return( -1 );
			}
			url_stream[ url_stream_index++ ] = (uint8_t) '%';

			byte_value = byte_stream[ byte_stream_index ] >> 4;

			if( byte_value <= 9 )
			{
				url_stream[ url_stream_index++ ] = (uint8_t) '0' + byte_value;
			}
			else
			{
				url_stream[ url_stream_index++ ] = (uint8_t) 'A' + byte_value;
			}
			byte_value = byte_stream[ byte_stream_index ] & 0x0f;

			if( byte_value < 9 )
			{
				url_stream[ url_stream_index++ ] = (uint8_t) '0' + byte_value;
			}
			else
			{
				url_stream[ url_stream_index++ ] = (uint8_t) 'A' + byte_value;
			}
		}
		byte_stream_index++;
	}
	return( 1 );
}

/* Determines the size of a byte stream from a url stream
 * Returns 1 if successful or -1 on error
 */
int libuna_url_stream_size_to_byte_stream(
     uint8_t *url_stream,
     size_t url_stream_size,
     size_t *byte_stream_size,
     libcerror_error_t **error )
{
	static char *function   = "libuna_url_stream_size_to_byte_stream";
	size_t url_stream_index = 0;

	if( url_stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid url stream.",
		 function );

		return( -1 );
	}
	if( url_stream_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid url stream size value exceeds maximum.",
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
	*byte_stream_size = 0;

	while( url_stream_index < url_stream_size )
	{
		if( url_stream[ url_stream_index ] == (uint8_t) '%' )
		{
			url_stream_index++;

			if( ( url_stream_index + 2 ) > url_stream_size )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: url stream is too small.",
				 function );

				return( -1 );
			}
			if( ( url_stream[ url_stream_index ] >= (uint8_t) 'A' )
			 && ( url_stream[ url_stream_index ] <= (uint8_t) 'F' ) )
			{
			}
			else if( ( url_stream[ url_stream_index ] >= (uint8_t) 'a' )
			      && ( url_stream[ url_stream_index ] <= (uint8_t) 'f' ) )
			{
			}
			else if( ( url_stream[ url_stream_index ] >= (uint8_t) '0' )
			      && ( url_stream[ url_stream_index ] <= (uint8_t) '9' ) )
			{
			}
			else
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
				 "%s: invalid URL encoded character.",
				 function );

				return( -1 );
			}
			url_stream_index++;

			if( ( url_stream[ url_stream_index ] >= (uint8_t) 'A' )
			 && ( url_stream[ url_stream_index ] <= (uint8_t) 'F' ) )
			{
			}
			else if( ( url_stream[ url_stream_index ] >= (uint8_t) 'a' )
			      && ( url_stream[ url_stream_index ] <= (uint8_t) 'f' ) )
			{
			}
			else if( ( url_stream[ url_stream_index ] >= (uint8_t) '0' )
			      && ( url_stream[ url_stream_index ] <= (uint8_t) '9' ) )
			{
			}
			else
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
				 "%s: invalid URL encoded character.",
				 function );

				return( -1 );
			}
			url_stream_index++;
		}
		else
		{
			url_stream_index++;
		}
		*byte_stream_size += 1;
	}
	return( 1 );
}

/* Copies a byte stream from a url stream
 * Returns 1 if successful or -1 on error
 */
int libuna_url_stream_copy_to_byte_stream(
     uint8_t *url_stream,
     size_t url_stream_size,
     uint8_t *byte_stream,
     size_t byte_stream_size,
     libcerror_error_t **error )
{
	static char *function    = "libuna_url_stream_copy_to_byte_stream";
	size_t url_stream_index  = 0;
	size_t byte_stream_index = 0;
	uint8_t byte_value       = 0;

	if( url_stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid url stream.",
		 function );

		return( -1 );
	}
	if( url_stream_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid url stream size value exceeds maximum.",
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
	while( url_stream_index < url_stream_size )
	{
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
		if( url_stream[ url_stream_index ] == (uint8_t) '%' )
		{
			url_stream_index++;

			if( ( url_stream_index + 2 ) > url_stream_size )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: url stream is too small.",
				 function );

				return( -1 );
			}
			if( ( url_stream[ url_stream_index ] >= (uint8_t) 'A' )
			 && ( url_stream[ url_stream_index ] <= (uint8_t) 'F' ) )
			{
				byte_value = url_stream[ url_stream_index ] - (uint8_t) 'A';
			}
			else if( ( url_stream[ url_stream_index ] >= (uint8_t) 'a' )
			      && ( url_stream[ url_stream_index ] <= (uint8_t) 'f' ) )
			{
				byte_value = url_stream[ url_stream_index ] - (uint8_t) 'a';
			}
			else if( ( url_stream[ url_stream_index ] >= (uint8_t) '0' )
			      && ( url_stream[ url_stream_index ] <= (uint8_t) '9' ) )
			{
				byte_value = url_stream[ url_stream_index ] - (uint8_t) '0';
			}
			else
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
				 "%s: invalid URL encoded character.",
				 function );

				return( -1 );
			}
			byte_value <<= 4;

			url_stream_index++;

			if( ( url_stream[ url_stream_index ] >= (uint8_t) 'A' )
			 && ( url_stream[ url_stream_index ] <= (uint8_t) 'F' ) )
			{
				byte_value += url_stream[ url_stream_index ] - (uint8_t) 'A';
			}
			else if( ( url_stream[ url_stream_index ] >= (uint8_t) 'a' )
			      && ( url_stream[ url_stream_index ] <= (uint8_t) 'f' ) )
			{
				byte_value += url_stream[ url_stream_index ] - (uint8_t) 'a';
			}
			else if( ( url_stream[ url_stream_index ] >= (uint8_t) '0' )
			      && ( url_stream[ url_stream_index ] <= (uint8_t) '9' ) )
			{
				byte_value += url_stream[ url_stream_index ] - (uint8_t) '0';
			}
			else
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
				 "%s: invalid URL encoded character.",
				 function );

				return( -1 );
			}
			url_stream_index++;

			byte_stream[ byte_stream_index++ ] = byte_value;
		}
		else
		{
			byte_stream[ byte_stream_index++ ] = url_stream[ url_stream_index++ ];
		}
	}
	return( 1 );
}

