/*
 * Cluster block functions
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
#include "libqcow_definitions.h"
#include "libqcow_libbfio.h"
#include "libqcow_libcerror.h"
#include "libqcow_libcnotify.h"

/* Creates a cluster block
 * Make sure the value cluster_block is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libqcow_cluster_block_initialize(
     libqcow_cluster_block_t **cluster_block,
     size_t data_size,
     libcerror_error_t **error )
{
	static char *function = "libqcow_cluster_block_initialize";

	if( cluster_block == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid cluster block.",
		 function );

		return( -1 );
	}
	if( *cluster_block != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid cluster block value already set.",
		 function );

		return( -1 );
	}
	if( data_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid data size value exceeds maximum.",
		 function );

		return( -1 );
	}
	*cluster_block = memory_allocate_structure(
	                  libqcow_cluster_block_t );

	if( *cluster_block == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create cluster block.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *cluster_block,
	     0,
	     sizeof( libqcow_cluster_block_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear cluster block.",
		 function );

		memory_free(
		 *cluster_block );

		*cluster_block = NULL;

		return( -1 );
	}
	if( data_size > 0 )
	{
		( *cluster_block )->data = (uint8_t *) memory_allocate(
		                                        sizeof( uint8_t ) * data_size );

		if( ( *cluster_block )->data == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_MEMORY,
			 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to create data.",
			 function );

			goto on_error;
		}
		( *cluster_block )->data_size = data_size;
	}
	return( 1 );

on_error:
	if( *cluster_block != NULL )
	{
		memory_free(
		 *cluster_block );

		*cluster_block = NULL;
	}
	return( -1 );
}

/* Frees a cluster block
 * Returns 1 if successful or -1 on error
 */
int libqcow_cluster_block_free(
     libqcow_cluster_block_t **cluster_block,
     libcerror_error_t **error )
{
	static char *function = "libqcow_cluster_block_free";
	int result            = 1;

	if( cluster_block == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid cluster block.",
		 function );

		return( -1 );
	}
	if( *cluster_block != NULL )
	{
		if( ( *cluster_block )->compressed_data != NULL )
		{
			memory_free(
			 ( *cluster_block )->compressed_data );
		}
		if( ( *cluster_block )->encrypted_data != NULL )
		{
			memory_free(
			 ( *cluster_block )->encrypted_data );
		}
		if( ( *cluster_block )->data != NULL )
		{
			if( memory_set(
			     ( *cluster_block )->data,
			     0,
			     ( *cluster_block )->data_size ) == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_MEMORY,
				 LIBCERROR_MEMORY_ERROR_SET_FAILED,
				 "%s: unable to clear data.",
				 function );

				result = -1;
			}
			memory_free(
			 ( *cluster_block )->data );
		}
		memory_free(
		 *cluster_block );

		*cluster_block = NULL;
	}
	return( result );
}

/* Reads cluster block
 * Returns 1 if successful or -1 on error
 */
int libqcow_cluster_block_read(
     libqcow_cluster_block_t *cluster_block,
     libbfio_handle_t *file_io_handle,
     off64_t cluster_block_offset,
     libcerror_error_t **error )
{
	static char *function = "libqcow_cluster_block_read";
	ssize_t read_count    = 0;

	if( cluster_block == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid cluster block.",
		 function );

		return( -1 );
	}
	if( cluster_block->data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid cluster block - missing data.",
		 function );

		return( -1 );
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: reading cluster block at offset: %" PRIi64 " (0x%08" PRIx64 ")\n",
		 function,
		 cluster_block_offset,
		 cluster_block_offset );
	}
#endif
	if( libbfio_handle_seek_offset(
	     file_io_handle,
	     cluster_block_offset,
	     SEEK_SET,
	     error ) == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_SEEK_FAILED,
		 "%s: unable to seek cluster block offset: %" PRIi64 " (0x%08" PRIx64 ").",
		 function,
		 cluster_block_offset,
		 cluster_block_offset );

		return( -1 );
	}
	read_count = libbfio_handle_read_buffer(
		      file_io_handle,
		      cluster_block->data,
		      cluster_block->data_size,
		      error );

	if( read_count != (ssize_t) cluster_block->data_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read cluster block.",
		 function );

		return( -1 );
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: cluster block:\n",
		 function );
		libcnotify_print_data(
		 cluster_block->data,
		 cluster_block->data_size,
		 LIBCNOTIFY_PRINT_DATA_FLAG_GROUP_DATA );
	}
#endif
	return( 1 );
}

