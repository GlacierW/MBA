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

#if !defined( LIBCFILE_INTERNAL_DEFINITIONS_H )
#define LIBCFILE_INTERNAL_DEFINITIONS_H

#include <common.h>
#include <types.h>

/* Define HAVE_LOCAL_LIBCFILE for local use of libcfile
 */
#if !defined( HAVE_LOCAL_LIBCFILE )
#include <libcfile/definitions.h>

/* The definitions in <libcfile/definitions.h> are copied here
 * for local use of libcfile
 */
#else

#define LIBCFILE_VERSION				20161106

/* The libcfile version string
 */
#define LIBCFILE_VERSION_STRING				"20161106"

/* The file access flags
 * bit 1	set to 1 for read access
 * bit 2	set to 1 for write access
 * bit 3	set to 1 to truncate an existing file on write
 * bit 4-8	not used
 */
enum LIBCFILE_ACCESS_FLAGS
{
	LIBCFILE_ACCESS_FLAG_READ			= 0x01,
	LIBCFILE_ACCESS_FLAG_WRITE			= 0x02,
	LIBCFILE_ACCESS_FLAG_TRUNCATE			= 0x04
};

/* The file access macros
 */
#define LIBCFILE_OPEN_READ				( LIBCFILE_ACCESS_FLAG_READ )
#define LIBCFILE_OPEN_WRITE				( LIBCFILE_ACCESS_FLAG_WRITE )
#define LIBCFILE_OPEN_READ_WRITE			( LIBCFILE_ACCESS_FLAG_READ | LIBCFILE_ACCESS_FLAG_WRITE )
#define LIBCFILE_OPEN_WRITE_TRUNCATE			( LIBCFILE_ACCESS_FLAG_WRITE | LIBCFILE_ACCESS_FLAG_TRUNCATE )
#define LIBCFILE_OPEN_READ_WRITE_TRUNCATE		( LIBCFILE_ACCESS_FLAG_READ | LIBCFILE_ACCESS_FLAG_WRITE | LIBCFILE_ACCESS_FLAG_TRUNCATE )

/* The access behavior types
 */
enum LIBCFILE_ACCESS_BEHAVIOR
{
	LIBCFILE_ACCESS_BEHAVIOR_NORMAL,
	LIBCFILE_ACCESS_BEHAVIOR_RANDOM,
	LIBCFILE_ACCESS_BEHAVIOR_SEQUENTIAL
};

#endif /* !defined( HAVE_LOCAL_LIBCFILE ) */

/* Platform specific macros
 */
#if defined( WINAPI )

#if defined( _MSC_VER )
#define  LIBCFILE_LARGE_INTEGER_ZERO			{ 0, 0 }

#elif defined( __BORLANDC__ )
#define  LIBCFILE_LARGE_INTEGER_ZERO			{ 0 }

#elif defined( __CYGWIN__ )
#define  LIBCFILE_LARGE_INTEGER_ZERO			{ { 0, 0 } }

#elif defined( __MINGW32_VERSION ) || defined( __MINGW64_VERSION_MAJOR )
#define  LIBCFILE_LARGE_INTEGER_ZERO			{ { 0, 0 } }

#endif

#if !defined( INVALID_FILE_ATTRIBUTES )
#define INVALID_FILE_ATTRIBUTES				( (DWORD) -1 )
#endif

#endif /* defined( WINAPI ) */

#endif

