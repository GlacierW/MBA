/*
 * Thread pool functions
 *
 * Copyright (C) 2012-2016, Joachim Metz <joachim.metz@gmail.com>
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

#if defined( WINAPI ) && ( WINVER >= 0x0602 )
#include <Threadpoolapiset.h>
#endif

#include "libcthreads_condition.h"
#include "libcthreads_definitions.h"
#include "libcthreads_libcerror.h"
#include "libcthreads_mutex.h"
#include "libcthreads_queue.h"
#include "libcthreads_thread.h"
#include "libcthreads_thread_attributes.h"
#include "libcthreads_thread_pool.h"
#include "libcthreads_types.h"
#include "libcthreads_unused.h"

#if !defined( HAVE_LOCAL_LIBCTHREADS ) || defined( HAVE_MULTI_THREAD_SUPPORT )

#if defined( WINAPI ) && ( WINVER >= 0x0602 )

void CALLBACK libcthreads_thread_pool_callback_function_helper(
               TP_CALLBACK_INSTANCE *callback_instance,
               void *arguments,
               TP_WORK *thread_pool_work )
{
	libcerror_error_t *error                                 = NULL;
	libcthreads_internal_thread_pool_t *internal_thread_pool = NULL;
	intptr_t *value                                          = NULL;
	int callback_function_result                             = 0;
	int pop_result                                           = 0;

	LIBCTHREADS_UNREFERENCED_PARAMETER( callback_instance )
	LIBCTHREADS_UNREFERENCED_PARAMETER( thread_pool_work )

	if( arguments == NULL )
	{
		return;
	}
	internal_thread_pool = (libcthreads_internal_thread_pool_t *) arguments;

	if( ( internal_thread_pool == NULL )
	 || ( internal_thread_pool->callback_function == NULL ) )
	{
		return;
	}
	pop_result = libcthreads_internal_thread_pool_pop(
		      internal_thread_pool,
		      &value,
		      &error );

	if( pop_result == -1 )
	{
#if defined( HAVE_VERBOSE_OUTPUT )
		libcerror_error_backtrace_fprint(
		 error,
		 stdout );
#endif
		libcerror_error_free(
		 &error );

		return;
	}
	if( pop_result == 0 )
	{
		return;
	}
	callback_function_result = internal_thread_pool->callback_function(
	                            value,
	                            internal_thread_pool->callback_function_arguments );

	if( callback_function_result != 1 )
	{
#if defined( HAVE_VERBOSE_OUTPUT )
		libcerror_error_backtrace_fprint(
		 error,
		 stdout );
#endif
		libcerror_error_free(
		 &error );
	}
}

#elif defined( WINAPI )

/* Start function helper function for WINAPI
 * Returns 0 if successful or 1 on error
 */
DWORD WINAPI libcthreads_thread_pool_callback_function_helper(
              void *arguments )
{
	libcerror_error_t *error                                 = NULL;
	libcthreads_internal_thread_pool_t *internal_thread_pool = NULL;
	intptr_t *value                                          = NULL;
	DWORD result                                             = 1;
	int callback_function_result                             = 0;
	int pop_result                                           = 0;

	if( arguments != NULL )
	{
		internal_thread_pool = (libcthreads_internal_thread_pool_t *) arguments;

		if( ( internal_thread_pool != NULL )
		 && ( internal_thread_pool->callback_function != NULL ) )
		{
			result = 0;

			do
			{
				pop_result = libcthreads_internal_thread_pool_pop(
					      internal_thread_pool,
					      &value,
					      &error );

				if( pop_result == -1 )
				{
					break;
				}
				else if( pop_result != 0 )
				{
					callback_function_result = internal_thread_pool->callback_function(
								    value,
								    internal_thread_pool->callback_function_arguments );

					if( ( callback_function_result != 1 )
					 && ( result == 1 ) )
					{
						result = callback_function_result;
					}
				}
				else if( internal_thread_pool->status == LIBCTHREADS_STATUS_EXIT )
				{
					break;
				}
			}
			while( pop_result != -1 );

			if( pop_result == -1 )
			{
				result = 1;
			}
			if( error != NULL )
			{
#if defined( HAVE_VERBOSE_OUTPUT )
				libcerror_error_backtrace_fprint(
				 error,
				 stdout );
#endif
				libcerror_error_free(
				 &error );
			}
		}
	}
	ExitThread(
	 result );
}

