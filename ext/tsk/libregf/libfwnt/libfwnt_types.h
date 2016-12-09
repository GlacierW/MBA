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

#if !defined( _LIBFWNT_INTERNAL_TYPES_H )
#define _LIBFWNT_INTERNAL_TYPES_H

#include <common.h>
#include <types.h>

/* Define HAVE_LOCAL_LIBFWNT for local use of libfwnt
 * The definitions in <libfwnt/types.h> are copied here
 * for local use of libfwnt
 */
#if defined( HAVE_LOCAL_LIBFWNT )

/* The following type definitions hide internal data structures
 */
#if defined( HAVE_DEBUG_OUTPUT ) && !defined( WINAPI )
typedef struct libfwnt_access_control_entry {}	libfwnt_access_control_entry_t;
typedef struct libfwnt_access_control_list {}	libfwnt_access_control_list_t;
typedef struct libfwnt_security_descriptor {}	libfwnt_security_descriptor_t;
typedef struct libfwnt_security_identifier {}	libfwnt_security_identifier_t;

#else
typedef intptr_t libfwnt_access_control_entry_t;
typedef intptr_t libfwnt_access_control_list_t;
typedef intptr_t libfwnt_security_descriptor_t;
typedef intptr_t libfwnt_security_identifier_t;

#endif /* defined( HAVE_DEBUG_OUTPUT ) && !defined( WINAPI ) */

#endif /* defined( HAVE_LOCAL_LIBFWNT ) */

#endif /* !defined( _LIBFWNT_INTERNAL_TYPES_H ) */

