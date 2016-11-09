/*
 * The stream functions
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
#include <memory.h>
#include <types.h>

#if defined( HAVE_UNISTD_H )
#include <unistd.h>
#endif

#include "libfdata_definitions.h"
#include "libfdata_libcerror.h"
#include "libfdata_libcnotify.h"
#include "libfdata_mapped_range.h"
#include "libfdata_range.h"
#include "libfdata_segments_array.h"
#include "libfdata_stream.h"
#include "libfdata_types.h"
#include "libfdata_unused.h"

/* Creates a stream
 * Make sure the value stream is referencing, is set to NULL
 *
 * If the flag LIBFDATA_DATA_HANDLE_FLAG_MANAGED is set the stream
 * takes over management of the data handle and the data handle is freed when
 * no longer needed
 *
 * Returns 1 if successful or -1 on error
 */
int libfdata_stream_initialize(
     libfdata_stream_t **stream,
     intptr_t *data_handle,
     int (*free_data_handle)(
            intptr_t **data_handle,
            libcerror_error_t **error ),
     int (*clone_data_handle)(
            intptr_t **destination_data_handle,
            intptr_t *source_data_handle,
            libcerror_error_t **error ),
     int (*create_segment)(
            intptr_t *data_handle,
            intptr_t *file_io_handle,
            int segment_index,
            int *segment_file_index,
            off64_t *segment_offset,
            size64_t *segment_size,
            uint32_t *segment_flags,
            libcerror_error_t **error ),
     ssize_t (*read_segment_data)(
                intptr_t *data_handle,
                intptr_t *file_io_handle,
                int segment_index,
                int segment_file_index,
                uint8_t *segment_data,
                size_t segment_data_size,
                uint32_t segment_flags,
                uint8_t read_flags,
                libcerror_error_t **error ),
     ssize_t (*write_segment_data)(
                intptr_t *data_handle,
                intptr_t *file_io_handle,
                int segment_index,
                int segment_file_index,
                const uint8_t *segment_data,
                size_t segment_data_size,
                uint32_t segment_flags,
                uint8_t write_flags,
                libcerror_error_t **error ),
     off64_t (*seek_segment_offset)(
                intptr_t *data_handle,
                intptr_t *file_io_handle,
                int segment_index,
                int segment_file_index,
                off64_t segment_offset,
                libcerror_error_t **error ),
     uint8_t flags,
     libcerror_error_t **error )
{
	libfdata_internal_stream_t *internal_stream = NULL;
	static char *function                       = "libfdata_stream_initialize";

	if( stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid stream.",
		 function );

		return( -1 );
	}
	if( *stream != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid stream value already set.",
		 function );

		return( -1 );
	}
	if( ( flags & 0xfe ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported flags: 0x%02" PRIx8 ".",
		 function );

		return( -1 );
	}
	internal_stream = memory_allocate_structure(
	                   libfdata_internal_stream_t );

	if( internal_stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create stream.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_stream,
	     0,
	     sizeof( libfdata_internal_stream_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear stream.",
		 function );

		memory_free(
		 internal_stream );

		return( -1 );
	}
	if( libcdata_array_initialize(
	     &( internal_stream->segments_array ),
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create segments array.",
		 function );

		goto on_error;
	}
	if( libcdata_array_initialize(
	     &( internal_stream->mapped_ranges_array ),
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create mapped ranges array.",
		 function );

		goto on_error;
	}
	internal_stream->flags              |= flags;
	internal_stream->data_handle         = data_handle;
	internal_stream->free_data_handle    = free_data_handle;
	internal_stream->clone_data_handle   = clone_data_handle;
	internal_stream->create_segment      = create_segment;
	internal_stream->read_segment_data   = read_segment_data;
	internal_stream->write_segment_data  = write_segment_data;
	internal_stream->seek_segment_offset = seek_segment_offset;

	*stream = (libfdata_stream_t *) internal_stream;

	return( 1 );

on_error:
	if( internal_stream != NULL )
	{
		if( internal_stream->segments_array != NULL )
		{
			libcdata_array_free(
			 &( internal_stream->segments_array ),
			 NULL,
			 NULL );
		}
		memory_free(
		 internal_stream );
	}
	return( -1 );
}

/* Frees a stream
 * Returns 1 if successful or -1 on error
 */
int libfdata_stream_free(
     libfdata_stream_t **stream,
     libcerror_error_t **error )
{
	libfdata_internal_stream_t *internal_stream = NULL;
	static char *function                       = "libfdata_stream_free";
	int result                                  = 1;

	if( stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid stream.",
		 function );

		return( -1 );
	}
	if( *stream != NULL )
	{
		internal_stream = (libfdata_internal_stream_t *) *stream;
		*stream         = NULL;

		if( libcdata_array_free(
		     &( internal_stream->segments_array ),
		     (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_range_free,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free the segments array.",
			 function );

			result = -1;
		}
		if( libcdata_array_free(
		     &( internal_stream->mapped_ranges_array ),
		     (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_mapped_range_free,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free the mapped ranges array.",
			 function );

			result = -1;
		}
		if( ( internal_stream->flags & LIBFDATA_DATA_HANDLE_FLAG_MANAGED ) != 0 )
		{
			if( internal_stream->data_handle != NULL )
			{
				if( internal_stream->free_data_handle == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: invalid source stream - missing free data handle function.",
					 function );

					result = -1;
				}
				else if( internal_stream->free_data_handle(
				          &( internal_stream->data_handle ),
				          error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
					 "%s: unable to free data handle.",
					 function );

					result = -1;
				}
			}
		}
		memory_free(
		 internal_stream );
	}
	return( result );
}

/* Clones (duplicates) the stream
 * Returns 1 if successful or -1 on error
 */
