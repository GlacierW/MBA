/*
 * The file header definition of a QEMU Copy-On-Write (QCOW) image file
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

#if !defined( _QCOW_FILE_HEADER_H )
#define _QCOW_FILE_HEADER_H

#include <common.h>
#include <types.h>

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct qcow_file_header_v1 qcow_file_header_v1_t;

struct qcow_file_header_v1
{
	/* The file signature
	 * Consists of 4 bytes
	 * Consists of: 0x51 0x46 0x49 0xfb
	 */
	uint8_t signature[ 4 ];

	/* The format version
	 * Consists of 4 bytes
	 */
	uint8_t format_version[ 4 ];

	/* The backing filename offset
	 * Consists of 8 bytes
	 */
	uint8_t backing_filename_offset[ 8 ];

	/* The backing filename size
	 * Consists of 4 bytes
	 */
	uint8_t backing_filename_size[ 4 ];

	/* The modification date and time
	 * Consists of 4 bytes
	 */
	uint8_t modification_time[ 4 ];

	/* The media size
	 * Consists of 8 bytes
	 */
	uint8_t media_size[ 8 ];

	/* The number of cluster block bits
	 * Consists of 1 byte
	 */
	uint8_t number_of_cluster_block_bits;

	/* The number of level 2 table bits
	 * Consists of 1 byte
	 */
	uint8_t number_of_level2_table_bits;

	/* Unknown
	 * Consists of 2 bytes
	 */
	uint8_t unknown1[ 2 ];

	/* The encryption method
	 * Consists of 4 bytes
	 */
	uint8_t encryption_method[ 4 ];

	/* The level 1 table offset
	 * Consists of 8 bytes
	 */
	uint8_t level1_table_offset[ 8 ];
};

typedef struct qcow_file_header_v2 qcow_file_header_v2_t;

struct qcow_file_header_v2
{
	/* The file signature
	 * Consists of 4 bytes
	 * Consists of: 0x51 0x46 0x49 0xfb
	 */
	uint8_t signature[ 4 ];

	/* The format version
	 * Consists of 4 bytes
	 */
	uint8_t format_version[ 4 ];

	/* The backing filename offset
	 * Consists of 8 bytes
	 */
	uint8_t backing_filename_offset[ 8 ];

	/* The backing filename size
	 * Consists of 4 bytes
	 */
	uint8_t backing_filename_size[ 4 ];

	/* The number of cluster block bits
	 * Consists of 4 bytes
	 */
	uint8_t number_of_cluster_block_bits[ 4 ];

	/* The media size
	 * Consists of 8 bytes
	 */
	uint8_t media_size[ 8 ];

	/* The encryption method
	 * Consists of 4 bytes
	 */
	uint8_t encryption_method[ 4 ];

	/* The number of level 1 table references
	 * Consists of 4 bytes
	 */
	uint8_t number_of_level1_table_references[ 4 ];

	/* The level 1 table offset
	 * Consists of 8 bytes
	 */
	uint8_t level1_table_offset[ 8 ];

	/* The reference count table offset
	 * Consists of 8 bytes
	 */
	uint8_t reference_count_table_offset[ 8 ];

	/* The reference count table clusters
	 * Consists of 4 bytes
	 */
	uint8_t reference_count_table_clusters[ 4 ];

	/* The number of snapshots
	 * Consists of 4 bytes
	 */
	uint8_t number_of_snapshots[ 4 ];

	/* The snapshots offset
	 * Consists of 8 bytes
	 */
	uint8_t snapshots_offset[ 8 ];
};

#if defined( __cplusplus )
}
#endif

#endif

