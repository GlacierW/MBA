/*
 * Notification stream functions
 *
 * Copyright (C) 2008-2016, Joachim Metz <joachim.metz@gmail.com>
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
#include <types.h>

#if defined( HAVE_ERRNO_H ) || defined( WINAPI )
#include <errno.h>
#endif

#include "libcnotify_libcerror.h"
#include "libcnotify_stream.h"

/* The notification stream
 */
FILE *libcnotify_stream = NULL;

/* Value to indicate if the notification stream
 * was opened by the library
 */
int libcnotify_stream_opened_in_library = 0;

/* Set the stream
 * Returns 1 if successful or -1 on error
 */
int libcnotify_stream_set(
     FILE *stream,
     libcerror_error_t **error )
{
	static char *function = "libcnotify_stream_set";

	if( libcnotify_stream_opened_in_library != 0 )
	{
		if( libcnotify_stream_close(
		     error ) != 0 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_OPEN_FAILED,
			 "%s: unable to close notify stream.",
			 function );

			return( -1 );
		}
	}
	libcnotify_stream = stream;

	return( 1 );
}

/* Opens the notification stream using a filename
 * The stream is opened in append mode
 * Returns 1 if successful or -1 on error
 */
int libcnotify_stream_open(
     const char *filename,
     libcerror_error_t **error )
{
	static char *function = "libcnotify_stream_open";

	if( filename == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid filename.",
		 function );

		return( -1 );
	}
	if( libcnotify_stream_opened_in_library != 0 )
	{
		if( libcnotify_stream_close(
		     error ) != 0 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_OPEN_FAILED,
			 "%s: unable to close notify stream.",
			 function );

			return( -1 );
		}
	}
	libcnotify_stream = file_stream_open(
	                    filename,
	                    FILE_STREAM_OPEN_APPEND );
	
	if( libcnotify_stream == NULL )
	{
		switch( errno )
		{
			case EACCES:
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_IO,
				 LIBCERROR_IO_ERROR_ACCESS_DENIED,
				 "%s: access denied to file: %s.",
				 function,
				 filename );

				break;

			case ENOENT:
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_IO,
				 LIBCERROR_IO_ERROR_INVALID_RESOURCE,
				 "%s: no such file: %s.",
				 function,
				 filename );

				break;

			default:
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_IO,
				 LIBCERROR_IO_ERROR_OPEN_FAILED,
				 "%s: unable to open file: %s.",
				 function,
				 filename );

				break;
		}
		return( -1 );
	}
	libcnotify_stream_opened_in_library = 1;

	return( 1 );
}

/* Closes the notification stream if opened using a filename
 * Returns 0 if successful or -1 on error
 */
int libcnotify_stream_close(
     libcerror_error_t **error )
{
	static char *function = "libcnotify_stream_close";

	if( libcnotify_stream_opened_in_library != 0 )
	{
		if( file_stream_close(
		     libcnotify_stream ) != 0 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_OPEN_FAILED,
			 "%s: unable to close stream.",
			 function );

			return( -1 );
		}
		libcnotify_stream                   = NULL;
		libcnotify_stream_opened_in_library = 0;
	}
	return( 0 );
}