int libfdata_stream_clone(
     libfdata_stream_t **destination_stream,
     libfdata_stream_t *source_stream,
     libcerror_error_t **error )
{
	libfdata_internal_stream_t *internal_destination_stream = NULL;
	libfdata_internal_stream_t *internal_source_stream      = NULL;
	static char *function                                   = "libfdata_stream_clone";

	if( destination_stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid destination stream.",
		 function );

		return( -1 );
	}
	if( *destination_stream != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: destination stream already set.",
		 function );

		return( -1 );
	}
	if( source_stream == NULL )
	{
		*destination_stream = NULL;

		return( 1 );
	}
	internal_source_stream = (libfdata_internal_stream_t *) source_stream;

	internal_destination_stream = memory_allocate_structure(
	                               libfdata_internal_stream_t );

	if( internal_destination_stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create destination stream.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_destination_stream,
	     0,
	     sizeof( libfdata_internal_stream_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear destination stream.",
		 function );

		memory_free(
		 internal_destination_stream );

		return( -1 );
	}
	if( internal_source_stream->data_handle != NULL )
	{
		if( internal_source_stream->free_data_handle == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: invalid source stream - missing free data handle function.",
			 function );

			goto on_error;
		}
		if( internal_source_stream->clone_data_handle == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: invalid source stream - missing clone data handle function.",
			 function );

			goto on_error;
		}
		if( internal_source_stream->clone_data_handle(
		     &( internal_destination_stream->data_handle ),
		     internal_source_stream->data_handle,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create destination data handle.",
			 function );

			goto on_error;
		}
	}
	if( libcdata_array_clone(
	     &( internal_destination_stream->segments_array ),
	     internal_source_stream->segments_array,
	     (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_range_free,
	     (int (*)(intptr_t **, intptr_t *, libcerror_error_t **)) &libfdata_range_clone,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create destination segments array.",
		 function );

		goto on_error;
	}
	if( libcdata_array_clone(
	     &( internal_destination_stream->mapped_ranges_array ),
	     internal_source_stream->mapped_ranges_array,
	     (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_mapped_range_free,
	     (int (*)(intptr_t **, intptr_t *, libcerror_error_t **)) &libfdata_mapped_range_clone,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create destination mapped ranges array.",
		 function );

		goto on_error;
	}
	internal_destination_stream->flags               = internal_source_stream->flags | LIBFDATA_DATA_HANDLE_FLAG_MANAGED;
	internal_destination_stream->free_data_handle    = internal_source_stream->free_data_handle;
	internal_destination_stream->clone_data_handle   = internal_source_stream->clone_data_handle;
	internal_destination_stream->create_segment      = internal_source_stream->create_segment;
	internal_destination_stream->read_segment_data   = internal_source_stream->read_segment_data;
	internal_destination_stream->write_segment_data  = internal_source_stream->write_segment_data;
	internal_destination_stream->seek_segment_offset = internal_source_stream->seek_segment_offset;

	*destination_stream = (libfdata_stream_t *) internal_destination_stream;

	return( 1 );

on_error:
	if( internal_destination_stream != NULL )
	{
		if( internal_destination_stream->segments_array != NULL )
		{
			libcdata_array_free(
			 &( internal_destination_stream->segments_array ),
			 (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_range_free,
			 NULL );
		}
		if( ( internal_destination_stream->data_handle != NULL )
		 && ( internal_source_stream->free_data_handle != NULL ) )
		{
			internal_source_stream->free_data_handle(
			 &( internal_destination_stream->data_handle ),
			 NULL );
		}
		memory_free(
		 internal_destination_stream );
	}
	return( -1 );
}

/* Segment functions
 */

/* Empties the stream
 * Returns 1 if successful or -1 on error
 */
int libfdata_stream_empty(
     libfdata_stream_t *stream,
     libcerror_error_t **error )
{
	libfdata_internal_stream_t *internal_stream = NULL;
	static char *function                       = "libfdata_stream_empty";

	if( stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid stream.",
		 function );

		return( -1 );
	}
	internal_stream = (libfdata_internal_stream_t *) stream;

	if( libcdata_array_empty(
	     internal_stream->segments_array,
	     (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_range_free,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
		 "%s: unable to empty segments array.",
		 function );

		return( -1 );
	}
	if( libcdata_array_empty(
	     internal_stream->mapped_ranges_array,
	     (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_mapped_range_free,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
		 "%s: unable to empty mapped ranges array.",
		 function );

		return( -1 );
	}
	internal_stream->size = 0;

	return( 1 );
}

/* Resizes the stream
 * Returns 1 if successful or -1 on error
 */
int libfdata_stream_resize(
     libfdata_stream_t *stream,
     int number_of_segments,
     libcerror_error_t **error )
{
	libfdata_internal_stream_t *internal_stream = NULL;
	static char *function                       = "libfdata_stream_resize";

	if( stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid stream.",
		 function );

		return( -1 );
	}
	internal_stream = (libfdata_internal_stream_t *) stream;

	if( libcdata_array_resize(
	     internal_stream->segments_array,
	     number_of_segments,
	     (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_range_free,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_RESIZE_FAILED,
		 "%s: unable to resize segments array.",
		 function );

		return( -1 );
	}
	if( libcdata_array_resize(
	     internal_stream->mapped_ranges_array,
	     number_of_segments,
	     (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_mapped_range_free,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_RESIZE_FAILED,
		 "%s: unable to resize mapped ranges array.",
		 function );

		return( -1 );
	}
	internal_stream->flags |= LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES;

	return( 1 );
}

/* Reverses the order of the segments
 * Returns 1 if successful or -1 on error
 */
int libfdata_stream_reverse(
     libfdata_stream_t *stream,
     libcerror_error_t **error )
{
	libfdata_internal_stream_t *internal_stream = NULL;
	static char *function                       = "libfdata_stream_reverse";

	if( stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid stream.",
		 function );

		return( -1 );
	}
	internal_stream = (libfdata_internal_stream_t *) stream;

	if( libcdata_array_reverse(
	     internal_stream->segments_array,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to reverse segments array.",
		 function );

		return( -1 );
	}
	internal_stream->flags |= LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES;

	return( 1 );
}

/* Retrieves the number of segments
 * Returns 1 if successful or -1 on error
 */
int libfdata_stream_get_number_of_segments(
     libfdata_stream_t *stream,
     int *number_of_segments,
     libcerror_error_t **error )
{
	libfdata_internal_stream_t *internal_stream = NULL;
	static char *function                       = "libfdata_stream_get_number_of_segments";

	if( stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid stream.",
		 function );

		return( -1 );
	}
	internal_stream = (libfdata_internal_stream_t *) stream;

	if( libcdata_array_get_number_of_entries(
	     internal_stream->segments_array,
	     number_of_segments,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of entries from segments array.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves a specific segment
 * Returns 1 if successful or -1 on error
 */
int libfdata_stream_get_segment_by_index(
     libfdata_stream_t *stream,
     int segment_index,
     int *segment_file_index,
     off64_t *segment_offset,
     size64_t *segment_size,
     uint32_t *segment_flags,
     libcerror_error_t **error )
{
	libfdata_internal_stream_t *internal_stream = NULL;
	static char *function                       = "libfdata_stream_get_segment_by_index";

	if( stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid stream.",
		 function );

		return( -1 );
	}
	internal_stream = (libfdata_internal_stream_t *) stream;

	if( libfdata_segments_array_get_segment_by_index(
	     internal_stream->segments_array,
	     segment_index,
	     segment_file_index,
	     segment_offset,
	     segment_size,
	     segment_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve segment: %d.",
		 function,
		 segment_index );

		return( -1 );
	}
	return( 1 );
}

/* Sets a specific segment
 * Returns 1 if successful or -1 on error
 */
int libfdata_stream_set_segment_by_index(
     libfdata_stream_t *stream,
     int segment_index,
     int segment_file_index,
     off64_t segment_offset,
     size64_t segment_size,
     uint32_t segment_flags,
     libcerror_error_t **error )
{
	libfdata_internal_stream_t *internal_stream = NULL;
	static char *function                       = "libfdata_stream_set_segment_by_index";

	if( stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid stream.",
		 function );

		return( -1 );
	}
	internal_stream = (libfdata_internal_stream_t *) stream;

	if( libfdata_segments_array_set_segment_by_index(
	     internal_stream->segments_array,
	     internal_stream->mapped_ranges_array,
	     &( internal_stream->size ),
	     segment_index,
	     segment_file_index,
	     segment_offset,
	     segment_size,
	     segment_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set segment: %d.",
		 function,
		 segment_index );

		return( -1 );
	}
	internal_stream->flags |= LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES;

	return( 1 );
}

/* Prepends a segment
 * Returns 1 if successful or -1 on error
 */
int libfdata_stream_prepend_segment(
     libfdata_stream_t *stream,
     int segment_file_index,
     off64_t segment_offset,
     size64_t segment_size,
     uint32_t segment_flags,
     libcerror_error_t **error )
{
	libfdata_internal_stream_t *internal_stream = NULL;
	static char *function                       = "libfdata_stream_prepend_segment";

	if( stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid stream.",
		 function );

		return( -1 );
	}
	internal_stream = (libfdata_internal_stream_t *) stream;

	if( libfdata_segments_array_prepend_segment(
	     internal_stream->segments_array,
	     internal_stream->mapped_ranges_array,
	     &( internal_stream->size ),
	     segment_file_index,
	     segment_offset,
	     segment_size,
	     segment_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
		 "%s: unable to prepend segment.",
		 function );

		return( -1 );
	}
	internal_stream->flags |= LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES;

	return( 1 );
}

/* Appends a segment
 * Returns 1 if successful or -1 on error
 */
int libfdata_stream_append_segment(
     libfdata_stream_t *stream,
     int *segment_index,
     int segment_file_index,
     off64_t segment_offset,
     size64_t segment_size,
     uint32_t segment_flags,
     libcerror_error_t **error )
{
	libfdata_internal_stream_t *internal_stream = NULL;
	static char *function                       = "libfdata_stream_append_segment";

	if( stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid stream.",
		 function );

		return( -1 );
	}
	internal_stream = (libfdata_internal_stream_t *) stream;

	if( libfdata_segments_array_append_segment(
	     internal_stream->segments_array,
	     internal_stream->mapped_ranges_array,
	     &( internal_stream->size ),
	     segment_index,
	     segment_file_index,
	     segment_offset,
	     segment_size,
	     segment_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
		 "%s: unable to append segment.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Sets the mapped size
 * The mapped size cannot be larger than the stream size
 * A value of 0 is equivalent for the stream size
 * Returns 1 if successful or -1 on error
 */
int libfdata_stream_set_mapped_size(
     libfdata_stream_t *stream,
     size64_t mapped_size,
     libcerror_error_t **error )
{
	libfdata_internal_stream_t *internal_stream = NULL;
	static char *function                       = "libfdata_stream_set_mapped_size";

	if( stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid stream.",
		 function );

		return( -1 );
	}
	internal_stream = (libfdata_internal_stream_t *) stream;

	if( mapped_size > internal_stream->size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid mapped size value out of bounds.",
		 function );

		return( -1 );
	}
	internal_stream->mapped_size = mapped_size;

	return( 1 );
}

/* Mapped range functions
 */

/* Retrieves the mapped range of a specific segment
 * Returns 1 if successful or -1 on error
 */
int libfdata_stream_get_segment_mapped_range(
     libfdata_stream_t *stream,
     int segment_index,
     off64_t *mapped_range_offset,
     size64_t *mapped_range_size,
     libcerror_error_t **error )
{
	libfdata_internal_stream_t *internal_stream = NULL;
	libfdata_mapped_range_t *mapped_range       = NULL;
	static char *function                       = "libfdata_stream_get_segment_mapped_range";

	if( stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid stream.",
		 function );

		return( -1 );
	}
	internal_stream = (libfdata_internal_stream_t *) stream;

	if( libcdata_array_get_entry_by_index(
	     internal_stream->mapped_ranges_array,
	     segment_index,
	     (intptr_t **) &mapped_range,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry: %d from mapped ranges array.",
		 function,
		 segment_index );

		return( -1 );
	}
	if( libfdata_mapped_range_get(
	     mapped_range,
	     mapped_range_offset,
	     mapped_range_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to retrieve values from mapped range: %d.",
		 function,
		 segment_index );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the segment index for a specific offset
 * The segment_data_offset value is set to the offset relative to the start of the segment
 * Returns 1 if successful, 0 if not or -1 on error
 */
int libfdata_stream_get_segment_index_at_offset(
     libfdata_stream_t *stream,
     off64_t offset,
     int *segment_index,
     off64_t *segment_data_offset,
     libcerror_error_t **error )
{
	libfdata_internal_stream_t *internal_stream = NULL;
	libfdata_mapped_range_t *mapped_range       = NULL;
	static char *function                       = "libfdata_stream_get_segment_index_at_offset";
	off64_t mapped_range_end_offset             = 0;
	off64_t mapped_range_start_offset           = 0;
	size64_t mapped_range_size                  = 0;
	size64_t stream_size                        = 0;
	int initial_segment_index                   = 0;
	int number_of_segments                      = 0;
	int result                                  = 0;
	int search_segment_index                    = 0;

#if defined( HAVE_DEBUG_OUTPUT )
	libfdata_range_t *segment_data_range        = NULL;
	off64_t segment_offset                      = 0;
	size64_t segment_size                       = 0;
	uint32_t segment_flags                      = 0;
	int segment_file_index                      = -1;
#endif

	if( stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid stream.",
		 function );

		return( -1 );
	}
	internal_stream = (libfdata_internal_stream_t *) stream;

	if( offset < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_LESS_THAN_ZERO,
		 "%s: invalid offset value less than zero.",
		 function );

		return( -1 );
	}
	if( segment_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid segment index.",
		 function );

		return( -1 );
	}
	if( segment_data_offset == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid segment data offset.",
		 function );

		return( -1 );
	}
	if( ( internal_stream->flags & LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES ) != 0 )
	{
		if( libfdata_segments_array_calculate_mapped_ranges(
		     internal_stream->segments_array,
		     internal_stream->mapped_ranges_array,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to calculate mapped ranges.",
			 function );

			return( -1 );
		}
		internal_stream->flags &= ~( LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES );
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: requested offset: 0x%08" PRIx64 "\n",
		 function,
		 offset );
	}
