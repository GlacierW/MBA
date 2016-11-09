/*
 * List functions
 *
 * Copyright (C) 2006-2016, Joachim Metz <joachim.metz@gmail.com>
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

#include "libcdata_definitions.h"
#include "libcdata_libcerror.h"
#include "libcdata_libcthreads.h"
#include "libcdata_list.h"
#include "libcdata_list_element.h"
#include "libcdata_types.h"

/* Creates a list
 * Make sure the value list is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libcdata_list_initialize(
     libcdata_list_t **list,
     libcerror_error_t **error )
{
	libcdata_internal_list_t *internal_list = NULL;
	static char *function                   = "libcdata_list_initialize";

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
	internal_list = memory_allocate_structure(
	                 libcdata_internal_list_t );

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
	     sizeof( libcdata_internal_list_t ) ) == NULL )
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
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_initialize(
	     &( internal_list->read_write_lock ),
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
	*list = (libcdata_list_t *) internal_list;

	return( 1 );

on_error:
	if( internal_list != NULL )
	{
		memory_free(
		 internal_list );
	}
	return( -1 );
}

/* Frees a list including the elements
 * Uses the value_free_function to free the element value
 * Returns 1 if successful or -1 on error
 */
int libcdata_list_free(
     libcdata_list_t **list,
     int (*value_free_function)(
            intptr_t **value,
            libcerror_error_t **error ),
     libcerror_error_t **error )
{
	libcdata_internal_list_t *internal_list = NULL;
	static char *function                   = "libcdata_list_free";
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
		internal_list = (libcdata_internal_list_t *) *list;
		*list         = NULL;

#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
		if( libcthreads_read_write_lock_free(
		     &( internal_list->read_write_lock ),
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
		if( libcdata_internal_list_empty(
		     internal_list,
		     value_free_function,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to empty list.",
			 function );

			result = -1;
		}
		memory_free(
		 internal_list );
	}
	return( result );
}

/* Empties a list and frees the elements
 * Uses the value_free_function to free the element value
 * This function is not multi-thread safe acquire write lock before call
 * Returns 1 if successful or -1 on error
 */
int libcdata_internal_list_empty(
     libcdata_internal_list_t *internal_list,
     int (*value_free_function)(
            intptr_t **value,
            libcerror_error_t **error ),
     libcerror_error_t **error )
{
	libcdata_list_element_t *list_element = NULL;
	libcdata_list_element_t *next_element = NULL;
	static char *function                 = "libcdata_internal_list_empty";
	int element_index                     = 0;
	int number_of_elements                = 0;
	int result                            = 1;

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
	if( internal_list->number_of_elements > 0 )
	{
		number_of_elements = internal_list->number_of_elements;

		list_element = internal_list->first_element;

		for( element_index = 0;
		     element_index < number_of_elements;
		     element_index++ )
		{
			if( libcdata_list_element_get_next_element(
			     list_element,
			     &next_element,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve next element from list element: %d.",
				 function,
				 element_index );

				return( -1 );
			}
			internal_list->first_element = next_element;

			if( internal_list->last_element == list_element )
			{
				internal_list->last_element = next_element;
			}
			internal_list->number_of_elements--;

			if( next_element != NULL )
			{
				if( libcdata_list_element_set_previous_element(
				     next_element,
				     NULL,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to set previous element of list element: %d.",
					 function,
					 element_index + 1 );

					return( -1 );
				}
			}
			if( libcdata_list_element_set_next_element(
			     list_element,
			     NULL,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set next element of list element: %d.",
				 function,
				 element_index );

				return( -1 );
			}
			if( libcdata_list_element_free(
			     &list_element,
			     value_free_function,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: unable to free list element: %d.",
				 function,
				 element_index );

				return( -1 );
			}
			list_element = next_element;
		}
	}
	return( result );
}

/* Empties a list and frees the elements
 * Uses the value_free_function to free the element value
 * Returns 1 if successful or -1 on error
 */
int libcdata_list_empty(
     libcdata_list_t *list,
     int (*value_free_function)(
            intptr_t **value,
            libcerror_error_t **error ),
     libcerror_error_t **error )
{
	libcdata_internal_list_t *internal_list = NULL;
	static char *function                   = "libcdata_list_empty";
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
	internal_list = (libcdata_internal_list_t *) list;

#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_grab_for_write(
	     internal_list->read_write_lock,
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
	if( libcdata_internal_list_empty(
	     internal_list,
	     value_free_function,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
		 "%s: unable to empty list.",
		 function );

		result = -1;
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_release_for_write(
	     internal_list->read_write_lock,
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
	return( result );
}

/* Clones the list and its elements
 *
 * The values are cloned using the value_clone_function
 * On error the values are freed using the value_free_function
 *
 * Returns 1 if successful or -1 on error
 */
int libcdata_list_clone(
     libcdata_list_t **destination_list,
     libcdata_list_t *source_list,
     int (*value_free_function)(
            intptr_t **value,
            libcerror_error_t **error ),
     int (*value_clone_function)(
            intptr_t **destination_value,
            intptr_t *source_value,
            libcerror_error_t **error ),
     libcerror_error_t **error )
{
	libcdata_internal_list_t *internal_source_list = NULL;
	libcdata_list_element_t *source_list_element   = NULL;
	intptr_t *destination_value                    = NULL;
	intptr_t *source_value                         = NULL;
	static char *function                          = "libcdata_list_clone";
	int element_index                              = 0;
	int result                                     = 1;

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
		 "%s: invalid destination list already set.",
		 function );

		return( -1 );
	}
	if( value_free_function == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value free function.",
		 function );

		return( -1 );
	}
	if( value_clone_function == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value clone function.",
		 function );

		return( -1 );
	}
	if( source_list == NULL )
	{
		*destination_list = NULL;

		return( 1 );
	}
	internal_source_list = (libcdata_internal_list_t *) source_list;

	if( libcdata_list_initialize(
	     destination_list,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create destination list.",
		 function );

		goto on_error;
	}
	if( *destination_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing destination list.",
		 function );

		goto on_error;
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_grab_for_read(
	     internal_source_list->read_write_lock,
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
	source_list_element = internal_source_list->first_element;

	for( element_index = 0;
	     element_index < internal_source_list->number_of_elements;
	     element_index++ )
	{
		result = libcdata_list_element_get_value(
		          source_list_element,
		          &source_value,
		          error );

		if( result != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve value from source list element: %d.",
			 function,
			 element_index );

			break;
		}
		result = value_clone_function(
		          &destination_value,
		          source_value,
		          error );

		if( result != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create destination value: %d.",
			 function,
			 element_index );

			break;
		}
		result = libcdata_list_append_value(
		          *destination_list,
		          destination_value,
		          error );

		if( result != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
			 "%s: unable to append destination value to destination list.",
			 function );

			break;
		}
		destination_value = NULL;

		result = libcdata_list_element_get_next_element(
		          source_list_element,
		          &source_list_element,
		          error );

		if( result != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve next element from source list element: %d.",
			 function,
			 element_index );

			break;
		}
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_release_for_read(
	     internal_source_list->read_write_lock,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to release read/write lock for reading.",
		 function );

		goto on_error;
	}
