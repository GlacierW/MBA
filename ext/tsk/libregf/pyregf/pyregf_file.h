/*
 * Python object definition of the libregf file
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

#if !defined( _PYREGF_FILE_H )
#define _PYREGF_FILE_H

#include <common.h>
#include <types.h>

#include "pyregf_libbfio.h"
#include "pyregf_libregf.h"
#include "pyregf_python.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct pyregf_file pyregf_file_t;

struct pyregf_file
{
	/* Python object initialization
	 */
	PyObject_HEAD

	/* The libregf file
	 */
	libregf_file_t *file;

	/* The libbfio file IO handle
	 */
	libbfio_handle_t *file_io_handle;
};

extern PyMethodDef pyregf_file_object_methods[];
extern PyTypeObject pyregf_file_type_object;

PyObject *pyregf_file_new(
           void );

PyObject *pyregf_file_new_open(
           PyObject *self,
           PyObject *arguments,
           PyObject *keywords );

PyObject *pyregf_file_new_open_file_object(
           PyObject *self,
           PyObject *arguments,
           PyObject *keywords );

int pyregf_file_init(
     pyregf_file_t *pyregf_file );

void pyregf_file_free(
      pyregf_file_t *pyregf_file );

PyObject *pyregf_file_signal_abort(
           pyregf_file_t *pyregf_file,
           PyObject *arguments );

PyObject *pyregf_file_open(
           pyregf_file_t *pyregf_file,
           PyObject *arguments,
           PyObject *keywords );

PyObject *pyregf_file_open_file_object(
           pyregf_file_t *pyregf_file,
           PyObject *arguments,
           PyObject *keywords );

PyObject *pyregf_file_close(
           pyregf_file_t *pyregf_file,
           PyObject *arguments );

PyObject *pyregf_file_is_corrupted(
           pyregf_file_t *pyregf_file,
           PyObject *arguments );

PyObject *pyregf_file_get_ascii_codepage(
           pyregf_file_t *pyregf_file,
           PyObject *arguments );

int pyregf_file_set_ascii_codepage_from_string(
     pyregf_file_t *pyregf_file,
     const char *codepage_string );

PyObject *pyregf_file_set_ascii_codepage(
           pyregf_file_t *pyregf_file,
           PyObject *arguments,
           PyObject *keywords );

int pyregf_file_set_ascii_codepage_setter(
     pyregf_file_t *pyregf_file,
     PyObject *string_object,
     void *closure );

PyObject *pyregf_file_get_format_version(
           pyregf_file_t *pyregf_file,
           PyObject *arguments );

PyObject *pyregf_file_get_type(
           pyregf_file_t *pyregf_file,
           PyObject *arguments );

PyObject *pyregf_file_get_root_key(
           pyregf_file_t *pyregf_file,
           PyObject *arguments );

PyObject *pyregf_file_get_key_by_path(
           pyregf_file_t *pyregf_file,
           PyObject *arguments,
           PyObject *keywords );

#if defined( __cplusplus )
}
#endif

#endif

