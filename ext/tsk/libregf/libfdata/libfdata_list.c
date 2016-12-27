/*
 * The list functions
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
#include "libfdata_libcdata.h"
#include "libfdata_libcerror.h"
#include "libfdata_libcnotify.h"
#include "libfdata_libfcache.h"
#include "libfdata_list.h"
#include "libfdata_list_element.h"
#include "libfdata_mapped_range.h"
#include "libfdata_range.h"
#include "libfdata_types.h"
#include "libfdata_unused.h"

/* Creates a list
 * Make sure the value list is referencing, is set to NULL
 *
 * If the flag LIBFDATA_DATA_HANDLE_FLAG_MANAGED is set the list
 * takes over management of the data handle and the data handle is freed when
 * no longer needed
 *
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_initialize(
     libfdata_list_t **list,
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
            libfdata_list_element_t *list_element,
            libfcache_cache_t *cache,
            int element_data_file_index,
            off64_t element_data_offset,
            size64_t element_data_size,
            uint32_t element_data_flags,
            uint8_t read_flags,
            libcerror_error_t **error ),
     int (*write_element_data)(
            intptr_t *data_handle,
            intptr_t *file_io_handle,
            libfdata_list_element_t *list_element,
            libfcache_cache_t *cache,
            int element_data_file_index,
            off64_t element_data_offset,
            size64_t element_data_size,
            uint32_t element_data_flags,
            uint8_t write_flags,
            libcerror_error_t **error ),
     uint8_t flags,
     libcerror_error_t **error )
{
	libfdata_internal_list_t *internal_list = NULL;
	static char *function                   = "libfdata_list_initialize";

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
	if( *list != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid list value already set.",
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
	internal_list = memory_allocate_structure(
	                 libfdata_internal_list_t );

	if( internal_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create list.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_list,
	     0,
	     sizeof( libfdata_internal_list_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear list.",
		 function );

		memory_free(
		 internal_list );

		return( -1 );
	}
	if( libcdata_array_initialize(
	     &( internal_list->elements_array ),
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create elements array.",
		 function );

		goto on_error;
	}
	if( libcdata_array_initialize(
	     &( internal_list->mapped_ranges_array ),
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
	internal_list->flags             |= flags;
	internal_list->data_handle        = data_handle;
	internal_list->free_data_handle   = free_data_handle;
	internal_list->clone_data_handle  = clone_data_handle;
	internal_list->read_element_data  = read_element_data;
	internal_list->write_element_data = write_element_data;

	*list = (libfdata_list_t *) internal_list;

	return( 1 );

on_error:
	if( internal_list != NULL )
	{
		if( internal_list->elements_array != NULL )
		{
			libcdata_array_free(
			 &( internal_list->elements_array ),
			 NULL,
			 NULL );
		}
		memory_free(
		 internal_list );
	}
	return( -1 );
}

/* Frees a list
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_free(
     libfdata_list_t **list,
     libcerror_error_t **error )
{
	libfdata_internal_list_t *internal_list = NULL;
	static char *function                   = "libfdata_list_free";
	int result                              = 1;

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
	if( *list != NULL )
	{
		internal_list = (libfdata_internal_list_t *) *list;
		*list         = NULL;

		if( libcdata_array_free(
		     &( internal_list->elements_array ),
		     (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_list_element_free,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free the elements array.",
			 function );

			result = -1;
		}
		if( libcdata_array_free(
		     &( internal_list->mapped_ranges_array ),
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
		if( ( internal_list->flags & LIBFDATA_DATA_HANDLE_FLAG_MANAGED ) != 0 )
		{
			if( internal_list->data_handle != NULL )
			{
				if( internal_list->free_data_handle == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: invalid list - missing free data handle function.",
					 function );

					result = -1;
				}
				else if( internal_list->free_data_handle(
				          &( internal_list->data_handle ),
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
		 internal_list );
	}
	return( result );
}

/* Clones (duplicates) the list
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_clone(
     libfdata_list_t **destination_list,
     libfdata_list_t *source_list,
     libcerror_error_t **error )
{
	libfdata_internal_list_t *internal_destination_list = NULL;
	libfdata_internal_list_t *internal_source_list      = NULL;
	static char *function                               = "libfdata_list_clone";

	if( destination_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid destination list.",
		 function );

		return( -1 );
	}
	if( *destination_list != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid destination list value already set.",
		 function );

		return( -1 );
	}
	if( source_list == NULL )
	{
		*destination_list = NULL;

		return( 1 );
	}
	internal_source_list = (libfdata_internal_list_t *) source_list;

	internal_destination_list = memory_allocate_structure(
	                             libfdata_internal_list_t );

	if( internal_destination_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create destination list.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_destination_list,
	     0,
	     sizeof( libfdata_internal_list_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear destination list.",
		 function );

		memory_free(
		 internal_destination_list );

		return( -1 );
	}
	if( internal_source_list->data_handle != NULL )
	{
		if( internal_source_list->free_data_handle == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: invalid source list - missing free data handle function.",
			 function );

			goto on_error;
		}
		if( internal_source_list->clone_data_handle == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: invalid source list - missing clone data handle function.",
			 function );

			goto on_error;
		}
		if( internal_source_list->clone_data_handle(
		     &( internal_destination_list->data_handle ),
		     internal_source_list->data_handle,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to clone data handle.",
			 function );

			goto on_error;
		}
	}
	if( libcdata_array_clone(
	     &( internal_destination_list->elements_array ),
	     internal_source_list->elements_array,
	     (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_list_element_free,
	     (int (*)(intptr_t **, intptr_t *, libcerror_error_t **)) &libfdata_list_element_clone,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create destination elements array.",
		 function );

		goto on_error;
	}
	if( libcdata_array_clone(
	     &( internal_destination_list->mapped_ranges_array ),
	     internal_source_list->mapped_ranges_array,
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
	internal_destination_list->flags              = internal_source_list->flags | LIBFDATA_DATA_HANDLE_FLAG_MANAGED;
	internal_destination_list->free_data_handle   = internal_source_list->free_data_handle;
	internal_destination_list->clone_data_handle  = internal_source_list->clone_data_handle;
	internal_destination_list->read_element_data  = internal_source_list->read_element_data;
	internal_destination_list->write_element_data = internal_source_list->write_element_data;

	*destination_list = (libfdata_list_t *) internal_destination_list;

	return( 1 );

on_error:
	if( internal_destination_list != NULL )
	{
		if( internal_destination_list->elements_array != NULL )
		{
			libcdata_array_free(
			 &( internal_destination_list->elements_array ),
			 (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_list_element_free,
			 NULL );
		}
		if( ( internal_destination_list->data_handle != NULL )
		 && ( internal_source_list->free_data_handle != NULL ) )
		{
			internal_source_list->free_data_handle(
			 &( internal_destination_list->data_handle ),
			 NULL );
		}
		memory_free(
		 internal_destination_list );
	}
	return( -1 );
}

/* Sets the calculate mapped ranges flag
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_set_calculate_mapped_ranges_flag(
     libfdata_list_t *list,
     libcerror_error_t **error )
{
	libfdata_internal_list_t *internal_list = NULL;
	static char *function                   = "libfdata_list_set_calculate_mapped_ranges_flag";

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
	internal_list = (libfdata_internal_list_t *) list;

	internal_list->flags |= LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES;

	return( 1 );
}

/* List elements functions
 */

