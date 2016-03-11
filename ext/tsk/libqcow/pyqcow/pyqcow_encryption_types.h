/*
 * Python object definition of the libqcow encryption types
 *
 * Copyright (C) 2010-2016, Joachim Metz <joachim.metz@gmail.com>
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

#if !defined( _PYQCOW_ENCRYPTION_TYPES_H )
#define _PYQCOW_ENCRYPTION_TYPES_H

#include <common.h>
#include <types.h>

#include "pyqcow_libqcow.h"
#include "pyqcow_python.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct pyqcow_encryption_types pyqcow_encryption_types_t;

struct pyqcow_encryption_types
{
	/* Python object initialization
	 */
	PyObject_HEAD
};

extern PyTypeObject pyqcow_encryption_types_type_object;

int pyqcow_encryption_types_init_type(
     PyTypeObject *type_object );

PyObject *pyqcow_encryption_types_new(
           void );

int pyqcow_encryption_types_init(
     pyqcow_encryption_types_t *pyqcow_encryption_types );

void pyqcow_encryption_types_free(
      pyqcow_encryption_types_t *pyqcow_encryption_types );

#if defined( __cplusplus )
}
#endif

#endif

