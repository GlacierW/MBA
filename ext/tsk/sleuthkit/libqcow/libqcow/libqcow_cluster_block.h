/*
 * Cluster block functions
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

#if !defined( _LIBQCOW_CLUSTER_BLOCK_H )
#define _LIBQCOW_CLUSTER_BLOCK_H

#include <common.h>
#include <types.h>

#include "libqcow_libbfio.h"
#include "libqcow_libcerror.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libqcow_cluster_block libqcow_cluster_block_t;

struct libqcow_cluster_block
{
	/* The compressed data
	 */
	uint8_t *compressed_data;

	/* The encrypted data
	 */
	uint8_t *encrypted_data;

	/* The data
	 */
	uint8_t *data;

	/* The data size
	 */
	size_t data_size;
};

int libqcow_cluster_block_initialize(
     libqcow_cluster_block_t **cluster_block,
     size_t data_size,
     libcerror_error_t **error );

int libqcow_cluster_block_free(
     libqcow_cluster_block_t **cluster_block,
     libcerror_error_t **error );

int libqcow_cluster_block_read(
     libqcow_cluster_block_t *cluster_block,
     libbfio_handle_t *file_io_handle,
     off64_t cluster_offset,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

