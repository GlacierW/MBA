/*
 * The list element functions
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

#include "libfdata_definitions.h"
#include "libfdata_libcerror.h"
#include "libfdata_libcnotify.h"
#include "libfdata_libfcache.h"
#include "libfdata_list.h"
#include "libfdata_list_element.h"
#include "libfdata_range.h"
#include "libfdata_types.h"

/* Creates an element
 * Make sure the value element is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_element_initialize(
     libfdata_list_element_t **element,
     libfdata_list_t *list,
     int element_index,
     libcerror_error_t **error )
{
	libfdata_internal_list_element_t *internal_element = NULL;
	static char *function                              = "libfdata_list_element_initialize";

	if( element == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid element.",
		 function );

		return( -1 );
	}
	if( *element != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid element value already set.",
		 function );

		return( -1 );
	}
	if( list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid list.",
		 function );

		return( -1 );
	}
	if( element_index < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_LESS_THAN_ZERO,
		 "%s: invalid element index value less than zero.",
		 function );

		return( -1 );
	}
	internal_element = memory_allocate_structure(
	                    libfdata_internal_list_element_t );

	if( internal_element == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create element.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_element,
	     0,
	     sizeof( libfdata_internal_list_element_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear element.",
		 function );

		memory_free(
		 internal_element );

		return( -1 );
	}
	if( libfdata_range_initialize(
	     &( internal_element->data_range ),
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create data range.",
		 function );

		goto on_error;
	}
	if( libfcache_date_time_get_timestamp(
	     &( internal_element->timestamp ),
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
	internal_element->list          = list;
	internal_element->element_index = element_index;

	*element = (libfdata_list_element_t *) internal_element;

	return( 1 );

on_error:
	if( internal_element != NULL )
	{
		if( internal_element->data_range != NULL )
		{
			libfdata_range_free(
			 &( internal_element->data_range ),
			 NULL );
		}
		memory_free(
		 internal_element );
	}
	return( -1 );
}

/* Frees an element
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_element_free(
     libfdata_list_element_t **element,
     libcerror_error_t **error )
{
	libfdata_internal_list_element_t *internal_element = NULL;
	static char *function                              = "libfdata_list_element_free";
	int result                                         = 1;

	if( element == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid element.",
		 function );

		return( -1 );
	}
	if( *element != NULL )
	{
		internal_element = (libfdata_internal_list_element_t *) *element;
		*element         = NULL;

		if( libfdata_range_free(
		     &( internal_element->data_range ),
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free data range.",
			 function );

			result = -1;
		}
		memory_free(
		 internal_element );
	}
	return( result );
}

/* Clones (duplicates) the element
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_element_clone(
     libfdata_list_element_t **destination_element,
     libfdata_list_element_t *source_element,
     libfdata_list_t *list,
     int element_index,
     libcerror_error_t **error )
{
	libfdata_internal_list_element_t *internal_destination_element = NULL;
	libfdata_internal_list_element_t *internal_source_element      = NULL;
	static char *function                                          = "libfdata_list_element_clone";

	if( destination_element == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid destination element.",
		 function );

		return( -1 );
	}
	if( *destination_element != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: destination element already set.",
		 function );

		return( -1 );
	}
	if( list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid list.",
		 function );

		return( -1 );
	}
	if( element_index < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_LESS_THAN_ZERO,
		 "%s: invalid element index value less than zero.",
		 function );

		return( -1 );
	}
	if( source_element == NULL )
	{
		*destination_element = NULL;

		return( 1 );
	}
	internal_source_element = (libfdata_internal_list_element_t *) source_element;

	internal_destination_element = memory_allocate_structure(
	                                libfdata_internal_list_element_t );

	if( internal_destination_element == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create destination element.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_destination_element,
	     0,
	     sizeof( libfdata_internal_list_element_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear destination element.",
		 function );

		memory_free(
		 internal_destination_element );

		return( -1 );
	}
	if( libfdata_range_clone(
	     &( internal_destination_element->data_range ),
	     internal_source_element->data_range,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create destination data range.",
		 function );

		goto on_error;
	}
	if( libfcache_date_time_get_timestamp(
	     &( internal_destination_element->timestamp ),
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve destination timestamp.",
		 function );

		goto on_error;
	}
	internal_destination_element->list          = list;
	internal_destination_element->element_index = element_index;
	internal_destination_element->mapped_size   = internal_source_element->mapped_size;

	*destination_element = (libfdata_list_element_t *) internal_destination_element;

	return( 1 );

on_error:
	if( internal_destination_element != NULL )
	{
		if( internal_destination_element->data_range != NULL )
		{
			libfdata_range_free(
			 &( internal_destination_element->data_range ),
			 NULL );
		}
		memory_free(
		 internal_destination_element );
	}
	return( -1 );
}

/* Retrieves the element index
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_element_get_element_index(
     libfdata_list_element_t *element,
     int *element_index,
     libcerror_error_t **error )
{
	libfdata_internal_list_element_t *internal_element = NULL;
	static char *function                              = "libfdata_list_element_get_element_index";

	if( element == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid element.",
		 function );

		return( -1 );
	}
	internal_element = (libfdata_internal_list_element_t *) element;

	if( element_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid element index.",
		 function );

		return( -1 );
	}
	*element_index = internal_element->element_index;

	return( 1 );
}

/* Sets the element index
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_element_set_element_index(
     libfdata_list_element_t *element,
     int element_index,
     libcerror_error_t **error )
{
	libfdata_internal_list_element_t *internal_element = NULL;
	static char *function                              = "libfdata_list_element_set_element_index";

	if( element == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid element.",
		 function );

		return( -1 );
	}
	internal_element = (libfdata_internal_list_element_t *) element;

	if( element_index < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_LESS_THAN_ZERO,
		 "%s: invalid element index value less than zero.",
		 function );

		return( -1 );
	}
	internal_element->element_index = element_index;

	return( 1 );
}

/* Retrieves the time stamp
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_element_get_timestamp(
     libfdata_list_element_t *element,
     time_t *timestamp,
     libcerror_error_t **error )
{
	libfdata_internal_list_element_t *internal_element = NULL;
	static char *function                              = "libfdata_list_element_get_timestamp";

	if( element == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid element.",
		 function );

		return( -1 );
	}
	internal_element = (libfdata_internal_list_element_t *) element;

	if( timestamp == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid time stamp.",
		 function );

		return( -1 );
	}
	*timestamp = internal_element->timestamp;

	return( 1 );
}

/* Data range functions
 */

