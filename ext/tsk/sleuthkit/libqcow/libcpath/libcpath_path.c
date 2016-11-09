/*
 * Path functions
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
#include <types.h>
#include <wide_string.h>

#if defined( HAVE_ERRNO_H )
#include <errno.h>
#endif

#if defined( HAVE_SYS_STAT_H )
#include <sys/stat.h>
#endif

#if defined( HAVE_UNISTD_H )
#include <unistd.h>
#endif

#include "libcpath_definitions.h"
#include "libcpath_libcerror.h"
#include "libcpath_libcsplit.h"
#include "libcpath_path.h"
#include "libcpath_system_string.h"

#if defined( WINAPI )
enum LIBCPATH_TYPES
{
	LIBCPATH_TYPE_ABSOLUTE,
	LIBCPATH_TYPE_DEVICE,
	LIBCPATH_TYPE_EXTENDED_LENGTH,
	LIBCPATH_TYPE_RELATIVE,
	LIBCPATH_TYPE_UNC
};

#else
enum LIBCPATH_TYPES
{
	LIBCPATH_TYPE_ABSOLUTE,
	LIBCPATH_TYPE_RELATIVE
};

#endif /* defined( WINAPI ) */

#if defined( WINAPI ) && ( WINVER <= 0x0500 )

/* Cross Windows safe version of CloseHandle
 * Returns TRUE if successful or FALSE on error
 */
BOOL libcpath_CloseHandle(
      HANDLE file_handle )
{
	FARPROC function       = NULL;
	HMODULE library_handle = NULL;
	BOOL result            = FALSE;

	if( file_handle == NULL )
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
		    (LPCSTR) "CloseHandle" );

	if( function != NULL )
	{
		result = function(
			  file_handle );
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

#if defined( WINAPI ) && ( WINVER <= 0x0500 )

/* Cross Windows safe version of SetCurrentDirectoryA
 * Returns TRUE if successful or FALSE on error
 */
BOOL libcpath_SetCurrentDirectoryA(
      LPCSTR path )
{
	FARPROC function       = NULL;
	HMODULE library_handle = NULL;
	BOOL result            = FALSE;

	if( path == NULL )
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
		    (LPCSTR) "SetCurrentDirectoryA" );

	if( function != NULL )
	{
		result = function(
			  path );
	}
	/* This call should be after using the function
	 * in most cases kernel32.dll will still be available after free
	 */
	if( FreeLibrary(
	     library_handle ) != TRUE )
	{
		libcpath_CloseHandle(
		 library_handle );

		return( FALSE );
	}
	return( result );
}

#endif /* defined( WINAPI ) && ( WINVER <= 0x0500 ) */

#if defined( WINAPI )

/* Changes the directory
 * This function uses the WINAPI function for Windows XP (0x0501) or later
 * or tries to dynamically call the function for Windows 2000 (0x0500) or earlier
 * Returns 1 if successful or -1 on error
 */
int libcpath_path_change_directory(
     const char *directory_name,
     libcerror_error_t **error )
{
	static char *function = "libcpath_path_change_directory";
	DWORD error_code      = 0;

	if( directory_name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid directory name.",
		 function );

		return( -1 );
	}
#if defined( WINAPI ) && ( WINVER <= 0x0500 )
	if( libcpath_SetCurrentDirectoryA(
	     directory_name ) == 0 )
#else
	if( SetCurrentDirectoryA(
	     directory_name ) == 0 )
#endif
	{
		error_code = GetLastError();

		libcerror_system_set_error(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 error_code,
		 "%s: unable to change directory.",
		 function );

		return( -1 );
	}
	return( 1 );
}

#elif defined( HAVE_CHDIR )

/* Changes the directory
 * This function uses the POSIX chdir function or equivalent
 * Returns 1 if successful or -1 on error
 */
int libcpath_path_change_directory(
     const char *directory_name,
     libcerror_error_t **error )
{
	static char *function = "libcpath_path_change_directory";

	if( directory_name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid directory name.",
		 function );

		return( -1 );
	}
	if( chdir(
	     directory_name ) != 0 )
	{
		libcerror_system_set_error(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 errno,
		 "%s: unable to change directory.",
		 function );

		return( -1 );
	}
	return( 1 );
}

#else
#error Missing change directory function
#endif

#if defined( WINAPI ) && ( WINVER <= 0x0500 )

/* Cross Windows safe version of GetCurrentDirectoryA
 * Returns the number of characters in the current directory string or 0 on error
 */
DWORD libcpath_GetCurrentDirectoryA(
       DWORD buffer_size,
       LPCSTR buffer )
{
	FARPROC function       = NULL;
	HMODULE library_handle = NULL;
	DWORD result           = 0;

	if( buffer == NULL )
	{
		return( 0 );
	}
	library_handle = LoadLibrary(
	                  _SYSTEM_STRING( "kernel32.dll" ) );

	if( library_handle == NULL )
	{
		return( 0 );
	}
	function = GetProcAddress(
		    library_handle,
		    (LPCSTR) "GetCurrentDirectoryA" );

	if( function != NULL )
	{
		result = function(
			  buffer_size,
			  buffer );
	}
	/* This call should be after using the function
	 * in most cases kernel32.dll will still be available after free
	 */
	if( FreeLibrary(
	     library_handle ) != TRUE )
	{
		libcpath_CloseHandle(
		 library_handle );

		return( 0 );
	}
	return( result );
}

#endif /* defined( WINAPI ) && ( WINVER <= 0x0500 ) */

#if defined( WINAPI )

/* Retrieves the current working directory
 * This function uses the WINAPI function for Windows XP (0x0501) or later
 * or tries to dynamically call the function for Windows 2000 (0x0500) or earlier
 * Returns 1 if successful or -1 on error
 */
int libcpath_path_get_current_working_directory(
     char **current_working_directory,
     size_t *current_working_directory_size,
     libcerror_error_t **error )
{
	static char *function                     = "libcpath_path_get_current_working_directory";
	DWORD safe_current_working_directory_size = 0;
	DWORD error_code                          = 0;

	if( current_working_directory == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid current working directory.",
		 function );

		return( -1 );
	}
	if( *current_working_directory != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid current working directory value already set.",
		 function );

		return( -1 );
	}
	if( current_working_directory_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid current working directory size.",
		 function );

		return( -1 );
	}
	safe_current_working_directory_size = GetCurrentDirectoryA(
	                                       0,
	                                       NULL );

	if( safe_current_working_directory_size == 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve current working directory size.",
		 function );

		goto on_error;
	}
	if( (size_t) safe_current_working_directory_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: current working directory size value out of bounds.",
		 function );

		goto on_error;
	}
	*current_working_directory_size = (size_t) safe_current_working_directory_size;

	*current_working_directory = narrow_string_allocate(
	                              *current_working_directory_size );

	if( *current_working_directory == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create current working directory.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *current_working_directory,
	     0,
	     sizeof( char ) * *current_working_directory_size ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear current working directory.",
		 function );

		goto on_error;
	}
#if defined( WINAPI ) && ( WINVER <= 0x0500 )
	if( libcpath_GetCurrentDirectoryA(
	     safe_current_working_directory_size,
	     *current_working_directory ) != ( safe_current_working_directory_size - 1 ) )
#else
	if( GetCurrentDirectoryA(
	     safe_current_working_directory_size,
	     *current_working_directory ) != ( safe_current_working_directory_size - 1 ) )
#endif
	{
		error_code = GetLastError();

		libcerror_system_set_error(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 error_code,
		 "%s: unable to retrieve current working directory.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( *current_working_directory != NULL )
	{
		memory_free(
		 *current_working_directory );

		*current_working_directory = NULL;
	}
	*current_working_directory_size = 0;

	return( -1 );
}

#elif defined( HAVE_GETCWD )

/* Retrieves the current working directory
 * This function uses the POSIX getcwd function or equivalent
 * Returns 1 if successful or -1 on error
 */
int libcpath_path_get_current_working_directory(
     char **current_working_directory,
     size_t *current_working_directory_size,
     libcerror_error_t **error )
{
	static char *function = "libcpath_path_get_current_working_directory";

	if( current_working_directory == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid current working directory.",
		 function );

		return( -1 );
	}
	if( *current_working_directory != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid current working directory value already set.",
		 function );

		return( -1 );
	}
	if( current_working_directory_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid current working directory size.",
		 function );

		return( -1 );
	}
	*current_working_directory_size = (size_t) PATH_MAX;

	*current_working_directory = narrow_string_allocate(
	                              *current_working_directory_size );

	if( *current_working_directory == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create current working directory.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *current_working_directory,
	     0,
	     sizeof( char ) * *current_working_directory_size ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear current working directory.",
		 function );

		goto on_error;
	}
	if( getcwd(
	     *current_working_directory,
	     *current_working_directory_size ) == NULL )
	{
		libcerror_system_set_error(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 errno,
		 "%s: unable to retrieve current working directory.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( *current_working_directory != NULL )
	{
		memory_free(
		 *current_working_directory );

		*current_working_directory = NULL;
	}
	*current_working_directory_size = 0;

	return( -1 );
}

#else
#error Missing get current working directory function
#endif

#if defined( WINAPI )

/* Determines the volume name and path type
 * Returns 1 if succesful or -1 on error
 */
int libcpath_path_get_volume_name_and_path_type(
     const char *path,
     size_t path_length,
     char **volume_name,
     size_t *volume_name_length,
     size_t *directory_name_index,
     uint8_t *path_type,
     libcerror_error_t **error )
{
	static char *function   = "libcpath_path_get_volume_name_and_path_type";
	size_t path_index       = 0;
	size_t share_name_index = 0;

	if( path == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid path.",
		 function );

		return( -1 );
	}
	if( path_length > (size_t) ( SSIZE_MAX - 1 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid path length value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( volume_name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid volume name.",
		 function );

		return( -1 );
	}
	if( volume_name_length == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid volume name length.",
		 function );

		return( -1 );
	}
	if( directory_name_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid directory name index.",
		 function );

		return( -1 );
	}
	if( path_type == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid path type.",
		 function );

		return( -1 );
	}
	if( ( path_length == 2 )
	 && ( path[ 1 ] == '\\' ) )
	{
		*path_type = LIBCPATH_TYPE_ABSOLUTE;
	}
	else if( path_length >= 2 )
	{
		/* Check if the path starts with a volume letter
		 */
		if( ( path[ 1 ] == ':' )
		 && ( ( ( path[ 0 ] >= 'A' )
		   && ( path[ 0 ] <= 'Z' ) )
		  || ( ( path[ 0 ] >= 'a' )
		   && ( path[ 0 ] <= 'z' ) ) ) )
		{
			*volume_name          = (char *) path;
			*volume_name_length   = 2;
			*directory_name_index = 2;

			if( ( path_length >= 3 )
			 && ( path[ 2 ] == '\\' ) )
			{
				*path_type             = LIBCPATH_TYPE_ABSOLUTE;
				*directory_name_index += 1;
			}
		}
		/* Check for special paths
		 * paths with prefix: \\
		 */
		else if( ( path[ 0 ] == '\\' )
		      && ( path[ 1 ] == '\\' ) )
		{
			/* Determine if the path is a special path
			 * device path prefix:          \\.\
			 * extended-length path prefix: \\?\
			 */
			if( ( path_length >= 4 )
			 && ( ( path[ 2 ] == '.' )
			   || ( path[ 2 ] == '?' ) )
			 && ( path[ 3 ] == '\\' ) )
			{
				if( path[ 2 ] == '.' )
				{
					*path_type = LIBCPATH_TYPE_DEVICE;
				}
				else
				{
					*path_type = LIBCPATH_TYPE_EXTENDED_LENGTH;
				}
				*directory_name_index = 4;
			}
			else
			{
				/* Determine the volume in an UNC path
				 * \\server\share
				 */
				for( share_name_index = 2;
				     share_name_index <= path_length;
				     share_name_index++ )
				{
					if( path[ share_name_index ] == '\\' )
					{
						break;
					}
				}
				if( share_name_index > path_length )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: invalid path - missing share name.",
					 function );

					return( -1 );
				}
				for( path_index = share_name_index + 1;
				     path_index <= path_length;
				     path_index++ )
				{
					if( path[ path_index ] == '\\' )
					{
						break;
					}
				}
				*path_type            = LIBCPATH_TYPE_UNC;
				*volume_name          = (char *) &( path[ 2 ] );
				*volume_name_length   = path_index - 2;
				*directory_name_index = path_index;
			}
		}
		/* Check for absolue paths
		 */
		else if( path[ 0 ] == (wchar_t) '\\' )
		{
			*path_type = LIBCPATH_TYPE_ABSOLUTE;
		}
	}
	return( 1 );
}

/* Retrieves the current working directory of a specific volume
 * Returns 1 if successful or -1 on error
 */
int libcpath_path_get_current_working_directory_by_volume(
     char *volume_name,
     size_t volume_name_length,
     char **current_working_directory,
     size_t *current_working_directory_size,
     libcerror_error_t **error )
{
	char *change_volume_name                     = NULL;
	char *current_volume_working_directory       = NULL;
	static char *function                        = "libcpath_path_get_current_working_directory_by_volume";
	size_t current_volume_working_directory_size = 0;
	int result                                   = 1;

	if( current_working_directory == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid current working directory.",
		 function );

		return( -1 );
	}
	if( current_working_directory_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid current working directory size.",
		 function );

		return( -1 );
	}
	/* If the path contains a volume name switch to that
	 * volume to determine the current directory
	 */
	if( volume_name != NULL )
	{
		if( libcpath_path_get_current_working_directory(
		     &current_volume_working_directory,
		     &current_volume_working_directory_size,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve current volume working directory.",
			 function );

			goto on_error;
		}
		change_volume_name = narrow_string_allocate(
		                      volume_name_length + 1 );

		if( change_volume_name == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_MEMORY,
			 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to create change volume name.",
			 function );

			goto on_error;
		}
		if( narrow_string_copy(
		     change_volume_name,
		     volume_name,
		     volume_name_length ) == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_MEMORY,
			 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
			 "%s: unable to set change volume name.",
			 function );

			goto on_error;
		}
		change_volume_name[ volume_name_length ] = 0;

		if( libcpath_path_change_directory(
		     change_volume_name,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to change current working directory.",
			 function );

			goto on_error;
		}
		memory_free(
		 change_volume_name );

		change_volume_name = NULL;
	}
	if( libcpath_path_get_current_working_directory(
	     current_working_directory,
	     current_working_directory_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve current directory.",
		 function );

		/* Make sure the current working directory has been changed
		 * back to its original value
		 */
		result = -1;
	}
	if( current_volume_working_directory != NULL )
	{
		if( libcpath_path_change_directory(
		     current_volume_working_directory,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to change current working directory.",
			 function );

			goto on_error;
		}
		memory_free(
		 current_volume_working_directory );

		current_volume_working_directory = NULL;
	}
	return( result );

