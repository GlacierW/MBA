/*
 * The internal type definitions
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

#if !defined( _LIBCSYSTEM_INTERNAL_TYPES_H )
#define _LIBCSYSTEM_INTERNAL_TYPES_H

#include <common.h>
#include <types.h>

/* Define HAVE_LOCAL_LIBCSYSTEM for local use of libcsystem
 * The definitions in <libcsystem/types.h> are copied here
 * for local use of libcsystem
 */
#if defined( HAVE_LOCAL_LIBCSYSTEM )

/* The following type definitions hide internal data structures
 */
#if defined( HAVE_DEBUG_OUTPUT ) && !defined( WINAPI )
typedef struct libcsystem_file {}	libcsystem_file_t;
typedef struct libcsystem_glob {}	libcsystem_glob_t;
typedef struct libcsystem_stream {}	libcsystem_stream_t;

#else
typedef intptr_t libcsystem_file_t;
typedef intptr_t libcsystem_glob_t;
typedef intptr_t libcsystem_stream_t;

#endif /* defined( HAVE_DEBUG_OUTPUT ) && !defined( WINAPI ) */

#if defined( WINAPI )
typedef unsigned long libcsystem_signal_t;

#elif defined( HAVE_SIGNAL_H )
typedef int libcsystem_signal_t;

#endif

#endif /* defined( HAVE_LOCAL_LIBCSYSTEM ) */

#endif /* !defined( _LIBCSYSTEM_INTERNAL_TYPES_H ) */

