/*
 * Library to access the Windows NT Registry File (REGF) format
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

#if defined( WINAPI )
#include <windows.h>
#endif

#include "libregf_unused.h"

/* Define HAVE_LOCAL_LIBREGF for local use of libregf
 */
#if !defined( HAVE_LOCAL_LIBREGF )

#if defined( WINAPI ) && defined( HAVE_DLLMAIN )

#if defined( _MANAGED )
#pragma managed( push, off )
#endif

/* Defines the entry point for the DLL
 */
BOOL WINAPI DllMain(
             HINSTANCE hinstDLL,
             DWORD fdwReason,
             LPVOID lpvReserved )
{
	LIBREGF_UNREFERENCED_PARAMETER( lpvReserved )

	switch( fdwReason )
	{
		case DLL_PROCESS_ATTACH:
			DisableThreadLibraryCalls(
			 hinstDLL );
			break;

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;

		case DLL_PROCESS_DETACH:
			break;
	}
	return( TRUE );
}

/* Function that indicates the library is a DLL
 * Returns 1
 */
int libregf_is_dll(
     void )
{
	return( 1 );
}

#endif /* defined( WINAPI ) && defined( HAVE_DLLMAIN ) */

#endif /* !defined( HAVE_LOCAL_LIBREGF ) */

