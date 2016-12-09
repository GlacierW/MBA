/*
 * Hive Bin functions
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

#if !defined( _LIBREGF_HIVE_BIN_H )
#define _LIBREGF_HIVE_BIN_H

#include <common.h>
#include <types.h>

#include "libregf_hive_bin_cell.h"
#include "libregf_libbfio.h"
#include "libregf_libcdata.h"
#include "libregf_libcerror.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libregf_hive_bin libregf_hive_bin_t;

struct libregf_hive_bin
{
	/* The offset of the hive bin
	 */
	uint32_t offset;

	/* The size of the hive bin
	 */
	uint32_t size;

	/* The hive bin (cells) data
	 */
	uint8_t *data;

	/* The hive bin (cells) data size
	 */
	size_t data_size;

	/* The hive bin cells array
	 */
	libcdata_array_t *cells_array;
};

int libregf_hive_bin_initialize(
     libregf_hive_bin_t **hive_bin,
     libcerror_error_t **error );

int libregf_hive_bin_free(
     libregf_hive_bin_t **hive_bin,
     libcerror_error_t **error );

int libregf_hive_bin_read_header(
     libregf_hive_bin_t *hive_bin,
     libbfio_handle_t *file_io_handle,
     libcerror_error_t **error );

int libregf_hive_bin_read_cells(
     libregf_hive_bin_t *hive_bin,
     libbfio_handle_t *file_io_handle,
     libcerror_error_t **error );

int libregf_hive_bin_get_number_of_cells(
     libregf_hive_bin_t *hive_bin,
     uint16_t *number_of_cells,
     libcerror_error_t **error );

int libregf_hive_bin_get_cell(
     libregf_hive_bin_t *hive_bin,
     uint16_t cell_index,
     libregf_hive_bin_cell_t **hive_bin_cell,
     libcerror_error_t **error );

int libregf_hive_bin_get_cell_at_offset(
     libregf_hive_bin_t *hive_bin,
     uint32_t cell_offset,
     libregf_hive_bin_cell_t **hive_bin_cell,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBREGF_HIVE_BIN_H ) */

