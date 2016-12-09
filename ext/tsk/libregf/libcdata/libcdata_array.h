/*
 * Array functions
 *
 * Copyright (C) 2006-2016, Joachim Metz <joachim.metz@gmail.com>
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

#if !defined( _LIBCDATA_ARRAY_H )
#define _LIBCDATA_ARRAY_H

#include <common.h>
#include <types.h>

#include "libcdata_extern.h"
#include "libcdata_libcerror.h"
#include "libcdata_libcthreads.h"
#include "libcdata_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libcdata_internal_array libcdata_internal_array_t;

struct libcdata_internal_array
{
	/* The number of allocated entries
	 */
	int number_of_allocated_entries;

	/* The number of entries
	 */
	int number_of_entries;

	/* The entries
	 */
	intptr_t **entries;

#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	/* The read/write lock
	 */
	libcthreads_read_write_lock_t *read_write_lock;
#endif
};

LIBCDATA_EXTERN \
int libcdata_array_initialize(
     libcdata_array_t **array,
     int number_of_entries,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_array_free(
     libcdata_array_t **array,
     int (*entry_free_function)(
            intptr_t **entry,
            libcerror_error_t **error ),
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_array_empty(
     libcdata_array_t *array,
     int (*entry_free_function)(
            intptr_t **entry,
            libcerror_error_t **error ),
     libcerror_error_t **error );

int libcdata_internal_array_clear(
     libcdata_internal_array_t *internal_array,
     int (*entry_free_function)(
            intptr_t **entry,
            libcerror_error_t **error ),
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_array_clear(
     libcdata_array_t *array,
     int (*entry_free_function)(
            intptr_t **entry,
            libcerror_error_t **error ),
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_array_clone(
     libcdata_array_t **destination_array,
     libcdata_array_t *source_array,
     int (*entry_free_function)(
            intptr_t **entry,
            libcerror_error_t **error ),
     int (*entry_clone_function)(
            intptr_t **destination_entry,
            intptr_t *source_entry,
            libcerror_error_t **error ),
     libcerror_error_t **error );

int libcdata_internal_array_resize(
     libcdata_internal_array_t *internal_array,
     int number_of_entries,
     int (*entry_free_function)(
            intptr_t **entry,
            libcerror_error_t **error ),
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_array_resize(
     libcdata_array_t *array,
     int number_of_entries,
     int (*entry_free_function)(
            intptr_t **entry,
            libcerror_error_t **error ),
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_array_reverse(
     libcdata_array_t *array,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_array_get_number_of_entries(
     libcdata_array_t *array,
     int *number_of_entries,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_array_get_entry_by_index(
     libcdata_array_t *array,
     int entry_index,
     intptr_t **entry,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_array_get_entry_by_value(
     libcdata_array_t *array,
     intptr_t *entry,
     int (*entry_compare_function)(
            intptr_t *first_entry,
            intptr_t *second_entry,
            libcerror_error_t **error ),
     intptr_t **existing_entry,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_array_set_entry_by_index(
     libcdata_array_t *array,
     int entry_index,
     intptr_t *entry,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_array_prepend_entry(
     libcdata_array_t *array,
     intptr_t *entry,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_array_append_entry(
     libcdata_array_t *array,
     int *entry_index,
     intptr_t *entry,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_array_insert_entry(
     libcdata_array_t *array,
     int *entry_index,
     intptr_t *entry,
     int (*entry_compare_function)(
            intptr_t *first_entry,
            intptr_t *second_entry,
            libcerror_error_t **error ),
     uint8_t insert_flags,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_array_remove_entry(
     libcdata_array_t *array,
     int entry_index,
     intptr_t **entry,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBCDATA_ARRAY_H ) */

