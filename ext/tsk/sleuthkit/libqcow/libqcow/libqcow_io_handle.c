/*
 * Input/Output (IO) handle functions
 *
 * Copyright (C) 2010-2016, Joachim Metz <joachim.metz@gmail.com>
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

#include "libqcow_cluster_block.h"
#include "libqcow_cluster_table.h"
#include "libqcow_encryption.h"
#include "libqcow_io_handle.h"
#include "libqcow_libbfio.h"
#include "libqcow_libcerror.h"
#include "libqcow_libcnotify.h"
#include "libqcow_libfcache.h"
#include "libqcow_libfdata.h"
#include "libqcow_unused.h"

#include "qcow_file_header.h"

const uint8_t qcow_file_signature[ 4 ] = { 0x51, 0x46, 0x49, 0xfb };

/* Creates an IO handle
 * Make sure the value io_handle is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libqcow_io_handle_initialize(
     libqcow_io_handle_t **io_handle,
     libcerror_error_t **error )
{
	static char *function = "libqcow_io_handle_initialize";

	if( io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid IO handle.",
		 function );

		return( -1 );
	}
	if( *io_handle != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid IO handle value already set.",
		 function );

		return( -1 );
	}
	*io_handle = memory_allocate_structure(
	              libqcow_io_handle_t );

	if( *io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create IO handle.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *io_handle,
	     0,
	     sizeof( libqcow_io_handle_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear IO handle.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( *io_handle != NULL )
	{
		memory_free(
		 *io_handle );

		*io_handle = NULL;
	}
	return( -1 );
}

/* Frees an IO handle
 * Returns 1 if successful or -1 on error
 */
int libqcow_io_handle_free(
     libqcow_io_handle_t **io_handle,
     libcerror_error_t **error )
{
	static char *function = "libqcow_io_handle_free";
	int result            = 1;

	if( io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid IO handle.",
		 function );

		return( -1 );
	}
	if( *io_handle != NULL )
	{
		if( libqcow_io_handle_clear(
		     *io_handle,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to clear IO handle.",
			 function );

			result = -1;
		}
		memory_free(
		 *io_handle );

		*io_handle = NULL;
	}
	return( result );
}

/* Clears the IO handle
 * Returns 1 if successful or -1 on error
 */
int libqcow_io_handle_clear(
     libqcow_io_handle_t *io_handle,
     libcerror_error_t **error )
{
	static char *function = "libqcow_io_handle_clear";

	if( io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid IO handle.",
		 function );

		return( -1 );
	}
	if( io_handle->backing_filename != NULL )
	{
		memory_free(
		 io_handle->backing_filename );
	}
	if( memory_set(
	     io_handle,
	     0,
	     sizeof( libqcow_io_handle_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear IO handle.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Reads the file header
 * Returns 1 if successful or -1 on error
 */
int libqcow_io_handle_read_file_header(
     libqcow_io_handle_t *io_handle,
     libbfio_handle_t *file_io_handle,
     uint32_t *encryption_method,
     libcerror_error_t **error )
{
	uint8_t *file_header_data                  = NULL;
	static char *function                      = "libqcow_io_handle_read_file_header";
	size_t read_size                           = 512;
	ssize_t read_count                         = 0;
	uint64_t backing_filename_offset           = 0;
	uint32_t number_of_level1_table_references = 0;

#if defined( HAVE_DEBUG_OUTPUT )
	uint64_t value_64bit                       = 0;
	uint32_t value_32bit                       = 0;
	uint16_t value_16bit                       = 0;
#endif

	if( io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid IO handle.",
		 function );

		return( -1 );
	}
	if( io_handle->backing_filename != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid IO handle - backing filename value already set.",
		 function );

		return( -1 );
	}
	if( encryption_method == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid encryption method.",
		 function );

		return( -1 );
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: reading file header at offset: 0 (0x00000000)\n",
		 function );
	}
