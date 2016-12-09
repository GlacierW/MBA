/*
 * Hive Bin functions
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

#include "libregf_debug.h"
#include "libregf_definitions.h"
#include "libregf_hive_bin.h"
#include "libregf_hive_bin_cell.h"
#include "libregf_libbfio.h"
#include "libregf_libcdata.h"
#include "libregf_libcerror.h"
#include "libregf_libcnotify.h"
#include "libregf_libfdatetime.h"

#include "regf_hive_bin.h"

const char *regf_hive_bin_signature = "hbin";

/* Creates a hive bin
 * Make sure the value hive_bin is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libregf_hive_bin_initialize(
     libregf_hive_bin_t **hive_bin,
     libcerror_error_t **error )
{
	static char *function = "libregf_hive_bin_initialize";

	if( hive_bin == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid hive bin.",
		 function );

		return( -1 );
	}
	if( *hive_bin != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid hive bin value already set.",
		 function );

		return( -1 );
	}
	*hive_bin = memory_allocate_structure(
	             libregf_hive_bin_t );

	if( *hive_bin == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create hive bin.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *hive_bin,
	     0,
	     sizeof( libregf_hive_bin_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear hive bin.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( *hive_bin != NULL )
	{
		memory_free(
		 *hive_bin );

		*hive_bin = NULL;
	}
	return( -1 );
}

/* Frees a hive bin
 * Returns 1 if successful or -1 on error
 */
int libregf_hive_bin_free(
     libregf_hive_bin_t **hive_bin,
     libcerror_error_t **error )
{
	static char *function = "libregf_hive_bin_free";
	int result            = 1;

	if( hive_bin == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid hive bin.",
		 function );

		return( -1 );
	}
	if( *hive_bin != NULL )
	{
		if( ( *hive_bin )->cells_array != NULL )
		{
			if( libcdata_array_free(
			     &( ( *hive_bin )->cells_array ),
			     (int (*)(intptr_t **, libcerror_error_t **)) &libregf_hive_bin_cell_free,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: unable to free the hive bin cells array.",
				 function );

				result = -1;
			}
		}
		if( ( *hive_bin )->data != NULL )
		{
			memory_free(
			 ( *hive_bin )->data );
		}
		memory_free(
		 *hive_bin );

		*hive_bin = NULL;
	}
	return( result );
}

/* Reads a hive bin header
 * Returns 1 if successful, 0 if no hive bin signature was found or -1 on error
 */
int libregf_hive_bin_read_header(
     libregf_hive_bin_t *hive_bin,
     libbfio_handle_t *file_io_handle,
     libcerror_error_t **error )
{
	regf_hive_bin_header_t hive_bin_header;

	static char *function = "libregf_hive_bin_read_header";
	ssize_t read_count    = 0;

#if defined( HAVE_DEBUG_OUTPUT )
	uint32_t value_32bit  = 0;
#endif

	if( hive_bin == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid hive bin.",
		 function );

		return( -1 );
	}
	read_count = libbfio_handle_read_buffer(
	              file_io_handle,
	              (uint8_t *) &hive_bin_header,
	              sizeof( regf_hive_bin_header_t ),
	              error );

	if( read_count != (ssize_t) sizeof( regf_hive_bin_header_t ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read hive bin header data.",
		 function );

		return( -1 );
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: hive bin header:\n",
		 function );
		libcnotify_print_data(
		 (uint8_t *) &hive_bin_header,
		 sizeof( regf_hive_bin_header_t ),
		 LIBCNOTIFY_PRINT_DATA_FLAG_GROUP_DATA );
	}
#endif
	if( memory_compare(
	     hive_bin_header.signature,
	     regf_hive_bin_signature,
	     4 ) != 0 )
	{
		return( 0 );
	}
	byte_stream_copy_to_uint32_little_endian(
	 hive_bin_header.hive_bin_offset,
	 hive_bin->offset );

	byte_stream_copy_to_uint32_little_endian(
	 hive_bin_header.size,
	 hive_bin->size );