/* Empties the list
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_empty(
     libfdata_list_t *list,
     libcerror_error_t **error )
{
	libfdata_internal_list_t *internal_list = NULL;
	static char *function                   = "libfdata_list_empty";

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
	internal_list = (libfdata_internal_list_t *) list;

	if( libcdata_array_empty(
	     internal_list->elements_array,
	     (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_list_element_free,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_RESIZE_FAILED,
		 "%s: unable to empty elements array.",
		 function );

		return( -1 );
	}
	if( libcdata_array_empty(
	     internal_list->mapped_ranges_array,
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
	internal_list->size = 0;

	return( 1 );
}

/* Resizes the list
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_resize(
     libfdata_list_t *list,
     int number_of_elements,
     libcerror_error_t **error )
{
	libfdata_internal_list_t *internal_list = NULL;
	static char *function                   = "libfdata_list_resize";

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
	internal_list = (libfdata_internal_list_t *) list;

	if( libcdata_array_resize(
	     internal_list->elements_array,
	     number_of_elements,
	     (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_list_element_free,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_RESIZE_FAILED,
		 "%s: unable to resize elements array.",
		 function );

		return( -1 );
	}
	if( libcdata_array_resize(
	     internal_list->mapped_ranges_array,
	     number_of_elements,
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
	internal_list->flags |= LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES;

	return( 1 );
}

/* Reverses the order of the elements
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_reverse(
     libfdata_list_t *list,
     libcerror_error_t **error )
{
	libfdata_internal_list_t *internal_list = NULL;
	libfdata_list_element_t *list_element   = NULL;
	static char *function                   = "libfdata_list_reverse";
	int element_index                       = 0;
	int number_of_elements                  = 0;

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
	internal_list = (libfdata_internal_list_t *) list;

	if( libcdata_array_reverse(
	     internal_list->elements_array,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to reverse elements array.",
		 function );

		return( -1 );
	}
	if( libcdata_array_get_number_of_entries(
	     internal_list->elements_array,
	     &number_of_elements,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of elements from elements array.",
		 function );

		return( -1 );
	}
	for( element_index = 0;
	     element_index < number_of_elements;
	     element_index++ )
	{
		if( libcdata_array_get_entry_by_index(
		     internal_list->elements_array,
		     element_index,
		     (intptr_t **) &list_element,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve entry: %d from elements array.",
			 function,
			 element_index );

			return( -1 );
		}
		if( libfdata_list_element_set_element_index(
		     list_element,
		     element_index,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set list element: %d index.",
			 function,
			 element_index );

			return( -1 );
		}
	}
	internal_list->flags |= LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES;

	return( 1 );
}

/* Retrieves the number of elements of the list
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_get_number_of_elements(
     libfdata_list_t *list,
     int *number_of_elements,
     libcerror_error_t **error )
{
	libfdata_internal_list_t *internal_list = NULL;
	static char *function                   = "libfdata_list_get_number_of_elements";

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
	internal_list = (libfdata_internal_list_t *) list;

	if( libcdata_array_get_number_of_entries(
	     internal_list->elements_array,
	     number_of_elements,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of elements from elements array.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves a specific list element
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_get_list_element_by_index(
     libfdata_list_t *list,
     int element_index,
     libfdata_list_element_t **element,
     libcerror_error_t **error )
{
	libfdata_internal_list_t *internal_list = NULL;
	static char *function                   = "libfdata_list_get_list_element_by_index";

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
	internal_list = (libfdata_internal_list_t *) list;

	if( libcdata_array_get_entry_by_index(
	     internal_list->elements_array,
	     element_index,
	     (intptr_t **) element,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry: %d from elements array.",
		 function,
		 element_index );

		return( -1 );
	}
	internal_list->current_element_index = element_index;

	return( 1 );
}

/* Retrieves the data range of a specific element
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_get_element_by_index(
     libfdata_list_t *list,
     int element_index,
     int *element_file_index,
     off64_t *element_offset,
     size64_t *element_size,
     uint32_t *element_flags,
     libcerror_error_t **error )
{
	libfdata_internal_list_t *internal_list = NULL;
	libfdata_list_element_t *list_element   = NULL;
	static char *function                   = "libfdata_list_get_element_by_index";

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
	internal_list = (libfdata_internal_list_t *) list;

	if( libcdata_array_get_entry_by_index(
	     internal_list->elements_array,
	     element_index,
	     (intptr_t **) &list_element,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry: %d from elements array.",
		 function,
		 element_index );

		return( -1 );
	}
	if( libfdata_list_element_get_data_range(
	     list_element,
	     element_file_index,
	     element_offset,
	     element_size,
	     element_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve data range from list element: %d.",
		 function,
		 element_index );

		return( -1 );
	}
	internal_list->current_element_index = element_index;

	return( 1 );
}

/* Sets the data range of a specific element
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_set_element_by_index(
     libfdata_list_t *list,
     int element_index,
     int element_file_index,
     off64_t element_offset,
     size64_t element_size,
     uint32_t element_flags,
     libcerror_error_t **error )
{
	libfdata_internal_list_t *internal_list = NULL;
	libfdata_list_element_t *list_element   = NULL;
	libfdata_mapped_range_t *mapped_range   = NULL;
	static char *function                   = "libfdata_list_set_element_by_index";
	off64_t previous_element_offset         = 0;
	size64_t mapped_size                    = 0;
	size64_t previous_element_size          = 0;
	uint32_t previous_element_flags         = 0;
	int previous_element_file_index         = 0;
	int result                              = 0;

#if defined( HAVE_DEBUG_OUTPUT )
	off64_t mapped_range_offset             = 0;
	size64_t mapped_range_size              = 0;
#endif

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
	internal_list = (libfdata_internal_list_t *) list;

	if( libcdata_array_get_entry_by_index(
	     internal_list->elements_array,
	     element_index,
	     (intptr_t **) &list_element,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry: %d from elements array.",
		 function,
		 element_index );

		return( -1 );
	}
	if( list_element == NULL )
	{
		if( libfdata_list_element_initialize(
		     &list_element,
		     list,
		     element_index,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create list element.",
			 function );

			return( -1 );
		}
		if( libcdata_array_set_entry_by_index(
		     internal_list->elements_array,
		     element_index,
		     (intptr_t *) list_element,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set entry: %d in elements array.",
			 function,
			 element_index );

			libfdata_list_element_free(
			 &list_element,
			 NULL );

			return( -1 );
		}
	}
	else
	{
		result = libfdata_list_element_get_mapped_size(
		          list_element,
		          &mapped_size,
		          error );

		if( result == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve mapped size of list element: %d.",
			 function,
			 element_index );

			return( -1 );
		}
		else if( result == 0 )
		{
			if( libfdata_list_element_get_data_range(
			     list_element,
			     &previous_element_file_index,
			     &previous_element_offset,
			     &previous_element_size,
			     &previous_element_flags,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve data range of list element: %d.",
				 function,
				 element_index );

				return( -1 );
			}
		}
	}
	if( libfdata_list_element_set_data_range(
	     list_element,
	     element_file_index,
	     element_offset,
	     element_size,
	     element_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set data range of list element: %d.",
		 function,
		 element_index );

		return( -1 );
	}
	/* Make sure the list has a mapped range entry for every element
	 */
	if( libcdata_array_get_entry_by_index(
	     internal_list->mapped_ranges_array,
	     element_index,
	     (intptr_t **) &mapped_range,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry: %d from mapped ranges array.",
		 function,
		 element_index );

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
		     internal_list->mapped_ranges_array,
		     element_index,
		     (intptr_t *) mapped_range,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set entry: %d in mapped ranges array.",
			 function,
			 element_index );

			libfdata_mapped_range_free(
			 &mapped_range,
			 NULL );

			return( -1 );
		}
		internal_list->size  += element_size;
		internal_list->flags |= LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES;
	}
	/* If the size of the element is mapped or if the element size did not change
	 * there is no need to recalculate the mapped range
	 */
	else if( ( mapped_size == 0 )
	      && ( previous_element_size != element_size ) )
	{
		internal_list->size  -= previous_element_size;
		internal_list->size  += element_size;
		internal_list->flags |= LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES;
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: element: %03d\tfile index: %03d offset: 0x%08" PRIx64 " - 0x%08" PRIx64 " (size: %" PRIu64 ")\n",
		 function,
		 element_index,
		 element_file_index,
		 element_offset,
		 element_offset + element_size,
		 element_size );

		if( ( internal_list->flags & LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES ) == 0 )
		{
			if( libfdata_mapped_range_get(
			     mapped_range,
			     &mapped_range_offset,
			     &mapped_range_size,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve values from mapped range: %d.",
				 function,
				 element_index );

				return( -1 );
			}
			libcnotify_printf(
			 "%s: element: %03d\tmapped range: 0x%08" PRIx64 " - 0x%08" PRIx64 " (size: %" PRIu64 ")\n",
			 function,
			 element_index,
			 mapped_range_offset,
			 mapped_range_offset + mapped_range_size,
			 mapped_range_size );
		}
		libcnotify_printf(
		 "\n" );
	}
