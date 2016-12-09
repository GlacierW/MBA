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

#include <common.h>
#include <types.h>

#if defined( HAVE_STDLIB_H ) || defined( HAVE_WINAPI )
#include <stdlib.h>
#endif

#include "pyregf_key.h"
#include "pyregf_libcerror.h"
#include "pyregf_libregf.h"
#include "pyregf_python.h"
#include "pyregf_value.h"
#include "pyregf_values.h"

PySequenceMethods pyregf_values_sequence_methods = {
	/* sq_length */
	(lenfunc) pyregf_values_len,
	/* sq_concat */
	0,
	/* sq_repeat */
	0,
	/* sq_item */
	(ssizeargfunc) pyregf_values_getitem,
	/* sq_slice */
	0,
	/* sq_ass_item */
	0,
	/* sq_ass_slice */
	0,
	/* sq_contains */
	0,
	/* sq_inplace_concat */
	0,
	/* sq_inplace_repeat */
	0
};

PyTypeObject pyregf_values_type_object = {
	PyVarObject_HEAD_INIT( NULL, 0 )

	/* tp_name */
	"pyregf._values",
	/* tp_basicsize */
	sizeof( pyregf_values_t ),
	/* tp_itemsize */
	0,
	/* tp_dealloc */
	(destructor) pyregf_values_free,
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
	&pyregf_values_sequence_methods,
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
	/* tp_flags */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_ITER,
	/* tp_doc */
	"internal pyregf values sequence and iterator object",
	/* tp_traverse */
	0,
	/* tp_clear */
	0,
	/* tp_richcompare */
	0,
	/* tp_weaklistoffset */
	0,
	/* tp_iter */
	(getiterfunc) pyregf_values_iter,
	/* tp_iternext */
	(iternextfunc) pyregf_values_iternext,
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
	(initproc) pyregf_values_init,
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

/* Creates a new values object
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyregf_values_new(
           pyregf_key_t *key_object,
           PyObject* (*get_value_by_index)(
                        pyregf_key_t *key_object,
                        int value_index ),
           int number_of_values )
{
	pyregf_values_t *pyregf_values = NULL;
	static char *function          = "pyregf_values_new";

	if( key_object == NULL )
	{
		PyErr_Format(
		 PyExc_ValueError,
		 "%s: invalid key object.",
		 function );

		return( NULL );
	}
	if( get_value_by_index == NULL )
	{
		PyErr_Format(
		 PyExc_ValueError,
		 "%s: invalid get value by index function.",
		 function );

		return( NULL );
	}
	/* Make sure the values values are initialized
	 */
	pyregf_values = PyObject_New(
	                 struct pyregf_values,
	                 &pyregf_values_type_object );

	if( pyregf_values == NULL )
	{
		PyErr_Format(
		 PyExc_MemoryError,
		 "%s: unable to initialize values.",
		 function );

		goto on_error;
	}
	if( pyregf_values_init(
	     pyregf_values ) != 0 )
	{
		PyErr_Format(
		 PyExc_MemoryError,
		 "%s: unable to initialize values.",
		 function );

		goto on_error;
	}
	pyregf_values->key_object         = key_object;
	pyregf_values->get_value_by_index = get_value_by_index;
	pyregf_values->number_of_values   = number_of_values;

	Py_IncRef(
	 (PyObject *) pyregf_values->key_object );

	return( (PyObject *) pyregf_values );

on_error:
	if( pyregf_values != NULL )
	{
		Py_DecRef(
		 (PyObject *) pyregf_values );
	}
	return( NULL );
}

/* Intializes a values object
 * Returns 0 if successful or -1 on error
 */
int pyregf_values_init(
     pyregf_values_t *pyregf_values )
{
	static char *function = "pyregf_values_init";

	if( pyregf_values == NULL )
	{
		PyErr_Format(
		 PyExc_ValueError,
		 "%s: invalid values.",
		 function );

		return( -1 );
	}
	/* Make sure the values values are initialized
	 */
	pyregf_values->key_object         = NULL;
	pyregf_values->get_value_by_index = NULL;
	pyregf_values->value_index        = 0;
	pyregf_values->number_of_values   = 0;

	return( 0 );
}

/* Frees a values object
 */
