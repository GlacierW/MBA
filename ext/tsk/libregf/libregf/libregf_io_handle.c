/*
 * Input/Output (IO) handle
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

#include "libregf_checksum.h"
#include "libregf_codepage.h"
#include "libregf_data_type.h"
#include "libregf_debug.h"
#include "libregf_definitions.h"
#include "libregf_hive_bin.h"
#include "libregf_io_handle.h"
#include "libregf_libbfio.h"
#include "libregf_libcerror.h"
#include "libregf_libcnotify.h"
#include "libregf_libfdatetime.h"
#include "libregf_unused.h"

#include "regf_file_header.h"

const char *regf_file_signature = "regf";

/* Creates an IO handle
 * Make sure the value io_handle is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libregf_io_handle_initialize(
     libregf_io_handle_t **io_handle,
     libcerror_error_t **error )
{
	static char *function = "libregf_io_handle_initialize";

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
	              libregf_io_handle_t );

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
	     sizeof( libregf_io_handle_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear IO handle.",
		 function );

		goto on_error;
	}
	( *io_handle )->ascii_codepage = LIBREGF_CODEPAGE_WINDOWS_1252;

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
int libregf_io_handle_free(
     libregf_io_handle_t **io_handle,
     libcerror_error_t **error )
{
	static char *function = "libregf_io_handle_free";
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
		memory_free(
		 *io_handle );

		*io_handle = NULL;
	}
	return( result );
}

/* Clears an IO handle
 * Returns 1 if successful or -1 on error
 */
int libregf_io_handle_clear(
     libregf_io_handle_t *io_handle,
     libcerror_error_t **error )
{
	static char *function = "libregf_io_handle_clear";

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
	if( memory_set(
	     io_handle,
	     0,
	     sizeof( libregf_io_handle_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear IO handle.",
		 function );

		return( -1 );
	}
	io_handle->ascii_codepage = LIBREGF_CODEPAGE_WINDOWS_1252;

	return( 1 );
}

/* Reads the file header
 * Returns 1 if successful or -1 on error
 */
int libregf_io_handle_read_file_header(
     libregf_io_handle_t *io_handle,
     libbfio_handle_t *file_io_handle,
     uint32_t *root_key_offset,
     uint32_t *hive_bins_size,
     libcerror_error_t **error )
{
	regf_file_header_t file_header;

	static char *function              = "libregf_io_handle_read_file_header";
	ssize_t read_count                 = 0;
	uint32_t calculated_xor32_checksum = 0;
	uint32_t stored_xor32_checksum     = 0;

#if defined( HAVE_DEBUG_OUTPUT )
	uint32_t value_32bit               = 0;
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
	if( root_key_offset == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid root key offset.",
		 function );

		return( -1 );
	}
	if( hive_bins_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid hive bins size.",
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

		return( -1 );
	}
	read_count = libbfio_handle_read_buffer(
	              file_io_handle,
	              (uint8_t *) &file_header,
	              sizeof( regf_file_header_t ),
	              error );

	if( read_count != (ssize_t) sizeof( regf_file_header_t ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read file header data.",
		 function );

		return( -1 );
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: file header:\n",
		 function );
		libcnotify_print_data(
		 (uint8_t *) &file_header,
		 sizeof( regf_file_header_t ),
		 LIBCNOTIFY_PRINT_DATA_FLAG_GROUP_DATA );
	}
#endif
	if( memory_compare(
	     file_header.signature,
	     regf_file_signature,
	     4 ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: invalid file signature.",
		 function );

		return( -1 );
	}
	if( libregf_checksum_calculate_little_endian_xor32(
	     &calculated_xor32_checksum,
	     (uint8_t *) &file_header,
	     508,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE,
		 "%s: unable to calculate XOR-32 checksum.",
		 function );

		return( -1 );
	}
	byte_stream_copy_to_uint32_little_endian(
	 file_header.checksum,
	 stored_xor32_checksum );

	if( stored_xor32_checksum != calculated_xor32_checksum )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_INPUT,
		 LIBCERROR_INPUT_ERROR_CHECKSUM_MISMATCH,
		 "%s: mismatch in file header checksum ( 0x%08" PRIx32 " != 0x%08" PRIx32 " ).",
		 function,
		 stored_xor32_checksum,
		 calculated_xor32_checksum );

		return( -1 );
	}
	byte_stream_copy_to_uint32_little_endian(
	 file_header.major_version,
	 io_handle->major_version );

	byte_stream_copy_to_uint32_little_endian(
	 file_header.minor_version,
	 io_handle->minor_version );

	byte_stream_copy_to_uint32_little_endian(
	 file_header.file_type,
	 io_handle->file_type );

	byte_stream_copy_to_uint32_little_endian(
	 file_header.root_key_offset,
	 *root_key_offset );

	byte_stream_copy_to_uint32_little_endian(
	 file_header.hive_bins_size,
	 *hive_bins_size );