#elif defined( HAVE_PTHREAD_H )

/* Start function helper function for pthread
 * Returns a pointer to a newly allocated int containing 1 if successful or -1 on error
 * NULL is return if the helper function was unable to run the callback
 */
void *libcthreads_thread_pool_callback_function_helper(
       void *arguments )
{
	libcerror_error_t *error                                 = NULL;
	libcthreads_internal_thread_pool_t *internal_thread_pool = NULL;
	intptr_t *value                                          = NULL;
	int *result                                              = NULL;
	int callback_function_result                             = 0;
	int pop_result                                           = 0;

	if( arguments != NULL )
	{
		internal_thread_pool = (libcthreads_internal_thread_pool_t *) arguments;

		if( ( internal_thread_pool != NULL )
		 && ( internal_thread_pool->callback_function != NULL ) )
		{
			result = (int *) memory_allocate(
			                  sizeof( int ) );

			if( result != NULL )
			{
				*result = 1;

				do
				{
					pop_result = libcthreads_internal_thread_pool_pop(
					              internal_thread_pool,
					              &value,
					              &error );

					if( pop_result == -1 )
					{
						break;
					}
					else if( pop_result != 0 )
					{
						callback_function_result = internal_thread_pool->callback_function(
									    value,
									    internal_thread_pool->callback_function_arguments );

						if( ( callback_function_result != 1 )
						 && ( *result == 1 ) )
						{
							*result = callback_function_result;
						}
					}
					else if( internal_thread_pool->status == LIBCTHREADS_STATUS_EXIT )
					{
						break;
					}
				}
				while( pop_result != -1 );

				if( pop_result == -1 )
				{
					*result = -1;
				}
				if( error != NULL )
				{
#if defined( HAVE_VERBOSE_OUTPUT )
					libcerror_error_backtrace_fprint(
					 error,
					 stdout );
#endif
					libcerror_error_free(
					 &error );
				}
			}
		}
	}
	pthread_exit(
	 (void *) result );
}

#endif /* defined( WINAPI ) && ( WINVER >= 0x0602 ) */

/* Creates a thread pool
 * Make sure the value thread_pool is referencing, is set to NULL
 *
 * The callback_function should return 1 if successful and -1 on error
 * Returns 1 if successful or -1 on error
 */
int libcthreads_thread_pool_create(
     libcthreads_thread_pool_t **thread_pool,
     const libcthreads_thread_attributes_t *thread_attributes,
     int number_of_threads,
     int maximum_number_of_values,
     int (*callback_function)(
            intptr_t *value,
            void *arguments ),
     void *callback_function_arguments,
     libcerror_error_t **error )
{
	libcthreads_internal_thread_pool_t *internal_thread_pool = NULL;
	static char *function                                    = "libcthreads_thread_pool_create";
	size_t array_size                                        = 0;

#if !defined( WINAPI ) || ( WINVER < 0x0602 )
	int thread_index                                         = 0;
#endif

#if defined( WINAPI )
	DWORD error_code                                         = 0;
#endif

#if defined( WINAPI ) && ( WINVER >= 0x0602 )
	BOOL result                                              = FALSE;

#elif defined( WINAPI )
	SECURITY_ATTRIBUTES *security_attributes                 = NULL;
	HANDLE thread_handle                                     = NULL;

#elif defined( HAVE_PTHREAD_H )
	pthread_attr_t *attributes                               = NULL;
	int *thread_return_value                                 = NULL;
	int pthread_result                                       = 0;
#endif

#if defined( WINAPI ) && ( WINVER >= 0x0602 )
	LIBCTHREADS_UNREFERENCED_PARAMETER( thread_attributes )
#endif

	if( thread_pool == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid thread pool.",
		 function );

		return( -1 );
	}
	if( *thread_pool != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid thread pool value already set.",
		 function );

		return( -1 );
	}
	if( number_of_threads <= 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_ZERO_OR_LESS,
		 "%s: invalid number of threads value zero or less.",
		 function );

		return( -1 );
	}
#if defined( WINAPI ) && ( WINVER >= 0x0602 )
#if SIZEOF_INT > 4
	if( number_of_threads > (int) UINT32_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid number of threads value exceeds maximum.",
		 function );

		return( -1 );
	}