#endif
	internal_list->current_element_index = element_index;

	return( 1 );
}

/* Prepends an element data range
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_prepend_element(
     libfdata_list_t *list,
     int element_file_index,
     off64_t element_offset,
     size64_t element_size,
     uint32_t element_flags,
     libcerror_error_t **error )
{
	libfdata_internal_list_t *internal_list = NULL;
	libfdata_list_element_t *list_element   = NULL;
	libfdata_mapped_range_t *mapped_range   = NULL;
	static char *function                   = "libfdata_list_prepend_element";
	off64_t mapped_offset                   = 0;
	int element_index                       = 0;
	int mapped_range_index                  = -1;
	int number_of_elements                  = 0;

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
	internal_list = (libfdata_internal_list_t *) list;

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
	mapped_offset = internal_list->mapped_offset + (off64_t) internal_list->size;

	if( libfdata_mapped_range_set(
	     mapped_range,
	     mapped_offset,
	     element_size,
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
	     internal_list->mapped_ranges_array,
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
	if( libfdata_list_element_initialize(
	     &list_element,
	     list,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create list element.",
		 function );

		goto on_error;
	}
	if( libfdata_list_element_set_data_range(
	     list_element,
	     element_file_index,
	     element_offset,
	     element_size,
	     element_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set data range of list element.",
		 function );

		goto on_error;
	}
	if( libcdata_array_prepend_entry(
	     internal_list->elements_array,
	     (intptr_t *) list_element,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
		 "%s: unable to prepend list element to elements array.",
		 function );

		goto on_error;
	}
	mapped_range_index = -1;
	mapped_range       = NULL;

	if( libcdata_array_get_number_of_entries(
	     internal_list->elements_array,
	     &number_of_elements,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of entries from elements array.",
		 function );

		goto on_error;
	}
	for( element_index = 0;
	     element_index < number_of_elements;
	     element_index++ )
	{
		if( libcdata_array_get_entry_by_index(
		     internal_list->elements_array,
		     element_index,
		     (intptr_t **) &list_element,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve entry: %d from elements array.",
			 function,
			 element_index );

			list_element = NULL;

			goto on_error;
		}
		if( libfdata_list_element_set_element_index(
		     list_element,
		     element_index,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set list element: %d index.",
			 function,
			 element_index );

			list_element = NULL;

			goto on_error;
		}
	}
	internal_list->current_element_index = 0;
	internal_list->size                 += element_size;
	internal_list->flags                |= LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES;

	return( 1 );

on_error:
	if( list_element != NULL )
	{
		libfdata_list_element_free(
		 &list_element,
		 NULL );
	}
	if( mapped_range_index != -1 )
	{
		libcdata_array_set_entry_by_index(
		 internal_list->mapped_ranges_array,
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

/* Appends an element data range
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_append_element(
     libfdata_list_t *list,
     int *element_index,
     int element_file_index,
     off64_t element_offset,
     size64_t element_size,
     uint32_t element_flags,
     libcerror_error_t **error )
{
	libfdata_internal_list_t *internal_list = NULL;
	libfdata_list_element_t *list_element   = NULL;
	libfdata_mapped_range_t *mapped_range   = NULL;
	static char *function                   = "libfdata_list_append_element";
	off64_t mapped_offset                   = 0;
	int mapped_range_index                  = -1;

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
	internal_list = (libfdata_internal_list_t *) list;

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
	mapped_offset = internal_list->mapped_offset + (off64_t) internal_list->size;

	if( libfdata_mapped_range_set(
	     mapped_range,
	     mapped_offset,
	     element_size,
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
	     internal_list->mapped_ranges_array,
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
	if( libfdata_list_element_initialize(
	     &list_element,
	     list,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create list element.",
		 function );

		goto on_error;
	}
	if( libfdata_list_element_set_data_range(
	     list_element,
	     element_file_index,
	     element_offset,
	     element_size,
	     element_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set data range of list element.",
		 function );

		goto on_error;
	}
	if( libcdata_array_append_entry(
	     internal_list->elements_array,
	     element_index,
	     (intptr_t *) list_element,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
		 "%s: unable to append list element to elements array.",
		 function );

		goto on_error;
	}
	mapped_range_index = -1;
	mapped_range       = NULL;

	if( libfdata_list_element_set_element_index(
	     list_element,
	     *element_index,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set list element index.",
		 function );

		list_element = NULL;

		goto on_error;
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: element: %03d\tfile index: %03d offset: 0x%08" PRIx64 " - 0x%08" PRIx64 " (size: %" PRIu64 ")\n",
		 function,
		 *element_index,
		 element_file_index,
		 element_offset,
		 element_offset + element_size,
		 element_size );

		libcnotify_printf(
		 "%s: element: %03d\tmapped range: 0x%08" PRIx64 " - 0x%08" PRIx64 " (size: %" PRIu64 ")\n",
		 function,
		 *element_index,
		 mapped_offset,
		 mapped_offset + element_size,
		 element_size );

		libcnotify_printf(
		 "\n" );
	}
#endif
	internal_list->current_element_index = *element_index;
	internal_list->size                 += element_size;

	return( 1 );

on_error:
	if( list_element != NULL )
	{
		libfdata_list_element_free(
		 &list_element,
		 NULL );
	}
	if( mapped_range_index != -1 )
	{
		libcdata_array_set_entry_by_index(
		 internal_list->mapped_ranges_array,
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

/* Appends the element of the source list to the list
 * The source list is emptied if successful
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_append_list(
     libfdata_list_t *list,
     libfdata_list_t *source_list,
     libcerror_error_t **error )
{
	libfdata_internal_list_t *internal_list        = NULL;
	libfdata_internal_list_t *internal_source_list = NULL;
	libfdata_list_element_t *list_element          = NULL;
	libfdata_mapped_range_t *mapped_range          = NULL;
	static char *function                          = "libfdata_list_append_list";
	off64_t mapped_range_offset                    = 0;
	size64_t mapped_range_size                     = 0;
	int element_index                              = 0;
	int new_number_of_elements                     = 0;
	int number_of_elements                         = 0;
	int result                                     = 1;
	int source_element_index                       = 0;
	int source_number_of_elements                  = 0;

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
	internal_list = (libfdata_internal_list_t *) list;

	if( source_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid source list.",
		 function );

		return( -1 );
	}
	internal_source_list = (libfdata_internal_list_t *) source_list;

	if( ( internal_list->flags & LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES ) != 0 )
	{
		if( libfdata_list_calculate_mapped_ranges(
		     internal_list,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to calculate mapped ranges.",
			 function );

			goto on_error;
		}
	}
	if( ( internal_source_list->flags & LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES ) != 0 )
	{
		if( libfdata_list_calculate_mapped_ranges(
		     internal_source_list,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to calculate mapped ranges of source list.",
			 function );

			goto on_error;
		}
	}
	if( ( internal_list->mapped_offset != 0 )
	 || ( internal_source_list->mapped_offset != 0 ) )
	{
		if( ( (size64_t) internal_list->mapped_offset + internal_list->size ) != (size64_t) internal_source_list->mapped_offset )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: invalid source mapped offset value out of bounds.",
			 function );

			goto on_error;
		}
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: appending source list with mapped offset: %" PRIi64 " and size: %" PRIu64 " to list with mapped offset: %" PRIi64 " and size: %" PRIu64 "\n",
		 function,
		 internal_source_list->mapped_offset,
		 internal_source_list->size,
		 internal_list->mapped_offset,
		 internal_list->size );
	}
#endif
	if( libcdata_array_get_number_of_entries(
	     internal_list->elements_array,
	     &number_of_elements,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of elements from elements array.",
		 function );

		goto on_error;
	}
	if( libcdata_array_get_number_of_entries(
	     internal_source_list->elements_array,
	     &source_number_of_elements,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of elements from source list elements array.",
		 function );

		goto on_error;
	}
	element_index          = number_of_elements;
	new_number_of_elements = number_of_elements + source_number_of_elements;

	if( libcdata_array_resize(
	     internal_list->elements_array,
	     new_number_of_elements,
	     (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_list_element_free,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_RESIZE_FAILED,
		 "%s: unable to resize elements array.",
		 function );

		goto on_error;
	}
	if( libcdata_array_resize(
	     internal_list->mapped_ranges_array,
	     new_number_of_elements,
	     (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_mapped_range_free,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_RESIZE_FAILED,
		 "%s: unable to resize mapped ranges array.",
		 function );

		goto on_error;
	}
	for( source_element_index = 0;
	     source_element_index < source_number_of_elements;
	     source_element_index++ )
	{
		if( libcdata_array_get_entry_by_index(
		     internal_source_list->elements_array,
		     source_element_index,
		     (intptr_t **) &list_element,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve entry: %d from source list elements array.",
			 function,
			 source_element_index );

			goto on_error;
		}
		if( libcdata_array_set_entry_by_index(
		     internal_list->elements_array,
		     element_index,
		     (intptr_t *) list_element,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set entry: %d in elements array.",
			 function,
			 element_index );

			goto on_error;
		}
		if( libcdata_array_get_entry_by_index(
		     internal_source_list->mapped_ranges_array,
		     source_element_index,
		     (intptr_t **) &mapped_range,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve entry: %d from source list mapped ranges array.",
			 function,
			 source_element_index );

			goto on_error;
		}
		if( libcdata_array_set_entry_by_index(
		     internal_list->mapped_ranges_array,
		     element_index,
		     (intptr_t *) mapped_range,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set entry: %d in mapped ranges array.",
			 function,
			 element_index );

			goto on_error;
		}
		element_index++;
	}
	element_index          = number_of_elements;
	number_of_elements     = new_number_of_elements;
	new_number_of_elements = 0;

	if( libcdata_array_empty(
	     internal_source_list->elements_array,
	     NULL,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_RESIZE_FAILED,
		 "%s: unable to empty source list elements array.",
		 function );

		result = -1;
	}
	if( libcdata_array_empty(
	     internal_source_list->mapped_ranges_array,
	     NULL,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_RESIZE_FAILED,
		 "%s: unable to empty source list mapped ranges array.",
		 function );

		result = -1;
	}
	internal_source_list->size = 0;

	while( element_index < number_of_elements )
	{
		if( libcdata_array_get_entry_by_index(
		     internal_list->elements_array,
		     element_index,
		     (intptr_t **) &list_element,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve entry: %d from elements array.",
			 function,
			 element_index );

			goto on_error;
		}
		( (libfdata_internal_list_element_t *) list_element )->list          = list;
		( (libfdata_internal_list_element_t *) list_element )->element_index = element_index;

		if( libcdata_array_get_entry_by_index(
		     internal_list->mapped_ranges_array,
		     element_index,
		     (intptr_t **) &mapped_range,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve entry: %d from mapped ranges array.",
			 function,
			 element_index );

			goto on_error;
		}
		if( libfdata_mapped_range_get(
		     mapped_range,
		     &mapped_range_offset,
		     &mapped_range_size,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve values from mapped range: %d.",
			 function,
			 element_index );

			goto on_error;
		}
		if( libfdata_mapped_range_set(
		     mapped_range,
		     internal_list->size,
		     mapped_range_size,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set values of mapped range: %d.",
			 function,
			 element_index );

			goto on_error;
		}
		internal_list->size += mapped_range_size;

		element_index++;
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "\n" );
	}
#endif
	return( result );

on_error:
	if( new_number_of_elements != 0 )
	{
		libcdata_array_resize(
		 internal_list->mapped_ranges_array,
		 number_of_elements,
		 NULL,
		 NULL );

		libcdata_array_resize(
		 internal_list->elements_array,
		 number_of_elements,
		 NULL,
		 NULL );
	}
	return( -1 );
}

/* Determines if a specific element is set
 * Returns 1 if element is set, 0 if not or -1 on error
 */
