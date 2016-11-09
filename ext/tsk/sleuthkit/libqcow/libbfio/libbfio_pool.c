/*
 * The internal pool functions
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
#include <memory.h>
#include <types.h>

#if defined( HAVE_UNISTD_H )
#include <unistd.h>
#endif

#include "libbfio_definitions.h"
#include "libbfio_handle.h"
#include "libbfio_libcdata.h"
#include "libbfio_libcerror.h"
#include "libbfio_pool.h"
#include "libbfio_types.h"

/* Creates a pool
 * Make sure the value pool is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libbfio_pool_initialize(
     libbfio_pool_t **pool,
     int number_of_handles,
     int maximum_number_of_open_handles,
     libcerror_error_t **error )
{
	libbfio_internal_pool_t *internal_pool = NULL;
	static char *function                  = "libbfio_pool_initialize";

	if( pool == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid pool.",
		 function );

		return( -1 );
	}
	if( *pool != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid pool value already set.",
		 function );

		return( -1 );
	}
	if( number_of_handles < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_LESS_THAN_ZERO,
		 "%s: invalid number of handles value less than zero.",
		 function );

		return( -1 );
	}
	if( maximum_number_of_open_handles < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_LESS_THAN_ZERO,
		 "%s: invalid maximum number of open handles value less than zero.",
		 function );

		return( -1 );
	}
	internal_pool = memory_allocate_structure(
	                 libbfio_internal_pool_t );

	if( internal_pool == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create pool.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_pool,
	     0,
	     sizeof( libbfio_internal_pool_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear pool.",
		 function );

		memory_free(
		 internal_pool );

		return( -1 );
	}
	if( libcdata_array_initialize(
	     &( internal_pool->handles_array ),
	     number_of_handles,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create handles array.",
		 function );

		goto on_error;
	}
	if( libcdata_list_initialize(
	     &( internal_pool->last_used_list ),
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create last used list.",
		 function );

		goto on_error;
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBBFIO )
	if( libcthreads_read_write_lock_initialize(
	     &( internal_pool->read_write_lock ),
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to intialize read/write lock.",
		 function );

		goto on_error;
	}
#endif
	internal_pool->maximum_number_of_open_handles = maximum_number_of_open_handles;

	*pool = (libbfio_pool_t *) internal_pool;

	return( 1 );

on_error:
	if( internal_pool != NULL )
	{
		if( internal_pool->last_used_list != NULL )
		{
			libcdata_list_free(
			 &( internal_pool->last_used_list ),
			 NULL,
			 NULL );
		}
		if( internal_pool->handles_array != NULL )
		{
			libcdata_array_free(
			 &( internal_pool->handles_array ),
			 (int (*)(intptr_t **, libcerror_error_t **)) &libbfio_handle_free,
			 NULL );
		}
		memory_free(
		 internal_pool );
	}
	return( -1 );
}

/* Frees a pool
 * Returns 1 if successful or -1 on error
 */
int libbfio_pool_free(
     libbfio_pool_t **pool,
     libcerror_error_t **error )
{
	libbfio_internal_pool_t *internal_pool = NULL;
	static char *function                  = "libbfio_pool_free";
	int result                             = 1;

	if( pool == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid pool.",
		 function );

		return( -1 );
	}
	if( *pool != NULL )
	{
		internal_pool = (libbfio_internal_pool_t *) *pool;
		*pool         = NULL;

#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBBFIO )
		if( libcthreads_read_write_lock_free(
		     &( internal_pool->read_write_lock ),
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free read/write lock.",
			 function );

			result = -1;
		}
#endif
		if( libcdata_array_free(
		     &( internal_pool->handles_array ),
		     (int (*)(intptr_t **, libcerror_error_t **)) &libbfio_handle_free,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free handles array.",
			 function );

			result = -1;
		}
		if( libcdata_list_free(
		     &( internal_pool->last_used_list ),
		     NULL,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free last used list.",
			 function );

			result = -1;
		}
		memory_free(
		 internal_pool );
	}
	return( result );
}

/* Clones (duplicates) the pool
 * The values in the offsets read list are not duplicated
 * Returns 1 if successful or -1 on error
 */
int libbfio_pool_clone(
     libbfio_pool_t **destination_pool,
     libbfio_pool_t *source_pool,
     libcerror_error_t **error )
{
	libbfio_internal_pool_t *internal_destination_pool = NULL;
	libbfio_internal_pool_t *internal_source_pool      = NULL;
	static char *function                              = "libbfio_pool_clone";

	if( destination_pool == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid destination pool.",
		 function );

		return( -1 );
	}
	if( *destination_pool != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: destination pool already set.",
		 function );

		return( -1 );
	}
	if( source_pool == NULL )
	{
		*destination_pool = NULL;

		return( 1 );
	}
	internal_source_pool = (libbfio_internal_pool_t *) source_pool;

#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBBFIO )
	if( libcthreads_read_write_lock_grab_for_read(
	     internal_source_pool->read_write_lock,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to grab read/write lock for reading.",
		 function );

		return( -1 );
	}
#endif
	internal_destination_pool = memory_allocate_structure(
	                             libbfio_internal_pool_t );

	if( internal_destination_pool == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create destination pool.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_destination_pool,
	     0,
	     sizeof( libbfio_internal_pool_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear destination pool.",
		 function );

		memory_free(
		 internal_destination_pool );

		return( -1 );
	}
	if( libcdata_array_clone(
	     &( internal_destination_pool->handles_array ),
	     internal_source_pool->handles_array,
	     (int (*)(intptr_t **, libcerror_error_t **)) &libbfio_handle_free,
	     (int (*)(intptr_t **, intptr_t *, libcerror_error_t **)) &libbfio_handle_clone,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create handles array.",
		 function );

		goto on_error;
	}
	if( libcdata_list_initialize(
	     &( internal_destination_pool->last_used_list ),
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create last used list.",
		 function );

		goto on_error;
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBBFIO )
	if( libcthreads_read_write_lock_initialize(
	     &( internal_destination_pool->read_write_lock ),
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to intialize read/write lock.",
		 function );

		goto on_error;
	}
