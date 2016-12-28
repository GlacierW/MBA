/*
 * Named key functions
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

#if !defined( _LIBREGF_NAMED_KEY_ENTRY_H )
#define _LIBREGF_NAMED_KEY_ENTRY_H

#include <common.h>
#include <types.h>

#include "libregf_libcdata.h"
#include "libregf_libcerror.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libregf_named_key libregf_named_key_t;

struct libregf_named_key
{
	/* The name hash
	 */
	uint32_t name_hash;

	/* The name
	 */
	uint8_t *name;

	/* The name size
	 */
	uint16_t name_size;

	/* The class name
	 */
	uint8_t *class_name;

	/* The class name size
	 */
	uint16_t class_name_size;

	/* The last written time
	 */
	uint64_t last_written_time;

	/* The flags
	 */
	uint16_t flags;
};

int libregf_named_key_initialize(
     libregf_named_key_t **named_key,
     libcerror_error_t **error );

int libregf_named_key_free(
     libregf_named_key_t **named_key,
     libcerror_error_t **error );

int libregf_named_key_read(
     libregf_named_key_t *named_key,
     const uint8_t *data,
     size_t data_size,
     uint32_t named_key_hash,
     int ascii_codepage,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBREGF_NAMED_KEY_ENTRY_H ) */