#endif
	if( libbfio_handle_seek_offset(
	     file_io_handle,
	     0,
	     SEEK_SET,
	     error ) == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_SEEK_FAILED,
		 "%s: unable to seek file header offset: 0.",
		 function );

		goto on_error;
	}
	file_header_data = (uint8_t *) memory_allocate(
	                                sizeof( uint8_t ) * read_size );

	if( file_header_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create file header data.",
		 function );

		goto on_error;
	}
	read_count = libbfio_handle_read_buffer(
	              file_io_handle,
	              file_header_data,
	              read_size,
	              error );

	if( read_count != (ssize_t) read_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read file header.",
		 function );

		goto on_error;
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: file header data:\n",
		 function );
		libcnotify_print_data(
		 file_header_data,
		 sizeof( qcow_file_header_v1_t ),
		 0 );
	}
#endif
	if( memory_compare(
	     ( (qcow_file_header_v1_t *) file_header_data )->signature,
	     qcow_file_signature,
	     4 ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported file signature.",
		 function );

		goto on_error;
	}
	byte_stream_copy_to_uint32_big_endian(
	 ( (qcow_file_header_v1_t *) file_header_data )->format_version,
	 io_handle->format_version );

	byte_stream_copy_to_uint64_big_endian(
	 ( (qcow_file_header_v1_t *) file_header_data )->backing_filename_offset,
	 backing_filename_offset );

	byte_stream_copy_to_uint32_big_endian(
	 ( (qcow_file_header_v1_t *) file_header_data )->backing_filename_size,
	 io_handle->backing_filename_size );

#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		byte_stream_copy_to_uint32_big_endian(
		 ( (qcow_file_header_v1_t *) file_header_data )->signature,
		 value_32bit );
		libcnotify_printf(
		 "%s: signature\t\t\t\t: 0x%08" PRIx32 "\n",
		 function,
		 value_32bit );

		libcnotify_printf(
		 "%s: format version\t\t\t: %" PRIu32 "\n",
		 function,
		 io_handle->format_version );

		libcnotify_printf(
		 "%s: backing filename offset\t\t: %" PRIu32 "\n",
		 function,
		 backing_filename_offset );

		libcnotify_printf(
		 "%s: backing filename size\t\t: %" PRIu32 "\n",
		 function,
		 io_handle->backing_filename_size );
	}
#endif
	if( ( io_handle->format_version != 1 )
	 && ( io_handle->format_version != 2 )
	 && ( io_handle->format_version != 3 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported format version: %" PRIu32 ".",
		 function,
		 io_handle->format_version );

		goto on_error;
	}
	if( io_handle->format_version == 1 )
	{
		byte_stream_copy_to_uint64_big_endian(
		 ( (qcow_file_header_v1_t *) file_header_data )->media_size,
		 io_handle->media_size );

		io_handle->number_of_cluster_block_bits = (uint32_t) ( (qcow_file_header_v1_t *) file_header_data )->number_of_cluster_block_bits;

		io_handle->number_of_level2_table_bits = (uint32_t) ( (qcow_file_header_v1_t *) file_header_data )->number_of_level2_table_bits;

		byte_stream_copy_to_uint32_big_endian(
		 ( (qcow_file_header_v1_t *) file_header_data )->encryption_method,
		 *encryption_method );

		byte_stream_copy_to_uint64_big_endian(
		 ( (qcow_file_header_v2_t *) file_header_data )->level1_table_offset,
		 io_handle->level1_table_offset );

#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			byte_stream_copy_to_uint32_big_endian(
			 ( (qcow_file_header_v1_t *) file_header_data )->modification_time,
			 value_32bit );
			libcnotify_printf(
			 "%s: modification time\t\t\t: %" PRIu32 "\n",
			 function,
			 value_32bit );

			libcnotify_printf(
			 "%s: media size\t\t\t\t: %" PRIu64 "\n",
			 function,
			 io_handle->media_size );

			libcnotify_printf(
			 "%s: number of cluster block bits\t: %" PRIu32 "\n",
			 function,
			 io_handle->number_of_cluster_block_bits );

			libcnotify_printf(
			 "%s: number of level 2 table bits\t: %" PRIu32 "\n",
			 function,
			 io_handle->number_of_level2_table_bits );

			byte_stream_copy_to_uint16_big_endian(
			 ( (qcow_file_header_v1_t *) file_header_data )->unknown1,
			 value_16bit );
			libcnotify_printf(
			 "%s: unknown1\t\t\t\t: 0x%04" PRIx16 "\n",
			 function,
			 value_16bit );

			libcnotify_printf(
			 "%s: encryption method\t\t\t: %" PRIu32 "\n",
			 function,
			 *encryption_method );

			libcnotify_printf(
			 "%s: level 1 table offset\t\t: 0x%08" PRIx64 "\n",
			 function,
			 io_handle->level1_table_offset );
		}