#endif /* SIZEOF_INT > 4 */

#else
#if SIZEOF_INT <= SIZEOF_SIZE_T
	if( (size_t) number_of_threads > (size_t) ( SSIZE_MAX / sizeof( libcthreads_thread_t * ) ) )
#else
	if( number_of_threads > (int) ( SSIZE_MAX / sizeof( libcthreads_thread_t * ) ) )
#endif
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid number of threads value exceeds maximum.",
		 function );

		return( -1 );
	}
#endif /* defined( WINAPI ) && ( WINVER >= 0x0602 ) */

	if( maximum_number_of_values <= 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_ZERO_OR_LESS,
		 "%s: invalid maximum number of values value zero or less.",
		 function );

		return( -1 );
	}
#if SIZEOF_INT <= SIZEOF_SIZE_T
	if( (size_t) maximum_number_of_values > (size_t) ( SSIZE_MAX / sizeof( intptr_t * ) ) )
#else
	if( maximum_number_of_values > (int) ( SSIZE_MAX / sizeof( intptr_t * ) ) )
#endif
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid maximum number of values value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( callback_function == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid callback function.",
		 function );

		return( -1 );
	}
	internal_thread_pool = memory_allocate_structure(
	                        libcthreads_internal_thread_pool_t );

	if( internal_thread_pool == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create thread pool.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_thread_pool,
	     0,
	     sizeof( libcthreads_internal_thread_pool_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear thread pool.",
		 function );

		memory_free(
		 internal_thread_pool );

		return( -1 );
	}
	array_size = sizeof( intptr_t * ) * maximum_number_of_values;

	if( array_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid values array size value exceeds maximum.",
		 function );

		goto on_error;
	}
	internal_thread_pool->values_array = (intptr_t **) memory_allocate(
	                                                    array_size );

	if( internal_thread_pool->values_array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create values array.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_thread_pool->values_array,
	     0,
	     array_size ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear values array.",
		 function );

		goto on_error;
	}
	internal_thread_pool->allocated_number_of_values = maximum_number_of_values;

	/* The callback must be set before the callback function helper threads are created */
	internal_thread_pool->callback_function           = callback_function;
	internal_thread_pool->callback_function_arguments = callback_function_arguments;

	if( libcthreads_mutex_initialize(
	     &( internal_thread_pool->condition_mutex ),
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create condition mutex.",
		 function );

		goto on_error;
	}
	if( libcthreads_condition_initialize(
	     &( internal_thread_pool->empty_condition ),
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create empty condition.",
		 function );

		goto on_error;
	}
	if( libcthreads_condition_initialize(
	     &( internal_thread_pool->full_condition ),
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create full condition.",
		 function );

		goto on_error;
	}
#if defined( WINAPI ) && ( WINVER >= 0x0602 )
	InitializeThreadpoolEnvironment(
	 &( internal_thread_pool->callback_environment ) );

	internal_thread_pool->thread_pool = CreateThreadpool(
	                                     NULL );

	if( internal_thread_pool->thread_pool == NULL )
	{
		error_code = GetLastError();

		libcerror_system_set_error(
		 error,
		 error_code,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create thread pool.",
		 function );

		goto on_error;
	}
	internal_thread_pool->cleanup_group = CreateThreadpoolCleanupGroup();

	result = SetThreadpoolThreadMinimum(
	          internal_thread_pool->thread_pool,
	          (DWORD) number_of_threads );

	if( result != TRUE )
	{
		error_code = GetLastError();

		libcerror_system_set_error(
		 error,
		 error_code,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set minimum number of threads.",
		 function );

		goto on_error;
	}
	SetThreadpoolThreadMaximum(
	 internal_thread_pool->thread_pool,
	 (DWORD) number_of_threads );

	if( internal_thread_pool->cleanup_group == NULL )
	{
		error_code = GetLastError();

		libcerror_system_set_error(
		 error,
		 error_code,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create thread pool cleanup group.",
		 function );

		goto on_error;
	}
	SetThreadpoolCallbackPool(
	 &( internal_thread_pool->callback_environment ),
	 internal_thread_pool->thread_pool );

	SetThreadpoolCallbackCleanupGroup(
	 &( internal_thread_pool->callback_environment ),
	 internal_thread_pool->cleanup_group,
	 NULL );