on_error:
	if( change_volume_name != NULL )
	{
		memory_free(
		 change_volume_name );
	}
	if( current_volume_working_directory != NULL )
	{
		memory_free(
		 current_volume_working_directory );
	}
	if( *current_working_directory != NULL )
	{
		memory_free(
		 *current_working_directory );

		*current_working_directory = NULL;
	}
	*current_working_directory_size = 0;

	return( -1 );
}

/* Determines the volume name
 * Returns 1 if succesful or -1 on error
 */
int libcpath_path_get_volume_name(
     const char *path,
     size_t path_size,
     char **volume_name,
     size_t *volume_name_length,
     size_t *directory_name_index,
     libcerror_error_t **error )
{
	static char *function   = "libcpath_path_get_volume_name";
	size_t path_index       = 0;
	size_t share_name_index = 0;

	if( path == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid path.",
		 function );

		return( -1 );
	}
	if( path_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid path size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( volume_name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid volume name.",
		 function );

		return( -1 );
	}
	if( volume_name_length == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid volume name length.",
		 function );

		return( -1 );
	}
	if( directory_name_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid directory name index.",
		 function );

		return( -1 );
	}
/* TODO clean up code */
	if( path_size >= 3 )
	{
		if( path == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing path.",
			 function );

			return( -1 );
		}
		/* Check if the path starts with a volume letter
		 */
		if( ( path[ 1 ] == ':' )
		 && ( ( ( path[ 0 ] >= 'A' )
		   && ( path[ 0 ] <= 'Z' ) )
		  || ( ( path[ 0 ] >= 'a' )
		   && ( path[ 0 ] <= 'z' ) ) ) )
		{
			if( *volume_name == NULL )
			{
				*volume_name        = (char *) path;
				*volume_name_length = 2;
			}
			path_index = 2;

			if( path[ 2 ] == '\\' )
			{
				path_index++;
			}
			*directory_name_index = path_index;
		}
		/* Check for special paths
		 * paths with prefix: \\
		 */
		else if( ( path[ 0 ] == '\\' )
		      && ( path[ 1 ] == '\\' ) )
		{
			/* Ignore the following special paths
			 * device path prefix:          \\.\
			 */
			if( ( path_size >= 4 )
			 && ( path[ 2 ] == '.' )
			 && ( path[ 3 ] == '\\' ) )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
				 "%s: unsupported path.",
				 function );

				return( -1 );
			}
			else
			{
				/* Determine the volume in an UNC path
				 * \\server\share
				 */
				for( share_name_index = 2;
				     share_name_index < path_size;
				     share_name_index++ )
				{
					if( path[ share_name_index ] == '\\' )
					{
						break;
					}
				}
				if( share_name_index >= path_size )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: invalid path - missing share name.",
					 function );

					return( -1 );
				}
				for( path_index = share_name_index + 1;
				     path_index < path_size;
				     path_index++ )
				{
					if( path[ path_index ] == '\\' )
					{
						break;
					}
				}
				if( *volume_name == NULL )
				{
					*volume_name = (char *) &( path[ 2 ] );

					if( path_index == path_size )
					{
						*volume_name_length = path_index - 3;
					}
					else
					{
						*volume_name_length = path_index - 2;
					}
				}
				*directory_name_index = path_index;
			}
		}
	}
	return( 1 );
}

/* Determines the full path of the Windows path specified
 * The function uses the extended-length path format
 * (path with \\?\ prefix)
 *
 * Multiple successive \ not at the start of the path are combined into one
 *
 * Scenario's that are considered full paths:
 * Device path:			\\.\PhysicalDrive0
 * Extended-length path:	\\?\C:\directory\file.txt
 * Extended-length UNC path:	\\?\UNC\server\share\directory\file.txt
 *
 * Scenario's that are not considered full paths:
 * Local 'absolute' path:	\directory\file.txt
 *                       	\directory\\file.txt
 * Local 'relative' path:	..\directory\file.txt
 * Local 'relative' path:	.\directory\file.txt
 * Volume 'absolute' path:	C:\directory\file.txt
 *                              C:\..\directory\file.txt
 * Volume 'relative' path:	C:directory\file.txt
 * UNC path:			\\server\share\directory\file.txt
 *
 * TODO handle:
 * Volume device path:		\\.\C:
 * Volume file system path:	\\.\C:\
 *
 * Returns 1 if succesful or -1 on error
 */
int libcpath_path_get_full_path(
     const char *path,
     size_t path_length,
     char **full_path,
     size_t *full_path_size,
     libcerror_error_t **error )
{
	libcsplit_narrow_split_string_t *current_directory_split_string = NULL;
	libcsplit_narrow_split_string_t *path_split_string              = NULL;
	char *current_directory                                         = NULL;
	char *current_directory_string_segment                          = NULL;
	char *full_path_prefix                                          = NULL;
	char *last_used_path_string_segment                             = NULL;
	char *path_string_segment                                       = NULL;
	char *volume_name                                               = NULL;
	static char *function                                           = "libcpath_path_get_full_path";
	size_t current_directory_length                                 = 0;
	size_t current_directory_name_index                             = 0;
	size_t current_directory_size                                   = 0;
	size_t current_directory_string_segment_size                    = 0;
	size_t full_path_index                                          = 0;
	size_t full_path_prefix_length                                  = 0;
	size_t last_used_path_string_segment_size                       = 0;
	size_t path_directory_name_index                                = 0;
	size_t path_string_segment_size                                 = 0;
	size_t safe_full_path_size                                      = 0;
	size_t share_name_index                                         = 0;
	size_t volume_name_length                                       = 0;
	uint8_t path_type                                               = LIBCPATH_TYPE_RELATIVE;
	int current_directory_number_of_segments                        = 0;
	int current_directory_segment_index                             = 0;
	int last_used_path_segment_index                                = -1;
	int path_number_of_segments                                     = 0;
	int path_segment_index                                          = 0;

	if( path == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid path.",
		 function );

		return( -1 );
	}
	if( path_length == 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_ZERO_OR_LESS,
		 "%s: invalid path length is zero.",
		 function );

		return( -1 );
	}
	if( path_length > (size_t) ( SSIZE_MAX - 1 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid path length value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( full_path == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid full path.",
		 function );

		return( -1 );
	}
	if( *full_path != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid full path value already set.",
		 function );

		return( -1 );
	}
	if( full_path_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid full path size.",
		 function );

		return( -1 );
	}
	if( libcpath_path_get_volume_name_and_path_type(
	     path,
	     path_length,
	     &volume_name,
	     &volume_name_length,
	     &path_directory_name_index,
	     &path_type,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to determine volume name and path type.",
		 function );

		goto on_error;
	}
	/* If the path is a device path, an extended-length path or an UNC
	 * do not bother to lookup the current directory
	 */
	if( ( path_type != LIBCPATH_TYPE_DEVICE )
	 && ( path_type != LIBCPATH_TYPE_EXTENDED_LENGTH )
	 && ( path_type != LIBCPATH_TYPE_UNC ) )
	{
		if( libcpath_path_get_current_working_directory_by_volume(
		     volume_name,
		     volume_name_length,
		     &current_directory,
		     &current_directory_size,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve current directory by volume.",
			 function );

			goto on_error;
		}
		/* Determine the volume name using the current directory if necessary
		 */
		if( libcpath_path_get_volume_name(
		     current_directory,
		     current_directory_size,
		     &volume_name,
		     &volume_name_length,
		     &current_directory_name_index,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve current directory by volume.",
			 function );

			goto on_error;
		}
	}
	if( ( current_directory != NULL )
	 && ( current_directory_name_index < current_directory_size ) )
	{
		current_directory_length = narrow_string_length(
		                            &( current_directory[ current_directory_name_index ] ) );

		if( libcsplit_narrow_string_split(
		     &( current_directory[ current_directory_name_index ] ),
		     current_directory_length + 1,
		     '\\',
		     &current_directory_split_string,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to split current directory.",
			 function );

			goto on_error;
		}
	}
	if( libcsplit_narrow_string_split(
	     &( path[ path_directory_name_index ] ),
	     path_length - path_directory_name_index + 1,
	     '\\',
	     &path_split_string,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to split path.",
		 function );

		goto on_error;
	}
	/* The size of the full path consists of:
	 * the size of the prefix (\\?\ or \\.\)
	 */
	safe_full_path_size = 4;

	/* If the path contains a volume name
	 * the length of the volume name
	 * a directory separator
	 */
	if( volume_name != NULL )
	{
		safe_full_path_size += volume_name_length + 1;
	}
	/* If the path contains an UNC path
	 * add the size of the UNC\ prefix
	 */
	if( share_name_index > 0 )
	{
		safe_full_path_size += 4;
	}
	/* If the path is relative
	 * add the size of the current directory
	 * a directory separator, if necessary
	 */
	if( ( path_type == LIBCPATH_TYPE_RELATIVE )
	 && ( current_directory_name_index < current_directory_size ) )
	{
		safe_full_path_size += ( current_directory_size - ( current_directory_name_index + 1 ) );

		if( ( current_directory_size >= 2 )
		 && ( current_directory[ current_directory_size - 2 ] != '\\' ) )
		{
			safe_full_path_size += 1;
		}
	}
	if( current_directory_split_string != NULL )
	{
		if( libcsplit_narrow_split_string_get_number_of_segments(
		     current_directory_split_string,
		     &current_directory_number_of_segments,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve number of current directory string segments.",
			 function );

			goto on_error;
		}
		current_directory_segment_index = current_directory_number_of_segments - 1;
	}
	if( libcsplit_narrow_split_string_get_number_of_segments(
	     path_split_string,
	     &path_number_of_segments,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of path string segments.",
		 function );

		goto on_error;
	}
	for( path_segment_index = 0;
	     path_segment_index < path_number_of_segments;
	     path_segment_index++ )
	{
		if( libcsplit_narrow_split_string_get_segment_by_index(
		     path_split_string,
		     path_segment_index,
		     &path_string_segment,
		     &path_string_segment_size,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve path string segment: %d.",
			 function,
			 path_segment_index );

			goto on_error;
		}
		if( path_string_segment == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing path string segment: %d.",
			 function,
			 path_segment_index );

			goto on_error;
		}
		/* If the path is .. reverse the current path by one directory
		 */
		if( ( path_string_segment_size == 3 )
		 && ( path_string_segment[ 0 ] == '.' )
		 && ( path_string_segment[ 1 ] == '.' ) )
		{
			if( ( current_directory_split_string != NULL )
			 && ( last_used_path_segment_index == -1 ) )
			{
				if( libcsplit_narrow_split_string_get_segment_by_index(
				     current_directory_split_string,
				     current_directory_segment_index,
				     &current_directory_string_segment,
				     &current_directory_string_segment_size,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve current directory string segment: %d.",
					 function,
					 current_directory_segment_index );

					goto on_error;
				}
				if( current_directory_string_segment == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: missing current directory string segment: %d.",
					 function,
					 current_directory_segment_index );

					goto on_error;
				}
				/* Remove the size of the parent directory name and a directory separator
				 * Note that the size includes the end of string character
				 */
				safe_full_path_size -= current_directory_string_segment_size;

				if( libcsplit_narrow_split_string_set_segment_by_index(
				     current_directory_split_string,
				     current_directory_segment_index,
				     NULL,
				     0,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to set current directory string segment: %d.",
					 function,
					 current_directory_segment_index );

					goto on_error;
				}
				current_directory_segment_index--;
			}
			else if( last_used_path_segment_index >= 0 )
			{
				if( libcsplit_narrow_split_string_get_segment_by_index(
				     path_split_string,
				     last_used_path_segment_index,
				     &last_used_path_string_segment,
				     &last_used_path_string_segment_size,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve last used path string segment: %d.",
					 function,
					 last_used_path_segment_index );

					goto on_error;
				}
				if( last_used_path_string_segment == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: missing last used path string string segment: %d.",
					 function,
					 last_used_path_segment_index );

					goto on_error;
				}
				/* Remove the size of the parent directory name and a directory separator
				 * Note that the size includes the end of string character
				 */
				safe_full_path_size -= last_used_path_string_segment_size;

				if( libcsplit_narrow_split_string_set_segment_by_index(
				     path_split_string,
				     last_used_path_segment_index,
				     NULL,
				     0,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to set path string segment: %d.",
					 function,
					 last_used_path_segment_index );

					goto on_error;
				}
				/* Find the previous path split value that contains a name
				 */
				while( last_used_path_segment_index > 0 )
				{
					last_used_path_segment_index--;

					if( libcsplit_narrow_split_string_get_segment_by_index(
					     path_split_string,
					     last_used_path_segment_index,
					     &last_used_path_string_segment,
					     &last_used_path_string_segment_size,
					     error ) != 1 )
					{
						libcerror_error_set(
						 error,
						 LIBCERROR_ERROR_DOMAIN_RUNTIME,
						 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
						 "%s: unable to retrieve last used path string segment: %d.",
						 function,
						 last_used_path_segment_index );

						goto on_error;
					}
					if( last_used_path_string_segment_size != 0 )
					{
						break;
					}
				}
			}
			if( libcsplit_narrow_split_string_set_segment_by_index(
			     path_split_string,
			     path_segment_index,
			     NULL,
			     0,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set path string segment: %d.",
				 function,
				 path_segment_index );

				goto on_error;
			}
		}
		/* If the path is . ignore the entry
		 */
		else if( ( path_string_segment_size == 2 )
		      && ( path_string_segment[ 0 ] == '.' ) )
		{
			if( libcsplit_narrow_split_string_set_segment_by_index(
			     path_split_string,
			     path_segment_index,
			     NULL,
			     0,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set path string segment: %d.",
				 function,
				 path_segment_index );

				goto on_error;
			}
		}
		/* If the path is empty ignore the entry
		 */
		else if( path_string_segment_size <= 1 )
		{
			if( libcsplit_narrow_split_string_set_segment_by_index(
			     path_split_string,
			     path_segment_index,
			     NULL,
			     0,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set path string segment: %d.",
				 function,
				 path_segment_index );

				goto on_error;
			}
		}
		else
		{
			/* Add the size of the directory or file name and a directory separator
			 * Note that the size includes the end of string character
			 */
			safe_full_path_size += path_string_segment_size;

			last_used_path_segment_index = path_segment_index;
		}
	}
	/* Note that the last path separator serves as the end of string
	 */
	full_path_index = 0;

	*full_path = narrow_string_allocate(
	              safe_full_path_size );

	if( *full_path == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create full path.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *full_path,
	     0,
	     sizeof( wchar_t ) * safe_full_path_size ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear full path.",
		 function );

		goto on_error;
	}
	*full_path_size = safe_full_path_size;

	if( path_type == LIBCPATH_TYPE_DEVICE )
	{
		full_path_prefix        = "\\\\.\\";
		full_path_prefix_length = 4;
	}
	else
	{
		full_path_prefix        = "\\\\?\\";
		full_path_prefix_length = 4;
	}
	if( narrow_string_copy(
	     &( ( *full_path )[ full_path_index ] ),
	     full_path_prefix,
	     full_path_prefix_length ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set prefix in full path.",
		 function );

		goto on_error;
	}
	full_path_index += full_path_prefix_length;

	/* If there is a share name the path is an UNC path
	 */
	if( share_name_index > 0 )
	{
		if( narrow_string_copy(
		     &( ( *full_path )[ full_path_index ] ),
		     "UNC\\",
		     4 ) == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set UNC\\ prefix in full path.",
			 function );

			goto on_error;
		}
		full_path_index += 4;
	}
	if( volume_name != NULL )
	{
		if( narrow_string_copy(
		     &( ( *full_path )[ full_path_index ] ),
		     volume_name,
		     volume_name_length ) == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set volume name in full path.",
			 function );

			goto on_error;
		}
		full_path_index += volume_name_length;

		( *full_path )[ full_path_index ] = '\\';

		full_path_index += 1;
	}
	/* If the path is relative
	 * add the current directory elements
	 */
	if( ( path_type == LIBCPATH_TYPE_RELATIVE )
	 && ( current_directory_split_string != NULL ) )
	{
		for( current_directory_segment_index = 0;
		     current_directory_segment_index < current_directory_number_of_segments;
		     current_directory_segment_index++ )
		{
			if( libcsplit_narrow_split_string_get_segment_by_index(
			     current_directory_split_string,
			     current_directory_segment_index,
			     &current_directory_string_segment,
			     &current_directory_string_segment_size,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve current directory string segment: %d.",
				 function,
				 current_directory_segment_index );

				goto on_error;
			}
			if( current_directory_string_segment_size != 0 )
			{
				if( current_directory_string_segment == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: missing current directory string segment: %d.",
					 function,
					 current_directory_segment_index );

					goto on_error;
				}
				if( narrow_string_copy(
				     &( ( *full_path )[ full_path_index ] ),
				     current_directory_string_segment,
				     current_directory_string_segment_size - 1 ) == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to set current directory split value: %d in full path.",
					 function,
					 current_directory_segment_index );

					goto on_error;
				}
				full_path_index += current_directory_string_segment_size - 1;

				( *full_path )[ full_path_index ] = '\\';

				full_path_index += 1;
			}
		}
	}
	for( path_segment_index = 0;
	     path_segment_index < path_number_of_segments;
	     path_segment_index++ )
	{
		if( libcsplit_narrow_split_string_get_segment_by_index(
		     path_split_string,
		     path_segment_index,
		     &path_string_segment,
		     &path_string_segment_size,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve path string segment: %d.",
			 function,
			 path_segment_index );

			goto on_error;
		}
		if( path_string_segment_size != 0 )
		{
			if( path_string_segment == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
				 "%s: missing path string segment: %d.",
				 function,
				 path_segment_index );

				goto on_error;
			}
			if( narrow_string_copy(
			     &( ( *full_path )[ full_path_index ] ),
			     path_string_segment,
			     path_string_segment_size - 1 ) == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set path split value: %d in full path.",
				 function,
				 path_segment_index );

				goto on_error;
			}
			full_path_index += path_string_segment_size - 1;

			( *full_path )[ full_path_index ] = '\\';

			full_path_index += 1;
		}
	}
	( *full_path )[ full_path_index - 1 ] = 0;

	if( libcsplit_narrow_split_string_free(
	     &path_split_string,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
		 "%s: unable to free path split string.",
		 function );

		goto on_error;
	}
	if( current_directory_split_string != NULL )
	{
		if( libcsplit_narrow_split_string_free(
		     &current_directory_split_string,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free current directory split string.",
			 function );

			goto on_error;
		}
	}
	if( current_directory != NULL )
	{
		memory_free(
		 current_directory );
	}
	return( 1 );