int libfdata_list_is_element_set(
     libfdata_list_t *list,
     int element_index,
     libcerror_error_t **error )
{
	libfdata_internal_list_t *internal_list = NULL;
	libfdata_list_element_t *list_element   = NULL;
	static char *function                   = "libfdata_list_is_element_set";

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
	internal_list = (libfdata_internal_list_t *) list;

	if( libcdata_array_get_entry_by_index(
	     internal_list->elements_array,
	     element_index,
	     (intptr_t **) &list_element,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry: %d from elements array.",
		 function,
		 element_index );

		return( -1 );
	}
	if( list_element == NULL )
	{
		return( 0 );
	}
	internal_list->current_element_index = element_index;

	return( 1 );
}

/* Mapped range functions
 */

/* Retrieves the mapped offset
 * Returns 1 if successful, 0 if not set or -1 on error
 */
int libfdata_list_get_mapped_offset(
     libfdata_list_t *list,
     off64_t *mapped_offset,
     libcerror_error_t **error )
{
	libfdata_internal_list_t *internal_list = NULL;
	static char *function                   = "libfdata_list_get_mapped_offset";

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
	internal_list = (libfdata_internal_list_t *) list;

	if( mapped_offset == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid mapped offset.",
		 function );

		return( -1 );
	}
	if( ( internal_list->flags & LIBFDATA_LIST_FLAG_HAS_MAPPED_OFFSET ) == 0 )
	{
		return( 0 );
	}
	*mapped_offset = internal_list->mapped_offset;

	return( 1 );
}

/* Sets the mapped offset
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_set_mapped_offset(
     libfdata_list_t *list,
     off64_t mapped_offset,
     libcerror_error_t **error )
{
	libfdata_internal_list_t *internal_list = NULL;
	static char *function                   = "libfdata_list_set_mapped_offset";

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
	internal_list = (libfdata_internal_list_t *) list;

	if( mapped_offset < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid offset value out of bounds.",
		 function );

		return( -1 );
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: mapped offset: %" PRIi64 "\n",
		 function,
		 mapped_offset );

		libcnotify_printf(
		 "\n" );
	}
#endif
	internal_list->mapped_offset = mapped_offset;
	internal_list->flags        |= LIBFDATA_LIST_FLAG_HAS_MAPPED_OFFSET | LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES;

	return( 1 );
}

/* Retrieves the mapped size of a specific element
 * Returns 1 if successful, 0 if not set or -1 on error
 */
int libfdata_list_get_mapped_size_by_index(
     libfdata_list_t *list,
     int element_index,
     size64_t *mapped_size,
     libcerror_error_t **error )
{
	libfdata_internal_list_t *internal_list = NULL;
	libfdata_list_element_t *list_element   = NULL;
	static char *function                   = "libfdata_list_get_mapped_size_by_index";
	int result                              = 0;

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
	internal_list = (libfdata_internal_list_t *) list;

	if( libcdata_array_get_entry_by_index(
	     internal_list->elements_array,
	     element_index,
	     (intptr_t **) &list_element,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry: %d from elements array.",
		 function,
		 element_index );

		return( -1 );
	}
	result = libfdata_list_element_get_mapped_size(
	          list_element,
	          mapped_size,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve mapped size of element: %d.",
		 function,
		 element_index );

		return( -1 );
	}
	internal_list->current_element_index = element_index;

	return( result );
}

/* Sets the mapped size of a specific element
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_set_mapped_size_by_index(
     libfdata_list_t *list,
     int element_index,
     size64_t mapped_size,
     libcerror_error_t **error )
{
	libfdata_internal_list_t *internal_list = NULL;
	libfdata_list_element_t *list_element   = NULL;
	static char *function                   = "libfdata_list_set_mapped_size_by_index";

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
	internal_list = (libfdata_internal_list_t *) list;

	if( libcdata_array_get_entry_by_index(
	     internal_list->elements_array,
	     element_index,
	     (intptr_t **) &list_element,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry: %d from elements array.",
		 function,
		 element_index );

		return( -1 );
	}
	if( libfdata_list_element_set_mapped_size(
	     list_element,
	     mapped_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set mapped size of element: %d.",
		 function,
		 element_index );

		return( -1 );
	}
	internal_list->current_element_index = element_index;

	return( 1 );
}

/* Retrieves the data range with its mapped size of a specific element
 * Returns 1 if successful, 0 if the element has no mapped size or -1 on error
 */
