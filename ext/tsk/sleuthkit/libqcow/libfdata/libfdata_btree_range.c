/*
 * The balanced tree range functions
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

#include "libfdata_btree_range.h"
#include "libfdata_definitions.h"
#include "libfdata_libcerror.h"
#include "libfdata_range.h"
#include "libfdata_types.h"

/* Creates a tree range
 * Make sure the value range is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libfdata_btree_range_initialize(
     libfdata_btree_range_t **range,
     libcerror_error_t **error )
{
	static char *function = "libfdata_btree_range_initialize";

	if( range == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid tree range.",
		 function );

		return( -1 );
	}
	if( *range != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid tree range value already set.",
		 function );

		return( -1 );
	}
	*range = memory_allocate_structure(
	          libfdata_btree_range_t );

	if( *range == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create tree range.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *range,
	     0,
	     sizeof( libfdata_btree_range_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear tree range.",
		 function );

		goto on_error;
	}
	( *range )->file_index = -1;
	( *range )->offset     = (off64_t) -1;

	return( 1 );

on_error:
	if( *range != NULL )
	{
		memory_free(
		 *range );

		*range = NULL;
	}
	return( -1 );
}

/* Frees a tree range
 * Returns 1 if successful or -1 on error
 */
int libfdata_btree_range_free(
     libfdata_btree_range_t **range,
     libcerror_error_t **error )
{
	static char *function = "libfdata_btree_range_free";
	int result            = 1;

	if( range == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid tree range.",
		 function );

		return( -1 );
	}
	if( *range != NULL )
	{
		if( ( *range )->key_value != NULL )
		{
			if( ( ( *range )->key_value_flags & LIBFDATA_KEY_VALUE_FLAG_MANAGED ) != 0 )
			{
				if( ( *range )->free_key_value == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: invalid tree range - missing free key value function.",
					 function );

					result = -1;
				}
				else if( ( *range )->free_key_value(
					  &( ( *range )->key_value ),
					  error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
					 "%s: unable to free key value.",
					 function );

					result = -1;
				}
			}
		}
		memory_free(
		 *range );

		*range = NULL;
	}
	return( result );
}

/* Clones (duplicates) the range
 * Returns 1 if successful or -1 on error
 */
int libfdata_btree_range_clone(
     libfdata_btree_range_t **destination_range,
     libfdata_btree_range_t *source_range,
     libcerror_error_t **error )
{
	static char *function = "libfdata_btree_range_clone";

	if( destination_range == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid destination range.",
		 function );

		return( -1 );
	}
	if( *destination_range != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid destination range value already set.",
		 function );

		return( -1 );
	}
	if( source_range == NULL )
	{
		*destination_range = NULL;

		return( 1 );
	}
	*destination_range = memory_allocate_structure(
	                      libfdata_btree_range_t );

	if( *destination_range == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create destination range.",
		 function );

		goto on_error;
	}
	if( memory_copy(
	     *destination_range,
	     source_range,
	     sizeof( libfdata_btree_range_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
		 "%s: unable to copy source to destination range.",
		 function );

		goto on_error;
	}
	( *destination_range )->key_value = NULL;

	return( 1 );

on_error:
	if( *destination_range != NULL )
	{
		memory_free(
		 *destination_range );

		*destination_range = NULL;
	}
	return( -1 );
}

/* Retrieves the tree range values
 * Returns 1 if successful or -1 on error
 */
int libfdata_btree_range_get(
     libfdata_btree_range_t *range,
     int *data_file_index,
     off64_t *data_offset,
     size64_t *data_size,
     uint32_t *data_flags,
     intptr_t **key_value,
     libcerror_error_t **error )
{
	static char *function = "libfdata_btree_range_get";

	if( range == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid tree range.",
		 function );

		return( -1 );
	}
	if( data_file_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid data file index.",
		 function );

		return( -1 );
	}
	if( data_offset == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid data offset.",
		 function );

		return( -1 );
	}
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
	if( data_flags == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid data flags.",
		 function );

		return( -1 );
	}
	if( key_value == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key value.",
		 function );

		return( -1 );
	}
	*data_file_index = range->file_index;
	*data_offset     = range->offset;
	*data_size       = range->size;
	*data_flags      = range->flags;
	*key_value       = range->key_value;

	return( 1 );
}

/* Sets the tree range values
 * Returns 1 if successful or -1 on error
 */
int libfdata_btree_range_set(
     libfdata_btree_range_t *range,
     int data_file_index,
     off64_t data_offset,
     size64_t data_size,
     uint32_t data_flags,
     intptr_t *key_value,
     int (*free_key_value)(
            intptr_t **key_value,
            libcerror_error_t **error ),
     uint8_t key_value_flags,
     libcerror_error_t **error )
{
	static char *function = "libfdata_range_set";

	if( range == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid tree range.",
		 function );

		return( -1 );
	}
	if( free_key_value != NULL )
	{
		if( ( range->key_value_flags & LIBFDATA_KEY_VALUE_FLAG_MANAGED ) != 0 )
		{
			if( range->key_value != NULL )
			{
				if( range->free_key_value == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: invalid node - missing free key value function.",
					 function );

					return( -1 );
				}
				if( range->free_key_value(
				     &( range->key_value ),
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
					 "%s: unable to free key value.",
					 function );

					return( -1 );
				}
			}
			range->key_value_flags &= ~( LIBFDATA_KEY_VALUE_FLAG_MANAGED );
		}
	}
	range->file_index       = data_file_index;
	range->offset           = data_offset;
	range->size             = data_size;
	range->flags            = data_flags;
	range->key_value        = key_value;
	range->free_key_value   = free_key_value;
	range->key_value_flags |= key_value_flags;

	return( 1 );
}

