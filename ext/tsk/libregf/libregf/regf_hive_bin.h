/*
 * The hive bin definition of a Windows NT Registry File (REGF)
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

#if !defined( _REGF_HIVE_BIN_H )
#define _REGF_HIVE_BIN_H

#include <common.h>
#include <types.h>

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct regf_hive_bin_header regf_hive_bin_header_t;

struct regf_hive_bin_header
{
	/* The signature
	 * Consists of 4 bytes
	 * Contains: "hbin"
	 */
	uint8_t signature[ 4 ];

	/* The offset of the hive bin relative from the start
	 * of the hive bin data
	 * Consists of 4 bytes
	 * Contains the number of bytes
	 */
	uint8_t hive_bin_offset[ 4 ];

	/* The size of the hive bin
	 * Consists of 4 bytes
	 * Contains the number of bytes
	 */
	uint8_t size[ 4 ];

	/* Unknown
	 * Consists of 4 bytes
	 */
	uint8_t unknown1[ 4 ];

	/* Unknown
	 * Consists of 4 bytes
	 */
	uint8_t unknown2[ 4 ];

	/* Unknown date and time
	 * Consists of 8 bytes
	 */
	uint8_t unknown_time[ 8 ];

	/* Unknown spare
	 * Consists of 4 bytes
	 */
	uint8_t unknown_spare[ 4 ];
};

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _REGF_HIVE_BIN_H ) */

