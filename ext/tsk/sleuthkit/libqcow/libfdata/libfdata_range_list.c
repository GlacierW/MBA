/*
 * The range list functions
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
#include "libfdata_range_list.h"
#include "libfdata_types.h"

/* Creates a range list
 * Make sure the value range_list is referencing, is set to NULL
 *
 * If the flag LIBFDATA_DATA_HANDLE_FLAG_MANAGED is set the range list
 * takes over management of the data handle and the data handle is freed when
 * no longer needed
 *
 * Returns 1 if successful or -1 on error
 */
int libfdata_range_list_initialize(
     libfdata_range_list_t **range_list,
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
            int element_file_index,
            off64_t element_offset,
            size64_t element_size,
            uint32_t element_flags,
            uint8_t read_flags,
            libcerror_error_t **error ),
     int (*write_element_data)(
            intptr_t *data_handle,
            intptr_t *file_io_handle,
            libfdata_list_element_t *list_element,
            libfcache_cache_t *cache,
            int element_file_index,
            off64_t element_offset,
            size64_t element_size,
            uint32_t element_flags,
            uint8_t write_flags,
            libcerror_error_t **error ),
     uint8_t flags,
     libcerror_error_t **error )
{
	libfdata_internal_range_list_t *internal_range_list = NULL;
	static char *function                               = "libfdata_range_list_initialize";

	if( range_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid range list.",
		 function );

		return( -1 );
	}
	if( *range_list != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid range list value already set.",
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
	internal_range_list = memory_allocate_structure(
	                       libfdata_internal_range_list_t );

	if( internal_range_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create range list.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_range_list,
	     0,
	     sizeof( libfdata_internal_range_list_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear range list.",
		 function );

		memory_free(
		 internal_range_list );

		return( -1 );
	}
	if( libcdata_range_list_initialize(
	     &( internal_range_list->elements_range_list ),
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create elements range list.",
		 function );

		goto on_error;
	}
	internal_range_list->flags             |= flags;
	internal_range_list->data_handle        = data_handle;
	internal_range_list->free_data_handle   = free_data_handle;
	internal_range_list->clone_data_handle  = clone_data_handle;
	internal_range_list->read_element_data  = read_element_data;
	internal_range_list->write_element_data = write_element_data;

	*range_list = (libfdata_range_list_t *) internal_range_list;

	return( 1 );

on_error:
	if( internal_range_list != NULL )
	{
		if( internal_range_list->elements_range_list != NULL )
		{
			libcdata_range_list_free(
			 &( internal_range_list->elements_range_list ),
			 NULL,
			 NULL );
		}
		memory_free(
		 internal_range_list );
	}
	return( -1 );
}

/* Frees a range list
 * Returns 1 if successful or -1 on error
 */