#elif defined( WINAPI )
	array_size = sizeof( HANDLE ) * number_of_threads;

	if( array_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid thread handles array size value out of bounds.",
		 function );

		goto on_error;
	}
	internal_thread_pool->thread_handles_array = (HANDLE *) memory_allocate(
	                                                         array_size );

	if( internal_thread_pool->thread_handles_array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create thread handles array.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_thread_pool->thread_handles_array,
	     0,
	     array_size ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear thread handles array.",
		 function );

		goto on_error;
	}
	internal_thread_pool->number_of_threads = number_of_threads;

	array_size = sizeof( DWORD ) * number_of_threads;

	if( array_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid thread identifiers array size value out of bounds.",
		 function );

		goto on_error;
	}
	internal_thread_pool->thread_identifiers_array = (DWORD *) memory_allocate(
	                                                            array_size );

	if( internal_thread_pool->thread_identifiers_array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create thread identifiers array.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_thread_pool->thread_identifiers_array,
	     0,
	     array_size ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear thread identifiers array.",
		 function );

		goto on_error;
	}
	if( thread_attributes != NULL )
	{
		security_attributes = &( ( (libcthreads_internal_thread_attributes_t *) thread_attributes )->security_attributes );
	}
	for( thread_index = 0;
	     thread_index < number_of_threads;
	     thread_index++ )
	{
		thread_handle = CreateThread(
		                 security_attributes,
		                 0, /* stack size */
		                 &libcthreads_thread_pool_callback_function_helper,
		                 (void *) internal_thread_pool,
		                 0, /* creation flags */
		                 &( internal_thread_pool->thread_identifiers_array[ thread_index ] ) );

		if( thread_handle == NULL )
		{
			error_code = GetLastError();

			libcerror_system_set_error(
			 error,
			 error_code,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create thread handle: %d.",
			 function,
			 thread_index );

			goto on_error;
		}
		internal_thread_pool->thread_handles_array[ thread_index ] = thread_handle;
	}

#elif defined( HAVE_PTHREAD_H )
	array_size = sizeof( pthread_t ) * number_of_threads;

	if( array_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid threads array size value out of bounds.",
		 function );

		goto on_error;
	}
	internal_thread_pool->threads_array = (pthread_t *) memory_allocate(
	                                                     array_size );

	if( internal_thread_pool->threads_array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create threads array.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_thread_pool->threads_array,
	     0,
	     array_size ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear threads array.",
		 function );

		goto on_error;
	}
	internal_thread_pool->number_of_threads = number_of_threads;

	if( thread_attributes != NULL )
	{
		attributes = &( ( (libcthreads_internal_thread_attributes_t *) thread_attributes )->attributes );
	}
	for( thread_index = 0;
	     thread_index < number_of_threads;
	     thread_index++ )
	{
		pthread_result = pthread_create(
				  &( internal_thread_pool->threads_array[ thread_index ] ),
				  attributes,
				  &libcthreads_thread_pool_callback_function_helper,
				  (void *) internal_thread_pool );

		if( pthread_result != 0 )
		{
			libcerror_system_set_error(
			 error,
			 pthread_result,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create thread: %d.",
			 function,
			 thread_index );

			goto on_error;
		}
	}
#endif /* defined( WINAPI ) && ( WINVER >= 0x0602 ) */

	*thread_pool = (libcthreads_thread_pool_t *) internal_thread_pool;

	return( 1 );

