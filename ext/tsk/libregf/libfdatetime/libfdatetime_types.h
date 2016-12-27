/*
 * The internal type definitions
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

#if !defined( _LIBFDATETIME_INTERNAL_TYPES_H )
#define _LIBFDATETIME_INTERNAL_TYPES_H

#include <common.h>
#include <types.h>

/* Define HAVE_LOCAL_LIBFDATETIME for local use of libfdatetime
 * The definitions in <libfdatetime/types.h> are copied here
 * for local use of libfdatetime
 */
#if defined( HAVE_LOCAL_LIBFDATETIME )

/* The following type definitions hide internal data structures
 */
#if defined( HAVE_DEBUG_OUTPUT ) && !defined( WINAPI )
typedef struct libfdatetime_fat_date_time {}	libfdatetime_fat_date_time_t;
typedef struct libfdatetime_fat_date {}		libfdatetime_fat_date_t;
typedef struct libfdatetime_fat_time {}		libfdatetime_fat_time_t;
typedef struct libfdatetime_filetime {}		libfdatetime_filetime_t;
typedef struct libfdatetime_floatingtime {}	libfdatetime_floatingtime_t;
typedef struct libfdatetime_hfs_time {}		libfdatetime_hfs_time_t;
typedef struct libfdatetime_nsf_timedate {}	libfdatetime_nsf_timedate_t;
typedef struct libfdatetime_posix_time {}	libfdatetime_posix_time_t;
typedef struct libfdatetime_systemtime {}	libfdatetime_systemtime_t;

#else
typedef intptr_t libfdatetime_fat_date_time_t;
typedef intptr_t libfdatetime_fat_date_t;
typedef intptr_t libfdatetime_fat_time_t;
typedef intptr_t libfdatetime_filetime_t;
typedef intptr_t libfdatetime_floatingtime_t;
typedef intptr_t libfdatetime_hfs_time_t;
typedef intptr_t libfdatetime_nsf_timedate_t;
typedef intptr_t libfdatetime_posix_time_t;
typedef intptr_t libfdatetime_systemtime_t;

#endif /* defined( HAVE_DEBUG_OUTPUT ) && !defined( WINAPI ) */

#endif /* defined( HAVE_LOCAL_LIBFDATETIME ) */

#endif /* !defined( _LIBFDATETIME_INTERNAL_TYPES_H ) */

