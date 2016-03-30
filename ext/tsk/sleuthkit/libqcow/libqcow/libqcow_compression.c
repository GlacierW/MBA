/*
 * Compression functions
 *
 * Copyright (c) 2010, Joachim Metz <joachim.metz@gmail.com>
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
#include <file_stream.h>
#include <memory.h>
#include <types.h>

#if defined( HAVE_STDLIB_H ) || defined( WINAPI )
#include <stdlib.h>
#endif

#if defined( HAVE_ZLIB ) || defined( ZLIB_DLL )
#include <zlib.h>
#endif

#include "libqcow_compression.h"
#include "libqcow_definitions.h"
#include "libqcow_deflate.h"
#include "libqcow_libcerror.h"
#include "libqcow_libcnotify.h"

/* Decompresses data using the compression method
 * Returns 1 on success, 0 on failure or -1 on error
 */
int libqcow_decompress_data(
     const uint8_t *compressed_data,
     size_t compressed_data_size,
     uint16_t compression_method,
     uint8_t *uncompressed_data,
     size_t *uncompressed_data_size,
     libcerror_error_t **error )
{
	static char *function              = "libqcow_decompress_data";
	int result                         = 0;

#if ( defined( HAVE_ZLIB ) && defined( HAVE_ZLIB_INFLATE ) ) || defined( ZLIB_DLL )
	z_stream zlib_stream;
#endif

	if( compressed_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid compressed data buffer.",
		 function );

		return( -1 );
	}
	if( uncompressed_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid uncompressed data buffer.",
		 function );

		return( -1 );
	}
	if( uncompressed_data == compressed_data )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid compressed data buffer equals uncompressed data buffer.",
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
	if( compression_method == LIBQCOW_COMPRESSION_METHOD_DEFLATE )
	{
#if ( defined( HAVE_ZLIB ) && defined( HAVE_ZLIB_INFLATE ) ) || defined( ZLIB_DLL )
		if( compressed_data_size > (size_t) ULONG_MAX )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
			 "%s: invalid compressed data size value exceeds maximum.",
			 function );

			return( -1 );
		}
		if( *uncompressed_data_size > (size_t) ULONG_MAX )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
			 "%s: invalid uncompressed data size value exceeds maximum.",
			 function );

			return( -1 );
		}
		if( memory_set(
		     &zlib_stream,
		     0,
		     sizeof( z_stream ) ) == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_MEMORY,
			 LIBCERROR_MEMORY_ERROR_SET_FAILED,
			 "%s: unable to clear zlib stream.",
			 function );

			return( -1 );
		}
		zlib_stream.next_in   = (Bytef *) compressed_data;
		zlib_stream.avail_in  = (uInt) compressed_data_size;
		zlib_stream.next_out  = (Bytef *) uncompressed_data;
		zlib_stream.avail_out = (uInt) *uncompressed_data_size;

#if defined( HAVE_ZLIB_INFLATE_INIT2 ) || defined( ZLIB_DLL )
		result = inflateInit2(
		          &zlib_stream,
		          -12 );
#else
		result = _inflateInit2(
		          &zlib_stream,
		          -12 );
#endif
		if( result != Z_OK )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to initialize zlib stream.",
			 function );

			return( -1 );
		}
		result = inflate(
		          &zlib_stream,
		          Z_FINISH );

		if( result == Z_STREAM_END )
		{
			*uncompressed_data_size = (size_t) zlib_stream.total_out;

			result = 1;
		}
		else if( result == Z_DATA_ERROR )
		{
#if defined( HAVE_DEBUG_OUTPUT )
			if( libcnotify_verbose != 0 )
			{
				libcnotify_printf(
				 "%s: unable to read compressed data: data error.\n",
				 function );
			}
#endif
			*uncompressed_data_size = 0;

			result = -1;
		}
		else if( result == Z_BUF_ERROR )
		{
#if defined( HAVE_DEBUG_OUTPUT )
			if( libcnotify_verbose != 0 )
			{
				libcnotify_printf(
				"%s: unable to read compressed data: target buffer too small.\n",
				 function );
			}
#endif
			/* Estimate that a factor 2 enlargement should suffice
			 */
			*uncompressed_data_size *= 2;

			result = 0;
		}
		else if( result == Z_MEM_ERROR )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_MEMORY,
			 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to read compressed data: insufficient memory.",
			 function );

			*uncompressed_data_size = 0;

			result = -1;
		}
		else
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_COMPRESSION,
			 LIBCERROR_COMPRESSION_ERROR_DECOMPRESS_FAILED,
			 "%s: zlib returned undefined error: %d.",
			 function,
			 result );

			*uncompressed_data_size = 0;

			result = -1;
		}
		if( result == -1 )
		{
			inflateEnd(
			 &zlib_stream );
		}
		else
		{
			if( inflateEnd(
			     &zlib_stream ) != Z_OK )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: unable to finalize zlib stream.",
				 function );

				return( -1 );
			}
		}
#else
		result = libqcow_deflate_decompress(
		          compressed_data,
		          compressed_data_size,
		          uncompressed_data,
		          uncompressed_data_size,
		          error );

		if( result != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ENCRYPTION,
			 LIBCERROR_ENCRYPTION_ERROR_GENERIC,
			 "%s: unable to decompress deflate compressed data.",
			 function );

			return( -1 );
		}
#endif /* ( defined( HAVE_ZLIB ) && defined( HAVE_ZLIB_INFLATE ) ) || defined( ZLIB_DLL ) */
	}
	else
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported compression method.",
		 function );

		return( -1 );
	}
	return( result );
}

