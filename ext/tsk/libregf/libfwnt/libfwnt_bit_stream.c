/*
 * Bit stream functions
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

#include "libfwnt_bit_stream.h"
#include "libfwnt_libcerror.h"
#include "libfwnt_libcnotify.h"

/* Creates a bit stream
 * Make sure the value bit_stream is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libfwnt_bit_stream_initialize(
     libfwnt_bit_stream_t **bit_stream,
     const uint8_t *byte_stream,
     size_t byte_stream_size,
     libcerror_error_t **error )
{
	static char *function = "libfwnt_bit_stream_initialize";

	if( bit_stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid bit stream.",
		 function );

		return( -1 );
	}
	if( *bit_stream != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid bit stream value already set.",
		 function );

		return( -1 );
	}
	*bit_stream = memory_allocate_structure(
	               libfwnt_bit_stream_t );

	if( *bit_stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create bit stream.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *bit_stream,
	     0,
	     sizeof( libfwnt_bit_stream_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear bit stream.",
		 function );

		goto on_error;
	}
	( *bit_stream )->byte_stream      = byte_stream;
	( *bit_stream )->byte_stream_size = byte_stream_size;

	return( 1 );

on_error:
	if( *bit_stream != NULL )
	{
		memory_free(
		 *bit_stream );

		*bit_stream = NULL;
	}
	return( -1 );
}

/* Frees a bit stream
 * Returns 1 if successful or -1 on error
 */
int libfwnt_bit_stream_free(
     libfwnt_bit_stream_t **bit_stream,
     libcerror_error_t **error )
{
	static char *function = "libfwnt_bit_stream_free";

	if( bit_stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid bit stream.",
		 function );

		return( -1 );
	}
	if( *bit_stream != NULL )
	{
		memory_free(
		 *bit_stream );

		*bit_stream = NULL;
	}
	return( 1 );
}

/* Reads bits from the underlying byte stream
 * Returns the number of bytes read, 0 if no data available or -1 on error
 */
ssize_t libfwnt_bit_stream_read(
         libfwnt_bit_stream_t *bit_stream,
         size_t read_size,
         libcerror_error_t **error )
{
	uint32_t bits          = 0;
	static char *function  = "libfwnt_bit_stream_read";
	uint8_t number_of_bits = 0;

	if( bit_stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid bit stream.",
		 function );

		return( -1 );
	}
	if( read_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid read size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( ( read_size != 2 )
	 && ( read_size != 4 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: read size value out of bounds.",
		 function );

		return( -1 );
	}
	if( bit_stream->byte_stream_offset >= bit_stream->byte_stream_size )
	{
		return( 0 );
	}
	if( ( bit_stream->byte_stream_offset + read_size ) > bit_stream->byte_stream_size )
	{
		read_size = bit_stream->byte_stream_size - bit_stream->byte_stream_offset;
	}
	if( read_size == 0 )
	{
		return( 0 );
	}
	number_of_bits = (uint8_t) ( read_size << 3 );

	if( ( number_of_bits + bit_stream->number_of_bits ) > 32 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: number of bits value out of bounds.",
		 function );

		return( -1 );
	}
/* TODO optimize the read for the maximum number of available bytes. ? */
/* TODO what about handling intermediate read size values ? */

	while( number_of_bits >= 16 )
	{
		bit_stream->bits <<= 16;

		byte_stream_copy_to_uint16_little_endian(
		 &( bit_stream->byte_stream[ bit_stream->byte_stream_offset ] ),
		 bits );

		bit_stream->bits               |= bits;
		bit_stream->byte_stream_offset += 2;
		bit_stream->number_of_bits     += 16;

		number_of_bits -= 16;
	}
	return( (ssize_t) read_size );
}