#endif
	if( internal_stream->mapped_size != 0 )
	{
		stream_size = internal_stream->mapped_size;
	}
	else
	{
		stream_size = internal_stream->size;
	}
	if( stream_size == 0 )
	{
		return( 0 );
	}
	if( libcdata_array_get_number_of_entries(
	     internal_stream->mapped_ranges_array,
	     &number_of_segments,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of entries from mapped ranges array.",
		 function );

		return( -1 );
	}
	/* This assumes a fairly even distribution of the sizes of the segments
	 */
	initial_segment_index = (int) ( ( number_of_segments * offset ) / stream_size );

	/* Look for the corresponding segment upwards in the array
	 */
	for( search_segment_index = initial_segment_index;
	     search_segment_index < number_of_segments;
	     search_segment_index++ )
	{
		if( libcdata_array_get_entry_by_index(
		     internal_stream->mapped_ranges_array,
		     search_segment_index,
		     (intptr_t **) &mapped_range,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve entry: %d from mapped ranges array.",
			 function,
			 search_segment_index );

			return( -1 );
		}
		if( libfdata_mapped_range_get(
		     mapped_range,
		     &mapped_range_start_offset,
		     &mapped_range_size,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to retrieve values from mapped range: %d.",
			 function,
			 search_segment_index );

			return( -1 );
		}
		mapped_range_end_offset = mapped_range_start_offset + (off64_t) mapped_range_size;

		if( mapped_range_end_offset < mapped_range_start_offset )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: invalid segment: %d - mapped range value out of bounds.",
			 function,
			 search_segment_index );

			return( -1 );
		}
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: segment: %03d\tmapped range: 0x%08" PRIx64 " - 0x%08" PRIx64 " (size: %" PRIu64 ")\n",
			 function,
			 search_segment_index,
			 mapped_range_start_offset,
			 mapped_range_end_offset,
			 mapped_range_size );
		}