/* Retrieves the data range
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_element_get_data_range(
     libfdata_list_element_t *element,
     int *file_index,
     off64_t *offset,
     size64_t *size,
     uint32_t *flags,
     libcerror_error_t **error )
{
	libfdata_internal_list_element_t *internal_element = NULL;
	static char *function                              = "libfdata_list_element_get_data_range";

	if( element == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid element.",
		 function );

		return( -1 );
	}
	internal_element = (libfdata_internal_list_element_t *) element;

	if( libfdata_range_get(
	     internal_element->data_range,
	     file_index,
	     offset,
	     size,
	     flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve data range.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Sets the data range
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_element_set_data_range(
     libfdata_list_element_t *element,
     int file_index,
     off64_t offset,
     size64_t size,
     uint32_t flags,
     libcerror_error_t **error )
{
	libfdata_internal_list_element_t *internal_element = NULL;
	static char *function                              = "libfdata_list_element_set_data_range";

	if( element == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid element.",
		 function );

		return( -1 );
	}
	internal_element = (libfdata_internal_list_element_t *) element;

	if( file_index < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_LESS_THAN_ZERO,
		 "%s: invalid file index less than zero.",
		 function );

		return( -1 );
	}
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
	if( size > (size64_t) INT64_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( libfdata_range_set(
	     internal_element->data_range,
	     file_index,
	     offset,
	     size,
	     flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set data range.",
		 function );

		return( -1 );
	}
	if( libfcache_date_time_get_timestamp(
	     &( internal_element->timestamp ),
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve cache timestamp.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Mapped range functions
 */

