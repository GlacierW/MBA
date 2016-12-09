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

#if !defined( _LIBCERROR_INTERNAL_TYPES_H )
#define _LIBCERROR_INTERNAL_TYPES_H

#include <common.h>
#include <types.h>

/* Define HAVE_LOCAL_LIBCERROR for local use of libcerror
 * The definitions in <libcerror/types.h> are copied here
 * for local use of libcerror
 */
#if defined( HAVE_LOCAL_LIBCERROR )

/* The following type definitions hide internal data structures
 */
typedef intptr_t libcerror_error_t;

/* Currently not supported
#if defined( HAVE_DEBUG_OUTPUT ) && !defined( WINAPI )
typedef struct libcerror_error {}	libcerror_error_t;

#else
typedef intptr_t libcerror_error_t;

#endif
*/

#endif /* defined( HAVE_LOCAL_LIBCERROR ) */

#endif /* !defined( _LIBCERROR_INTERNAL_TYPES_H ) */

