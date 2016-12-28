/*
 * LZNT1 (de)compression functions
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

#include "libfwnt_libcerror.h"
#include "libfwnt_libcnotify.h"
#include "libfwnt_lznt1.h"

/* Compresses data using LZNT1 compression
 * Returns 1 on success or -1 on error
 */
int libfwnt_lznt1_compress(
     const uint8_t *uncompressed_data,
     size_t uncompressed_data_size,
     uint8_t *compressed_data,
     size_t *compressed_data_size,
     libcerror_error_t **error )
{
	static char *function = "libfwnt_lznt1_compress";

	if( uncompressed_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid uncompressed data.",
		 function );

		return( -1 );
	}
	if( uncompressed_data_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid uncompressed data size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( compressed_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid compressed data.",
		 function );

		return( -1 );
	}
	if( compressed_data_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid compressed data size.",
		 function );

		return( -1 );
	}
/* TODO implement */
	return( -1 );
}

/* Decompresses a LZNT1 compressed chunk
 * Returns 1 on success or -1 on error
 */
int libfwnt_lznt1_decompress_chunk(
     const uint8_t *compressed_data,
     size_t compressed_data_size,
     size_t *compressed_data_offset,
     size_t compression_chunk_size,
     uint8_t *uncompressed_data,
     size_t *uncompressed_data_size,
     libcerror_error_t **error )
{
	static char *function                   = "libfwnt_lznt1_decompress_chunk";
	size_t compression_tuple_index          = 0;
	size_t compression_tuple_threshold      = 0;
	size_t uncompressed_data_offset         = 0;
	uint16_t compression_tuple              = 0;
	uint16_t compression_tuple_offset_shift = 0;
	uint16_t compression_tuple_size         = 0;
	uint16_t compression_tuple_size_mask    = 0;
	int16_t compression_tuple_offset        = 0;
	uint8_t compression_flag_bit_index      = 0;
	uint8_t compression_flag_byte           = 0;

	if( compressed_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid compressed data.",
		 function );

		return( -1 );
	}
	if( compressed_data_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid compressed data size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( compressed_data_offset == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid compressed data offset.",
		 function );

		return( -1 );
	}
	if( *compressed_data_offset > compressed_data_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid compressed data offset value out of bounds.",
		 function );

		return( -1 );
	}
	if( compression_chunk_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid compression chunk size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( ( *compressed_data_offset + compression_chunk_size ) > compressed_data_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: compressed data too small.",
		 function );

		return( -1 );
	}
	if( uncompressed_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid uncompressed data.",
		 function );

		return( -1 );
	}
	if( uncompressed_data_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid uncompressed data size.",
		 function );

		return( -1 );
	}
	compression_tuple_threshold    = 16;
	compression_tuple_offset_shift = 12;
	compression_tuple_size_mask    = 0x0fff;

	while( compression_chunk_size > 0 )
	{
		compression_flag_byte = compressed_data[ *compressed_data_offset ];

#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: compressed data offset\t\t\t: %" PRIzd " (0x%08" PRIzx ")\n",
			 function,
			 *compressed_data_offset,
			 *compressed_data_offset );

			libcnotify_printf(
			 "%s: compression flag byte\t\t\t: 0x%02" PRIx8 "\n",
			 function,
			 compression_flag_byte );

			libcnotify_printf(
			 "\n" );
		}
