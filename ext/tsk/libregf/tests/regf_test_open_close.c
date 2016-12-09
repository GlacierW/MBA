/*
 * Library open close testing program
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
#include <file_stream.h>
#include <system_string.h>
#include <types.h>

#if defined( HAVE_STDLIB_H ) || defined( WINAPI )
#include <stdlib.h>
#endif

#include "regf_test_libcerror.h"
#include "regf_test_libcsystem.h"
#include "regf_test_libregf.h"

/* Tests single open and close of a file
 * Returns 1 if successful, 0 if not or -1 on error
 */
int regf_test_single_open_close_file(
     system_character_t *filename,
     int access_flags,
     int expected_result )
{
	libcerror_error_t *error = NULL;
	libregf_file_t *file      = NULL;
	static char *function    = "regf_test_single_open_close_file";
	char *access_string      = NULL;
	int result               = 0;

	if( access_flags == LIBREGF_OPEN_READ )
	{
		access_string = "read";
	}
	else if( access_flags == LIBREGF_OPEN_WRITE )
	{
		access_string = "write";
	}
	else
	{
		access_string = "UNKNOWN";
	}
	fprintf(
	 stdout,
	 "Testing single open close of: " );

	if( filename != NULL )
	{
		fprintf(
		 stdout,
		 "%" PRIs_SYSTEM "",
		 filename );
	}
	else
	{
		fprintf(
		 stdout,
		 "NULL" );
	}
	fprintf(
	 stdout,
	 " with access: %s\t",
	 access_string );

	if( libregf_file_initialize(
	     &file,
	     &error ) != 1 )
	{
		libcerror_error_set(
		 &error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create file.",
		 function );

		goto on_error;
	}
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
	result = libregf_file_open_wide(
	          file,
	          filename,
	          access_flags,
	          &error );
#else
	result = libregf_file_open(
	          file,
	          filename,
	          access_flags,
	          &error );
#endif
	if( result == 1 )
	{
		if( libregf_file_close(
		     file,
		     &error ) != 0 )
		{
			libcerror_error_set(
			 &error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_CLOSE_FAILED,
			 "%s: unable to close file.",
			 function );

			goto on_error;
		}
	}
	if( libregf_file_free(
	     &file,
	     &error ) != 1 )
	{
		libcerror_error_set(
		 &error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
		 "%s: unable to free file.",
		 function );

		goto on_error;
	}
	result = ( expected_result == result );

	if( result == 1 )
	{
		fprintf(
		 stdout,
		 "(PASS)" );
	}
	else
	{
		fprintf(
		 stdout,
		 "(FAIL)" );
	}
	fprintf(
	 stdout,
	 "\n" );

	if( error != NULL )
	{
		libcerror_error_backtrace_fprint(
		 error,
		 stderr );
		libcerror_error_free(
		 &error );
	}
	return( result );

on_error:
	if( error != NULL )
	{
		libcerror_error_backtrace_fprint(
		 error,
		 stderr );
		libcerror_error_free(
		 &error );
	}
	if( file != NULL )
	{
		libregf_file_free(
		 &file,
		 NULL);
	}
	return( -1 );
}

/* Tests multiple open and close of a file
 * Returns 1 if successful, 0 if not or -1 on error
 */