on_error:
	if( *full_path != NULL )
	{
		memory_free(
		 *full_path );

		*full_path = NULL;
	}
	*full_path_size = 0;

	if( path_split_string != NULL )
	{
		libcsplit_narrow_split_string_free(
		 &path_split_string,
		 NULL );
	}
	if( current_directory_split_string != NULL )
	{
		libcsplit_narrow_split_string_free(
		 &current_directory_split_string,
		 NULL );
	}
	if( current_directory != NULL )
	{
		memory_free(
		 current_directory );
	}
	return( -1 );
}

#else

/* Determines the full path of the POSIX path specified
 * Multiple successive / are combined into one
 *
 * Scenarios:
 * /home/user/file.txt
 * /home/user//file.txt
 * /home/user/../user/file.txt
 * /../home/user/file.txt
 * user/../user/file.txt
 *
 * Returns 1 if succesful or -1 on error
 */
int libcpath_path_get_full_path(
     const char *path,
     size_t path_length,
     char **full_path,
     size_t *full_path_size,
     libcerror_error_t **error )
{
	libcsplit_narrow_split_string_t *current_directory_split_string = NULL;
	libcsplit_narrow_split_string_t *path_split_string              = NULL;
	char *current_directory                                         = NULL;
	char *current_directory_string_segment                          = NULL;
	char *last_used_path_string_segment                             = NULL;
	char *path_string_segment                                       = NULL;
	static char *function                                           = "libcpath_path_get_full_path";
	size_t current_directory_length                                 = 0;
	size_t current_directory_size                                   = 0;
	size_t current_directory_string_segment_size                    = 0;
	size_t full_path_index                                          = 0;
	size_t last_used_path_string_segment_size                       = 0;
	size_t path_string_segment_size                                 = 0;
	size_t safe_full_path_size                                      = 0;
	uint8_t path_type                                               = LIBCPATH_TYPE_RELATIVE;
	int current_directory_number_of_segments                        = 0;
	int current_directory_segment_index                             = 0;
	int last_used_path_segment_index                                = -1;
	int path_number_of_segments                                     = 0;
	int path_segment_index                                          = 0;

	if( path == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid path.",
		 function );

		return( -1 );
	}
	if( path_length == 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_ZERO_OR_LESS,
		 "%s: invalid path length is zero.",
		 function );

		return( -1 );
	}
	if( path_length > (size_t) ( SSIZE_MAX - 1 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid path length value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( full_path == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid full path.",
		 function );

		return( -1 );
	}
	if( *full_path != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid full path value already set.",
		 function );

		return( -1 );
	}
	if( full_path_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid full path size.",
		 function );

		return( -1 );
	}
	if( path[ 0 ] == '/' )
	{
		path_type = LIBCPATH_TYPE_ABSOLUTE;
	}
	else
	{
		if( libcpath_path_get_current_working_directory(
		     &current_directory,
		     &current_directory_size,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve current directory.",
			 function );

			goto on_error;
		}
	}
	if( current_directory != NULL )
	{
		current_directory_length = narrow_string_length(
		                            current_directory );

		if( libcsplit_narrow_string_split(
		     current_directory,
		     current_directory_length + 1,
		     '/',
		     &current_directory_split_string,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to split current directory.",
			 function );

			goto on_error;
		}
	}
	if( libcsplit_narrow_string_split(
	     path,
	     path_length + 1,
	     '/',
	     &path_split_string,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to split path.",
		 function );

		goto on_error;
	}
	/* Determine the full path size
	 */
	/* If the path is absolute
	 * a directory separator
	 */
	if( path_type == LIBCPATH_TYPE_ABSOLUTE )
	{
		safe_full_path_size = 1;
	}
	/* If the path is relative
	 * add the size of the current directory
	 * a directory separator, if necessary
	 */
	else if( path_type == LIBCPATH_TYPE_RELATIVE )
	{
		if( current_directory == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing current directory.",
			 function );

			goto on_error;
		}
		safe_full_path_size = current_directory_size - 1;

		if( ( current_directory_size >= 2 )
		 && ( current_directory[ current_directory_size - 2 ] != '/' ) )
		{
			safe_full_path_size++;
		}
	}
	if( current_directory_split_string != NULL )
	{
		if( libcsplit_narrow_split_string_get_number_of_segments(
		     current_directory_split_string,
		     &current_directory_number_of_segments,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve number of current directory string segments.",
			 function );

			goto on_error;
		}
		current_directory_segment_index = current_directory_number_of_segments - 1;
	}
	if( libcsplit_narrow_split_string_get_number_of_segments(
	     path_split_string,
	     &path_number_of_segments,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of path string segments.",
		 function );

		goto on_error;
	}
	for( path_segment_index = 0;
	     path_segment_index < path_number_of_segments;
	     path_segment_index++ )
	{
		if( libcsplit_narrow_split_string_get_segment_by_index(
		     path_split_string,
		     path_segment_index,
		     &path_string_segment,
		     &path_string_segment_size,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve path string segment: %d.",
			 function,
			 path_segment_index );

			goto on_error;
		}
		if( path_string_segment == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing path string segment: %d.",
			 function,
			 path_segment_index );

			goto on_error;
		}
		/* If the path is .. reverse the current path by one directory
		 */
		if( ( path_string_segment_size == 3 )
		 && ( path_string_segment[ 0 ] == '.' )
		 && ( path_string_segment[ 1 ] == '.' ) )
		{
			if( ( current_directory_split_string != NULL )
			 && ( last_used_path_segment_index == -1 ) )
			{
				if( libcsplit_narrow_split_string_get_segment_by_index(
				     current_directory_split_string,
				     current_directory_segment_index,
				     &current_directory_string_segment,
				     &current_directory_string_segment_size,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve current directory string segment: %d.",
					 function,
					 current_directory_segment_index );

					goto on_error;
				}
				if( current_directory_string_segment == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: missing current directory string segment: %d.",
					 function,
					 current_directory_segment_index );

					goto on_error;
				}
				/* Remove the size of the parent directory name and a directory separator
				 * Note that the size includes the end of string character
				 */
				safe_full_path_size -= current_directory_string_segment_size;

				if( libcsplit_narrow_split_string_set_segment_by_index(
				     current_directory_split_string,
				     current_directory_segment_index,
				     NULL,
				     0,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to set current directory string segment: %d.",
					 function,
					 current_directory_segment_index );

					goto on_error;
				}
				current_directory_segment_index--;
			}
			else if( last_used_path_segment_index >= 0 )
			{
				if( libcsplit_narrow_split_string_get_segment_by_index(
				     path_split_string,
				     last_used_path_segment_index,
				     &last_used_path_string_segment,
				     &last_used_path_string_segment_size,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve last used path string segment: %d.",
					 function,
					 last_used_path_segment_index );

					goto on_error;
				}
				if( last_used_path_string_segment == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: missing last used path string string segment: %d.",
					 function,
					 last_used_path_segment_index );

					goto on_error;
				}
				/* Remove the size of the parent directory name and a directory separator
				 * Note that the size includes the end of string character
				 */
				safe_full_path_size -= last_used_path_string_segment_size;

				if( libcsplit_narrow_split_string_set_segment_by_index(
				     path_split_string,
				     last_used_path_segment_index,
				     NULL,
				     0,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to set path string segment: %d.",
					 function,
					 last_used_path_segment_index );

					goto on_error;
				}
				/* Find the previous path split value that contains a name
				 */
				while( last_used_path_segment_index > 0 )
				{
					last_used_path_segment_index--;

					if( libcsplit_narrow_split_string_get_segment_by_index(
					     path_split_string,
					     last_used_path_segment_index,
					     &last_used_path_string_segment,
					     &last_used_path_string_segment_size,
					     error ) != 1 )
					{
						libcerror_error_set(
						 error,
						 LIBCERROR_ERROR_DOMAIN_RUNTIME,
						 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
						 "%s: unable to retrieve last used path string segment: %d.",
						 function,
						 last_used_path_segment_index );

						goto on_error;
					}
					if( last_used_path_string_segment_size != 0 )
					{
						break;
					}
				}
			}
			if( libcsplit_narrow_split_string_set_segment_by_index(
			     path_split_string,
			     path_segment_index,
			     NULL,
			     0,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set path string segment: %d.",
				 function,
				 path_segment_index );

				goto on_error;
			}
		}
		/* If the path is . ignore the entry
		 */
		else if( ( path_string_segment_size == 2 )
		      && ( path_string_segment[ 0 ] == '.' ) )
		{
			if( libcsplit_narrow_split_string_set_segment_by_index(
			     path_split_string,
			     path_segment_index,
			     NULL,
			     0,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set path string segment: %d.",
				 function,
				 path_segment_index );

				goto on_error;
			}
		}
		/* If the path is empty ignore the entry
		 */
		else if( path_string_segment_size <= 1 )
		{
			if( libcsplit_narrow_split_string_set_segment_by_index(
			     path_split_string,
			     path_segment_index,
			     NULL,
			     0,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set path string segment: %d.",
				 function,
				 path_segment_index );

				goto on_error;
			}
		}
		else
		{
			/* Add the size of the directory or file name and a directory separator
			 * Note that the size includes the end of string character
			 */
			safe_full_path_size += path_string_segment_size;

			last_used_path_segment_index = path_segment_index;
		}
	}
	/* Note that the last path separator serves as the end of string
	 */
	full_path_index = 0;

	*full_path = narrow_string_allocate(
	              safe_full_path_size );

	if( *full_path == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create full path.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *full_path,
	     0,
	     sizeof( char ) * safe_full_path_size ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear full path.",
		 function );

		goto on_error;
	}
	*full_path_size = safe_full_path_size;

	if( path_type == LIBCPATH_TYPE_ABSOLUTE )
	{
		( *full_path )[ full_path_index ] = '/';

		full_path_index += 1;
	}
	/* If the path is relative
	 * add the current directory elements
	 */
	if( ( path_type == LIBCPATH_TYPE_RELATIVE )
	 && ( current_directory_split_string != NULL ) )
	{
		for( current_directory_segment_index = 0;
		     current_directory_segment_index < current_directory_number_of_segments;
		     current_directory_segment_index++ )
		{
			if( libcsplit_narrow_split_string_get_segment_by_index(
			     current_directory_split_string,
			     current_directory_segment_index,
			     &current_directory_string_segment,
			     &current_directory_string_segment_size,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve current directory string segment: %d.",
				 function,
				 current_directory_segment_index );

				goto on_error;
			}
			if( current_directory_string_segment_size != 0 )
			{
				if( current_directory_string_segment == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: missing current directory string segment: %d.",
					 function,
					 current_directory_segment_index );

					goto on_error;
				}
				if( narrow_string_copy(
				     &( ( *full_path )[ full_path_index ] ),
				     current_directory_string_segment,
				     current_directory_string_segment_size - 1 ) == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to set current directory split value: %d in full path.",
					 function,
					 current_directory_segment_index );

					goto on_error;
				}
				full_path_index += current_directory_string_segment_size - 1;

				( *full_path )[ full_path_index ] = '/';

				full_path_index += 1;
			}
		}
	}
	for( path_segment_index = 0;
	     path_segment_index < path_number_of_segments;
	     path_segment_index++ )
	{
		if( libcsplit_narrow_split_string_get_segment_by_index(
		     path_split_string,
		     path_segment_index,
		     &path_string_segment,
		     &path_string_segment_size,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve path string segment: %d.",
			 function,
			 path_segment_index );

			goto on_error;
		}
		if( path_string_segment_size != 0 )
		{
			if( path_string_segment == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
				 "%s: missing path string segment: %d.",
				 function,
				 path_segment_index );

				goto on_error;
			}
			if( narrow_string_copy(
			     &( ( *full_path )[ full_path_index ] ),
			     path_string_segment,
			     path_string_segment_size - 1 ) == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set path split value: %d in full path.",
				 function,
				 path_segment_index );

				goto on_error;
			}
			full_path_index += path_string_segment_size - 1;

			( *full_path )[ full_path_index ] = '/';

			full_path_index += 1;
		}
	}
	( *full_path )[ full_path_index - 1 ] = 0;

	if( libcsplit_narrow_split_string_free(
	     &path_split_string,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
		 "%s: unable to free path split string.",
		 function );

		goto on_error;
	}
	if( current_directory_split_string != NULL )
	{
		if( libcsplit_narrow_split_string_free(
		     &current_directory_split_string,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free current directory split string.",
			 function );

			goto on_error;
		}
	}
	if( current_directory != NULL )
	{
		memory_free(
		 current_directory );
	}
	return( 1 );

