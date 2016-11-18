/*
 * Unicode character functions
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

#if !defined( _LIBUNA_UNICDODE_CHARACTER_H )
#define _LIBUNA_UNICDODE_CHARACTER_H

#include <common.h>
#include <types.h>

#include "libuna_definitions.h"
#include "libuna_extern.h"
#include "libuna_inline.h"
#include "libuna_libcerror.h"
#include "libuna_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

LIBUNA_EXTERN \
LIBUNA_INLINE \
int libuna_unicode_character_size_to_byte_stream(
     libuna_unicode_character_t unicode_character,
     int codepage,
     size_t *byte_stream_character_size,
     libcerror_error_t **error );

LIBUNA_EXTERN \
LIBUNA_INLINE \
int libuna_unicode_character_copy_from_byte_stream(
     libuna_unicode_character_t *unicode_character,
     const uint8_t *byte_stream,
     size_t byte_stream_size,
     size_t *byte_stream_index,
     int codepage,
     libcerror_error_t **error );

LIBUNA_EXTERN \
LIBUNA_INLINE \
int libuna_unicode_character_copy_to_byte_stream(
     libuna_unicode_character_t unicode_character,
     uint8_t *byte_stream,
     size_t byte_stream_size,
     size_t *byte_stream_index,
     int codepage,
     libcerror_error_t **error );

LIBUNA_EXTERN \
LIBUNA_INLINE \
int libuna_unicode_character_size_to_utf7_stream(
     libuna_unicode_character_t unicode_character,
     size_t *utf7_stream_character_size,
     uint32_t *utf7_stream_base64_data,
     libcerror_error_t **error );

LIBUNA_EXTERN \
LIBUNA_INLINE \
int libuna_unicode_character_copy_from_utf7_stream(
     libuna_unicode_character_t *unicode_character,
     const uint8_t *utf7_stream,
     size_t utf7_stream_size,
     size_t *utf7_stream_index,
     uint32_t *utf7_stream_base64_data,
     libcerror_error_t **error );

LIBUNA_EXTERN \
LIBUNA_INLINE \
int libuna_unicode_character_copy_to_utf7_stream(
     libuna_unicode_character_t unicode_character,
     uint8_t *utf7_stream,
     size_t utf7_stream_size,
     size_t *utf7_stream_index,
     uint32_t *utf7_stream_base64_data,
     libcerror_error_t **error );

LIBUNA_EXTERN \
LIBUNA_INLINE \
int libuna_unicode_character_size_to_utf8(
     libuna_unicode_character_t unicode_character,
     size_t *utf8_character_size,
     libcerror_error_t **error );

LIBUNA_EXTERN \
LIBUNA_INLINE \
int libuna_unicode_character_copy_from_utf8(
     libuna_unicode_character_t *unicode_character,
     const libuna_utf8_character_t *utf8_string,
     size_t utf8_string_size,
     size_t *utf8_string_index,
     libcerror_error_t **error );

LIBUNA_EXTERN \
LIBUNA_INLINE \
int libuna_unicode_character_copy_to_utf8(
     libuna_unicode_character_t unicode_character,
     libuna_utf8_character_t *utf8_string,
     size_t utf8_string_size,
     size_t *utf8_string_index,
     libcerror_error_t **error );

LIBUNA_EXTERN \
LIBUNA_INLINE \
int libuna_unicode_character_size_to_utf16(
     libuna_unicode_character_t unicode_character,
     size_t *utf16_character_size,
     libcerror_error_t **error );

LIBUNA_EXTERN \
LIBUNA_INLINE \
int libuna_unicode_character_copy_from_utf16(
     libuna_unicode_character_t *unicode_character,
     const libuna_utf16_character_t *utf16_string,
     size_t utf16_string_size,
     size_t *utf16_string_index,
     libcerror_error_t **error );

LIBUNA_EXTERN \
LIBUNA_INLINE \
int libuna_unicode_character_copy_to_utf16(
     libuna_unicode_character_t unicode_character,
     libuna_utf16_character_t *utf16_string,
     size_t utf16_string_size,
     size_t *utf16_string_index,
     libcerror_error_t **error );

LIBUNA_EXTERN \
LIBUNA_INLINE \
int libuna_unicode_character_copy_from_utf16_stream(
     libuna_unicode_character_t *unicode_character,
     const uint8_t *utf16_stream,
     size_t utf16_stream_size,
     size_t *utf16_stream_index,
     int byte_order,
     libcerror_error_t **error );

LIBUNA_EXTERN \
LIBUNA_INLINE \
int libuna_unicode_character_copy_to_utf16_stream(
     libuna_unicode_character_t unicode_character,
     uint8_t *utf16_stream,
     size_t utf16_stream_size,
     size_t *utf16_stream_index,
     int byte_order,
     libcerror_error_t **error );

LIBUNA_EXTERN \
LIBUNA_INLINE \
int libuna_unicode_character_size_to_utf32(
     libuna_unicode_character_t unicode_character,
     size_t *utf32_character_size,
     libcerror_error_t **error );

LIBUNA_EXTERN \
LIBUNA_INLINE \
int libuna_unicode_character_copy_from_utf32(
     libuna_unicode_character_t *unicode_character,
     const libuna_utf32_character_t *utf32_string,
     size_t utf32_string_size,
     size_t *utf32_string_index,
     libcerror_error_t **error );

LIBUNA_EXTERN \
LIBUNA_INLINE \
int libuna_unicode_character_copy_to_utf32(
     libuna_unicode_character_t unicode_character,
     libuna_utf32_character_t *utf32_string,
     size_t utf32_string_size,
     size_t *utf32_string_index,
     libcerror_error_t **error );

LIBUNA_EXTERN \
LIBUNA_INLINE \
int libuna_unicode_character_copy_from_utf32_stream(
     libuna_unicode_character_t *unicode_character,
     const uint8_t *utf32_stream,
     size_t utf32_stream_size,
     size_t *utf32_stream_index,
     int byte_order,
     libcerror_error_t **error );

LIBUNA_EXTERN \
LIBUNA_INLINE \
int libuna_unicode_character_copy_to_utf32_stream(
     libuna_unicode_character_t unicode_character,
     uint8_t *utf32_stream,
     size_t utf32_stream_size,
     size_t *utf32_stream_index,
     int byte_order,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