#endif
	internal_destination_pool->maximum_number_of_open_handles = internal_source_pool->maximum_number_of_open_handles;

#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBBFIO )
	if( libcthreads_read_write_lock_grab_for_read(
	     internal_source_pool->read_write_lock,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to grab read/write lock for reading.",
		 function );

		goto on_error;
	}
#endif
	*destination_pool = (libbfio_pool_t *) internal_destination_pool;

	return( 1 );

on_error:
	if( internal_destination_pool != NULL )
	{
		if( internal_destination_pool->last_used_list != NULL )
		{
			libcdata_list_free(
			 &( internal_destination_pool->last_used_list ),
			 NULL,
			 NULL );
		}
		if( internal_destination_pool->handles_array != NULL )
		{
			libcdata_array_free(
			 &( internal_destination_pool->handles_array ),
			 (int (*)(intptr_t **, libcerror_error_t **)) &libbfio_handle_free,
			 NULL );
		}
		memory_free(
		 internal_destination_pool );
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBBFIO )
	libcthreads_read_write_lock_free(
	 &( internal_source_pool->read_write_lock ),
	 NULL );
#endif
	return( -1 );
}

/* Resizes the pool
 * Returns 1 if successful or -1 on error
 */
int libbfio_pool_resize(
     libbfio_pool_t *pool,
     int number_of_handles,
     libcerror_error_t **error )
{
	libbfio_internal_pool_t *internal_pool = NULL;
	static char *function                  = "libbfio_pool_resize";

	if( pool == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid pool.",
		 function );

		return( -1 );
	}
	internal_pool = (libbfio_internal_pool_t *) pool;

	if( libcdata_array_resize(
	     internal_pool->handles_array,
	     number_of_handles,
	     (int (*)(intptr_t **, libcerror_error_t **)) &libbfio_handle_free,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to resize handles array.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Opens the handle
 * Returns 1 if successful or -1 on error
 */
int libbfio_pool_open_handle(
     libbfio_internal_pool_t *internal_pool,
     libbfio_handle_t *handle,
     int access_flags,
     libcerror_error_t **error )
{
	libbfio_internal_handle_t *internal_handle = NULL;
	static char *function                      = "libbfio_pool_open_handle";
	int is_open                                = 0;

	if( internal_pool == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid pool.",
		 function );

		return( -1 );
	}
	if( internal_pool->last_used_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid pool - missing last used list.",
		 function );

		return( -1 );
	}
	if( handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid handle.",
		 function );

		return( -1 );
	}
	is_open = libbfio_handle_is_open(
	           handle,
	           error );

	if( is_open == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to determine if handle is open.",
		 function );

		return( -1 );
	}
	else if( is_open == 1 )
	{
		return( 1 );
	}
	if( internal_pool->maximum_number_of_open_handles != LIBBFIO_POOL_UNLIMITED_NUMBER_OF_OPEN_HANDLES )
	{
		if( libbfio_pool_append_handle_to_last_used_list(
		     internal_pool,
		     handle,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
			 "%s: unable to append handle to last used list.",
			 function );

			return( -1 );
		}
	}
	if( libbfio_handle_open(
	     handle,
	     access_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_OPEN_FAILED,
		 "%s: unable to open handle.",
		 function );

		return( -1 );
	}
	internal_handle = (libbfio_internal_handle_t *) handle;

	if( libbfio_handle_seek_offset(
	     handle,
	     internal_handle->current_offset,
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
	return( 1 );
}

/* Appends the handle to the last used list
 * Closes the less frequently used handle if necessary
 * Returns 1 if successful or -1 on error
 */
int libbfio_pool_append_handle_to_last_used_list(
     libbfio_internal_pool_t *internal_pool,
     libbfio_handle_t *handle,
     libcerror_error_t **error )
{
	libbfio_internal_handle_t *internal_handle      = NULL;
	libcdata_list_element_t *last_used_list_element = NULL;
	static char *function                           = "libbfio_pool_append_handle_to_last_used_list";

	if( internal_pool == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid pool.",
		 function );

		return( -1 );
	}
	if( internal_pool->last_used_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid pool - missing last used list.",
		 function );

		return( -1 );
	}
	if( handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid handle.",
		 function );

		return( -1 );
	}
	/* Check if there is room in the pool for another open handle
	 */
	if( ( internal_pool->maximum_number_of_open_handles != LIBBFIO_POOL_UNLIMITED_NUMBER_OF_OPEN_HANDLES )
	 && ( ( internal_pool->number_of_open_handles + 1 ) >= internal_pool->maximum_number_of_open_handles ) )
	{
		if( libcdata_list_get_last_element(
		     internal_pool->last_used_list,
		     &last_used_list_element,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve last list element from last used list.",
			 function );

			return( -1 );
		}
		if( libcdata_list_element_get_value(
		     last_used_list_element,
		     (intptr_t **) &internal_handle,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve value from last used list element.",
			 function );

			return( -1 );
		}
		if( libcdata_list_remove_element(
		     internal_pool->last_used_list,
		     last_used_list_element,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_REMOVE_FAILED,
			 "%s: unable to remove last list element from last used list.",
			 function );

			return( -1 );
		}
		if( internal_handle != NULL )
		{
			if( libbfio_handle_close(
			     (libbfio_handle_t *) internal_handle,
			     error ) != 0 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_IO,
				 LIBCERROR_IO_ERROR_CLOSE_FAILED,
				 "%s: unable to close handle.",
				 function );

				libcdata_list_element_free(
				 &last_used_list_element,
				 NULL,
				 NULL );

				return( -1 );
			}
			internal_handle->pool_last_used_list_element = NULL;

			/* Make sure the truncate flag is removed from the handle
			 */
			internal_handle->access_flags &= ~( LIBBFIO_ACCESS_FLAG_TRUNCATE );
		}
		/* The last used list element is reused to contain the new last used entry
		 */
	}
	else
	{
		if( libcdata_list_element_initialize(
		     &last_used_list_element,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create last used list element.",
			 function );

			return( -1 );
		}
		internal_pool->number_of_open_handles++;
	}
	internal_handle = (libbfio_internal_handle_t *) handle;

	if( libcdata_list_element_set_value(
	     last_used_list_element,
	     (intptr_t *) handle,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set value int last used list element.",
		 function );

		return( -1 );
	}
	internal_handle->pool_last_used_list_element = last_used_list_element;

	if( libcdata_list_prepend_element(
	     internal_pool->last_used_list,
	     last_used_list_element,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
		 "%s: unable to prepend last used list element to list.",
		 function );

		libcdata_list_element_free(
		 &last_used_list_element,
		 NULL,
		 NULL );

		return( -1 );
	}
	return( 1 );
}

