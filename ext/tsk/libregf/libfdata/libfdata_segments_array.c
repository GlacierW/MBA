/*
 * The segments array functions
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

#include "libfdata_libcdata.h"
#include "libfdata_libcerror.h"
#include "libfdata_libcnotify.h"
#include "libfdata_mapped_range.h"
#include "libfdata_range.h"
#include "libfdata_segments_array.h"

/* Retrieves a specific segment
 * Returns 1 if successful or -1 on error
 */
int libfdata_segments_array_get_segment_by_index(
     libcdata_array_t *segments_array,
     int segment_index,
     int *segment_file_index,
     off64_t *segment_offset,
     size64_t *segment_size,
     uint32_t *segment_flags,
     libcerror_error_t **error )
{
	libfdata_range_t *segment_data_range = NULL;
	static char *function                = "libfdata_segments_array_get_segment_by_index";

	if( libcdata_array_get_entry_by_index(
	     segments_array,
	     segment_index,
	     (intptr_t **) &segment_data_range,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry: %d from segments array.",
		 function,
		 segment_index );

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
		 segment_index );

		return( -1 );
	}
	return( 1 );
}

/* Sets the offset and size of a specific segment
 * Returns 1 if successful or -1 on error
 */
int libfdata_segments_array_set_segment_by_index(
     libcdata_array_t *segments_array,
     libcdata_array_t *mapped_ranges_array,
     size64_t *data_size,
     int segment_index,
     int segment_file_index,
     off64_t segment_offset,
     size64_t segment_size,
     uint32_t segment_flags,
     libcerror_error_t **error )
{
	libfdata_mapped_range_t *mapped_range = NULL;
	libfdata_range_t *segment_data_range  = NULL;
	static char *function                 = "libfdata_segments_array_set_segment_by_index";
	off64_t previous_segment_offset       = 0;
	size64_t previous_segment_size        = 0;
	uint32_t previous_segment_flags       = 0;
	int previous_segment_file_index       = 0;

	if( data_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid data size.",
		 function );

		return( -1 );
	}
	if( segment_file_index < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid segment file index value out of bounds.",
		 function );

		return( -1 );
	}
	if( segment_offset < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid segment offset value out of bounds.",
		 function );

		return( -1 );
	}
	if( libcdata_array_get_entry_by_index(
	     segments_array,
	     segment_index,
	     (intptr_t **) &segment_data_range,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry: %d from segments array.",
		 function,
		 segment_index );

		return( -1 );
	}
	if( segment_data_range == NULL )
	{
		if( libfdata_range_initialize(
		     &segment_data_range,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create segment data range.",
			 function );

			return( -1 );
		}
		if( libcdata_array_set_entry_by_index(
		     segments_array,
		     segment_index,
		     (intptr_t *) segment_data_range,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set entry: %d to segments array.",
			 function,
			 segment_index );

			libfdata_range_free(
			 &segment_data_range,
			 NULL );

			return( -1 );
		}
	}
	else
	{
		if( libfdata_range_get(
		     segment_data_range,
		     &previous_segment_file_index,
		     &previous_segment_offset,
		     &previous_segment_size,
		     &previous_segment_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve segment: %d data range values.",
			 function,
			 segment_index );

			return( -1 );
		}
		*data_size -= previous_segment_size;
	}
	if( libfdata_range_set(
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
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set segment data range values.",
		 function );

		return( -1 );
	}
	/* Make sure there is a mapped range entry for every segment
	 */
	if( libcdata_array_get_entry_by_index(
	     mapped_ranges_array,
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
	if( mapped_range == NULL )
	{
		if( libfdata_mapped_range_initialize(
		     &mapped_range,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create mapped range.",
			 function );

			return( -1 );
		}
		if( libcdata_array_set_entry_by_index(
		     mapped_ranges_array,
		     segment_index,
		     (intptr_t *) mapped_range,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set entry: %d in mapped ranges array.",
			 function,
			 segment_index );

			libfdata_mapped_range_free(
			 &mapped_range,
			 NULL );

			return( -1 );
		}
	}
	*data_size += segment_size;

	return( 1 );
}

/* Prepends a segment
 * Returns 1 if successful or -1 on error
 */
int libfdata_segments_array_prepend_segment(
     libcdata_array_t *segments_array,
     libcdata_array_t *mapped_ranges_array,
     size64_t *data_size,
     int segment_file_index,
     off64_t segment_offset,
     size64_t segment_size,
     uint32_t segment_flags,
     libcerror_error_t **error )
{
	libfdata_mapped_range_t *mapped_range = NULL;
	libfdata_range_t *segment_data_range  = NULL;
	static char *function                 = "libfdata_segments_array_prepend_segment";
	int mapped_range_index                = -1;

	if( data_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid data size.",
		 function );

		return( -1 );
	}
	if( segment_file_index < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid segment file index value out of bounds.",
		 function );

		return( -1 );
	}
	if( segment_offset < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid segment offset value out of bounds.",
		 function );

		return( -1 );
	}
	if( segment_size > (size64_t) INT64_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid segment size value out of bounds.",
		 function );

		return( -1 );
	}
	if( libfdata_mapped_range_initialize(
	     &mapped_range,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create mapped range.",
		 function );

		goto on_error;
	}
	if( libfdata_mapped_range_set(
	     mapped_range,
	     (off64_t) *data_size,
	     segment_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set mapped range values.",
		 function );

		goto on_error;
	}
	if( libcdata_array_append_entry(
	     mapped_ranges_array,
	     &mapped_range_index,
	     (intptr_t *) mapped_range,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
		 "%s: unable to append mapped range to array.",
		 function );

		goto on_error;
	}
	if( libfdata_range_initialize(
	     &segment_data_range,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create segment data range.",
		 function );

		goto on_error;
	}
	if( libfdata_range_set(
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
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set segment data range values.",
		 function );

		goto on_error;
	}
	if( libcdata_array_prepend_entry(
	     segments_array,
	     (intptr_t *) segment_data_range,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
		 "%s: unable to prepend data range to segments array.",
		 function );

		goto on_error;
	}
	*data_size += segment_size;

	return( 1 );

