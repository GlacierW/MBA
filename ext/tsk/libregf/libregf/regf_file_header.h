/*
 * The file header definition of a Windows NT Registry File (REGF)
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

#if !defined( _REGF_FILE_HEADER_H )
#define _REGF_FILE_HEADER_H

#include <common.h>
#include <types.h>

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct regf_file_header regf_file_header_t;

struct regf_file_header
{
	/* The signature
	 * Consists of 4 bytes
	 * Contains: "regf"
	 */
	uint8_t signature[ 4 ];

	/* The primary sequence number
	 * Consists of 4 bytes
	 */
	uint8_t primary_sequence_number[ 4 ];

	/* The secondary sequence number
	 * Consists of 4 bytes
	 */
	uint8_t secondary_sequence_number[ 4 ];

	/* The last modification date and time
	 * Consists of 8 bytes
	 * Contains a filetime
	 */
	uint8_t modification_time[ 8 ];

	/* The major version
	 * Consists of 4 bytes
	 */
	uint8_t major_version[ 4 ];

	/* The minor version
	 * Consists of 4 bytes
	 */
	uint8_t minor_version[ 4 ];

	/* The file type
	 * Consists of 4 bytes
	 */
	uint8_t file_type[ 4 ];

	/* Unknown
	 * Consists of 4 bytes
	 */
	uint8_t unknown3[ 4 ];

	/* The root key offset
	 * Consists of 4 bytes
	 */
	uint8_t root_key_offset[ 4 ];

	/* The hive bins size
	 * Consists of 4 bytes
	 */
	uint8_t hive_bins_size[ 4 ];

	/* Unknown
	 * Consists of 4 bytes
	 */
	uint8_t unknown4[ 4 ];

	/* Unknown
	 * Consists of 64 bytes
	 */
	uint8_t unknown5[ 64 ];

	/* Unknown
	 * Consists of 396 bytes
	 */
	uint8_t unknown6[ 396 ];

	/* The XOR-32 checksum of the previous 508 bytes
	 * Consists of 4 bytes
	 */
	uint8_t checksum[ 4 ];
};

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _REGF_FILE_HEADER_H ) */

