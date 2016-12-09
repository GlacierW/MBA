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

#include <common.h>
#include <types.h>

#if defined( HAVE_STDLIB_H ) || defined( HAVE_WINAPI )
#include <stdlib.h>
#endif

#include "pyregf_libregf.h"
#include "pyregf_python.h"
#include "pyregf_unused.h"
#include "pyregf_file_types.h"

PyTypeObject pyregf_file_types_type_object = {
	PyVarObject_HEAD_INIT( NULL, 0 )

	/* tp_name */
	"pyregf.file_types",
	/* tp_basicsize */
	sizeof( pyregf_file_types_t ),
	/* tp_itemsize */
	0,
	/* tp_dealloc */
	(destructor) pyregf_file_types_free,
	/* tp_print */
	0,
	/* tp_getattr */
	0,
	/* tp_setattr */
	0,
	/* tp_compare */
	0,
	/* tp_repr */
	0,
	/* tp_as_number */
	0,
	/* tp_as_sequence */
	0,
	/* tp_as_mapping */
	0,
	/* tp_hash */
	0,
	/* tp_call */
	0,
	/* tp_str */
	0,
	/* tp_getattro */
	0,
	/* tp_setattro */
	0,
	/* tp_as_buffer */
	0,
	/* tp_types */
	Py_TPFLAGS_DEFAULT,
	/* tp_doc */
	"pyregf file types object (wraps LIBREGF_FILE_TYPES)",
	/* tp_traverse */
	0,
	/* tp_clear */
	0,
	/* tp_richcompare */
	0,
	/* tp_weaklistoffset */
	0,
	/* tp_iter */
	0,
	/* tp_iternext */
	0,
	/* tp_methods */
	0,
	/* tp_members */
	0,
	/* tp_getset */
	0,
	/* tp_base */
	0,
	/* tp_dict */
	0,
	/* tp_descr_get */
	0,
	/* tp_descr_set */
	0,
	/* tp_dictoffset */
	0,
	/* tp_init */
	(initproc) pyregf_file_types_init,
	/* tp_alloc */
	0,
	/* tp_new */
	0,
	/* tp_free */
	0,
	/* tp_is_gc */
	0,
	/* tp_bases */
	NULL,
	/* tp_mro */
	NULL,
	/* tp_cache */
	NULL,
	/* tp_subclasses */
	NULL,
	/* tp_weaklist */
	NULL,
	/* tp_del */
	0
};

/* Initializes the type object
 * Returns 1 if successful or -1 on error
 */
int pyregf_file_types_init_type(
     PyTypeObject *type_object )
{
	PyObject *file_object = NULL;

	if( type_object == NULL )
	{
		return( -1 );
	}
	type_object->tp_dict = PyDict_New();

	if( type_object->tp_dict == NULL )
	{
		return( -1 );
	}
#if PY_MAJOR_VERSION >= 3
	file_object = PyLong_FromLong(
	               LIBREGF_FILE_TYPE_REGISTRY );
#else
	file_object = PyInt_FromLong(
	               LIBREGF_FILE_TYPE_REGISTRY );
#endif
	if( PyDict_SetItemString(
	     type_object->tp_dict,
	     "REGISTRY",
	     file_object ) != 0 )
	{
		goto on_error;
	}
#if PY_MAJOR_VERSION >= 3
	file_object = PyLong_FromLong(
	               LIBREGF_FILE_TYPE_TRANSACTION_LOG );
#else
	file_object = PyInt_FromLong(
	               LIBREGF_FILE_TYPE_TRANSACTION_LOG );
#endif
	if( PyDict_SetItemString(
	     type_object->tp_dict,
	     "TRANSACTION_LOG",
	     file_object ) != 0 )
	{
		goto on_error;
	}
	return( 1 );

on_error:
	if( type_object->tp_dict != NULL )
	{
		Py_DecRef(
		 type_object->tp_dict );

		type_object->tp_dict = NULL;
	}
	return( -1 );
}

/* Creates a new file types object
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyregf_file_types_new(
           void )
{
	pyregf_file_types_t *pyregf_file_types = NULL;
	static char *function                  = "pyregf_file_types_new";

	pyregf_file_types = PyObject_New(
	                     struct pyregf_file_types,
	                     &pyregf_file_types_type_object );

	if( pyregf_file_types == NULL )
	{
		PyErr_Format(
		 PyExc_MemoryError,
		 "%s: unable to initialize file types.",
		 function );

		goto on_error;
	}
	if( pyregf_file_types_init(
	     pyregf_file_types ) != 0 )
	{
		PyErr_Format(
		 PyExc_MemoryError,
		 "%s: unable to initialize file types.",
		 function );

		goto on_error;
	}
	return( (PyObject *) pyregf_file_types );

on_error:
	if( pyregf_file_types != NULL )
	{
		Py_DecRef(
		 (PyObject *) pyregf_file_types );
	}
	return( NULL );
}

/* Intializes a file types object
 * Returns 0 if successful or -1 on error
 */
int pyregf_file_types_init(
     pyregf_file_types_t *pyregf_file_types )
{
	static char *function = "pyregf_file_types_init";

	if( pyregf_file_types == NULL )
	{
		PyErr_Format(
		 PyExc_TypeError,
		 "%s: invalid file types.",
		 function );

		return( -1 );
	}
	return( 0 );
}

/* Frees a file types object
 */
void pyregf_file_types_free(
      pyregf_file_types_t *pyregf_file_types )
{
	struct _typeobject *ob_type = NULL;
	static char *function       = "pyregf_file_types_free";

	if( pyregf_file_types == NULL )
	{
		PyErr_Format(
		 PyExc_TypeError,
		 "%s: invalid file types.",
		 function );

		return;
	}
	ob_type = Py_TYPE(
	           pyregf_file_types );

	if( ob_type == NULL )
	{
		PyErr_Format(
		 PyExc_ValueError,
		 "%s: missing ob_type.",
		 function );

		return;
	}
	if( ob_type->tp_free == NULL )
	{
		PyErr_Format(
		 PyExc_ValueError,
		 "%s: invalid ob_type - missing tp_free.",
		 function );

		return;
	}
	ob_type->tp_free(
	 (PyObject*) pyregf_file_types );
}