#endif
	if( result != 1 )
	{
		goto on_error;
	}
	return( 1 );

on_error:
	if( destination_value != NULL )
	{
		value_free_function(
		 &destination_value,
		 NULL );
	}
	if( *destination_list != NULL )
	{
		libcdata_list_free(
		 destination_list,
		 value_free_function,
		 error );
	}
	return( -1 );
}

/* Retrieves the number of elements in the list
 * Returns 1 if successful or -1 on error
 */
int libcdata_list_get_number_of_elements(
     libcdata_list_t *list,
     int *number_of_elements,
     libcerror_error_t **error )
{
	libcdata_internal_list_t *internal_list = NULL;
	static char *function                   = "libcdata_list_get_number_of_elements";

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
	internal_list = (libcdata_internal_list_t *) list;

	if( number_of_elements == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid number of elements.",
		 function );

		return( -1 );
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_grab_for_read(
	     internal_list->read_write_lock,
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
	*number_of_elements = internal_list->number_of_elements;

#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_release_for_read(
	     internal_list->read_write_lock,
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
}

/* Retrieves the first elements in the list
 * Returns 1 if successful or -1 on error
 */
int libcdata_list_get_first_element(
     libcdata_list_t *list,
     libcdata_list_element_t **element,
     libcerror_error_t **error )
{
	libcdata_internal_list_t *internal_list = NULL;
	static char *function                   = "libcdata_list_get_first_element";

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
	internal_list = (libcdata_internal_list_t *) list;

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
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_grab_for_read(
	     internal_list->read_write_lock,
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
	*element = internal_list->first_element;

#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_release_for_read(
	     internal_list->read_write_lock,
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
}

/* Sets the first element in the list
 * This function is not multi-thread safe acquire write lock before call
 * Returns 1 if successful or -1 on error
 */
int libcdata_internal_list_set_first_element(
     libcdata_internal_list_t *internal_list,
     libcdata_list_element_t *element,
     libcerror_error_t **error )
{
	static char *function = "libcdata_internal_list_set_first_element";

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
	if( element != NULL )
	{
		if( libcdata_list_element_set_next_element(
		     element,
		     internal_list->first_element,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set next element of list element.",
			 function );

			return( -1 );
		}
	}
	if( internal_list->first_element != NULL )
	{
		if( libcdata_list_element_set_previous_element(
		     internal_list->first_element,
		     element,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set previous element of first element.",
			 function );

			return( -1 );
		}
	}
	internal_list->first_element = element;

	return( 1 );
}

/* Sets the first element in the list
 * Returns 1 if successful or -1 on error
 */
int libcdata_list_set_first_element(
     libcdata_list_t *list,
     libcdata_list_element_t *element,
     libcerror_error_t **error )
{
	libcdata_internal_list_t *internal_list = NULL;
	static char *function                   = "libcdata_list_set_first_element";
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
	internal_list = (libcdata_internal_list_t *) list;

#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_grab_for_write(
	     internal_list->read_write_lock,
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
	if( libcdata_internal_list_set_first_element(
	     internal_list,
	     element,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set first element.",
		 function );

		result = -1;
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_release_for_write(
	     internal_list->read_write_lock,
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
	return( result );
}

/* Retrieves the last elements in the list
 * Returns 1 if successful or -1 on error
 */
int libcdata_list_get_last_element(
     libcdata_list_t *list,
     libcdata_list_element_t **element,
     libcerror_error_t **error )
{
	libcdata_internal_list_t *internal_list = NULL;
	static char *function                   = "libcdata_list_get_last_element";

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
	internal_list = (libcdata_internal_list_t *) list;

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
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_grab_for_read(
	     internal_list->read_write_lock,
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
	*element = internal_list->last_element;

#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_release_for_read(
	     internal_list->read_write_lock,
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
}

/* Sets the last element in the list
 * This function is not multi-thread safe acquire write lock before call
 * Returns 1 if successful or -1 on error
 */
int libcdata_internal_list_set_last_element(
     libcdata_internal_list_t *internal_list,
     libcdata_list_element_t *element,
     libcerror_error_t **error )
{
	static char *function = "libcdata_internal_list_set_last_element";

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
	if( element != NULL )
	{
		if( libcdata_list_element_set_previous_element(
		     element,
		     internal_list->last_element,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set previous element of list element.",
			 function );

			return( -1 );
		}
	}
	if( internal_list->last_element != NULL )
	{
		if( libcdata_list_element_set_next_element(
		     internal_list->last_element,
		     element,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set next element of last element.",
			 function );

			return( -1 );
		}
	}
	internal_list->last_element = element;

	return( 1 );
}

/* Sets the last element in the list
 * Returns 1 if successful or -1 on error
 */
int libcdata_list_set_last_element(
     libcdata_list_t *list,
     libcdata_list_element_t *element,
     libcerror_error_t **error )
{
	libcdata_internal_list_t *internal_list = NULL;
	static char *function                   = "libcdata_list_set_last_element";
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
	internal_list = (libcdata_internal_list_t *) list;

#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_grab_for_write(
	     internal_list->read_write_lock,
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
	if( libcdata_internal_list_set_last_element(
	     internal_list,
	     element,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set last element.",
		 function );

		result = -1;
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_release_for_write(
	     internal_list->read_write_lock,
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
	return( result );
}

/* Retrieves a specific element from the list
 * Returns 1 if successful or -1 on error
 */
int libcdata_list_get_element_by_index(
     libcdata_list_t *list,
     int element_index,
     libcdata_list_element_t **element,
     libcerror_error_t **error )
{
	libcdata_internal_list_t *internal_list = NULL;
	libcdata_list_element_t *list_element   = NULL;
	static char *function                   = "libcdata_list_get_element_by_index";
	int element_iterator                    = 0;

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
	internal_list = (libcdata_internal_list_t *) list;

	if( ( element_index < 0 )
	 || ( element_index >= internal_list->number_of_elements ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid element index value out of bounds.",
		 function );

		return( -1 );
	}
	if( element == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid list element.",
		 function );

		return( -1 );
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_grab_for_read(
	     internal_list->read_write_lock,
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
	if( element_index < ( internal_list->number_of_elements / 2 ) )
	{
		list_element = internal_list->first_element;

		for( element_iterator = 0;
		     element_iterator < element_index;
		     element_iterator++ )
		{
			if( list_element == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
				 "%s: corruption detected in element: %d.",
				 function,
				 element_iterator );

				goto on_error;
			}
			if( libcdata_list_element_get_next_element(
			     list_element,
			     &list_element,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve next element from list element: %d.",
				 function,
				 element_index );

				goto on_error;
			}
		}
	}
	else
	{
		list_element = internal_list->last_element;

		for( element_iterator = ( internal_list->number_of_elements - 1 );
		     element_iterator > element_index;
		     element_iterator-- )
		{
			if( list_element == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
				 "%s: corruption detected in element: %d.",
				 function,
				 element_iterator );

				goto on_error;
			}
			if( libcdata_list_element_get_previous_element(
			     list_element,
			     &list_element,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve previous element from list element: %d.",
				 function,
				 element_index );

				goto on_error;
			}
		}
	}
	if( list_element == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: corruption detected - missing list element.",
		 function );

		goto on_error;
	}
	*element = list_element;

#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_release_for_read(
	     internal_list->read_write_lock,
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
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	libcthreads_read_write_lock_release_for_read(
	 internal_list->read_write_lock,
	 NULL );
#endif
	return( -1 );
}

/* Retrieves a specific value from the list
 * Returns 1 if successful or -1 on error
 */
int libcdata_list_get_value_by_index(
     libcdata_list_t *list,
     int element_index,
     intptr_t **value,
     libcerror_error_t **error )
{
	libcdata_list_element_t *list_element = NULL;
	static char *function                 = "libcdata_list_get_value_by_index";

	if( libcdata_list_get_element_by_index(
	     list,
	     element_index,
	     &list_element,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve list element: %d from list.",
		 function,
		 element_index );

		return( -1 );
	}
	if( libcdata_list_element_get_value(
	     list_element,
	     value,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value from list element: %d.",
		 function,
		 element_index );

		return( -1 );
	}
	return( 1 );
}

/* Prepends a list element to the list
 * Returns 1 if successful or -1 on error
 */
int libcdata_list_prepend_element(
     libcdata_list_t *list,
     libcdata_list_element_t *element,
     libcerror_error_t **error )
{
	libcdata_internal_list_t *internal_list = NULL;
	static char *function                   = "libcdata_list_prepend_element";

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
	internal_list = (libcdata_internal_list_t *) list;

	if( element == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid list element.",
		 function );

		return( -1 );
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_grab_for_write(
	     internal_list->read_write_lock,
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
	if( internal_list->last_element == NULL )
	{
		internal_list->last_element = element;
	}
	if( libcdata_internal_list_set_first_element(
	     internal_list,
	     element,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set first element.",
		 function );

		goto on_error;
	}
	internal_list->number_of_elements++;

#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_release_for_write(
	     internal_list->read_write_lock,
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
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	libcthreads_read_write_lock_release_for_write(
	 internal_list->read_write_lock,
	 NULL );
#endif
	return( -1 );
}

/* Prepends a value to the list
 * Creates a new list element
 * Returns 1 if successful or -1 on error
 */
int libcdata_list_prepend_value(
     libcdata_list_t *list,
     intptr_t *value,
     libcerror_error_t **error )
{
	libcdata_list_element_t *list_element = NULL;
	static char *function                 = "libcdata_list_prepend_value";

	if( libcdata_list_element_initialize(
	     &list_element,
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
	if( libcdata_list_prepend_element(
	     list,
	     list_element,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
		 "%s: unable to prepend element to list.",
		 function );

		goto on_error;
	}
	if( libcdata_list_element_set_value(
	     list_element,
	     value,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to set value of list element.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( list_element != NULL )
	{
		libcdata_list_element_free(
		 &list_element,
		 NULL,
		 NULL );
	}
	return( -1 );
}

/* Appends a list element to the list
 * Returns 1 if successful or -1 on error
 */
int libcdata_list_append_element(
     libcdata_list_t *list,
     libcdata_list_element_t *element,
     libcerror_error_t **error )
{
	libcdata_internal_list_t *internal_list = NULL;
	static char *function                   = "libcdata_list_append_element";

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
	internal_list = (libcdata_internal_list_t *) list;

	if( element == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid list element.",
		 function );

		return( -1 );
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_grab_for_write(
	     internal_list->read_write_lock,
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
	if( internal_list->first_element == NULL )
	{
		internal_list->first_element = element;
	}
	if( libcdata_internal_list_set_last_element(
	     internal_list,
	     element,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set last element.",
		 function );

		goto on_error;
	}
	internal_list->number_of_elements++;

#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_release_for_write(
	     internal_list->read_write_lock,
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
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	libcthreads_read_write_lock_release_for_write(
	 internal_list->read_write_lock,
	 NULL );
#endif
	return( -1 );
}

/* Appends a value to the list
 * Creates a new list element
 * Returns 1 if successful or -1 on error
 */
int libcdata_list_append_value(
     libcdata_list_t *list,
     intptr_t *value,
     libcerror_error_t **error )
{
	libcdata_list_element_t *list_element = NULL;
	static char *function                 = "libcdata_list_append_value";

	if( libcdata_list_element_initialize(
	     &list_element,
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
	if( libcdata_list_append_element(
	     list,
	     list_element,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
		 "%s: unable to append element to list.",
		 function );

		goto on_error;
	}
	if( libcdata_list_element_set_value(
	     list_element,
	     value,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to set value of list element.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( list_element != NULL )
	{
		libcdata_list_element_free(
		 &list_element,
		 NULL,
		 NULL );
	}
	return( -1 );
}

/* Inserts a list element into the list
 *
 * Uses the value_compare_function to determine the order of the entries
 * The value_compare_function should return LIBCDATA_COMPARE_LESS,
 * LIBCDATA_COMPARE_EQUAL, LIBCDATA_COMPARE_GREATER if successful or -1 on error
 *
 * Duplicate entries are allowed by default and inserted after the last duplicate value.
 * Only allowing unique entries can be enforced by setting the flag LIBCDATA_INSERT_FLAG_UNIQUE_ENTRIES
 *
 * Returns 1 if successful, 0 if the list element already exists or -1 on error
 */
int libcdata_list_insert_element(
     libcdata_list_t *list,
     libcdata_list_element_t *element,
     int (*value_compare_function)(
            intptr_t *first_value,
            intptr_t *second_value,
            libcerror_error_t **error ),
     uint8_t insert_flags,
     libcerror_error_t **error )
{
	libcdata_internal_list_t *internal_list   = NULL;
	libcdata_list_element_t *list_element     = NULL;
	libcdata_list_element_t *next_element     = NULL;
	libcdata_list_element_t *previous_element = NULL;
	intptr_t *element_value                   = NULL;
	intptr_t *list_element_value              = NULL;
	static char *function                     = "libcdata_list_insert_element";
	int compare_result                        = 0;
	int element_index                         = 0;
	int result                                = 1;

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
	internal_list = (libcdata_internal_list_t *) list;

	if( element == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid list element.",
		 function );

		return( -1 );
	}
	if( value_compare_function == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value compare function.",
		 function );

		return( -1 );
	}
	if( ( insert_flags & ~( LIBCDATA_INSERT_FLAG_UNIQUE_ENTRIES ) ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported insert flags: 0x%02" PRIx8 ".",
		 function,
		 insert_flags );

		return( -1 );
	}
	if( libcdata_list_element_get_elements(
	     element,
	     &previous_element,
	     &next_element,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve previous and next element from list element.",
		 function );

		return( -1 );
	}
	if( ( previous_element != NULL )
	 || ( next_element != NULL ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: list element already part of a list.",
		 function );

		return( -1 );
	}
	if( libcdata_list_element_get_value(
	     element,
	     &element_value,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve value from list element.",
		 function );

		return( -1 );
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_grab_for_write(
	     internal_list->read_write_lock,
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
	if( internal_list->number_of_elements == 0 )
	{
		if( internal_list->first_element != NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: corruption detected - first element already set.",
			 function );

			goto on_error;
		}
		if( internal_list->last_element != NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: corruption detected - last element already set.",
			 function );

			goto on_error;
		}
		internal_list->first_element = element;
		internal_list->last_element  = element;

		internal_list->number_of_elements++;
	}
	else
	{
		if( internal_list->first_element == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: corruption detected - missing first.",
			 function );

			goto on_error;
		}
		if( internal_list->last_element == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: corruption detected - missing last.",
			 function );

			goto on_error;
		}
		list_element = internal_list->first_element;

		for( element_index = 0;
		     element_index < internal_list->number_of_elements;
		     element_index++ )
		{
			if( libcdata_list_element_get_value(
			     list_element,
			     &list_element_value,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve value from list element: %d.",
				 function,
				 element_index );

				goto on_error;
			}
			compare_result = value_compare_function(
			                  element_value,
			                  list_element_value,
			                  error );

			if( compare_result == -1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to compare list element: %d.",
				 function,
				 element_index );

				goto on_error;
			}
			else if( compare_result == LIBCDATA_COMPARE_EQUAL )
			{
				if( ( insert_flags & LIBCDATA_INSERT_FLAG_UNIQUE_ENTRIES ) != 0 )
				{
					result = 0;

					break;
				}
			}
			else if( compare_result == LIBCDATA_COMPARE_LESS )
			{
				result = 1;

				break;
			}
			else if( compare_result != LIBCDATA_COMPARE_GREATER )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
				 "%s: unsupported value compare function return value: %d.",
				 function,
				 compare_result );

				goto on_error;
			}
			if( libcdata_list_element_get_next_element(
			     list_element,
			     &list_element,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve next element from list element: %d.",
				 function,
				 element_index );

				goto on_error;
			}
		}
		if( result != 0 )
		{
			if( compare_result == LIBCDATA_COMPARE_LESS )
			{
				if( libcdata_list_element_get_previous_element(
				     list_element,
				     &previous_element,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve previous element from list element: %d.",
					 function,
					 element_index );

					goto on_error;
				}
				if( libcdata_list_element_set_elements(
				     element,
				     previous_element,
				     list_element,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to set previous and next element of list element.",
					 function );

					goto on_error;
				}
				if( list_element == internal_list->first_element )
				{
					internal_list->first_element = element;
				}
				else
				{
					if( libcdata_list_element_set_next_element(
					     previous_element,
					     element,
					     error ) != 1 )
					{
						libcerror_error_set(
						 error,
						 LIBCERROR_ERROR_DOMAIN_RUNTIME,
						 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
						 "%s: unable to set next element of list element: %d.",
						 function,
						 element_index - 1 );

						goto on_error;
					}
				}
				if( libcdata_list_element_set_previous_element(
				     list_element,
				     element,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to set previous element of list element: %d.",
					 function,
					 element_index );

					goto on_error;
				}
			}
			else
			{
				if( libcdata_internal_list_set_last_element(
				     internal_list,
				     element,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to set last element.",
					 function );

					goto on_error;
				}
			}
			internal_list->number_of_elements++;
		}
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_release_for_write(
	     internal_list->read_write_lock,
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
	return( result );

on_error:
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	libcthreads_read_write_lock_release_for_write(
	 internal_list->read_write_lock,
	 NULL );
#endif
	return( -1 );
}

/* Inserts a value to the list
 *
 * Creates a new list element
 *
 * Uses the value_compare_function to determine the order of the entries
 * The value_compare_function should return LIBCDATA_COMPARE_LESS,
 * LIBCDATA_COMPARE_EQUAL, LIBCDATA_COMPARE_GREATER if successful or -1 on error
 *
 * Duplicate entries are allowed by default and inserted after the last duplicate value.
 * Only allowing unique entries can be enforced by setting the flag LIBCDATA_INSERT_FLAG_UNIQUE_ENTRIES
 *
 * Returns 1 if successful, 0 if the list element already exists or -1 on error
 */
int libcdata_list_insert_value(
     libcdata_list_t *list,
     intptr_t *value,
     int (*value_compare_function)(
            intptr_t *first_value,
            intptr_t *second_value,
            libcerror_error_t **error ),
     uint8_t insert_flags,
     libcerror_error_t **error )
{
	libcdata_list_element_t *list_element = NULL;
	static char *function                 = "libcdata_list_insert_value";
	int result                            = 0;

	if( libcdata_list_element_initialize(
	     &list_element,
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
	if( libcdata_list_element_set_value(
	     list_element,
	     value,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to set value of list element.",
		 function );

		goto on_error;
	}
	result = libcdata_list_insert_element(
	          list,
	          list_element,
	          value_compare_function,
	          insert_flags,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
		 "%s: unable to insert element to list.",
		 function );

		goto on_error;
	}
	else if( result == 0 )
	{
		if( libcdata_list_element_free(
		     &list_element,
		     NULL,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free list element.",
			 function );

			goto on_error;
		}
	}
	return( result );

on_error:
	if( list_element != NULL )
	{
		libcdata_list_element_free(
		 &list_element,
		 NULL,
		 NULL );
	}
	return( -1 );
}

/* Removes a list element from the list
 * Returns 1 if successful or -1 on error
 */
int libcdata_list_remove_element(
     libcdata_list_t *list,
     libcdata_list_element_t *element,
     libcerror_error_t **error )
{
	libcdata_internal_list_t *internal_list   = NULL;
	libcdata_list_element_t *next_element     = NULL;
	libcdata_list_element_t *previous_element = NULL;
	static char *function                     = "libcdata_list_remove_element";

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
	internal_list = (libcdata_internal_list_t *) list;

	if( element == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid list element.",
		 function );

		return( -1 );
	}
	if( libcdata_list_element_get_elements(
	     element,
	     &previous_element,
	     &next_element,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve previous and next element from list element.",
		 function );

		return( -1 );
	}
	if( element == internal_list->first_element )
	{
		internal_list->first_element = next_element;
	}
	if( element == internal_list->last_element )
	{
		internal_list->last_element = previous_element;
	}
	if( next_element != NULL )
	{
		if( libcdata_list_element_set_previous_element(
		     next_element,
		     previous_element,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set previous element of next element.",
			 function );

			return( -1 );
		}
	}
	if( previous_element != NULL )
	{
		if( libcdata_list_element_set_next_element(
		     previous_element,
		     next_element,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set next element of previous element.",
			 function );

			return( -1 );
		}
	}
	if( libcdata_list_element_set_elements(
	     element,
	     NULL,
	     NULL,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set previous and next element of list element.",
		 function );

		return( -1 );
	}
	internal_list->number_of_elements--;

	return( 1 );
}