#endif
		/* Check if the offset is in the mapped range
		 */
		if( ( offset >= mapped_range_start_offset )
		 && ( offset < mapped_range_end_offset ) )
		{
			offset -= mapped_range_start_offset;

			break;
		}
		/* Check if the offset is out of bounds
		 */
		if( offset < mapped_range_start_offset )
		{
			search_segment_index = number_of_segments;

			break;
		}
	}
	if( search_segment_index >= number_of_segments )
	{
		/* Look for the corresponding segment downwards in the array
		 */
		for( search_segment_index = initial_segment_index;
		     search_segment_index >= 0;
		     search_segment_index-- )
		{
			if( libcdata_array_get_entry_by_index(
			     internal_stream->mapped_ranges_array,
			     search_segment_index,
			     (intptr_t **) &mapped_range,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve entry: %d from mapped ranges array.",
				 function,
				 search_segment_index );

				return( -1 );
			}
			if( libfdata_mapped_range_get(
			     mapped_range,
			     &mapped_range_start_offset,
			     &mapped_range_size,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to retrieve values from mapped range: %d.",
				 function,
				 search_segment_index );

				return( -1 );
			}
			mapped_range_end_offset = mapped_range_start_offset + (off64_t) mapped_range_size;

			if( mapped_range_end_offset < mapped_range_start_offset )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: invalid segment: %d - mapped range value out of bounds.",
				 function,
				 search_segment_index );

				return( -1 );
			}
