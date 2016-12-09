/*
 * Notification functions
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
#include <file_stream.h>
#include <types.h>

#if defined( HAVE_STDLIB_H ) || defined( WINAPI )
#include <stdlib.h>
#endif

#include "libfdata_libcerror.h"
#include "libfdata_libcnotify.h"
#include "libfdata_notify.h"

#if !defined( HAVE_LOCAL_LIBFDATA )

/* Sets the verbose notification
 */
void libfdata_notify_set_verbose(
      int verbose )
{
	libcnotify_verbose_set(
	 verbose );
}

/* Sets the notification stream
 * Returns 1 if successful or -1 on error
 */
int libfdata_notify_set_stream(
     FILE *stream,
     libcerror_error_t **error )
{
	static char *function = "libfdata_notify_set_stream";

	if( libcnotify_stream_set(
	     stream,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set stream.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Opens the notification stream using a filename
 * The stream is opened in append mode
 * Returns 1 if successful or -1 on error
 */
int libfdata_notify_stream_open(
     const char *filename,
     libcerror_error_t **error )
{
	static char *function = "libfdata_notify_stream_open";

	if( libcnotify_stream_open(
	     filename,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_OPEN_FAILED,
		 "%s: unable to open stream.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Closes the notification stream if opened using a filename
 * Returns 0 if successful or -1 on error
 */
int libfdata_notify_stream_close(
     libcerror_error_t **error )
{
	static char *function = "libfdata_notify_stream_close";

	if( libcnotify_stream_close(
	     error ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_OPEN_FAILED,
		 "%s: unable to open stream.",
		 function );

		return( -1 );
	}
	return( 0 );
}

#endif /* !defined( HAVE_LOCAL_LIBFDATA ) */