on_error:
	if( *full_path != NULL )
	{
		memory_free(
		 *full_path );

		*full_path = NULL;
	}
	*full_path_size = 0;

	if( path_split_string != NULL )
	{
		libcsplit_narrow_split_string_free(
		 &path_split_string,
		 NULL );
	}
	if( current_directory_split_string != NULL )
	{
		libcsplit_narrow_split_string_free(
		 &current_directory_split_string,
		 NULL );
	}
	if( current_directory != NULL )
	{
		memory_free(
		 current_directory );
	}
	return( -1 );
}

#endif /* defined( WINAPI ) */

/* Retrieves a sanitized version of the filename
 * Returns 1 if successful or -1 on error
 */
int libcpath_path_get_sanitized_filename(
     const char *filename,
     size_t filename_size,
     char **sanitized_filename,
     size_t *sanitized_filename_size,
     libcerror_error_t **error )
{
	static char *function = "libcpath_path_get_sanitized_filename";

	if( sanitized_filename == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid sanitized filename.",
		 function );

		return( -1 );
	}
	if( *sanitized_filename != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid sanitized filename value already set.",
		 function );

		return( -1 );
	}
	if( sanitized_filename_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid sanitized filename size.",
		 function );

		return( -1 );
	}
/* TODO implement */
	return( -1 );
}

/* Retrieves a sanitized version of the path
 * Returns 1 if successful or -1 on error
 */
int libcpath_path_get_sanitized_path(
     const char *path,
     size_t path_size,
     char **sanitized_path,
     size_t *sanitized_path_size,
     libcerror_error_t **error )
{
	static char *function = "libcpath_path_get_sanitized_path";

	if( sanitized_path == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid sanitized path.",
		 function );

		return( -1 );
	}
	if( *sanitized_path != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid sanitized path value already set.",
		 function );

		return( -1 );
	}
	if( sanitized_path_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid sanitized path size.",
		 function );

		return( -1 );
	}
/* TODO implement */
	return( -1 );
}

/* Combines the directory name and filename into a path
 * Returns 1 if successful or -1 on error
 */
int libcpath_path_join(
     char **path,
     size_t *path_size,
     const char *directory_name,
     size_t directory_name_length,
     const char *filename,
     size_t filename_length,
     libcerror_error_t **error )
{
	static char *function = "libcpath_path_join";
	size_t filename_index = 0;
	size_t path_index     = 0;

	if( path == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid path.",
		 function );

		return( -1 );
	}
	if( *path != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid path value already set.",
		 function );

		return( -1 );
	}
	if( path_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid path size.",
		 function );

		return( -1 );
	}
	if( directory_name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid directory name.",
		 function );

		return( -1 );
	}
	if( directory_name_length > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid directory name length value exceeds maximum.",
		 function );

		return( -1 );
	}
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
	if( filename_length > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid filename length value exceeds maximum.",
		 function );

		return( -1 );
	}
/* TODO strip other patterns like /./ */
	while( directory_name_length > 0 )
	{
		if( directory_name[ directory_name_length - 1 ] != (char) LIBCPATH_SEPARATOR )
		{
			break;
		}
		directory_name_length--;
	}
	while( filename_length > 0 )
	{
		if( filename[ filename_index ] != (char) LIBCPATH_SEPARATOR )
		{
			break;
		}
		filename_index++;
		filename_length--;
	}
	*path_size = directory_name_length + filename_length + 2;

	*path = narrow_string_allocate(
	         *path_size );

	if( *path == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create path.",
		 function );

		goto on_error;
	}
	if( narrow_string_copy(
	     *path,
	     directory_name,
	     directory_name_length ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
		 "%s: unable to copy directory name to path.",
		 function );

		goto on_error;
	}
	path_index = directory_name_length;

	( *path )[ path_index++ ] = (char) LIBCPATH_SEPARATOR;

	if( narrow_string_copy(
	     &( ( *path )[ path_index ] ),
	     &( filename[ filename_index ] ),
	     filename_length ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
		 "%s: unable to copy filename to path.",
		 function );

		goto on_error;
	}
	path_index += filename_length;

	( *path )[ path_index ] = 0;

	return( 1 );

on_error:
	if( *path != NULL )
	{
		memory_free(
		 path );

		*path = NULL;
	}
	*path_size = 0;

	return( -1 );
}

#if defined( WINAPI ) && ( WINVER <= 0x0500 )

/* Cross Windows safe version of CreateDirectoryA
 * Returns TRUE if successful or FALSE on error
 */
BOOL libcpath_CreateDirectoryA(
      LPCSTR path,
      SECURITY_ATTRIBUTES *security_attributes )
{
	FARPROC function       = NULL;
	HMODULE library_handle = NULL;
	BOOL result            = FALSE;

	if( path == NULL )
	{
		return( 0 );
	}
	library_handle = LoadLibrary(
	                  _SYSTEM_STRING( "kernel32.dll" ) );

	if( library_handle == NULL )
	{
		return( 0 );
	}
	function = GetProcAddress(
		    library_handle,
		    (LPCSTR) "CreateDirectoryA" );

	if( function != NULL )
	{
		result = function(
			  path,
			  security_attributes );
	}
	/* This call should be after using the function
	 * in most cases kernel32.dll will still be available after free
	 */
	if( FreeLibrary(
	     library_handle ) != TRUE )
	{
		libcpath_CloseHandle(
		 library_handle );

		return( 0 );
	}
	return( result );
}

#endif /* defined( WINAPI ) && ( WINVER <= 0x0500 ) */

#if defined( WINAPI )

/* Makes the directory
 * This function uses the WINAPI function for Windows XP (0x0501) or later
 * or tries to dynamically call the function for Windows 2000 (0x0500) or earlier
 * Returns 1 if successful or -1 on error
 */
int libcpath_path_make_directory(
     const char *directory_name,
     libcerror_error_t **error )
{
	static char *function = "libcpath_path_make_directory";
	DWORD error_code      = 0;

	if( directory_name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid directory name.",
		 function );

		return( -1 );
	}
#if defined( WINAPI ) && ( WINVER <= 0x0500 )
	if( libcpath_CreateDirectoryA(
	     directory_name,
	     NULL ) == 0 )
#else
	if( CreateDirectoryA(
	     directory_name,
	     NULL ) == 0 )
#endif
	{
		error_code = GetLastError();

		libcerror_system_set_error(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 error_code,
		 "%s: unable to make directory.",
		 function );

		return( -1 );
	}
	return( 1 );
}

#elif defined( HAVE_MKDIR )

/* Makes the directory
 * This function uses the POSIX mkdir function or equivalent
 * Returns 1 if successful or -1 on error
 */
int libcpath_path_make_directory(
     const char *directory_name,
     libcerror_error_t **error )
{
	static char *function = "libcpath_path_make_directory";

	if( directory_name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid directory name.",
		 function );

		return( -1 );
	}
	if( mkdir(
	     directory_name,
	     0755 ) != 0 )
	{
		libcerror_system_set_error(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 errno,
		 "%s: unable to make directory.",
		 function );

		return( -1 );
	}

	return( 1 );
}

#else
#error Missing make directory function
#endif

/* Sanitizes the path
 * Returns 1 if successful or -1 on error
 */
int libcpath_path_sanitize(
     char *path,
     size_t *path_size,
     libcerror_error_t **error )
{
	static char *function = "libcpath_path_sanitize";
	size_t path_index     = 0;

	if( path == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid path.",
		 function );

		return( -1 );
	}
	if( path_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid path size.",
		 function );

		return( -1 );
	}
	if( *path_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid path size value exceeds maximum.",
		 function );

		return( -1 );
	}
#if defined( WINAPI ) || defined( __CYGWIN__ )
	if( *path_size > 32767 )
	{
		path[ 32766 ] = 0;

		*path_size = 32767;
	}
#endif
	for( path_index = 0;
	     path_index < *path_size;
	     path_index++ )
	{
		if( path[ path_index ] == 0x00 )
		{
			break;
		}
		else if( ( path[ path_index ] >= 0x01 )
		      && ( path[ path_index ] <= 0x1f ) )
		{
			path[ path_index ] = '_';
		}
#if defined( WINAPI )
		else if( path[ path_index ] == '/' )
#else
		else if( path[ path_index ] == '\\' )
#endif
		{
			path[ path_index ] = '_';
		}
		else if( ( path[ path_index ] == '!' )
		      || ( path[ path_index ] == '$' )
		      || ( path[ path_index ] == '%' )
		      || ( path[ path_index ] == '&' )
		      || ( path[ path_index ] == '*' )
		      || ( path[ path_index ] == '+' )
		      || ( path[ path_index ] == ':' )
		      || ( path[ path_index ] == ';' )
		      || ( path[ path_index ] == '<' )
		      || ( path[ path_index ] == '>' )
		      || ( path[ path_index ] == '?' )
		      || ( path[ path_index ] == '@' )
		      || ( path[ path_index ] == '|' )
		      || ( path[ path_index ] == '~' )
		      || ( path[ path_index ] == 0x7f ) )
		{
			path[ path_index ] = '_';
		}
	}
	return( 1 );
}

/* Sanitizes the filename
 * Returns 1 if successful or -1 on error
 */
int libcpath_path_sanitize_filename(
     char *filename,
     size_t *filename_size,
     libcerror_error_t **error )
{
	static char *function = "libcpath_path_sanitize_filename";
	size_t filename_index = 0;

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
	if( filename_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid filename size.",
		 function );

		return( -1 );
	}
	if( *filename_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid filename size value exceeds maximum.",
		 function );

		return( -1 );
	}
#if defined( WINAPI ) || defined( __CYGWIN__ )
	if( *filename_size > 256 )
	{
		filename[ 255 ] = 0;

		*filename_size = 256;
	}
#endif
	for( filename_index = 0;
	     filename_index < *filename_size;
	     filename_index++ )
	{
		if( filename[ filename_index ] == 0x00 )
		{
			break;
		}
		else if( ( filename[ filename_index ] >= 0x01 )
		      && ( filename[ filename_index ] <= 0x1f ) )
		{
			filename[ filename_index ] = '_';
		}
		else if( ( filename[ filename_index ] == '/' )
		      || ( filename[ filename_index ] == '\\' ) )
		{
			filename[ filename_index ] = '_';
		}
		else if( ( filename[ filename_index ] == '!' )
		      || ( filename[ filename_index ] == '$' )
		      || ( filename[ filename_index ] == '%' )
		      || ( filename[ filename_index ] == '&' )
		      || ( filename[ filename_index ] == '*' )
		      || ( filename[ filename_index ] == '+' )
		      || ( filename[ filename_index ] == ':' )
		      || ( filename[ filename_index ] == ';' )
		      || ( filename[ filename_index ] == '<' )
		      || ( filename[ filename_index ] == '>' )
		      || ( filename[ filename_index ] == '?' )
		      || ( filename[ filename_index ] == '@' )
		      || ( filename[ filename_index ] == '|' )
		      || ( filename[ filename_index ] == '~' )
		      || ( filename[ filename_index ] == 0x7f ) )
		{
			filename[ filename_index ] = '_';
		}
	}
	return( 1 );
}

#if defined( HAVE_WIDE_CHARACTER_TYPE )

#if defined( WINAPI ) && ( WINVER <= 0x0500 )

/* Cross Windows safe version of SetCurrentDirectoryW
 * Returns TRUE if successful or FALSE on error
 */
BOOL libcpath_SetCurrentDirectoryW(
      LPCWSTR path )
{
	FARPROC function       = NULL;
	HMODULE library_handle = NULL;
	BOOL result            = FALSE;

	if( path == NULL )
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
		    (LPCSTR) "SetCurrentDirectoryW" );

	if( function != NULL )
	{
		result = function(
			  path );
	}
	/* This call should be after using the function
	 * in most cases kernel32.dll will still be available after free
	 */
	if( FreeLibrary(
	     library_handle ) != TRUE )
	{
		libcpath_CloseHandle(
		 library_handle );

		return( FALSE );
	}
	return( result );
}

#endif /* defined( WINAPI ) && ( WINVER <= 0x0500 ) */

#if defined( WINAPI )

/* Changes the directory
 * This function uses the WINAPI function for Windows XP (0x0501) or later
 * or tries to dynamically call the function for Windows 2000 (0x0500) or earlier
 * Returns 1 if successful or -1 on error
 */
int libcpath_path_change_directory_wide(
     const wchar_t *directory_name,
     libcerror_error_t **error )
{
	static char *function = "libcpath_path_change_directory_wide";
	DWORD error_code      = 0;

	if( directory_name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid directory name.",
		 function );

		return( -1 );
	}
#if defined( WINAPI ) && ( WINVER <= 0x0500 )
	if( libcpath_SetCurrentDirectoryW(
	     directory_name ) == 0 )
#else
	if( SetCurrentDirectoryW(
	     directory_name ) == 0 )
#endif
	{
		error_code = GetLastError();

		libcerror_system_set_error(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 error_code,
		 "%s: unable to change directory.",
		 function );

		return( -1 );
	}
	return( 1 );
}

#elif defined( HAVE_CHDIR )

/* Changes the directory
 * This function uses the POSIX chdir function or equivalent
 * Returns 1 if successful or -1 on error
 */