#if defined( HAVE_DEBUG_OUTPUT )
			if( libcnotify_verbose != 0 )
			{
				libcnotify_printf(
				 "%s: segment: %03d\tmapped range: 0x%08" PRIx64 " - 0x%08" PRIx64 " (size: %" PRIu64 ")\n",
				 function,
				 search_segment_index,
				 mapped_range_start_offset,
				 mapped_range_end_offset,
				 mapped_range_size );
			}
#endif
			/* Check if the offset is in the mapped range
			 */
			if( ( offset >= mapped_range_start_offset )
			 && ( offset < mapped_range_end_offset ) )
			{
				offset -= mapped_range_start_offset;

				break;
			}
			/* Check if the offset is out of bounds
			 */
			if( offset > mapped_range_start_offset )
			{
				search_segment_index--;

				break;
			}
		}
	}
	if( ( search_segment_index >= 0 )
	 && ( search_segment_index < number_of_segments ) )
	{
		if( offset < 0 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: invalid offset value out of bounds.",
			 function );

			return( -1 );
		}
		*segment_data_offset = offset;

#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			if( libcdata_array_get_entry_by_index(
			     internal_stream->segments_array,
			     search_segment_index,
			     (intptr_t **) &segment_data_range,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve entry: %d from segments array.",
				 function,
				 search_segment_index );

				return( -1 );
			}
			if( libfdata_range_get(
			     segment_data_range,
			     &segment_file_index,
			     &segment_offset,
			     &segment_size,
			     &segment_flags,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve segment: %d data range values.",
				 function,
				 search_segment_index );

				return( -1 );
			}
			libcnotify_printf(
			 "%s: segment: %03d\tfile index: %03d offset: 0x%08" PRIx64 " - 0x%08" PRIx64 " (size: %" PRIu64 ")\n",
			 function,
			 search_segment_index,
			 segment_file_index,
			 segment_offset,
			 segment_offset + segment_size,
			 segment_size );
		}
#endif
		result = 1;
	}
	if( result == 1 )
	{
		*segment_index = search_segment_index;
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "\n" );
	}
#endif
	return( result );
}

/* Retrieves the segment for a specific offset
 * The segment_data_offset value is set to the offset relative to the start of the segment
 * Returns 1 if successful, 0 if not or -1 on error
 */
int libfdata_stream_get_segment_at_offset(
     libfdata_stream_t *stream,
     off64_t offset,
     int *segment_index,
     off64_t *segment_data_offset,
     int *segment_file_index,
     off64_t *segment_offset,
     size64_t *segment_size,
     uint32_t *segment_flags,
     libcerror_error_t **error )
{
	libfdata_internal_stream_t *internal_stream = NULL;
	libfdata_range_t *segment_data_range        = NULL;
	static char *function                       = "libfdata_stream_get_segment_at_offset";
	int result                                  = 0;

	if( stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid stream.",
		 function );

		return( -1 );
	}
	internal_stream = (libfdata_internal_stream_t *) stream;

	if( segment_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid segment index.",
		 function );

		return( -1 );
	}
	result = libfdata_stream_get_segment_index_at_offset(
	          stream,
	          offset,
	          segment_index,
	          segment_data_offset,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve segment index at offset: 0x%08" PRIx64 ".",
		 function,
		 offset );

		return( -1 );
	}
	else if( result != 0 )
	{
		if( libcdata_array_get_entry_by_index(
		     internal_stream->segments_array,
		     *segment_index,
		     (intptr_t **) &segment_data_range,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve entry: %d from segments array.",
			 function,
			 *segment_index );

			return( -1 );
		}
		if( libfdata_range_get(
		     segment_data_range,
		     segment_file_index,
		     segment_offset,
		     segment_size,
		     segment_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve segment: %d data range values.",
			 function,
			 *segment_index );

			return( -1 );
		}
	}
	return( result );
}

/* IO functions
 */

/* Reads data from the current offset into a buffer
 * Returns the number of bytes read or -1 on error
 */
