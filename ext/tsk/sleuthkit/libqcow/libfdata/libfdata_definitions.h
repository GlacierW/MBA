/*
 * The internal definitions
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

#if !defined( _LIBFDATA_INTERNAL_DEFINITIONS_H )
#define _LIBFDATA_INTERNAL_DEFINITIONS_H

#include <common.h>

/* Define HAVE_LOCAL_LIBFDATA for local use of libfdata
 */
#if !defined( HAVE_LOCAL_LIBFDATA )
#include <libfdata/definitions.h>

/* The definitions in <libfdata/definitions.h> are copied here
 * for local use of libfdata
 */
#else
#define LIBFDATA_VERSION						20161107

/* The libfdata version string
 */
#define LIBFDATA_VERSION_STRING						"20161107"

/* The library flag definitions
 */
enum LIBFDATA_FLAGS
{
	/* The data is not managed by the library
	 */
        LIBFDATA_FLAG_DATA_NON_MANAGED					= 0x00,

	/* The data is managed by the library
	 */
        LIBFDATA_FLAG_DATA_MANAGED					= 0x01,

	/* The mapped ranges need to be calculated
	 */
	LIBFDATA_FLAG_CALCULATE_MAPPED_RANGES				= 0x80
};

/* The area element value flag definitions
 */
enum LIBFDATA_AREA_ELEMENT_VALUE_FLAGS
{
	LIBFDATA_AREA_ELEMENT_VALUE_FLAG_NON_MANAGED			= LIBFDATA_FLAG_DATA_NON_MANAGED,
	LIBFDATA_AREA_ELEMENT_VALUE_FLAG_MANAGED			= LIBFDATA_FLAG_DATA_MANAGED
};

/* The balanced tree leaf value flag definitions
 */
enum LIBFDATA_BTREE_LEAF_VALUE_FLAGS
{
	LIBFDATA_BTREE_LEAF_VALUE_FLAG_NON_MANAGED			= LIBFDATA_FLAG_DATA_NON_MANAGED,
	LIBFDATA_BTREE_LEAF_VALUE_FLAG_MANAGED				= LIBFDATA_FLAG_DATA_MANAGED
};

/* The data handle flag definitions
 */
enum LIBFDATA_DATA_HANDLE_FLAGS
{
	LIBFDATA_DATA_HANDLE_FLAG_NON_MANAGED				= LIBFDATA_FLAG_DATA_NON_MANAGED,
	LIBFDATA_DATA_HANDLE_FLAG_MANAGED				= LIBFDATA_FLAG_DATA_MANAGED
};

/* The key value flag definitions
 */
enum LIBFDATA_KEY_VALUE_FLAGS
{
	LIBFDATA_KEY_VALUE_FLAG_NON_MANAGED				= LIBFDATA_FLAG_DATA_NON_MANAGED,
	LIBFDATA_KEY_VALUE_FLAG_MANAGED					= LIBFDATA_FLAG_DATA_MANAGED
};

/* The list element value flag definitions
 */
enum LIBFDATA_LIST_ELEMENT_VALUE_FLAGS
{
	LIBFDATA_LIST_ELEMENT_VALUE_FLAG_NON_MANAGED			= LIBFDATA_FLAG_DATA_NON_MANAGED,
	LIBFDATA_LIST_ELEMENT_VALUE_FLAG_MANAGED			= LIBFDATA_FLAG_DATA_MANAGED
};

/* The range list value flag definitions
 */
enum LIBFDATA_RANGE_LIST_VALUE_FLAGS
{
	LIBFDATA_RANGE_LIST_VALUE_FLAG_NON_MANAGED			= LIBFDATA_FLAG_DATA_NON_MANAGED,
	LIBFDATA_RANGE_LIST_VALUE_FLAG_MANAGED				= LIBFDATA_FLAG_DATA_MANAGED
};

/* The tree node value flag definitions
 */
enum LIBFDATA_TREE_NODE_VALUE_FLAGS
{
	LIBFDATA_TREE_NODE_VALUE_FLAG_NON_MANAGED			= LIBFDATA_FLAG_DATA_NON_MANAGED,
	LIBFDATA_TREE_NODE_VALUE_FLAG_MANAGED				= LIBFDATA_FLAG_DATA_MANAGED
};

/* The vector element value flag definitions
 */
enum LIBFDATA_VECTOR_ELEMENT_VALUE_FLAGS
{
	LIBFDATA_VECTOR_ELEMENT_VALUE_FLAG_NON_MANAGED			= LIBFDATA_FLAG_DATA_NON_MANAGED,
	LIBFDATA_VECTOR_ELEMENT_VALUE_FLAG_MANAGED			= LIBFDATA_FLAG_DATA_MANAGED
};

/* The comparison function definitions
 */
enum LIBFDATA_COMPARE_DEFINITIONS
{
	/* The first value is less than the second value
	 */
        LIBFDATA_COMPARE_LESS,

