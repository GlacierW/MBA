/*
 * Bit stream functions
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

#if !defined( _LIBFWNT_BIT_STREAM_H )
#define _LIBFWNT_BIT_STREAM_H

#include <common.h>
#include <types.h>

#include "libfwnt_libcerror.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libfwnt_bit_stream libfwnt_bit_stream_t;

struct libfwnt_bit_stream
{
	/* The byte stream
	 */
	const uint8_t *byte_stream;

	/* The byte stream offset
	 */
	size_t byte_stream_offset;

	/* The byte stream size
	 */
	size_t byte_stream_size;

	/* The bits
	 */
	uint32_t bits;

	/* The number of bits
	 */
	uint8_t number_of_bits;
};

int libfwnt_bit_stream_initialize(
     libfwnt_bit_stream_t **bit_stream,
     const uint8_t *byte_stream,
     size_t byte_stream_size,
     libcerror_error_t **error );

int libfwnt_bit_stream_free(
     libfwnt_bit_stream_t **bit_stream,
     libcerror_error_t **error );

ssize_t libfwnt_bit_stream_read(
         libfwnt_bit_stream_t *bit_stream,
         size_t read_size,
         libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBFWNT_BIT_STREAM_H ) */