#endif
		*compressed_data_offset += 1;
		compression_chunk_size -= 1;

		for( compression_flag_bit_index = 0;
		     compression_flag_bit_index < 8;
		     compression_flag_bit_index++ )
		{
			/* Check if the data is compressed (tag bit is set)
			 */
			if( ( compression_flag_byte & 0x01 ) != 0 )
			{
				/* Read the compression ( size, offset ) tuple
				 */
				byte_stream_copy_to_uint16_little_endian(
				 &( compressed_data[ *compressed_data_offset ] ),
				 compression_tuple );

				compression_tuple_offset = ( compression_tuple >> compression_tuple_offset_shift ) + 1;
				compression_tuple_size   = ( compression_tuple & compression_tuple_size_mask ) + 3;

#if defined( HAVE_DEBUG_OUTPUT )
				if( libcnotify_verbose != 0 )
				{
					libcnotify_printf(
					 "%s: compressed data offset\t\t\t: %" PRIzd " (0x%08" PRIzx ")\n",
					 function,
					 *compressed_data_offset,
					 *compressed_data_offset );

					libcnotify_printf(
					 "%s: compression tuple\t\t\t: 0x%04" PRIx16 " (shift: %" PRIu16 ", mask: 0x%04" PRIx16 ")\n",
					 function,
					 compression_tuple,
					 compression_tuple_offset_shift,
					 compression_tuple_size_mask );

					libcnotify_printf(
					 "%s: compression tuple offset\t\t: %" PRIi16 "\n",
					 function,
					 compression_tuple_offset );

					libcnotify_printf(
					 "%s: compression tuple size\t\t\t: %" PRIu16 "\n",
					 function,
					 compression_tuple_size );

					libcnotify_printf(
					 "%s: uncompressed data offset\t\t: %" PRIzd "\n",
					 function,
					 uncompressed_data_offset );

					libcnotify_printf(
					 "\n" );
				}
#endif
				*compressed_data_offset += 2;
				compression_chunk_size -= 2;

				/* The compression tuple offset refers to an offset in the uncompressed data
				 */
				if( (size_t) compression_tuple_offset > uncompressed_data_offset )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
					 "%s: compression tuple offset value out of bounds.",
					 function );

					return( -1 );
				}
				compression_tuple_index = uncompressed_data_offset - compression_tuple_offset;

				while( compression_tuple_size > 0 )
				{
					if( compression_tuple_index > uncompressed_data_offset )
					{
						libcerror_error_set(
						 error,
						 LIBCERROR_ERROR_DOMAIN_RUNTIME,
						 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
						 "%s: compression tuple index value out of bounds.",
						 function );

						return( -1 );
					}
					if( uncompressed_data_offset > *uncompressed_data_size )
					{
						libcerror_error_set(
						 error,
						 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
						 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
						 "%s: uncompressed data too small.",
						 function );

						return( -1 );
					}
					uncompressed_data[ uncompressed_data_offset++ ] = uncompressed_data[ compression_tuple_index++ ];

					compression_tuple_size -= 1;
				}
			}
			else
			{
				if( uncompressed_data_offset > *uncompressed_data_size )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
					 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
					 "%s: uncompressed data too small.",
					 function );

					return( -1 );
				}
				uncompressed_data[ uncompressed_data_offset++ ] = compressed_data[ *compressed_data_offset ];

				*compressed_data_offset += 1;
				compression_chunk_size -= 1;
			}
			compression_flag_byte >>= 1;

			if( compression_chunk_size == 0 )
			{
				break;
			}
			/* The compression tuple size mask and offset shift
			 * are dependent on the current buffer offset in the uncompressed data
			 */
			while( uncompressed_data_offset > compression_tuple_threshold )
			{
				compression_tuple_offset_shift -= 1;
				compression_tuple_size_mask   >>= 1;
				compression_tuple_threshold   <<= 1;
			}
		}
	}
	*uncompressed_data_size = uncompressed_data_offset;

	return( 1 );
}

/* Decompresses data using LZNT1 compression
 * Returns 1 on success or -1 on error
 */