int libfdata_list_get_element_by_index_with_mapped_size(
     libfdata_list_t *list,
     int element_index,
     int *element_file_index,
     off64_t *element_offset,
     size64_t *element_size,
     uint32_t *element_flags,
     size64_t *mapped_size,
     libcerror_error_t **error )
{
	libfdata_internal_list_t *internal_list = NULL;
	libfdata_list_element_t *list_element   = NULL;
	static char *function                   = "libfdata_list_get_element_by_index_with_mapped_size";
	int result                              = 0;

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
	internal_list = (libfdata_internal_list_t *) list;

	if( libcdata_array_get_entry_by_index(
	     internal_list->elements_array,
	     element_index,
	     (intptr_t **) &list_element,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry: %d from elements array.",
		 function,
		 element_index );

		return( -1 );
	}
	if( libfdata_list_element_get_data_range(
	     list_element,
	     element_file_index,
	     element_offset,
	     element_size,
	     element_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve data range from list element: %d.",
		 function,
		 element_index );

		return( -1 );
	}
	result = libfdata_list_element_get_mapped_size(
	          list_element,
	          mapped_size,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve mapped size of list element: %d.",
		 function,
		 element_index );

		return( -1 );
	}
	internal_list->current_element_index = element_index;

	return( result );
}

/* Sets the data range of a specific element with its mapped size
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_set_element_by_index_with_mapped_size(
     libfdata_list_t *list,
     int element_index,
     int element_file_index,
     off64_t element_offset,
     size64_t element_size,
     uint32_t element_flags,
     size64_t mapped_size,
     libcerror_error_t **error )
{
	libfdata_internal_list_t *internal_list = NULL;
	libfdata_list_element_t *list_element   = NULL;
	libfdata_mapped_range_t *mapped_range   = NULL;
	static char *function                   = "libfdata_list_set_element_by_index_with_mapped_size";
	off64_t previous_element_offset         = 0;
	size64_t previous_element_size          = 0;
	size64_t previous_mapped_size           = 0;
	uint32_t previous_element_flags         = 0;
	int previous_element_file_index         = 0;
	int result                              = 0;

#if defined( HAVE_DEBUG_OUTPUT )
	off64_t mapped_range_offset             = 0;
	size64_t mapped_range_size              = 0;
#endif

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
	internal_list = (libfdata_internal_list_t *) list;

	if( libcdata_array_get_entry_by_index(
	     internal_list->elements_array,
	     element_index,
	     (intptr_t **) &list_element,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry: %d from elements array.",
		 function,
		 element_index );

		return( -1 );
	}
	if( list_element == NULL )
	{
		if( libfdata_list_element_initialize(
		     &list_element,
		     list,
		     element_index,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create list element.",
			 function );

			return( -1 );
		}
		if( libcdata_array_set_entry_by_index(
		     internal_list->elements_array,
		     element_index,
		     (intptr_t *) list_element,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set entry: %d in elements array.",
			 function,
			 element_index );

			libfdata_list_element_free(
			 &list_element,
			 NULL );

			return( -1 );
		}
	}
	else
	{
		result = libfdata_list_element_get_mapped_size(
		          list_element,
		          &previous_mapped_size,
		          error );

		if( result == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve mapped size of list element: %d.",
			 function,
			 element_index );

			return( -1 );
		}
		else if( result == 0 )
		{
			if( libfdata_list_element_get_data_range(
			     list_element,
			     &previous_element_file_index,
			     &previous_element_offset,
			     &previous_element_size,
			     &previous_element_flags,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve data range of list element: %d.",
				 function,
				 element_index );

				return( -1 );
			}
		}
	}
	if( libfdata_list_element_set_data_range(
	     list_element,
	     element_file_index,
	     element_offset,
	     element_size,
	     element_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set data range of list element: %d.",
		 function,
		 element_index );

		return( -1 );
	}
	if( libfdata_list_element_set_mapped_size(
	     list_element,
	     mapped_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set mapped size of list element.",
		 function );

		return( -1 );
	}
	/* Make sure the list has a mapped range entry for every element
	 */
	if( libcdata_array_get_entry_by_index(
	     internal_list->mapped_ranges_array,
	     element_index,
	     (intptr_t **) &mapped_range,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry: %d from mapped ranges array.",
		 function,
		 element_index );

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
		     internal_list->mapped_ranges_array,
		     element_index,
		     (intptr_t *) mapped_range,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set entry: %d in mapped ranges array.",
			 function,
			 element_index );

			libfdata_mapped_range_free(
			 &mapped_range,
			 NULL );

			return( -1 );
		}
		internal_list->size  += mapped_size;
		internal_list->flags |= LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES;
	}
        else if( previous_mapped_size != mapped_size )
	{
		if( previous_mapped_size != 0 )
		{
			internal_list->size -= previous_mapped_size;
		}
		else
		{
			internal_list->size -= previous_element_size;
		}
		if( mapped_size != 0 )
		{
			internal_list->size += mapped_size;
		}
		else
		{
			internal_list->size += element_size;
		}
		internal_list->flags |= LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES;
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: element: %03d\tfile index: %03d offset: 0x%08" PRIx64 " - 0x%08" PRIx64 " (size: %" PRIu64 ")\n",
		 function,
		 element_index,
		 element_file_index,
		 element_offset,
		 element_offset + element_size,
		 element_size );

		if( ( internal_list->flags & LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES ) == 0 )
		{
			if( libfdata_mapped_range_get(
			     mapped_range,
			     &mapped_range_offset,
			     &mapped_range_size,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve values from mapped range: %d.",
				 function,
				 element_index );

				return( -1 );
			}
			libcnotify_printf(
			 "%s: element: %03d\tmapped range: 0x%08" PRIx64 " - 0x%08" PRIx64 " (size: %" PRIu64 ")\n",
			 function,
			 element_index,
			 mapped_range_offset,
			 mapped_range_offset + mapped_range_size,
			 mapped_range_size );
		}
		libcnotify_printf(
		 "\n" );
	}
#endif
	internal_list->current_element_index = element_index;

	return( 1 );
}

/* Appends an element data range with its mapped size
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_append_element_with_mapped_size(
     libfdata_list_t *list,
     int *element_index,
     int element_file_index,
     off64_t element_offset,
     size64_t element_size,
     uint32_t element_flags,
     size64_t mapped_size,
     libcerror_error_t **error )
{
	libfdata_internal_list_t *internal_list = NULL;
	libfdata_list_element_t *list_element   = NULL;
	libfdata_mapped_range_t *mapped_range   = NULL;
	static char *function                   = "libfdata_list_append_element_with_mapped_size";
	off64_t mapped_offset                   = 0;
	int mapped_range_index                  = -1;
	uint8_t list_flags                      = 0;

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
	internal_list = (libfdata_internal_list_t *) list;

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
	mapped_offset = internal_list->mapped_offset + (off64_t) internal_list->size;

	if( libfdata_mapped_range_set(
	     mapped_range,
	     mapped_offset,
	     mapped_size,
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
	     internal_list->mapped_ranges_array,
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
	if( libfdata_list_element_initialize(
	     &list_element,
	     list,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create list element.",
		 function );

		goto on_error;
	}
	if( libfdata_list_element_set_data_range(
	     list_element,
	     element_file_index,
	     element_offset,
	     element_size,
	     element_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set data range of list element.",
		 function );

		goto on_error;
	}
	list_flags = internal_list->flags;

	if( libfdata_list_element_set_mapped_size(
	     list_element,
	     mapped_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set mapped size of list element.",
		 function );

		goto on_error;
	}
	/* Setting the mapped size in the list element will set the calculate mapped ranges flag in the list
	 * Reset the flag if it was not set before setting the mapped size in the list element
	 */
	if( ( list_flags & LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES ) == 0 )
	{
		internal_list->flags &= ~( LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES );
	}
	if( libcdata_array_append_entry(
	     internal_list->elements_array,
	     element_index,
	     (intptr_t *) list_element,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
		 "%s: unable to append list element to elements array.",
		 function );

		goto on_error;
	}
	mapped_range_index = -1;
	mapped_range       = NULL;

	if( libfdata_list_element_set_element_index(
	     list_element,
	     *element_index,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set list element index.",
		 function );

		list_element = NULL;

		goto on_error;
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: element: %03d\tfile index: %03d offset: 0x%08" PRIx64 " - 0x%08" PRIx64 " (size: %" PRIu64 ")\n",
		 function,
		 *element_index,
		 element_file_index,
		 element_offset,
		 element_offset + element_size,
		 element_size );

		libcnotify_printf(
		 "%s: element: %03d\tmapped range: 0x%08" PRIx64 " - 0x%08" PRIx64 " (size: %" PRIu64 ")\n",
		 function,
		 *element_index,
		 mapped_offset,
		 mapped_offset + mapped_size,
		 mapped_size );

		libcnotify_printf(
		 "\n" );
	}