ssize_t libfdata_stream_read_buffer(
         libfdata_stream_t *stream,
         intptr_t *file_io_handle,
         uint8_t *buffer,
         size_t buffer_size,
         uint8_t read_flags,
         libcerror_error_t **error )
{
	libfdata_internal_stream_t *internal_stream = NULL;
	libfdata_range_t *segment_data_range        = NULL;
	static char *function                       = "libfdata_stream_read_buffer";
	off64_t result_offset                       = 0;
	off64_t segment_offset                      = 0;
	size64_t segment_size                       = 0;
	size64_t segment_data_size                  = 0;
	size64_t stream_size                        = 0;
	size_t buffer_offset                        = 0;
	size_t read_size                            = 0;
	ssize_t read_count                          = 0;
	uint32_t segment_flags                      = 0;
	int segment_file_index                      = 0;

	if( stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid stream.",
		 function );

		return( -1 );
	}
	internal_stream = (libfdata_internal_stream_t *) stream;

	if( internal_stream->read_segment_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid stream - missing read segment data function.",
		 function );

		return( -1 );
	}
	if( internal_stream->seek_segment_offset == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid stream - missing seek segment offset function.",
		 function );

		return( -1 );
	}
	if( buffer == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid buffer.",
		 function );

		return( -1 );
	}
	if( buffer_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid buffer size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( internal_stream->current_offset < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid stream - current offset value out of bounds.",
		 function );

		return( -1 );
	}
	if( internal_stream->mapped_size != 0 )
	{
		stream_size = internal_stream->mapped_size;
	}
	else
	{
		stream_size = internal_stream->size;
	}
	/* Bail out early for requests to read empty buffers and beyond the end of the stream
	 */
	if( ( buffer_size == 0 )
	 || ( (size64_t) internal_stream->current_offset >= stream_size ) )
	{
		return( 0 );
	}
	if( (size64_t) ( internal_stream->current_offset + buffer_size ) > stream_size )
	{
		buffer_size = (size_t) ( stream_size - internal_stream->current_offset );
	}
	if( libcdata_array_get_entry_by_index(
	     internal_stream->segments_array,
	     internal_stream->current_segment_index,
	     (intptr_t **) &segment_data_range,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry: %d from segments array.",
		 function,
		 internal_stream->current_segment_index );

		return( -1 );
	}
	if( libfdata_range_get(
	     segment_data_range,
	     &segment_file_index,
	     &segment_offset,
	     &segment_size,
	     &segment_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve segment: %d data range values.",
		 function,
		 internal_stream->current_segment_index );

		return( -1 );
	}
	if( ( internal_stream->segment_data_offset < 0 )
	 || ( (size64_t) internal_stream->segment_data_offset >= segment_size ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid stream - segment data offset value out of bounds.",
		 function );

		return( -1 );
	}
	segment_offset += internal_stream->segment_data_offset;

	segment_data_size = segment_size - internal_stream->segment_data_offset;

	result_offset = internal_stream->seek_segment_offset(
	                 internal_stream->data_handle,
	                 file_io_handle,
	                 internal_stream->current_segment_index,
	                 segment_file_index,
	                 segment_offset,
	                 error );

	if( result_offset != segment_offset )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_SEEK_FAILED,
		 "%s: unable to seek segment: %d offset: %" PRIi64 ".",
		 function,
		 internal_stream->current_segment_index,
		 segment_offset );

		return( -1 );
	}
	while( buffer_size > 0 )
	{
		if( (size64_t) buffer_size <= segment_data_size )
		{
			read_size = buffer_size;
		}
		else
		{
			read_size = (size_t) segment_data_size;
		}
		if( read_size == 0 )
		{
			break;
		}
		read_count = internal_stream->read_segment_data(
		              internal_stream->data_handle,
		              file_io_handle,
		              internal_stream->current_segment_index,
		              segment_file_index,
		              &( buffer[ buffer_offset ]),
		              read_size,
		              segment_flags,
		              read_flags,
		              error );

		if( read_count != (ssize_t) read_size )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_READ_FAILED,
			 "%s: unable to read segment: %d data at offset: 0x%08" PRIx64 ".",
			 function,
			 internal_stream->current_segment_index,
			 segment_offset );

			return( -1 );
		}
		internal_stream->current_offset      += read_size;
		internal_stream->segment_data_offset += read_size;
		segment_offset                       += read_size;
		segment_data_size                    -= read_size;
		buffer_size                          -= read_size;
		buffer_offset                        += read_size;

		if( (size64_t) internal_stream->current_offset >= stream_size )
		{
			break;
		}
		if( (size64_t) internal_stream->segment_data_offset >= segment_size )
		{
			if( (size64_t) internal_stream->segment_data_offset > segment_size )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: invalid stream - segment data offset value out of bounds.",
				 function );

				return( -1 );
			}
			internal_stream->current_segment_index++;

			internal_stream->segment_data_offset = 0;

			if( libcdata_array_get_entry_by_index(
			     internal_stream->segments_array,
			     internal_stream->current_segment_index,
			     (intptr_t **) &segment_data_range,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve entry: %d from segments array.",
				 function,
				 internal_stream->current_segment_index );

				return( -1 );
			}
			if( libfdata_range_get(
			     segment_data_range,
			     &segment_file_index,
			     &segment_offset,
			     &segment_size,
			     &segment_flags,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve segment: %d data range values.",
				 function,
				 internal_stream->current_segment_index );

				return( -1 );
			}
			segment_data_size = segment_size;

			result_offset = internal_stream->seek_segment_offset(
			                 internal_stream->data_handle,
			                 file_io_handle,
			                 internal_stream->current_segment_index,
			                 segment_file_index,
			                 segment_offset,
			                 error );

			if( result_offset != segment_offset )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_IO,
				 LIBCERROR_IO_ERROR_SEEK_FAILED,
				 "%s: unable to seek segment: %d offset: %" PRIi64 ".",
				 function,
				 internal_stream->current_segment_index,
				 segment_offset );

				return( -1 );
			}
		}
	}
	return( (ssize_t) buffer_offset );
}

/* Reads data at a specific offset into a buffer
 * Returns the number of bytes read or -1 on error
 */
ssize_t libfdata_stream_read_buffer_at_offset(
         libfdata_stream_t *stream,
         intptr_t *file_io_handle,
         uint8_t *buffer,
         size_t buffer_size,
         off64_t offset,
         uint8_t read_flags,
         libcerror_error_t **error )
{
	static char *function = "libfdata_stream_read_buffer_at_offset";
	ssize_t read_count    = 0;

	if( libfdata_stream_seek_offset(
	     stream,
	     offset,
	     SEEK_SET,
	     error ) == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_SEEK_FAILED,
		 "%s: unable to seek offset.",
		 function );

		return( -1 );
	}
	read_count = libfdata_stream_read_buffer(
	              stream,
	              file_io_handle,
	              buffer,
	              buffer_size,
	              read_flags,
	              error );

	if( read_count < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read buffer.",
		 function );

		return( -1 );
	}
	return( read_count );
}

/* Writes data in the buffer to the current offset
 * Returns the number of bytes written or -1 on error
 */