#endif
	}
	else if( ( io_handle->format_version == 2 )
	      || ( io_handle->format_version == 3 ) )
	{
		byte_stream_copy_to_uint32_big_endian(
		 ( (qcow_file_header_v2_t *) file_header_data )->number_of_cluster_block_bits,
		 io_handle->number_of_cluster_block_bits );

		byte_stream_copy_to_uint32_big_endian(
		 ( (qcow_file_header_v2_t *) file_header_data )->number_of_level1_table_references,
		 number_of_level1_table_references );

		byte_stream_copy_to_uint64_big_endian(
		 ( (qcow_file_header_v2_t *) file_header_data )->level1_table_offset,
		 io_handle->level1_table_offset );

		byte_stream_copy_to_uint64_big_endian(
		 ( (qcow_file_header_v2_t *) file_header_data )->media_size,
		 io_handle->media_size );

		byte_stream_copy_to_uint32_big_endian(
		 ( (qcow_file_header_v2_t *) file_header_data )->encryption_method,
		 *encryption_method );

#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: number of cluster block bits\t: %" PRIu32 "\n",
			 function,
			 io_handle->number_of_cluster_block_bits );

			libcnotify_printf(
			 "%s: media size\t\t\t\t: %" PRIu64 "\n",
			 function,
			 io_handle->media_size );

			libcnotify_printf(
			 "%s: encryption method\t\t\t: %" PRIu32 "\n",
			 function,
			 *encryption_method );

			libcnotify_printf(
			 "%s: number of level 1 table references\t: %" PRIu32 "\n",
			 function,
			 number_of_level1_table_references );

			libcnotify_printf(
			 "%s: level 1 table offset\t\t: 0x%08" PRIx64 "\n",
			 function,
			 io_handle->level1_table_offset );

			byte_stream_copy_to_uint64_big_endian(
			 ( (qcow_file_header_v2_t *) file_header_data )->reference_count_table_offset,
			 value_64bit );
			libcnotify_printf(
			 "%s: reference count table offset\t: 0x%08" PRIx64 "\n",
			 function,
			 value_64bit );

			byte_stream_copy_to_uint32_big_endian(
			 ( (qcow_file_header_v2_t *) file_header_data )->reference_count_table_clusters,
			 value_32bit );
			libcnotify_printf(
			 "%s: reference count table clusters\t: %" PRIu32 "\n",
			 function,
			 value_32bit );

			byte_stream_copy_to_uint32_big_endian(
			 ( (qcow_file_header_v2_t *) file_header_data )->number_of_snapshots,
			 value_32bit );
			libcnotify_printf(
			 "%s: number of snapshots\t\t\t: %" PRIu32 "\n",
			 function,
			 value_32bit );

			byte_stream_copy_to_uint64_big_endian(
			 ( (qcow_file_header_v2_t *) file_header_data )->snapshots_offset,
			 value_64bit );
			libcnotify_printf(
			 "%s: snapshots offset\t\t\t: 0x%08" PRIx64 "\n",
			 function,
			 value_64bit );
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
	memory_free(
	 file_header_data );

	file_header_data = NULL;

	if( io_handle->format_version == 1 )
	{
		io_handle->offset_bit_mask           = 0x7fffffffffffffffULL;
		io_handle->compression_flag_bit_mask = (uint64_t) 1 << 63;
		io_handle->compression_bit_shift     = 63 - io_handle->number_of_cluster_block_bits;
	}
	else if( ( io_handle->format_version == 2 )
	      || ( io_handle->format_version == 3 ) )
	{
		if( io_handle->number_of_cluster_block_bits <= 8 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: invalid number of cluster block bits value out of bounds.",
			 function );

			goto on_error;
		}
		io_handle->number_of_level2_table_bits = io_handle->number_of_cluster_block_bits - 3;
		io_handle->offset_bit_mask             = 0x3fffffffffffffffULL;
		io_handle->compression_flag_bit_mask   = (uint64_t) 1 << 62;
		io_handle->compression_bit_shift       = 62 - ( io_handle->number_of_cluster_block_bits - 8 );
	}
	io_handle->level1_index_bit_shift = io_handle->number_of_cluster_block_bits
	                                  + io_handle->number_of_level2_table_bits;

	io_handle->level2_index_bit_mask  = ~( (uint64_t) -1 << io_handle->number_of_level2_table_bits );
	io_handle->cluster_block_bit_mask = ~( (uint64_t) -1 << io_handle->number_of_cluster_block_bits );
	io_handle->compression_bit_mask   = ~( (uint64_t) -1 << io_handle->compression_bit_shift );
	io_handle->level2_table_size      = (size_t) 1 << io_handle->number_of_level2_table_bits;
	io_handle->cluster_block_size     = (size_t) 1 << io_handle->number_of_cluster_block_bits;

	if( io_handle->format_version == 1 )
	{
		io_handle->level1_table_size = (uint32_t) ( io_handle->cluster_block_size * io_handle->level2_table_size );

		if( ( io_handle->media_size % io_handle->level1_table_size ) != 0 )
		{
			io_handle->level1_table_size = (uint32_t) ( ( io_handle->media_size / io_handle->level1_table_size ) + 1 );
		}
		else
		{
			io_handle->level1_table_size = (uint32_t) ( io_handle->media_size / io_handle->level1_table_size );
		}
	}
	else if( ( io_handle->format_version == 2 )
	      || ( io_handle->format_version == 3 ) )
	{
		io_handle->level1_table_size = number_of_level1_table_references;
	}
	io_handle->level1_table_size *= 8;
	io_handle->level2_table_size *= 8;