int libfdata_range_list_free(
     libfdata_range_list_t **range_list,
     libcerror_error_t **error )
{
	libfdata_internal_range_list_t *internal_range_list = NULL;
	static char *function                               = "libfdata_range_list_free";
	int result                                          = 1;

	if( range_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid list.",
		 function );

		return( -1 );
	}
	if( *range_list != NULL )
	{
		internal_range_list = (libfdata_internal_range_list_t *) *range_list;
		*range_list         = NULL;

		if( libcdata_range_list_free(
		     &( internal_range_list->elements_range_list ),
		     (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_list_free,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free the elements range list.",
			 function );

			result = -1;
		}
		if( ( internal_range_list->flags & LIBFDATA_DATA_HANDLE_FLAG_MANAGED ) != 0 )
		{
			if( internal_range_list->data_handle != NULL )
			{
				if( internal_range_list->free_data_handle == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: invalid range list - missing free data handle function.",
					 function );

					result = -1;
				}
				else if( internal_range_list->free_data_handle(
				          &( internal_range_list->data_handle ),
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
		 internal_range_list );
	}
	return( result );
}

/* Clones (duplicates) the range list
 * Returns 1 if successful or -1 on error
 */
int libfdata_range_list_clone(
     libfdata_range_list_t **destination_range_list,
     libfdata_range_list_t *source_range_list,
     libcerror_error_t **error )
{
	libfdata_internal_range_list_t *internal_destination_range_list = NULL;
	libfdata_internal_range_list_t *internal_source_range_list      = NULL;
	static char *function                                           = "libfdata_range_list_clone";

	if( destination_range_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid destination range list.",
		 function );

		return( -1 );
	}
	if( *destination_range_list != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid destination range list value already set.",
		 function );

		return( -1 );
	}
	if( source_range_list == NULL )
	{
		*destination_range_list = NULL;

		return( 1 );
	}
	internal_source_range_list = (libfdata_internal_range_list_t *) source_range_list;

	internal_destination_range_list = memory_allocate_structure(
	                                   libfdata_internal_range_list_t );

	if( internal_destination_range_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create destination range list.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_destination_range_list,
	     0,
	     sizeof( libfdata_internal_range_list_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear destination range list.",
		 function );

		memory_free(
		 internal_destination_range_list );

		return( -1 );
	}
	if( internal_source_range_list->data_handle != NULL )
	{
		if( internal_source_range_list->free_data_handle == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: invalid source range list - missing free data handle function.",
			 function );

			goto on_error;
		}
		if( internal_source_range_list->clone_data_handle == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: invalid source range list - missing clone data handle function.",
			 function );

			goto on_error;
		}
		if( internal_source_range_list->clone_data_handle(
		     &( internal_destination_range_list->data_handle ),
		     internal_source_range_list->data_handle,
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
	if( libcdata_range_list_clone(
	     &( internal_destination_range_list->elements_range_list ),
	     internal_source_range_list->elements_range_list,
	     (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_list_free,
	     (int (*)(intptr_t **, intptr_t *, libcerror_error_t **)) &libfdata_list_clone,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create destination elements range list.",
		 function );

		goto on_error;
	}
	internal_destination_range_list->flags              = internal_source_range_list->flags | LIBFDATA_DATA_HANDLE_FLAG_MANAGED;
	internal_destination_range_list->free_data_handle   = internal_source_range_list->free_data_handle;
	internal_destination_range_list->clone_data_handle  = internal_source_range_list->clone_data_handle;
	internal_destination_range_list->read_element_data  = internal_source_range_list->read_element_data;
	internal_destination_range_list->write_element_data = internal_source_range_list->write_element_data;

	*destination_range_list = (libfdata_range_list_t *) internal_destination_range_list;

	return( 1 );

on_error:
	if( internal_destination_range_list != NULL )
	{
		if( ( internal_destination_range_list->data_handle != NULL )
		 && ( internal_source_range_list->free_data_handle != NULL ) )
		{
			internal_source_range_list->free_data_handle(
			 &( internal_destination_range_list->data_handle ),
			 NULL );
		}
		memory_free(
		 internal_destination_range_list );
	}
	return( -1 );
}

/* List elements functions
 */

/* Empties the range list
 * Returns 1 if successful or -1 on error
 */
int libfdata_range_list_empty(
     libfdata_range_list_t *range_list,
     libcerror_error_t **error )
{
	libfdata_internal_range_list_t *internal_range_list = NULL;
	static char *function                               = "libfdata_range_list_empty";

	if( range_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid range list.",
		 function );

		return( -1 );
	}
	internal_range_list = (libfdata_internal_range_list_t *) range_list;

	if( libcdata_range_list_empty(
	     internal_range_list->elements_range_list,
	     (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_list_free,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_RESIZE_FAILED,
		 "%s: unable to empty elements range list.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Mapped range functions
 */

/* Retrieves the list element for a specific offset
 * Returns 1 if successful, 0 if not or -1 on error
 */
int libfdata_range_list_get_list_element_at_offset(
     libfdata_range_list_t *range_list,
     off64_t offset,
     off64_t *element_data_offset,
     libfdata_list_element_t **element,
     libcerror_error_t **error )
{
	libfdata_internal_range_list_t *internal_range_list = NULL;
	libfdata_list_t *list                               = NULL;
	static char *function                               = "libfdata_range_list_get_list_element_at_offset";
	off64_t mapped_range_offset                         = 0;
	size64_t mapped_range_size                          = 0;
	int element_index                                   = 0;
	int result                                          = 0;

	if( range_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid range list.",
		 function );

		return( -1 );
	}
	internal_range_list = (libfdata_internal_range_list_t *) range_list;

#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: requested offset: 0x%08" PRIx64 "\n",
		 function,
		 offset );
	}
#endif
	result = libcdata_range_list_get_range_at_offset(
	          internal_range_list->elements_range_list,
	          (uint64_t) offset,
	          (uint64_t *) &mapped_range_offset,
	          (uint64_t *) &mapped_range_size,
	          (intptr_t **) &list,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve range from elements range list for offset: %" PRIi64 ".",
		 function,
		 offset );

		return( -1 );
	}
	else if( result != 0 )
	{
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: mapped range: 0x%08" PRIx64 " - 0x%08" PRIx64 " (size: %" PRIu64 ")\n",
			 function,
			 mapped_range_offset,
			 mapped_range_offset + mapped_range_size,
			 mapped_range_size );
		}
#endif
		result = libfdata_list_get_list_element_at_offset(
		          list,
		          offset,
		          &element_index,
		          element_data_offset,
		          element,
		          error );

		if( result == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve element from list for offset: %" PRIi64 ".",
			 function,
			 offset );

			return( -1 );
		}
		else if( result == 0 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: invalid list - element missing for offset: %" PRIi64 ".",
			 function,
			 offset );

			return( -1 );
		}
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