/* Moves the handle to the front of the last used list
 * Returns 1 if successful or -1 on error
 */
int libbfio_pool_move_handle_to_front_of_last_used_list(
     libbfio_internal_pool_t *internal_pool,
     libbfio_handle_t *handle,
     libcerror_error_t **error )
{
	libbfio_internal_handle_t *internal_handle      = NULL;
	libcdata_list_element_t *first_list_element     = NULL;
	libcdata_list_element_t *last_used_list_element = NULL;
	static char *function                           = "libbfio_pool_move_handle_to_front_of_last_used_list";

	if( internal_pool == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid pool.",
		 function );

		return( -1 );
	}
	if( internal_pool->last_used_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid pool - missing last used list.",
		 function );

		return( -1 );
	}
	if( handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid handle.",
		 function );

		return( -1 );
	}
	internal_handle = (libbfio_internal_handle_t *) handle;

	if( libcdata_list_get_first_element(
	     internal_pool->last_used_list,
	     &first_list_element,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve first list element from last used list.",
		 function );

		return( -1 );
	}
	last_used_list_element = internal_handle->pool_last_used_list_element;

	if( last_used_list_element == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing last used list element.",
		 function );

		return( -1 );
	}
	if( last_used_list_element != first_list_element )
	{
		if( libcdata_list_remove_element(
		     internal_pool->last_used_list,
		     last_used_list_element,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_REMOVE_FAILED,
			 "%s: unable to remove last used list element from list.",
			 function );

			return( -1 );
		}
		if( libcdata_list_prepend_element(
		     internal_pool->last_used_list,
		     last_used_list_element,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
			 "%s: unable to prepend last used list element to list.",
			 function );

			internal_handle->pool_last_used_list_element = NULL;

			libcdata_list_element_free(
			 &last_used_list_element,
			 NULL,
			 NULL );

			return( -1 );
		}
	}
	return( 1 );
}

/* Retrieves the number of handles in the pool
 * Returns 1 if successful or -1 on error
 */
int libbfio_pool_get_number_of_handles(
     libbfio_pool_t *pool,
     int *number_of_handles,
     libcerror_error_t **error )
{
	libbfio_internal_pool_t *internal_pool = NULL;
	static char *function                  = "libbfio_pool_get_number_of_handles";

	if( pool == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid pool.",
		 function );

		return( -1 );
	}
	internal_pool = (libbfio_internal_pool_t *) pool;

	if( libcdata_array_get_number_of_entries(
	     internal_pool->handles_array,
	     number_of_handles,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of handles.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves a certain handle from the pool
 * Returns 1 if successful or -1 on error
 */
int libbfio_pool_get_handle(
     libbfio_pool_t *pool,
     int entry,
     libbfio_handle_t **handle,
     libcerror_error_t **error )
{
	libbfio_internal_pool_t *internal_pool = NULL;
	static char *function                  = "libbfio_pool_get_handle";

	if( pool == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid pool.",
		 function );

		return( -1 );
	}
	internal_pool = (libbfio_internal_pool_t *) pool;

#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBBFIO )
	if( libcthreads_read_write_lock_grab_for_read(
	     internal_pool->read_write_lock,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to grab read/write lock for reading.",
		 function );

		return( -1 );
	}
#endif
	if( libcdata_array_get_entry_by_index(
	     internal_pool->handles_array,
	     entry,
	     (intptr_t **) handle,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve handle: %d.",
		 function,
		 entry );

		goto on_error;
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBBFIO )
	if( libcthreads_read_write_lock_release_for_read(
	     internal_pool->read_write_lock,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to release read/write lock for reading.",
		 function );

		return( -1 );
	}
#endif
	return( 1 );

on_error:
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBBFIO )
	libcthreads_read_write_lock_release_for_read(
	 internal_pool->read_write_lock,
	 NULL );
#endif
	return( -1 );
}

/* Appends a handle to the pool
 * Sets the entry
 * Returns 1 if successful or -1 on error
 */