int libfwnt_lznt1_decompress(
     const uint8_t *compressed_data,
     size_t compressed_data_size,
     uint8_t *uncompressed_data,
     size_t *uncompressed_data_size,
     libcerror_error_t **error )
{
	static char *function             = "libfwnt_lznt1_decompress";
	size_t compressed_data_offset     = 0;
	size_t uncompressed_chunk_size    = 0;
	size_t uncompressed_data_offset   = 0;
	uint16_t compression_chunk_header = 0;
	uint16_t compression_chunk_size   = 0;

	if( compressed_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid compressed data.",
		 function );

		return( -1 );
	}
	if( compressed_data_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid compressed data size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( uncompressed_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid uncompressed data.",
		 function );

		return( -1 );
	}
	if( uncompressed_data_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid uncompressed data size.",
		 function );

		return( -1 );
	}
	while( compressed_data_offset < compressed_data_size )
	{
		if( uncompressed_data_offset >= *uncompressed_data_size )
		{
			break;
		}
		if( ( compressed_data_offset + 1 ) >= compressed_data_size )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
			 "%s: compressed data size value too small.",
			 function );

			return( -1 );
		}
		/* The first 2 bytes contain the compressed chunk header
		 * 0  - 11	compressed chunk size
		 * 12 - 14	signature value
		 * 15		is compressed flag
		 */
		byte_stream_copy_to_uint16_little_endian(
		 &( compressed_data[ compressed_data_offset ] ),
		 compression_chunk_header );

#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: compressed data offset\t\t\t\t: %" PRIzd " (0x%08" PRIzx ")\n",
			 function,
			 compressed_data_offset,
			 compressed_data_offset );

			libcnotify_printf(
			 "%s: compression chunk header\t\t\t: 0x%04" PRIx16 "\n",
			 function,
			 compression_chunk_header );

			libcnotify_printf(
			 "%s: compressed chunk size\t\t\t\t: %" PRIu16 "\n",
			 function,
			 ( compression_chunk_header & 0x0fff ) + 1 );

			libcnotify_printf(
			 "%s: signature value\t\t\t\t: %" PRIu16 "\n",
			 function,
			 ( compression_chunk_header >> 12 ) & 0x0007 );

			libcnotify_printf(
			 "%s: is compressed flag\t\t\t\t: %" PRIu16 "\n",
			 function,
			 compression_chunk_header >> 15 );

			libcnotify_printf(
			 "\n" );
		}
#endif
		compressed_data_offset += 2;

		if( compression_chunk_header == 0 )
		{
			break;
		}
		compression_chunk_size = ( compression_chunk_header & 0x0fff ) + 1;

		if( ( compression_chunk_header & 0x8000 ) != 0 )
		{
			/* Adjust the compression chunk size for the iteration
			 */
			uncompressed_chunk_size = *uncompressed_data_size - uncompressed_data_offset;

			if( libfwnt_lznt1_decompress_chunk(
			     compressed_data,
			     compressed_data_size,
			     &compressed_data_offset,
			     compression_chunk_size,
			     &( uncompressed_data[ uncompressed_data_offset ] ),
			     &uncompressed_chunk_size,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_COMPRESSION,
				 LIBCERROR_COMPRESSION_ERROR_DECOMPRESS_FAILED,
				 "%s: unable to decompress chunk.",
				 function );

				return( -1 );
			}
			uncompressed_data_offset += uncompressed_chunk_size;
		}
		else
		{
			if( ( compressed_data_offset + compression_chunk_size ) > compressed_data_size )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: compressed data too small.",
				 function );

				return( -1 );
			}
			if( ( uncompressed_data_offset + compression_chunk_size ) > *uncompressed_data_size )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
				 "%s: uncompressed data too small.",
				 function );

				return( -1 );
			}
			if( memory_copy(
			     &( uncompressed_data[ uncompressed_data_offset ] ),
			     &( compressed_data[ compressed_data_offset ] ),
			     compression_chunk_size ) == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_MEMORY,
				 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
				 "%s: unable to copy copy compressed data to uncompressed data.",
				 function );

				return( -1 );
			}
			compressed_data_offset   += (size_t) compression_chunk_size;
			uncompressed_data_offset += (size_t) compression_chunk_size;
		}
	}
	*uncompressed_data_size = uncompressed_data_offset;

	return( 1 );
}