int libcpath_path_change_directory_wide(
     const wchar_t *directory_name,
     libcerror_error_t **error )
{
	static char *function             = "libcpath_path_change_directory_wide";
	char *narrow_directory_name       = 0;
	size_t directory_name_length      = 0;
	size_t narrow_directory_name_size = 0;

	if( directory_name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid directory name.",
		 function );

		return( -1 );
	}
	directory_name_length = wide_string_length(
	                         directory_name );

	if( libcpath_system_string_size_from_wide_string(
	     directory_name,
	     directory_name_length + 1,
	     &narrow_directory_name_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_CONVERSION,
		 LIBCERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to determine narrow directory name size.",
		 function );

		goto on_error;
	}
	if( ( narrow_directory_name_size > (size_t) SSIZE_MAX )
	 || ( ( sizeof( char ) * narrow_directory_name_size )  > (size_t) SSIZE_MAX ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid narrow directory name size value exceeds maximum.",
		 function );

		goto on_error;
	}
	narrow_directory_name = narrow_string_allocate(
	                         narrow_directory_name_size );

	if( narrow_directory_name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create narrow directory name.",
		 function );

		goto on_error;
	}
	if( libcpath_system_string_copy_from_wide_string(
	     narrow_directory_name,
	     narrow_directory_name_size,
	     directory_name,
	     directory_name_length + 1,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_CONVERSION,
		 LIBCERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to set name.",
		 function );

		goto on_error;
	}
	if( chdir(
	     narrow_directory_name ) != 0 )
	{
		libcerror_system_set_error(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 errno,
		 "%s: unable to change directory.",
		 function );

		return( -1 );
	}
	memory_free(
	 narrow_directory_name );

	return( 1 );

on_error:
	if( narrow_directory_name != NULL )
	{
		memory_free(
		 narrow_directory_name );
	}
	return( -1 );
}

#else
#error Missing change directory function
#endif

#if defined( WINAPI ) && ( WINVER <= 0x0500 )

/* Cross Windows safe version of GetCurrentDirectoryW
 * Returns the number of characters in the current directory string or 0 on error
 */
DWORD libcpath_GetCurrentDirectoryW(
       DWORD buffer_size,
       LPCWSTR buffer )
{
	FARPROC function       = NULL;
	HMODULE library_handle = NULL;
	DWORD result           = 0;

	if( buffer == NULL )
	{
		return( 0 );
	}
	library_handle = LoadLibrary(
	                  _SYSTEM_STRING( "kernel32.dll" ) );

	if( library_handle == NULL )
	{
		return( 0 );
	}
	function = GetProcAddress(
		    library_handle,
		    (LPCSTR) "GetCurrentDirectoryW" );

	if( function != NULL )
	{
		result = function(
			  buffer_size,
			  buffer );
	}
	/* This call should be after using the function
	 * in most cases kernel32.dll will still be available after free
	 */
	if( FreeLibrary(
	     library_handle ) != TRUE )
	{
		libcpath_CloseHandle(
		 library_handle );

		return( 0 );
	}
	return( result );
}

#endif /* defined( WINAPI ) && ( WINVER <= 0x0500 ) */

#if defined( WINAPI )

/* Retrieves the current working directory
 * This function uses the WINAPI function for Windows XP (0x0501) or later
 * or tries to dynamically call the function for Windows 2000 (0x0500) or earlier
 * Returns 1 if successful or -1 on error
 */
int libcpath_path_get_current_working_directory_wide(
     wchar_t **current_working_directory,
     size_t *current_working_directory_size,
     libcerror_error_t **error )
{
	static char *function                     = "libcpath_path_get_current_working_directory_wide";
	DWORD safe_current_working_directory_size = 0;
	DWORD error_code                          = 0;

	if( current_working_directory == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid current working directory.",
		 function );

		return( -1 );
	}
	if( *current_working_directory != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid current working directory value already set.",
		 function );

		return( -1 );
	}
	if( current_working_directory_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid current working directory size.",
		 function );

		return( -1 );
	}
	safe_current_working_directory_size = GetCurrentDirectoryW(
	                                       0,
	                                       NULL );

	if( safe_current_working_directory_size == 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve current working directory size.",
		 function );

		goto on_error;
	}
	if( (size_t) safe_current_working_directory_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: current working directory size value out of bounds.",
		 function );

		goto on_error;
	}
	*current_working_directory_size = (size_t) safe_current_working_directory_size;

	*current_working_directory = wide_string_allocate(
	                              *current_working_directory_size );

	if( *current_working_directory == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create current working directory.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *current_working_directory,
	     0,
	     sizeof( wchar_t ) * *current_working_directory_size ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear current working directory.",
		 function );

		goto on_error;
	}
#if defined( WINAPI ) && ( WINVER <= 0x0500 )
	if( libcpath_GetCurrentDirectoryW(
	     safe_current_working_directory_size,
	     *current_working_directory ) != ( safe_current_working_directory_size - 1 ) )
#else
	if( GetCurrentDirectoryW(
	     safe_current_working_directory_size,
	     *current_working_directory ) != ( safe_current_working_directory_size - 1 ) )
#endif
	{
		error_code = GetLastError();

		libcerror_system_set_error(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 error_code,
		 "%s: unable to retrieve current working directory.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( *current_working_directory != NULL )
	{
		memory_free(
		 *current_working_directory );

		*current_working_directory = NULL;
	}
	*current_working_directory_size = 0;

	return( -1 );
}

#elif defined( HAVE_GETCWD )

/* Retrieves the current working directory
 * This function uses the POSIX getcwd function or equivalent
 * Returns 1 if successful or -1 on error
 */
int libcpath_path_get_current_working_directory_wide(
     wchar_t **current_working_directory,
     size_t *current_working_directory_size,
     libcerror_error_t **error )
{
	static char *function                          = "libcpath_path_get_current_working_directory_wide";
	char *narrow_current_working_directory         = 0;
	size_t narrow_current_working_directory_length = 0;

	if( current_working_directory == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid current working directory.",
		 function );

		return( -1 );
	}
	if( *current_working_directory != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid current working directory value already set.",
		 function );

		return( -1 );
	}
	if( current_working_directory_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid current working directory size.",
		 function );

		return( -1 );
	}
	narrow_current_working_directory = narrow_string_allocate(
	                                    PATH_MAX );

	if( narrow_current_working_directory == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create narrow current working directory.",
		 function );

		goto on_error;
	}
	if( getcwd(
	     narrow_current_working_directory,
	     PATH_MAX ) == NULL )
	{
		libcerror_system_set_error(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 errno,
		 "%s: unable to retrieve current working directory.",
		 function );

		goto on_error;
	}
	narrow_current_working_directory_length = narrow_string_length(
	                                           narrow_current_working_directory );

	/* Convert the current working directory to a wide string
	 * if the platform has no wide character open function
	 */
	if( libcpath_system_string_size_from_narrow_string(
	     narrow_current_working_directory,
	     narrow_current_working_directory_length + 1,
	     current_working_directory_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_CONVERSION,
		 LIBCERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to determine wide character current working directory size.",
		 function );

		return( -1 );
	}
	*current_working_directory = wide_string_allocate(
	                              *current_working_directory_size );

	if( *current_working_directory == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create current working directory.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *current_working_directory,
	     0,
	     sizeof( wchar_t ) * *current_working_directory_size ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear current working directory.",
		 function );

		goto on_error;
	}
	if( libcpath_system_string_copy_to_wide_string(
	     narrow_current_working_directory,
	     narrow_current_working_directory_length + 1,
	     *current_working_directory,
	     *current_working_directory_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_CONVERSION,
		 LIBCERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to set current working directory.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( narrow_current_working_directory != NULL )
	{
		memory_free(
		 narrow_current_working_directory );
	}
	if( *current_working_directory != NULL )
	{
		memory_free(
		 *current_working_directory );

		*current_working_directory = NULL;
	}
	*current_working_directory_size = 0;

	return( -1 );
}

#else
#error Missing get current working directory function
#endif

#if defined( WINAPI )

/* Determines the volume name and path type
 * Returns 1 if succesful or -1 on error
 */
int libcpath_path_get_volume_name_and_path_type_wide(
     const wchar_t *path,
     size_t path_length,
     wchar_t **volume_name,
     size_t *volume_name_length,
     size_t *directory_name_index,
     uint8_t *path_type,
     libcerror_error_t **error )
{
	static char *function   = "libcpath_path_get_volume_name_and_path_type_wide";
	size_t path_index       = 0;
	size_t share_name_index = 0;

	if( path == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid path.",
		 function );

		return( -1 );
	}
	if( path_length > (size_t) ( SSIZE_MAX - 1 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid path length value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( volume_name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid volume name.",
		 function );

		return( -1 );
	}
	if( volume_name_length == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid volume name length.",
		 function );

		return( -1 );
	}
	if( directory_name_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid directory name index.",
		 function );

		return( -1 );
	}
	if( path_type == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid path type.",
		 function );

		return( -1 );
	}
	if( ( path_length == 2 )
	 && ( path[ 1 ] == (wchar_t) '\\' ) )
	{
		*path_type = LIBCPATH_TYPE_ABSOLUTE;
	}
	else if( path_length >= 2 )
	{
		/* Check if the path starts with a volume letter
		 */
		if( ( path[ 1 ] == (wchar_t) ':' )
		 && ( ( ( path[ 0 ] >= (wchar_t) 'A' )
		   && ( path[ 0 ] <= (wchar_t) 'Z' ) )
		  || ( ( path[ 0 ] >= (wchar_t) 'a' )
		   && ( path[ 0 ] <= (wchar_t) 'z' ) ) ) )
		{
			*volume_name          = (wchar_t *) path;
			*volume_name_length   = 2;
			*directory_name_index = 2;

			if( ( path_length >= 3 )
			 && ( path[ 2 ] == (wchar_t) '\\' ) )
			{
				*path_type             = LIBCPATH_TYPE_ABSOLUTE;
				*directory_name_index += 1;
			}
		}
		/* Check for special paths
		 * paths with prefix: \\
		 */
		else if( ( path[ 0 ] == (wchar_t) '\\' )
		      && ( path[ 1 ] == (wchar_t) '\\' ) )
		{
			/* Determine if the path is a special path
			 * device path prefix:          \\.\
			 * extended-length path prefix: \\?\
			 */
			if( ( path_length >= 4 )
			 && ( ( path[ 2 ] == (wchar_t) '.' )
			   || ( path[ 2 ] == (wchar_t) '?' ) )
			 && ( path[ 3 ] == (wchar_t) '\\' ) )
			{
				if( path[ 2 ] == (wchar_t) '.' )
				{
					*path_type = LIBCPATH_TYPE_DEVICE;
				}
				else
				{
					*path_type = LIBCPATH_TYPE_EXTENDED_LENGTH;
				}
				*directory_name_index = 4;
			}
			else
			{
				/* Determine the volume in an UNC path
				 * \\server\share
				 */
				for( share_name_index = 2;
				     share_name_index <= path_length;
				     share_name_index++ )
				{
					if( path[ share_name_index ] == (wchar_t) '\\' )
					{
						break;
					}
				}
				if( share_name_index > path_length )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: invalid path - missing share name.",
					 function );

					return( -1 );
				}
				for( path_index = share_name_index + 1;
				     path_index <= path_length;
				     path_index++ )
				{
					if( path[ path_index ] == (wchar_t) '\\' )
					{
						break;
					}
				}
				*path_type            = LIBCPATH_TYPE_UNC;
				*volume_name          = (wchar_t *) &( path[ 2 ] );
				*volume_name_length   = path_index - 2;
				*directory_name_index = path_index;
			}
		}
		/* Check for absolue paths
		 */
		else if( path[ 0 ] == (wchar_t) '\\' )
		{
			*path_type = LIBCPATH_TYPE_ABSOLUTE;
		}
	}
	return( 1 );
}

/* Retrieves the current working directory of a specific volume
 * Returns 1 if successful or -1 on error
 */
int libcpath_path_get_current_working_directory_by_volume_wide(
     wchar_t *volume_name,
     size_t volume_name_length,
     wchar_t **current_working_directory,
     size_t *current_working_directory_size,
     libcerror_error_t **error )
{
	wchar_t *change_volume_name                  = NULL;
	wchar_t *current_volume_working_directory    = NULL;
	static char *function                        = "libcpath_path_get_current_working_directory_by_volume_wide";
	size_t current_volume_working_directory_size = 0;
	int result                                   = 1;

	if( current_working_directory == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid current working directory.",
		 function );

		return( -1 );
	}
	if( current_working_directory_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid current working directory size.",
		 function );

		return( -1 );
	}
	/* If the path contains a volume name switch to that
	 * volume to determine the current directory
	 */
	if( volume_name != NULL )
	{
		if( libcpath_path_get_current_working_directory_wide(
		     &current_volume_working_directory,
		     &current_volume_working_directory_size,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve current volume working directory.",
			 function );

			goto on_error;
		}
		change_volume_name = wide_string_allocate(
		                      volume_name_length + 1 );

		if( change_volume_name == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_MEMORY,
			 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to create change volume name.",
			 function );

			goto on_error;
		}
		if( wide_string_copy(
		     change_volume_name,
		     volume_name,
		     volume_name_length ) == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_MEMORY,
			 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
			 "%s: unable to set change volume name.",
			 function );

			goto on_error;
		}
		change_volume_name[ volume_name_length ] = 0;

		if( libcpath_path_change_directory_wide(
		     change_volume_name,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to change current working directory.",
			 function );

			goto on_error;
		}
		memory_free(
		 change_volume_name );

		change_volume_name = NULL;
	}
	if( libcpath_path_get_current_working_directory_wide(
	     current_working_directory,
	     current_working_directory_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve current directory.",
		 function );

		/* Make sure the current working directory has been changed
		 * back to its original value
		 */
		result = -1;
	}
	if( current_volume_working_directory != NULL )
	{
		if( libcpath_path_change_directory_wide(
		     current_volume_working_directory,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to change current working directory.",
			 function );

			goto on_error;
		}
		memory_free(
		 current_volume_working_directory );

		current_volume_working_directory = NULL;
	}
	return( result );

on_error:
	if( change_volume_name != NULL )
	{
		memory_free(
		 change_volume_name );
	}
	if( current_volume_working_directory != NULL )
	{
		memory_free(
		 current_volume_working_directory );
	}
	if( *current_working_directory != NULL )
	{
		memory_free(
		 *current_working_directory );

		*current_working_directory = NULL;
	}
	*current_working_directory_size = 0;

	return( -1 );
}

/* Determines the volume name
 * Returns 1 if succesful or -1 on error
 */
