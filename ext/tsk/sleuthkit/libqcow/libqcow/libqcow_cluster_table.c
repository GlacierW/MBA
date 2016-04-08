/*
 * Cluster table functions
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

#include "libqcow_cluster_table.h"
#include "libqcow_libbfio.h"
#include "libqcow_libcerror.h"
#include "libqcow_libcnotify.h"

/* Creates a cluster table
 * Make sure the value cluste_table is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libqcow_cluster_table_initialize(
     libqcow_cluster_table_t **cluster_table,
     libcerror_error_t **error )
{
	static char *function = "libqcow_cluster_table_initialize";

	if( cluster_table == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid cluster table.",
		 function );

		return( -1 );
	}
	if( *cluster_table != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid cluster table value already set.",
		 function );

		return( -1 );
	}
	*cluster_table = memory_allocate_structure(
	                  libqcow_cluster_table_t );

	if( *cluster_table == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create cluster table.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *cluster_table,
	     0,
	     sizeof( libqcow_cluster_table_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear cluster table.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( *cluster_table != NULL )
	{
		memory_free(
		 *cluster_table );

		*cluster_table = NULL;
	}
	return( -1 );
}

/* Frees a cluster table
 * Returns 1 if successful or -1 on error
 */
int libqcow_cluster_table_free(
     libqcow_cluster_table_t **cluster_table,
     libcerror_error_t **error )
{
	static char *function = "libqcow_cluster_table_free";

	if( cluster_table == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid cluster table.",
		 function );

		return( -1 );
	}
	if( *cluster_table != NULL )
	{
		if( ( *cluster_table )->references != NULL )
		{
			memory_free(
			 ( *cluster_table )->references );
		}
		memory_free(
		 *cluster_table );

		*cluster_table = NULL;
	}
	return( 1 );
}

/* Retrieves the number of references in the cluster table
 * Returns 1 if successful or -1 on error
 */
int libqcow_cluster_table_get_number_of_references(
     libqcow_cluster_table_t *cluster_table,
     int *number_of_references,
     libcerror_error_t **error )
{
	static char *function = "libqcow_cluster_table_get_number_of_references";

	if( cluster_table == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid cluster table.",
		 function );

		return( -1 );
	}
	if( number_of_references == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid number of references.",
		 function );

		return( -1 );
	}
	*number_of_references = cluster_table->number_of_references;

	return( 1 );
}

/* Retrieves a specific reference from the cluster table
 * Returns 1 if successful or -1 on error
 */
int libqcow_cluster_table_get_reference_by_index(
     libqcow_cluster_table_t *cluster_table,
     int reference_index,
     uint64_t *reference,
     libcerror_error_t **error )
{
	static char *function = "libqcow_cluster_table_get_reference_by_index";

	if( cluster_table == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid cluster table.",
		 function );

		return( -1 );
	}
	if( cluster_table->references == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid cluster table - missing references.",
		 function );

		return( -1 );
	}
	if( ( reference_index < 0 )
	 || ( reference_index >= cluster_table->number_of_references ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid reference index value out of bounds.",
		 function );

		return( -1 );
	}
	if( reference == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid reference.",
		 function );

		return( -1 );
	}
	*reference = cluster_table->references[ reference_index ];

	return( 1 );
}

/* Reads the cluster table
 * Returns 1 if successful or -1 on error
 */
int libqcow_cluster_table_read(
     libqcow_cluster_table_t *cluster_table,
     libbfio_handle_t *file_io_handle,
     off64_t file_offset,
     size_t cluster_table_size,
     libcerror_error_t **error )
{
	uint8_t *cluster_table_data      = NULL;
	static char *function            = "libqcow_cluster_table_read";
	size_t cluster_table_data_offset = 0;
	ssize_t read_count               = 0;
	int cluster_table_index          = 0;

	if( cluster_table == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid cluster table.",
		 function );

		return( -1 );
	}
	if( cluster_table->references != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid cluster table - references already set.",
		 function );

		return( -1 );
	}
	if( cluster_table_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid cluster table size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( ( cluster_table_size % 8 ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported cluster table size value - value not a multitude of 8.",
		 function );

		return( -1 );
	}
	if( ( cluster_table_size / 8 ) > (size_t) INT_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid number of references value exceeds maximum.",
		 function );

		return( -1 );
	}
	cluster_table->number_of_references = (int) ( cluster_table_size / 8 );

	cluster_table->references = (uint64_t *) memory_allocate(
	                                          cluster_table_size );

	if( cluster_table->references == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create references.",
		 function );

		goto on_error;
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: reading cluster table at offset: %" PRIi64 " (0x%08" PRIx64 ")\n",
		 function,
		 file_offset,
		 file_offset );
	}
#endif
	if( libbfio_handle_seek_offset(
	     file_io_handle,
	     file_offset,
	     SEEK_SET,
	     error ) == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_SEEK_FAILED,
		 "%s: unable to seek cluster table offset: %" PRIi64 ".",
		 function,
		 file_offset );

		goto on_error;
	}
	cluster_table_data = (uint8_t *) memory_allocate(
	                                  cluster_table_size );

	if( cluster_table_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create cluster table data.",
		 function );

		goto on_error;
	}
	read_count = libbfio_handle_read_buffer(
	              file_io_handle,
	              cluster_table_data,
	              cluster_table_size,
	              error );

	if( read_count != (ssize_t) cluster_table_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read cluster table.",
		 function );

		goto on_error;
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: cluster table data:\n",
		 function );
		libcnotify_print_data(
		 cluster_table_data,
		 cluster_table_size,
		 LIBCNOTIFY_PRINT_DATA_FLAG_GROUP_DATA );
	}
#endif
	for( cluster_table_index = 0;
	     cluster_table_index < cluster_table->number_of_references;
	     cluster_table_index++ )
	{
		byte_stream_copy_to_uint64_big_endian(
		 &( cluster_table_data[ cluster_table_data_offset ] ),
		 ( cluster_table->references )[ cluster_table_index ] );

		cluster_table_data_offset += 8;

#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: cluster table reference: %03d\t\t: 0x%08" PRIx64 "\n",
			 function,
			 cluster_table_index,
			 ( cluster_table->references )[ cluster_table_index ] );
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
	 cluster_table_data );

	cluster_table_data = NULL;

	return( 1 );

on_error:
	if( cluster_table_data != NULL )
	{
		memory_free(
		 cluster_table_data );
	}
	if( cluster_table->references != NULL )
	{
		memory_free(
		 cluster_table->references );

		cluster_table->references = NULL;
	}
	return( -1 );
}

