/*
 * The (data) mapped range functions
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

#include "libfdata_libcerror.h"
#include "libfdata_mapped_range.h"

/* Creates a mapped range
 * Make sure the value mapped_range is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libfdata_mapped_range_initialize(
     libfdata_mapped_range_t **mapped_range,
     libcerror_error_t **error )
{
	static char *function = "libfdata_mapped_range_initialize";

	if( mapped_range == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid mapped range.",
		 function );

		return( -1 );
	}
	if( *mapped_range != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid mapped range value already set.",
		 function );

		return( -1 );
	}
	*mapped_range = memory_allocate_structure(
	                 libfdata_mapped_range_t );

	if( *mapped_range == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create mapped range.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *mapped_range,
	     0,
	     sizeof( libfdata_mapped_range_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear mapped range.",
		 function );

		goto on_error;
	}
	( *mapped_range )->offset = (off64_t) -1;

	return( 1 );

on_error:
	if( *mapped_range != NULL )
	{
		memory_free(
		 *mapped_range );

		*mapped_range = NULL;
	}
	return( -1 );
}

/* Frees a mapped range
 * Returns 1 if successful or -1 on error
 */
int libfdata_mapped_range_free(
     libfdata_mapped_range_t **mapped_range,
     libcerror_error_t **error )
{
	static char *function = "libfdata_mapped_range_free";

	if( mapped_range == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid mapped range.",
		 function );

		return( -1 );
	}
	if( *mapped_range != NULL )
	{
		memory_free(
		 *mapped_range );

		*mapped_range = NULL;
	}
	return( 1 );
}

/* Clones (duplicates) the mapped range
 * Returns 1 if successful or -1 on error
 */
int libfdata_mapped_range_clone(
     libfdata_mapped_range_t **destination_mapped_range,
     libfdata_mapped_range_t *source_mapped_range,
     libcerror_error_t **error )
{
	static char *function = "libfdata_mapped_range_clone";

	if( destination_mapped_range == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid destination mapped range.",
		 function );

		return( -1 );
	}
	if( *destination_mapped_range != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid destination mapped range value already set.",
		 function );

		return( -1 );
	}
	if( source_mapped_range == NULL )
	{
		*destination_mapped_range = NULL;

		return( 1 );
	}
	*destination_mapped_range = memory_allocate_structure(
	                             libfdata_mapped_range_t );

	if( *destination_mapped_range == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create destination mapped range.",
		 function );

		goto on_error;
	}
	if( memory_copy(
	     *destination_mapped_range,
	     source_mapped_range,
	     sizeof( libfdata_mapped_range_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
		 "%s: unable to copy source to destination mapped range.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( *destination_mapped_range != NULL )
	{
		memory_free(
		 *destination_mapped_range );

		*destination_mapped_range = NULL;
	}
	return( -1 );
}

/* Retrieves the mapped range values
 * Returns 1 if successful or -1 on error
 */
int libfdata_mapped_range_get(
     libfdata_mapped_range_t *mapped_range,
     off64_t *offset,
     size64_t *size,
     libcerror_error_t **error )
{
	static char *function = "libfdata_mapped_range_get";

	if( mapped_range == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid mapped range.",
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
	*offset = mapped_range->offset;
	*size   = mapped_range->size;

	return( 1 );
}

/* Sets the mapped range values
 * Returns 1 if successful or -1 on error
 */
int libfdata_mapped_range_set(
     libfdata_mapped_range_t *mapped_range,
     off64_t offset,
     size64_t size,
     libcerror_error_t **error )
{
	static char *function = "libfdata_mapped_range_set";

	if( mapped_range == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid mapped range.",
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
	mapped_range->offset = offset;
	mapped_range->size   = size;

	return( 1 );
}

