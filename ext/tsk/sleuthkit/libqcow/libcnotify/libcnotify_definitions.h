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

#if !defined( LIBCNOTIFY_INTERNAL_DEFINITIONS_H )
#define LIBCNOTIFY_INTERNAL_DEFINITIONS_H

#include <common.h>
#include <types.h>

/* Define HAVE_LOCAL_LIBCNOTIFY for local use of libcnotify
 */
#if !defined( HAVE_LOCAL_LIBCNOTIFY )
#include <libcnotify/definitions.h>

/* The definitions in <libcnotify/definitions.h> are copied here
 * for local use of libcnotify
 */
#else

#define LIBCNOTIFY_VERSION				20161106

/* The libcnotify version string
 */
#define LIBCNOTIFY_VERSION_STRING			"20161106"

/* The print data flags
 */
enum LIBCNOTIFY_NOTIFY_PRINT_DATA_FLAGS
{
	LIBCNOTIFY_PRINT_DATA_FLAG_GROUP_DATA		= 0x01,
};

#endif /* !defined( HAVE_LOCAL_LIBCNOTIFY ) */

#endif

