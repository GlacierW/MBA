/*
 * Python object definition of the libregf key
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

#if !defined( _PYREGF_KEY_H )
#define _PYREGF_KEY_H

#include <common.h>
#include <types.h>

#include "pyregf_file.h"
#include "pyregf_libregf.h"
#include "pyregf_python.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct pyregf_key pyregf_key_t;

struct pyregf_key
{
	/* Python object initialization
	 */
	PyObject_HEAD

	/* The libregf key
	 */
	libregf_key_t *key;

	/* The pyregf file object
	 */
	pyregf_file_t *file_object;
};

extern PyMethodDef pyregf_key_object_methods[];
extern PyTypeObject pyregf_key_type_object;

PyObject *pyregf_key_new(
           libregf_key_t *key,
           pyregf_file_t *file_object );

int pyregf_key_init(
     pyregf_key_t *pyregf_key );

void pyregf_key_free(
      pyregf_key_t *pyregf_key );

PyObject *pyregf_key_is_corrupted(
           pyregf_key_t *pyregf_key,
           PyObject *arguments );

PyObject *pyregf_key_get_offset(
           pyregf_key_t *pyregf_key,
           PyObject *arguments );

PyObject *pyregf_key_get_name(
           pyregf_key_t *pyregf_key,
           PyObject *arguments );

PyObject *pyregf_key_get_class_name(
           pyregf_key_t *pyregf_key,
           PyObject *arguments );

PyObject *pyregf_key_get_last_written_time(
           pyregf_key_t *pyregf_key,
           PyObject *arguments );

PyObject *pyregf_key_get_last_written_time_as_integer(
           pyregf_key_t *pyregf_key,
           PyObject *arguments );

PyObject *pyregf_key_get_number_of_sub_keys(
           pyregf_key_t *pyregf_key,
           PyObject *arguments );

PyObject *pyregf_key_get_sub_key_by_index(
           pyregf_key_t *pyregf_key,
           int sub_key_index );

PyObject *pyregf_key_get_sub_key(
           pyregf_key_t *pyregf_key,
           PyObject *arguments,
           PyObject *keywords );

PyObject *pyregf_key_get_sub_keys(
           pyregf_key_t *pyregf_key,
           PyObject *arguments );

PyObject *pyregf_key_get_sub_key_by_name(
           pyregf_key_t *pyregf_key,
           PyObject *arguments,
           PyObject *keywords );

PyObject *pyregf_key_get_sub_key_by_path(
           pyregf_key_t *pyregf_key,
           PyObject *arguments,
           PyObject *keywords );

PyObject *pyregf_key_get_number_of_values(
           pyregf_key_t *pyregf_key,
           PyObject *arguments );

PyObject *pyregf_key_get_value_by_index(
           pyregf_key_t *pyregf_key,
           int value_index );

PyObject *pyregf_key_get_value(
           pyregf_key_t *pyregf_key,
           PyObject *arguments,
           PyObject *keywords );

PyObject *pyregf_key_get_values(
           pyregf_key_t *pyregf_key,
           PyObject *arguments );

PyObject *pyregf_key_get_value_by_name(
           pyregf_key_t *pyregf_key,
           PyObject *arguments,
           PyObject *keywords );

#if defined( __cplusplus )
}
#endif

#endif