on_error:
	if( internal_thread_pool != NULL )
	{
#if defined( WINAPI ) && ( WINVER >= 0x0602 )
		if( internal_thread_pool->cleanup_group != NULL )
		{
			CloseThreadpoolCleanupGroup(
			 internal_thread_pool->cleanup_group );

			internal_thread_pool->cleanup_group = NULL;
		}
		if( internal_thread_pool->thread_pool != NULL )
		{
			CloseThreadpool(
			 internal_thread_pool->thread_pool );

			internal_thread_pool->thread_pool = NULL;
		}

#elif defined( WINAPI )
		if( internal_thread_pool->thread_identifiers_array != NULL )
		{
			memory_free(
			 internal_thread_pool->thread_identifiers_array );
		}
		if( internal_thread_pool->thread_handles_array != NULL )
		{
			while( thread_index >= 0 )
			{
				WaitForSingleObject(
				 internal_thread_pool->thread_handles_array[ thread_index ],
				 INFINITE );

				thread_index--;
			}
			memory_free(
			 internal_thread_pool->thread_handles_array );
		}
#elif defined( HAVE_PTHREAD_H )
		if( internal_thread_pool->threads_array != NULL )
		{
			while( thread_index >= 0 )
			{
				pthread_join(
				 internal_thread_pool->threads_array[ thread_index ],
				 (void **) &thread_return_value );

				if( thread_return_value != NULL )
				{
					memory_free(
					 thread_return_value );

					thread_return_value = NULL;
				}
				thread_index--;
			}
			memory_free(
			 internal_thread_pool->threads_array );
		}
#endif /* defined( WINAPI ) && ( WINVER >= 0x0602 ) */

		if( internal_thread_pool->full_condition != NULL )
		{
			libcthreads_condition_free(
			 &( internal_thread_pool->full_condition ),
			 NULL );
		}
		if( internal_thread_pool->empty_condition != NULL )
		{
			libcthreads_condition_free(
			 &( internal_thread_pool->empty_condition ),
			 NULL );
		}
		if( internal_thread_pool->condition_mutex != NULL )
		{
			libcthreads_mutex_free(
			 &( internal_thread_pool->condition_mutex ),
			 NULL );
		}
		if( internal_thread_pool->values_array != NULL )
		{
			memory_free(
			 internal_thread_pool->values_array );
		}
		memory_free(
		 internal_thread_pool );
	}
	return( -1 );
}

/* Pops a value off the queue of the thread pool
 * Returns 1 if successful, 0 if no value available or -1 on error
 */
int libcthreads_internal_thread_pool_pop(
     libcthreads_internal_thread_pool_t *internal_thread_pool,
     intptr_t **value,
     libcerror_error_t **error )
{
	static char *function = "libcthreads_internal_thread_pool_pop";
	int result            = 0;

	if( internal_thread_pool == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid thread pool.",
		 function );

		return( -1 );
	}
	if( internal_thread_pool->values_array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid thread pool - missing values array.",
		 function );

		return( -1 );
	}
	if( libcthreads_mutex_grab(
	     internal_thread_pool->condition_mutex,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to grab condition mutex.",
		 function );

		return( -1 );
	}
	while( internal_thread_pool->number_of_values == 0 )
	{
		if( internal_thread_pool->status == LIBCTHREADS_STATUS_EXIT )
		{
			break;
		}
		if( libcthreads_condition_wait(
		     internal_thread_pool->empty_condition,
		     internal_thread_pool->condition_mutex,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to wait for empty condition.",
			 function );

			goto on_error;
		}
	}
	if( internal_thread_pool->number_of_values > 0 )
	{
		*value = internal_thread_pool->values_array[ internal_thread_pool->pop_index ];

		internal_thread_pool->pop_index++;

		if( internal_thread_pool->pop_index >= internal_thread_pool->allocated_number_of_values )
		{
			internal_thread_pool->pop_index = 0;
		}
		internal_thread_pool->number_of_values--;

		result = 1;

		if( libcthreads_condition_broadcast(
		     internal_thread_pool->full_condition,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to broadcast full condition.",
			 function );

			goto on_error;
		}
	}
	if( libcthreads_mutex_release(
	     internal_thread_pool->condition_mutex,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to release condition mutex.",
		 function );

		return( -1 );
	}
	return( result );

on_error:
	libcthreads_mutex_release(
	 internal_thread_pool->condition_mutex,
	 NULL );

	return( -1 );
}

/* Pushes a value onto the queue of the thread pool
 * Returns 1 if successful or -1 on error
 */
