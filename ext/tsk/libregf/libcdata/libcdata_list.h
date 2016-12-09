/*
 * List functions
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

#if !defined( _LIBCDATA_LIST_H )
#define _LIBCDATA_LIST_H

#include <common.h>
#include <types.h>

#include "libcdata_extern.h"
#include "libcdata_libcerror.h"
#include "libcdata_libcthreads.h"
#include "libcdata_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libcdata_internal_list libcdata_internal_list_t;

struct libcdata_internal_list
{
	/* The number of elements
	 */
	int number_of_elements;

	/* The first element
	 */
	libcdata_list_element_t *first_element;

	/* The last element
	 */
	libcdata_list_element_t *last_element;

#if defined( HAVE_MULTI_THREAD_SUPPORT ) && !defined( HAVE_LOCAL_LIBCDATA )
	/* The read/write lock
	 */
	libcthreads_read_write_lock_t *read_write_lock;
#endif
};

LIBCDATA_EXTERN \
int libcdata_list_initialize(
     libcdata_list_t **list,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_list_free(
     libcdata_list_t **list,
     int (*value_free_function)(
            intptr_t **value,
            libcerror_error_t **error ),
     libcerror_error_t **error );

int libcdata_internal_list_empty(
     libcdata_internal_list_t *internal_list,
     int (*value_free_function)(
            intptr_t **value,
            libcerror_error_t **error ),
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_list_empty(
     libcdata_list_t *list,
     int (*value_free_function)(
            intptr_t **value,
            libcerror_error_t **error ),
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_list_clone(
     libcdata_list_t **destination_list,
     libcdata_list_t *source_list,
     int (*value_free_function)(
            intptr_t **value,
            libcerror_error_t **error ),
     int (*value_clone_function)(
            intptr_t **destination_value,
            intptr_t *source_value,
            libcerror_error_t **error ),
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_list_get_number_of_elements(
     libcdata_list_t *list,
     int *number_of_elements,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_list_get_first_element(
     libcdata_list_t *list,
     libcdata_list_element_t **element,
     libcerror_error_t **error );

int libcdata_internal_list_set_first_element(
     libcdata_internal_list_t *internal_list,
     libcdata_list_element_t *element,
     libcerror_error_t **error );

int libcdata_list_set_first_element(
     libcdata_list_t *list,
     libcdata_list_element_t *element,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_list_get_last_element(
     libcdata_list_t *list,
     libcdata_list_element_t **element,
     libcerror_error_t **error );

int libcdata_internal_list_set_last_element(
     libcdata_internal_list_t *internal_list,
     libcdata_list_element_t *element,
     libcerror_error_t **error );

int libcdata_list_set_last_element(
     libcdata_list_t *list,
     libcdata_list_element_t *element,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_list_get_element_by_index(
     libcdata_list_t *list,
     int element_index,
     libcdata_list_element_t **element,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_list_get_value_by_index(
     libcdata_list_t *list,
     int element_index,
     intptr_t **value,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_list_prepend_element(
     libcdata_list_t *list,
     libcdata_list_element_t *element,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_list_prepend_value(
     libcdata_list_t *list,
     intptr_t *value,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_list_append_element(
     libcdata_list_t *list,
     libcdata_list_element_t *element,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_list_append_value(
     libcdata_list_t *list,
     intptr_t *value,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_list_insert_element(
     libcdata_list_t *list,
     libcdata_list_element_t *element,
     int (*value_compare_function)(
            intptr_t *first,
            intptr_t *second,
            libcerror_error_t **error ),
     uint8_t insert_flags,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_list_insert_value(
     libcdata_list_t *list,
     intptr_t *value,
     int (*value_compare_function)(
            intptr_t *first,
            intptr_t *second,
            libcerror_error_t **error ),
     uint8_t insert_flags,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_list_remove_element(
     libcdata_list_t *list,
     libcdata_list_element_t *element,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBCDATA_LIST_H ) */