int libbfio_pool_append_handle(
     libbfio_pool_t *pool,
     int *entry,
     libbfio_handle_t *handle,
     int access_flags,
     libcerror_error_t **error )
{
	libbfio_internal_pool_t *internal_pool = NULL;
	static char *function                  = "libbfio_pool_append_handle";
	int is_open                            = 0;
	int number_of_handles                  = 0;

	if( pool == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid pool.",
		 function );

		return( -1 );
	}
	internal_pool = (libbfio_internal_pool_t *) pool;

	if( internal_pool->last_used_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid pool - missing last used list.",
		 function );

		return( -1 );
	}
	if( entry == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid entry.",
		 function );

		return( -1 );
	}
	if( handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid handle.",
		 function );

		return( -1 );
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBBFIO )
	if( libcthreads_read_write_lock_grab_for_write(
	     internal_pool->read_write_lock,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to grab read/write lock for writing.",
		 function );

		return( -1 );
	}
#endif
	/* Check if the handle is open
	 */
	is_open = libbfio_handle_is_open(
	           handle,
	           error );

	if( is_open == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to determine if handle is open.",
		 function );

		goto on_error;
	}
	else if( is_open == 0 )
	{
		/* Set the access flags is the handle is not open
		 */
		if( libbfio_handle_set_access_flags(
		     handle,
		     access_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to set access flags.",
			 function );

			goto on_error;
		}
	}
	if( libcdata_array_get_number_of_entries(
	     internal_pool->handles_array,
	     &number_of_handles,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of handles.",
		 function );

		goto on_error;
	}
	if( ( internal_pool->number_of_used_handles + 1 ) >= number_of_handles )
	{
		if( libcdata_array_append_entry(
		     internal_pool->handles_array,
		     entry,
		     (intptr_t *) handle,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
			 "%s: unable to append handle.",
			 function );

			goto on_error;
		}
	}
	else
	{
		*entry = internal_pool->number_of_used_handles;

		if( libcdata_array_set_entry_by_index(
		     internal_pool->handles_array,
		     *entry,
		     (intptr_t *) handle,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to set handle: %d.",
			 function,
			 *entry );

			goto on_error;
		}
	}
	internal_pool->number_of_used_handles += 1;

	if( is_open != 0 )
	{
		if( internal_pool->maximum_number_of_open_handles != LIBBFIO_POOL_UNLIMITED_NUMBER_OF_OPEN_HANDLES )
		{
			if( libbfio_pool_append_handle_to_last_used_list(
			     internal_pool,
			     handle,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
				 "%s: unable to append handle to last used list.",
				 function );

				goto on_error;
			}
		}
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBBFIO )
	if( libcthreads_read_write_lock_release_for_write(
	     internal_pool->read_write_lock,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to release read/write lock for writing.",
		 function );

		return( -1 );
	}
#endif
	return( 1 );

on_error:
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBBFIO )
	libcthreads_read_write_lock_release_for_write(
	 internal_pool->read_write_lock,
	 NULL );
#endif
	return( -1 );
}

/* Sets a certain handle in the pool
 * Returns 1 if successful or -1 on error
 */
int libbfio_pool_set_handle(
     libbfio_pool_t *pool,
     int entry,
     libbfio_handle_t *handle,
     int access_flags,
     libcerror_error_t **error )
{
	libbfio_internal_handle_t *internal_handle = NULL;
	libbfio_internal_pool_t *internal_pool     = NULL;
	static char *function                      = "libbfio_pool_set_handle";
	int is_open                                = 0;

	if( pool == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid pool.",
		 function );

		return( -1 );
	}
	internal_pool = (libbfio_internal_pool_t *) pool;

	if( internal_pool->last_used_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid pool - missing last used list.",
		 function );

		return( -1 );
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBBFIO )
	if( libcthreads_read_write_lock_grab_for_write(
	     internal_pool->read_write_lock,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to grab read/write lock for writing.",
		 function );

		return( -1 );
	}
#endif
	/* Check if the handle is open
	 */
	is_open = libbfio_handle_is_open(
	           handle,
	           error );

	if( is_open == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to determine if handle is open.",
		 function );

		goto on_error;
	}
	else if( is_open == 0 )
	{
		/* Set the access flags is the handle is not open
		 */
		if( libbfio_handle_set_access_flags(
		     handle,
		     access_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to set access flags.",
			 function );

			goto on_error;
		}
	}
	if( libcdata_array_get_entry_by_index(
	     internal_pool->handles_array,
	     entry,
	     (intptr_t **) &internal_handle,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve handle: %d.",
		 function,
		 entry );

		goto on_error;
	}
/* TODO allow to re set handles, make sure all pool references are removed */
	if( internal_handle != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid handle entry: %d value already set.",
		 function,
		 entry );

		goto on_error;
	}
	if( libcdata_array_set_entry_by_index(
	     internal_pool->handles_array,
	     entry,
	     (intptr_t *) handle,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set handle: %d.",
		 function,
		 entry );

		goto on_error;
	}
	if( is_open != 0 )
	{
		if( internal_pool->maximum_number_of_open_handles != LIBBFIO_POOL_UNLIMITED_NUMBER_OF_OPEN_HANDLES )
		{
			if( libbfio_pool_append_handle_to_last_used_list(
			     internal_pool,
			     handle,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
				 "%s: unable to append handle to last used list.",
				 function );

				goto on_error;
			}
		}
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBBFIO )
	if( libcthreads_read_write_lock_release_for_write(
	     internal_pool->read_write_lock,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to release read/write lock for writing.",
		 function );

		return( -1 );
	}
#endif
	return( 1 );

on_error:
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBBFIO )
	libcthreads_read_write_lock_release_for_write(
	 internal_pool->read_write_lock,
	 NULL );
#endif
	return( -1 );
}

/* Removes a certain handle from the pool
 * Returns 1 if successful or -1 on error
 */