#endif
	internal_list->current_element_index = *element_index;
	internal_list->size                 += mapped_size;

	return( 1 );

on_error:
	if( list_element != NULL )
	{
		libfdata_list_element_free(
		 &list_element,
		 NULL );
	}
	if( mapped_range_index != -1 )
	{
		libcdata_array_set_entry_by_index(
		 internal_list->mapped_ranges_array,
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

/* Calculates the mapped ranges
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_calculate_mapped_ranges(
     libfdata_internal_list_t *internal_list,
     libcerror_error_t **error )
{
	libfdata_list_element_t *list_element = NULL;
	libfdata_mapped_range_t *mapped_range = NULL;
	static char *function                 = "libfdata_list_calculate_mapped_ranges";
	off64_t mapped_offset                 = 0;
	off64_t element_offset                = 0;
	size64_t element_size                 = 0;
	size64_t mapped_size                  = 0;
	uint32_t element_flags                = 0;
	int element_file_index                = -1;
	int element_index                     = -1;
	int number_of_elements                = 0;
	int result                            = 0;

	if( internal_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid list.",
		 function );

		return( -1 );
	}
	if( libcdata_array_get_number_of_entries(
	     internal_list->elements_array,
	     &number_of_elements,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of elements from elements array.",
		 function );

		return( -1 );
	}
	mapped_offset = internal_list->mapped_offset;

	for( element_index = 0;
	     element_index < number_of_elements;
	     element_index++ )
	{
		if( libcdata_array_get_entry_by_index(
		     internal_list->elements_array,
		     element_index,
		     (intptr_t **) &list_element,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve entry: %d from elements array.",
			 function,
			 element_index );

			return( -1 );
		}
		if( libcdata_array_get_entry_by_index(
		     internal_list->mapped_ranges_array,
		     element_index,
		     (intptr_t **) &mapped_range,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve entry: %d from mapped ranges array.",
			 function,
			 element_index );

			return( -1 );
		}
		if( libfdata_list_element_get_data_range(
		     list_element,
		     &element_file_index,
		     &element_offset,
		     &element_size,
		     &element_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve data range of list element: %d.",
			 function,
			 element_index );

			return( -1 );
		}
		result = libfdata_list_element_get_mapped_size(
		          list_element,
		          &mapped_size,
		          error );

		if( result == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve mapped size of list element: %d.",
			 function,
			 element_index );

			return( -1 );
		}
		else if( result == 0 )
		{
			mapped_size = element_size;
		}
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: element: %03d\tfile index: %03d offset: 0x%08" PRIx64 " - 0x%08" PRIx64 " (size: %" PRIu64 ")\n",
			 function,
			 element_index,
			 element_file_index,
			 element_offset,
			 element_offset + element_size,
			 element_size );

			libcnotify_printf(
			 "%s: element: %03d\tmapped range: 0x%08" PRIx64 " - 0x%08" PRIx64 " (size: %" PRIu64 ")\n",
			 function,
			 element_index,
			 mapped_offset,
			 mapped_offset + mapped_size,
			 mapped_size );
		}
#endif
		if( libfdata_mapped_range_set(
		     mapped_range,
		     mapped_offset,
		     mapped_size,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set mapped range: %d values.",
			 function,
			 element_index );

			return( -1 );
		}
		mapped_offset += (off64_t) mapped_size;
	}
	internal_list->size   = (size64_t) mapped_offset - internal_list->mapped_offset;
	internal_list->flags &= ~( LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES );

#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "\n" );
	}
#endif
	return( 1 );
}

/* Retrieves the element index for a specific offset
 * The element_data_offset value is set to the offset relative to the start of the element
 * Returns 1 if successful, 0 if not or -1 on error
 */
int libfdata_list_get_element_index_at_offset(
     libfdata_list_t *list,
     off64_t offset,
     int *element_index,
     off64_t *element_data_offset,
     libcerror_error_t **error )
{
	libfdata_internal_list_t *internal_list = NULL;
	libfdata_mapped_range_t *mapped_range   = NULL;
	static char *function                   = "libfdata_list_get_element_index_at_offset";
	off64_t list_offset                     = 0;
	off64_t mapped_range_end_offset         = 0;
	off64_t mapped_range_start_offset       = 0;
	size64_t mapped_range_size              = 0;
	int initial_element_index               = 0;
	int number_of_elements                  = 0;
	int result                              = 0;
	int search_element_index                = 0;

#if defined( HAVE_DEBUG_OUTPUT )
	libfdata_list_element_t *element        = NULL;
	off64_t element_offset                  = 0;
	size64_t element_size                   = 0;
	uint32_t element_flags                  = 0;
	int element_file_index                  = -1;
#endif

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
	internal_list = (libfdata_internal_list_t *) list;

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
	if( element_data_offset == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid element data offset.",
		 function );

		return( -1 );
	}
	if( ( internal_list->flags & LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES ) != 0 )
	{
		if( libfdata_list_calculate_mapped_ranges(
		     internal_list,
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
	if( internal_list->size == 0 )
	{
		return( 0 );
	}
	if( offset < internal_list->mapped_offset )
	{
		return( 0 );
	}
	list_offset = offset - internal_list->mapped_offset;

	if( (size64_t) list_offset >= internal_list->size )
	{
		return( 0 );
	}
	if( libcdata_array_get_number_of_entries(
	     internal_list->mapped_ranges_array,
	     &number_of_elements,
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
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: number of elements: %d\n",
		 function,
		 number_of_elements );
	}
#endif
	/* This assumes a fairly even distribution of the sizes of the elements
	 */
	initial_element_index = (int) ( ( number_of_elements * list_offset ) / internal_list->size );

	/* Look for the corresponding element upwards in the array
	 */
	for( search_element_index = initial_element_index;
	     search_element_index < number_of_elements;
	     search_element_index++ )
	{
		if( libcdata_array_get_entry_by_index(
		     internal_list->mapped_ranges_array,
		     search_element_index,
		     (intptr_t **) &mapped_range,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve entry: %d from mapped ranges array.",
			 function,
			 search_element_index );

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
			 search_element_index );

			return( -1 );
		}
		mapped_range_end_offset = mapped_range_start_offset + (off64_t) mapped_range_size;

		if( mapped_range_end_offset < mapped_range_start_offset )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: invalid element: %d - mapped range value out of bounds.",
			 function,
			 search_element_index );

			return( -1 );
		}
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: element: %03d\tmapped range: 0x%08" PRIx64 " - 0x%08" PRIx64 " (size: %" PRIu64 ")\n",
			 function,
			 search_element_index,
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
			search_element_index = number_of_elements;

			break;
		}
	}
	if( search_element_index >= number_of_elements )
	{
		/* Look for the corresponding element downwards in the array
		 */
		for( search_element_index = initial_element_index;
		     search_element_index >= 0;
		     search_element_index-- )
		{
			if( libcdata_array_get_entry_by_index(
			     internal_list->mapped_ranges_array,
			     search_element_index,
			     (intptr_t **) &mapped_range,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve entry: %d from mapped ranges array.",
				 function,
				 search_element_index );

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
				 search_element_index );

				return( -1 );
			}
			mapped_range_end_offset = mapped_range_start_offset + (off64_t) mapped_range_size;

			if( mapped_range_end_offset < mapped_range_start_offset )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: invalid element: %d - mapped range value out of bounds.",
				 function,
				 search_element_index );

				return( -1 );
			}
