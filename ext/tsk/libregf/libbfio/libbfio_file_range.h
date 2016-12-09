/*
 * File range functions
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

#if !defined( _LIBBFIO_FILE_RANGE_H )
#define _LIBBFIO_FILE_RANGE_H

#include <common.h>
#include <types.h>

#include "libbfio_extern.h"
#include "libbfio_file.h"
#include "libbfio_libcerror.h"
#include "libbfio_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libbfio_file_range_io_handle libbfio_file_range_io_handle_t;

struct libbfio_file_range_io_handle
{
	/* The file IO handle
	 */
	libbfio_file_io_handle_t *file_io_handle;

	/* The range (start) offset
	 */
	off64_t range_offset;

	/* The range size
	 */
	size64_t range_size;
};

int libbfio_file_range_io_handle_initialize(
     libbfio_file_range_io_handle_t **file_range_io_handle,
     libcerror_error_t **error );

LIBBFIO_EXTERN \
int libbfio_file_range_initialize(
     libbfio_handle_t **handle,
     libcerror_error_t **error );

int libbfio_file_range_io_handle_free(
     libbfio_file_range_io_handle_t **file_range_io_handle,
     libcerror_error_t **error );

int libbfio_file_range_io_handle_clone(
     libbfio_file_range_io_handle_t **destination_file_range_io_handle,
     libbfio_file_range_io_handle_t *source_file_range_io_handle,
     libcerror_error_t **error );

LIBBFIO_EXTERN \
int libbfio_file_range_get_name_size(
     libbfio_handle_t *handle,
     size_t *name_size,
     libcerror_error_t **error );

LIBBFIO_EXTERN \
int libbfio_file_range_get_name(
     libbfio_handle_t *handle,
     char *name,
     size_t name_size,
     libcerror_error_t **error );

LIBBFIO_EXTERN \
int libbfio_file_range_set_name(
     libbfio_handle_t *handle,
     const char *name,
     size_t name_length,
     libcerror_error_t **error );

#if defined( HAVE_WIDE_CHARACTER_TYPE )
LIBBFIO_EXTERN \
int libbfio_file_range_get_name_size_wide(
     libbfio_handle_t *handle,
     size_t *name_size,
     libcerror_error_t **error );

LIBBFIO_EXTERN \
int libbfio_file_range_get_name_wide(
     libbfio_handle_t *handle,
     wchar_t *name,
     size_t name_size,
     libcerror_error_t **error );

LIBBFIO_EXTERN \
int libbfio_file_range_set_name_wide(
     libbfio_handle_t *handle,
     const wchar_t *name,
     size_t name_length,
     libcerror_error_t **error );

#endif /* defined( HAVE_WIDE_CHARACTER_TYPE ) */

LIBBFIO_EXTERN \
int libbfio_file_range_get(
     libbfio_handle_t *handle,
     off64_t *range_offset,
     size64_t *range_size,
     libcerror_error_t **error );

LIBBFIO_EXTERN \
int libbfio_file_range_set(
     libbfio_handle_t *handle,
     off64_t range_offset,
     size64_t range_size,
     libcerror_error_t **error );

int libbfio_file_range_open(
     libbfio_file_range_io_handle_t *file_range_io_handle,
     int access_flags,
     libcerror_error_t **error );

int libbfio_file_range_close(
     libbfio_file_range_io_handle_t *file_range_io_handle,
     libcerror_error_t **error );

ssize_t libbfio_file_range_read(
         libbfio_file_range_io_handle_t *file_range_io_handle,
         uint8_t *buffer,
         size_t size,
         libcerror_error_t **error );

ssize_t libbfio_file_range_write(
         libbfio_file_range_io_handle_t *file_range_io_handle,
         const uint8_t *buffer,
         size_t size,
         libcerror_error_t **error );

off64_t libbfio_file_range_seek_offset(
         libbfio_file_range_io_handle_t *file_range_io_handle,
         off64_t offset,
         int whence,
         libcerror_error_t **error );

int libbfio_file_range_exists(
     libbfio_file_range_io_handle_t *file_range_io_handle,
     libcerror_error_t **error );

int libbfio_file_range_is_open(
     libbfio_file_range_io_handle_t *file_range_io_handle,
     libcerror_error_t **error );

int libbfio_file_range_get_size(
     libbfio_file_range_io_handle_t *file_range_io_handle,
     size64_t *size,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBBFIO_FILE_RANGE_H ) */