int libbfio_pool_remove_handle(
     libbfio_pool_t *pool,
     int entry,
     libbfio_handle_t **handle,
     libcerror_error_t **error )
{
	libbfio_handle_t *last_used_handle              = NULL;
	libbfio_internal_pool_t *internal_pool          = NULL;
	libcdata_list_element_t *last_used_list_element = NULL;
	static char *function                           = "libbfio_pool_remove_handle";

	if( pool == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid pool.",
		 function );

		return( -1 );
	}
	internal_pool = (libbfio_internal_pool_t *) pool;

	if( internal_pool->last_used_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid pool - missing last used list.",
		 function );

		return( -1 );
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBBFIO )
	if( libcthreads_read_write_lock_grab_for_write(
	     internal_pool->read_write_lock,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to grab read/write lock for writing.",
		 function );

		return( -1 );
	}
#endif
	if( libcdata_array_remove_entry(
	     internal_pool->handles_array,
	     entry,
	     (intptr_t **) handle,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_REMOVE_FAILED,
		 "%s: unable to remove handle: %d.",
		 function,
		 entry );

		goto on_error;
	}
	if( libcdata_list_get_first_element(
	     internal_pool->last_used_list,
	     &last_used_list_element,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve first list element from last used list.",
		 function );

		return( -1 );
	}
	while( last_used_list_element != NULL )
	{
		if( libcdata_list_element_get_value(
		     last_used_list_element,
		     (intptr_t **) &last_used_handle,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve last used handle.",
			 function );

			return( -1 );
		}
		if( last_used_handle == *handle )
		{
			break;
		}
		if( libcdata_list_element_get_next_element(
		     last_used_list_element,
		     &last_used_list_element,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve next last used list element.",
			 function );

			return( -1 );
		}
	}
	if( last_used_list_element != NULL )
	{
		if( libcdata_list_remove_element(
		     internal_pool->last_used_list,
		     last_used_list_element,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_REMOVE_FAILED,
			 "%s: unable to remove last list element from last used list.",
			 function );

			return( -1 );
		}
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBBFIO )
	if( libcthreads_read_write_lock_release_for_write(
	     internal_pool->read_write_lock,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to release read/write lock for writing.",
		 function );

		return( -1 );
	}
#endif
	return( 1 );

on_error:
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBBFIO )
	libcthreads_read_write_lock_release_for_write(
	 internal_pool->read_write_lock,
	 NULL );
#endif
	return( -1 );
}

/* Retrieves the maximum number of open handles in the pool
 * Returns 1 if successful or -1 on error
 */
int libbfio_pool_get_maximum_number_of_open_handles(
     libbfio_pool_t *pool,
     int *maximum_number_of_open_handles,
     libcerror_error_t **error )
{
	libbfio_internal_pool_t *internal_pool = NULL;
	static char *function                  = "libbfio_pool_get_maximum_number_of_open_handles";

	if( pool == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid pool.",
		 function );

		return( -1 );
	}
	internal_pool = (libbfio_internal_pool_t *) pool;

	if( maximum_number_of_open_handles == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid maximum number of open handles.",
		 function );

		return( -1 );
	}
	*maximum_number_of_open_handles = internal_pool->maximum_number_of_open_handles;

	return( 1 );
}

/* Sets the maximum number of open handles in the pool
 * Returns 1 if successful or -1 on error
 */
int libbfio_pool_set_maximum_number_of_open_handles(
     libbfio_pool_t *pool,
     int maximum_number_of_open_handles,
     libcerror_error_t **error )
{
	libbfio_internal_handle_t *internal_handle      = NULL;
	libbfio_internal_pool_t *internal_pool          = NULL;
	libcdata_list_element_t *last_used_list_element = NULL;
	static char *function                           = "libbfio_pool_set_maximum_number_of_open_handles";

	if( pool == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid pool.",
		 function );

		return( -1 );
	}
	internal_pool = (libbfio_internal_pool_t *) pool;

	internal_pool->maximum_number_of_open_handles = maximum_number_of_open_handles;

	while( ( internal_pool->maximum_number_of_open_handles != LIBBFIO_POOL_UNLIMITED_NUMBER_OF_OPEN_HANDLES )
	    && ( internal_pool->number_of_open_handles > internal_pool->maximum_number_of_open_handles ) )
	{
		if( libcdata_list_get_last_element(
		     internal_pool->last_used_list,
		     &last_used_list_element,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve last list element from last used list.",
			 function );

			return( -1 );
		}
		if( libcdata_list_element_get_value(
		     last_used_list_element,
		     (intptr_t **) &internal_handle,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve value from last used list element.",
			 function );

			goto on_error;
		}
		if( internal_handle == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing last used list element value.",
			 function );

			goto on_error;
		}
		if( libcdata_list_remove_element(
		     internal_pool->last_used_list,
		     last_used_list_element,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_REMOVE_FAILED,
			 "%s: unable to remove last used list element from list.",
			 function );

			goto on_error;
		}
		if( libbfio_handle_close(
		     (libbfio_handle_t *) internal_handle,
		     error ) != 0 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_CLOSE_FAILED,
			 "%s: unable to close handle.",
			 function );

			goto on_error;
		}
		internal_pool->number_of_open_handles--;

		internal_handle->pool_last_used_list_element = NULL;

		/* Make sure the truncate flag is removed from the handle
		 */
		internal_handle->access_flags &= ~( LIBBFIO_ACCESS_FLAG_TRUNCATE );

		if( libcdata_list_element_free(
		     &last_used_list_element,
		     NULL,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free last used list element.",
			 function );

			goto on_error;
		}
	}
	return( 1 );