void pyregf_values_free(
      pyregf_values_t *pyregf_values )
{
	struct _typeobject *ob_type = NULL;
	static char *function       = "pyregf_values_free";

	if( pyregf_values == NULL )
	{
		PyErr_Format(
		 PyExc_ValueError,
		 "%s: invalid values.",
		 function );

		return;
	}
	ob_type = Py_TYPE(
	           pyregf_values );

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
	if( pyregf_values->key_object != NULL )
	{
		Py_DecRef(
		 (PyObject *) pyregf_values->key_object );
	}
	ob_type->tp_free(
	 (PyObject*) pyregf_values );
}

/* The values len() function
 */
Py_ssize_t pyregf_values_len(
            pyregf_values_t *pyregf_values )
{
	static char *function = "pyregf_values_len";

	if( pyregf_values == NULL )
	{
		PyErr_Format(
		 PyExc_ValueError,
		 "%s: invalid values.",
		 function );

		return( -1 );
	}
	return( (Py_ssize_t) pyregf_values->number_of_values );
}

/* The values getitem() function
 */
PyObject *pyregf_values_getitem(
           pyregf_values_t *pyregf_values,
           Py_ssize_t item_index )
{
	PyObject *value_object = NULL;
	static char *function  = "pyregf_values_getitem";

	if( pyregf_values == NULL )
	{
		PyErr_Format(
		 PyExc_ValueError,
		 "%s: invalid values.",
		 function );

		return( NULL );
	}
	if( pyregf_values->get_value_by_index == NULL )
	{
		PyErr_Format(
		 PyExc_ValueError,
		 "%s: invalid values - missing get value by index function.",
		 function );

		return( NULL );
	}
	if( pyregf_values->number_of_values < 0 )
	{
		PyErr_Format(
		 PyExc_ValueError,
		 "%s: invalid values - invalid number of -1ecords.",
		 function );

		return( NULL );
	}
	if( ( item_index < 0 )
	 || ( item_index >= (Py_ssize_t) pyregf_values->number_of_values ) )
	{
		PyErr_Format(
		 PyExc_ValueError,
		 "%s: invalid invalid item index value out of bounds.",
		 function );

		return( NULL );
	}
	value_object = pyregf_values->get_value_by_index(
	                pyregf_values->key_object,
	                (int) item_index );

	return( value_object );
}

/* The values iter() function
 */
PyObject *pyregf_values_iter(
           pyregf_values_t *pyregf_values )
{
	static char *function = "pyregf_values_iter";

	if( pyregf_values == NULL )
	{
		PyErr_Format(
		 PyExc_ValueError,
		 "%s: invalid values.",
		 function );

		return( NULL );
	}
	Py_IncRef(
	 (PyObject *) pyregf_values );

	return( (PyObject *) pyregf_values );
}

/* The values iternext() function
 */
PyObject *pyregf_values_iternext(
           pyregf_values_t *pyregf_values )
{
	PyObject *value_object = NULL;
	static char *function  = "pyregf_values_iternext";

	if( pyregf_values == NULL )
	{
		PyErr_Format(
		 PyExc_ValueError,
		 "%s: invalid values.",
		 function );

		return( NULL );
	}
	if( pyregf_values->get_value_by_index == NULL )
	{
		PyErr_Format(
		 PyExc_ValueError,
		 "%s: invalid values - missing get value by index function.",
		 function );

		return( NULL );
	}
	if( pyregf_values->value_index < 0 )
	{
		PyErr_Format(
		 PyExc_ValueError,
		 "%s: invalid values - invalid value index.",
		 function );

		return( NULL );
	}
	if( pyregf_values->number_of_values < 0 )
	{
		PyErr_Format(
		 PyExc_ValueError,
		 "%s: invalid values - invalid number of values.",
		 function );

		return( NULL );
	}
	if( pyregf_values->value_index >= pyregf_values->number_of_values )
	{
		PyErr_SetNone(
		 PyExc_StopIteration );

		return( NULL );
	}
	value_object = pyregf_values->get_value_by_index(
	                pyregf_values->key_object,
	                pyregf_values->value_index );

	if( value_object != NULL )
	{
		pyregf_values->value_index++;
	}
	return( value_object );
}

