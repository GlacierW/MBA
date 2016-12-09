/*
 * Array functions
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

#include "libcdata_array.h"
#include "libcdata_definitions.h"
#include "libcdata_libcerror.h"
#include "libcdata_libcthreads.h"
#include "libcdata_types.h"

/* Creates an array
 * Make sure the value array is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libcdata_array_initialize(
     libcdata_array_t **array,
     int number_of_entries,
     libcerror_error_t **error )
{
	libcdata_internal_array_t *internal_array = NULL;
	static char *function                     = "libcdata_array_initialize";
	size_t entries_size                       = 0;
	int number_of_allocated_entries           = 0;

	if( array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid array.",
		 function );

		return( -1 );
	}
	if( *array != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid array value already set.",
		 function );

		return( -1 );
	}
	if( number_of_entries < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_LESS_THAN_ZERO,
		 "%s: invalid number of entries value less than zero.",
		 function );

		return( -1 );
	}
	internal_array = memory_allocate_structure(
	                  libcdata_internal_array_t );

	if( internal_array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create array.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_array,
	     0,
	     sizeof( libcdata_internal_array_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear array.",
		 function );

		memory_free(
		 internal_array );

		return( -1 );
	}
	/* Pre-allocate in blocks of 16 entries
	 */
	number_of_allocated_entries = ( number_of_entries & ~( 15 ) ) + 16;

#if SIZEOF_INT <= SIZEOF_SIZE_T
	if( (size_t) number_of_allocated_entries > (size_t) ( SSIZE_MAX / sizeof( intptr_t * ) ) )
#else
	if( number_of_allocated_entries > (int) ( SSIZE_MAX / sizeof( intptr_t * ) ) )
#endif
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid number of allocated entries value exceeds maximum.",
		 function );

		goto on_error;
	}
	entries_size = sizeof( intptr_t * ) * number_of_allocated_entries;

	if( entries_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid entries size value exceeds maximum.",
		 function );

		goto on_error;
	}
	internal_array->entries = (intptr_t **) memory_allocate(
						 entries_size );

	if( internal_array->entries == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create array entries.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_array->entries,
	     0,
	     entries_size ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear array entries.",
		 function );

		goto on_error;
	}
	internal_array->number_of_allocated_entries = number_of_allocated_entries;
	internal_array->number_of_entries           = number_of_entries;

#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_initialize(
	     &( internal_array->read_write_lock ),
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
	*array = (libcdata_array_t *) internal_array;

	return( 1 );

on_error:
	if( internal_array != NULL )
	{
		if( internal_array->entries != NULL )
		{
			memory_free(
			 internal_array->entries );
		}
		memory_free(
		 internal_array );
	}
	return( -1 );
}

/* Frees an array
 * The entries are freed using the entry_free_function
 * Returns 1 if successful or -1 on error
 */