#if defined( HAVE_DEBUG_OUTPUT )
			if( libcnotify_verbose != 0 )
			{
				libcnotify_printf(
				 "%s: element: %03d\tmapped range: 0x%08" PRIx64 " - 0x%08" PRIx64 " (size: %" PRIu64 ")\n",
				 function,
				 search_element_index,
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
				search_element_index--;

				break;
			}
		}
	}
	if( ( search_element_index >= 0 )
	 && ( search_element_index < number_of_elements ) )
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
		*element_data_offset = offset;

#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			if( libcdata_array_get_entry_by_index(
			     internal_list->elements_array,
			     search_element_index,
			     (intptr_t **) &element,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve entry: %d from elements array.",
				 function,
				 search_element_index );

				return( -1 );
			}
			if( libfdata_list_element_get_data_range(
			     element,
			     &element_file_index,
			     &element_offset,
			     &element_size,
			     &element_flags,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve data range of list element: %d.",
				 function,
				 search_element_index );

				return( -1 );
			}
			libcnotify_printf(
			 "%s: element: %03d\tfile index: %03d offset: 0x%08" PRIx64 " - 0x%08" PRIx64 " (size: %" PRIu64 ")\n",
			 function,
			 search_element_index,
			 element_file_index,
			 element_offset,
			 element_offset + element_size,
			 element_size );
		}
#endif
		internal_list->current_element_index = search_element_index;

		result = 1;
	}
	if( result == 1 )
	{
		*element_index = search_element_index;
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

/* Retrieves the list element for a specific offset
 * The element_data_offset value is set to the offset relative to the start of the element
 * Returns 1 if successful, 0 if not or -1 on error
 */
int libfdata_list_get_list_element_at_offset(
     libfdata_list_t *list,
     off64_t offset,
     int *element_index,
     off64_t *element_data_offset,
     libfdata_list_element_t **element,
     libcerror_error_t **error )
{
	libfdata_internal_list_t *internal_list = NULL;
	static char *function                   = "libfdata_list_get_list_element_at_offset";
	int result                              = 0;

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
	internal_list = (libfdata_internal_list_t *) list;

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
	result = libfdata_list_get_element_index_at_offset(
	          list,
	          offset,
	          element_index,
	          element_data_offset,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve element index at offset: 0x%08" PRIx64 ".",
		 function,
		 offset );

		return( -1 );
	}
	else if( result != 0 )
	{
		if( libcdata_array_get_entry_by_index(
		     internal_list->elements_array,
		     *element_index,
		     (intptr_t **) element,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve entry: %d from elements array.",
			 function,
			 *element_index );

			return( -1 );
		}
	}
	return( result );
}

/* Retrieves the data range of an element at a specific offset
 * Returns 1 if successful, 0 if not or -1 on error
 */
int libfdata_list_get_element_at_offset(
     libfdata_list_t *list,
     off64_t offset,
     int *element_index,
     off64_t *element_data_offset,
     int *element_file_index,
     off64_t *element_offset,
     size64_t *element_size,
     uint32_t *element_flags,
     libcerror_error_t **error )
{
	libfdata_internal_list_t *internal_list = NULL;
	libfdata_list_element_t *list_element   = NULL;
	static char *function                   = "libfdata_list_get_element_at_offset";
	int result                              = 0;

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
	internal_list = (libfdata_internal_list_t *) list;

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
	result = libfdata_list_get_element_index_at_offset(
	          list,
	          offset,
	          element_index,
	          element_data_offset,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve element index at offset: 0x%08" PRIx64 ".",
		 function,
		 offset );

		return( -1 );
	}
	else if( result != 0 )
	{
		if( libcdata_array_get_entry_by_index(
		     internal_list->elements_array,
		     *element_index,
		     (intptr_t **) &list_element,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve entry: %d from elements array.",
			 function,
			 *element_index );

			return( -1 );
		}
		if( libfdata_list_element_get_data_range(
		     list_element,
		     element_file_index,
		     element_offset,
		     element_size,
		     element_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve data range of list element: %d.",
			 function,
			 *element_index );

			return( -1 );
		}
	}
	return( result );
}

/* List element value functions
 */

/* Caches the element value
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_cache_element_value(
     libfdata_list_t *list,
     libfcache_cache_t *cache,
     int element_index,
     int element_file_index,
     off64_t element_offset,
     size64_t element_size,
     uint32_t element_flags,
     time_t element_timestamp,
     intptr_t *element_value,
     int (*free_element_value)(
            intptr_t **element_value,
            libcerror_error_t **error ),
     uint8_t write_flags,
     libcerror_error_t **error )
{
	libfdata_internal_list_t *internal_list = NULL;
	static char *function                   = "libfdata_list_cache_element_value";
	int cache_entry_index                   = -1;
	int number_of_cache_entries             = 0;

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
	internal_list = (libfdata_internal_list_t *) list;

	if( element_index < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid element index value out of bounds.",
		 function );

		return( -1 );
	}
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
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid number of cache entries value out of bounds.",
		 function );

		return( -1 );
	}
	if( internal_list->calculate_cache_entry_index == NULL )
	{
		cache_entry_index = element_index % number_of_cache_entries;
	}
	else
	{
		cache_entry_index = internal_list->calculate_cache_entry_index(
		                     element_index,
		                     element_file_index,
		                     element_offset,
		                     element_size,
		                     element_flags,
		                     number_of_cache_entries );
	}
	if( libfcache_cache_set_value_by_index(
	     cache,
	     cache_entry_index,
	     element_file_index,
	     element_offset,
	     element_timestamp,
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

/* Retrieves the element value
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_get_element_value(
     libfdata_list_t *list,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     libfdata_list_element_t *element,
     intptr_t **element_value,
     uint8_t read_flags,
     libcerror_error_t **error )
{
	libfcache_cache_value_t *cache_value    = NULL;
	libfdata_internal_list_t *internal_list = NULL;
	static char *function                   = "libfdata_list_get_element_value";
        off64_t cache_value_offset              = (off64_t) -1;
	off64_t element_offset                  = 0;
	size64_t element_size                   = 0;
	time_t cache_value_timestamp            = 0;
	time_t element_timestamp                = 0;
	uint32_t element_flags                  = 0;
	int cache_entry_index                   = -1;
	int cache_value_file_index              = -1;
	int element_file_index                  = -1;
	int element_index                       = -1;
	int number_of_cache_entries             = 0;
	int result                              = 0;

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
	internal_list = (libfdata_internal_list_t *) list;

	if( internal_list->read_element_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid list - missing read element data function.",
		 function );

		return( -1 );
	}
	if( libfdata_list_element_get_data_range(
	     element,
	     &element_file_index,
	     &element_offset,
	     &element_size,
	     &element_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve data range from list element.",
		 function );

		return( -1 );
	}
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
	if( libfdata_list_element_get_element_index(
	     element,
	     &element_index,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve element index from list element.",
		 function );

		return( -1 );
	}
	if( number_of_cache_entries <= 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid number of cache entries value out of bounds.",
		 function );

		return( -1 );
	}
	if( ( read_flags & LIBFDATA_READ_FLAG_IGNORE_CACHE ) == 0 )
	{
		if( internal_list->calculate_cache_entry_index == NULL )
		{
			cache_entry_index = element_index % number_of_cache_entries;
		}
		else
		{
			cache_entry_index = internal_list->calculate_cache_entry_index(
			                     element_index,
			                     element_file_index,
			                     element_offset,
			                     element_size,
			                     element_flags,
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
			if( libfdata_list_element_get_timestamp(
			     element,
			     &element_timestamp,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve time stamp from list element.",
				 function );

				return( -1 );
			}
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
		if( ( element_file_index == cache_value_file_index )
                 && ( element_offset == cache_value_offset )
		 && ( element_timestamp == cache_value_timestamp ) )
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
			 element_offset,
			 element_offset,
			 element_size );
		}
#endif
		if( internal_list->read_element_data(
		     internal_list->data_handle,
		     file_io_handle,
		     element,
		     cache,
		     element_file_index,
		     element_offset,
		     element_size,
		     element_flags,
		     read_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_READ_FAILED,
			 "%s: unable to read element data at offset: 0x%08" PRIx64 ".",
			 function,
			 element_offset );

			return( -1 );
		}
		if( internal_list->calculate_cache_entry_index == NULL )
		{
			cache_entry_index = element_index % number_of_cache_entries;
		}
		else
		{
			cache_entry_index = internal_list->calculate_cache_entry_index(
			                     element_index,
			                     element_file_index,
			                     element_offset,
			                     element_size,
			                     element_flags,
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
			if( libfdata_list_element_get_timestamp(
			     element,
			     &element_timestamp,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve time stamp from list element.",
				 function );

				return( -1 );
			}
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
		if( ( element_file_index != cache_value_file_index )
		 || ( element_offset != cache_value_offset )
		 || ( element_timestamp != cache_value_timestamp ) )
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

/* Retrieves the value of a specific element
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_get_element_value_by_index(
     libfdata_list_t *list,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     int element_index,
     intptr_t **element_value,
     uint8_t read_flags,
     libcerror_error_t **error )
{
	libfdata_internal_list_t *internal_list = NULL;
	libfdata_list_element_t *list_element   = NULL;
	static char *function                   = "libfdata_list_get_element_value_by_index";

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
	internal_list = (libfdata_internal_list_t *) list;

	if( libcdata_array_get_entry_by_index(
	     internal_list->elements_array,
	     element_index,
	     (intptr_t **) &list_element,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry: %d from elements array.",
		 function,
		 element_index );

		return( -1 );
	}
	if( libfdata_list_get_element_value(
	     list,
	     file_io_handle,
	     cache,
	     list_element,
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
	internal_list->current_element_index = element_index;

	return( 1 );
}

/* Retrieves the value an element at a specific offset
 * Returns 1 if successful, 0 if not or -1 on error
 */