on_error:
	if( segment_data_range != NULL )
	{
		libfdata_range_free(
		 &segment_data_range,
		 NULL );
	}
	if( mapped_range_index != -1 )
	{
		libcdata_array_set_entry_by_index(
		 mapped_ranges_array,
		 mapped_range_index,
		 NULL,
		 NULL );
	}
	if( mapped_range != NULL )
	{
		libfdata_mapped_range_free(
		 &mapped_range,
		 NULL );
	}
	return( -1 );
}

/* Appends a segment
 * Returns 1 if successful or -1 on error
 */
int libfdata_segments_array_append_segment(
     libcdata_array_t *segments_array,
     libcdata_array_t *mapped_ranges_array,
     size64_t *data_size,
     int *segment_index,
     int segment_file_index,
     off64_t segment_offset,
     size64_t segment_size,
     uint32_t segment_flags,
     libcerror_error_t **error )
{
	libfdata_mapped_range_t *mapped_range = NULL;
	libfdata_range_t *segment_data_range  = NULL;
	static char *function                 = "libfdata_segments_array_append_segment";
	int mapped_range_index                = -1;

	if( data_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid data size.",
		 function );

		return( -1 );
	}
	if( segment_file_index < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid segment file index value out of bounds.",
		 function );

		return( -1 );
	}
	if( segment_offset < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid segment offset value out of bounds.",
		 function );

		return( -1 );
	}
	if( libfdata_mapped_range_initialize(
	     &mapped_range,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create mapped range.",
		 function );

		goto on_error;
	}
	if( libfdata_mapped_range_set(
	     mapped_range,
	     (off64_t) *data_size,
	     segment_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set mapped range values.",
		 function );

		goto on_error;
	}
	if( libcdata_array_append_entry(
	     mapped_ranges_array,
	     &mapped_range_index,
	     (intptr_t *) mapped_range,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
		 "%s: unable to append mapped range to array.",
		 function );

		goto on_error;
	}
	if( libfdata_range_initialize(
	     &segment_data_range,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create segment data range.",
		 function );

		goto on_error;
	}
	if( libfdata_range_set(
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
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set segment data range values.",
		 function );

		goto on_error;
	}
	if( libcdata_array_append_entry(
	     segments_array,
	     segment_index,
	     (intptr_t *) segment_data_range,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
		 "%s: unable to append data range to segments array.",
		 function );

		goto on_error;
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: segment: %03d\tfile index: %03d offset: 0x%08" PRIx64 " - 0x%08" PRIx64 " (size: %" PRIu64 ")\n",
		 function,
		 *segment_index,
		 segment_file_index,
		 segment_offset,
		 segment_offset + segment_size,
		 segment_size );

		libcnotify_printf(
		 "%s: segment: %03d\tmapped range: 0x%08" PRIx64 " - 0x%08" PRIx64 " (size: %" PRIu64 ")\n",
		 function,
		 *segment_index,
		 *data_size,
		 *data_size + segment_size,
		 segment_size );

		libcnotify_printf(
		 "\n" );
	}