int libcthreads_thread_pool_push(
     libcthreads_thread_pool_t *thread_pool,
     intptr_t *value,
     libcerror_error_t **error )
{
	libcthreads_internal_thread_pool_t *internal_thread_pool = NULL;
	static char *function                                    = "libcthreads_thread_pool_push";

#if defined( WINAPI ) && ( WINVER >= 0x0602 )
	TP_WORK *thread_pool_work                                = NULL;
	DWORD error_code                                         = 0;
#endif

	if( thread_pool == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid thread pool.",
		 function );

		return( -1 );
	}
	internal_thread_pool = (libcthreads_internal_thread_pool_t *) thread_pool;

	if( internal_thread_pool->values_array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid thread pool - missing values array.",
		 function );

		return( -1 );
	}
	if( value == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
		 function );

		return( -1 );
	}
	if( libcthreads_mutex_grab(
	     internal_thread_pool->condition_mutex,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to grab condition mutex.",
		 function );

		return( -1 );
	}
	while( internal_thread_pool->number_of_values == internal_thread_pool->allocated_number_of_values )
	{
		if( libcthreads_condition_wait(
		     internal_thread_pool->full_condition,
		     internal_thread_pool->condition_mutex,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to wait for full condition.",
			 function );

			goto on_error;
		}
	}
	internal_thread_pool->values_array[ internal_thread_pool->push_index ] = value;

	internal_thread_pool->push_index++;

	if( internal_thread_pool->push_index >= internal_thread_pool->allocated_number_of_values )
	{
		internal_thread_pool->push_index = 0;
	}
	internal_thread_pool->number_of_values++;

	/* The condition broadcast must be protected by the mutex for the WINAPI version
	 */
	if( libcthreads_condition_broadcast(
	     internal_thread_pool->empty_condition,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to broadcast empty condition.",
		 function );

		goto on_error;
	}
	if( libcthreads_mutex_release(
	     internal_thread_pool->condition_mutex,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to release condition mutex.",
		 function );

		return( -1 );
	}
#if defined( WINAPI ) && ( WINVER >= 0x0602 )
	thread_pool_work = CreateThreadpoolWork(
	                    &libcthreads_thread_pool_callback_function_helper,
	                    (void *) internal_thread_pool,
	                    &( internal_thread_pool->callback_environment ) );

	if( thread_pool_work == NULL )
	{
		error_code = GetLastError();

		libcerror_system_set_error(
		 error,
		 error_code,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create thread pool work.",
		 function );

		return( -1 );
	}
	SubmitThreadpoolWork(
	 thread_pool_work );

#endif /* defined( WINAPI ) && ( WINVER >= 0x0602 ) */

	return( 1 );

on_error:
	libcthreads_mutex_release(
	 internal_thread_pool->condition_mutex,
	 NULL );

	return( -1 );
}

/* Pushes a value onto the thread pool in sorted order
 *
 * Uses the value_compare_function to determine the similarity of the values
 * The value_compare_function should return LIBCTHREADS_COMPARE_LESS,
 * LIBCTHREADS_COMPARE_EQUAL, LIBCTHREADS_COMPARE_GREATER if successful or -1 on error
 *
 * Returns 1 if successful, 0 if the value already exists or -1 on error
 */
