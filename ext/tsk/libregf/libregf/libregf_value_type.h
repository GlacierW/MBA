/*
 * Values type functions
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

#if !defined( _LIBREGF_VALUE_TYPE_H )
#define _LIBREGF_VALUE_TYPE_H

#include <common.h>
#include <types.h>

#include "libregf_libcerror.h"

#if defined( __cplusplus )
extern "C" {
#endif

int libregf_value_type_copy_to_32bit(
     uint8_t *value_data,
     size_t value_data_size,
     uint8_t byte_order,
     uint32_t *value_32bit,
     libcerror_error_t **error );

int libregf_value_type_copy_to_64bit(
     uint8_t *value_data,
     size_t value_data_size,
     uint8_t byte_order,
     uint64_t *value_64bit,
     libcerror_error_t **error );

int libregf_value_type_get_utf8_string_size(
     uint8_t *value_data,
     size_t value_data_size,
     size_t *utf8_string_size,
     libcerror_error_t **error );

int libregf_value_type_copy_to_utf8_string(
     uint8_t *value_data,
     size_t value_data_size,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     libcerror_error_t **error );

int libregf_value_type_get_utf16_string_size(
     uint8_t *value_data,
     size_t value_data_size,
     size_t *utf16_string_size,
     libcerror_error_t **error );

int libregf_value_type_copy_to_utf16_string(
     uint8_t *value_data,
     size_t value_data_size,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     libcerror_error_t **error );

int libregf_value_type_get_binary_data_size(
     uint8_t *value_data,
     size_t value_data_size,
     size_t *binary_data_size,
     libcerror_error_t **error );

int libregf_value_type_copy_to_binary_data(
     uint8_t *value_data,
     size_t value_data_size,
     uint8_t *binary_data,
     size_t binary_data_size,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBREGF_VALUE_TYPE_H ) */

