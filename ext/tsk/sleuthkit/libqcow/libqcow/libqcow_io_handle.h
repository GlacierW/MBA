/*
 * Input/Output (IO) handle functions
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

#if !defined( _LIBQCOW_IO_HANDLE_H )
#define _LIBQCOW_IO_HANDLE_H

#include <common.h>
#include <types.h>

#include "libqcow_libbfio.h"
#include "libqcow_libcerror.h"
#include "libqcow_libfcache.h"
#include "libqcow_libfdata.h"

#if defined( __cplusplus )
extern "C" {
#endif

extern const uint8_t qcow_file_signature[ 4 ];

typedef struct libqcow_io_handle libqcow_io_handle_t;

struct libqcow_io_handle
{
	/* The (storage) media size
	 */
	size64_t media_size;

	/* The format version
	 */
	uint32_t format_version;

	/* The level 1 table offset
 	 */
	off64_t level1_table_offset;

	/* The level 1 table size
 	 */
	uint32_t level1_table_size;

	/* The number of cluster block bits
 	 */
	uint32_t number_of_cluster_block_bits;

	/* The number of level 2 table bits
 	 */
	uint32_t number_of_level2_table_bits;

	/* The level 1 index bit shift
 	 */
	uint32_t level1_index_bit_shift;

	/* The level 2 index bit mask
 	 */
	uint64_t level2_index_bit_mask;

	/* The cluster block bit mask
 	 */
	uint64_t cluster_block_bit_mask;

	/* The offset bit mask
 	 */
	uint64_t offset_bit_mask;

	/* The compression flag bit mask
 	 */
	uint64_t compression_flag_bit_mask;

	/* The compression (offset) bit mask
 	 */
	uint64_t compression_bit_mask;

	/* The compression (size) bit shift
 	 */
	uint64_t compression_bit_shift;

	/* The level 2 table size
 	 */
	size_t level2_table_size;

	/* The cluster block size
 	 */
	size_t cluster_block_size;

	/* The backing filename
	 */
	uint8_t *backing_filename;

	/* The backing filename size
	 */
	size_t backing_filename_size;
};

int libqcow_io_handle_initialize(
     libqcow_io_handle_t **io_handle,
     libcerror_error_t **error );

int libqcow_io_handle_free(
     libqcow_io_handle_t **io_handle,
     libcerror_error_t **error );

int libqcow_io_handle_clear(
     libqcow_io_handle_t *io_handle,
     libcerror_error_t **error );

int libqcow_io_handle_read_file_header(
     libqcow_io_handle_t *io_handle,
     libbfio_handle_t *file_io_handle,
     uint32_t *encryption_method,
     libcerror_error_t **error );

int libqcow_io_handle_read_level2_table(
     intptr_t *data_handle,
     libbfio_handle_t *file_io_handle,
     libfdata_vector_t *vector,
     libfcache_cache_t *cache,
     int element_index,
     int element_data_file_index,
     off64_t element_data_offset,
     size64_t element_data_size,
     uint32_t element_data_flags,
     uint8_t read_flags,
     libcerror_error_t **error );

int libqcow_io_handle_read_cluster_block(
     intptr_t *data_handle,
     libbfio_handle_t *file_io_handle,
     libfdata_vector_t *vector,
     libfcache_cache_t *cache,
     int element_index,
     int element_data_file_index,
     off64_t element_data_offset,
     size64_t element_data_size,
     uint32_t element_data_flags,
     uint8_t read_flags,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