int libcdata_array_free(
     libcdata_array_t **array,
     int (*entry_free_function)(
            intptr_t **entry,
            libcerror_error_t **error ),
     libcerror_error_t **error )
{
	libcdata_internal_array_t *internal_array = NULL;
	static char *function                     = "libcdata_array_free";
	int result                                = 1;

	if( array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid array.",
		 function );

		return( -1 );
	}
	if( *array != NULL )
	{
		internal_array = (libcdata_internal_array_t *) *array;
		*array         = NULL;

#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
		if( libcthreads_read_write_lock_free(
		     &( internal_array->read_write_lock ),
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
		if( internal_array->entries != NULL )
		{
			if( libcdata_internal_array_clear(
			     internal_array,
			     entry_free_function,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: unable to clear array.",
				 function );

				result = -1;
			}
			memory_free(
			 internal_array->entries );
		}
		memory_free(
		 internal_array );
	}
	return( result );
}

/* Empties an array and frees its entries
 * The entries are freed using the entry_free_function
 * Returns 1 if successful or -1 on error
 */
int libcdata_array_empty(
     libcdata_array_t *array,
     int (*entry_free_function)(
            intptr_t **entry,
            libcerror_error_t **error ),
     libcerror_error_t **error )
{
	libcdata_internal_array_t *internal_array = NULL;
	static char *function                     = "libcdata_array_empty";
	int result                                = 1;

	if( array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid array.",
		 function );

		return( -1 );
	}
	internal_array = (libcdata_internal_array_t *) array;

#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_grab_for_write(
	     internal_array->read_write_lock,
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
	if( libcdata_internal_array_clear(
	     internal_array,
	     entry_free_function,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
		 "%s: unable to clear array.",
		 function );

		result = -1;
	}
	internal_array->number_of_entries = 0;

#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_release_for_write(
	     internal_array->read_write_lock,
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

/* Clears an array and frees its entries
 * The entries are freed using the entry_free_function
 * This function is not multi-thread safe acquire write lock before call
 * Returns 1 if successful or -1 on error
 */
int libcdata_internal_array_clear(
     libcdata_internal_array_t *internal_array,
     int (*entry_free_function)(
            intptr_t **entry,
            libcerror_error_t **error ),
     libcerror_error_t **error )
{
	static char *function = "libcdata_internal_array_clear";
	int entry_iterator    = 0;
	int result            = 1;

	if( internal_array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid array.",
		 function );

		return( -1 );
	}
	if( internal_array->entries != NULL )
	{
		for( entry_iterator = 0;
		     entry_iterator < internal_array->number_of_entries;
		     entry_iterator++ )
		{
			if( internal_array->entries[ entry_iterator ] != NULL )
			{
				if( entry_free_function != NULL )
				{
					if( entry_free_function(
					     &( internal_array->entries[ entry_iterator ] ),
					     error ) != 1 )
					{
						libcerror_error_set(
						 error,
						 LIBCERROR_ERROR_DOMAIN_RUNTIME,
						 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
						 "%s: unable to free array entry: %d.",
						 function,
						 entry_iterator );

						result = -1;
					}
				}
				internal_array->entries[ entry_iterator ] = NULL;
			}
		}
	}
	return( result );
}

/* Clears an array and frees its entries
 * The entries are freed using the entry_free_function
 * Returns 1 if successful or -1 on error
 */
int libcdata_array_clear(
     libcdata_array_t *array,
     int (*entry_free_function)(
            intptr_t **entry,
            libcerror_error_t **error ),
     libcerror_error_t **error )
{
	libcdata_internal_array_t *internal_array = NULL;
	static char *function                     = "libcdata_array_clear";
	int result                                = 1;

	if( array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid array.",
		 function );

		return( -1 );
	}
	internal_array = (libcdata_internal_array_t *) array;

#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_grab_for_write(
	     internal_array->read_write_lock,
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
	if( libcdata_internal_array_clear(
	     internal_array,
	     entry_free_function,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
		 "%s: unable to clear array.",
		 function );

		result = -1;
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_release_for_write(
	     internal_array->read_write_lock,
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

/* Clones the array and its entries
 *
 * The entries are cloned using the entry_clone_function
 * On error the entries are freed using the entry_free_function
 *
 * Returns 1 if successful or -1 on error
 */
int libcdata_array_clone(
     libcdata_array_t **destination_array,
     libcdata_array_t *source_array,
     int (*entry_free_function)(
            intptr_t **entry,
            libcerror_error_t **error ),
     int (*entry_clone_function)(
            intptr_t **destination_entry,
            intptr_t *source_entry,
            libcerror_error_t **error ),
     libcerror_error_t **error )
{
	libcdata_internal_array_t *internal_destination_array = NULL;
	libcdata_internal_array_t *internal_source_array      = NULL;
	static char *function                                 = "libcdata_array_clone";
	int entry_iterator                                    = 0;
	int result                                            = 1;

	if( destination_array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid destination array.",
		 function );

		return( -1 );
	}
	if( *destination_array != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid destination array already set.",
		 function );

		return( -1 );
	}
	if( entry_free_function == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid entry free function.",
		 function );

		return( -1 );
	}
	if( entry_clone_function == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid entry clone function.",
		 function );

		return( -1 );
	}
	if( source_array == NULL )
	{
		*destination_array = NULL;

		return( 1 );
	}
	internal_source_array = (libcdata_internal_array_t *) source_array;

	if( libcdata_array_initialize(
	     destination_array,
	     internal_source_array->number_of_entries,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create destination array.",
		 function );

		goto on_error;
	}
	if( *destination_array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing destination array.",
		 function );

		goto on_error;
	}
	internal_destination_array = (libcdata_internal_array_t *) *destination_array;