#if UINT32_MAX > SSIZE_MAX
	if( io_handle->level1_table_size > (uint32_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid level 1 table size value exceeds maximum.",
		 function );

		goto on_error;
	}
#endif
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: level 1 table size\t\t\t: %" PRIzd "\n",
		 function,
		 io_handle->level1_table_size );

		libcnotify_printf(
		 "%s: level 2 table size\t\t\t: %" PRIzd "\n",
		 function,
		 io_handle->level2_table_size );

		libcnotify_printf(
		 "%s: cluster block size\t\t\t: %" PRIzd "\n",
		 function,
		 io_handle->cluster_block_size );
	}
#endif
	if( ( backing_filename_offset > 0 )
	 && ( io_handle->backing_filename_size > 0 ) )
	{
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: reading backing filename at offset: %" PRIu64 " (0x%08" PRIx64 ")\n",
			 function,
			 backing_filename_offset,
			 backing_filename_offset );
		}
#endif
		if( libbfio_handle_seek_offset(
		     file_io_handle,
		     backing_filename_offset,
		     SEEK_SET,
		     error ) == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_SEEK_FAILED,
			 "%s: unable to seek file header offset: %" PRIu64 ".",
			 function,
			 backing_filename_offset );

			goto on_error;
		}
		io_handle->backing_filename = (uint8_t *) memory_allocate(
		                                           sizeof( uint8_t ) * io_handle->backing_filename_size );

		if( io_handle->backing_filename == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_MEMORY,
			 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to create backing filename.",
			 function );

			goto on_error;
		}
		read_count = libbfio_handle_read_buffer(
			      file_io_handle,
			      io_handle->backing_filename,
			      io_handle->backing_filename_size,
			      error );

		if( read_count != (ssize_t) io_handle->backing_filename_size )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_READ_FAILED,
			 "%s: unable to read file header.",
			 function );

			goto on_error;
		}
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: backing filename data:\n",
			 function );
			libcnotify_print_data(
			 io_handle->backing_filename,
			 (size_t) io_handle->backing_filename_size,
			 0 );
		}
#endif
	}
	return( 1 );

on_error:
	if( io_handle->backing_filename != NULL )
	{
		memory_free(
		 io_handle->backing_filename );

		io_handle->backing_filename      = NULL;
		io_handle->backing_filename_size = 0;
	}
	if( file_header_data != NULL )
	{
		memory_free(
		 file_header_data );
	}
	return( -1 );
}

/* Reads a level 2 table
 * Callback function for the level 2 table vector
 * Returns 1 if successful or -1 on error
 */