int libcpath_path_get_volume_name_wide(
     const wchar_t *path,
     size_t path_size,
     wchar_t **volume_name,
     size_t *volume_name_length,
     size_t *directory_name_index,
     libcerror_error_t **error )
{
	static char *function   = "libcpath_path_get_volume_name_wide";
	size_t path_index       = 0;
	size_t share_name_index = 0;

	if( path == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid path.",
		 function );

		return( -1 );
	}
	if( path_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid path size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( volume_name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid volume name.",
		 function );

		return( -1 );
	}
	if( volume_name_length == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid volume name length.",
		 function );

		return( -1 );
	}
	if( directory_name_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid directory name index.",
		 function );

		return( -1 );
	}
/* TODO clean up code */
	if( path_size >= 3 )
	{
		if( path == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing path.",
			 function );

			return( -1 );
		}
		/* Check if the path starts with a volume letter
		 */
		if( ( path[ 1 ] == (wchar_t) ':' )
		 && ( ( ( path[ 0 ] >= (wchar_t) 'A' )
		   && ( path[ 0 ] <= (wchar_t) 'Z' ) )
		  || ( ( path[ 0 ] >= (wchar_t) 'a' )
		   && ( path[ 0 ] <= (wchar_t) 'z' ) ) ) )
		{
			if( *volume_name == NULL )
			{
				*volume_name        = (wchar_t *) path;
				*volume_name_length = 2;
			}
			path_index = 2;

			if( path[ 2 ] == (wchar_t) '\\' )
			{
				path_index++;
			}
			*directory_name_index = path_index;
		}
		/* Check for special paths
		 * paths with prefix: \\
		 */
		else if( ( path[ 0 ] == (wchar_t) '\\' )
		      && ( path[ 1 ] == (wchar_t) '\\' ) )
		{
			/* Ignore the following special paths
			 * device path prefix:          \\.\
			 */
			if( ( path_size >= 4 )
			 && ( path[ 2 ] == (wchar_t) '.' )
			 && ( path[ 3 ] == (wchar_t) '\\' ) )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
				 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
				 "%s: unsupported path.",
				 function );

				return( -1 );
			}
			else
			{
				/* Determine the volume in an UNC path
				 * \\server\share
				 */
				for( share_name_index = 2;
				     share_name_index < path_size;
				     share_name_index++ )
				{
					if( path[ share_name_index ] == (wchar_t) '\\' )
					{
						break;
					}
				}
				if( share_name_index >= path_size )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: invalid path - missing share name.",
					 function );

					return( -1 );
				}
				for( path_index = share_name_index + 1;
				     path_index < path_size;
				     path_index++ )
				{
					if( path[ path_index ] == (wchar_t) '\\' )
					{
						break;
					}
				}
				if( *volume_name == NULL )
				{
					*volume_name = (wchar_t *) &( path[ 2 ] );

					if( path_index == path_size )
					{
						*volume_name_length = path_index - 3;
					}
					else
					{
						*volume_name_length = path_index - 2;
					}
				}
				*directory_name_index = path_index;
			}
		}
	}
	return( 1 );
}

/* Determines the full path of the Windows path specified
 * The function uses the extended-length path format
 * (path with \\?\ prefix)
 *
 * Multiple successive \ not at the start of the path are combined into one
 *
 * Scenario's that are considered full paths:
 * Device path:			\\.\PhysicalDrive0
 * Extended-length path:	\\?\C:\directory\file.txt
 * Extended-length UNC path:	\\?\UNC\server\share\directory\file.txt
 *
 * Scenario's that are not considered full paths:
 * Local 'absolute' path:	\directory\file.txt
 * Local 'relative' path:	..\directory\file.txt
 * Local 'relative' path:	.\directory\file.txt
 * Volume 'absolute' path:	C:\directory\file.txt
 *                              C:\..\directory\file.txt
 * Volume 'relative' path:	C:directory\file.txt
 * UNC path:			\\server\share\directory\file.txt
 *
 * Returns 1 if succesful or -1 on error
 */
int libcpath_path_get_full_path_wide(
     const wchar_t *path,
     size_t path_length,
     wchar_t **full_path,
     size_t *full_path_size,
     libcerror_error_t **error )
{
	libcsplit_wide_split_string_t *current_directory_split_string = NULL;
	libcsplit_wide_split_string_t *path_split_string              = NULL;
	wchar_t *current_directory                                    = NULL;
	wchar_t *current_directory_string_segment                     = NULL;
	wchar_t *full_path_prefix                                     = NULL;
	wchar_t *last_used_path_string_segment                        = NULL;
	wchar_t *path_string_segment                                  = NULL;
	wchar_t *volume_name                                          = NULL;
	static char *function                                         = "libcpath_path_get_full_path_wide";
	size_t current_directory_length                               = 0;
	size_t current_directory_name_index                           = 0;
	size_t current_directory_size                                 = 0;
	size_t current_directory_string_segment_size                  = 0;
	size_t full_path_index                                        = 0;
	size_t full_path_prefix_length                                = 0;
	size_t last_used_path_string_segment_size                     = 0;
	size_t path_directory_name_index                              = 0;
	size_t path_string_segment_size                               = 0;
	size_t safe_full_path_size                                    = 0;
	size_t share_name_index                                       = 0;
	size_t volume_name_length                                     = 0;
	uint8_t path_type                                             = LIBCPATH_TYPE_RELATIVE;
	int current_directory_number_of_segments                      = 0;
	int current_directory_segment_index                           = 0;
	int last_used_path_segment_index                              = -1;
	int path_number_of_segments                                   = 0;
	int path_segment_index                                        = 0;

	if( path == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid path.",
		 function );

		return( -1 );
	}
	if( path_length == 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_ZERO_OR_LESS,
		 "%s: invalid path length is zero.",
		 function );

		return( -1 );
	}
	if( path_length > (size_t) ( SSIZE_MAX - 1 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid path length value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( full_path == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid full path.",
		 function );

		return( -1 );
	}
	if( *full_path != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid full path value already set.",
		 function );

		return( -1 );
	}
	if( full_path_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid full path size.",
		 function );

		return( -1 );
	}
	if( libcpath_path_get_volume_name_and_path_type_wide(
	     path,
	     path_length,
	     &volume_name,
	     &volume_name_length,
	     &path_directory_name_index,
	     &path_type,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to determine volume name and path type.",
		 function );

		goto on_error;
	}
	/* If the path is a device path, an extended-length path or an UNC
	 * do not bother to lookup the current directory
	 */
	if( ( path_type != LIBCPATH_TYPE_DEVICE )
	 && ( path_type != LIBCPATH_TYPE_EXTENDED_LENGTH )
	 && ( path_type != LIBCPATH_TYPE_UNC ) )
	{
		if( libcpath_path_get_current_working_directory_by_volume_wide(
		     volume_name,
		     volume_name_length,
		     &current_directory,
		     &current_directory_size,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve current directory by volume.",
			 function );

			goto on_error;
		}
		/* Determine the volume name using the current directory if necessary
		 */
		if( libcpath_path_get_volume_name_wide(
		     current_directory,
		     current_directory_size,
		     &volume_name,
		     &volume_name_length,
		     &current_directory_name_index,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve current directory by volume.",
			 function );

			goto on_error;
		}
	}
	if( ( current_directory != NULL )
	 && ( current_directory_name_index < current_directory_size ) )
	{
		current_directory_length = wide_string_length(
		                            &( current_directory[ current_directory_name_index ] ) );

		if( libcsplit_wide_string_split(
		     &( current_directory[ current_directory_name_index ] ),
		     current_directory_length + 1,
		     (wchar_t) '\\',
		     &current_directory_split_string,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to split current directory.",
			 function );

			goto on_error;
		}
	}
	if( libcsplit_wide_string_split(
	     &( path[ path_directory_name_index ] ),
	     path_length - path_directory_name_index + 1,
	     (wchar_t) '\\',
	     &path_split_string,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to split path.",
		 function );

		goto on_error;
	}
	/* The size of the full path consists of:
	 * the size of the prefix (\\?\ or \\.\)
	 * the length of the volume name
	 * a directory separator
	 */
	safe_full_path_size = 4;

	/* If the path contains a volume name
	 * the length of the volume name
	 * a directory separator
	 */
	if( volume_name != NULL )
	{
		safe_full_path_size += volume_name_length + 1;
	}
	/* If the path contains an UNC path
	 * add the size of the UNC\ prefix
	 */
	if( share_name_index > 0 )
	{
		safe_full_path_size += 4;
	}
	/* If the path is relative
	 * add the size of the current directory
	 * a directory separator, if necessary
	 */
	if( ( path_type == LIBCPATH_TYPE_RELATIVE )
	 && ( current_directory_name_index < current_directory_size ) )
	{
		safe_full_path_size += ( current_directory_size - ( current_directory_name_index + 1 ) );

		if( ( current_directory_size >= 2 )
		 && ( current_directory[ current_directory_size - 2 ] != (wchar_t) '\\' ) )
		{
			safe_full_path_size += 1;
		}
	}
	if( current_directory_split_string != NULL )
	{
		if( libcsplit_wide_split_string_get_number_of_segments(
		     current_directory_split_string,
		     &current_directory_number_of_segments,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve number of current directory string segments.",
			 function );

			goto on_error;
		}
		current_directory_segment_index = current_directory_number_of_segments - 1;
	}
	if( libcsplit_wide_split_string_get_number_of_segments(
	     path_split_string,
	     &path_number_of_segments,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of path string segments.",
		 function );

		goto on_error;
	}
	for( path_segment_index = 0;
	     path_segment_index < path_number_of_segments;
	     path_segment_index++ )
	{
		if( libcsplit_wide_split_string_get_segment_by_index(
		     path_split_string,
		     path_segment_index,
		     &path_string_segment,
		     &path_string_segment_size,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve path string segment: %d.",
			 function,
			 path_segment_index );

			goto on_error;
		}
		if( path_string_segment == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing path string segment: %d.",
			 function,
			 path_segment_index );

			goto on_error;
		}
		/* If the path is .. reverse the current path by one directory
		 */
		if( ( path_string_segment_size == 3 )
		 && ( path_string_segment[ 0 ] == (wchar_t) '.' )
		 && ( path_string_segment[ 1 ] == (wchar_t) '.' ) )
		{
			if( ( current_directory_split_string != NULL )
			 && ( last_used_path_segment_index == -1 ) )
			{
				if( libcsplit_wide_split_string_get_segment_by_index(
				     current_directory_split_string,
				     current_directory_segment_index,
				     &current_directory_string_segment,
				     &current_directory_string_segment_size,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve current directory string segment: %d.",
					 function,
					 current_directory_segment_index );

					goto on_error;
				}
				if( current_directory_string_segment == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: missing current directory string segment: %d.",
					 function,
					 current_directory_segment_index );

					goto on_error;
				}
				/* Remove the size of the parent directory name and a directory separator
				 * Note that the size includes the end of string character
				 */
				safe_full_path_size -= current_directory_string_segment_size;

				if( libcsplit_wide_split_string_set_segment_by_index(
				     current_directory_split_string,
				     current_directory_segment_index,
				     NULL,
				     0,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to set current directory string segment: %d.",
					 function,
					 current_directory_segment_index );

					goto on_error;
				}
				current_directory_segment_index--;
			}
			else if( last_used_path_segment_index >= 0 )
			{
				if( libcsplit_wide_split_string_get_segment_by_index(
				     path_split_string,
				     last_used_path_segment_index,
				     &last_used_path_string_segment,
				     &last_used_path_string_segment_size,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve last used path string segment: %d.",
					 function,
					 last_used_path_segment_index );

					goto on_error;
				}
				if( last_used_path_string_segment == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: missing last used path string string segment: %d.",
					 function,
					 last_used_path_segment_index );

					goto on_error;
				}
				/* Remove the size of the parent directory name and a directory separator
				 * Note that the size includes the end of string character
				 */
				safe_full_path_size -= last_used_path_string_segment_size;

				if( libcsplit_wide_split_string_set_segment_by_index(
				     path_split_string,
				     last_used_path_segment_index,
				     NULL,
				     0,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to set path string segment: %d.",
					 function,
					 last_used_path_segment_index );

					goto on_error;
				}
				/* Find the previous path split value that contains a name
				 */
				while( last_used_path_segment_index > 0 )
				{
					last_used_path_segment_index--;

					if( libcsplit_wide_split_string_get_segment_by_index(
					     path_split_string,
					     last_used_path_segment_index,
					     &last_used_path_string_segment,
					     &last_used_path_string_segment_size,
					     error ) != 1 )
					{
						libcerror_error_set(
						 error,
						 LIBCERROR_ERROR_DOMAIN_RUNTIME,
						 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
						 "%s: unable to retrieve last used path string segment: %d.",
						 function,
						 last_used_path_segment_index );

						goto on_error;
					}
					if( last_used_path_string_segment_size != 0 )
					{
						break;
					}
				}
			}
			if( libcsplit_wide_split_string_set_segment_by_index(
			     path_split_string,
			     path_segment_index,
			     NULL,
			     0,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set path string segment: %d.",
				 function,
				 path_segment_index );

				goto on_error;
			}
		}
		/* If the path is . ignore the entry
		 */
		else if( ( path_string_segment_size == 2 )
		      && ( path_string_segment[ 0 ] == (wchar_t) '.' ) )
		{
			if( libcsplit_wide_split_string_set_segment_by_index(
			     path_split_string,
			     path_segment_index,
			     NULL,
			     0,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set path string segment: %d.",
				 function,
				 path_segment_index );

				goto on_error;
			}
		}
		/* If the path is empty ignore the entry
		 */
		else if( path_string_segment_size <= 1 )
		{
			if( libcsplit_wide_split_string_set_segment_by_index(
			     path_split_string,
			     path_segment_index,
			     NULL,
			     0,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set path string segment: %d.",
				 function,
				 path_segment_index );

				goto on_error;
			}
		}
		else
		{
			/* Add the size of the directory or file name and a directory separator
			 * Note that the size includes the end of string character
			 */
			safe_full_path_size += path_string_segment_size;

			last_used_path_segment_index = path_segment_index;
		}
	}
	/* Note that the last path separator serves as the end of string
	 */
	full_path_index = 0;

	*full_path = wide_string_allocate(
	              safe_full_path_size );

	if( *full_path == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create full path.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *full_path,
	     0,
	     sizeof( wchar_t ) * safe_full_path_size ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear full path.",
		 function );

		goto on_error;
	}
	*full_path_size = safe_full_path_size;

	if( path_type == LIBCPATH_TYPE_DEVICE )
	{
		full_path_prefix        = L"\\\\.\\";
		full_path_prefix_length = 4;
	}
	else
	{
		full_path_prefix        = L"\\\\?\\";
		full_path_prefix_length = 4;
	}
	if( wide_string_copy(
	     &( ( *full_path )[ full_path_index ] ),
	     full_path_prefix,
	     full_path_prefix_length ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set prefix in full path.",
		 function );

		goto on_error;
	}
	full_path_index += full_path_prefix_length;

	/* If there is a share name the path is an UNC path
	 */
	if( share_name_index > 0 )
	{
		if( wide_string_copy(
		     &( ( *full_path )[ full_path_index ] ),
		     L"UNC\\",
		     4 ) == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set UNC\\ prefix in full path.",
			 function );

			goto on_error;
		}
		full_path_index += 4;
	}
	if( volume_name != NULL )
	{
		if( wide_string_copy(
		     &( ( *full_path )[ full_path_index ] ),
		     volume_name,
		     volume_name_length ) == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set volume name in full path.",
			 function );

			goto on_error;
		}
		full_path_index += volume_name_length;

		( *full_path )[ full_path_index ] = (wchar_t) '\\';

		full_path_index += 1;
	}
	/* If the path is relative
	 * add the current directory elements
	 */
	if( ( path_type == LIBCPATH_TYPE_RELATIVE )
	 && ( current_directory_split_string != NULL ) )
	{
		for( current_directory_segment_index = 0;
		     current_directory_segment_index < current_directory_number_of_segments;
		     current_directory_segment_index++ )
		{
			if( libcsplit_wide_split_string_get_segment_by_index(
			     current_directory_split_string,
			     current_directory_segment_index,
			     &current_directory_string_segment,
			     &current_directory_string_segment_size,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve current directory string segment: %d.",
				 function,
				 current_directory_segment_index );

				goto on_error;
			}
			if( current_directory_string_segment_size != 0 )
			{
				if( current_directory_string_segment == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: missing current directory string segment: %d.",
					 function,
					 current_directory_segment_index );

					goto on_error;
				}
				if( wide_string_copy(
				     &( ( *full_path )[ full_path_index ] ),
				     current_directory_string_segment,
				     current_directory_string_segment_size - 1 ) == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to set current directory split value: %d in full path.",
					 function,
					 current_directory_segment_index );

					goto on_error;
				}
				full_path_index += current_directory_string_segment_size - 1;

				( *full_path )[ full_path_index ] = (wchar_t) '\\';

				full_path_index += 1;
			}
		}
	}
	for( path_segment_index = 0;
	     path_segment_index < path_number_of_segments;
	     path_segment_index++ )
	{
		if( libcsplit_wide_split_string_get_segment_by_index(
		     path_split_string,
		     path_segment_index,
		     &path_string_segment,
		     &path_string_segment_size,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve path string segment: %d.",
			 function,
			 path_segment_index );

			goto on_error;
		}
		if( path_string_segment_size != 0 )
		{
			if( path_string_segment == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
				 "%s: missing path string segment: %d.",
				 function,
				 path_segment_index );

				goto on_error;
			}
			if( wide_string_copy(
			     &( ( *full_path )[ full_path_index ] ),
			     path_string_segment,
			     path_string_segment_size - 1 ) == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set path split value: %d in full path.",
				 function,
				 path_segment_index );

				goto on_error;
			}
			full_path_index += path_string_segment_size - 1;

			( *full_path )[ full_path_index ] = (wchar_t) '\\';

			full_path_index += 1;
		}
	}
	( *full_path )[ full_path_index - 1 ] = 0;

	if( libcsplit_wide_split_string_free(
	     &path_split_string,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
		 "%s: unable to free path split string.",
		 function );

		goto on_error;
	}
	if( current_directory_split_string != NULL )
	{
		if( libcsplit_wide_split_string_free(
		     &current_directory_split_string,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free current directory split string.",
			 function );

			goto on_error;
		}
	}
	if( current_directory != NULL )
	{
		memory_free(
		 current_directory );
	}
	return( 1 );

