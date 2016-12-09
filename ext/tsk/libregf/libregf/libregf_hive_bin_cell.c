/*
 * Hive bin cell functions
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
#include <byte_stream.h>
#include <memory.h>
#include <types.h>

#include "libregf_hive_bin_cell.h"
#include "libregf_libcerror.h"

/* Creates a hive bin cell
 * Make sure the value hive_bin_cell is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libregf_hive_bin_cell_initialize(
     libregf_hive_bin_cell_t **hive_bin_cell,
     libcerror_error_t **error )
{
	static char *function = "libregf_hive_bin_cell_initialize";

	if( hive_bin_cell == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid hive bin cell.",
		 function );

		return( -1 );
	}
	if( *hive_bin_cell != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid hive bin cell value already set.",
		 function );

		return( -1 );
	}
	*hive_bin_cell = memory_allocate_structure(
	                  libregf_hive_bin_cell_t );

	if( *hive_bin_cell == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create hive bin cell.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *hive_bin_cell,
	     0,
	     sizeof( libregf_hive_bin_cell_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear hive bin cell.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( *hive_bin_cell != NULL )
	{
		memory_free(
		 *hive_bin_cell );

		*hive_bin_cell = NULL;
	}
	return( -1 );
}

/* Frees a hive bin cell
 * Returns 1 if successful or -1 on error
 */
int libregf_hive_bin_cell_free(
     libregf_hive_bin_cell_t **hive_bin_cell,
     libcerror_error_t **error )
{
	static char *function = "libregf_hive_bin_cell_free";

	if( hive_bin_cell == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid hive bin cell.",
		 function );

		return( -1 );
	}
	if( *hive_bin_cell != NULL )
	{
		/* The hive bin cell data is referenced
		 * and maintained by the hive bin
		 */
		memory_free(
		 *hive_bin_cell );

		*hive_bin_cell = NULL;
	}
	return( 1 );
}

