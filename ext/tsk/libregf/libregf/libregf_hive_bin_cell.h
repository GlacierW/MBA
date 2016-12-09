/*
 * Hive bin cell functions
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

#if !defined( _LIBREGF_HIVE_BIN_CELL_H )
#define _LIBREGF_HIVE_BIN_CELL_H

#include <common.h>
#include <types.h>

#include "libregf_libcerror.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libregf_hive_bin_cell libregf_hive_bin_cell_t;

struct libregf_hive_bin_cell
{
	/* The offset of the cell data
	 */
	uint32_t offset;

	/* The reference to the hive bin cell data
	 */
	uint8_t *data;

	/* The hive bin cell size
	 */
	uint32_t size;

	/* The flags of the cell
	 */
	uint8_t flags;
};

int libregf_hive_bin_cell_initialize(
     libregf_hive_bin_cell_t **hive_bin_cell,
     libcerror_error_t **error );

int libregf_hive_bin_cell_free(
     libregf_hive_bin_cell_t **hive_bin_cell,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBREGF_HIVE_BIN_CELL_H ) */

