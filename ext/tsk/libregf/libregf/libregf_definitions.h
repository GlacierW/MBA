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

#if !defined( _LIBREGF_INTERNAL_DEFINITIONS_H )
#define _LIBREGF_INTERNAL_DEFINITIONS_H

#include <common.h>
#include <byte_stream.h>

#define LIBREGF_ENDIAN_BIG				_BYTE_STREAM_ENDIAN_BIG
#define LIBREGF_ENDIAN_LITTLE				_BYTE_STREAM_ENDIAN_LITTLE

/* Define HAVE_LOCAL_LIBREGF for local use of libregf
 */
#if !defined( HAVE_LOCAL_LIBREGF )
#include <libregf/definitions.h>

/* The definitions in <libregf/definitions.h> are copied here
 * for local use of libregf
 */
#else
#define LIBREGF_VERSION					20161108

/* The libregf version string
 */
#define LIBREGF_VERSION_STRING				"20161108"

/* The libregf file access
 * bit 1        set to 1 for read access
 * bit 2        set to 1 for write access
 * bit 3-8      not used
 */
enum LIBREGF_ACCESS_FLAGS
{
	LIBREGF_ACCESS_FLAG_READ			= 0x01,
/* Reserved: not supported yet */
	LIBREGF_ACCESS_FLAG_WRITE			= 0x02
};

/* The libregf file access macros
 */
#define LIBREGF_OPEN_READ				( LIBREGF_ACCESS_FLAG_READ )
/* Reserved: not supported yet */
#define LIBREGF_OPEN_WRITE				( LIBREGF_ACCESS_FLAG_WRITE )
/* Reserved: not supported yet */
#define LIBREGF_OPEN_READ_WRITE				( LIBREGF_ACCESS_FLAG_READ | LIBREGF_ACCESS_FLAG_WRITE )

/* The key path segment separator
 */
#define LIBREGF_SEPARATOR				'\\'

/* The file types
 */
enum LIBREGF_FILE_TYPES
{
	LIBREGF_FILE_TYPE_REGISTRY			= 0x00000000UL,
	LIBREGF_FILE_TYPE_TRANSACTION_LOG		= 0x00000001UL
};

/* The value types
 */
enum LIBREGF_VALUE_TYPES
{
	LIBREGF_VALUE_TYPE_UNDEFINED			= 0,
	LIBREGF_VALUE_TYPE_STRING			= 1,
	LIBREGF_VALUE_TYPE_EXPANDABLE_STRING		= 2,
	LIBREGF_VALUE_TYPE_BINARY_DATA			= 3,
	LIBREGF_VALUE_TYPE_INTEGER_32BIT_LITTLE_ENDIAN	= 4,
	LIBREGF_VALUE_TYPE_INTEGER_32BIT_BIG_ENDIAN	= 5,
	LIBREGF_VALUE_TYPE_SYMBOLIC_LINK		= 6,
	LIBREGF_VALUE_TYPE_MULTI_VALUE_STRING		= 7,
	LIBREGF_VALUE_TYPE_RESOURCE_LIST		= 8,
	LIBREGF_VALUE_TYPE_FULL_RESOURCE_DESCRIPTOR	= 9,
	LIBREGF_VALUE_TYPE_RESOURCE_REQUIREMENTS_LIST	= 10,
	LIBREGF_VALUE_TYPE_INTEGER_64BIT_LITTLE_ENDIAN	= 11
};

#endif

/* The IO handle flags
 */
enum LIBREGF_IO_HANDLE_FLAGS
{
	/* The file is corrupted
	 */
	LIBREGF_IO_HANDLE_FLAG_IS_CORRUPTED		= 0x01
};

/* The hive bins flags
 */
enum LIBREGF_HIVE_BINS_FLAGS
{
	/* The hive bins are corrupted
	 */
	LIBREGF_HIVE_BINS_FLAG_IS_CORRUPTED		= 0x01
};

/* The key item flags
 */
enum LIBREGF_KEY_ITEM_FLAGS
{
	/* The key item is corrupted
	 */
	LIBREGF_KEY_ITEM_FLAG_IS_CORRUPTED		= 0x01
};

/* The value item data types
 */
enum LIBREGF_VALUE_ITEM_DATA_TYPES
{
	LIBREGF_VALUE_ITEM_DATA_TYPE_BUFFER		= 1,
	LIBREGF_VALUE_ITEM_DATA_TYPE_BLOCK		= 2
};

/* The value item flags
 */
enum LIBREGF_VALUE_ITEM_FLAGS
{
	/* The value item is corrupted
	 */
	LIBREGF_VALUE_ITEM_FLAG_IS_CORRUPTED		= 0x01
};

/* The hive bin cell flags
 */
enum LIBREGF_HIVE_BIN_CELL_FLAGS
{
	LIBREGF_HIVE_BIN_CELL_FLAG_UNALLOCATED		= 0x01
};

/* The named key flags
 */
enum LIBREGF_NAMED_KEY_FLAGS
{
	LIBREGF_NAMED_KEY_FLAG_NAME_IS_ASCII		= 0x0020,
};

/* The value key flags
 */
enum LIBREGF_VALUE_KEY_FLAGS
{
	LIBREGF_VALUE_KEY_FLAG_NAME_IS_ASCII		= 0x0001,
};

#define LIBREGF_MAXIMUM_CACHE_ENTRIES_HIVE_BINS		64 * 1024
#define LIBREGF_MAXIMUM_CACHE_ENTRIES_KEYS		256 * 1024
#define LIBREGF_MAXIMUM_CACHE_ENTRIES_VALUES		128

#endif

