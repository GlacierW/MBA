/*
 * System functions
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
#include <memory.h>
#include <narrow_string.h>
#include <system_string.h>
#include <types.h>
#include <wide_string.h>

#if defined( HAVE_STDLIB_H ) || defined( WINAPI )
#include <stdlib.h>
#endif

#if defined( HAVE_STRING_H ) || defined( WINAPI )
#include <string.h>
#endif

#if defined( HAVE_STDARG_H ) || defined( WINAPI )
#include <stdarg.h>
#elif defined( HAVE_VARARGS_H )
#include <varargs.h>
#else
#error Missing headers stdarg.h and varargs.h
#endif

#include "libcerror_error.h"
#include "libcerror_system.h"
#include "libcerror_types.h"

/* The make language identifier macro for the WINAPI FormatMessage function
 */
#if !defined( MAKELANGID )
#define MAKELANGID( primary_language_identifier, sub_language_identifier ) \
	( ( ( (WORD) ( sub_language_identifier ) ) << 10 ) | (WORD) ( primary_language_identifier ) )
#endif

#if !defined( LANG_NEUTRAL )
#define LANG_NEUTRAL		0
#endif

#if !defined( SUBLANG_DEFAULT )
#define SUBLANG_DEFAULT		1
#endif

#if defined( WINAPI ) && ( WINVER <= 0x0500 )

/* Cross Windows safe version of FormatMessageA
 * Returns TRUE if successful or FALSE on error
 */
DWORD libcerror_FormatMessageA(
       DWORD flags,
       LPCVOID source,
       DWORD message_identifier,
       DWORD language_identifier,
       LPCSTR string,
       DWORD string_size,
       va_list *argument_list )
{
	FARPROC function       = NULL;
	HMODULE library_handle = NULL;
	DWORD result           = 0;

	if( string == NULL )
	{
		return( FALSE );
	}
	library_handle = LoadLibrary(
	                  _SYSTEM_STRING( "kernel32.dll" ) );

	if( library_handle == NULL )
	{
		return( FALSE );
	}
	function = GetProcAddress(
		    library_handle,
		    (LPCSTR) "FormatMessageA" );

	if( function != NULL )
	{
		result = function(
			  flags,
			  source,
			  message_identifier,
			  language_identifier,
			  string,
			  string_size,
			  argument_list );
	}
	/* This call should be after using the function
	 * in most cases kernel32.dll will still be available after free
	 */
	if( FreeLibrary(
	     library_handle ) != TRUE )
	{
		result = FALSE;
	}
	return( result );
}

/* Cross Windows safe version of FormatMessageW
 * Returns TRUE if successful or FALSE on error
 */
DWORD libcerror_FormatMessageW(
       DWORD flags,
       LPCVOID source,
       DWORD message_identifier,
       DWORD language_identifier,
       LPWSTR string,
       DWORD string_size,
       va_list *argument_list )
{
	FARPROC function       = NULL;
	HMODULE library_handle = NULL;
	DWORD result           = 0;

	if( string == NULL )
	{
		return( FALSE );
	}
	library_handle = LoadLibrary(
	                  _SYSTEM_STRING( "kernel32.dll" ) );

	if( library_handle == NULL )
	{
		return( FALSE );
	}
	function = GetProcAddress(
		    library_handle,
		    (LPCSTR) "FormatMessageA" );

	if( function != NULL )
	{
		result = function(
			  flags,
			  source,
			  message_identifier,
			  language_identifier,
			  string,
			  string_size,
			  argument_list );
	}
	/* This call should be after using the function
	 * in most cases kernel32.dll will still be available after free
	 */
	if( FreeLibrary(
	     library_handle ) != TRUE )
	{
		result = FALSE;
	}
	return( result );
}

#endif

#if defined( WINAPI ) && ( WINVER >= 0x0501 )

/* Retrieves a descriptive string of the error number
 * This function uses the WINAPI functions for Windows XP or later
 * Returns the string_length if successful or -1 on error
 */
