/*
 * The cell values definition of a Windows NT Registry File (REGF)
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

#if !defined( _REGF_CELL_VALUES_H )
#define _REGF_CELL_VALUES_H

#include <common.h>
#include <types.h>

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct regf_named_key regf_named_key_t;

struct regf_named_key
{
	/* The signature
	 * Consists of 2 bytes
	 * Contains: "nk"
	 */
	uint8_t signature[ 2 ];

	/* The flags
	 * Consists of 2 bytes
	 */
	uint8_t flags[ 2 ];

	/* The key last written date and time
	 * Consists of 8 bytes
	 */
	uint8_t last_written_time[ 8 ];

	/* Unknown
	 * Consists of 4 bytes
	 */
	uint8_t unknown1[ 4 ];

	/* The parent key offset
	 * Consists of 4 bytes
	 */
	uint8_t parent_key_offset[ 4 ];

	/* The number of sub keys
	 * Consists of 4 bytes
	 */
	uint8_t number_of_sub_keys[ 4 ];

	/* The number of volatile sub keys
	 * Consists of 4 bytes
	 */
	uint8_t number_of_volatile_sub_keys[ 4 ];

	/* The sub keys list offset
	 * Consists of 4 bytes
	 */
	uint8_t sub_keys_list_offset[ 4 ];

	/* The volatile sub keys list offset
	 * Consists of 4 bytes
	 */
	uint8_t volatile_sub_keys_list_offset[ 4 ];

	/* The number of values
	 * Consists of 4 bytes
	 */
	uint8_t number_of_values[ 4 ];

	/* The values list offset
	 * Consists of 4 bytes
	 */
	uint8_t values_list_offset[ 4 ];

	/* The security key offset
	 * Consists of 4 bytes
	 */
	uint8_t security_key_offset[ 4 ];

	/* The class name offset
	 * Consists of 4 bytes
	 */
	uint8_t class_name_offset[ 4 ];

	/* The largest sub key name size
	 * Consists of 4 bytes
	 */
	uint8_t largest_sub_key_name_size[ 4 ];

	/* The largest sub key class name size
	 * Consists of 4 bytes
	 */
	uint8_t largest_sub_key_class_name_size[ 4 ];

	/* The largest value name size
	 * Consists of 4 bytes
	 */
	uint8_t largest_value_name_size[ 4 ];

	/* The largest value data size
	 * Consists of 4 bytes
	 */
	uint8_t largest_value_data_size[ 4 ];

	/* Unknown
	 * Consists of 4 bytes
	 */
	uint8_t unknown6[ 4 ];

	/* The key name size
	 * Consists of 2 bytes
	 */
	uint8_t key_name_size[ 2 ];

	/* The class name size
	 * Consists of 2 bytes
	 */
	uint8_t class_name_size[ 2 ];

	/* The key name
	 */
};

typedef struct regf_sub_key_list regf_sub_key_list_t;

struct regf_sub_key_list
{
	/* The signature
	 * Consists of 2 bytes
	 * Contains: "lf", "lh", "li" or "ri"
	 */
	uint8_t signature[ 2 ];

	/* The number of elements
	 * Consists of 2 bytes
	 */
	uint8_t number_of_elements[ 2 ];
};

typedef struct regf_security_key regf_security_key_t;

struct regf_security_key
{
	/* The signature
	 * Consists of 2 bytes
	 * Contains: "sk"
	 */
	uint8_t signature[ 2 ];

	/* Unknown
	 * Consists of 2 bytes
	 */
	uint8_t unknown1[ 2 ];

	/* The previous security key offset
	 * Consists of 4 bytes
	 */
	uint8_t previous_security_key_offset[ 4 ];

	/* The next security key offset
	 * Consists of 4 bytes
	 */
	uint8_t next_security_key_offset[ 4 ];

	/* The reference count
	 * Consists of 4 bytes
	 */
	uint8_t reference_count[ 4 ];
};

typedef struct regf_value_key regf_value_key_t;

struct regf_value_key
{
	/* The signature
	 * Consists of 2 bytes
	 * Contains: "vk"
	 */
	uint8_t signature[ 2 ];

	/* The value name size
	 * Consists of 2 bytes
	 */
	uint8_t value_name_size[ 2 ];

	/* The data size
	 * Consists of 4 bytes
	 */
	uint8_t data_size[ 4 ];

	/* The data offset
	 * Consists of 4 bytes
	 */
	uint8_t data_offset[ 4 ];

	/* The data type
	 * Consists of 4 bytes
	 */
	uint8_t data_type[ 4 ];

	/* The flags
	 * Consists of 2 bytes
	 */
	uint8_t flags[ 2 ];

	/* Unknown
	 * Consists of 2 bytes
	 */
	uint8_t unknown1[ 2 ];

	/* The value name
	 */
};

typedef struct regf_data_block_key regf_data_block_key_t;

struct regf_data_block_key
{
	/* The signature
	 * Consists of 2 bytes
	 * Contains: "db"
	 */
	uint8_t signature[ 2 ];

	/* The number of segments
	 * Consists of 2 bytes
	 */
	uint8_t number_of_segments[ 2 ];

	/* The data block segment list offset
	 * Consists of 4 bytes
	 */
	uint8_t data_block_list_offset[ 4 ];
};

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _REGF_CELL_VALUES_H ) */

