/*
 * Python object definition of the values sequence and iterator
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

#if !defined( _PYREGF_VALUES_H )
#define _PYREGF_VALUES_H

#include <common.h>
#include <types.h>

#include "pyregf_key.h"
#include "pyregf_libregf.h"
#include "pyregf_python.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct pyregf_values pyregf_values_t;

struct pyregf_values
{
	/* Python object initialization
	 */
	PyObject_HEAD

	/* The pyregf key object
	 */
	pyregf_key_t *key_object;

	/* The get value by index callback function
	 */
	PyObject* (*get_value_by_index)(
	             pyregf_key_t *key_object,
	             int value_index );

	/* The (current) value index
	 */
	int value_index;

	/* The number of values
	 */
	int number_of_values;
};

extern PyTypeObject pyregf_values_type_object;

PyObject *pyregf_values_new(
           pyregf_key_t *key_object,
           PyObject* (*get_value_by_index)(
                        pyregf_key_t *key_object,
                        int value_index ),
           int number_of_values );

int pyregf_values_init(
     pyregf_values_t *pyregf_values );

void pyregf_values_free(
      pyregf_values_t *pyregf_values );

Py_ssize_t pyregf_values_len(
            pyregf_values_t *pyregf_values );

PyObject *pyregf_values_getitem(
           pyregf_values_t *pyregf_values,
           Py_ssize_t item_index );

PyObject *pyregf_values_iter(
           pyregf_values_t *pyregf_values );

PyObject *pyregf_values_iternext(
           pyregf_values_t *pyregf_values );

#if defined( __cplusplus )
}
#endif

#endif