int libqcow_io_handle_read_level2_table(
     intptr_t *data_handle LIBQCOW_ATTRIBUTE_UNUSED,
     libbfio_handle_t *file_io_handle,
     libfdata_vector_t *vector,
     libfcache_cache_t *cache,
     int element_index,
     int element_data_file_index LIBQCOW_ATTRIBUTE_UNUSED,
     off64_t element_data_offset,
     size64_t element_data_size,
     uint32_t element_data_flags LIBQCOW_ATTRIBUTE_UNUSED,
     uint8_t read_flags LIBQCOW_ATTRIBUTE_UNUSED,
     libcerror_error_t **error )
{
	libqcow_cluster_table_t *level2_table = NULL;
	static char *function                 = "libqcow_io_handle_read_level2_table";

	LIBQCOW_UNREFERENCED_PARAMETER( data_handle );
	LIBQCOW_UNREFERENCED_PARAMETER( element_data_file_index );
	LIBQCOW_UNREFERENCED_PARAMETER( element_data_flags );
	LIBQCOW_UNREFERENCED_PARAMETER( read_flags );

	if( element_data_size > (size64_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid element data size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( libqcow_cluster_table_initialize(
	     &level2_table,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create level 2 table.",
		 function );

		goto on_error;
	}
	if( libqcow_cluster_table_read(
	     level2_table,
	     file_io_handle,
	     element_data_offset,
	     (size_t) element_data_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read level 2 table.",
		 function );

		goto on_error;
	}
	if( libfdata_vector_set_element_value_by_index(
	     vector,
	     (intptr_t *) file_io_handle,
	     cache,
	     element_index,
	     (intptr_t *) level2_table,
	     (int (*)(intptr_t **, libcerror_error_t **)) &libqcow_cluster_table_free,
	     LIBFDATA_VECTOR_ELEMENT_VALUE_FLAG_MANAGED,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set level 2 table as element value.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( level2_table != NULL )
	{
		libqcow_cluster_table_free(
		 &level2_table,
		 NULL );
	}
	return( -1 );
}

/* Reads a cluster block
 * Callback function for the cluster block vector
 * Returns 1 if successful or -1 on error
 */
int libqcow_io_handle_read_cluster_block(
     intptr_t *data_handle LIBQCOW_ATTRIBUTE_UNUSED,
     libbfio_handle_t *file_io_handle,
     libfdata_vector_t *vector,
     libfcache_cache_t *cache,
     int element_index,
     int element_data_file_index LIBQCOW_ATTRIBUTE_UNUSED,
     off64_t element_data_offset,
     size64_t element_data_size,
     uint32_t element_data_flags LIBQCOW_ATTRIBUTE_UNUSED,
     uint8_t read_flags LIBQCOW_ATTRIBUTE_UNUSED,
     libcerror_error_t **error )
{
	libqcow_cluster_block_t *cluster_block = NULL;
	static char *function                  = "libqcow_io_handle_read_cluster_block";

	LIBQCOW_UNREFERENCED_PARAMETER( data_handle );
	LIBQCOW_UNREFERENCED_PARAMETER( element_data_file_index );
	LIBQCOW_UNREFERENCED_PARAMETER( element_data_flags );
	LIBQCOW_UNREFERENCED_PARAMETER( read_flags );

	if( element_data_size > (size64_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid element data size value exceeds maximum.",
		 function );

		goto on_error;
	}
	if( libqcow_cluster_block_initialize(
	     &cluster_block,
	     (size_t) element_data_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create cluster block.",
		 function );

		goto on_error;
	}
	if( libqcow_cluster_block_read(
	     cluster_block,
	     file_io_handle,
             element_data_offset,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read cluster block.",
		 function );

		goto on_error;
	}
	if( libfdata_vector_set_element_value_by_index(
	     vector,
	     (intptr_t *) file_io_handle,
	     cache,
	     element_index,
	     (intptr_t *) cluster_block,
	     (int (*)(intptr_t **, libcerror_error_t **)) &libqcow_cluster_block_free,
	     LIBFDATA_VECTOR_ELEMENT_VALUE_FLAG_MANAGED,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set cluster block as element value.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( cluster_block != NULL )
	{
		libqcow_cluster_block_free(
		 &cluster_block,
		 NULL );
	}
	return( -1 );
}