ssize_t libfdata_stream_write_buffer(
         libfdata_stream_t *stream,
         intptr_t *file_io_handle,
         const uint8_t *buffer,
         size_t buffer_size,
         uint8_t write_flags,
         libcerror_error_t **error )
{
	libfdata_internal_stream_t *internal_stream = NULL;
	libfdata_range_t *segment_data_range        = NULL;
	static char *function                       = "libfdata_stream_write_buffer";
	off64_t result_offset                       = 0;
	off64_t segment_offset                      = 0;
	size64_t segment_size                       = 0;
	size64_t segment_data_size                  = 0;
	size_t buffer_offset                        = 0;
	size_t write_size                           = 0;
	ssize_t write_count                         = 0;
	uint32_t segment_flags                      = 0;
	int number_of_segments                      = 0;
	int result                                  = 0;
	int segment_file_index                      = 0;
	int segment_index                           = 0;

	if( stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid stream.",
		 function );

		return( -1 );
	}
	internal_stream = (libfdata_internal_stream_t *) stream;

	if( internal_stream->create_segment == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid stream - missing create segment function.",
		 function );

		return( -1 );
	}
	if( internal_stream->write_segment_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid stream - missing write segment data function.",
		 function );

		return( -1 );
	}
	if( internal_stream->seek_segment_offset == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid stream - missing seek segment offset function.",
		 function );

		return( -1 );
	}
	if( buffer == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid buffer.",
		 function );

		return( -1 );
	}
	if( buffer_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid buffer size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( internal_stream->current_offset < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid stream - current offset value out of bounds.",
		 function );

		return( -1 );
	}
	/* Bail out early for requests to write empty buffers
	 */
	if( buffer_size == 0 )
	{
		return( 0 );
	}
	if( libcdata_array_get_number_of_entries(
	     internal_stream->segments_array,
	     &number_of_segments,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of entries from segments array.",
		 function );

		return( -1 );
	}
	if( (size64_t) internal_stream->current_offset < internal_stream->size )
	{
		if( libcdata_array_get_entry_by_index(
		     internal_stream->segments_array,
		     internal_stream->current_segment_index,
		     (intptr_t **) &segment_data_range,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve entry: %d from segments array.",
			 function,
			 internal_stream->current_segment_index );

			return( -1 );
		}
		if( libfdata_range_get(
		     segment_data_range,
		     &segment_file_index,
		     &segment_offset,
		     &segment_size,
		     &segment_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve segment: %d data range values.",
			 function,
			 internal_stream->current_segment_index );

			return( -1 );
		}
		if( ( internal_stream->segment_data_offset < 0 )
		 || ( (size64_t) internal_stream->segment_data_offset >= segment_size ) )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: invalid stream - segment data offset value out of bounds.",
			 function );

			return( -1 );
		}
		segment_offset += internal_stream->segment_data_offset;

		segment_data_size = segment_size - internal_stream->segment_data_offset;
	}
	else while( (size64_t) internal_stream->current_offset >= internal_stream->size )
	{
		if( internal_stream->current_segment_index < number_of_segments )
		{
			internal_stream->current_segment_index++;
		}
		internal_stream->segment_data_offset = 0;

		result = internal_stream->create_segment(
			  internal_stream->data_handle,
			  file_io_handle,
			  internal_stream->current_segment_index,
			  &segment_file_index,
			  &segment_offset,
			  &segment_size,
			  &segment_flags,
			  error );

		if( result == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create segment: %d.",
			 function,
			 internal_stream->current_segment_index );

			return( -1 );
		}
		else if( result == 0 )
		{
			return( 0 );
		}
		if( libfdata_stream_append_segment(
		     stream,
		     &segment_index,
		     segment_file_index,
		     segment_offset,
		     segment_size,
		     segment_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
			 "%s: unable to append segment to stream.",
			 function );

			return( -1 );
		}
		if( internal_stream->current_segment_index != segment_index )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: invalid segment index value out of bounds.",
			 function );

			return( -1 );
		}
		number_of_segments++;

		segment_data_size = segment_size;
	}
	result_offset = internal_stream->seek_segment_offset(
			 internal_stream->data_handle,
			 file_io_handle,
			 internal_stream->current_segment_index,
			 segment_file_index,
			 segment_offset,
			 error );

	if( result_offset != segment_offset )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_SEEK_FAILED,
		 "%s: unable to seek segment: %d offset: %" PRIi64 ".",
		 function,
		 internal_stream->current_segment_index,
		 segment_offset );

		return( -1 );
	}
	while( buffer_size > 0 )
	{
		if( (size64_t) buffer_size <= segment_data_size )
		{
			write_size = buffer_size;
		}
		else
		{
			write_size = (size_t) segment_data_size;
		}
		if( write_size == 0 )
		{
			break;
		}
		write_count = internal_stream->write_segment_data(
		               internal_stream->data_handle,
		               file_io_handle,
		               internal_stream->current_segment_index,
		               segment_file_index,
		               &( buffer[ buffer_offset ] ),
		               write_size,
		               segment_flags,
		               write_flags,
		               error );

		if( write_count != (ssize_t) write_size )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_WRITE_FAILED,
			 "%s: unable to write segment: %d data at offset: 0x%08" PRIx64 ".",
			 function,
			 internal_stream->current_segment_index,
			 segment_offset + internal_stream->segment_data_offset );

			return( -1 );
		}
		internal_stream->current_offset      += write_size;
		internal_stream->segment_data_offset += write_size;
		segment_offset                       += write_size;
		segment_data_size                    -= write_size;
		buffer_size                          -= write_size;
		buffer_offset                        += write_size;

		if( (size64_t) internal_stream->segment_data_offset >= segment_size )
		{
			if( (size64_t) internal_stream->segment_data_offset > segment_size )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: invalid stream - segment data offset value out of bounds.",
				 function );

				return( -1 );
			}
			internal_stream->current_segment_index++;

			internal_stream->segment_data_offset = 0;

			if( internal_stream->current_segment_index < number_of_segments )
			{
				if( libcdata_array_get_entry_by_index(
				     internal_stream->segments_array,
				     internal_stream->current_segment_index,
				     (intptr_t **) &segment_data_range,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve entry: %d from segments array.",
					 function,
					 internal_stream->current_segment_index );

					return( -1 );
				}
				if( libfdata_range_get(
				     segment_data_range,
				     &segment_file_index,
				     &segment_offset,
				     &segment_size,
				     &segment_flags,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve segment: %d data range values.",
					 function,
					 internal_stream->current_segment_index );

					return( -1 );
				}
			}
			else
			{
				result = internal_stream->create_segment(
					  internal_stream->data_handle,
					  file_io_handle,
					  internal_stream->current_segment_index,
					  &segment_file_index,
					  &segment_offset,
					  &segment_size,
					  &segment_flags,
					  error );

				if( result == -1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
					 "%s: unable to create segment: %d.",
					 function,
					 internal_stream->current_segment_index );

					return( -1 );
				}
				else if( result == 0 )
				{
					break;
				}
				if( libfdata_stream_append_segment(
				     stream,
				     &segment_index,
				     segment_file_index,
				     segment_offset,
				     segment_size,
				     segment_flags,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
					 "%s: unable to append segment to stream.",
					 function );

					return( -1 );
				}
				if( internal_stream->current_segment_index != segment_index )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
					 "%s: invalid segment index value out of bounds.",
					 function );

					return( -1 );
				}
				number_of_segments++;
			}
			segment_data_size = segment_size;

			result_offset = internal_stream->seek_segment_offset(
					 internal_stream->data_handle,
					 file_io_handle,
					 internal_stream->current_segment_index,
					 segment_file_index,
					 segment_offset,
					 error );

			if( result_offset != segment_offset )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_IO,
				 LIBCERROR_IO_ERROR_SEEK_FAILED,
				 "%s: unable to seek segment: %d offset: %" PRIi64 ".",
				 function,
				 internal_stream->current_segment_index,
				 segment_offset );

				return( -1 );
			}
		}
	}
	return( (ssize_t) buffer_offset );
}