on_error:
	if( last_used_list_element != NULL )
	{
		libcdata_list_element_free(
		 &last_used_list_element,
		 NULL,
		 NULL );
	}
	return( -1 );
}

/* Opens a handle in the pool
 * Returns 1 if successful or -1 on error
 */
int libbfio_pool_open(
     libbfio_pool_t *pool,
     int entry,
     int access_flags,
     libcerror_error_t **error )
{
	libbfio_handle_t *handle               = NULL;
	libbfio_internal_pool_t *internal_pool = NULL;
	static char *function                  = "libbfio_pool_open";
	int is_open                            = 0;

	if( pool == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid pool.",
		 function );

		return( -1 );
	}
	internal_pool = (libbfio_internal_pool_t *) pool;

#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBBFIO )
	if( libcthreads_read_write_lock_grab_for_write(
	     internal_pool->read_write_lock,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to grab read/write lock for writing.",
		 function );

		return( -1 );
	}
#endif
	if( libcdata_array_get_entry_by_index(
	     internal_pool->handles_array,
	     entry,
	     (intptr_t **) &handle,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve handle: %d.",
		 function,
		 entry );

		goto on_error;
	}
	/* Make sure the handle is not open
	 */
	is_open = libbfio_handle_is_open(
	           handle,
	           error );

	if( is_open == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to determine if entry: %d is open.",
		 function,
	         entry );

		goto on_error;
	}
	else if( is_open == 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: entry: %d is already open.",
		 function,
	         entry );

		goto on_error;
	}
	if( libbfio_pool_open_handle(
	     internal_pool,
	     handle,
	     access_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_OPEN_FAILED,
		 "%s: unable to open entry: %d.",
		 function,
		 entry );

		goto on_error;
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBBFIO )
	if( libcthreads_read_write_lock_release_for_write(
	     internal_pool->read_write_lock,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to release read/write lock for writing.",
		 function );

		return( -1 );
	}
#endif
	return( 1 );

on_error:
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBBFIO )
	libcthreads_read_write_lock_release_for_write(
	 internal_pool->read_write_lock,
	 NULL );
#endif
	return( -1 );
}

/* Reopens a handle in the pool
 * Returns 1 if successful or -1 on error
 */
int libbfio_pool_reopen(
     libbfio_pool_t *pool,
     int entry,
     int access_flags,
     libcerror_error_t **error )
{
	libbfio_handle_t *handle               = NULL;
	libbfio_internal_pool_t *internal_pool = NULL;
	static char *function                  = "libbfio_pool_reopen";

	if( pool == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid pool.",
		 function );

		return( -1 );
	}
	internal_pool = (libbfio_internal_pool_t *) pool;

	if( libcdata_array_get_entry_by_index(
	     internal_pool->handles_array,
	     entry,
	     (intptr_t **) &handle,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve handle: %d.",
		 function,
		 entry );

		return( -1 );
	}
	if( libbfio_handle_reopen(
	     handle,
	     access_flags,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_OPEN_FAILED,
		 "%s: unable to reopen handle for entry: %d.",
		 function,
		 entry );

		return( -1 );
	}
	return( 1 );
}

/* Closes a handle in the pool
 * Returns 0 if successful or -1 on error
 */
int libbfio_pool_close(
     libbfio_pool_t *pool,
     int entry,
     libcerror_error_t **error )
{
	libbfio_handle_t *handle                        = NULL;
	libbfio_internal_handle_t *internal_handle      = NULL;
	libbfio_internal_pool_t *internal_pool          = NULL;
	libcdata_list_element_t *last_used_list_element = NULL;
	static char *function                           = "libbfio_pool_close";

	if( pool == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid pool.",
		 function );

		return( -1 );
	}
	internal_pool = (libbfio_internal_pool_t *) pool;

	if( libcdata_array_get_entry_by_index(
	     internal_pool->handles_array,
	     entry,
	     (intptr_t **) &handle,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve handle: %d.",
		 function,
		 entry );

		return( -1 );
	}
	if( handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid pool - missing handle for entry: %d.",
		 function,
		 entry );

		return( -1 );
	}
	if( internal_pool->maximum_number_of_open_handles != LIBBFIO_POOL_UNLIMITED_NUMBER_OF_OPEN_HANDLES )
	{
		internal_handle = (libbfio_internal_handle_t *) handle;

/* TODO add function for locks */
		last_used_list_element = internal_handle->pool_last_used_list_element;

		if( libcdata_list_element_get_value(
		     last_used_list_element,
		     (intptr_t **) &internal_handle,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve value from last used list element.",
			 function );

			goto on_error;
		}
		if( internal_handle == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing last used list element value.",
			 function );

			goto on_error;
		}
		if( libcdata_list_remove_element(
		     internal_pool->last_used_list,
		     last_used_list_element,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_REMOVE_FAILED,
			 "%s: unable to remove last used list element from list.",
			 function );

			goto on_error;
		}
/* TODO add function for locks */
		internal_handle->pool_last_used_list_element = NULL;

		if( libcdata_list_element_free(
		     &last_used_list_element,
		     NULL,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free last used list element.",
			 function );

			goto on_error;
		}
	}
	if( libbfio_handle_close(
	     handle,
	     error ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_CLOSE_FAILED,
		 "%s: unable to close handle for entry: %d.",
		 function,
		 entry );

		return( -1 );
	}
	return( 0 );

on_error:
	if( last_used_list_element != NULL )
	{
		libcdata_list_element_free(
		 &last_used_list_element,
		 NULL,
		 NULL );
	}
	return( -1 );
}

/* Closes all the files in the pool
 * Returns 0 if successful or -1 on error
 */