/* Retrieves the mapped size
 * Returns 1 if successful, 0 if not set or -1 on error
 */
int libfdata_list_element_get_mapped_size(
     libfdata_list_element_t *element,
     size64_t *mapped_size,
     libcerror_error_t **error )
{
	libfdata_internal_list_element_t *internal_element = NULL;
	static char *function                              = "libfdata_list_element_get_mapped_size";

	if( element == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid element.",
		 function );

		return( -1 );
	}
	internal_element = (libfdata_internal_list_element_t *) element;

	if( mapped_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid mapped size.",
		 function );

		return( -1 );
	}
	if( ( internal_element->flags & LIBFDATA_LIST_ELEMENT_FLAG_HAS_MAPPED_SIZE ) == 0 )
	{
		return( 0 );
	}
	*mapped_size = internal_element->mapped_size;

	return( 1 );
}

/* Sets the mapped size
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_element_set_mapped_size(
     libfdata_list_element_t *element,
     size64_t mapped_size,
     libcerror_error_t **error )
{
	libfdata_internal_list_element_t *internal_element = NULL;
	static char *function                              = "libfdata_list_element_set_mapped_size";

	if( element == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid element.",
		 function );

		return( -1 );
	}
	internal_element = (libfdata_internal_list_element_t *) element;

	if( mapped_size > (size64_t) INT64_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid mapped size value exceeds maximum.",
		 function );

		return( -1 );
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: element: %03d\tmapped size: %" PRIu64 "\n",
		 function,
		 internal_element->element_index,
		 mapped_size );

		libcnotify_printf(
		 "\n" );
	}
#endif
	internal_element->mapped_size = mapped_size;
	internal_element->flags      |= LIBFDATA_LIST_ELEMENT_FLAG_HAS_MAPPED_SIZE;

	if( libfdata_list_set_calculate_mapped_ranges_flag(
	     internal_element->list,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set the calculate mapped ranges flag in list.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Element value functions
 */

/* Retrieves the element value
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_element_get_element_value(
     libfdata_list_element_t *element,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     intptr_t **element_value,
     uint8_t read_flags,
     libcerror_error_t **error )
{
	libfdata_internal_list_element_t *internal_element = NULL;
	static char *function                              = "libfdata_list_element_get_element_value";

	if( element == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid element.",
		 function );

		return( -1 );
	}
	internal_element = (libfdata_internal_list_element_t *) element;

	if( libfdata_list_get_element_value(
	     internal_element->list,
	     file_io_handle,
	     cache,
	     element,
	     element_value,
	     read_flags,
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

/* Sets the element value
 *
 * If the flag LIBFDATA_LIST_ELEMENT_VALUE_FLAG_MANAGED is set the list element
 * takes over management of the value and the value is freed when
 * no longer needed.
 *
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_element_set_element_value(
     libfdata_list_element_t *element,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     intptr_t *element_value,
     int (*free_element_value)(
            intptr_t **element_value,
            libcerror_error_t **error ),
     uint8_t write_flags,
     libcerror_error_t **error )
{
	libfdata_internal_list_element_t *internal_element = NULL;
	static char *function                              = "libfdata_list_element_set_element_value";

	if( element == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid element.",
		 function );

		return( -1 );
	}
	internal_element = (libfdata_internal_list_element_t *) element;

	if( libfdata_list_set_element_value(
	     internal_element->list,
	     file_io_handle,
	     cache,
	     element,
	     element_value,
	     free_element_value,
	     write_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set element value.",
		 function );

		return( -1 );
	}
	return( 1 );
}