	/* The first value is less equal than the second value
	 */
        LIBFDATA_COMPARE_LESS_EQUAL,

	/* The first and second values are equal
	 */
        LIBFDATA_COMPARE_EQUAL,

	/* The first value is greater than the second value
	 */
        LIBFDATA_COMPARE_GREATER,

	/* The first value is greater equal than the second value
	 */
        LIBFDATA_COMPARE_GREATER_EQUAL
};

/* The range flag definitions
 */
enum LIBFDATA_RANGE_FLAGS
{
	/* The range is sparse
	 */
        LIBFDATA_RANGE_FLAG_IS_SPARSE					= 0x00000001UL,

	/* The range is compressed
	 */
        LIBFDATA_RANGE_FLAG_IS_COMPRESSED				= 0x00000002UL,

	/* User defined flags
	 */
        LIBFDATA_RANGE_FLAG_USER_DEFINED_1				= 0x00000100UL,
        LIBFDATA_RANGE_FLAG_USER_DEFINED_2				= 0x00000200UL,
        LIBFDATA_RANGE_FLAG_USER_DEFINED_3				= 0x00000400UL,
        LIBFDATA_RANGE_FLAG_USER_DEFINED_4				= 0x00000800UL,
        LIBFDATA_RANGE_FLAG_USER_DEFINED_5				= 0x00001000UL,
        LIBFDATA_RANGE_FLAG_USER_DEFINED_6				= 0x00002000UL,
        LIBFDATA_RANGE_FLAG_USER_DEFINED_7				= 0x00004000UL,
        LIBFDATA_RANGE_FLAG_USER_DEFINED_8				= 0x00008000UL,
};

#define LIBFDATA_RANGE_FLAG_MAXIMUM \
	LIBFDATA_RANGE_FLAG_USER_DEFINED_8

/* The read flag definitions
 */
enum LIBFDATA_READ_FLAGS
{
	/* Flag to indicate cached data should be ignored
	 */
        LIBFDATA_READ_FLAG_IGNORE_CACHE					= 0x01,

	/* Internal flag to indicate data should not be cached
	 */
        LIBFDATA_READ_FLAG_NO_CACHE					= 0x80
};

/* The btree search flag definitions
 */
enum LIBFDATA_BTREE_SEARCH_FLAGS
{
	/* Flag to additionally scan the previous node
	 */
        LIBFDATA_BTREE_SEARCH_FLAG_SCAN_PREVIOUS_NODE			= 0x01,

	/* Flag to additionally scan the next node
	 */
        LIBFDATA_BTREE_SEARCH_FLAG_SCAN_NEXT_NODE			= 0x02
};

/* The tree node flag definitions
 */
enum LIBFDATA_TREE_NODE_FLAGS
{
	/* The node is a leaf
	 */
	LIBFDATA_TREE_NODE_FLAG_IS_LEAF					= 0x02,

	/* The node is virtual
	 */
	LIBFDATA_TREE_NODE_FLAG_IS_VIRTUAL				= 0x04,

	/* The node is deleted
	 */
	LIBFDATA_TREE_NODE_FLAG_IS_DELETED				= 0x08,

	/* The sub nodes data range was set
	 */
	LIBFDATA_TREE_NODE_FLAG_SUB_NODES_DATA_RANGE_SET		= 0x10,

	/* The sub nodes have been read
	 */
	LIBFDATA_TREE_NODE_FLAG_SUB_NODES_READ				= 0x20,

	/* The node data has been read (at least once)
	 */
	LIBFDATA_TREE_NODE_FLAG_NOTE_DATA_READ				= 0x40
};

/* The tree node insert flag definitions
 */
enum LIBFDATA_TREE_NODE_INSERT_FLAGS
{
	/* Allow duplicate sub node values
	 */
	LIBFDATA_TREE_NODE_INSERT_FLAG_NON_UNIQUE_SUB_NODE_VALUES	= 0x00,

	/* Only allow unique sub node values, no duplicates
	 */
	LIBFDATA_TREE_NODE_INSERT_FLAG_UNIQUE_SUB_NODE_VALUES		= 0x01
};

#endif

/* The btree node flag definitions
 */
enum LIBFDATA_BTREE_NODE_FLAGS
{
	/* The node is a branch
	 */
	LIBFDATA_BTREE_NODE_FLAG_IS_BRANCH				= 0x02,

	/* The node is a leaf
	 */
	LIBFDATA_BTREE_NODE_FLAG_IS_LEAF				= 0x04
};

#define LIBFDATA_BTREE_NODE_LEVEL_UNLIMITED				-1

/* The list flag definitions
 */
enum LIBFDATA_LIST_FLAGS
{
	LIBFDATA_LIST_FLAG_HAS_MAPPED_OFFSET				= 0x10
};

/* The list element flag definitions
 */
enum LIBFDATA_LIST_ELEMENT_FLAGS
{
	LIBFDATA_LIST_ELEMENT_FLAG_HAS_MAPPED_SIZE			= 0x10
};

#endif