int libbfio_pool_close_all(
     libbfio_pool_t *pool,
     libcerror_error_t **error )
{
	libbfio_handle_t *handle               = NULL;
	libbfio_internal_pool_t *internal_pool = NULL;
	static char *function                  = "libbfio_pool_close_all";
	int entry                              = 0;
	int is_open                            = 0;
	int number_of_handles                  = 0;
	int result                             = 0;

	if( pool == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid pool.",
		 function );

		return( -1 );
	}
	internal_pool = (libbfio_internal_pool_t *) pool;

	if( libcdata_array_get_number_of_entries(
	     internal_pool->handles_array,
	     &number_of_handles,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of handles.",
		 function );

		return( -1 );
	}
	for( entry = 0;
	     entry < number_of_handles;
	     entry++ )
	{
		if( libcdata_array_get_entry_by_index(
		     internal_pool->handles_array,
		     entry,
		     (intptr_t **) &handle,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve handle: %d.",
			 function,
			 entry );

			result = -1;
		}
		if( handle != NULL )
		{
			/* Make sure the handle is open
			 */
			is_open = libbfio_handle_is_open(
				   handle,
				   error );

			if( is_open == -1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to determine if entry: %d is open.",
				 function,
				 entry );

				result = -1;
			}
			else if( is_open != 0 )
			{
/* TODO locking issue ? */
				if( libbfio_pool_close(
				     pool,
				     entry,
				     error ) != 0 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_IO,
					 LIBCERROR_IO_ERROR_CLOSE_FAILED,
					 "%s: unable to close handle: %d.",
					 function,
					 entry );

					result = -1;
				}
			}
		}
	}
	return( result );
}

/* Reads a buffer from a handle in the pool
 * Returns the number of bytes read or -1 on error
 */
ssize_t libbfio_pool_read_buffer(
         libbfio_pool_t *pool,
         int entry,
         uint8_t *buffer,
         size_t size,
         libcerror_error_t **error )
{
	libbfio_handle_t *handle               = NULL;
	libbfio_internal_pool_t *internal_pool = NULL;
	static char *function                  = "libbfio_pool_read_buffer";
	ssize_t read_count                     = 0;
	int access_flags                       = 0;
	int is_open                            = 0;

	if( pool == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid pool.",
		 function );

		return( -1 );
	}
	internal_pool = (libbfio_internal_pool_t *) pool;

	if( libcdata_array_get_entry_by_index(
	     internal_pool->handles_array,
	     entry,
	     (intptr_t **) &handle,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve handle: %d.",
		 function,
		 entry );

		return( -1 );
	}
	/* Make sure the handle is open
	 */
	is_open = libbfio_handle_is_open(
	           handle,
	           error );

	if( is_open == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to determine if entry: %d is open.",
		 function,
	         entry );

		return( -1 );
	}
	else if( is_open == 0 )
	{
		if( libbfio_handle_get_access_flags(
		     handle,
		     &access_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve access flags.",
			 function );

			return( -1 );
		}
		if( libbfio_pool_open_handle(
		     internal_pool,
		     handle,
		     access_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_OPEN_FAILED,
			 "%s: unable to open entry: %d.",
			 function,
			 entry );

			return( -1 );
		}
	}
	if( internal_pool->maximum_number_of_open_handles != LIBBFIO_POOL_UNLIMITED_NUMBER_OF_OPEN_HANDLES )
	{
		if( libbfio_pool_move_handle_to_front_of_last_used_list(
		     internal_pool,
		     handle,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to move handle to front of last used list.",
			 function );

			return( -1 );
		}
	}
	read_count = libbfio_handle_read_buffer(
	              handle,
	              buffer,
	              size,
	              error );

	if( read_count < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read from entry: %d.",
		 function,
		 entry );

		return( -1 );
	}
	return( read_count );
}

/* Writes a buffer to a handle in the pool
 * Returns the number of bytes written or -1 on error
 */
ssize_t libbfio_pool_write_buffer(
         libbfio_pool_t *pool,
         int entry,
         const uint8_t *buffer,
         size_t size,
         libcerror_error_t **error )
{
	libbfio_handle_t *handle               = NULL;
	libbfio_internal_pool_t *internal_pool = NULL;
	static char *function                  = "libbfio_pool_write_buffer";
	ssize_t write_count                    = 0;
	int access_flags                       = 0;
	int is_open                            = 0;

	if( pool == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid pool.",
		 function );

		return( -1 );
	}
	internal_pool = (libbfio_internal_pool_t *) pool;

	if( libcdata_array_get_entry_by_index(
	     internal_pool->handles_array,
	     entry,
	     (intptr_t **) &handle,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve handle: %d.",
		 function,
		 entry );

		return( -1 );
	}
	/* Make sure the handle is open
	 */
	is_open = libbfio_handle_is_open(
	           handle,
	           error );

	if( is_open == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to determine if entry: %d is open.",
		 function,
	         entry );

		return( -1 );
	}
	else if( is_open == 0 )
	{
		if( libbfio_handle_get_access_flags(
		     handle,
		     &access_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve access flags.",
			 function );

			return( -1 );
		}
		if( libbfio_pool_open_handle(
		     internal_pool,
		     handle,
		     access_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_OPEN_FAILED,
			 "%s: unable to open entry: %d.",
			 function,
			 entry );

			return( -1 );
		}
	}
	if( internal_pool->maximum_number_of_open_handles != LIBBFIO_POOL_UNLIMITED_NUMBER_OF_OPEN_HANDLES )
	{
		if( libbfio_pool_move_handle_to_front_of_last_used_list(
		     internal_pool,
		     handle,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to move handle to front of last used list.",
			 function );

			return( -1 );
		}
	}
	write_count = libbfio_handle_write_buffer(
	               handle,
	               buffer,
	               size,
	               error );

	if( write_count < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_WRITE_FAILED,
		 "%s: unable to write to entry: %d.",
		 function,
		 entry );

		return( -1 );
	}
	return( write_count );
}

