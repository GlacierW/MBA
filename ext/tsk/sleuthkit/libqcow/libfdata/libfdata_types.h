/*
 * The internal type definitions
 *
 * Copyright (C) 2010-2016, Joachim Metz <joachim.metz@gmail.com>
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

#if !defined( _LIBFDATA_INTERNAL_TYPES_H )
#define _LIBFDATA_INTERNAL_TYPES_H

#include <common.h>
#include <types.h>

/* Define HAVE_LOCAL_LIBFDATA for local use of libfdata
 * The definitions in <libfdata/types.h> are copied here
 * for local use of libfdata
 */
#if defined( HAVE_LOCAL_LIBFDATA )

#include "libfdata_libfcache.h"

typedef libfcache_cache_t			libfdata_cache_t;

/* The following type definitions hide internal data structures
 */
#if defined( HAVE_DEBUG_OUTPUT ) && !defined( WINAPI )
typedef struct libfdata_area {}			libfdata_area_t;
typedef struct libfdata_btree {}		libfdata_btree_t;
typedef struct libfdata_btree_node {}		libfdata_btree_node_t;
typedef struct libfdata_list {}			libfdata_list_t;
typedef struct libfdata_list_element {}		libfdata_list_element_t;
typedef struct libfdata_range_list {}		libfdata_range_list_t;
typedef struct libfdata_stream {}		libfdata_stream_t;
typedef struct libfdata_tree {}			libfdata_tree_t;
typedef struct libfdata_tree_node {}		libfdata_tree_node_t;
typedef struct libfdata_vector {}		libfdata_vector_t;

#else
typedef intptr_t libfdata_area_t;
typedef intptr_t libfdata_btree_t;
typedef intptr_t libfdata_btree_node_t;
typedef intptr_t libfdata_list_t;
typedef intptr_t libfdata_list_element_t;
typedef intptr_t libfdata_range_list_t;
typedef intptr_t libfdata_stream_t;
typedef intptr_t libfdata_tree_t;
typedef intptr_t libfdata_tree_node_t;
typedef intptr_t libfdata_vector_t;

#endif /* defined( HAVE_DEBUG_OUTPUT ) && !defined( WINAPI ) */

#endif /* defined( HAVE_LOCAL_LIBFDATA ) */

#endif /* !defined( _LIBFDATA_INTERNAL_TYPES_H ) */

