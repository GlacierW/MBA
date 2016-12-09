/*
 * The internal type definitions
 *
 * Copyright (C) 2006-2016, Joachim Metz <joachim.metz@gmail.com>
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

#if !defined( _LIBCDATA_INTERNAL_TYPES_H )
#define _LIBCDATA_INTERNAL_TYPES_H

#include <common.h>
#include <types.h>

/* Define HAVE_LOCAL_LIBCDATA for local use of libcdata
 * The definitions in <libcdata/types.h> are copied here
 * for local use of libcdata
 */
#if defined( HAVE_LOCAL_LIBCDATA )

/* The following type definitions hide internal data structures
 */
#if defined( HAVE_DEBUG_OUTPUT ) && !defined( WINAPI )
typedef struct libcdata_array {}	libcdata_array_t;
typedef struct libcdata_btree {}	libcdata_btree_t;
typedef struct libcdata_list {}		libcdata_list_t;
typedef struct libcdata_list_element {}	libcdata_list_element_t;
typedef struct libcdata_range_list {}	libcdata_range_list_t;
typedef struct libcdata_tree_node {}	libcdata_tree_node_t;

#else
typedef intptr_t libcdata_array_t;
typedef intptr_t libcdata_btree_t;
typedef intptr_t libcdata_list_t;
typedef intptr_t libcdata_list_element_t;
typedef intptr_t libcdata_range_list_t;
typedef intptr_t libcdata_tree_node_t;

#endif /* defined( HAVE_DEBUG_OUTPUT ) && !defined( WINAPI ) */

#endif /* defined( HAVE_LOCAL_LIBCDATA ) */

#endif /* !defined( _LIBCDATA_INTERNAL_TYPES_H ) */