on_error:
	if( *full_path != NULL )
	{
		memory_free(
		 *full_path );

		*full_path = NULL;
	}
	*full_path_size = 0;

	if( path_split_string != NULL )
	{
		libcsplit_wide_split_string_free(
		 &path_split_string,
		 NULL );
	}
	if( current_directory_split_string != NULL )
	{
		libcsplit_wide_split_string_free(
		 &current_directory_split_string,
		 NULL );
	}
	if( current_directory != NULL )
	{
		memory_free(
		 current_directory );
	}
	return( -1 );
}

#else

/* Determines the full path of the POSIX path specified
 * Multiple successive / are combined into one
 *
 * Scenarios:
 * /home/user/file.txt
 * /home/user//file.txt
 * /home/user/../user/file.txt
 * /../home/user/file.txt
 * user/../user/file.txt
 *
 * Returns 1 if succesful or -1 on error
 */
int libcpath_path_get_full_path_wide(
     const wchar_t *path,
     size_t path_length,
     wchar_t **full_path,
     size_t *full_path_size,
     libcerror_error_t **error )
{
	libcsplit_wide_split_string_t *current_directory_split_string = NULL;
	libcsplit_wide_split_string_t *path_split_string              = NULL;
	wchar_t *current_directory                                    = NULL;
	wchar_t *current_directory_string_segment                     = NULL;
	wchar_t *last_used_path_string_segment                        = NULL;
	wchar_t *path_string_segment                                  = NULL;
	static char *function                                         = "libcpath_path_get_full_path_wide";
	size_t current_directory_length                               = 0;
	size_t current_directory_size                                 = 0;
	size_t current_directory_string_segment_size                  = 0;
	size_t full_path_index                                        = 0;
	size_t last_used_path_string_segment_size                     = 0;
	size_t path_string_segment_size                               = 0;
	size_t safe_full_path_size                                    = 0;
	uint8_t path_type                                             = LIBCPATH_TYPE_RELATIVE;
	int current_directory_number_of_segments                      = 0;
	int current_directory_segment_index                           = 0;
	int last_used_path_segment_index                              = -1;
	int path_number_of_segments                                   = 0;
	int path_segment_index                                        = 0;

	if( path == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid path.",
		 function );

		return( -1 );
	}
	if( path_length == 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_ZERO_OR_LESS,
		 "%s: invalid path length is zero.",
		 function );

		return( -1 );
	}
	if( path_length > (size_t) ( SSIZE_MAX - 1 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid path length value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( full_path == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid full path.",
		 function );

		return( -1 );
	}
	if( *full_path != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid full path value already set.",
		 function );

		return( -1 );
	}
	if( full_path_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid full path size.",
		 function );

		return( -1 );
	}
	if( path[ 0 ] == (wchar_t) '/' )
	{
		path_type = LIBCPATH_TYPE_ABSOLUTE;
	}
	else
	{
		if( libcpath_path_get_current_working_directory_wide(
		     &current_directory,
		     &current_directory_size,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve current directory.",
			 function );

			goto on_error;
		}
	}
	if( current_directory != NULL )
	{
		current_directory_length = wide_string_length(
		                            current_directory );

		if( libcsplit_wide_string_split(
		     current_directory,
		     current_directory_length + 1,
		     (wchar_t) '/',
		     &current_directory_split_string,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to split current directory.",
			 function );

			goto on_error;
		}
	}
	if( libcsplit_wide_string_split(
	     path,
	     path_length + 1,
	     (wchar_t) '/',
	     &path_split_string,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to split path.",
		 function );

		goto on_error;
	}
	/* Determine the full path size
	 */
	/* If the path is absolute
	 * a directory separator
	 */
	if( path_type == LIBCPATH_TYPE_ABSOLUTE )
	{
		safe_full_path_size = 1;
	}
	/* If the path is relative
	 * add the size of the current directory
	 * a directory separator, if necessary
	 */
	else if( path_type == LIBCPATH_TYPE_RELATIVE )
	{
		if( current_directory == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing current directory.",
			 function );

			goto on_error;
		}
		safe_full_path_size = current_directory_size - 1;

		if( ( current_directory_size >= 2 )
		 && ( current_directory[ current_directory_size - 2 ] != (wchar_t) '/' ) )
		{
			safe_full_path_size++;
		}
	}
	if( current_directory_split_string != NULL )
	{
		if( libcsplit_wide_split_string_get_number_of_segments(
		     current_directory_split_string,
		     &current_directory_number_of_segments,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve number of current directory string segments.",
			 function );

			goto on_error;
		}
		current_directory_segment_index = current_directory_number_of_segments - 1;
	}
	if( libcsplit_wide_split_string_get_number_of_segments(
	     path_split_string,
	     &path_number_of_segments,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of path string segments.",
		 function );

		goto on_error;
	}
	for( path_segment_index = 0;
	     path_segment_index < path_number_of_segments;
	     path_segment_index++ )
	{
		if( libcsplit_wide_split_string_get_segment_by_index(
		     path_split_string,
		     path_segment_index,
		     &path_string_segment,
		     &path_string_segment_size,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve path string segment: %d.",
			 function,
			 path_segment_index );

			goto on_error;
		}
		if( path_string_segment == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing path string segment: %d.",
			 function,
			 path_segment_index );

			goto on_error;
		}
		/* If the path is .. reverse the current path by one directory
		 */
		if( ( path_string_segment_size == 3 )
		 && ( path_string_segment[ 0 ] == (wchar_t) '.' )
		 && ( path_string_segment[ 1 ] == (wchar_t) '.' ) )
		{
			if( ( current_directory_split_string != NULL )
			 && ( last_used_path_segment_index == -1 ) )
			{
				if( libcsplit_wide_split_string_get_segment_by_index(
				     current_directory_split_string,
				     current_directory_segment_index,
				     &current_directory_string_segment,
				     &current_directory_string_segment_size,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve current directory string segment: %d.",
					 function,
					 current_directory_segment_index );

					goto on_error;
				}
				if( current_directory_string_segment == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: missing current directory string segment: %d.",
					 function,
					 current_directory_segment_index );

					goto on_error;
				}
				/* Remove the size of the parent directory name and a directory separator
				 * Note that the size includes the end of string character
				 */
				safe_full_path_size -= current_directory_string_segment_size;

				if( libcsplit_wide_split_string_set_segment_by_index(
				     current_directory_split_string,
				     current_directory_segment_index,
				     NULL,
				     0,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to set current directory string segment: %d.",
					 function,
					 current_directory_segment_index );

					goto on_error;
				}
				current_directory_segment_index--;
			}
			else if( last_used_path_segment_index >= 0 )
			{
				if( libcsplit_wide_split_string_get_segment_by_index(
				     path_split_string,
				     last_used_path_segment_index,
				     &last_used_path_string_segment,
				     &last_used_path_string_segment_size,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve last used path string segment: %d.",
					 function,
					 last_used_path_segment_index );

					goto on_error;
				}
				if( last_used_path_string_segment == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: missing last used path string string segment: %d.",
					 function,
					 last_used_path_segment_index );

					goto on_error;
				}
				/* Remove the size of the parent directory name and a directory separator
				 * Note that the size includes the end of string character
				 */
				safe_full_path_size -= last_used_path_string_segment_size;

				if( libcsplit_wide_split_string_set_segment_by_index(
				     path_split_string,
				     last_used_path_segment_index,
				     NULL,
				     0,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to set path string segment: %d.",
					 function,
					 last_used_path_segment_index );

					goto on_error;
				}
				/* Find the previous path split value that contains a name
				 */
				while( last_used_path_segment_index > 0 )
				{
					last_used_path_segment_index--;

					if( libcsplit_wide_split_string_get_segment_by_index(
					     path_split_string,
					     last_used_path_segment_index,
					     &last_used_path_string_segment,
					     &last_used_path_string_segment_size,
					     error ) != 1 )
					{
						libcerror_error_set(
						 error,
						 LIBCERROR_ERROR_DOMAIN_RUNTIME,
						 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
						 "%s: unable to retrieve last used path string segment: %d.",
						 function,
						 last_used_path_segment_index );

						goto on_error;
					}
					if( last_used_path_string_segment_size != 0 )
					{
						break;
					}
				}
			}
			if( libcsplit_wide_split_string_set_segment_by_index(
			     path_split_string,
			     path_segment_index,
			     NULL,
			     0,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set path string segment: %d.",
				 function,
				 path_segment_index );

				goto on_error;
			}
		}
		/* If the path is . ignore the entry
		 */
		else if( ( path_string_segment_size == 2 )
		      && ( path_string_segment[ 0 ] == (wchar_t) '.' ) )
		{
			if( libcsplit_wide_split_string_set_segment_by_index(
			     path_split_string,
			     path_segment_index,
			     NULL,
			     0,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set path string segment: %d.",
				 function,
				 path_segment_index );

				goto on_error;
			}
		}
		/* If the path is empty ignore the entry
		 */
		else if( path_string_segment_size <= 1 )
		{
			if( libcsplit_wide_split_string_set_segment_by_index(
			     path_split_string,
			     path_segment_index,
			     NULL,
			     0,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set path string segment: %d.",
				 function,
				 path_segment_index );

				goto on_error;
			}
		}
		else
		{
			/* Add the size of the directory or file name and a directory separator
			 * Note that the size includes the end of string character
			 */
			safe_full_path_size += path_string_segment_size;

			last_used_path_segment_index = path_segment_index;
		}
	}
	/* Note that the last path separator serves as the end of string
	 */
	full_path_index = 0;

	*full_path = wide_string_allocate(
	              safe_full_path_size );

	if( *full_path == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create full path.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *full_path,
	     0,
	     sizeof( wchar_t ) * safe_full_path_size ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear full path.",
		 function );

		goto on_error;
	}
	*full_path_size = safe_full_path_size;

	if( path_type == LIBCPATH_TYPE_ABSOLUTE )
	{
		( *full_path )[ full_path_index ] = (wchar_t) '/';

		full_path_index += 1;
	}
	/* If the path is relative
	 * add the current directory elements
	 */
	if( ( path_type == LIBCPATH_TYPE_RELATIVE )
	 && ( current_directory_split_string != NULL ) )
	{
		for( current_directory_segment_index = 0;
		     current_directory_segment_index < current_directory_number_of_segments;
		     current_directory_segment_index++ )
		{
			if( libcsplit_wide_split_string_get_segment_by_index(
			     current_directory_split_string,
			     current_directory_segment_index,
			     &current_directory_string_segment,
			     &current_directory_string_segment_size,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve current directory string segment: %d.",
				 function,
				 current_directory_segment_index );

				goto on_error;
			}
			if( current_directory_string_segment_size != 0 )
			{
				if( current_directory_string_segment == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: missing current directory string segment: %d.",
					 function,
					 current_directory_segment_index );

					goto on_error;
				}
				if( wide_string_copy(
				     &( ( *full_path )[ full_path_index ] ),
				     current_directory_string_segment,
				     current_directory_string_segment_size - 1 ) == NULL )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to set current directory split value: %d in full path.",
					 function,
					 current_directory_segment_index );

					goto on_error;
				}
				full_path_index += current_directory_string_segment_size - 1;

				( *full_path )[ full_path_index ] = (wchar_t) '/';

				full_path_index += 1;
			}
		}
	}
	for( path_segment_index = 0;
	     path_segment_index < path_number_of_segments;
	     path_segment_index++ )
	{
		if( libcsplit_wide_split_string_get_segment_by_index(
		     path_split_string,
		     path_segment_index,
		     &path_string_segment,
		     &path_string_segment_size,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve path string segment: %d.",
			 function,
			 path_segment_index );

			goto on_error;
		}
		if( path_string_segment_size != 0 )
		{
			if( path_string_segment == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
				 "%s: missing path string segment: %d.",
				 function,
				 path_segment_index );

				goto on_error;
			}
			if( wide_string_copy(
			     &( ( *full_path )[ full_path_index ] ),
			     path_string_segment,
			     path_string_segment_size - 1 ) == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set path split value: %d in full path.",
				 function,
				 path_segment_index );

				goto on_error;
			}
			full_path_index += path_string_segment_size - 1;

			( *full_path )[ full_path_index ] = (wchar_t) '/';

			full_path_index += 1;
		}
	}
	( *full_path )[ full_path_index - 1 ] = 0;

	if( libcsplit_wide_split_string_free(
	     &path_split_string,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
		 "%s: unable to free path split string.",
		 function );

		goto on_error;
	}
	if( current_directory_split_string != NULL )
	{
		if( libcsplit_wide_split_string_free(
		     &current_directory_split_string,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free current directory split string.",
			 function );

			goto on_error;
		}
	}
	if( current_directory != NULL )
	{
		memory_free(
		 current_directory );
	}
	return( 1 );

