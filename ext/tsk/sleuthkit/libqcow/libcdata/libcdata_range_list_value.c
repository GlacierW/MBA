/*
 * Range list value
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

#include "libcdata_libcerror.h"
#include "libcdata_range_list_value.h"

/* Creates a range list value
 * Make sure the value range_list_value is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libcdata_range_list_value_initialize(
     libcdata_range_list_value_t **range_list_value,
     libcerror_error_t **error )
{
	static char *function = "libcdata_range_list_value_initialize";

	if( range_list_value == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid range list value.",
		 function );

		return( -1 );
	}
	if( *range_list_value != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid range list value value already set.",
		 function );

		return( -1 );
	}
	*range_list_value = memory_allocate_structure(
	                     libcdata_range_list_value_t );

	if( *range_list_value == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create range list value.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *range_list_value,
	     0,
	     sizeof( libcdata_range_list_value_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear range list value.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( *range_list_value != NULL )
	{
		memory_free(
		 *range_list_value );

		*range_list_value = NULL;
	}
	return( -1 );
}

/* Frees a range list value
 * Uses the value_free_function to free the element value
 * Returns 1 if successful or -1 on error
 */
int libcdata_range_list_value_free(
     libcdata_range_list_value_t **range_list_value,
     int (*value_free_function)(
            intptr_t **value,
            libcerror_error_t **error ),
     libcerror_error_t **error )
{
	static char *function = "libcdata_range_list_value_free";
	int result            = 1;

	if( range_list_value == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid range list value.",
		 function );

		return( -1 );
	}
	if( *range_list_value != NULL )
	{
		if( value_free_function != NULL )
		{
			if( value_free_function(
			     &( ( *range_list_value )->value ),
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: unable to free value.",
				 function );

				result = -1;
			}
		}
		memory_free(
		 *range_list_value );

		*range_list_value = NULL;
	}
	return( result );
}

/* Clones the range list value
 *
 * The values are cloned using the value_clone_function
 * On error the values are freed using the value_free_function
 *
 * Returns 1 if successful or -1 on error
 */
int libcdata_range_list_value_clone(
     libcdata_range_list_value_t **destination_range_list_value,
     libcdata_range_list_value_t *source_range_list_value,
     int (*value_free_function)(
            intptr_t **value,
            libcerror_error_t **error ),
     int (*value_clone_function)(
            intptr_t **destination_value,
            intptr_t *source_value,
            libcerror_error_t **error ),
     libcerror_error_t **error )
{
	static char *function = "libcdata_range_list_value_clone";

	if( destination_range_list_value == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid destination range list value.",
		 function );

		return( -1 );
	}
	if( *destination_range_list_value != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid destination range list value value already set.",
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
	if( source_range_list_value == NULL )
	{
		*destination_range_list_value = NULL;

		return( 1 );
	}
	*destination_range_list_value = memory_allocate_structure(
	                                 libcdata_range_list_value_t );

	if( *destination_range_list_value == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create destination range list value.",
		 function );

		goto on_error;
	}
	if( memory_copy(
	     *destination_range_list_value,
	     source_range_list_value,
	     sizeof( libcdata_range_list_value_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
		 "%s: unable to copy source to destination range list value.",
		 function );

		memory_free(
		 *destination_range_list_value );

		*destination_range_list_value = NULL;

		return( -1 );
	}
	( *destination_range_list_value )->value = NULL;

	if( value_clone_function(
	     &( ( *destination_range_list_value )->value ),
	     source_range_list_value->value,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create destination value.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( *destination_range_list_value != NULL )
	{
		if( ( *destination_range_list_value )->value != NULL )
		{
			value_free_function(
			 &( ( *destination_range_list_value )->value ),
			 NULL );
		}
		memory_free(
		 *destination_range_list_value );

		*destination_range_list_value = NULL;
	}
	return( -1 );
}

/* Merges the range list values
 *
 * The values are merged using the value_merge_function.
 * If the source value is NULL the merge function is not called.
 *
 * Returns 1 if successful or -1 on error
 */
int libcdata_range_list_value_merge(
     libcdata_range_list_value_t *destination_range_list_value,
     libcdata_range_list_value_t *source_range_list_value,
     int (*value_merge_function)(
            intptr_t *destination_value,
            intptr_t *source_value,
            libcerror_error_t **error ),
     libcerror_error_t **error )
{
	static char *function = "libcdata_range_list_value_merge";

	if( destination_range_list_value == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid destination range list value.",
		 function );

		return( -1 );
	}
	if( source_range_list_value == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid source range list value.",
		 function );

		return( -1 );
	}
	if( source_range_list_value->value != NULL )
	{
		if( destination_range_list_value->value == NULL )
		{
			destination_range_list_value->value = source_range_list_value->value;
		}
		else
		{
			if( value_merge_function == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
				 "%s: invalid value merge function.",
				 function );

				return( -1 );
			}
			if( value_merge_function(
			     destination_range_list_value->value,
			     source_range_list_value->value,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
				 "%s: unable to merge source with destination value.",
				 function );

				return( -1 );
			}
		}
	}
	if( destination_range_list_value->start > source_range_list_value->start )
	{
		/* Merge a preceding range
		 */
		destination_range_list_value->size += destination_range_list_value->start - source_range_list_value->start;
		destination_range_list_value->start = source_range_list_value->start;
	}
	if( destination_range_list_value->end < source_range_list_value->end )
	{
		/* Merge a successive range
		 */
		destination_range_list_value->size += source_range_list_value->end - destination_range_list_value->end;
		destination_range_list_value->end   = source_range_list_value->end;
	}
	return( 1 );
}