#endif
	*data_size += segment_size;

	return( 1 );

on_error:
	if( segment_data_range != NULL )
	{
		libfdata_range_free(
		 &segment_data_range,
		 NULL );
	}
	if( mapped_range_index != -1 )
	{
		libcdata_array_set_entry_by_index(
		 mapped_ranges_array,
		 mapped_range_index,
		 NULL,
		 NULL );
	}
	if( mapped_range != NULL )
	{
		libfdata_mapped_range_free(
		 &mapped_range,
		 NULL );
	}
	return( -1 );
}

/* Calculates the mapped ranges from the segments
 * Returns 1 if successful or -1 on error
 */
int libfdata_segments_array_calculate_mapped_ranges(
     libcdata_array_t *segments_array,
     libcdata_array_t *mapped_ranges_array,
     libcerror_error_t **error )
{
	libfdata_mapped_range_t *mapped_range = NULL;
	libfdata_range_t *segment_data_range  = NULL;
	static char *function                 = "libfdata_segments_array_calculate_mapped_ranges";
	off64_t mapped_offset                 = 0;
	off64_t segment_offset                = 0;
	size64_t segment_size                 = 0;
	uint32_t segment_flags                = 0;
	int number_of_segments                = 0;
	int segment_file_index                = 0;
	int segment_index                     = 0;

	if( libcdata_array_get_number_of_entries(
	     segments_array,
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
	for( segment_index = 0;
	     segment_index < number_of_segments;
	     segment_index++ )
	{
		if( libcdata_array_get_entry_by_index(
		     segments_array,
		     segment_index,
		     (intptr_t **) &segment_data_range,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve entry: %d from segments array.",
			 function,
			 segment_index );

			return( -1 );
		}
		if( libcdata_array_get_entry_by_index(
		     mapped_ranges_array,
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
			 segment_index );

			return( -1 );
		}
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: segment: %03d\tfile index: %03d offset: 0x%08" PRIx64 " - 0x%08" PRIx64 " (size: %" PRIu64 ")\n",
			 function,
			 segment_index,
			 segment_file_index,
			 segment_offset,
			 segment_offset + segment_size,
			 segment_size );

			libcnotify_printf(
			 "%s: segment: %03d\tmapped range: 0x%08" PRIx64 " - 0x%08" PRIx64 " (size: %" PRIu64 ")\n",
			 function,
			 segment_index,
			 mapped_offset,
			 mapped_offset + segment_size,
			 segment_size );
		}
#endif
		if( libfdata_mapped_range_set(
		     mapped_range,
		     mapped_offset,
		     segment_size,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set mapped range: %d values.",
			 function,
			 segment_index );

			return( -1 );
		}
		mapped_offset += (off64_t) segment_size;
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "\n" );
	}
#endif
	return( 1 );
}

/* Retrieves the segment data range for a specific offset
 * Returns 1 if successful or -1 on error
 */
int libfdata_segments_array_get_data_range_at_offset(
     libcdata_array_t *segments_array,
     off64_t value_offset,
     off64_t *segment_data_offset,
     libfdata_range_t **segment_data_range,
     libcerror_error_t **error )
{
	static char *function  = "libfdata_segments_array_get_data_range_at_offset";
	size64_t segment_size  = 0;
	int number_of_segments = 0;
	int segment_index      = 0;

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
	if( segment_data_range == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid segment data range.",
		 function );

		return( -1 );
	}
	if( libcdata_array_get_number_of_entries(
	     segments_array,
	     &number_of_segments,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of segments.",
		 function );

		return( -1 );
	}
	if( number_of_segments <= 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid number of segments value out of bounds.",
		 function );

		return( -1 );
	}
	for( segment_index = 0;
	     segment_index < number_of_segments;
	     segment_index++ )
	{
		if( libcdata_array_get_entry_by_index(
		     segments_array,
		     segment_index,
		     (intptr_t **) segment_data_range,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve segment data range: %d from array.",
			 function,
			 segment_index );

			return( -1 );
		}
		if( libfdata_range_get_size(
		     *segment_data_range,
		     &segment_size,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve size from segment data range: %d.",
			 function,
			 segment_index );

			return( -1 );
		}
/* TODO what about compressed data ranges */
		if( (size64_t) value_offset < segment_size )
		{
			*segment_data_offset = value_offset;

			break;
		}
		value_offset -= segment_size;
	}
	if( segment_index >= number_of_segments )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid segment index value out of bounds.",
		 function );

		return( -1 );
	}
	return( 1 );
}