/* Retrieves the data range of an element at a specific offset
 * Returns 1 if successful, 0 if not or -1 on error
 */
int libfdata_range_list_get_element_at_offset(
     libfdata_range_list_t *range_list,
     off64_t offset,
     off64_t *element_data_offset,
     int *element_file_index,
     off64_t *element_offset,
     size64_t *element_size,
     uint32_t *element_flags,
     libcerror_error_t **error )
{
	libfdata_list_element_t *list_element = NULL;
	static char *function                 = "libfdata_list_get_element_at_offset";
	int result                            = 0;

	result = libfdata_range_list_get_list_element_at_offset(
	          range_list,
	          offset,
	          element_data_offset,
	          &list_element,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve element at offset: %" PRIi64 ".",
		 function,
		 offset );

		return( -1 );
	}
	else if( result != 0 )
	{
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
			 "%s: unable to retrieve data range of list element at offset: %" PRIi64 ".",
			 function,
			 offset );

			return( -1 );
		}
	}
	return( result );
}

/* Inserts an element data range based on its mapped range
 * Returns 1 if successful or -1 on error
 */
int libfdata_range_list_insert_element(
     libfdata_range_list_t *range_list,
     off64_t offset,
     size64_t size,
     int element_file_index,
     off64_t element_offset,
     size64_t element_size,
     uint32_t element_flags,
     libcerror_error_t **error )
{
	libfdata_internal_range_list_t *internal_range_list = NULL;
	libfdata_list_t *list                               = NULL;
	libfdata_list_element_t *list_element               = NULL;
	static char *function                               = "libfdata_range_list_insert_element";
	off64_t element_data_offset                         = 0;
	off64_t mapped_range_offset                         = 0;
	size64_t element_mapped_size                        = 0;
	size64_t mapped_range_size                          = 0;
	int element_index                                   = 0;
	int result                                          = 0;

	if( range_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid range list.",
		 function );

		return( -1 );
	}
	internal_range_list = (libfdata_internal_range_list_t *) range_list;

	if( offset < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid offset value out of bounds.",
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
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: file index: %03d offset: 0x%08" PRIx64 " - 0x%08" PRIx64 " (size: %" PRIu64 ")\n",
		 function,
		 element_file_index,
		 element_offset,
		 element_offset + element_size,
		 element_size );

		libcnotify_printf(
		 "%s: requested range: 0x%08" PRIx64 " - 0x%08" PRIx64 " (size: %" PRIu64 ")\n",
		 function,
		 offset,
		 offset + size,
		 size );
	}
