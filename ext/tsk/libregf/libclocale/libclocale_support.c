/*
 * Support functions
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
#include <types.h>

#if defined( HAVE_LIBINTL_H )
#include <libintl.h>
#endif

#include "libclocale_codepage.h"
#include "libclocale_definitions.h"
#include "libclocale_libcerror.h"
#include "libclocale_locale.h"
#include "libclocale_support.h"

#if !defined( HAVE_LOCAL_LIBCLOCALE )

/* Returns the library version as a string
 */
const char *libclocale_get_version(
             void )
{
	return( (const char *) LIBCLOCALE_VERSION_STRING );
}

#endif /* !defined( HAVE_LOCAL_LIBCLOCALE ) */

/* Initializes the locale values
 * Returns 1 if successful or -1 on error
 */
int libclocale_initialize(
     const char *domain_name,
     libcerror_error_t **error )
{
	static char *function = "libclocale_initialize";

#if !defined( HAVE_WIDE_SYSTEM_CHARACTER )
	int codepage          = 0;
#endif

	if( domain_name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid domain name.",
		 function );

		return( -1 );
	}
#if defined( HAVE_BINDTEXTDOMAIN ) && defined( HAVE_TEXTDOMAIN )
	if( bindtextdomain(
	     domain_name,
	     LOCALEDIR ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to bind text domain.",
		 function );

		return( -1 );
	}
	if( textdomain(
	     domain_name ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set text domain.",
		 function );

		return( -1 );
	}
#endif /* defined( HAVE_BINDTEXTDOMAIN ) && defined( HAVE_TEXTDOMAIN ) */

#if !defined( HAVE_WIDE_SYSTEM_CHARACTER )
	if( libclocale_locale_get_codepage(
	     &codepage,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve codepage.",
		 function );

		return( -1 );
	}
	if( libclocale_codepage_set(
	     codepage,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set codepage.",
		 function );

		return( -1 );
	}
#endif /* !defined( HAVE_WIDE_SYSTEM_CHARACTER ) */

	return( 1 );
}