#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_grab_for_read(
	     internal_source_array->read_write_lock,
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
	if( internal_source_array->entries != NULL )
	{
		for( entry_iterator = 0;
		     entry_iterator < internal_source_array->number_of_entries;
		     entry_iterator++ )
		{
			if( internal_source_array->entries[ entry_iterator ] != NULL )
			{
				result = entry_clone_function(
				          &( internal_destination_array->entries[ entry_iterator ] ),
				          internal_source_array->entries[ entry_iterator ],
				          error );

				if( result != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
					 "%s: unable to create destination array entry: %d.",
					 function,
					 entry_iterator );

					break;
				}
			}
		}
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_release_for_read(
	     internal_source_array->read_write_lock,
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
	if( *destination_array != NULL )
	{
		libcdata_array_free(
		 destination_array,
		 entry_free_function,
		 NULL );
	}
	return( -1 );
}

/* Resizes an array
 * This function is not multi-thread safe acquire write lock before call
 * Returns 1 if successful or -1 on error
 */
int libcdata_internal_array_resize(
     libcdata_internal_array_t *internal_array,
     int number_of_entries,
     int (*entry_free_function)(
            intptr_t **entry,
            libcerror_error_t **error ),
     libcerror_error_t **error )
{
	void *reallocation              = NULL;
	static char *function           = "libcdata_internal_array_resize";
	size_t entries_size             = 0;
	int entry_iterator              = 0;
	int number_of_allocated_entries = 0;
	int result                      = 1;

	if( internal_array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid array.",
		 function );

		return( -1 );
	}
	if( number_of_entries < 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_LESS_THAN_ZERO,
		 "%s: invalid number of entries value less than zero.",
		 function );

		return( -1 );
	}
	if( number_of_entries > internal_array->number_of_allocated_entries )
	{
		/* Pre-allocate in blocks of 16 entries
		 */
		number_of_allocated_entries = ( number_of_entries & ~( 15 ) ) + 16;

#if SIZEOF_INT <= SIZEOF_SIZE_T
		if( (size_t) number_of_allocated_entries > (size_t) ( SSIZE_MAX / sizeof( intptr_t * ) ) )
#else
		if( number_of_allocated_entries > (int) ( SSIZE_MAX / sizeof( intptr_t * ) ) )
#endif
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
			 "%s: invalid number of allocated entries value exceeds maximum.",
			 function );

			return( -1 );
		}
		entries_size = sizeof( intptr_t * ) * number_of_allocated_entries;

		if( entries_size > (size_t) SSIZE_MAX )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
			 "%s: invalid entries size value exceeds maximum.",
			 function );

			return( -1 );
		}
		reallocation = memory_reallocate(
		                internal_array->entries,
		                entries_size );

		if( reallocation == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_MEMORY,
			 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to resize array entries.",
			 function );

			return( -1 );
		}
		internal_array->entries = (intptr_t **) reallocation;

		if( memory_set(
		     &( internal_array->entries[ internal_array->number_of_allocated_entries ] ),
		     0,
		     sizeof( intptr_t * ) * ( number_of_allocated_entries - internal_array->number_of_allocated_entries ) ) == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_MEMORY,
			 LIBCERROR_MEMORY_ERROR_SET_FAILED,
			 "%s: unable to clear array entries.",
			 function );

			result = -1;
		}
		internal_array->number_of_allocated_entries = number_of_allocated_entries;
		internal_array->number_of_entries           = number_of_entries;
	}
	else if( number_of_entries > internal_array->number_of_entries )
	{
		internal_array->number_of_entries = number_of_entries;
	}
	else if( internal_array->entries != NULL )
	{
		for( entry_iterator = number_of_entries;
		     entry_iterator < internal_array->number_of_entries;
		     entry_iterator++ )
		{
			if( internal_array->entries[ entry_iterator ] != NULL )
			{
				if( entry_free_function == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
					 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
					 "%s: invalid entry free function.",
					 function );

					return( -1 );
				}
				if( entry_free_function(
				     &( internal_array->entries[ entry_iterator ] ),
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
					 "%s: unable to free array entry: %d.",
					 function,
					 entry_iterator );

					result = -1;
				}
				internal_array->entries[ entry_iterator ] = NULL;
			}
		}
		internal_array->number_of_entries = number_of_entries;
	}
	return( result );
}