#endif
	result = libcdata_range_list_get_range_at_offset(
	          internal_range_list->elements_range_list,
	          (uint64_t) offset,
	          (uint64_t *) &mapped_range_offset,
	          (uint64_t *) &mapped_range_size,
	          (intptr_t **) &list,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve range from elements range list for offset: %" PRIi64 ".",
		 function,
		 offset );

		return( -1 );
	}
	else if( result != 0 )
	{
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: mapped range: 0x%08" PRIx64 " - 0x%08" PRIx64 " (size: %" PRIu64 ")\n",
			 function,
			 mapped_range_offset,
			 mapped_range_offset + mapped_range_size,
			 mapped_range_size );
		}
#endif
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
			 "%s: unable to retrieve element from list for offset: %" PRIi64 ".",
			 function,
			 offset );

			return( -1 );
		}
		else if( result == 0 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: invalid list - element missing for offset: %" PRIi64 ".",
			 function,
			 offset );

			return( -1 );
		}
		if( element_data_offset != 0 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: invalid offset value out of bounds.",
			 function );

			return( -1 );
		}
		if( libfdata_list_element_get_mapped_size(
		     list_element,
		     &element_mapped_size,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve mapped size from list element at offset: %" PRIi64 ".",
			 function,
			 offset );

			return( -1 );
		}
		if( size != element_mapped_size )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: invalid size value out of bounds.",
			 function );

			return( -1 );
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
			 "%s: unable to set data range in list element at offset: %" PRIi64 ".",
			 function,
			 offset );

			return( -1 );
		}
	}
	else
	{
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: no mapped range found.\n",
			 function );
		}
#endif
		if( libfdata_list_initialize(
		     &list,
		     internal_range_list->data_handle,
		     internal_range_list->free_data_handle,
		     internal_range_list->clone_data_handle,
		     internal_range_list->read_element_data,
		     internal_range_list->write_element_data,
		     LIBFDATA_DATA_HANDLE_FLAG_NON_MANAGED,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create list.",
			 function );

			return( -1 );
		}
		if( libfdata_list_set_mapped_offset(
		     list,
		     offset,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set mapped offset in list.",
			 function );

			libfdata_list_free(
			 &list,
			 NULL );

			return( -1 );
		}
		if( libfdata_list_append_element_with_mapped_size(
		     list,
		     &element_index,
		     element_file_index,
		     element_offset,
		     element_size,
		     element_flags,
		     size,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
			 "%s: unable to append element to list.",
			 function );

			libfdata_list_free(
			 &list,
			 NULL );

			return( -1 );
		}
		if( libcdata_range_list_insert_range(
		     internal_range_list->elements_range_list,
		     (uint64_t) offset,
		     (uint64_t) size,
		     (intptr_t *) list,
		     (int (*)(intptr_t **, libcerror_error_t **)) &libfdata_list_free,
		     (int (*)(intptr_t *, intptr_t *, libcerror_error_t **)) &libfdata_list_append_list,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
			 "%s: unable to insert range in elements range list.",
			 function );

			libfdata_list_free(
			 &list,
			 NULL );

			return( -1 );
		}
		list = NULL;
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

/* List element value functions
 */

/* Retrieves the value an element at a specific offset
 * Returns 1 if successful, 0 if not or -1 on error
 */
