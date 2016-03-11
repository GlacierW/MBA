/*
 * Cluster table functions
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

#if !defined( _LIBQCOW_CLUSTER_TABLE_H )
#define _LIBQCOW_CLUSTER_TABLE_H

#include <common.h>
#include <types.h>

#include "libqcow_libbfio.h"
#include "libqcow_libcerror.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libqcow_cluster_table libqcow_cluster_table_t;

struct libqcow_cluster_table
{
	/* The number of references
 	 */
	int number_of_references;

	/* The references
	 */
	uint64_t *references;
};

int libqcow_cluster_table_initialize(
     libqcow_cluster_table_t **cluster_table,
     libcerror_error_t **error );

int libqcow_cluster_table_free(
     libqcow_cluster_table_t **cluster_table,
     libcerror_error_t **error );

int libqcow_cluster_table_get_number_of_references(
     libqcow_cluster_table_t *cluster_table,
     int *number_of_references,
     libcerror_error_t **error );

int libqcow_cluster_table_get_reference_by_index(
     libqcow_cluster_table_t *cluster_table,
     int reference_index,
     uint64_t *reference,
     libcerror_error_t **error );

int libqcow_cluster_table_read(
     libqcow_cluster_table_t *cluster_table,
     libbfio_handle_t *file_io_handle,
     off64_t file_offset,
     size_t cluster_table_size,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

