/*
 * File pool functions
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
#include <narrow_string.h>
#include <types.h>
#include <wide_string.h>

#include "libbfio_definitions.h"
#include "libbfio_file.h"
#include "libbfio_file_pool.h"
#include "libbfio_handle.h"
#include "libbfio_libcerror.h"
#include "libbfio_pool.h"
#include "libbfio_types.h"

/* Appends file handles to the pool for the corresponding filenames
 * Returns 1 if successful or -1 on error
 */
int libbfio_file_pool_append_handles_for_names(
     libbfio_pool_t *pool,
     char * const names[],
     int number_of_names,
     int access_flags,
     libcerror_error_t **error )
{
	libbfio_handle_t *file_io_handle = NULL;
	static char *function            = "libbfio_file_pool_append_handles_for_names";
	size_t name_length               = 0;
	int pool_entry                   = 0;
	int name_iterator                = 0;

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
	if( names == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid names.",
		 function );

		return( -1 );
	}
	if( number_of_names <= 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_ZERO_OR_LESS,
		 "%s: invalid number of names zero or less.",
		 function );

		return( -1 );
	}
	for( name_iterator = 0;
	     name_iterator < number_of_names;
	     name_iterator++ )
	{
		name_length = narrow_string_length(
		               names[ name_iterator ] );

		if( libbfio_file_initialize(
		     &file_io_handle,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create file IO handle: %d.",
			 function,
			 name_iterator );

			return( -1 );
		}
		if( libbfio_file_set_name(
		     file_io_handle,
		     names[ name_iterator ],
		     name_length + 1,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set name in file IO handle: %d.",
			 function,
			 name_iterator );

			libbfio_handle_free(
			 &file_io_handle,
			 NULL );

			return( -1 );
		}
		if( libbfio_pool_append_handle(
		     pool,
		     &pool_entry,
		     file_io_handle,
		     access_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
			 "%s: unable to append file IO handle: %d to pool.",
			 function,
			 name_iterator );

			libbfio_handle_free(
			 &file_io_handle,
			 NULL );

			return( -1 );
		}
		file_io_handle = NULL;
	}
	return( 1 );
}

#if defined( HAVE_WIDE_CHARACTER_TYPE )

/* Appends file handles to the pool for the corresponding filenames
 * Returns 1 if successful or -1 on error
 */
int libbfio_file_pool_append_handles_for_names_wide(
     libbfio_pool_t *pool,
     wchar_t * const names[],
     int number_of_names,
     int access_flags,
     libcerror_error_t **error )
{
	libbfio_handle_t *file_io_handle = NULL;
	static char *function            = "libbfio_file_pool_append_handles_for_names_wide";
	size_t name_length               = 0;
	int pool_entry                   = 0;
	int name_iterator                = 0;

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
	if( names == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid names.",
		 function );

		return( -1 );
	}
	if( number_of_names <= 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_ZERO_OR_LESS,
		 "%s: invalid number of names zero or less.",
		 function );

		return( -1 );
	}
	for( name_iterator = 0;
	     name_iterator < number_of_names;
	     name_iterator++ )
	{
		name_length = wide_string_length(
		               names[ name_iterator ] );

		if( libbfio_file_initialize(
		     &file_io_handle,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create file IO handle: %d.",
			 function,
			 name_iterator );

			return( -1 );
		}
		if( libbfio_file_set_name_wide(
		     file_io_handle,
		     names[ name_iterator ],
		     name_length + 1,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set name in file IO handle: %d.",
			 function,
			 name_iterator );

			libbfio_handle_free(
			 &file_io_handle,
			 NULL );

			return( -1 );
		}
		if( libbfio_pool_append_handle(
		     pool,
		     &pool_entry,
		     file_io_handle,
		     access_flags,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
			 "%s: unable to append file IO handle: %d to pool.",
			 function,
			 name_iterator );

			libbfio_handle_free(
			 &file_io_handle,
			 NULL );

			return( -1 );
		}
		file_io_handle = NULL;
	}
	return( 1 );
}

#endif /* defined( HAVE_WIDE_CHARACTER_TYPE ) */

