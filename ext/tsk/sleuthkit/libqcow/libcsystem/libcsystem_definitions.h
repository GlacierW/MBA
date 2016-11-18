/*
 * The internal definitions
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

#if !defined( LIBCSYSTEM_INTERNAL_DEFINITIONS_H )
#define LIBCSYSTEM_INTERNAL_DEFINITIONS_H

#include <common.h>
#include <types.h>

/* Define HAVE_LOCAL_LIBCSYSTEM for local use of libcsystem
 */
#if !defined( HAVE_LOCAL_LIBCSYSTEM )
#include <libcsystem/definitions.h>

/* The definitions in <libcsystem/definitions.h> are copied here
 * for local use of libcsystem
 */
#else

#define LIBCSYSTEM_VERSION				20161106

/* The libcsystem version string
 */
#define LIBCSYSTEM_VERSION_STRING			"20161106"

/* The file access flags
 * bit 1	set to 1 for read access
 * bit 2	set to 1 for write access
 * bit 3	set to 1 to truncate an existing file on write
 * bit 4-8	not used
 */
enum LIBCSYSTEM_ACCESS_FLAGS
{
	LIBCSYSTEM_ACCESS_FLAG_READ			= 0x01,
	LIBCSYSTEM_ACCESS_FLAG_WRITE			= 0x02,
	LIBCSYSTEM_ACCESS_FLAG_TRUNCATE			= 0x04
};

/* The file access macros
 */
#define LIBCSYSTEM_OPEN_READ				( LIBCSYSTEM_ACCESS_FLAG_READ )
#define LIBCSYSTEM_OPEN_WRITE				( LIBCSYSTEM_ACCESS_FLAG_WRITE )
#define LIBCSYSTEM_OPEN_READ_WRITE			( LIBCSYSTEM_ACCESS_FLAG_READ | LIBCSYSTEM_ACCESS_FLAG_WRITE )
#define LIBCSYSTEM_OPEN_WRITE_TRUNCATE			( LIBCSYSTEM_ACCESS_FLAG_WRITE | LIBCSYSTEM_ACCESS_FLAG_TRUNCATE )
#define LIBCSYSTEM_OPEN_READ_WRITE_TRUNCATE		( LIBCSYSTEM_ACCESS_FLAG_READ | LIBCSYSTEM_ACCESS_FLAG_WRITE | LIBCSYSTEM_ACCESS_FLAG_TRUNCATE )

/* The access behavior types
 */
enum LIBCSYSTEM_ACCESS_BEHAVIOR
{
	LIBCSYSTEM_ACCESS_BEHAVIOR_NORMAL,
	LIBCSYSTEM_ACCESS_BEHAVIOR_RANDOM,
	LIBCSYSTEM_ACCESS_BEHAVIOR_SEQUENTIAL
};

#endif /* !defined( HAVE_LOCAL_LIBCSYSTEM ) */

/* Platform specific macros
 */
#if defined( WINAPI )

#if defined( _MSC_VER )
#define  LIBCSYSTEM_LARGE_INTEGER_ZERO			{ 0, 0 }

#elif defined( __BORLANDC__ )
#define  LIBCSYSTEM_LARGE_INTEGER_ZERO			{ 0 }

#elif defined( __CYGWIN__ )
#define  LIBCSYSTEM_LARGE_INTEGER_ZERO			{ { 0, 0 } }

#elif defined( __MINGW32_VERSION ) || defined( __MINGW64_VERSION_MAJOR )
#define  LIBCSYSTEM_LARGE_INTEGER_ZERO			{ { 0, 0 } }

#endif

#if !defined( INVALID_FILE_ATTRIBUTES )
#define INVALID_FILE_ATTRIBUTES				( (DWORD) -1 )
#endif

#endif /* defined( WINAPI ) */

#endif