int libfdata_list_get_element_value_at_offset(
     libfdata_list_t *list,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     off64_t offset,
     int *element_index,
     off64_t *element_data_offset,
     intptr_t **element_value,
     uint8_t read_flags,
     libcerror_error_t **error )
{
	libfdata_list_element_t *list_element = NULL;
	static char *function                 = "libfdata_list_get_element_value_at_offset";
	int result                            = 0;

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
	result = libfdata_list_get_list_element_at_offset(
	          list,
	          offset,
	          element_index,
	          element_data_offset,
	          &list_element,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve element at offset: 0x%08" PRIx64 ".",
		 function,
		 offset );

		return( -1 );
	}
	else if( result != 0 )
	{
		if( libfdata_list_get_element_value(
		     list,
		     file_io_handle,
		     cache,
		     list_element,
		     element_value,
		     read_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve element: %d value.",
			 function,
			 *element_index );

			return( -1 );
		}
	}
	return( result );
}

/* Sets the value of a specific element
 *
 * If the flag LIBFDATA_LIST_ELEMENT_VALUE_FLAG_MANAGED is set the list
 * takes over management of the value and the value is freed when
 * no longer needed.
 *
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_set_element_value(
     libfdata_list_t *list,
     intptr_t *file_io_handle LIBFDATA_ATTRIBUTE_UNUSED,
     libfcache_cache_t *cache,
     libfdata_list_element_t *element,
     intptr_t *element_value,
     int (*free_element_value)(
            intptr_t **element_value,
            libcerror_error_t **error ),
     uint8_t write_flags,
     libcerror_error_t **error )
{
	static char *function    = "libfdata_list_set_element_value";
	off64_t element_offset   = 0;
	size64_t element_size    = 0;
	time_t element_timestamp = 0;
	uint32_t element_flags   = 0;
	int element_file_index   = -1;
	int element_index        = -1;

	LIBFDATA_UNREFERENCED_PARAMETER( file_io_handle )

	if( libfdata_list_element_get_data_range(
	     element,
	     &element_file_index,
	     &element_offset,
	     &element_size,
	     &element_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve data range from list element.",
		 function );

		return( -1 );
	}
	if( libfdata_list_element_get_element_index(
	     element,
	     &element_index,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve element index from list element.",
		 function );

		return( -1 );
	}
	if( libfdata_list_element_get_timestamp(
	     element,
	     &element_timestamp,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve time stamp from list element.",
		 function );

		return( -1 );
	}
	if( libfdata_list_cache_element_value(
	     list,
	     cache,
	     element_index,
	     element_file_index,
	     element_offset,
	     element_size,
	     element_flags,
	     element_timestamp,
	     element_value,
	     free_element_value,
	     write_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to cache element value.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Sets the value of a specific element
 *
 * If the flag LIBFDATA_LIST_ELEMENT_VALUE_FLAG_MANAGED is set the list
 * takes over management of the value and the value is freed when
 * no longer needed.
 *
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_set_element_value_by_index(
     libfdata_list_t *list,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     int element_index,
     intptr_t *element_value,
     int (*free_element_value)(
            intptr_t **element_value,
            libcerror_error_t **error ),
     uint8_t write_flags,
     libcerror_error_t **error )
{
	libfdata_internal_list_t *internal_list = NULL;
	libfdata_list_element_t *list_element   = NULL;
	static char *function                   = "libfdata_list_set_element_value_by_index";

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
	internal_list = (libfdata_internal_list_t *) list;

	if( libcdata_array_get_entry_by_index(
	     internal_list->elements_array,
	     element_index,
	     (intptr_t **) &list_element,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve entry: %d from elements array.",
		 function,
		 element_index );

		return( -1 );
	}
	if( libfdata_list_set_element_value(
	     list,
	     file_io_handle,
	     cache,
	     list_element,
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
	internal_list->current_element_index = element_index;

	return( 1 );
}

/* Sets the value of an element at a specific offset
 *
 * If the flag LIBFDATA_LIST_ELEMENT_VALUE_FLAG_MANAGED is set the list
 * takes over management of the value and the value is freed when
 * no longer needed.
 *
 * Returns 1 if successful, 0 if not or -1 on error
 */
int libfdata_list_set_element_value_at_offset(
     libfdata_list_t *list,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     off64_t offset,
     intptr_t *element_value,
     int (*free_element_value)(
            intptr_t **element_value,
            libcerror_error_t **error ),
     uint8_t write_flags,
     libcerror_error_t **error )
{
	libfdata_list_element_t *list_element = NULL;
	static char *function                 = "libfdata_list_set_element_value_at_offset";
	off64_t element_data_offset           = 0;
	int element_index                     = 0;
	int result                            = 0;

	result = libfdata_list_get_list_element_at_offset(
	          list,
	          offset,
	          &element_index,
	          &element_data_offset,
	          &list_element,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve element at offset: 0x%08" PRIx64 ".",
		 function,
		 offset );

		return( -1 );
	}
	else if( result != 0 )
	{
		if( libfdata_list_set_element_value(
		     list,
		     file_io_handle,
		     cache,
		     list_element,
		     element_value,
		     free_element_value,
		     write_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set value of element: %d.",
			 function,
			 element_index );

			return( -1 );
		}
	}
	return( result );
}

/* Retrieves the size of the list
 * Returns 1 if successful or -1 on error
 */
int libfdata_list_get_size(
     libfdata_list_t *list,
     size64_t *size,
     libcerror_error_t **error )
{
	libfdata_internal_list_t *internal_list = NULL;
	static char *function                   = "libfdata_list_get_size";

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
	internal_list = (libfdata_internal_list_t *) list;

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
	if( ( internal_list->flags & LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES ) != 0 )
	{
		if( libfdata_list_calculate_mapped_ranges(
		     internal_list,
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
	}
	*size = internal_list->size;

	return( 1 );
}