#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: signature\t\t\t\t\t: %c%c%c%c\n",
		 function,
		 hive_bin_header.signature[ 0 ],
		 hive_bin_header.signature[ 1 ],
		 hive_bin_header.signature[ 2 ],
		 hive_bin_header.signature[ 3 ] );

		libcnotify_printf(
		 "%s: hive bin offset\t\t\t\t: %" PRIu32 " (0x%08" PRIx32 ")\n",
		 function,
		 hive_bin->offset,
		 hive_bin->offset );
		libcnotify_printf(
		 "%s: size\t\t\t\t\t: %" PRIu32 " bytes\n",
		 function,
		 hive_bin->size );

		byte_stream_copy_to_uint32_little_endian(
		 hive_bin_header.unknown1,
		 value_32bit );
		libcnotify_printf(
		 "%s: unknown1\t\t\t\t\t: 0x%08" PRIx32 " (%" PRIu32 ")\n",
		 function,
		 value_32bit,
		 value_32bit );
		byte_stream_copy_to_uint32_little_endian(
		 hive_bin_header.unknown2,
		 value_32bit );
		libcnotify_printf(
		 "%s: unknown2\t\t\t\t\t: 0x%08" PRIx32 " (%" PRIu32 ")\n",
		 function,
		 value_32bit,
		 value_32bit );

		if( libregf_debug_print_filetime_value(
		     function,
		     "unknown time\t\t\t\t",
		     hive_bin_header.unknown_time,
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
		byte_stream_copy_to_uint32_little_endian(
		 hive_bin_header.unknown_spare,
		 value_32bit );
		libcnotify_printf(
		 "%s: unknown spare\t\t\t\t: 0x%08" PRIx32 " (%" PRIu32 ")\n",
		 function,
		 value_32bit,
		 value_32bit );

		libcnotify_printf(
		 "\n" );
	}
#endif
	return( 1 );
}

/* Reads a hive bin and determines its cells
 * Returns 1 if successful or -1 on error
 */
int libregf_hive_bin_read_cells(
     libregf_hive_bin_t *hive_bin,
     libbfio_handle_t *file_io_handle,
     libcerror_error_t **error )
{
	libregf_hive_bin_cell_t *hive_bin_cell = NULL;
	uint8_t *hive_bin_cells_data           = NULL;
	static char *function                  = "libregf_hive_bin_read_cells";
	ssize_t read_count                     = 0;
	size_t hive_bin_cells_data_size        = 0;
	uint32_t hive_bin_cell_size            = 0;
	uint32_t hive_bin_cells_offset         = 0;
	int hive_bin_cell_index                = 0;

	if( hive_bin == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid hive bin.",
		 function );

		return( -1 );
	}
	if( hive_bin->data != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid hive bin - data already set.",
		 function );

		return( -1 );
	}
#if SIZEOF_SIZE_T <= 4
	if( hive_bin->size > (size_t) SSIZE_MAX )
#else
	if( hive_bin->size > (uint32_t) SSIZE_MAX )
#endif
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid hive bin - size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( hive_bin->cells_array != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid hive bin - cells array already set.",
		 function );

		return( -1 );
	}
	hive_bin->data_size = (size_t) hive_bin->size - sizeof( regf_hive_bin_header_t );

	hive_bin->data = (uint8_t *) memory_allocate(
	                              sizeof( uint8_t ) * hive_bin->data_size );
	
	if( hive_bin->data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create hive cells data.",
		 function );

		goto on_error;
	}
	read_count = libbfio_handle_read_buffer(
	              file_io_handle,
	              hive_bin->data,
	              hive_bin->data_size,
	              error );

	if( read_count != (ssize_t) hive_bin->data_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read hive bin cells data.",
		 function );

		goto on_error;
	}
	if( libcdata_array_initialize(
	     &( hive_bin->cells_array ),
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create hive bin cells array.",
		 function );

		goto on_error;
	}
	hive_bin_cells_data      = hive_bin->data;
	hive_bin_cells_data_size = hive_bin->data_size;
	hive_bin_cells_offset    = hive_bin->offset + sizeof( regf_hive_bin_header_t );

#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: hive bin cells:\n",
		 function );
		libcnotify_print_data(
		 hive_bin_cells_data,
		 hive_bin_cells_data_size,
		 LIBCNOTIFY_PRINT_DATA_FLAG_GROUP_DATA );
	}
#endif
	while( hive_bin_cells_data_size > 0 )
	{
		if( libregf_hive_bin_cell_initialize(
		     &hive_bin_cell,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create hive bin cell.",
			 function );

			goto on_error;
		}
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: hive bin cell: %03d offset\t\t\t: 0x%08" PRIx32 "\n",
			 function,
			 hive_bin_cell_index,
			 hive_bin_cells_offset );
		}
#endif
		hive_bin_cell->offset = (uint32_t) hive_bin_cells_offset;

		byte_stream_copy_to_uint32_little_endian(
		 hive_bin_cells_data,
		 hive_bin_cell_size );

		hive_bin_cells_data      += 4;
		hive_bin_cells_offset    += 4;
		hive_bin_cells_data_size -= 4;

#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: hive bin cell: %03d size\t\t\t: 0x%08" PRIx32 " (%" PRIi32 ")\n",
			 function,
			 hive_bin_cell_index,
			 hive_bin_cell_size,
			 (int32_t) hive_bin_cell_size );
		}