on_error:
	if( *full_path != NULL )
	{
		memory_free(
		 *full_path );

		*full_path = NULL;
	}
	*full_path_size = 0;

	if( path_split_string != NULL )
	{
		libcsplit_wide_split_string_free(
		 &path_split_string,
		 NULL );
	}
	if( current_directory_split_string != NULL )
	{
		libcsplit_wide_split_string_free(
		 &current_directory_split_string,
		 NULL );
	}
	if( current_directory != NULL )
	{
		memory_free(
		 current_directory );
	}
	return( -1 );
}

#endif /* defined( WINAPI ) */

/* Retrieves a sanitized version of the filename
 * Returns 1 if successful or -1 on error
 */
int libcpath_path_get_sanitized_filename_wide(
     const wchar_t *filename,
     size_t filename_size,
     wchar_t **sanitized_filename,
     size_t *sanitized_filename_size,
     libcerror_error_t **error )
{
	static char *function = "libcpath_path_get_sanitized_filename_wide";

	if( sanitized_filename == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid sanitized filename.",
		 function );

		return( -1 );
	}
	if( *sanitized_filename != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid sanitized filename value already set.",
		 function );

		return( -1 );
	}
	if( sanitized_filename_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid sanitized filename size.",
		 function );

		return( -1 );
	}
/* TODO implement */
	return( -1 );
}

/* Retrieves a sanitized version of the path
 * Returns 1 if successful or -1 on error
 */
int libcpath_path_get_sanitized_path_wide(
     const wchar_t *path,
     size_t path_size,
     wchar_t **sanitized_path,
     size_t *sanitized_path_size,
     libcerror_error_t **error )
{
	static char *function = "libcpath_path_get_sanitized_path_wide";

	if( sanitized_path == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid sanitized path.",
		 function );

		return( -1 );
	}
	if( *sanitized_path != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid sanitized path value already set.",
		 function );

		return( -1 );
	}
	if( sanitized_path_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid sanitized path size.",
		 function );

		return( -1 );
	}
/* TODO implement */
	return( -1 );
}

/* Combines the directory name and filename into a path
 * Returns 1 if successful or -1 on error
 */
int libcpath_path_join_wide(
     wchar_t **path,
     size_t *path_size,
     const wchar_t *directory_name,
     size_t directory_name_length,
     const wchar_t *filename,
     size_t filename_length,
     libcerror_error_t **error )
{
	static char *function = "libcpath_path_join_wide";
	size_t filename_index = 0;
	size_t path_index     = 0;

	if( path == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid path.",
		 function );

		return( -1 );
	}
	if( *path != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid path value already set.",
		 function );

		return( -1 );
	}
	if( path_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid path size.",
		 function );

		return( -1 );
	}
	if( directory_name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid directory name.",
		 function );

		return( -1 );
	}
	if( directory_name_length > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid directory name length value exceeds maximum.",
		 function );

		return( -1 );
	}
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
	if( filename_length > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid filename length value exceeds maximum.",
		 function );

		return( -1 );
	}
/* TODO strip other patterns like /./ */
	while( directory_name_length > 0 )
	{
		if( directory_name[ directory_name_length - 1 ] != (wchar_t) LIBCPATH_SEPARATOR )
		{
			break;
		}
		directory_name_length--;
	}
	while( filename_length > 0 )
	{
		if( filename[ filename_index ] != (wchar_t) LIBCPATH_SEPARATOR )
		{
			break;
		}
		filename_index++;
		filename_length--;
	}
	*path_size = directory_name_length + filename_length + 2;

	*path = wide_string_allocate(
	         *path_size );

	if( *path == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create path.",
		 function );

		goto on_error;
	}
	if( wide_string_copy(
	     *path,
	     directory_name,
	     directory_name_length ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
		 "%s: unable to copy directory name to path.",
		 function );

		goto on_error;
	}
	path_index = directory_name_length;

	( *path )[ path_index++ ] = (wchar_t) LIBCPATH_SEPARATOR;

	if( wide_string_copy(
	     &( ( *path )[ path_index ] ),
	     &( filename[ filename_index ] ),
	     filename_length ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
		 "%s: unable to copy filename to path.",
		 function );

		goto on_error;
	}
	path_index += filename_length;

	( *path )[ path_index ] = 0;

	return( 1 );

on_error:
	if( *path != NULL )
	{
		memory_free(
		 path );

		*path = NULL;
	}
	*path_size = 0;

	return( -1 );
}

#if defined( WINAPI ) && ( WINVER <= 0x0500 )

/* Cross Windows safe version of CreateDirectoryW
 * Returns TRUE if successful or FALSE on error
 */
BOOL libcpath_CreateDirectoryW(
      LPCWSTR path,
      SECURITY_ATTRIBUTES *security_attributes )
{
	FARPROC function       = NULL;
	HMODULE library_handle = NULL;
	BOOL result            = FALSE;

	if( path == NULL )
	{
		return( 0 );
	}
	library_handle = LoadLibrary(
	                  _SYSTEM_STRING( "kernel32.dll" ) );

	if( library_handle == NULL )
	{
		return( 0 );
	}
	function = GetProcAddress(
		    library_handle,
		    (LPCSTR) "CreateDirectoryW" );

	if( function != NULL )
	{
		result = function(
			  path,
			  security_attributes );
	}
	/* This call should be after using the function
	 * in most cases kernel32.dll will still be available after free
	 */
	if( FreeLibrary(
	     library_handle ) != TRUE )
	{
		libcpath_CloseHandle(
		 library_handle );

		return( 0 );
	}
	return( result );
}

#endif /* defined( WINAPI ) && ( WINVER <= 0x0500 ) */

#if defined( WINAPI )

/* Makes the directory
 * This function uses the WINAPI function for Windows XP (0x0501) or later
 * or tries to dynamically call the function for Windows 2000 (0x0500) or earlier
 * Returns 1 if successful or -1 on error
 */
int libcpath_path_make_directory_wide(
     const wchar_t *directory_name,
     libcerror_error_t **error )
{
	static char *function = "libcpath_path_make_directory_wide";
	DWORD error_code      = 0;

	if( directory_name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid directory name.",
		 function );

		return( -1 );
	}
#if defined( WINAPI ) && ( WINVER <= 0x0500 )
	if( libcpath_CreateDirectoryW(
	     directory_name,
	     NULL ) == 0 )
#else
	if( CreateDirectoryW(
	     directory_name,
	     NULL ) == 0 )
#endif
	{
		error_code = GetLastError();

		libcerror_system_set_error(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 error_code,
		 "%s: unable to make directory.",
		 function );

		return( -1 );
	}
	return( 1 );
}

#elif defined( HAVE_MKDIR )

/* Makes the directory
 * This function uses the POSIX mkdir function or equivalent
 * Returns 1 if successful or -1 on error
 */
int libcpath_path_make_directory_wide(
     const wchar_t *directory_name,
     libcerror_error_t **error )
{
	static char *function             = "libcpath_path_make_directory_wide";
	char *narrow_directory_name       = 0;
	size_t directory_name_length      = 0;
	size_t narrow_directory_name_size = 0;

	if( directory_name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid directory name.",
		 function );

		return( -1 );
	}
	directory_name_length = wide_string_length(
	                         directory_name );

	if( libcpath_system_string_size_from_wide_string(
	     directory_name,
	     directory_name_length + 1,
	     &narrow_directory_name_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_CONVERSION,
		 LIBCERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to determine narrow directory name size.",
		 function );

		goto on_error;
	}
	if( ( narrow_directory_name_size > (size_t) SSIZE_MAX )
	 || ( ( sizeof( char ) * narrow_directory_name_size )  > (size_t) SSIZE_MAX ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid narrow directory name size value exceeds maximum.",
		 function );

		goto on_error;
	}
	narrow_directory_name = narrow_string_allocate(
	                         narrow_directory_name_size );

	if( narrow_directory_name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create narrow directory name.",
		 function );

		goto on_error;
	}
	if( libcpath_system_string_copy_from_wide_string(
	     narrow_directory_name,
	     narrow_directory_name_size,
	     directory_name,
	     directory_name_length + 1,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_CONVERSION,
		 LIBCERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to set name.",
		 function );

		goto on_error;
	}
	if( mkdir(
	     narrow_directory_name,
	     0755 ) != 0 )
	{
		libcerror_system_set_error(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 errno,
		 "%s: unable to make directory.",
		 function );

		goto on_error;
	}
	memory_free(
	 narrow_directory_name );

	return( 1 );

on_error:
	if( narrow_directory_name != NULL )
	{
		memory_free(
		 narrow_directory_name );
	}
	return( -1 );
}

#else
#error Missing make directory function
#endif

/* Sanitizes the path
 * Returns 1 if successful or -1 on error
 */
int libcpath_path_sanitize_wide(
     wchar_t *path,
     size_t *path_size,
     libcerror_error_t **error )
{
	static char *function = "libcpath_path_sanitize_wide";
	size_t path_index     = 0;

	if( path == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid path.",
		 function );

		return( -1 );
	}
	if( path_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid path size.",
		 function );

		return( -1 );
	}
	if( *path_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid path size value exceeds maximum.",
		 function );

		return( -1 );
	}
#if defined( WINAPI ) || defined( __CYGWIN__ )
	if( *path_size > 32767 )
	{
		path[ 32766 ] = 0;

		*path_size = 32767;
	}
#endif
	for( path_index = 0;
	     path_index < *path_size;
	     path_index++ )
	{
		if( path[ path_index ] == 0x00 )
		{
			break;
		}
		else if( ( path[ path_index ] >= 0x01 )
		      && ( path[ path_index ] <= 0x1f ) )
		{
			path[ path_index ] = (wchar_t) '_';
		}
#if defined( WINAPI )
		else if( path[ path_index ] == (wchar_t) '/' )
#else
		else if( path[ path_index ] == (wchar_t) '\\' )
#endif
		{
			path[ path_index ] = (wchar_t) '_';
		}
		else if( ( path[ path_index ] == (wchar_t) '!' )
		      || ( path[ path_index ] == (wchar_t) '$' )
		      || ( path[ path_index ] == (wchar_t) '%' )
		      || ( path[ path_index ] == (wchar_t) '&' )
		      || ( path[ path_index ] == (wchar_t) '*' )
		      || ( path[ path_index ] == (wchar_t) '+' )
		      || ( path[ path_index ] == (wchar_t) ':' )
		      || ( path[ path_index ] == (wchar_t) ';' )
		      || ( path[ path_index ] == (wchar_t) '<' )
		      || ( path[ path_index ] == (wchar_t) '>' )
		      || ( path[ path_index ] == (wchar_t) '?' )
		      || ( path[ path_index ] == (wchar_t) '@' )
		      || ( path[ path_index ] == (wchar_t) '|' )
		      || ( path[ path_index ] == (wchar_t) '~' )
		      || ( path[ path_index ] == 0x7f ) )
		{
			path[ path_index ] = (wchar_t) '_';
		}
	}
	return( 1 );
}

/* Sanitizes the filename
 * Returns 1 if successful or -1 on error
 */
int libcpath_path_sanitize_filename_wide(
     wchar_t *filename,
     size_t *filename_size,
     libcerror_error_t **error )
{
	static char *function = "libcpath_path_sanitize_filename_wide";
	size_t filename_index = 0;

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
	if( filename_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid filename size.",
		 function );

		return( -1 );
	}
	if( *filename_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid filename size value exceeds maximum.",
		 function );

		return( -1 );
	}
#if defined( WINAPI ) || defined( __CYGWIN__ )
	if( *filename_size > 256 )
	{
		filename[ 255 ] = 0;

		*filename_size = 256;
	}
#endif
	for( filename_index = 0;
	     filename_index < *filename_size;
	     filename_index++ )
	{
		if( filename[ filename_index ] == 0x00 )
		{
			break;
		}
		else if( ( filename[ filename_index ] >= 0x01 )
		      && ( filename[ filename_index ] <= 0x1f ) )
		{
			filename[ filename_index ] = (wchar_t) '_';
		}
		else if( ( filename[ filename_index ] == (wchar_t) '/' )
		      || ( filename[ filename_index ] == (wchar_t) '\\' ) )
		{
			filename[ filename_index ] = (wchar_t) '_';
		}
		else if( ( filename[ filename_index ] == (wchar_t) '!' )
		      || ( filename[ filename_index ] == (wchar_t) '$' )
		      || ( filename[ filename_index ] == (wchar_t) '%' )
		      || ( filename[ filename_index ] == (wchar_t) '&' )
		      || ( filename[ filename_index ] == (wchar_t) '*' )
		      || ( filename[ filename_index ] == (wchar_t) '+' )
		      || ( filename[ filename_index ] == (wchar_t) ':' )
		      || ( filename[ filename_index ] == (wchar_t) ';' )
		      || ( filename[ filename_index ] == (wchar_t) '<' )
		      || ( filename[ filename_index ] == (wchar_t) '>' )
		      || ( filename[ filename_index ] == (wchar_t) '?' )
		      || ( filename[ filename_index ] == (wchar_t) '@' )
		      || ( filename[ filename_index ] == (wchar_t) '|' )
		      || ( filename[ filename_index ] == (wchar_t) '~' )
		      || ( filename[ filename_index ] == 0x7f ) )
		{
			filename[ filename_index ] = (wchar_t) '_';
		}
	}
	return( 1 );
}

#endif /* defined( HAVE_WIDE_CHARACTER_TYPE ) */

