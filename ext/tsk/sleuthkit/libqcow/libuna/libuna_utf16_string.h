/*
 * UTF-16 string functions
 *
 * Copyright (C) 2008-2016, Joachim Metz <joachim.metz@gmail.com>
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

#if !defined( _LIBUNA_UTF16_STRING_H )
#define _LIBUNA_UTF16_STRING_H

#include <common.h>
#include <types.h>

#include "libuna_extern.h"
#include "libuna_libcerror.h"
#include "libuna_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

LIBUNA_EXTERN \
int libuna_utf16_string_size_from_byte_stream(
     const uint8_t *byte_stream,
     size_t byte_stream_size,
     int codepage,
     size_t *utf16_string_size,
     libcerror_error_t **error );

LIBUNA_EXTERN \
int libuna_utf16_string_copy_from_byte_stream(
     libuna_utf16_character_t *utf16_string,
     size_t utf16_string_size,
     const uint8_t *byte_stream,
     size_t byte_stream_size,
     int codepage,
     libcerror_error_t **error );

/* The functionality for libuna_utf16_string_copy_to_byte_stream is implemented by
 * libuna_byte_stream_copy_from_utf16
 */

LIBUNA_EXTERN \
int libuna_utf16_string_with_index_copy_from_byte_stream(
     libuna_utf16_character_t *utf16_string,
     size_t utf16_string_size,
     size_t *utf16_string_index,
     const uint8_t *byte_stream,
     size_t byte_stream_size,
     int codepage,
     libcerror_error_t **error );

LIBUNA_EXTERN \
int libuna_utf16_string_compare_with_byte_stream(
     const libuna_utf16_character_t *utf16_string,
     size_t utf16_string_size,
     const uint8_t *byte_stream,
     size_t byte_stream_size,
     int codepage,
     libcerror_error_t **error );

LIBUNA_EXTERN \
int libuna_utf16_string_size_from_utf7_stream(
     const uint8_t *utf7_stream,
     size_t utf7_stream_size,
     size_t *utf16_string_size,
     libcerror_error_t **error );

LIBUNA_EXTERN \
int libuna_utf16_string_copy_from_utf7_stream(
     libuna_utf16_character_t *utf16_string,
     size_t utf16_string_size,
     const uint8_t *utf7_stream,
     size_t utf7_stream_size,
     libcerror_error_t **error );

/* The functionality for libuna_utf16_string_copy_to_utf7_stream is implemented by
 * libuna_utf7_stream_copy_from_utf16
 */

LIBUNA_EXTERN \
int libuna_utf16_string_with_index_copy_from_utf7_stream(
     libuna_utf16_character_t *utf16_string,
     size_t utf16_string_size,
     size_t *utf16_string_index,
     const uint8_t *utf7_stream,
     size_t utf7_stream_size,
     libcerror_error_t **error );

LIBUNA_EXTERN \
int libuna_utf16_string_compare_with_utf7_stream(
     const libuna_utf16_character_t *utf16_string,
     size_t utf16_string_size,
     const uint8_t *utf7_stream,
     size_t utf7_stream_size,
     libcerror_error_t **error );

LIBUNA_EXTERN \
int libuna_utf16_string_size_from_utf8(
     const libuna_utf8_character_t *utf8_string,
     size_t utf8_string_size,
     size_t *utf16_string_size,
     libcerror_error_t **error );

LIBUNA_EXTERN \
int libuna_utf16_string_copy_from_utf8(
     libuna_utf16_character_t *utf16_string,
     size_t utf16_string_size,
     const libuna_utf8_character_t *utf8_string,
     size_t utf8_string_size,
     libcerror_error_t **error );

/* The functionality for libuna_utf16_string_copy_to_utf8 is implemented by
 * libuna_utf8_string_copy_from_utf16
 */

LIBUNA_EXTERN \
int libuna_utf16_string_with_index_copy_from_utf8(
     libuna_utf16_character_t *utf16_string,
     size_t utf16_string_size,
     size_t *utf16_string_index,
     const libuna_utf8_character_t *utf8_string,
     size_t utf8_string_size,
     libcerror_error_t **error );

/* The functionality for libuna_utf16_string_compare_with_utf8 is implemented by
 * libuna_utf8_string_compare_with_utf16
 */

LIBUNA_EXTERN \
int libuna_utf16_string_size_from_utf8_stream(
     const uint8_t *utf16_stream,
     size_t utf16_stream_size,
     size_t *utf16_string_size,
     libcerror_error_t **error );

LIBUNA_EXTERN \
int libuna_utf16_string_copy_from_utf8_stream(
     libuna_utf16_character_t *utf16_string,
     size_t utf16_string_size,
     const uint8_t *utf8_stream,
     size_t utf8_stream_size,
     libcerror_error_t **error );

