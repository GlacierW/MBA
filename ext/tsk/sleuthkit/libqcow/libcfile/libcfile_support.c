/*
 * Support functions
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

#if defined( HAVE_SYS_STAT_H )
#include <sys/stat.h>
#endif

#if defined( HAVE_ERRNO_H )
#include <errno.h>
#endif

#if defined( HAVE_UNISTD_H )
#include <unistd.h>
#endif

#include "libcfile_definitions.h"
#include "libcfile_libcerror.h"
#include "libcfile_libclocale.h"
#include "libcfile_libuna.h"
#include "libcfile_support.h"

#if !defined( HAVE_LOCAL_LIBCFILE )

/* Returns the library version as a string
 */
const char *libcfile_get_version(
             void )
{
	return( (const char *) LIBCFILE_VERSION_STRING );
}

/* Retrieves the narrow system string codepage
 * A value of 0 represents no codepage, UTF-8 encoding is used instead
 * Returns 1 if successful or -1 on error
 */
int libcfile_get_codepage(
     int *codepage,
     libcerror_error_t **error )
{
	static char *function = "libcfile_get_codepage";

	if( libclocale_codepage_get(
	     codepage,
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
	return( 1 );
}

/* Sets the narrow system string codepage
 * A value of 0 represents no codepage, UTF-8 encoding is used instead
 * Returns 1 if successful or -1 on error
 */
int libcfile_set_codepage(
     int codepage,
     libcerror_error_t **error )
{
	static char *function = "libcfile_set_codepage";

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
	return( 1 );
}

#endif /* !defined( HAVE_LOCAL_LIBCFILE ) */

#if defined( WINAPI ) && ( WINVER <= 0x0500 )

/* Cross Windows safe version of GetFileAttributesA
 * Returns the file attributs if successful or INVALID_FILE_ATTRIBUTES on error
 */
DWORD libcfile_GetFileAttributesA(
       LPCSTR filename )
{
	FARPROC function       = NULL;
	HMODULE library_handle = NULL;
	DWORD result           = 0;

	if( filename == NULL )
	{
		return( INVALID_FILE_ATTRIBUTES );
	}
	library_handle = LoadLibrary(
	                  _SYSTEM_STRING( "kernel32.dll" ) );

	if( library_handle == NULL )
	{
		return( INVALID_FILE_ATTRIBUTES );
	}
	function = GetProcAddress(
		    library_handle,
		    (LPCSTR) "GetFileAttributesA" );

	if( function != NULL )
	{
		result = function(
			  filename );
	}
	/* This call should be after using the function
	 * in most cases kernel32.dll will still be available after free
	 */
	if( FreeLibrary(
	     library_handle ) != TRUE )
	{
		result = INVALID_FILE_ATTRIBUTES;
	}
	return( result );
}

#endif /* defined( WINAPI ) && ( WINVER <= 0x0500 ) */

#if defined( WINAPI )

/* Determines if a file exists
 * This function uses the WINAPI function for Windows XP (0x0501) or later,
 * or tries to dynamically call the function for Windows 2000 (0x0500) or earlier
 * Returns 1 if the file exists, 0 if not or -1 on error
 */
int libcfile_file_exists(
     const char *filename,
     libcerror_error_t **error )
{
	static char *function = "libcfile_file_exists";
	int result            = 1;
	DWORD error_code      = 0;
	DWORD file_attributes = 0;

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
#if ( WINVER <= 0x0500 )
	file_attributes = libcfile_GetFileAttributesA(
	                   (LPCSTR) filename );
#else
	file_attributes = GetFileAttributesA(
	                   (LPCSTR) filename );
#endif
	if( file_attributes == INVALID_FILE_ATTRIBUTES )
	{
		error_code = GetLastError();

		switch( error_code )
		{
			case ERROR_ACCESS_DENIED:
				result = 1;

				break;

			case ERROR_FILE_NOT_FOUND:
			case ERROR_PATH_NOT_FOUND:
				result = 0;

				break;

			default:
				libcerror_system_set_error(
				 error,
				 LIBCERROR_ERROR_DOMAIN_IO,
				 LIBCERROR_IO_ERROR_GENERIC,
				 error_code,
				 "%s: unable to determine attributes of file: %s.",
				 function,
				 filename );

				return( -1 );
		}
	}
	return( result );
}

#elif defined( HAVE_STAT )

/* Determines if a file exists
 * This function uses the POSIX stat function or equivalent
 * Returns 1 if the file exists, 0 if not or -1 on error
 */
int libcfile_file_exists(
     const char *filename,
     libcerror_error_t **error )
{
	struct stat file_statistics;

	static char *function = "libcfile_file_exists";
	int result            = 0;

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
	if( memory_set(
	     &file_statistics,
	     0,
	     sizeof( struct stat ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear file statistics.",
		 function );

		return( -1 );
	}
	result = stat(
	          filename,
	          &file_statistics );

	if( result != 0 )
	{
		switch( errno )
		{
			case EACCES:
				result = 1;

				break;

			case ENOENT:
				result = 0;

				break;

			default:
				libcerror_system_set_error(
				 error,
				 LIBCERROR_ERROR_DOMAIN_IO,
				 LIBCERROR_IO_ERROR_GENERIC,
				 errno,
				 "%s: unable to stat file: %" PRIs_SYSTEM ".",
				 function,
				 filename );

				return( -1 );
		}
	}
	else
	{
		result = 1;
	}
	return( result );
}

#else
#error Missing file exists function
#endif

#if defined( HAVE_WIDE_CHARACTER_TYPE )

#if defined( WINAPI ) && ( WINVER <= 0x0500 )

/* Cross Windows safe version of GetFileAttributesW
 * Returns the file attributs if successful or INVALID_FILE_ATTRIBUTES on error
 */
DWORD libcfile_GetFileAttributesW(
       LPCWSTR filename )
{
	FARPROC function       = NULL;
	HMODULE library_handle = NULL;
	DWORD result           = 0;

	if( filename == NULL )
	{
		return( INVALID_FILE_ATTRIBUTES );
	}
	library_handle = LoadLibrary(
	                  _SYSTEM_STRING( "kernel32.dll" ) );

	if( library_handle == NULL )
	{
		return( INVALID_FILE_ATTRIBUTES );
	}
	function = GetProcAddress(
		    library_handle,
		    (LPCSTR) "GetFileAttributesW" );

	if( function != NULL )
	{
		result = function(
			  filename );
	}
	/* This call should be after using the function
	 * in most cases kernel32.dll will still be available after free
	 */
	if( FreeLibrary(
	     library_handle ) != TRUE )
	{
		result = INVALID_FILE_ATTRIBUTES;
	}
	return( result );
}

#endif /* defined( WINAPI ) && ( WINVER <= 0x0500 ) */

#if defined( WINAPI )

/* Determines if a file exists using get file attibutes
 * This function uses the WINAPI function for Windows XP (0x0501) or later,
 * or tries to dynamically call the function for Windows 2000 (0x0500) or earlier
 * Returns 1 if the file exists, 0 if not or -1 on error
 */
int libcfile_file_exists_wide(
     const wchar_t *filename,
     libcerror_error_t **error )
{
	static char *function = "libcfile_file_exists_wide";
	int result            = 1;
	DWORD error_code      = 0;
	DWORD file_attributes = 0;

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
#if ( WINVER <= 0x0500 )
	file_attributes = libcfile_GetFileAttributesW(
	                   (LPCWSTR) filename );
#else
	file_attributes = GetFileAttributesW(
	                   (LPCWSTR) filename );
#endif
	if( file_attributes == INVALID_FILE_ATTRIBUTES )
	{
		error_code = GetLastError();

		switch( error_code )
		{
			case ERROR_ACCESS_DENIED:
				result = 1;

				break;

			case ERROR_FILE_NOT_FOUND:
			case ERROR_PATH_NOT_FOUND:
				result = 0;

				break;

			default:
				libcerror_system_set_error(
				 error,
				 LIBCERROR_ERROR_DOMAIN_IO,
				 LIBCERROR_IO_ERROR_GENERIC,
				 error_code,
				 "%s: unable to determine attributes of file: %ls.",
				 function,
				 filename );

				return( -1 );
		}
	}
	return( result );
}

#elif defined( HAVE_STAT )

/* Determines if a file exists
 * This function uses the POSIX stat function or equivalent
 * Returns 1 if the file exists, 0 if not or -1 on error
 */
int libcfile_file_exists_wide(
     const wchar_t *filename,
     libcerror_error_t **error )
{
	struct stat file_statistics;

	char *narrow_filename       = NULL;
	static char *function       = "libcfile_file_exists_wide";
	size_t narrow_filename_size = 0;
	size_t filename_size        = 0;
	int result                  = 0;

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
	filename_size = 1 + wide_string_length(
	                     filename );

	/* Convert the filename to a narrow string
	 * if the platform has no wide character open function
	 */
	if( libclocale_codepage == 0 )
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_utf8_string_size_from_utf32(
		          (libuna_utf32_character_t *) filename,
		          filename_size,
		          &narrow_filename_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_utf8_string_size_from_utf16(
		          (libuna_utf16_character_t *) filename,
		          filename_size,
		          &narrow_filename_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	else
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_byte_stream_size_from_utf32(
		          (libuna_utf32_character_t *) filename,
		          filename_size,
		          libclocale_codepage,
		          &narrow_filename_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_byte_stream_size_from_utf16(
		          (libuna_utf16_character_t *) filename,
		          filename_size,
		          libclocale_codepage,
		          &narrow_filename_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	if( result != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_CONVERSION,
		 LIBCERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to determine narrow character filename size.",
		 function );

		return( -1 );
	}
	narrow_filename = narrow_string_allocate(
	                   narrow_filename_size );

	if( narrow_filename == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create narrow character filename.",
		 function );

		return( -1 );
	}
	if( libclocale_codepage == 0 )
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_utf8_string_copy_from_utf32(
		          (libuna_utf8_character_t *) narrow_filename,
		          narrow_filename_size,
		          (libuna_utf32_character_t *) filename,
		          filename_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_utf8_string_copy_from_utf16(
		          (libuna_utf8_character_t *) narrow_filename,
		          narrow_filename_size,
		          (libuna_utf16_character_t *) filename,
		          filename_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	else
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_byte_stream_copy_from_utf32(
		          (uint8_t *) narrow_filename,
		          narrow_filename_size,
		          libclocale_codepage,
		          (libuna_utf32_character_t *) filename,
		          filename_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_byte_stream_copy_from_utf16(
		          (uint8_t *) narrow_filename,
		          narrow_filename_size,
		          libclocale_codepage,
		          (libuna_utf16_character_t *) filename,
		          filename_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	if( result != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_CONVERSION,
		 LIBCERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to set narrow character filename.",
		 function );

		memory_free(
		 narrow_filename );

		return( -1 );
	}
	result = stat(
	          narrow_filename,
	          &file_statistics );

	memory_free(
	 narrow_filename );

	if( result != 0 )
	{
		switch( errno )
		{
			case EACCES:
				result = 1;

				break;

			case ENOENT:
				result = 0;

				break;

			default:
				libcerror_system_set_error(
				 error,
				 LIBCERROR_ERROR_DOMAIN_IO,
				 LIBCERROR_IO_ERROR_GENERIC,
				 errno,
				 "%s: unable to stat file: %" PRIs_SYSTEM ".",
				 function,
				 filename );

				return( -1 );
		}
	}
	else
	{
		result = 1;
	}
	return( result );
}

#else
#error Missing file exists wide function
#endif

#endif /* defined( HAVE_WIDE_CHARACTER_TYPE ) */

#if defined( WINAPI ) && ( WINVER <= 0x0500 )

/* Cross Windows safe version of DeleteFileA
 * Returns TRUE if successful or FALSE on error
 */
BOOL libcfile_DeleteFileA(
      LPCSTR filename )
{
	FARPROC function       = NULL;
	HMODULE library_handle = NULL;
	BOOL result            = FALSE;

	if( filename == NULL )
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
		    (LPCSTR) "DeleteFileA" );

	if( function != NULL )
	{
		result = function(
			  filename );
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

#endif /* defined( WINAPI ) && ( WINVER <= 0x0500 ) */

/* Removes a file
 * Returns 1 if successful or -1 on error
 */
int libcfile_file_remove(
     const char *filename,
     libcerror_error_t **error )
{
	static char *function = "libcfile_file_remove";
	uint32_t error_code   = 0;

	if( libcfile_file_remove_with_error_code(
	     filename,
	     &error_code,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_UNLINK_FAILED,
		 "%s: unable to remove file.",
		 function );

		return( -1 );
	}
	return( 1 );
}

#if defined( WINAPI )

/* Removes a file
 * This function uses the WINAPI function for Windows XP (0x0501) or later,
 * or tries to dynamically call the function for Windows 2000 (0x0500) or earlier
 * Returns 1 if successful or -1 on error
 */
int libcfile_file_remove_with_error_code(
     const char *filename,
     uint32_t *error_code,
     libcerror_error_t **error )
{
	static char *function = "libcfile_file_remove_with_error_code";
	BOOL result           = FALSE;

	if( error_code == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid error code.",
		 function );

		return( -1 );
	}
#if ( WINVER <= 0x0500 )
	result = libcfile_DeleteFileA(
	          filename );
#else
	result = DeleteFileA(
	          filename );
#endif
	if( result == 0 )
	{
		*error_code = GetLastError();

		libcerror_system_set_error(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_UNLINK_FAILED,
		 *error_code,
		 "%s: unable to remove file.",
		 function );

		return( -1 );
	}
	return( 1 );
}

#elif defined( HAVE_UNLINK )

/* Removes a file
 * This function uses the POSIX unlink function or equivalent
 * Returns 1 if successful or -1 on error
 */
int libcfile_file_remove_with_error_code(
     const char *filename,
     uint32_t *error_code,
     libcerror_error_t **error )
{
	static char *function = "libcfile_file_remove_with_error_code";

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
	if( error_code == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid error code.",
		 function );

		return( -1 );
	}
	if( unlink(
	     filename ) != 0 )
	{
		*error_code = (uint32_t) errno;

		libcerror_system_set_error(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_UNLINK_FAILED,
		 *error_code,
		 "%s: unable to unlink file.",
		 function );

		return( -1 );
	}
	return( 1 );
}

#else
#error Missing file remove function
#endif

#if defined( HAVE_WIDE_CHARACTER_TYPE )

#if defined( WINAPI ) && ( WINVER <= 0x0500 )

/* Cross Windows safe version of DeleteFileW
 * Returns TRUE if successful or FALSE on error
 */
BOOL libcfile_DeleteFileW(
      LPCWSTR filename )
{
	FARPROC function       = NULL;
	HMODULE library_handle = NULL;
	BOOL result            = FALSE;

	if( filename == NULL )
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
		    (LPCSTR) "DeleteFileW" );

	if( function != NULL )
	{
		result = function(
			  filename );
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

#endif /* defined( WINAPI ) && ( WINVER <= 0x0500 ) */

/* Removes a file
 * Returns 1 if successful or -1 on error
 */
int libcfile_file_remove_wide(
     const wchar_t *filename,
     libcerror_error_t **error )
{
	static char *function = "libcfile_file_remove_wide";
	uint32_t error_code   = 0;

	if( libcfile_file_remove_wide_with_error_code(
	     filename,
	     &error_code,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_UNLINK_FAILED,
		 "%s: unable to remove file.",
		 function );

		return( -1 );
	}
	return( 1 );
}

#if defined( WINAPI )

/* Removes a file
 * This function uses the WINAPI function for Windows XP (0x0501) or later,
 * or tries to dynamically call the function for Windows 2000 (0x0500) or earlier
 * Returns 1 if successful or -1 on error
 */
int libcfile_file_remove_wide_with_error_code(
     const wchar_t *filename,
     uint32_t *error_code,
     libcerror_error_t **error )
{
	static char *function = "libcfile_file_remove_wide_with_error_code";
	BOOL result           = FALSE;

	if( error_code == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid error code.",
		 function );

		return( -1 );
	}
#if ( WINVER <= 0x0500 )
	result = libcfile_DeleteFileW(
	          filename );
#else
	result = DeleteFileW(
	          filename );
#endif
	if( result == 0 )
	{
		*error_code = GetLastError();

		libcerror_system_set_error(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_UNLINK_FAILED,
		 *error_code,
		 "%s: unable to remove file.",
		 function );

		return( -1 );
	}
	return( 1 );
}

#elif defined( HAVE_UNLINK )

/* Removes a file
 * This function uses the POSIX unlink function or equivalent
 * Returns 1 if successful or -1 on error
 */
int libcfile_file_remove_wide_with_error_code(
     const wchar_t *filename,
     uint32_t *error_code,
     libcerror_error_t **error )
{
	char *narrow_filename       = NULL;
	static char *function       = "libcfile_file_remove_wide_with_error_code";
	size_t narrow_filename_size = 0;
	size_t filename_size        = 0;
	int result                  = 0;

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
	if( error_code == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid error code.",
		 function );

		return( -1 );
	}
	filename_size = 1 + wide_string_length(
	                     filename );

	/* Convert the filename to a narrow string
	 * if the platform has no wide character open function
	 */
	if( libclocale_codepage == 0 )
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_utf8_string_size_from_utf32(
		          (libuna_utf32_character_t *) filename,
		          filename_size,
		          &narrow_filename_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_utf8_string_size_from_utf16(
		          (libuna_utf16_character_t *) filename,
		          filename_size,
		          &narrow_filename_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	else
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_byte_stream_size_from_utf32(
		          (libuna_utf32_character_t *) filename,
		          filename_size,
		          libclocale_codepage,
		          &narrow_filename_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_byte_stream_size_from_utf16(
		          (libuna_utf16_character_t *) filename,
		          filename_size,
		          libclocale_codepage,
		          &narrow_filename_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	if( result != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_CONVERSION,
		 LIBCERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to determine narrow character filename size.",
		 function );

		goto on_error;
	}
	narrow_filename = narrow_string_allocate(
	                   narrow_filename_size );

	if( narrow_filename == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create narrow character filename.",
		 function );

		goto on_error;
	}
	if( libclocale_codepage == 0 )
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_utf8_string_copy_from_utf32(
		          (libuna_utf8_character_t *) narrow_filename,
		          narrow_filename_size,
		          (libuna_utf32_character_t *) filename,
		          filename_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_utf8_string_copy_from_utf16(
		          (libuna_utf8_character_t *) narrow_filename,
		          narrow_filename_size,
		          (libuna_utf16_character_t *) filename,
		          filename_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	else
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_byte_stream_copy_from_utf32(
		          (uint8_t *) narrow_filename,
		          narrow_filename_size,
		          libclocale_codepage,
		          (libuna_utf32_character_t *) filename,
		          filename_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_byte_stream_copy_from_utf16(
		          (uint8_t *) narrow_filename,
		          narrow_filename_size,
		          libclocale_codepage,
		          (libuna_utf16_character_t *) filename,
		          filename_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	if( result != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_CONVERSION,
		 LIBCERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to set narrow character filename.",
		 function );

		goto on_error;
	}
	if( unlink(
	     narrow_filename ) != 0 )
	{
		*error_code = (uint32_t) errno;

		libcerror_system_set_error(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_UNLINK_FAILED,
		 *error_code,
		 "%s: unable to unlink file.",
		 function );

		goto on_error;
	}
	memory_free(
	 narrow_filename );

	return( 1 );

on_error:
	if( narrow_filename != NULL )
	{
		memory_free(
		 narrow_filename );
	}
	return( -1 );
}

#else
#error Missing file remove wide function
#endif

#endif /* defined( HAVE_WIDE_CHARACTER_TYPE ) */

