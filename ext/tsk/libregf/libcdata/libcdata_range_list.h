/*
 * Range list
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

#if !defined( _LIBCDATA_RANGE_LIST_H )
#define _LIBCDATA_RANGE_LIST_H

#include <common.h>
#include <types.h>

#include "libcdata_extern.h"
#include "libcdata_libcerror.h"
#include "libcdata_range_list_value.h"
#include "libcdata_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libcdata_internal_range_list libcdata_internal_range_list_t;

struct libcdata_internal_range_list
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

	/* The current list element
	 */
	libcdata_list_element_t *current_element;

	/* The current list element index
	 */
	int current_element_index;
};

LIBCDATA_EXTERN \
int libcdata_range_list_initialize(
     libcdata_range_list_t **range_list,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_range_list_free(
     libcdata_range_list_t **range_list,
     int (*value_free_function)(
            intptr_t **value,
            libcerror_error_t **error ),
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_range_list_empty(
     libcdata_range_list_t *range_list,
     int (*value_free_function)(
            intptr_t **value,
            libcerror_error_t **error ),
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_range_list_clone(
     libcdata_range_list_t **destination_range_list,
     libcdata_range_list_t *source_range_list,
     int (*value_free_function)(
            intptr_t **value,
            libcerror_error_t **error ),
     int (*value_clone_function)(
            intptr_t **destination_value,
            intptr_t *source_value,
            libcerror_error_t **error ),
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_range_list_get_number_of_elements(
     libcdata_range_list_t *range_list,
     int *number_of_elements,
     libcerror_error_t **error );

int libcdata_range_list_set_first_element(
     libcdata_range_list_t *range_list,
     libcdata_list_element_t *element,
     libcerror_error_t **error );

int libcdata_range_list_set_last_element(
     libcdata_range_list_t *range_list,
     libcdata_list_element_t *element,
     libcerror_error_t **error );

int libcdata_range_list_append_element(
     libcdata_range_list_t *range_list,
     libcdata_list_element_t *element,
     libcerror_error_t **error );

int libcdata_range_list_append_value(
     libcdata_range_list_t *range_list,
     libcdata_range_list_value_t *value,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_range_list_insert_range(
     libcdata_range_list_t *range_list,
     uint64_t range_start,
     uint64_t range_size,
     intptr_t *value,
     int (*value_free_function)(
            intptr_t **value,
            libcerror_error_t **error ),
     int (*value_merge_function)(
            intptr_t *destination_value,
            intptr_t *source_value,
            libcerror_error_t **error ),
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_range_list_insert_range_list(
     libcdata_range_list_t *range_list,
     libcdata_range_list_t *source_range_list,
     int (*value_free_function)(
            intptr_t **value,
            libcerror_error_t **error ),
     int (*value_merge_function)(
            intptr_t *destination_value,
            intptr_t *source_value,
            libcerror_error_t **error ),
     libcerror_error_t **error );

int libcdata_range_list_insert_element(
     libcdata_range_list_t *range_list,
     libcdata_list_element_t *range_list_element,
     libcdata_list_element_t *element,
     libcerror_error_t **error );

int libcdata_range_list_insert_value(
     libcdata_range_list_t *range_list,
     libcdata_list_element_t *range_list_element,
     libcdata_range_list_value_t *range_list_value,
     libcerror_error_t **error );

int libcdata_range_list_remove_element(
     libcdata_range_list_t *range_list,
     libcdata_list_element_t *element,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_range_list_remove_range(
     libcdata_range_list_t *range_list,
     uint64_t range_start,
     uint64_t range_size,
     int (*value_free_function)(
            intptr_t **value,
            libcerror_error_t **error ),
     int (*value_split_function)(
            intptr_t **destination_value,
            intptr_t *source_value,
            uint64_t split_range_offset,
            libcerror_error_t **error ),
     libcerror_error_t **error );

int libcdata_range_list_get_element_by_index(
     libcdata_range_list_t *range_list,
     int element_index,
     libcdata_list_element_t **element,
     libcerror_error_t **error );

int libcdata_range_list_get_element_at_offset(
     libcdata_range_list_t *range_list,
     uint64_t range_offset,
     libcdata_list_element_t **element,
     libcerror_error_t **error );

int libcdata_range_list_get_value_by_index(
     libcdata_range_list_t *range_list,
     int element_index,
     libcdata_range_list_value_t **range_list_value,
     libcerror_error_t **error );

int libcdata_range_list_get_value_at_offset(
     libcdata_range_list_t *range_list,
     uint64_t range_offset,
     libcdata_range_list_value_t **range_list_value,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_range_list_get_range_by_index(
     libcdata_range_list_t *range_list,
     int element_index,
     uint64_t *range_start,
     uint64_t *range_size,
     intptr_t **value,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_range_list_get_range_at_offset(
     libcdata_range_list_t *range_list,
     uint64_t range_offset,
     uint64_t *range_start,
     uint64_t *range_size,
     intptr_t **value,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_range_list_range_is_present(
     libcdata_range_list_t *range_list,
     uint64_t range_start,
     uint64_t range_size,
     libcerror_error_t **error );

LIBCDATA_EXTERN \
int libcdata_range_list_get_spanning_range(
     libcdata_range_list_t *range_list,
     uint64_t *range_start,
     uint64_t *range_size,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBCDATA_RANGE_LIST_H ) */

