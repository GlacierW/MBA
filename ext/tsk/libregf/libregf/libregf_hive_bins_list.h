/*
 * Hive bins list functions
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

#if !defined( _LIBREGF_HIVE_BINS_LIST_H )
#define _LIBREGF_HIVE_BINS_LIST_H

#include <common.h>
#include <types.h>

#include "libregf_io_handle.h"
#include "libregf_libbfio.h"
#include "libregf_libcerror.h"
#include "libregf_libfcache.h"
#include "libregf_libfdata.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libregf_hive_bins_list libregf_hive_bins_list_t;

struct libregf_hive_bins_list
{
	/* The IO handle
	 */
	libregf_io_handle_t *io_handle;

	/* The data list
	 */
	libfdata_list_t *data_list;

	/* The data cache
	 */
	libfcache_cache_t *data_cache;

	/* Various flags
	 */
	uint8_t flags;
};

int libregf_hive_bins_list_initialize(
     libregf_hive_bins_list_t **hive_bins_list,
     libregf_io_handle_t *io_handle,
     libcerror_error_t **error );

int libregf_hive_bins_list_free(
     libregf_hive_bins_list_t **hive_bins_list,
     libcerror_error_t **error );

int libregf_hive_bins_list_get_index_at_offset(
     libregf_hive_bins_list_t *hive_bins_list,
     off64_t offset,
     int *hive_bin_index,
     libcerror_error_t **error );

int libregf_hive_bins_list_read(
     libregf_hive_bins_list_t *hive_bins_list,
     libbfio_handle_t *file_io_handle,
     off64_t file_offset,
     uint32_t hive_bins_size,
     libcerror_error_t **error );

int libregf_hive_bins_list_read_element_data(
     intptr_t *data_handle,
     libbfio_handle_t *file_io_handle,
     libfdata_list_element_t *element,
     libfcache_cache_t *cache,
     int data_range_file_index,
     off64_t data_range_offset,
     size64_t data_range_size,
     uint32_t data_range_flags,
     uint8_t read_flags,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBREGF_HIVE_BINS_LIST_H ) */

