/*
 * Python object definition of the libregf value
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

#if !defined( _PYREGF_VALUE_H )
#define _PYREGF_VALUE_H

#include <common.h>
#include <types.h>

#include "pyregf_file.h"
#include "pyregf_libregf.h"
#include "pyregf_python.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct pyregf_value pyregf_value_t;

struct pyregf_value
{
	/* Python object initialization
	 */
	PyObject_HEAD

	/* The libregf value
	 */
	libregf_value_t *value;

	/* The pyregf file object
	 */
	pyregf_file_t *file_object;
};

extern PyMethodDef pyregf_value_object_methods[];
extern PyTypeObject pyregf_value_type_object;

PyObject *pyregf_value_new(
           libregf_value_t *value,
           pyregf_file_t *file_object );

int pyregf_value_init(
     pyregf_value_t *pyregf_value );

void pyregf_value_free(
      pyregf_value_t *pyregf_value );

PyObject *pyregf_value_is_corrupted(
           pyregf_value_t *pyregf_value,
           PyObject *arguments );

PyObject *pyregf_value_get_offset(
           pyregf_value_t *pyregf_value,
           PyObject *arguments );

PyObject *pyregf_value_get_name(
           pyregf_value_t *pyregf_value,
           PyObject *arguments );

PyObject *pyregf_value_get_type(
           pyregf_value_t *pyregf_value,
           PyObject *arguments );

PyObject *pyregf_value_get_data_size(
           pyregf_value_t *pyregf_value,
           PyObject *arguments );

PyObject *pyregf_value_get_data(
           pyregf_value_t *pyregf_value,
           PyObject *arguments );

PyObject *pyregf_value_get_data_as_integer(
           pyregf_value_t *pyregf_value,
           PyObject *arguments );

PyObject *pyregf_value_get_data_as_string(
           pyregf_value_t *pyregf_value,
           PyObject *arguments );

#if defined( __cplusplus )
}
#endif

#endif

