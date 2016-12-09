/*
 * Python object definition of the libregf file types
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

#if !defined( _PYREGF_FILE_TYPES_H )
#define _PYREGF_FILE_TYPES_H

#include <common.h>
#include <types.h>

#include "pyregf_libregf.h"
#include "pyregf_python.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct pyregf_file_types pyregf_file_types_t;

struct pyregf_file_types
{
	/* Python object initialization
	 */
	PyObject_HEAD
};

extern PyTypeObject pyregf_file_types_type_object;

int pyregf_file_types_init_type(
     PyTypeObject *type_object );

PyObject *pyregf_file_types_new(
           void );

int pyregf_file_types_init(
     pyregf_file_types_t *pyregf_file_types );

void pyregf_file_types_free(
      pyregf_file_types_t *pyregf_file_types );

#if defined( __cplusplus )
}
#endif

#endif

