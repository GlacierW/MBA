/*
 * The area functions
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

#include "libfdata_area.h"
#include "libfdata_definitions.h"
#include "libfdata_libcdata.h"
#include "libfdata_libcerror.h"
#include "libfdata_libcnotify.h"
#include "libfdata_libfcache.h"
#include "libfdata_mapped_range.h"
#include "libfdata_range.h"
#include "libfdata_segments_array.h"
#include "libfdata_types.h"
#include "libfdata_unused.h"

/* Creates an area
 * Make sure the value area is referencing, is set to NULL
 *
 * If the flag LIBFDATA_DATA_HANDLE_FLAG_MANAGED is set the area
 * takes over management of the data handle and the data handle is freed when
 * no longer needed
 *
 * Returns 1 if successful or -1 on error
 */
int libfdata_area_initialize(
     libfdata_area_t **area,
     size64_t element_data_size,
     intptr_t *data_handle,
     int (*free_data_handle)(
            intptr_t **data_handle,
            libcerror_error_t **error ),
     int (*clone_data_handle)(
            intptr_t **destination_data_handle,
            intptr_t *source_data_handle,
            libcerror_error_t **error ),
     int (*read_element_data)(
            intptr_t *data_handle,
            intptr_t *file_io_handle,
            libfdata_area_t *area,
            libfcache_cache_t *cache,
            off64_t element_value_offset,
            int element_data_file_index,
            off64_t element_data_offset,
            size64_t element_data_size,
            uint32_t element_data_flags,
            uint8_t read_flags,
            libcerror_error_t **error ),
     int (*write_element_data)(
            intptr_t *data_handle,
            intptr_t *file_io_handle,
            libfdata_area_t *area,
            libfcache_cache_t *cache,
            off64_t element_value_offset,
            int element_data_file_index,
            off64_t element_data_offset,
            size64_t element_data_size,
            uint32_t element_data_flags,
            uint8_t write_flags,
            libcerror_error_t **error ),
     uint8_t flags,
     libcerror_error_t **error )
{
	libfdata_internal_area_t *internal_area = NULL;
	static char *function                   = "libfdata_area_initialize";

	if( area == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid area.",
		 function );

		return( -1 );
	}
	if( *area != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid area value already set.",
		 function );

		return( -1 );
	}
	if( element_data_size == 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_ZERO_OR_LESS,
		 "%s: invalid element data size value zero or less.",
		 function );

		return( -1 );
	}
	internal_area = memory_allocate_structure(
	                 libfdata_internal_area_t );

	if( internal_area == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create area.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_area,
	     0,
	     sizeof( libfdata_internal_area_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear area.",
		 function );

		memory_free(
		 internal_area );

		return( -1 );
	}
	if( libcdata_array_initialize(
	     &( internal_area->segments_array ),
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
	     &( internal_area->mapped_ranges_array ),
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
	if( libfcache_date_time_get_timestamp(
	     &( internal_area->timestamp ),
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve cache timestamp.",
		 function );

		goto on_error;
	}
	internal_area->element_data_size  = element_data_size;
	internal_area->flags             |= flags;
	internal_area->data_handle        = data_handle;
	internal_area->free_data_handle   = free_data_handle;
	internal_area->clone_data_handle  = clone_data_handle;
	internal_area->read_element_data  = read_element_data;
	internal_area->write_element_data = write_element_data;

	*area = (libfdata_area_t *) internal_area;

	return( 1 );

on_error:
	if( internal_area != NULL )
	{
		if( internal_area->segments_array != NULL )
		{
			libcdata_array_free(
			 &( internal_area->segments_array ),
			 NULL,
			 NULL );
		}
		memory_free(
		 internal_area );
	}
	return( -1 );
}

/* Frees an area
 * Returns 1 if successful or -1 on error
 */
int libfdata_area_free(
     libfdata_area_t **area,
     libcerror_error_t **error )
{
	libfdata_internal_area_t *internal_area = NULL;
	static char *function                   = "libfdata_area_free";
	int result                              = 1;

	if( area == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid area.",
		 function );

		return( -1 );
	}
	if( *area != NULL )
	{
		internal_area = (libfdata_internal_area_t *) *area;
		*area         = NULL;

		if( libcdata_array_free(
		     &( internal_area->segments_array ),
		     (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_range_free,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free segments array.",
			 function );

			result = -1;
		}
		if( libcdata_array_free(
		     &( internal_area->mapped_ranges_array ),
		     (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_mapped_range_free,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free mapped ranges array.",
			 function );

			result = -1;
		}
		if( ( internal_area->flags & LIBFDATA_DATA_HANDLE_FLAG_MANAGED ) != 0 )
		{
			if( internal_area->data_handle != NULL )
			{
				if( internal_area->free_data_handle == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: invalid area - missing free data handle function.",
					 function );

					result = -1;
				}
				else if( internal_area->free_data_handle(
				          &( internal_area->data_handle ),
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
		 internal_area );
	}
	return( result );
}

/* Clones (duplicates) the area
 * Returns 1 if successful or -1 on error
 */
int libfdata_area_clone(
     libfdata_area_t **destination_area,
     libfdata_area_t *source_area,
     libcerror_error_t **error )
{
	libfdata_internal_area_t *internal_destination_area = NULL;
	libfdata_internal_area_t *internal_source_area      = NULL;
	static char *function                               = "libfdata_area_clone";

	if( destination_area == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid destination area.",
		 function );

		return( -1 );
	}
	if( *destination_area != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid destination area value already set.",
		 function );

		return( -1 );
	}
	if( source_area == NULL )
	{
		*destination_area = source_area;

		return( 1 );
	}
	internal_source_area = (libfdata_internal_area_t *) source_area;

	internal_destination_area = memory_allocate_structure(
	                             libfdata_internal_area_t );

	if( internal_destination_area == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create destination area.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_destination_area,
	     0,
	     sizeof( libfdata_internal_area_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear destination area.",
		 function );

		memory_free(
		 internal_destination_area );

		return( -1 );
	}
	if( internal_source_area->data_handle != NULL )
	{
		if( internal_source_area->free_data_handle == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: invalid source area - missing free data handle function.",
			 function );

			goto on_error;
		}
		if( internal_source_area->clone_data_handle == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: invalid source area - missing clone data handle function.",
			 function );

			goto on_error;
		}
		if( internal_source_area->clone_data_handle(
		     &( internal_destination_area->data_handle ),
		     internal_source_area->data_handle,
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
	     &( internal_destination_area->segments_array ),
	     internal_source_area->segments_array,
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
	     &( internal_destination_area->mapped_ranges_array ),
	     internal_source_area->mapped_ranges_array,
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
	internal_destination_area->element_data_size  = internal_source_area->element_data_size;
	internal_destination_area->timestamp          = internal_source_area->timestamp;
	internal_destination_area->flags              = internal_source_area->flags | LIBFDATA_DATA_HANDLE_FLAG_MANAGED;
	internal_destination_area->data_handle        = internal_source_area->data_handle;
	internal_destination_area->free_data_handle   = internal_source_area->free_data_handle;
	internal_destination_area->clone_data_handle  = internal_source_area->clone_data_handle;
	internal_destination_area->read_element_data  = internal_source_area->read_element_data;
	internal_destination_area->write_element_data = internal_source_area->write_element_data;

	return( 1 );

on_error:
	if( internal_destination_area != NULL )
	{
		if( internal_destination_area->segments_array != NULL )
		{
			libcdata_array_free(
			 &( internal_destination_area->segments_array ),
			 (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_range_free,
			 NULL );
		}
		if( ( internal_destination_area->data_handle != NULL )
		 && ( internal_source_area->free_data_handle != NULL ) )
		{
			internal_source_area->free_data_handle(
			 &( internal_destination_area->data_handle ),
			 NULL );
		}
		memory_free(
		 internal_destination_area );
	}
	return( -1 );
}

/* Segment functions
 */

/* Empties the area
 * Returns 1 if successful or -1 on error
 */
int libfdata_area_empty(
     libfdata_area_t *area,
     libcerror_error_t **error )
{
	libfdata_internal_area_t *internal_area = NULL;
	static char *function                   = "libfdata_area_empty";

	if( area == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid area.",
		 function );

		return( -1 );
	}
	internal_area = (libfdata_internal_area_t *) area;

	if( libcdata_array_empty(
	     internal_area->segments_array,
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
	     internal_area->mapped_ranges_array,
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
	internal_area->size = 0;

	return( 1 );
}

/* Resizes the segments
 * Returns 1 if successful or -1 on error
 */
int libfdata_area_resize_segments(
     libfdata_area_t *area,
     int number_of_segments,
     libcerror_error_t **error )
{
	libfdata_internal_area_t *internal_area = NULL;
	static char *function                   = "libfdata_area_resize_segments";

	if( area == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid area.",
		 function );

		return( -1 );
	}
	internal_area = (libfdata_internal_area_t *) area;

	if( libcdata_array_resize(
	     internal_area->segments_array,
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
	     internal_area->mapped_ranges_array,
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
	internal_area->flags |= LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES;

	return( 1 );
}

/* Retrieves the number of segments of the area
 * Returns 1 if successful or -1 on error
 */
int libfdata_area_get_number_of_segments(
     libfdata_area_t *area,
     int *number_of_segments,
     libcerror_error_t **error )
{
	libfdata_internal_area_t *internal_area = NULL;
	static char *function                   = "libfdata_area_get_number_of_segments";

	if( area == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid area.",
		 function );

		return( -1 );
	}
	internal_area = (libfdata_internal_area_t *) area;

	if( libcdata_array_get_number_of_entries(
	     internal_area->segments_array,
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

/* Retrieves the offset and size of a specific segment
 * Returns 1 if successful or -1 on error
 */
int libfdata_area_get_segment_by_index(
     libfdata_area_t *area,
     int segment_index,
     int *segment_file_index,
     off64_t *segment_offset,
     size64_t *segment_size,
     uint32_t *segment_flags,
     libcerror_error_t **error )
{
	libfdata_internal_area_t *internal_area = NULL;
	static char *function                   = "libfdata_area_get_segment_by_index";

	if( area == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid area.",
		 function );

		return( -1 );
	}
	internal_area = (libfdata_internal_area_t *) area;

	if( libfdata_segments_array_get_segment_by_index(
	     internal_area->segments_array,
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

/* Sets the offset and size of a specific segment
 * Returns 1 if successful or -1 on error
 */
int libfdata_area_set_segment_by_index(
     libfdata_area_t *area,
     int segment_index,
     int segment_file_index,
     off64_t segment_offset,
     size64_t segment_size,
     uint32_t segment_flags,
     libcerror_error_t **error )
{
	libfdata_internal_area_t *internal_area = NULL;
	static char *function                   = "libfdata_area_set_segment_by_index";

	if( area == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid area.",
		 function );

		return( -1 );
	}
	internal_area = (libfdata_internal_area_t *) area;

	if( libfdata_segments_array_set_segment_by_index(
	     internal_area->segments_array,
	     internal_area->mapped_ranges_array,
	     &( internal_area->size ),
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
	internal_area->flags |= LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES;

	return( 1 );
}

/* Prepends a segment
 * Returns 1 if successful or -1 on error
 */
int libfdata_area_prepend_segment(
     libfdata_area_t *area,
     int segment_file_index,
     off64_t segment_offset,
     size64_t segment_size,
     uint32_t segment_flags,
     libcerror_error_t **error )
{
	libfdata_internal_area_t *internal_area = NULL;
	static char *function                   = "libfdata_area_prepend_segment";

	if( area == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid area.",
		 function );

		return( -1 );
	}
	internal_area = (libfdata_internal_area_t *) area;

	if( libfdata_segments_array_prepend_segment(
	     internal_area->segments_array,
	     internal_area->mapped_ranges_array,
	     &( internal_area->size ),
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
	internal_area->flags |= LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES;

	return( 1 );
}

/* Appends a segment
 * Returns 1 if successful or -1 on error
 */
int libfdata_area_append_segment(
     libfdata_area_t *area,
     int *segment_index,
     int segment_file_index,
     off64_t segment_offset,
     size64_t segment_size,
     uint32_t segment_flags,
     libcerror_error_t **error )
{
	libfdata_internal_area_t *internal_area = NULL;
	static char *function                   = "libfdata_area_append_segment";

	if( area == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid area.",
		 function );

		return( -1 );
	}
	internal_area = (libfdata_internal_area_t *) area;

	if( libfdata_segments_array_append_segment(
	     internal_area->segments_array,
	     internal_area->mapped_ranges_array,
	     &( internal_area->size ),
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

/* Area element functions
 */

/* Retrieves the element data size of the area
 * Returns 1 if successful or -1 on error
 */
int libfdata_area_get_element_data_size(
     libfdata_area_t *area,
     size64_t *element_data_size,
     libcerror_error_t **error )
{
	libfdata_internal_area_t *internal_area = NULL;
	static char *function                   = "libfdata_area_get_element_data_size";

	if( area == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid area.",
		 function );

		return( -1 );
	}
	internal_area = (libfdata_internal_area_t *) area;

	if( element_data_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid element data size.",
		 function );

		return( -1 );
	}
	*element_data_size = internal_area->element_data_size;

	return( 1 );
}

/* Area element value functions
 */

/* Retrieves the value an element at a specific offset
 * Returns 1 if successful or -1 on error
 */
int libfdata_area_get_element_value_at_offset(
     libfdata_area_t *area,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     off64_t element_value_offset,
     intptr_t **element_value,
     uint8_t read_flags,
     libcerror_error_t **error )
{
	libfcache_cache_value_t *cache_value    = NULL;
	libfdata_internal_area_t *internal_area = NULL;
	libfdata_range_t *segment_data_range    = NULL;
	static char *function                   = "libfdata_area_get_element_value_at_offset";
	off64_t cache_value_offset              = (off64_t) -1;
	off64_t element_data_offset             = (off64_t) -1;
	time_t cache_value_timestamp            = 0;
	uint32_t element_data_flags             = 0;
	int cache_entry_index                   = -1;
	int cache_value_file_index              = -1;
	int element_data_file_index             = -1;
	int element_index                       = -1;
	int number_of_cache_entries             = 0;
	int result                              = 0;

	if( area == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid area.",
		 function );

		return( -1 );
	}
	internal_area = (libfdata_internal_area_t *) area;

	if( internal_area->read_element_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid area - missing read element data function.",
		 function );

		return( -1 );
	}
	if( internal_area->element_data_size == 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid area - element data size value out of bounds.",
		 function );

		return( -1 );
	}
	if( ( internal_area->size == 0 )
	 || ( internal_area->size > (off64_t) INT64_MAX ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid area - size value out of bounds.",
		 function );

		return( -1 );
	}
	if( ( (size64_t) element_value_offset > internal_area->size )
	 || ( (size64_t) element_value_offset > ( internal_area->size - internal_area->element_data_size ) ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid element value offset value out of bounds.",
		 function );

		return( -1 );
	}
	if( libfdata_segments_array_get_data_range_at_offset(
	     internal_area->segments_array,
	     element_value_offset,
	     &element_data_offset,
	     &segment_data_range,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve segment data range for offset: 0x%08" PRIx64 ".",
		 function,
		 element_value_offset );

		return( -1 );
	}
	if( segment_data_range == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing segment data range.",
		 function );

		return( -1 );
	}
	element_data_file_index = segment_data_range->file_index;
	element_data_offset    += segment_data_range->offset;
	element_data_flags      = segment_data_range->flags;

	if( libfcache_cache_get_number_of_entries(
	     cache,
	     &number_of_cache_entries,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of cache entries.",
		 function );

		return( -1 );
	}
	if( number_of_cache_entries <= 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid number of cache entries value out of bounds.",
		 function );

		return( -1 );
	}
	element_index = (int) ( element_value_offset / internal_area->element_data_size );

	if( ( read_flags & LIBFDATA_READ_FLAG_IGNORE_CACHE ) == 0 )
	{
		if( internal_area->calculate_cache_entry_index == NULL )
		{
			cache_entry_index = element_index % number_of_cache_entries;
		}
		else
		{
			cache_entry_index = internal_area->calculate_cache_entry_index(
			                     element_index,
			                     element_data_file_index,
			                     element_data_offset,
			                     internal_area->element_data_size,
			                     element_data_flags,
			                     number_of_cache_entries );
		}
		if( libfcache_cache_get_value_by_index(
		     cache,
		     cache_entry_index,
		     &cache_value,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve cache entry: %d from cache.",
			 function,
			 cache_entry_index );

			return( -1 );
		}
		if( cache_value != NULL )
		{
			if( libfcache_cache_value_get_identifier(
			     cache_value,
			     &cache_value_file_index,
			     &cache_value_offset,
			     &cache_value_timestamp,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve cache value identifier.",
				 function );

				return( -1 );
			}
		}
		if( ( element_data_file_index == cache_value_file_index )
		 && ( element_data_offset == cache_value_offset )
		 && ( internal_area->timestamp == cache_value_timestamp ) )
		{
			result = 1;
		}
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			if( result == 0 )
			{
				libcnotify_printf(
				 "%s: cache: 0x%08" PRIjx " miss (%d out of %d)\n",
				 function,
				 (intptr_t) cache,
				 cache_entry_index,
				 number_of_cache_entries );
			}
			else
			{
				libcnotify_printf(
				 "%s: cache: 0x%08" PRIjx " hit (%d out of %d)\n",
				 function,
				 (intptr_t) cache,
				 cache_entry_index,
				 number_of_cache_entries );
			}
		}
#endif
	}
	if( result == 0 )
	{
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: reading element data at offset: %" PRIi64 " (0x%08" PRIx64 ") of size: %" PRIu64 "\n",
			 function,
			 element_data_offset,
			 element_data_offset,
			 internal_area->element_data_size );
		}
#endif
		if( internal_area->read_element_data(
		     internal_area->data_handle,
		     file_io_handle,
		     area,
		     cache,
		     element_value_offset,
		     element_data_file_index,
		     element_data_offset,
		     internal_area->element_data_size,
		     element_data_flags,
		     read_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_READ_FAILED,
			 "%s: unable to read element data at offset: 0x%08" PRIx64 ".",
			 function,
			 element_data_offset );

			return( -1 );
		}
		if( internal_area->calculate_cache_entry_index == NULL )
		{
			cache_entry_index = element_index % number_of_cache_entries;
		}
		else
		{
			cache_entry_index = internal_area->calculate_cache_entry_index(
			                     element_index,
			                     element_data_file_index,
			                     element_data_offset,
			                     internal_area->element_data_size,
			                     element_data_flags,
			                     number_of_cache_entries );
		}
		if( libfcache_cache_get_value_by_index(
		     cache,
		     cache_entry_index,
		     &cache_value,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve cache entry: %d from cache.",
			 function,
			 cache_entry_index );

			return( -1 );
		}
		if( cache_value != NULL )
		{
			if( libfcache_cache_value_get_identifier(
			     cache_value,
			     &cache_value_file_index,
			     &cache_value_offset,
			     &cache_value_timestamp,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve cache value identifier.",
				 function );

				return( -1 );
			}
		}
		if( ( element_data_file_index != cache_value_file_index )
		 || ( element_data_offset != cache_value_offset )
		 || ( internal_area->timestamp != cache_value_timestamp ) )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing cache value.",
			 function );

			return( -1 );
		}
	}
	if( libfcache_cache_value_get_value(
	     cache_value,
	     element_value,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve element value.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Sets the value of a specific element
 *
 * If the flag LIBFDATA_VECTOR_ELEMENT_VALUE_FLAG_MANAGED is set the area
 * takes over management of the value and the value is freed when
 * no longer needed.
 *
 * Returns 1 if successful or -1 on error
 */
int libfdata_area_set_element_value_at_offset(
     libfdata_area_t *area,
     intptr_t *file_io_handle LIBFDATA_ATTRIBUTE_UNUSED,
     libfcache_cache_t *cache,
     off64_t element_value_offset,
     intptr_t *element_value,
     int (*free_element_value)(
            intptr_t **element_value,
            libcerror_error_t **error ),
     uint8_t write_flags,
     libcerror_error_t **error )
{
	libfdata_internal_area_t *internal_area = NULL;
	libfdata_range_t *segment_data_range    = NULL;
	static char *function                   = "libfdata_area_set_element_value_at_offset";
	off64_t element_data_offset             = (off64_t) -1;
	uint32_t element_data_flags             = 0;
	int cache_entry_index                   = -1;
	int element_data_file_index             = -1;
	int element_index                       = -1;
	int number_of_cache_entries             = 0;

	LIBFDATA_UNREFERENCED_PARAMETER( file_io_handle )

	if( area == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid area.",
		 function );

		return( -1 );
	}
	internal_area = (libfdata_internal_area_t *) area;

	if( internal_area->element_data_size == 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid area - element data size value out of bounds.",
		 function );

		return( -1 );
	}
	if( ( internal_area->size == 0 )
	 || ( internal_area->size > (off64_t) INT64_MAX ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid area - size value out of bounds.",
		 function );

		return( -1 );
	}
	if( ( (size64_t) element_value_offset > internal_area->size )
	 || ( (size64_t) element_value_offset > ( internal_area->size - internal_area->element_data_size ) ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid element value offset value out of bounds.",
		 function );

		return( -1 );
	}
	if( libfdata_segments_array_get_data_range_at_offset(
	     internal_area->segments_array,
	     element_value_offset,
	     &element_data_offset,
	     &segment_data_range,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve segment data range for offset: 0x%08" PRIx64 ".",
		 function,
		 element_value_offset );

		return( -1 );
	}
	if( segment_data_range == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing segment data range.",
		 function );

		return( -1 );
	}
	element_data_file_index = segment_data_range->file_index;
	element_data_offset    += segment_data_range->offset;
	element_data_flags      = segment_data_range->flags;

	if( libfcache_cache_get_number_of_entries(
	     cache,
	     &number_of_cache_entries,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of cache entries.",
		 function );

		return( -1 );
	}
	if( number_of_cache_entries <= 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid number of cache entries value out of bounds.",
		 function );

		return( -1 );
	}
	element_index = (int) ( element_value_offset / internal_area->element_data_size );

	if( internal_area->calculate_cache_entry_index == NULL )
	{
		cache_entry_index = element_index % number_of_cache_entries;
	}
	else
	{
		cache_entry_index = internal_area->calculate_cache_entry_index(
		                     element_index,
		                     element_data_file_index,
		                     element_data_offset,
		                     internal_area->element_data_size,
		                     element_data_flags,
		                     number_of_cache_entries );
	}
	if( libfcache_cache_set_value_by_index(
	     cache,
	     cache_entry_index,
	     element_data_file_index,
	     element_data_offset,
	     internal_area->timestamp,
	     element_value,
	     free_element_value,
	     write_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set value in cache entry: %d.",
		 function,
		 cache_entry_index );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the size
 * Returns 1 if successful or -1 on error
 */
int libfdata_area_get_size(
     libfdata_area_t *area,
     size64_t *size,
     libcerror_error_t **error )
{
	libfdata_internal_area_t *internal_area = NULL;
	static char *function                   = "libfdata_area_get_size";

	if( area == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid area.",
		 function );

		return( -1 );
	}
	internal_area = (libfdata_internal_area_t *) area;

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
	if( ( internal_area->flags & LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES ) != 0 )
	{
		if( libfdata_segments_array_calculate_mapped_ranges(
		     internal_area->segments_array,
		     internal_area->mapped_ranges_array,
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
		internal_area->flags &= ~( LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES );
	}
	*size = internal_area->size;

	return( 1 );
}