/* Seeks a certain offset of the data
 * Returns the offset if seek is successful or -1 on error
 */
off64_t libfdata_stream_seek_offset(
         libfdata_stream_t *stream,
         off64_t offset,
         int whence,
         libcerror_error_t **error )
{
	libfdata_internal_stream_t *internal_stream = NULL;
	static char *function                       = "libfdata_stream_seek_offset";
	off64_t segment_data_offset                 = 0;
	size64_t stream_size                        = 0;
	int segment_index                           = 0;

	if( stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid stream.",
		 function );

		return( -1 );
	}
	internal_stream = (libfdata_internal_stream_t *) stream;

	if( internal_stream->current_offset < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid stream - current offset value out of bounds.",
		 function );

		return( -1 );
	}
	if( ( whence != SEEK_CUR )
	 && ( whence != SEEK_END )
	 && ( whence != SEEK_SET ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported whence.",
		 function );

		return( -1 );
	}
	if( internal_stream->mapped_size != 0 )
	{
		stream_size = internal_stream->mapped_size;
	}
	else
	{
		stream_size = internal_stream->size;
	}
	if( whence == SEEK_CUR )
	{
		offset += internal_stream->current_offset;
	}
	else if( whence == SEEK_END )
	{
		offset += (off64_t) stream_size;
	}
#if defined( HAVE_VERBOSE_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: seeking offset: %" PRIi64 ".\n",
		 function,
		 offset );
	}
#endif
	if( offset < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: offset value out of bounds.",
		 function );

		return( -1 );
	}
	if( offset < (off64_t) stream_size )
	{
		if( libfdata_stream_get_segment_index_at_offset(
		     stream,
		     offset,
		     &segment_index,
		     &segment_data_offset,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve segment index at offset: 0x%08" PRIx64 ".",
			 function,
			 offset );

			return( -1 );
		}
	}
	else
	{
		if( libcdata_array_get_number_of_entries(
		     internal_stream->segments_array,
		     &segment_index,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve number of entries from segments array.",
			 function );

			return( -1 );
		}
		segment_data_offset = 0;
	}
	internal_stream->current_segment_index = segment_index;
	internal_stream->current_offset        = offset;
	internal_stream->segment_data_offset   = segment_data_offset;

	return( offset );
}

/* Retrieves the offset
 * Returns 1 if successful or -1 on error
 */
int libfdata_stream_get_offset(
     libfdata_stream_t *stream,
     off64_t *offset,
     libcerror_error_t **error )
{
	libfdata_internal_stream_t *internal_stream = NULL;
	static char *function                       = "libfdata_stream_get_data_offset";

	if( stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid stream.",
		 function );

		return( -1 );
	}
	internal_stream = (libfdata_internal_stream_t *) stream;

	if( offset == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid offset.",
		 function );

		return( -1 );
	}
	*offset = internal_stream->current_offset;

	return( 1 );
}

/* Retrieves the size
 * Returns 1 if successful or -1 on error
 */
int libfdata_stream_get_size(
     libfdata_stream_t *stream,
     size64_t *size,
     libcerror_error_t **error )
{
	libfdata_internal_stream_t *internal_stream = NULL;
	static char *function                       = "libfdata_stream_get_size";

	if( stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid stream.",
		 function );

		return( -1 );
	}
	internal_stream = (libfdata_internal_stream_t *) stream;

	if( size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid size.",
		 function );

		return( -1 );
	}
	if( ( internal_stream->flags & LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES ) != 0 )
	{
		if( libfdata_segments_array_calculate_mapped_ranges(
		     internal_stream->segments_array,
		     internal_stream->mapped_ranges_array,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to calculate mapped ranges.",
			 function );

			return( -1 );
		}
		internal_stream->flags &= ~( LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES );
	}
	if( internal_stream->mapped_size != 0 )
	{
		*size = internal_stream->mapped_size;
	}
	else
	{
		*size = internal_stream->size;
	}
	return( 1 );
}