/* The functionality for libuna_utf16_string_copy_to_utf8_stream is implemented by
 * libuna_utf8_stream_copy_from_utf16
 */

LIBUNA_EXTERN \
int libuna_utf16_string_with_index_copy_from_utf8_stream(
     libuna_utf16_character_t *utf16_string,
     size_t utf16_string_size,
     size_t *utf16_string_index,
     const uint8_t *utf8_stream,
     size_t utf8_stream_size,
     libcerror_error_t **error );

LIBUNA_EXTERN \
int libuna_utf16_string_compare_with_utf8_stream(
     const libuna_utf16_character_t *utf16_string,
     size_t utf16_string_size,
     const uint8_t *utf8_stream,
     size_t utf8_stream_size,
     libcerror_error_t **error );

LIBUNA_EXTERN \
int libuna_utf16_string_size_from_utf16_stream(
     const uint8_t *utf16_stream,
     size_t utf16_stream_size,
     int byte_order,
     size_t *utf16_string_size,
     libcerror_error_t **error );

LIBUNA_EXTERN \
int libuna_utf16_string_copy_from_utf16_stream(
     libuna_utf16_character_t *utf16_string,
     size_t utf16_string_size,
     const uint8_t *utf16_stream,
     size_t utf16_stream_size,
     int byte_order,
     libcerror_error_t **error );

/* The functionality for libuna_utf16_string_copy_to_utf16_stream is implemented by
 * libuna_utf16_stream_copy_from_utf16
 */

LIBUNA_EXTERN \
int libuna_utf16_string_with_index_copy_from_utf16_stream(
     libuna_utf16_character_t *utf16_string,
     size_t utf16_string_size,
     size_t *utf16_string_index,
     const uint8_t *utf16_stream,
     size_t utf16_stream_size,
     int byte_order,
     libcerror_error_t **error );

LIBUNA_EXTERN \
int libuna_utf16_string_compare_with_utf16_stream(
     const libuna_utf16_character_t *utf16_string,
     size_t utf16_string_size,
     const uint8_t *utf16_stream,
     size_t utf16_stream_size,
     int byte_order,
     libcerror_error_t **error );

LIBUNA_EXTERN \
int libuna_utf16_string_size_from_utf32(
     const libuna_utf32_character_t *utf32_string,
     size_t utf32_string_size,
     size_t *utf16_string_size,
     libcerror_error_t **error );

LIBUNA_EXTERN \
int libuna_utf16_string_copy_from_utf32(
     libuna_utf16_character_t *utf16_string,
     size_t utf16_string_size,
     const libuna_utf32_character_t *utf32_string,
     size_t utf32_string_size,
     libcerror_error_t **error );

/* The functionality for libuna_utf16_string_copy_to_utf32 is implemented by
 * libuna_utf32_string_copy_from_utf16
 */

LIBUNA_EXTERN \
int libuna_utf16_string_with_index_copy_from_utf32(
     libuna_utf16_character_t *utf16_string,
     size_t utf16_string_size,
     size_t *utf16_string_index,
     const libuna_utf32_character_t *utf32_string,
     size_t utf32_string_size,
     libcerror_error_t **error );

LIBUNA_EXTERN \
int libuna_utf16_string_compare_with_utf32(
     const libuna_utf16_character_t *utf16_string,
     size_t utf16_string_size,
     const libuna_utf32_character_t *utf32_string,
     size_t utf32_string_size,
     libcerror_error_t **error );

LIBUNA_EXTERN \
int libuna_utf16_string_size_from_utf32_stream(
     const uint8_t *utf32_stream,
     size_t utf32_stream_size,
     int byte_order,
     size_t *utf16_string_size,
     libcerror_error_t **error );

LIBUNA_EXTERN \
int libuna_utf16_string_copy_from_utf32_stream(
     libuna_utf16_character_t *utf16_string,
     size_t utf16_string_size,
     const uint8_t *utf32_stream,
     size_t utf32_stream_size,
     int byte_order,
     libcerror_error_t **error );

/* The functionality for libuna_utf16_string_copy_to_utf32_stream is implemented by
 * libuna_utf32_stream_copy_from_utf16
 */

LIBUNA_EXTERN \
int libuna_utf16_string_with_index_copy_from_utf32_stream(
     libuna_utf16_character_t *utf16_string,
     size_t utf16_string_size,
     size_t *utf16_string_index,
     const uint8_t *utf32_stream,
     size_t utf32_stream_size,
     int byte_order,
     libcerror_error_t **error );

LIBUNA_EXTERN \
int libuna_utf16_string_compare_with_utf32_stream(
     const libuna_utf16_character_t *utf16_string,
     size_t utf16_string_size,
     const uint8_t *utf32_stream,
     size_t utf32_stream_size,
     int byte_order,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