#endif
		if( (int32_t) hive_bin_cell_size < 0 )
		{
			hive_bin_cell_size = (uint32_t) ( -1 * (int32_t) hive_bin_cell_size );
		}
		else
		{
			hive_bin_cell->flags |= LIBREGF_HIVE_BIN_CELL_FLAG_UNALLOCATED;
		}
		if( ( hive_bin_cell_size % 8 ) != 0 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
			 "%s: invalid cell size value should be multitude of 8.",
			 function );

			goto on_error;
		}
		/* Remove the size of the cell size value
		 */
		hive_bin_cell_size -= 4;

		if( hive_bin_cell_size > hive_bin_cells_data_size )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
			 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: invalid cell size value exceeds hive bin size.",
			 function );

			goto on_error;
		}
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: hive bin cell: %03d data:\n",
			 function,
			 hive_bin_cell_index );
			libcnotify_print_data(
			 hive_bin_cells_data,
			 hive_bin_cell_size,
			 LIBCNOTIFY_PRINT_DATA_FLAG_GROUP_DATA );
		}
#endif
		hive_bin_cell->data = hive_bin_cells_data;
		hive_bin_cell->size = hive_bin_cell_size;

		if( libcdata_array_append_entry(
		     hive_bin->cells_array,
		     &hive_bin_cell_index,
		     (intptr_t *) hive_bin_cell,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
			 "%s: unable to append hive bin cell: %d.",
			 function,
			 hive_bin_cell_index );

			goto on_error;
		}
		hive_bin_cell = NULL;

		hive_bin_cells_data      += hive_bin_cell_size;
		hive_bin_cells_offset    += hive_bin_cell_size;
		hive_bin_cells_data_size -= hive_bin_cell_size;

		hive_bin_cell_index++;
	}
	return( 1 );

on_error:
	if( hive_bin_cell != NULL )
	{
		libregf_hive_bin_cell_free(
		 &hive_bin_cell,
		 NULL );
	}
	if( hive_bin->data != NULL )
	{
		memory_free(
		 hive_bin->data );

		hive_bin->data = NULL;
	}
	hive_bin->data_size = 0;

	return( -1 );
}

/* Retrieves the number of hive bin cells
 * Returns 1 if successful or -1 on error
 */
int libregf_hive_bin_get_number_of_cells(
     libregf_hive_bin_t *hive_bin,
     uint16_t *number_of_cells,
     libcerror_error_t **error )
{
	static char *function = "libregf_hive_bin_get_number_of_cells";
	int number_of_entries = 0;

	if( hive_bin == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid hive bin.",
		 function );

		return( -1 );
	}
	if( number_of_cells == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid number of cells.",
		 function );

		return( -1 );
	}
	if( libcdata_array_get_number_of_entries(
	     hive_bin->cells_array,
	     &number_of_entries,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of entries.",
		 function );

		return( -1 );
	}
	if( number_of_entries > (int) UINT16_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid number of entries value exceeds maximum.",
		 function );

		return( -1 );
	}
	*number_of_cells = (uint16_t) number_of_entries;

	return( 1 );
}

/* Retrieves the hive bin cell at the index
 * Returns 1 if successful or -1 on error
 */
int libregf_hive_bin_get_cell(
     libregf_hive_bin_t *hive_bin,
     uint16_t cell_index,
     libregf_hive_bin_cell_t **hive_bin_cell,
     libcerror_error_t **error )
{
	static char *function = "libregf_hive_bin_get_cell";

	if( hive_bin == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid hive bin.",
		 function );

		return( -1 );
	}
	if( libcdata_array_get_entry_by_index(
	     hive_bin->cells_array,
	     (int) cell_index,
	     (intptr_t **) hive_bin_cell,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve hive bin cell: %" PRIu16 ".",
		 function,
		 cell_index );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the hive bin cell at the offset
 * Returns 1 if successful, 0 if not available or -1 on error
 */
int libregf_hive_bin_get_cell_at_offset(
     libregf_hive_bin_t *hive_bin,
     uint32_t cell_offset,
     libregf_hive_bin_cell_t **hive_bin_cell,
     libcerror_error_t **error )
{
	static char *function = "libregf_hive_bin_get_cell_at_offset";
	int entry_index       = 0;
	int number_of_entries = 0;

	if( hive_bin == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid hive bin.",
		 function );

		return( -1 );
	}
	if( hive_bin->cells_array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid hive bin - missing cells array.",
		 function );

		return( -1 );
	}
	if( hive_bin_cell == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid hive bin cell.",
		 function );

		return( -1 );
	}
	if( libcdata_array_get_number_of_entries(
	     hive_bin->cells_array,
	     &number_of_entries,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve the number of hive bin cells.",
		 function );

		return( -1 );
	}
	for( entry_index = 0;
	     entry_index < number_of_entries;
	     entry_index++ )
	{
		if( libcdata_array_get_entry_by_index(
		     hive_bin->cells_array,
		     entry_index,
		     (intptr_t **) hive_bin_cell,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve hive bin cell: %d.",
			 function,
			 entry_index );

			return( -1 );
		}
		if( ( *hive_bin_cell )->offset == cell_offset )
		{
			return( 1 );
		}
	}
	*hive_bin_cell = NULL;

	return( 0 );
}