/* Seeks an offset in a handle in the pool
 * Returns the offset if successful or -1 on error
 */
off64_t libbfio_pool_seek_offset(
         libbfio_pool_t *pool,
         int entry,
         off64_t offset,
         int whence,
         libcerror_error_t **error )
{
	libbfio_handle_t *handle               = NULL;
	libbfio_internal_pool_t *internal_pool = NULL;
	static char *function                  = "libbfio_pool_seek_offset";
	off64_t seek_offset                    = 0;
	int access_flags                       = 0;
	int is_open                            = 0;

	if( pool == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid pool.",
		 function );

		return( -1 );
	}
	internal_pool = (libbfio_internal_pool_t *) pool;

	if( libcdata_array_get_entry_by_index(
	     internal_pool->handles_array,
	     entry,
	     (intptr_t **) &handle,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve handle: %d.",
		 function,
		 entry );

		return( -1 );
	}
	/* Make sure the handle is open
	 */
	is_open = libbfio_handle_is_open(
	           handle,
	           error );

	if( is_open == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to determine if entry: %d is open.",
		 function,
	         entry );

		return( -1 );
	}
	else if( is_open == 0 )
	{
		if( libbfio_handle_get_access_flags(
		     handle,
		     &access_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve access flags.",
			 function );

			return( -1 );
		}
		if( libbfio_pool_open_handle(
		     internal_pool,
		     handle,
		     access_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_OPEN_FAILED,
			 "%s: unable to open entry: %d.",
			 function,
			 entry );

			return( -1 );
		}
	}
	if( internal_pool->maximum_number_of_open_handles != LIBBFIO_POOL_UNLIMITED_NUMBER_OF_OPEN_HANDLES )
	{
		if( libbfio_pool_move_handle_to_front_of_last_used_list(
		     internal_pool,
		     handle,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to move handle to front of last used list.",
			 function );

			return( -1 );
		}
	}
	seek_offset = libbfio_handle_seek_offset(
	               handle,
	               offset,
	               whence,
	               error );

	if( seek_offset == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_SEEK_FAILED,
		 "%s: unable to seek offset: %" PRIi64 " in entry: %d.",
		 function,
		 offset,
		 entry );

		return( -1 );
	}
	return( seek_offset );
}

/* Retrieves the current offset in a handle in the pool
 * Returns 1 if successful or -1 on error
 */
int libbfio_pool_get_offset(
     libbfio_pool_t *pool,
     int entry,
     off64_t *offset,
     libcerror_error_t **error )
{
	libbfio_handle_t *handle               = NULL;
	libbfio_internal_pool_t *internal_pool = NULL;
	static char *function                  = "libbfio_pool_get_offset";
	int access_flags                       = 0;
	int is_open                            = 0;

	if( pool == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid pool.",
		 function );

		return( -1 );
	}
	internal_pool = (libbfio_internal_pool_t *) pool;

	if( libcdata_array_get_entry_by_index(
	     internal_pool->handles_array,
	     entry,
	     (intptr_t **) &handle,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve handle: %d.",
		 function,
		 entry );

		return( -1 );
	}
	/* Make sure the handle is open
	 */
	is_open = libbfio_handle_is_open(
	           handle,
	           error );

	if( is_open == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to determine if entry: %d is open.",
		 function,
	         entry );

		return( -1 );
	}
	else if( is_open == 0 )
	{
		if( libbfio_handle_get_access_flags(
		     handle,
		     &access_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve access flags.",
			 function );

			return( -1 );
		}
		if( libbfio_pool_open_handle(
		     internal_pool,
		     handle,
		     access_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_OPEN_FAILED,
			 "%s: unable to open entry: %d.",
			 function,
			 entry );

			return( -1 );
		}
	}
	if( libbfio_handle_get_offset(
	     handle,
	     offset,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve offset.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the size of  a handle in the pool
 * Returns 1 if successful or -1 on error
 */
int libbfio_pool_get_size(
     libbfio_pool_t *pool,
     int entry,
     size64_t *size,
     libcerror_error_t **error )
{
	libbfio_handle_t *handle               = NULL;
	libbfio_internal_pool_t *internal_pool = NULL;
	static char *function                  = "libbfio_pool_get_size";
	int access_flags                       = 0;
	int is_open                            = 0;

	if( pool == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid pool.",
		 function );

		return( -1 );
	}
	internal_pool = (libbfio_internal_pool_t *) pool;

	if( libcdata_array_get_entry_by_index(
	     internal_pool->handles_array,
	     entry,
	     (intptr_t **) &handle,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve handle: %d.",
		 function,
		 entry );

		return( -1 );
	}
	/* Make sure the handle is open
	 */
	is_open = libbfio_handle_is_open(
	           handle,
	           error );

	if( is_open == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to determine if entry: %d is open.",
		 function,
	         entry );

		return( -1 );
	}
	else if( is_open == 0 )
	{
		if( libbfio_handle_get_access_flags(
		     handle,
		     &access_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve access flags.",
			 function );

			return( -1 );
		}
		if( libbfio_pool_open_handle(
		     internal_pool,
		     handle,
		     access_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_OPEN_FAILED,
			 "%s: unable to open entry: %d.",
			 function,
			 entry );

			return( -1 );
		}
	}
	if( libbfio_handle_get_size(
	     handle,
	     size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve size of entry: %d.",
		 function,
		 entry );

		return( -1 );
	}
	return( 1 );
}