int regf_test_multi_open_close_file(
     system_character_t *filename,
     int access_flags,
     int expected_result )
{
	libcerror_error_t *error = NULL;
	libregf_file_t *file      = NULL;
	static char *function    = "regf_test_multi_open_close_file";
	char *access_string      = NULL;
	int result               = 0;

	if( access_flags == LIBREGF_OPEN_READ )
	{
		access_string = "read";
	}
	else if( access_flags == LIBREGF_OPEN_WRITE )
	{
		access_string = "write";
	}
	else
	{
		access_string = "UNKNOWN";
	}
	fprintf(
	 stdout,
	 "Testing multi open close of: " );

	if( filename != NULL )
	{
		fprintf(
		 stdout,
		 "%" PRIs_SYSTEM "",
		 filename );
	}
	else
	{
		fprintf(
		 stdout,
		 "NULL" );
	}
	fprintf(
	 stdout,
	 " with access: %s\t",
	 access_string );

	if( libregf_file_initialize(
	     &file,
	     &error ) != 1 )
	{
		libcerror_error_set(
		 &error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create file.",
		 function );

		goto on_error;
	}
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
	result = libregf_file_open_wide(
	          file,
	          filename,
	          access_flags,
	          &error );
#else
	result = libregf_file_open(
	          file,
	          filename,
	          access_flags,
	          &error );
#endif
	if( result == 1 )
	{
		if( libregf_file_close(
		     file,
		     &error ) != 0 )
		{
			libcerror_error_set(
			 &error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_CLOSE_FAILED,
			 "%s: unable to close file.",
			 function );

			goto on_error;
		}
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
		result = libregf_file_open_wide(
		          file,
		          filename,
		          access_flags,
		          &error );
#else
		result = libregf_file_open(
		          file,
		          filename,
		          access_flags,
		          &error );
#endif
		if( result == 1 )
		{
			if( libregf_file_close(
			     file,
			     &error ) != 0 )
			{
				libcerror_error_set(
				 &error,
				 LIBCERROR_ERROR_DOMAIN_IO,
				 LIBCERROR_IO_ERROR_CLOSE_FAILED,
				 "%s: unable to close file.",
				 function );

				goto on_error;
			}
		}
	}
	if( libregf_file_free(
	     &file,
	     &error ) != 1 )
	{
		libcerror_error_set(
		 &error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
		 "%s: unable to free file.",
		 function );

		goto on_error;
	}
	result = ( expected_result == result );

	if( result == 1 )
	{
		fprintf(
		 stdout,
		 "(PASS)" );
	}
	else
	{
		fprintf(
		 stdout,
		 "(FAIL)" );
	}
	fprintf(
	 stdout,
	 "\n" );

	if( error != NULL )
	{
		libcerror_error_backtrace_fprint(
		 error,
		 stderr );
		libcerror_error_free(
		 &error );
	}
	return( result );

on_error:
	if( error != NULL )
	{
		libcerror_error_backtrace_fprint(
		 error,
		 stderr );
		libcerror_error_free(
		 &error );
	}
	if( file != NULL )
	{
		libregf_file_free(
		 &file,
		 NULL);
	}
	return( -1 );
}

/* The main program
 */
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
int wmain( int argc, wchar_t * const argv[] )
#else
int main( int argc, char * const argv[] )
#endif
{
	system_character_t *source = NULL;
	system_integer_t option    = 0;

	while( ( option = libcsystem_getopt(
	                   argc,
	                   argv,
	                   _SYSTEM_STRING( "" ) ) ) != (system_integer_t) -1 )
	{
		switch( option )
		{
			case (system_integer_t) '?':
			default:
				fprintf(
				 stderr,
				 "Invalid argument: %" PRIs_SYSTEM ".\n",
				 argv[ optind - 1 ] );

				return( EXIT_FAILURE );
		}
	}
	if( optind == argc )
	{
		fprintf(
		 stderr,
		 "Missing source file or device.\n" );

		return( EXIT_FAILURE );
	}
	source = argv[ optind ];

#if defined( HAVE_DEBUG_OUTPUT ) && defined( REGF_TEST_OPEN_CLOSE_VERBOSE )
	libregf_notify_set_verbose(
	 1 );
	libregf_notify_set_stream(
	 stderr,
	 NULL );
#endif

	/* Case 0: single open and close of a file using filename
	 */
	if( regf_test_single_open_close_file(
	     source,
	     LIBREGF_OPEN_READ,
	     1 ) != 1 )
	{
		fprintf(
		 stderr,
		 "Unable to test single open close.\n" );

		return( EXIT_FAILURE );
	}
	if( regf_test_single_open_close_file(
	     NULL,
	     LIBREGF_OPEN_READ,
	     -1 ) != 1 )
	{
		fprintf(
		 stderr,
		 "Unable to test single open close.\n" );

		return( EXIT_FAILURE );
	}
	if( regf_test_single_open_close_file(
	     source,
	     LIBREGF_OPEN_WRITE,
	     -1 ) != 1 )
	{
		fprintf(
		 stderr,
		 "Unable to test single open close.\n" );

		return( EXIT_FAILURE );
	}
	/* Case 1: multiple open and close of a file using filename
	 */
	if( regf_test_multi_open_close_file(
	     source,
	     LIBREGF_OPEN_READ,
	     1 ) != 1 )
	{
		fprintf(
		 stderr,
		 "Unable to test multi open close.\n" );

		return( EXIT_FAILURE );
	}
	return( EXIT_SUCCESS );
}