/* Resizes an array
 * Returns 1 if successful or -1 on error
 */
int libcdata_array_resize(
     libcdata_array_t *array,
     int number_of_entries,
     int (*entry_free_function)(
            intptr_t **entry,
            libcerror_error_t **error ),
     libcerror_error_t **error )
{
	libcdata_internal_array_t *internal_array = NULL;
	static char *function                     = "libcdata_array_resize";
	int result                                = 1;

	if( array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid array.",
		 function );

		return( -1 );
	}
	internal_array = (libcdata_internal_array_t *) array;

#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_grab_for_write(
	     internal_array->read_write_lock,
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
	if( libcdata_internal_array_resize(
	     internal_array,
	     number_of_entries,
	     entry_free_function,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_RESIZE_FAILED,
		 "%s: unable to resize array.",
		 function );

		result = -1;
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_release_for_write(
	     internal_array->read_write_lock,
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

/* Reverses the order of the entries in the array
 * Returns 1 if successful or -1 on error
 */
int libcdata_array_reverse(
     libcdata_array_t *array,
     libcerror_error_t **error )
{
	libcdata_internal_array_t *internal_array = NULL;
	intptr_t *entry                           = NULL;
	static char *function                     = "libcdata_array_reverse";
	int entry_iterator                        = 0;
	int reverse_entry_iterator                = 0;

	if( array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid array.",
		 function );

		return( -1 );
	}
	internal_array = (libcdata_internal_array_t *) array;

	if( internal_array->entries == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid array - missing entries.",
		 function );

		return( -1 );
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_grab_for_write(
	     internal_array->read_write_lock,
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
	if( internal_array->number_of_entries > 1 )
	{
		reverse_entry_iterator = internal_array->number_of_entries - 1;

		while( entry_iterator < reverse_entry_iterator )
		{
			entry = internal_array->entries[ reverse_entry_iterator ];
			internal_array->entries[ reverse_entry_iterator ] = internal_array->entries[ entry_iterator ];
			internal_array->entries[ entry_iterator ] = entry;

			entry_iterator++;
			reverse_entry_iterator--;
		}
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_release_for_write(
	     internal_array->read_write_lock,
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
}

/* Retrieves the number of entries in the array
 * Returns 1 if successful or -1 on error
 */
int libcdata_array_get_number_of_entries(
     libcdata_array_t *array,
     int *number_of_entries,
     libcerror_error_t **error )
{
	libcdata_internal_array_t *internal_array = NULL;
	static char *function                     = "libcdata_array_get_number_of_entries";

	if( array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid array.",
		 function );

		return( -1 );
	}
	internal_array = (libcdata_internal_array_t *) array;

	if( number_of_entries == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid number of entries.",
		 function );

		return( -1 );
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_grab_for_read(
	     internal_array->read_write_lock,
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
	*number_of_entries = internal_array->number_of_entries;

#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_release_for_read(
	     internal_array->read_write_lock,
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

/* Retrieves a specific entry from the array
 * Returns 1 if successful or -1 on error
 */
int libcdata_array_get_entry_by_index(
     libcdata_array_t *array,
     int entry_index,
     intptr_t **entry,
     libcerror_error_t **error )
{
	libcdata_internal_array_t *internal_array = NULL;
	static char *function                     = "libcdata_array_get_entry_by_index";

	if( array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid array.",
		 function );

		return( -1 );
	}
	internal_array = (libcdata_internal_array_t *) array;

	if( internal_array->entries == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid array - missing entries.",
		 function );

		return( -1 );
	}
	if( ( entry_index < 0 )
	 || ( entry_index >= internal_array->number_of_entries ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid entry index value out of bounds.",
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
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_grab_for_read(
	     internal_array->read_write_lock,
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
	*entry = internal_array->entries[ entry_index ];

#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_release_for_read(
	     internal_array->read_write_lock,
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

/* Retrieves a specific entry from the array
 *
 * Uses the entry_compare_function to determine the similarity of the entries
 * The entry_compare_function should return LIBCDATA_COMPARE_LESS,
 * LIBCDATA_COMPARE_EQUAL, LIBCDATA_COMPARE_GREATER if successful or -1 on error
 *
 * Returns 1 if successful, 0 if no such value or -1 on error
 */
int libcdata_array_get_entry_by_value(
     libcdata_array_t *array,
     intptr_t *entry,
     int (*entry_compare_function)(
            intptr_t *first_entry,
            intptr_t *second_entry,
            libcerror_error_t **error ),
     intptr_t **existing_entry,
     libcerror_error_t **error )
{
	libcdata_internal_array_t *internal_array = NULL;
	static char *function                     = "libcdata_array_get_entry_by_value";
	int compare_result                        = 0;
	int entry_index                           = 0;
	int result                                = 0;

	if( array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid array.",
		 function );

		return( -1 );
	}
	internal_array = (libcdata_internal_array_t *) array;

	if( entry_compare_function == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid entry compare function.",
		 function );

		return( -1 );
	}
	if( existing_entry == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid existing entry.",
		 function );

		return( -1 );
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_grab_for_read(
	     internal_array->read_write_lock,
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
	if( internal_array->entries != NULL )
	{
		for( entry_index = 0;
		     entry_index < internal_array->number_of_entries;
		     entry_index++ )
		{
			compare_result = entry_compare_function(
			                  entry,
			                  internal_array->entries[ entry_index ],
			                  error );

			if( compare_result == -1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to compare entry: %d.",
				 function,
				 entry_index );

				result = -1;

				break;
			}
			else if( compare_result == LIBCDATA_COMPARE_EQUAL )
			{
				*existing_entry = internal_array->entries[ entry_index ];

				result = 1;

				break;
			}
		}
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_release_for_read(
	     internal_array->read_write_lock,
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
	return( result );
}

/* Sets a specific entry in the array
 * Returns 1 if successful or -1 on error
 */
int libcdata_array_set_entry_by_index(
     libcdata_array_t *array,
     int entry_index,
     intptr_t *entry,
     libcerror_error_t **error )
{
	libcdata_internal_array_t *internal_array = NULL;
	static char *function                     = "libcdata_array_set_entry_by_index";

	if( array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid array.",
		 function );

		return( -1 );
	}
	internal_array = (libcdata_internal_array_t *) array;

	if( internal_array->entries == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid array - missing entries.",
		 function );

		return( -1 );
	}
	if( ( entry_index < 0 )
	 || ( entry_index >= internal_array->number_of_entries ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid entry index value out of bounds.",
		 function );

		return( -1 );
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_grab_for_write(
	     internal_array->read_write_lock,
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
	internal_array->entries[ entry_index ] = entry;

#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_release_for_write(
	     internal_array->read_write_lock,
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
}

/* Prepends an entry
 * Returns 1 if successful or -1 on error
 */
int libcdata_array_prepend_entry(
     libcdata_array_t *array,
     intptr_t *entry,
     libcerror_error_t **error )
{
	libcdata_internal_array_t *internal_array = NULL;
	static char *function                     = "libcdata_array_prepend_entry";
	int entry_iterator                        = 0;

	if( array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid array.",
		 function );

		return( -1 );
	}
	internal_array = (libcdata_internal_array_t *) array;

#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_grab_for_write(
	     internal_array->read_write_lock,
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
	if( libcdata_internal_array_resize(
	     internal_array,
	     internal_array->number_of_entries + 1,
	     NULL,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_RESIZE_FAILED,
		 "%s: unable to resize array.",
		 function );

		goto on_error;
	}
	if( internal_array->number_of_entries > 1 )
	{
		for( entry_iterator = internal_array->number_of_entries - 2;
		     entry_iterator >= 0;
		     entry_iterator-- )
		{
			internal_array->entries[ entry_iterator + 1 ] = internal_array->entries[ entry_iterator ];
		}
	}
	internal_array->entries[ 0 ] = entry;

#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_release_for_write(
	     internal_array->read_write_lock,
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
	 internal_array->read_write_lock,
	 NULL );
#endif
	return( -1 );
}

/* Appends an entry
 * Sets the entry index to the newly appended entry
 * Returns 1 if successful or -1 on error
 */
int libcdata_array_append_entry(
     libcdata_array_t *array,
     int *entry_index,
     intptr_t *entry,
     libcerror_error_t **error )
{
	libcdata_internal_array_t *internal_array = NULL;
	static char *function                     = "libcdata_array_append_entry";

	if( array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid array.",
		 function );

		return( -1 );
	}
	internal_array = (libcdata_internal_array_t *) array;

	if( entry_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid entry index.",
		 function );

		return( -1 );
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_grab_for_write(
	     internal_array->read_write_lock,
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
	*entry_index = internal_array->number_of_entries;

	if( libcdata_internal_array_resize(
	     internal_array,
	     internal_array->number_of_entries + 1,
	     NULL,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_RESIZE_FAILED,
		 "%s: unable to resize array.",
		 function );

		goto on_error;
	}
	internal_array->entries[ *entry_index ] = entry;

#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_release_for_write(
	     internal_array->read_write_lock,
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
	 internal_array->read_write_lock,
	 NULL );
#endif
	return( -1 );
}

/* Inserts an entry in the array
 *
 * Uses the entry_compare_function to determine the order of the entries
 * The entry_compare_function should return LIBCDATA_COMPARE_LESS,
 * LIBCDATA_COMPARE_EQUAL, LIBCDATA_COMPARE_GREATER if successful or -1 on error
 *
 * Duplicate entries are allowed by default and inserted after the last duplicate entry.
 * Only allowing unique entries can be enforced by setting the flag LIBCDATA_INSERT_FLAG_UNIQUE_ENTRIES
 *
 * Returns 1 if successful, 0 if the entry already exists or -1 on error
 */
int libcdata_array_insert_entry(
     libcdata_array_t *array,
     int *entry_index,
     intptr_t *entry,
     int (*entry_compare_function)(
            intptr_t *first_entry,
            intptr_t *second_entry,
            libcerror_error_t **error ),
     uint8_t insert_flags,
     libcerror_error_t **error )
{
	libcdata_internal_array_t *internal_array = NULL;
	static char *function                     = "libcdata_tree_node_insert_node";
	int compare_result                        = 0;
	int entry_iterator                        = 0;
	int result                                = 1;

	if( array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid array.",
		 function );

		return( -1 );
	}
	internal_array = (libcdata_internal_array_t *) array;

	if( entry_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid entry index.",
		 function );

		return( -1 );
	}
	if( entry_compare_function == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid entry compare function.",
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
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_grab_for_write(
	     internal_array->read_write_lock,
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
	if( internal_array->entries != NULL )
	{
		for( entry_iterator = 0;
		     entry_iterator < internal_array->number_of_entries;
		     entry_iterator++ )
		{
			compare_result = entry_compare_function(
			                  entry,
			                  internal_array->entries[ entry_iterator ],
			                  error );

			if( compare_result == -1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to compare entry: %d.",
				 function,
				 entry_iterator );

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
				 "%s: unsupported entry compare function return value: %d.",
				 function,
				 compare_result );

				goto on_error;
			}
		}
	}
	if( result != 0 )
	{
		if( ( internal_array->entries != NULL )
		 && ( compare_result == LIBCDATA_COMPARE_LESS ) )
		{
			*entry_index = entry_iterator;

			if( libcdata_internal_array_resize(
			     internal_array,
			     internal_array->number_of_entries + 1,
			     NULL,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_RESIZE_FAILED,
				 "%s: unable to resize array.",
				 function );

				goto on_error;
			}
			for( entry_iterator = internal_array->number_of_entries - 1;
			     entry_iterator > *entry_index;
			     entry_iterator-- )
			{
				internal_array->entries[ entry_iterator ] = internal_array->entries[ entry_iterator - 1 ];
			}
			internal_array->entries[ *entry_index ] = entry;
		}
		else
		{
			*entry_index = internal_array->number_of_entries;

			if( libcdata_internal_array_resize(
			     internal_array,
			     internal_array->number_of_entries + 1,
			     NULL,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_RESIZE_FAILED,
				 "%s: unable to resize array.",
				 function );

				goto on_error;
			}
			if( internal_array->entries == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
				 "%s: invalid array - missing entries.",
				 function );

				goto on_error;
			}
			internal_array->entries[ *entry_index ] = entry;
		}
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_release_for_write(
	     internal_array->read_write_lock,
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
	 internal_array->read_write_lock,
	 NULL );
#endif
	return( -1 );
}

/* Removes an entry
 * Returns 1 if successful or -1 on error
 */
int libcdata_array_remove_entry(
     libcdata_array_t *array,
     int entry_index,
     intptr_t **entry,
     libcerror_error_t **error )
{
	libcdata_internal_array_t *internal_array = NULL;
	static char *function                     = "libcdata_array_remove_entry";
	int entry_iterator                        = 0;

	if( array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid array.",
		 function );

		return( -1 );
	}
	internal_array = (libcdata_internal_array_t *) array;

	if( ( entry_index < 0 )
	 || ( entry_index >= internal_array->number_of_entries ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid entry index value out of bounds.",
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
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_grab_for_write(
	     internal_array->read_write_lock,
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
	*entry = internal_array->entries[ entry_index ];

	if( entry_index == ( internal_array->number_of_entries - 1 ) )
	{
		internal_array->entries[ entry_index ] = NULL;
	}
	else
	{
		for( entry_iterator = ( internal_array->number_of_entries - 1 );
		     entry_iterator > entry_index;
		     entry_iterator-- )
		{
			internal_array->entries[ entry_iterator - 1 ] = internal_array->entries[ entry_iterator ];
			internal_array->entries[ entry_iterator ]     = NULL;
		}
	}
	if( libcdata_internal_array_resize(
	     internal_array,
	     internal_array->number_of_entries - 1,
	     NULL,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_RESIZE_FAILED,
		 "%s: unable to resize array.",
		 function );

		goto on_error;
	}
#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	if( libcthreads_read_write_lock_release_for_write(
	     internal_array->read_write_lock,
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
	 internal_array->read_write_lock,
	 NULL );
#endif
	return( -1 );
}