int libcerror_system_copy_string_from_error_number(
     system_character_t *string,
     size_t string_size,
     uint32_t error_number )
{
	DWORD flags       = 0;
	DWORD print_count = 0;

	if( string == NULL )
	{
		return( -1 );
	}
	flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;

	if( string_size > (size_t) INT_MAX )
	{
		return( -1 );
	}
#if ( WINVER <= 0x0500 )
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
	print_count = libcerror_FormatMessageW(
	               flags,
	               NULL,
	               (DWORD) error_number,
	               MAKELANGID(
	                LANG_NEUTRAL,
	                SUBLANG_DEFAULT ),
	               string,
	               (DWORD) string_size,
	               NULL );
#else
	print_count = libcerror_FormatMessageA(
	               flags,
	               NULL,
	               (DWORD) error_number,
	               MAKELANGID(
	                LANG_NEUTRAL,
	                SUBLANG_DEFAULT ),
	               string,
	               (DWORD) string_size,
	               NULL );
#endif
#else
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
	print_count = FormatMessageW(
	               flags,
	               NULL,
	               (DWORD) error_number,
	               MAKELANGID(
	                LANG_NEUTRAL,
	                SUBLANG_DEFAULT ),
	               string,
	               (DWORD) string_size,
	               NULL );
#else
	print_count = FormatMessageA(
	               flags,
	               NULL,
	               (DWORD) error_number,
	               MAKELANGID(
	                LANG_NEUTRAL,
	                SUBLANG_DEFAULT ),
	               string,
	               (DWORD) string_size,
	               NULL );
#endif
#endif /* ( WINVER <= 0x0500 ) */


	if( print_count == 0 )
	{
		return( -1 );
	}
	return( (int) print_count );
}

#elif defined( HAVE_STRERROR_R )

/* Retrieves a descriptive string of the error number
 * This function uses the POSIX strerror_r function or equivalent
 * Returns the string_length if successful or -1 on error
 */
int libcerror_system_copy_string_from_error_number(
     system_character_t *string,
     size_t string_size,
     uint32_t error_number )
{
	size_t string_length = 0;

	if( string == NULL )
	{
		return( -1 );
	}
	if( string_size > (size_t) INT_MAX )
	{
		return( -1 );
	}
/* Sanity check
 */
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
#error Missing wide character strerror_r function
#endif

#if defined( STRERROR_R_CHAR_P )
	if( strerror_r(
	     (int) error_number,
	     string,
	     string_size ) == NULL )
#else
	if( strerror_r(
	     (int) error_number,
	     string,
	     string_size ) != 0 )
#endif
	{
		return( -1 );
	}
	string[ string_size - 1 ] = (system_character_t) 0;

	string_length = system_string_length(
	                 string );

	return( (int) string_length );
}

#elif defined( HAVE_STRERROR ) || defined( WINAPI )

/* Retrieves a descriptive string of the error number
 * This function uses the POSIX strerror function or equivalent
 * Returns the string_length if successful or -1 on error
 */
int libcerror_system_copy_string_from_error_number(
     system_character_t *string,
     size_t string_size,
     uint32_t error_number )
{
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
	const wchar_t *static_error_string = NULL;
#else
	const char *static_error_string    = NULL;
#endif
	size_t static_error_string_length  = 0;

	if( string == NULL )
	{
		return( -1 );
	}
	if( string_size > (size_t) INT_MAX )
	{
		return( -1 );
	}
/* Sanity check
 */
#if defined( HAVE_WIDE_SYSTEM_CHARACTER ) && !defined( WINAPI )
#error Missing wide character strerror function
#endif

#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
	static_error_string = _wcserror(
	                       (int) error_number );
#else
	static_error_string = strerror(
	                       (int) error_number );
#endif

	if( static_error_string == NULL )
	{
		return( -1 );
	}
	static_error_string_length = system_string_length(
	                              static_error_string );

	if( system_string_copy(
	     string,
	     static_error_string,
	     static_error_string_length ) == NULL )
	{
		return( -1 );
	}
	string[ static_error_string_length ] = 0;

	return( (int) static_error_string_length );
}

#else
#error Missing error to string system function
#endif

#if defined( HAVE_STDARG_H ) || defined( WINAPI )
#define VARARGS( function, error, error_domain, error_code, system_error_code, type, argument ) \
        function( error, error_domain, error_code, system_error_code, type argument, ... )
