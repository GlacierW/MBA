/*
 * The internal pool functions
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

#if !defined( _LIBBFIO_POOL_H )
#define _LIBBFIO_POOL_H

#include <common.h>
#include <types.h>

#include "libbfio_extern.h"
#include "libbfio_libcdata.h"
#include "libbfio_libcerror.h"
#include "libbfio_libcthreads.h"
#include "libbfio_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libbfio_internal_pool libbfio_internal_pool_t;

struct libbfio_internal_pool
{
	/* The number of used handles in the pool
	 */
	int number_of_used_handles;

	/* The number of open handles in the pool
	 */
	int number_of_open_handles;

	/* The maximum number of open handles in the pool
	 */
	int maximum_number_of_open_handles;

	/* The handles array
	 */
	libcdata_array_t *handles_array;

	/* A list containing the file IO handles in order of the last use
	 * it starts with the last used at the beginning of the list
	 * the value of the list element refers to the corresponding file IO handle
	 */
	libcdata_list_t *last_used_list;

#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBBFIO )
	/* The read/write lock
	 */
	libcthreads_read_write_lock_t *read_write_lock;
#endif
};

LIBBFIO_EXTERN \
int libbfio_pool_initialize(
     libbfio_pool_t **pool,
     int number_of_handles,
     int maximum_number_of_open_handles,
     libcerror_error_t **error );

LIBBFIO_EXTERN \
int libbfio_pool_free(
     libbfio_pool_t **pool,
     libcerror_error_t **error );

LIBBFIO_EXTERN \
int libbfio_pool_clone(
     libbfio_pool_t **destination_pool,
     libbfio_pool_t *source_pool,
     libcerror_error_t **error );

LIBBFIO_EXTERN \
int libbfio_pool_resize(
     libbfio_pool_t *pool,
     int number_of_handles,
     libcerror_error_t **error );

int libbfio_pool_open_handle(
     libbfio_internal_pool_t *internal_pool,
     libbfio_handle_t *handle,
     int access_flags,
     libcerror_error_t **error );

int libbfio_pool_append_handle_to_last_used_list(
     libbfio_internal_pool_t *internal_pool,
     libbfio_handle_t *handle,
     libcerror_error_t **error );

int libbfio_pool_move_handle_to_front_of_last_used_list(
     libbfio_internal_pool_t *internal_pool,
     libbfio_handle_t *handle,
     libcerror_error_t **error );

LIBBFIO_EXTERN \
int libbfio_pool_get_number_of_handles(
     libbfio_pool_t *pool,
     int *number_of_handles,
     libcerror_error_t **error );

LIBBFIO_EXTERN \
int libbfio_pool_get_handle(
     libbfio_pool_t *pool,
     int entry,
     libbfio_handle_t **handle,
     libcerror_error_t **error );

LIBBFIO_EXTERN \
int libbfio_pool_append_handle(
     libbfio_pool_t *pool,
     int *entry,
     libbfio_handle_t *handle,
     int access_flags,
     libcerror_error_t **error );

LIBBFIO_EXTERN \
int libbfio_pool_set_handle(
     libbfio_pool_t *pool,
     int entry,
     libbfio_handle_t *handle,
     int access_flags,
     libcerror_error_t **error );

LIBBFIO_EXTERN \
int libbfio_pool_remove_handle(
     libbfio_pool_t *pool,
     int entry,
     libbfio_handle_t **handle,
     libcerror_error_t **error );

LIBBFIO_EXTERN \
int libbfio_pool_get_maximum_number_of_open_handles(
     libbfio_pool_t *pool,
     int *maximum_number_of_open_handles,
     libcerror_error_t **error );

LIBBFIO_EXTERN \
int libbfio_pool_set_maximum_number_of_open_handles(
     libbfio_pool_t *pool,
     int maximum_number_of_open_handles,
     libcerror_error_t **error );

LIBBFIO_EXTERN \
int libbfio_pool_open(
     libbfio_pool_t *pool,
     int entry,
     int access_flags,
     libcerror_error_t **error );

LIBBFIO_EXTERN \
int libbfio_pool_reopen(
     libbfio_pool_t *pool,
     int entry,
     int access_flags,
     libcerror_error_t **error );

LIBBFIO_EXTERN \
int libbfio_pool_close(
     libbfio_pool_t *pool,
     int entry,
     libcerror_error_t **error );

LIBBFIO_EXTERN \
int libbfio_pool_close_all(
     libbfio_pool_t *pool,
     libcerror_error_t **error );

LIBBFIO_EXTERN \
ssize_t libbfio_pool_read_buffer(
         libbfio_pool_t *pool,
         int entry,
         uint8_t *buffer,
         size_t size,
         libcerror_error_t **error );

LIBBFIO_EXTERN \
ssize_t libbfio_pool_write_buffer(
         libbfio_pool_t *pool,
         int entry,
         const uint8_t *buffer,
         size_t size,
         libcerror_error_t **error );

LIBBFIO_EXTERN \
off64_t libbfio_pool_seek_offset(
         libbfio_pool_t *pool,
         int entry,
         off64_t offset,
         int whence,
         libcerror_error_t **error );

LIBBFIO_EXTERN \
int libbfio_pool_get_offset(
     libbfio_pool_t *pool,
     int entry,
     off64_t *offset,
     libcerror_error_t **error );

LIBBFIO_EXTERN \
int libbfio_pool_get_size(
     libbfio_pool_t *pool,
     int entry,
     size64_t *size,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBBFIO_POOL_H ) */

