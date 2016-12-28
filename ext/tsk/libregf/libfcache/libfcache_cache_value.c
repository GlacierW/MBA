/*
 * Cache value functions
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

#include "libfcache_cache_value.h"
#include "libfcache_date_time.h"
#include "libfcache_definitions.h"
#include "libfcache_libcerror.h"
#include "libfcache_types.h"

/* Creates a cache value
 * Make sure the value cache_value is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libfcache_cache_value_initialize(
     libfcache_cache_value_t **cache_value,
     libcerror_error_t **error )
{
	libfcache_internal_cache_value_t *internal_cache_value = NULL;
	static char *function                                  = "libfcache_cache_value_initialize";

	if( cache_value == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid cache value.",
		 function );

		return( -1 );
	}
	if( *cache_value != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid cache value value already set.",
		 function );

		return( -1 );
	}
	internal_cache_value = memory_allocate_structure(
	                        libfcache_internal_cache_value_t );

	if( internal_cache_value == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create cache value.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     internal_cache_value,
	     0,
	     sizeof( libfcache_internal_cache_value_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear cache value.",
		 function );

		goto on_error;
	}
	internal_cache_value->file_index = -1;
	internal_cache_value->offset     = (off64_t) -1;

	*cache_value = (libfcache_cache_value_t *) internal_cache_value;

	return( 1 );

on_error:
	if( internal_cache_value != NULL )
	{
		memory_free(
		 internal_cache_value );
	}
	return( -1 );
}

/* Frees a cache value
 * Returns 1 if successful or -1 on error
 */
int libfcache_cache_value_free(
     libfcache_cache_value_t **cache_value,
     libcerror_error_t **error )
{
	libfcache_internal_cache_value_t *internal_cache_value = NULL;
	static char *function                                  = "libfcache_cache_value_free";
	int result                                             = 1;

	if( cache_value == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid cache value.",
		 function );

		return( -1 );
	}
	if( *cache_value != NULL )
	{
		internal_cache_value = (libfcache_internal_cache_value_t *) *cache_value;
		*cache_value         = NULL;

		if( internal_cache_value->value != NULL )
		{
			if( ( internal_cache_value->flags & LIBFCACHE_CACHE_VALUE_FLAG_MANAGED ) != 0 )
			{
				if( internal_cache_value->free_value == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: invalid cache value - missing free value function.",
					 function );

					result = -1;
				}
				else if( internal_cache_value->free_value(
					  &( internal_cache_value->value ),
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
		}
		memory_free(
		 internal_cache_value );
	}
	return( result );
}

/* Clears the cache value
 * This function does not free the value
 * Returns 1 if successful or -1 on error
 */
int libfcache_cache_value_clear(
     libfcache_cache_value_t *cache_value,
     libcerror_error_t **error )
{
	libfcache_internal_cache_value_t *internal_cache_value = NULL;
	static char *function                                  = "libfcache_cache_value_free";

	if( cache_value == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid cache value.",
		 function );

		return( -1 );
	}
	internal_cache_value = (libfcache_internal_cache_value_t *) cache_value;

	if( memory_set(
	     internal_cache_value,
	     0,
	     sizeof( libfcache_internal_cache_value_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear cache value.",
		 function );

		return( -1 );
	}
	internal_cache_value->file_index = -1;
	internal_cache_value->offset     = (off64_t) -1;

	return( 1 );
}

/* Retrieves the cache value identifier
 * Returns 1 if successful or -1 on error
 */
int libfcache_cache_value_get_identifier(
     libfcache_cache_value_t *cache_value,
     int *file_index,
     off64_t *offset,
     time_t *timestamp,
     libcerror_error_t **error )
{
	libfcache_internal_cache_value_t *internal_cache_value = NULL;
	static char *function                                  = "libfcache_cache_value_get_identifier";

	if( cache_value == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid cache value.",
		 function );

		return( -1 );
	}
	internal_cache_value = (libfcache_internal_cache_value_t *) cache_value;

	if( file_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid file index.",
		 function );

		return( -1 );
	}
	if( offset == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid offset.",
		 function );

		return( -1 );
	}
	if( timestamp == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid timestamp.",
		 function );

		return( -1 );
	}
	*file_index = internal_cache_value->file_index;
	*offset     = internal_cache_value->offset;
	*timestamp  = internal_cache_value->timestamp;

	return( 1 );
}

/* Sets the cache value identifier
 * Returns 1 if successful or -1 on error
 */
int libfcache_cache_value_set_identifier(
     libfcache_cache_value_t *cache_value,
     int file_index,
     off64_t offset,
     time_t timestamp,
     libcerror_error_t **error )
{
	libfcache_internal_cache_value_t *internal_cache_value = NULL;
	static char *function                                  = "libfcache_cache_value_set_identifier";

	if( cache_value == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid cache value.",
		 function );

		return( -1 );
	}
	internal_cache_value = (libfcache_internal_cache_value_t *) cache_value;

	internal_cache_value->file_index = file_index;
	internal_cache_value->offset     = offset;
	internal_cache_value->timestamp  = timestamp;

	return( 1 );
}

/* Retrieves the cache value
 * Returns 1 if successful or -1 on error
 */
int libfcache_cache_value_get_value(
     libfcache_cache_value_t *cache_value,
     intptr_t **value,
     libcerror_error_t **error )
{
	libfcache_internal_cache_value_t *internal_cache_value = NULL;
	static char *function                                  = "libfcache_cache_value_get_value";

	if( cache_value == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid cache value.",
		 function );

		return( -1 );
	}
	internal_cache_value = (libfcache_internal_cache_value_t *) cache_value;

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
	*value = internal_cache_value->value;

	return( 1 );
}

/* Sets the cache value
 * Returns 1 if successful or -1 on error
 */
int libfcache_cache_value_set_value(
     libfcache_cache_value_t *cache_value,
     intptr_t *value,
     int (*free_value)(
            intptr_t **value,
            libcerror_error_t **error ),
     uint8_t flags,
     libcerror_error_t **error )
{
	libfcache_internal_cache_value_t *internal_cache_value = NULL;
	static char *function                                  = "libfcache_cache_value_set_value";

	if( cache_value == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid cache value.",
		 function );

		return( -1 );
	}
	internal_cache_value = (libfcache_internal_cache_value_t *) cache_value;

	if( free_value == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid free value function.",
		 function );

		return( -1 );
	}
	if( ( internal_cache_value->flags & LIBFCACHE_CACHE_VALUE_FLAG_MANAGED ) != 0 )
	{
		if( internal_cache_value->value != NULL )
		{
			if( internal_cache_value->free_value == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
				 "%s: invalid cache value - missing free value function.",
				 function );

				return( -1 );
			}
			if( internal_cache_value->free_value(
			     &( internal_cache_value->value ),
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: unable to free value.",
				 function );

				return( -1 );
			}
		}
		internal_cache_value->flags &= ~( LIBFCACHE_CACHE_VALUE_FLAG_MANAGED );
	}
	internal_cache_value->value      = value;
	internal_cache_value->free_value = free_value;
	internal_cache_value->flags     |= flags;

	return( 1 );
}