#define VASTART( argument_list, type, name ) \
        va_start( argument_list, name )
#define VAEND( argument_list ) \
        va_end( argument_list )

#elif defined( HAVE_VARARGS_H )
#define VARARGS( function, error, error_domain, error_code, system_error_code, type, argument ) \
        function( error, error_domain, error_code, system_error_code, va_alist ) va_dcl
#define VASTART( argument_list, type, name ) \
        { type name; va_start( argument_list ); name = va_arg( argument_list, type )
#define VAEND( argument_list ) \
        va_end( argument_list ); }

#endif

/* Sets an error and adds a system specific error string if possible
 * Creates the error if necessary
 * The error domain and code are set only the first time and the error message is appended for back tracing
 */
void VARARGS(
      libcerror_system_set_error,
      libcerror_error_t **error,
      int error_domain,
      int error_code,
      uint32_t system_error_code,
      const char *,
      format_string )
{
	va_list argument_list;

	libcerror_internal_error_t *internal_error = NULL;
	system_character_t *system_format_string   = NULL;
	void *reallocation                         = NULL;
	size_t format_string_length                = 0;
	size_t message_size                        = LIBCERROR_MESSAGE_INCREMENT_SIZE;
	size_t string_index                        = 0;
	int message_index                          = 0;
	int print_count                            = 0;

	if( error == NULL )
	{
		return;
	}
	if( format_string == NULL )
	{
		return;
	}
	format_string_length = narrow_string_length(
	                        format_string );

	if( format_string_length > message_size )
	{
		message_size = ( ( format_string_length / LIBCERROR_MESSAGE_INCREMENT_SIZE ) + 1 )
		             * LIBCERROR_MESSAGE_INCREMENT_SIZE;
	}
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
	do
	{
		reallocation = memory_reallocate(
		                system_format_string,
		                sizeof( system_character_t ) * ( format_string_length + 1 ) );

		if( reallocation == NULL )
		{
			goto on_error;
		}
		system_format_string = (system_character_t *) reallocation;

#if defined( __BORLANDC__ ) || defined( _MSC_VER )
		print_count = wide_string_snwprintf(
		               system_format_string,
		               format_string_length + 1,
		               L"%S",
		               format_string );
#else
		print_count = wide_string_snwprintf(
		               system_format_string,
		               format_string_length + 1,
		               L"%s",
		               format_string );
#endif

		if( print_count <= -1 )
		{
			format_string_length += LIBCERROR_MESSAGE_INCREMENT_SIZE;
		}
		else if( ( (size_t) print_count > format_string_length )
		      || ( system_format_string[ print_count ] != 0 ) )
		{
			format_string_length = (size_t) print_count;
			print_count  = -1;
		}
		if( format_string_length >= LIBCERROR_MESSAGE_MAXIMUM_SIZE )
		{
			goto on_error;
		}
	}
	while( print_count <= -1 );
#else
	system_format_string = (system_character_t *) format_string;
#endif

#if defined( __BORLANDC__ ) || defined( _MSC_VER )
	/* Rewrite %s to %S
	 */
	string_index  = 0;

	while( string_index < format_string_length )
	{
		if( system_format_string[ string_index ] == 0 )
		{
			break;
		}
		else if( system_format_string[ string_index ] == (system_character_t) '%' )
		{
			string_index++;

			if( system_format_string[ string_index ] == (system_character_t) 's' )
			{
				 system_format_string[ string_index ] = (system_character_t) 'S';
			}
		}
		string_index++;
	}
#endif
	if( *error == NULL )
	{
		internal_error = memory_allocate_structure(
		                  libcerror_internal_error_t );

		if( internal_error == NULL )
		{
			goto on_error;
		}
		internal_error->domain             = error_domain;
		internal_error->code               = error_code;
		internal_error->number_of_messages = 0;
		internal_error->messages           = NULL;
		internal_error->sizes              = NULL;

		*error = (libcerror_error_t *) internal_error;
	}
	else
	{
		internal_error = (libcerror_internal_error_t *) *error;
	}
	reallocation = memory_reallocate(
	                internal_error->messages,
	                sizeof( system_character_t * ) * ( internal_error->number_of_messages + 1 ) );

	if( reallocation == NULL )
	{
		goto on_error;
	}
	internal_error->messages = (system_character_t **) reallocation;

	reallocation = memory_reallocate(
	                internal_error->sizes,
	                sizeof( size_t ) * ( internal_error->number_of_messages + 1 ) );

	if( reallocation == NULL )
	{
		goto on_error;
	}
	internal_error->sizes = (size_t *) reallocation;

	message_index                             = internal_error->number_of_messages;
	internal_error->messages[ message_index ] = NULL;
	internal_error->sizes[ message_index ]    = 0;
	internal_error->number_of_messages       += 1;

	do
	{
		reallocation = memory_reallocate(
		                internal_error->messages[ message_index ],
		                sizeof( system_character_t ) * message_size );

		if( reallocation == NULL )
		{
			memory_free(
			 internal_error->messages[ message_index ] );

			internal_error->messages[ message_index ] = NULL;

			break;
		}
		internal_error->messages[ message_index ] = (system_character_t *) reallocation;

		VASTART(
		 argument_list,
		 const char *,
		 format_string );

		print_count = system_string_vsprintf(
		               internal_error->messages[ message_index ],
		               message_size,
		               system_format_string,
		               argument_list );

		VAEND(
		 argument_list );

		if( print_count <= -1 )
		{
			message_size += LIBCERROR_MESSAGE_INCREMENT_SIZE;
		}
		else if( ( (size_t) print_count > message_size )
		      || ( ( internal_error->messages[ message_index ] )[ print_count ] != 0 ) )
		{
			message_size = (size_t) ( print_count + 1 );
			print_count  = -1;
		}
		if( message_size >= LIBCERROR_MESSAGE_MAXIMUM_SIZE )
		{
			memory_free(
			 internal_error->messages[ message_index ] );

			internal_error->messages[ message_index ] = NULL;
			internal_error->sizes[ message_index ]    = 0;

			break;
		}
		internal_error->sizes[ message_index ] = (size_t) print_count + 1;
	}
	while( print_count <= -1 );

#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
	memory_free(
	 system_format_string );

	system_format_string = NULL;
#endif

	string_index = internal_error->sizes[ message_index ] - 1;

	if( ( internal_error->messages[ message_index ] != NULL )
	 && ( ( internal_error->messages[ message_index ] )[ string_index - 1 ] == (system_character_t) '.' ) )
	{
		string_index -= 1;
	}
	reallocation = memory_reallocate(
			internal_error->messages[ message_index ],
			sizeof( system_character_t ) * ( message_size + 13 + 512 ) );

	if( reallocation == NULL )
	{
		memory_free(
		 internal_error->messages[ message_index ] );

		internal_error->messages[ message_index ] = NULL;

		goto on_error;
	}
	internal_error->messages[ message_index ] = (system_character_t *) reallocation;

	if( system_string_copy(
	     &( ( internal_error->messages[ message_index ] )[ string_index ] ),
	     _SYSTEM_STRING( " with error: " ),
	     13 ) == NULL )
	{
		memory_free(
		 internal_error->messages[ message_index ] );

		internal_error->messages[ message_index ] = NULL;

		goto on_error;
	}
	internal_error->sizes[ message_index ] += 13;
	string_index                           += 13;

	print_count = libcerror_system_copy_string_from_error_number(
	               &( ( internal_error->messages[ message_index ] )[ string_index ] ),
	               512,
	               system_error_code );

	if( print_count == -1 )
	{
		goto on_error;
	}
	message_size += (size_t) print_count;

	if( message_size >= LIBCERROR_MESSAGE_MAXIMUM_SIZE )
	{
		memory_free(
		 internal_error->messages[ message_index ] );

		internal_error->messages[ message_index ] = NULL;
		internal_error->sizes[ message_index ]    = 0;

		goto on_error;
	}
	internal_error->sizes[ message_index ] += print_count;

	return;

on_error:
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
	if( system_format_string != NULL )
	{
		memory_free(
		 system_format_string );
	}
#endif
	if( ( *error == NULL )
	 && ( internal_error != NULL ) )
	{
		memory_free(
		 internal_error );
	}
	return;
}

#undef VARARGS
#undef VASTART
#undef VAEND