int libcthreads_thread_pool_push_sorted(
     libcthreads_thread_pool_t *thread_pool,
     intptr_t *value,
     int (*value_compare_function)(
            intptr_t *first_value,
            intptr_t *second_value,
            libcerror_error_t **error ),
     uint8_t sort_flags,
     libcerror_error_t **error )
{
	libcthreads_internal_thread_pool_t *internal_thread_pool = NULL;
	static char *function                                    = "libcthreads_thread_pool_push_sorted";
	int compare_result                                       = 0;
	int pop_index                                            = 0;
	int previous_push_index                                  = 0;
	int push_index                                           = 0;
	int result                                               = 1;
	int value_index                                          = 0;

#if defined( WINAPI ) && ( WINVER >= 0x0602 )
	TP_WORK *thread_pool_work                                = NULL;
	DWORD error_code                                         = 0;
#endif

	if( thread_pool == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid thread pool.",
		 function );

		return( -1 );
	}
	internal_thread_pool = (libcthreads_internal_thread_pool_t *) thread_pool;

	if( internal_thread_pool->values_array == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid queue - missing values array.",
		 function );

		return( -1 );
	}
	if( value == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid value.",
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
	if( ( sort_flags & ~( LIBCTHREADS_SORT_FLAG_UNIQUE_VALUES ) ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported sort flags: 0x%02" PRIx8 ".",
		 function,
		 sort_flags );

		return( -1 );
	}
	if( libcthreads_mutex_grab(
	     internal_thread_pool->condition_mutex,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to grab condition mutex.",
		 function );

		return( -1 );
	}
	while( internal_thread_pool->number_of_values == internal_thread_pool->allocated_number_of_values )
	{
		if( libcthreads_condition_wait(
		     internal_thread_pool->full_condition,
		     internal_thread_pool->condition_mutex,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to wait for full condition.",
			 function );

			goto on_error;
		}
	}
	pop_index = internal_thread_pool->pop_index;

	for( value_index = 0;
	     value_index < internal_thread_pool->number_of_values;
	     value_index++ )
	{
		compare_result = value_compare_function(
				  value,
				  internal_thread_pool->values_array[ pop_index ],
				  error );

		if( compare_result == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to compare value: %d.",
			 function,
			 value_index );

			goto on_error;
		}
		else if( compare_result == LIBCTHREADS_COMPARE_EQUAL )
		{
			if( ( sort_flags & LIBCTHREADS_SORT_FLAG_UNIQUE_VALUES ) != 0 )
			{
				result = 0;

				break;
			}
		}
		else if( compare_result == LIBCTHREADS_COMPARE_LESS )
		{
			result = 1;

			break;
		}
		else if( compare_result != LIBCTHREADS_COMPARE_GREATER )
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
		pop_index++;

		if( pop_index >= internal_thread_pool->allocated_number_of_values )
		{
			pop_index = 0;
		}
	}
	if( result != 0 )
	{
		push_index = internal_thread_pool->push_index;

		if( compare_result == LIBCTHREADS_COMPARE_LESS )
		{
			previous_push_index = push_index - 1;

			while( push_index != pop_index )
			{
				if( previous_push_index < 0 )
				{
					previous_push_index = internal_thread_pool->allocated_number_of_values - 1;
				}
				internal_thread_pool->values_array[ push_index ] = internal_thread_pool->values_array[ previous_push_index ];

				push_index = previous_push_index;

				previous_push_index--;
			}
		}
		internal_thread_pool->values_array[ push_index ] = value;

		internal_thread_pool->push_index++;

		if( internal_thread_pool->push_index >= internal_thread_pool->allocated_number_of_values )
		{
			internal_thread_pool->push_index = 0;
		}
		internal_thread_pool->number_of_values++;

		/* The condition broadcast must be protected by the mutex for the WINAPI version
		 */
		if( libcthreads_condition_broadcast(
		     internal_thread_pool->empty_condition,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to broadcast empty condition.",
			 function );

			goto on_error;
		}
	}
	if( libcthreads_mutex_release(
	     internal_thread_pool->condition_mutex,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to release condition mutex.",
		 function );

		return( -1 );
	}
#if defined( WINAPI ) && ( WINVER >= 0x0602 )
	thread_pool_work = CreateThreadpoolWork(
	                    &libcthreads_thread_pool_callback_function_helper,
	                    (void *) internal_thread_pool,
	                    &( internal_thread_pool->callback_environment ) );

	if( thread_pool_work == NULL )
	{
		error_code = GetLastError();

		libcerror_system_set_error(
		 error,
		 error_code,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create thread pool work.",
		 function );

		return( -1 );
	}
	SubmitThreadpoolWork(
	 thread_pool_work );

#endif /* defined( WINAPI ) && ( WINVER >= 0x0602 ) */

	return( result );

on_error:
	libcthreads_mutex_release(
	 internal_thread_pool->condition_mutex,
	 NULL );

	return( -1 );
}

/* Joins the current with a specified thread pool
 * The the thread is freed after join
 * Returns 1 if successful or -1 on error
 */
