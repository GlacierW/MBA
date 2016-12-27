/*
 * Python object definition of the keys sequence and iterator
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

#if !defined( _PYREGF_KEYS_H )
#define _PYREGF_KEYS_H

#include <common.h>
#include <types.h>

#include "pyregf_key.h"
#include "pyregf_libregf.h"
#include "pyregf_python.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct pyregf_keys pyregf_keys_t;

struct pyregf_keys
{
	/* Python object initialization
	 */
	PyObject_HEAD

	/* The pyregf key object
	 */
	pyregf_key_t *key_object;

	/* The get sub key by index callback function
	 */
	PyObject* (*get_sub_key_by_index)(
	             pyregf_key_t *key_object,
	             int sub_key_index );

	/* The (current) sub key index
	 */
	int sub_key_index;

	/* The number of sub keys
	 */
	int number_of_sub_keys;
};

extern PyTypeObject pyregf_keys_type_object;

PyObject *pyregf_keys_new(
           pyregf_key_t *key_object,
           PyObject* (*get_sub_key_by_index)(
                        pyregf_key_t *key_object,
                        int sub_key_index ),
           int number_of_sub_keys );

int pyregf_keys_init(
     pyregf_keys_t *pyregf_keys );

void pyregf_keys_free(
      pyregf_keys_t *pyregf_keys );

Py_ssize_t pyregf_keys_len(
            pyregf_keys_t *pyregf_keys );

PyObject *pyregf_keys_getitem(
           pyregf_keys_t *pyregf_keys,
           Py_ssize_t item_index );

PyObject *pyregf_keys_iter(
           pyregf_keys_t *pyregf_keys );

PyObject *pyregf_keys_iternext(
           pyregf_keys_t *pyregf_keys );

#if defined( __cplusplus )
}
#endif

#endif

