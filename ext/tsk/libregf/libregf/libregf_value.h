/*
 * Value functions
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

#if !defined( _LIBREGF_INTERNAL_VALUE_H )
#define _LIBREGF_INTERNAL_VALUE_H

#include <common.h>
#include <types.h>

#include "libregf_extern.h"
#include "libregf_io_handle.h"
#include "libregf_libbfio.h"
#include "libregf_libcerror.h"
#include "libregf_libfcache.h"
#include "libregf_libfdata.h"
#include "libregf_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libregf_internal_value libregf_internal_value_t;

struct libregf_internal_value
{
	/* The IO handle
	 */
	libregf_io_handle_t *io_handle;

	/* The file IO handle
	 */
	libbfio_handle_t *file_io_handle;

	/* The values list element
	 */
	libfdata_list_element_t *values_list_element;

	/* The values cache
	 */
	libfcache_cache_t *values_cache;
};

int libregf_value_initialize(
     libregf_value_t **value,
     libregf_io_handle_t *io_handle,
     libbfio_handle_t *file_io_handle,
     libfdata_list_element_t *values_list_element,
     libfcache_cache_t *values_cache,
     libcerror_error_t **error );

LIBREGF_EXTERN \
int libregf_value_free(
     libregf_value_t **value,
     libcerror_error_t **error );

LIBREGF_EXTERN \
int libregf_value_is_corrupted(
     libregf_value_t *value,
     libcerror_error_t **error );

LIBREGF_EXTERN \
int libregf_value_get_offset(
     libregf_value_t *value,
     off64_t *offset,
     libcerror_error_t **error );

LIBREGF_EXTERN \
int libregf_value_get_name_size(
     libregf_value_t *value,
     size_t *name_size,
     libcerror_error_t **error );

LIBREGF_EXTERN \
int libregf_value_get_name(
     libregf_value_t *value,
     uint8_t *name,
     size_t name_size,
     libcerror_error_t **error );

LIBREGF_EXTERN \
int libregf_value_get_utf8_name_size(
     libregf_value_t *value,
     size_t *utf8_name_size,
     libcerror_error_t **error );

LIBREGF_EXTERN \
int libregf_value_get_utf8_name(
     libregf_value_t *value,
     uint8_t *utf8_name,
     size_t utf8_name_size,
     libcerror_error_t **error );

LIBREGF_EXTERN \
int libregf_value_get_utf16_name_size(
     libregf_value_t *value,
     size_t *utf16_name_size,
     libcerror_error_t **error );

LIBREGF_EXTERN \
int libregf_value_get_utf16_name(
     libregf_value_t *value,
     uint16_t *utf16_name,
     size_t utf16_name_size,
     libcerror_error_t **error );

LIBREGF_EXTERN \
int libregf_value_get_value_type(
     libregf_value_t *value,
     uint32_t *value_type,
     libcerror_error_t **error );

LIBREGF_EXTERN \
int libregf_value_get_value_data_size(
     libregf_value_t *value,
     size_t *value_data_size,
     libcerror_error_t **error );

LIBREGF_EXTERN \
int libregf_value_get_value_data(
     libregf_value_t *value,
     uint8_t *value_data,
     size_t value_data_size,
     libcerror_error_t **error );

LIBREGF_EXTERN \
int libregf_value_get_value_32bit(
     libregf_value_t *value,
     uint32_t *value_32bit,
     libcerror_error_t **error );

LIBREGF_EXTERN \
int libregf_value_get_value_64bit(
     libregf_value_t *value,
     uint64_t *value_64bit,
     libcerror_error_t **error );

LIBREGF_EXTERN \
int libregf_value_get_value_utf8_string_size(
     libregf_value_t *value,
     size_t *utf8_string_size,
     libcerror_error_t **error );

LIBREGF_EXTERN \
int libregf_value_get_value_utf8_string(
     libregf_value_t *value,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     libcerror_error_t **error );

LIBREGF_EXTERN \
int libregf_value_get_value_utf16_string_size(
     libregf_value_t *value,
     size_t *utf16_string_size,
     libcerror_error_t **error );

LIBREGF_EXTERN \
int libregf_value_get_value_utf16_string(
     libregf_value_t *value,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     libcerror_error_t **error );

LIBREGF_EXTERN \
int libregf_value_get_value_binary_data_size(
     libregf_value_t *value,
     size_t *size,
     libcerror_error_t **error );

LIBREGF_EXTERN \
int libregf_value_get_value_binary_data(
     libregf_value_t *value,
     uint8_t *binary_data,
     size_t size,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBREGF_INTERNAL_VALUE_H ) */