#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: signature\t\t\t\t: %c%c%c%c\n",
		 function,
		 file_header.signature[ 0 ],
		 file_header.signature[ 1 ],
		 file_header.signature[ 2 ],
		 file_header.signature[ 3 ] );

		byte_stream_copy_to_uint32_little_endian(
		 file_header.primary_sequence_number,
		 value_32bit );
		libcnotify_printf(
		 "%s: primary sequence number\t\t: %" PRIu32 "\n",
		 function,
		 value_32bit );
		byte_stream_copy_to_uint32_little_endian(
		 file_header.secondary_sequence_number,
		 value_32bit );
		libcnotify_printf(
		 "%s: secondary sequence number\t\t: %" PRIu32 "\n",
		 function,
		 value_32bit );

		if( libregf_debug_print_filetime_value(
		     function,
		     "modification time\t\t\t",
		     file_header.modification_time,
		     8,
		     LIBFDATETIME_ENDIAN_LITTLE,
		     LIBFDATETIME_STRING_FORMAT_TYPE_CTIME | LIBFDATETIME_STRING_FORMAT_FLAG_DATE_TIME_NANO_SECONDS,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_PRINT_FAILED,
			 "%s: unable to print filetime value.",
			 function );

			return( -1 );
		}
		libcnotify_printf(
		 "%s: major version\t\t\t: %" PRIu32 "\n",
		 function,
		 io_handle->major_version );

		libcnotify_printf(
		 "%s: minor version\t\t\t: %" PRIu32 "\n",
		 function,
		 io_handle->minor_version );

		libcnotify_printf(
		 "%s: file type\t\t\t\t: %" PRIu32 "\n",
		 function,
		 io_handle->file_type );

		byte_stream_copy_to_uint32_little_endian(
		 file_header.unknown3,
		 value_32bit );
		libcnotify_printf(
		 "%s: unknown3\t\t\t\t: 0x%08" PRIx32 " (%" PRIu32 ")\n",
		 function,
		 value_32bit,
		 value_32bit );

		libcnotify_printf(
		 "%s: root key offset\t\t\t: 0x%08" PRIx32 "\n",
		 function,
		 *root_key_offset );
		libcnotify_printf(
		 "%s: hive bins size\t\t\t: %" PRIu32 " bytes\n",
		 function,
		 *hive_bins_size );

		byte_stream_copy_to_uint32_little_endian(
		 file_header.unknown4,
		 value_32bit );
		libcnotify_printf(
		 "%s: unknown4\t\t\t\t: 0x%08" PRIx32 " (%" PRIu32 ")\n",
		 function,
		 value_32bit,
		 value_32bit );

		libcnotify_printf(
		 "%s: unknown5:\n",
		 function );
		libcnotify_print_data(
		 file_header.unknown5,
		 64,
		 LIBCNOTIFY_PRINT_DATA_FLAG_GROUP_DATA );

		libcnotify_printf(
		 "%s: unknown6:\n",
		 function );
		libcnotify_print_data(
		 file_header.unknown6,
		 396,
		 LIBCNOTIFY_PRINT_DATA_FLAG_GROUP_DATA );

		libcnotify_printf(
		 "%s: checksum\t\t\t\t: 0x%08" PRIx32 "\n",
		 function,
		 stored_xor32_checksum );

		libcnotify_printf(
		 "\n" );
	}
#endif
	return( 1 );
}

/* Reads the segment data into the buffer
 * Callback function for the value item data stream
 * Returns the number of bytes read or -1 on error
 */
ssize_t libregf_io_handle_read_segment_data(
         intptr_t *data_handle LIBREGF_ATTRIBUTE_UNUSED,
         libbfio_handle_t *file_io_handle,
         int segment_index LIBREGF_ATTRIBUTE_UNUSED,
         int segment_file_index LIBREGF_ATTRIBUTE_UNUSED,
         uint8_t *segment_data,
         size_t segment_data_size,
         uint32_t segment_flags LIBREGF_ATTRIBUTE_UNUSED,
         uint8_t read_flags LIBREGF_ATTRIBUTE_UNUSED,
         libcerror_error_t **error )
{
	static char *function = "libregf_io_handle_read_segment_data";
	ssize_t read_count    = 0;

	LIBREGF_UNREFERENCED_PARAMETER( data_handle )
	LIBREGF_UNREFERENCED_PARAMETER( segment_index )
	LIBREGF_UNREFERENCED_PARAMETER( segment_file_index )
	LIBREGF_UNREFERENCED_PARAMETER( segment_flags )
	LIBREGF_UNREFERENCED_PARAMETER( read_flags )

	read_count = libbfio_handle_read_buffer(
		      file_io_handle,
		      segment_data,
		      segment_data_size,
		      error );

	if( read_count != (ssize_t) segment_data_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read segment data.",
		 function );

		return( -1 );
	}
	return( read_count );
}

/* Seeks a certain segment offset
 * Callback function for the value item data stream
 * Returns the offset or -1 on error
 */
off64_t libregf_io_handle_seek_segment_offset(
         intptr_t *data_handle LIBREGF_ATTRIBUTE_UNUSED,
         libbfio_handle_t *file_io_handle,
         int segment_index LIBREGF_ATTRIBUTE_UNUSED,
         int segment_file_index LIBREGF_ATTRIBUTE_UNUSED,
         off64_t segment_offset,
         libcerror_error_t **error )
{
	static char *function = "libregf_io_handle_seek_segment_offset";

	LIBREGF_UNREFERENCED_PARAMETER( data_handle )
	LIBREGF_UNREFERENCED_PARAMETER( segment_index )
	LIBREGF_UNREFERENCED_PARAMETER( segment_file_index )

	segment_offset = libbfio_handle_seek_offset(
	                  file_io_handle,
	                  segment_offset,
	                  SEEK_SET,
	                  error );

	if( segment_offset == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to seek segment offset.",
		 function );

		return( -1 );
	}
	return( segment_offset );
}