int libfdata_range_list_get_element_value_at_offset(
     libfdata_range_list_t *range_list,
     intptr_t *file_io_handle,
     libfcache_cache_t *cache,
     off64_t offset,
     off64_t *element_data_offset,
     intptr_t **element_value,
     uint8_t read_flags,
     libcerror_error_t **error )
{
	libfdata_internal_range_list_t *internal_range_list = NULL;
	libfdata_list_t *list                               = NULL;
	static char *function                               = "libfdata_range_list_get_element_value_at_offset";
	off64_t mapped_range_offset                         = 0;
	size64_t mapped_range_size                          = 0;
	int element_index                                   = 0;
	int result                                          = 0;

	if( range_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid range list.",
		 function );

		return( -1 );
	}
	internal_range_list = (libfdata_internal_range_list_t *) range_list;

#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: requested offset: 0x%08" PRIx64 "\n",
		 function,
		 offset );
	}
#endif
	result = libcdata_range_list_get_range_at_offset(
	          internal_range_list->elements_range_list,
	          (uint64_t) offset,
	          (uint64_t *) &mapped_range_offset,
	          (uint64_t *) &mapped_range_size,
	          (intptr_t **) &list,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve range from elements range list for offset: %" PRIi64 ".",
		 function,
		 offset );

		return( -1 );
	}
	else if( result != 0 )
	{
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: mapped range: 0x%08" PRIx64 " - 0x%08" PRIx64 " (size: %" PRIu64 ")\n",
			 function,
			 mapped_range_offset,
			 mapped_range_offset + mapped_range_size,
			 mapped_range_size );
		}
#endif
		result = libfdata_list_get_element_value_at_offset(
		          list,
		          file_io_handle,
		          cache,
		          offset,
		          &element_index,
		          element_data_offset,
		          element_value,
		          read_flags,
		          error );

		if( result == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve element value from list for offset: %" PRIi64 ".",
			 function,
			 offset );

			return( -1 );
		}
		else if( result == 0 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: invalid list - element value missing for offset: %" PRIi64 ".",
			 function,
			 offset );

			return( -1 );
		}
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

/* Sets the value of an element at a specific offset
 *
 * If the flag LIBFDATA_RANGE_LIST_VALUE_FLAG_MANAGED is set the list
 * takes over management of the value and the value is freed when
 * no longer needed.
 *
 * Returns 1 if successful, 0 if not or -1 on error
 */
int libfdata_range_list_set_element_value_at_offset(
     libfdata_range_list_t *range_list,
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
	libfdata_internal_range_list_t *internal_range_list = NULL;
	libfdata_list_t *list                               = NULL;
	static char *function                               = "libfdata_range_list_set_element_value_at_offset";
	off64_t mapped_range_offset                         = 0;
	size64_t mapped_range_size                          = 0;
	int result                                          = 0;

	if( range_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid range list.",
		 function );

		return( -1 );
	}
	internal_range_list = (libfdata_internal_range_list_t *) range_list;

#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: requested offset: 0x%08" PRIx64 "\n",
		 function,
		 offset );
	}
#endif
	result = libcdata_range_list_get_range_at_offset(
	          internal_range_list->elements_range_list,
	          (uint64_t) offset,
	          (uint64_t *) &mapped_range_offset,
	          (uint64_t *) &mapped_range_size,
	          (intptr_t **) &list,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve range from elements range list for offset: %" PRIi64 ".",
		 function,
		 offset );

		return( -1 );
	}
	else if( result != 0 )
	{
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: mapped range: 0x%08" PRIx64 " - 0x%08" PRIx64 " (size: %" PRIu64 ")\n",
			 function,
			 mapped_range_offset,
			 mapped_range_offset + mapped_range_size,
			 mapped_range_size );
		}
#endif
		result = libfdata_list_set_element_value_at_offset(
		          list,
		          file_io_handle,
		          cache,
		          offset,
		          element_value,
		          free_element_value,
		          write_flags,
		          error );

		if( result == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set element value in list for offset: %" PRIi64 ".",
			 function,
			 offset );

			return( -1 );
		}
		else if( result == 0 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: invalid list - element value missing for offset: %" PRIi64 ".",
			 function,
			 offset );

			return( -1 );
		}
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

