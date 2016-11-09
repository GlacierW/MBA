/*
 * The internal definitions
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

#if !defined( _LIBBFIO_INTERNAL_DEFINITIONS_H )
#define _LIBBFIO_INTERNAL_DEFINITIONS_H

#include <common.h>

/* Define HAVE_LOCAL_LIBBFIO for local use of libbfio
 */
#if !defined( HAVE_LOCAL_LIBBFIO )
#include <libbfio/definitions.h>

/* The definitions in <libbfio/definitions.h> are copied here
 * for local use of libbfio
 */
#else
#define LIBBFIO_VERSION					20161109

/* The libbfio version string
 */
#define LIBBFIO_VERSION_STRING				"20161109"

/* The library flags definitions
 */
enum LIBBFIO_FLAGS
{
	/* The IO handle is not managed by the library
	 */
        LIBBFIO_FLAG_IO_HANDLE_NON_MANAGED		= 0x00,

	/* The IO handle is managed by the library
	 */
        LIBBFIO_FLAG_IO_HANDLE_MANAGED			= 0x01,

	/* The IO handle is cloned by the function
	 */
        LIBBFIO_FLAG_IO_HANDLE_CLONE_BY_FUNCTION	= 0x00,

	/* The IO handle is not cloned, but passed as a reference
	 */
        LIBBFIO_FLAG_IO_HANDLE_CLONE_BY_REFERENCE	= 0x02,
};

/* The access flags definitions
 * bit 1	set to 1 for read access
 * bit 2	set to 1 for write access
 * bit 3	set to 1 to truncate an existing file on write
 * bit 4-8	not used
 */
enum LIBBFIO_ACCESS_FLAGS
{
	LIBBFIO_ACCESS_FLAG_READ			= 0x01,
	LIBBFIO_ACCESS_FLAG_WRITE			= 0x02,
	LIBBFIO_ACCESS_FLAG_TRUNCATE			= 0x04
};

/* The file access macros
 */
#define LIBBFIO_OPEN_READ				( LIBBFIO_ACCESS_FLAG_READ )
#define LIBBFIO_OPEN_WRITE				( LIBBFIO_ACCESS_FLAG_WRITE )
#define LIBBFIO_OPEN_READ_WRITE				( LIBBFIO_ACCESS_FLAG_READ | LIBBFIO_ACCESS_FLAG_WRITE )
#define LIBBFIO_OPEN_WRITE_TRUNCATE			( LIBBFIO_ACCESS_FLAG_WRITE | LIBBFIO_ACCESS_FLAG_TRUNCATE )
#define LIBBFIO_OPEN_READ_WRITE_TRUNCATE		( LIBBFIO_ACCESS_FLAG_READ | LIBBFIO_ACCESS_FLAG_WRITE | LIBBFIO_ACCESS_FLAG_TRUNCATE )

#define LIBBFIO_POOL_UNLIMITED_NUMBER_OF_OPEN_HANDLES	0

#endif /* HAVE_LOCAL_LIBBFIO */

#endif