int libcthreads_thread_pool_join(
     libcthreads_thread_pool_t **thread_pool,
     libcerror_error_t **error )
{
	libcthreads_internal_thread_pool_t *internal_thread_pool = NULL;
	static char *function                                    = "libcthreads_thread_pool_join";
	int result                                               = 1;

#if !defined( WINAPI ) || ( WINVER < 0x0602 )
	int thread_index                                         = 0;

#if defined( WINAPI )
	DWORD error_code                                         = 0;
	DWORD wait_status                                        = 0;

#elif defined( HAVE_PTHREAD_H )
	int *thread_return_value                                 = NULL;
	int pthread_result                                       = 0;
#endif
#endif /* !defined( WINAPI ) || ( WINVER < 0x0602 ) */

	if( thread_pool == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid thread pool.",
		 function );

		return( -1 );
	}
	if( *thread_pool == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: missing thread pool value.",
		 function );

		return( -1 );
	}
	internal_thread_pool = (libcthreads_internal_thread_pool_t *) *thread_pool;
	*thread_pool         = NULL;

	if( libcthreads_mutex_grab(
	     internal_thread_pool->condition_mutex,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to grab condition mutex.",
		 function );

		return( -1 );
	}
	internal_thread_pool->status = LIBCTHREADS_STATUS_EXIT;

	/* Wake up the worker threads
	 */
	if( libcthreads_condition_broadcast(
	     internal_thread_pool->empty_condition,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to broadcast empty condition.",
		 function );

		result = -1;
	}
	while( internal_thread_pool->number_of_values != 0 )
	{
		/* Wait here to all the values in the queue have been processed
		 */
		if( libcthreads_condition_wait(
		     internal_thread_pool->full_condition,
		     internal_thread_pool->condition_mutex,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to wait for full condition.",
			 function );

			result = -1;

			break;
		}
	}
	if( libcthreads_mutex_release(
	     internal_thread_pool->condition_mutex,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to release condition mutex.",
		 function );

		return( -1 );
	}
#if defined( WINAPI ) && ( WINVER >= 0x0602 )
	CloseThreadpoolCleanupGroupMembers(
	 internal_thread_pool->cleanup_group,
	 FALSE,
	 NULL );

	CloseThreadpoolCleanupGroup(
	 internal_thread_pool->cleanup_group );

	internal_thread_pool->cleanup_group = NULL;

	CloseThreadpool(
	 internal_thread_pool->thread_pool );

	internal_thread_pool->thread_pool = NULL;

#elif defined( WINAPI )
	for( thread_index = 0;
	     thread_index < internal_thread_pool->number_of_threads;
	     thread_index++ )
	{
		wait_status = WaitForSingleObject(
			       internal_thread_pool->thread_handles_array[ thread_index ],
			       INFINITE );

		if( wait_status == WAIT_FAILED )
		{
			error_code = GetLastError();

			libcerror_system_set_error(
			 error,
			 error_code,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: wait for thread: %d failed.",
			 function,
			 thread_index );

			result = -1;
		}
	}

#elif defined( HAVE_PTHREAD_H )
	for( thread_index = 0;
	     thread_index < internal_thread_pool->number_of_threads;
	     thread_index++ )
	{
		pthread_result = pthread_join(
		                  internal_thread_pool->threads_array[ thread_index ],
		                  (void **) &thread_return_value );

		if( pthread_result != 0 )
		{
			libcerror_system_set_error(
			 error,
			 result,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to join thread: %d.",
			 function,
			 thread_index );

			result = -1;
		}
		/* If the thread returns NULL it never got around to launching the callback function
		 */
		else if( ( thread_return_value != NULL )
		      && ( *thread_return_value != 1 ) )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: thread: %d returned an error status of: %d.",
			 function,
			 thread_index,
			 *thread_return_value );

			result = -1;
		}
		if( thread_return_value != NULL )
		{
			memory_free(
			 thread_return_value );

			thread_return_value = NULL;
		}
	}
#endif /* defined( WINAPI ) && ( WINVER >= 0x0602 ) */
	if( libcthreads_condition_free(
	     &( internal_thread_pool->full_condition ),
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
		 "%s: unable to free full condition.",
		 function );

		result = -1;
	}
	if( libcthreads_condition_free(
	     &( internal_thread_pool->empty_condition ),
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
		 "%s: unable to free empty condition.",
		 function );

		result = -1;
	}
	if( libcthreads_mutex_free(
	     &( internal_thread_pool->condition_mutex ),
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
		 "%s: unable to free condition mutex.",
		 function );

		result = -1;
	}
#if defined( WINAPI ) && ( WINVER < 0x0602 )
	memory_free(
	 internal_thread_pool->thread_identifiers_array );

	memory_free(
	 internal_thread_pool->thread_handles_array );

#elif defined( HAVE_PTHREAD_H )
	memory_free(
	 internal_thread_pool->threads_array );
#endif

	memory_free(
	 internal_thread_pool->values_array );
	memory_free(
	 internal_thread_pool );

	return( result );
}

#endif /* !defined( HAVE_LOCAL_LIBCTHREADS ) || defined( HAVE_MULTI_THREAD_SUPPORT ) */

