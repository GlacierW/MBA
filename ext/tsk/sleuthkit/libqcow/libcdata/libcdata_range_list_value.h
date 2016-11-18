/*
 * Range list value
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

#if !defined( _LIBCDATA_RANGE_LIST_VALUE_H )
#define _LIBCDATA_RANGE_LIST_VALUE_H

#include <common.h>
#include <types.h>

#include "libcdata_extern.h"
#include "libcdata_libcerror.h"
#include "libcdata_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libcdata_range_list_value libcdata_range_list_value_t;

struct libcdata_range_list_value
{
	/* The (range) start value
	 */
	uint64_t start;

	/* The (range) end
	 */
	uint64_t end;

	/* The (range) size
	 */
	uint64_t size;

	/* The value
	 */
	intptr_t *value;
};

int libcdata_range_list_value_initialize(
     libcdata_range_list_value_t **range_list_value,
     libcerror_error_t **error );

int libcdata_range_list_value_free(
     libcdata_range_list_value_t **range_list_value,
     int (*value_free_function)(
            intptr_t **value,
            libcerror_error_t **error ),
     libcerror_error_t **error );

int libcdata_range_list_value_clone(
     libcdata_range_list_value_t **destination_range_list_value,
     libcdata_range_list_value_t *source_range_list_value,
     int (*value_free_function)(
            intptr_t **value,
            libcerror_error_t **error ),
     int (*value_clone_function)(
            intptr_t **destination_value,
            intptr_t *source_value,
            libcerror_error_t **error ),
     libcerror_error_t **error );

int libcdata_range_list_value_merge(
     libcdata_range_list_value_t *destination_range_list_value,
     libcdata_range_list_value_t *source_range_list_value,
     int (*value_merge_function)(
            intptr_t *destination_value,
            intptr_t *source_value,
            libcerror_error_t **error ),
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBCDATA_RANGE_LIST_VALUE_H ) */

