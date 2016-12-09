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

#include <common.h>
#include <narrow_string.h>
#include <types.h>

#if defined( HAVE_STDLIB_H ) || defined( HAVE_WINAPI )
#include <stdlib.h>
#endif

#include "pyregf_datetime.h"
#include "pyregf_error.h"
#include "pyregf_file.h"
#include "pyregf_integer.h"
#include "pyregf_key.h"
#include "pyregf_keys.h"
#include "pyregf_libcerror.h"
#include "pyregf_libregf.h"
#include "pyregf_python.h"
#include "pyregf_unused.h"
#include "pyregf_value.h"
#include "pyregf_values.h"

PyMethodDef pyregf_key_object_methods[] = {

	/* Functions to access the key values */

	{ "is_corrupted",
	  (PyCFunction) pyregf_key_is_corrupted,
	  METH_NOARGS,
	  "is_corrupted() -> Boolean\n"
	  "\n"
	  "Indicates if the key is corrupted." },

	{ "get_offset",
	  (PyCFunction) pyregf_key_get_offset,
	  METH_NOARGS,
	  "get_offset() -> Integer\n"
	  "\n"
	  "Retrieves the offset." },

	{ "get_name",
	  (PyCFunction) pyregf_key_get_name,
	  METH_NOARGS,
	  "get_name -> Unicode string or None\n"
	  "\n"
	  "Retrieves the name." },

	{ "get_class_name",
	  (PyCFunction) pyregf_key_get_class_name,
	  METH_NOARGS,
	  "get_class_name -> Unicode string or None\n"
	  "\n"
	  "Retrieves the class name." },

	{ "get_last_written_time",
	  (PyCFunction) pyregf_key_get_last_written_time,
	  METH_NOARGS,
	  "get_last_written_time() -> Datetime\n"
	  "\n"
	  "Returns the last written date and time." },

	{ "get_last_written_time_as_integer",
	  (PyCFunction) pyregf_key_get_last_written_time_as_integer,
	  METH_NOARGS,
	  "get_last_written_time_as_integer() -> Integer\n"
	  "\n"
	  "Returns the last written date and time as a 64-bit integer containing a FILETIME value." },

	/* Functions to access the sub keys */

	{ "get_number_of_sub_keys",
	  (PyCFunction) pyregf_key_get_number_of_sub_keys,
	  METH_NOARGS,
	  "get_number_of_sub_keys() -> Integer\n"
	  "\n"
	  "Retrieves the number of sub keys." },

	{ "get_sub_key",
	  (PyCFunction) pyregf_key_get_sub_key,
	  METH_VARARGS | METH_KEYWORDS,
	  "get_sub_key(index) -> Object or None\n"
	  "\n"
	  "Retrieves a specific sub key." },

	{ "get_sub_key_by_name",
	  (PyCFunction) pyregf_key_get_sub_key_by_name,
	  METH_VARARGS | METH_KEYWORDS,
	  "get_sub_key_by_name(name) -> Object or None\n"
	  "\n"
	  "Retrieves a sub key specified by the sub key name." },

	{ "get_sub_key_by_path",
	  (PyCFunction) pyregf_key_get_sub_key_by_path,
	  METH_VARARGS | METH_KEYWORDS,
	  "get_sub_key_by_path(path) -> Object or None\n"
	  "\n"
	  "Retrieves a sub key specified by the sub key path." },

	/* Functions to access the values */

	{ "get_number_of_values",
	  (PyCFunction) pyregf_key_get_number_of_values,
	  METH_NOARGS,
	  "get_number_of_values() -> Integer\n"
	  "\n"
	  "Retrieves the number of values." },

	{ "get_value",
	  (PyCFunction) pyregf_key_get_value,
	  METH_VARARGS | METH_KEYWORDS,
	  "get_value(index) -> Object or None\n"
	  "\n"
	  "Retrieves a specific value." },

	{ "get_value_by_name",
	  (PyCFunction) pyregf_key_get_value_by_name,
	  METH_VARARGS | METH_KEYWORDS,
	  "get_value_by_name(name) -> Object or None\n"
	  "\n"
	  "Retrieves a value specified by the value name, use an empty string to retrieve the default value." },

	/* Sentinel */
	{ NULL, NULL, 0, NULL }
};

PyGetSetDef pyregf_key_object_get_set_definitions[] = {

	{ "corrupted",
	  (getter) pyregf_key_is_corrupted,
	  (setter) 0,
	  "Value to indicate the key is corrupted.",
	  NULL },

	{ "offset",
	  (getter) pyregf_key_get_offset,
	  (setter) 0,
	  "The offset.",
	  NULL },

	{ "name",
	  (getter) pyregf_key_get_name,
	  (setter) 0,
	  "The name.",
	  NULL },

	{ "class_name",
	  (getter) pyregf_key_get_class_name,
	  (setter) 0,
	  "The class name.",
	  NULL },

	{ "last_written_time",
	  (getter) pyregf_key_get_last_written_time,
	  (setter) 0,
	  "The last written date and time.",
	  NULL },

	{ "number_of_sub_keys",
	  (getter) pyregf_key_get_number_of_sub_keys,
	  (setter) 0,
	  "The number of sub keys.",
	  NULL },

	{ "sub_keys",
	  (getter) pyregf_key_get_sub_keys,
	  (setter) 0,
	  "The sub keys.",
	  NULL },

	{ "number_of_values",
	  (getter) pyregf_key_get_number_of_values,
	  (setter) 0,
	  "The number of values.",
	  NULL },

	{ "values",
	  (getter) pyregf_key_get_values,
	  (setter) 0,
	  "The values.",
	  NULL },

	/* Sentinel */
	{ NULL, NULL, NULL, NULL, NULL }
};

PyTypeObject pyregf_key_type_object = {
	PyVarObject_HEAD_INIT( NULL, 0 )

	/* tp_name */
	"pyregf.key",
	/* tp_basicsize */
	sizeof( pyregf_key_t ),
	/* tp_itemsize */
	0,
	/* tp_dealloc */
	(destructor) pyregf_key_free,
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
	/* tp_flags */
	Py_TPFLAGS_DEFAULT,
	/* tp_doc */
	"pyregf key object (wraps libregf_key_t)",
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
	pyregf_key_object_methods,
	/* tp_members */
	0,
	/* tp_getset */
	pyregf_key_object_get_set_definitions,
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
	(initproc) pyregf_key_init,
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

/* Creates a new key object
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyregf_key_new(
           libregf_key_t *key,
           pyregf_file_t *file_object )
{
	pyregf_key_t *pyregf_key = NULL;
	static char *function    = "pyregf_key_new";

	if( key == NULL )
	{
		PyErr_Format(
		 PyExc_TypeError,
		 "%s: invalid key.",
		 function );

		return( NULL );
	}
	pyregf_key = PyObject_New(
	              struct pyregf_key,
	              &pyregf_key_type_object );

	if( pyregf_key == NULL )
	{
		PyErr_Format(
		 PyExc_MemoryError,
		 "%s: unable to initialize key.",
		 function );

		goto on_error;
	}
	if( pyregf_key_init(
	     pyregf_key ) != 0 )
	{
		PyErr_Format(
		 PyExc_MemoryError,
		 "%s: unable to initialize key.",
		 function );

		goto on_error;
	}
	pyregf_key->key         = key;
	pyregf_key->file_object = file_object;

	Py_IncRef(
	 (PyObject *) pyregf_key->file_object );

	return( (PyObject *) pyregf_key );

on_error:
	if( pyregf_key != NULL )
	{
		Py_DecRef(
		 (PyObject *) pyregf_key );
	}
	return( NULL );
}

/* Intializes an key object
 * Returns 0 if successful or -1 on error
 */
int pyregf_key_init(
     pyregf_key_t *pyregf_key )
{
	static char *function = "pyregf_key_init";

	if( pyregf_key == NULL )
	{
		PyErr_Format(
		 PyExc_TypeError,
		 "%s: invalid key.",
		 function );

		return( -1 );
	}
	/* Make sure libregf key is set to NULL
	 */
	pyregf_key->key = NULL;

	return( 0 );
}

/* Frees an key object
 */
void pyregf_key_free(
      pyregf_key_t *pyregf_key )
{
	libcerror_error_t *error    = NULL;
	struct _typeobject *ob_type = NULL;
	static char *function       = "pyregf_key_free";

	if( pyregf_key == NULL )
	{
		PyErr_Format(
		 PyExc_TypeError,
		 "%s: invalid key.",
		 function );

		return;
	}
	if( pyregf_key->key == NULL )
	{
		PyErr_Format(
		 PyExc_TypeError,
		 "%s: invalid key - missing libregf key.",
		 function );

		return;
	}
	ob_type = Py_TYPE(
	           pyregf_key );

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
	if( libregf_key_free(
	     &( pyregf_key->key ),
	     &error ) != 1 )
	{
		pyregf_error_raise(
		 error,
		 PyExc_IOError,
		 "%s: unable to free libregf key.",
		 function );

		libcerror_error_free(
		 &error );
	}
	if( pyregf_key->file_object != NULL )
	{
		Py_DecRef(
		 (PyObject *) pyregf_key->file_object );
	}
	ob_type->tp_free(
	 (PyObject*) pyregf_key );
}

/* Determines if the key is corrupted
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyregf_key_is_corrupted(
           pyregf_key_t *pyregf_key,
           PyObject *arguments PYREGF_ATTRIBUTE_UNUSED )
{
	libcerror_error_t *error = NULL;
	static char *function    = "pyregf_key_is_corrupted";
	int result               = 0;

	PYREGF_UNREFERENCED_PARAMETER( arguments )

	if( pyregf_key == NULL )
	{
		PyErr_Format(
		 PyExc_ValueError,
		 "%s: invalid key.",
		 function );

		return( NULL );
	}
	Py_BEGIN_ALLOW_THREADS

	result = libregf_key_is_corrupted(
	          pyregf_key->key,
	          &error );

	Py_END_ALLOW_THREADS

	if( result == -1 )
	{
		pyregf_error_raise(
		 error,
		 PyExc_IOError,
		 "%s: unable to determine if key is corrupted.",
		 function );

		libcerror_error_free(
		 &error );

		return( NULL );
	}
	if( result != 0 )
	{
		Py_IncRef(
		 (PyObject *) Py_True );

		return( Py_True );
	}
	Py_IncRef(
	 (PyObject *) Py_False );

	return( Py_False );
}

/* Retrieves the offset
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyregf_key_get_offset(
           pyregf_key_t *pyregf_key,
           PyObject *arguments PYREGF_ATTRIBUTE_UNUSED )
{
	libcerror_error_t *error = NULL;
	PyObject *integer_object = NULL;
	static char *function    = "pyregf_key_get_offset";
	off64_t offset           = 0;
	int result               = 0;

	PYREGF_UNREFERENCED_PARAMETER( arguments )

	if( pyregf_key == NULL )
	{
		PyErr_Format(
		 PyExc_TypeError,
		 "%s: invalid key.",
		 function );

		return( NULL );
	}
	Py_BEGIN_ALLOW_THREADS

	result = libregf_key_get_offset(
	          pyregf_key->key,
	          &offset,
	          &error );

	Py_END_ALLOW_THREADS

	if( result != 1 )
	{
		pyregf_error_raise(
		 error,
		 PyExc_IOError,
		 "%s: unable to retrieve offset.",
		 function );

		libcerror_error_free(
		 &error );

		return( NULL );
	}
	integer_object = pyregf_integer_signed_new_from_64bit(
	                  (int64_t) offset );

	return( integer_object );
}

/* Retrieves the name
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyregf_key_get_name(
           pyregf_key_t *pyregf_key,
           PyObject *arguments PYREGF_ATTRIBUTE_UNUSED )
{
	libcerror_error_t *error = NULL;
	PyObject *string_object  = NULL;
	const char *errors       = NULL;
	uint8_t *name            = NULL;
	static char *function    = "pyregf_key_get_name";
	size_t name_size         = 0;
	int result               = 0;

	PYREGF_UNREFERENCED_PARAMETER( arguments )

	if( pyregf_key == NULL )
	{
		PyErr_Format(
		 PyExc_TypeError,
		 "%s: invalid key.",
		 function );

		return( NULL );
	}
	Py_BEGIN_ALLOW_THREADS

	result = libregf_key_get_utf8_name_size(
	          pyregf_key->key,
	          &name_size,
	          &error );

	Py_END_ALLOW_THREADS

	if( result == -1 )
	{
		pyregf_error_raise(
		 error,
		 PyExc_IOError,
		 "%s: unable to retrieve name size.",
		 function );

		libcerror_error_free(
		 &error );

		goto on_error;
	}
	else if( ( result == 0 )
	      || ( name_size == 0 ) )
	{
		Py_IncRef(
		 Py_None );

		return( Py_None );
	}
	name = (uint8_t *) PyMem_Malloc(
	                    sizeof( uint8_t ) * name_size );

	if( name == NULL )
	{
		PyErr_Format(
		 PyExc_IOError,
		 "%s: unable to create name.",
		 function );

		goto on_error;
	}
	Py_BEGIN_ALLOW_THREADS

	result = libregf_key_get_utf8_name(
		  pyregf_key->key,
		  name,
		  name_size,
		  &error );

	Py_END_ALLOW_THREADS

	if( result != 1 )
	{
		pyregf_error_raise(
		 error,
		 PyExc_IOError,
		 "%s: unable to retrieve name.",
		 function );

		libcerror_error_free(
		 &error );

		goto on_error;
	}
	/* Pass the string length to PyUnicode_DecodeUTF8
	 * otherwise it makes the end of string character is part
	 * of the string
	 */
	string_object = PyUnicode_DecodeUTF8(
			 (char *) name,
			 (Py_ssize_t) name_size - 1,
			 errors );

	PyMem_Free(
	 name );

	return( string_object );

on_error:
	if( name != NULL )
	{
		PyMem_Free(
		 name );
	}
	return( NULL );
}

/* Retrieves the class name
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyregf_key_get_class_name(
           pyregf_key_t *pyregf_key,
           PyObject *arguments PYREGF_ATTRIBUTE_UNUSED )
{
	libcerror_error_t *error = NULL;
	PyObject *string_object  = NULL;
	const char *errors       = NULL;
	uint8_t *class_name      = NULL;
	static char *function    = "pyregf_key_get_class_name";
	size_t class_name_size   = 0;
	int result               = 0;

	PYREGF_UNREFERENCED_PARAMETER( arguments )

	if( pyregf_key == NULL )
	{
		PyErr_Format(
		 PyExc_TypeError,
		 "%s: invalid key.",
		 function );

		return( NULL );
	}
	Py_BEGIN_ALLOW_THREADS

	result = libregf_key_get_utf8_class_name_size(
	          pyregf_key->key,
	          &class_name_size,
	          &error );

	Py_END_ALLOW_THREADS

	if( result == -1 )
	{
		pyregf_error_raise(
		 error,
		 PyExc_IOError,
		 "%s: unable to retrieve class name size.",
		 function );

		libcerror_error_free(
		 &error );

		goto on_error;
	}
	else if( ( result == 0 )
	      || ( class_name_size == 0 ) )
	{
		Py_IncRef(
		 Py_None );

		return( Py_None );
	}
	class_name = (uint8_t *) PyMem_Malloc(
	                          sizeof( uint8_t ) * class_name_size );

	if( class_name == NULL )
	{
		PyErr_Format(
		 PyExc_IOError,
		 "%s: unable to create class name.",
		 function );

		goto on_error;
	}
	Py_BEGIN_ALLOW_THREADS

	result = libregf_key_get_utf8_class_name(
		  pyregf_key->key,
		  class_name,
		  class_name_size,
		  &error );

	Py_END_ALLOW_THREADS

	if( result != 1 )
	{
		pyregf_error_raise(
		 error,
		 PyExc_IOError,
		 "%s: unable to retrieve class name.",
		 function );

		libcerror_error_free(
		 &error );

		goto on_error;
	}
	/* Pass the string length to PyUnicode_DecodeUTF8
	 * otherwise it makes the end of string character is part
	 * of the string
	 */
	string_object = PyUnicode_DecodeUTF8(
			 (char *) class_name,
			 (Py_ssize_t) class_name_size - 1,
			 errors );

	PyMem_Free(
	 class_name );

	return( string_object );

on_error:
	if( class_name != NULL )
	{
		PyMem_Free(
		 class_name );
	}
	return( NULL );
}

/* Retrieves the last written date and time
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyregf_key_get_last_written_time(
           pyregf_key_t *pyregf_key,
           PyObject *arguments PYREGF_ATTRIBUTE_UNUSED )
{
	libcerror_error_t *error   = NULL;
	PyObject *date_time_object = NULL;
	static char *function      = "pyregf_key_get_last_written_time";
	uint64_t filetime          = 0;
	int result                 = 0;

	PYREGF_UNREFERENCED_PARAMETER( arguments )

	if( pyregf_key == NULL )
	{
		PyErr_Format(
		 PyExc_ValueError,
		 "%s: invalid key.",
		 function );

		return( NULL );
	}
	Py_BEGIN_ALLOW_THREADS

	result = libregf_key_get_last_written_time(
	          pyregf_key->key,
	          &filetime,
	          &error );

	Py_END_ALLOW_THREADS

	if( result != 1 )
	{
		pyregf_error_raise(
		 error,
		 PyExc_IOError,
		 "%s: unable to retrieve last written time.",
		 function );

		libcerror_error_free(
		 &error );

		return( NULL );
	}
	date_time_object = pyregf_datetime_new_from_filetime(
	                    filetime );

	return( date_time_object );
}

/* Retrieves the last written date and time as an integer
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyregf_key_get_last_written_time_as_integer(
           pyregf_key_t *pyregf_key,
           PyObject *arguments PYREGF_ATTRIBUTE_UNUSED )
{
	libcerror_error_t *error = NULL;
	PyObject *integer_object = NULL;
	static char *function    = "pyregf_key_get_last_written_time_as_integer";
	uint64_t filetime        = 0;
	int result               = 0;

	PYREGF_UNREFERENCED_PARAMETER( arguments )

	if( pyregf_key == NULL )
	{
		PyErr_Format(
		 PyExc_ValueError,
		 "%s: invalid key.",
		 function );

		return( NULL );
	}
	Py_BEGIN_ALLOW_THREADS

	result = libregf_key_get_last_written_time(
	          pyregf_key->key,
	          &filetime,
	          &error );

	Py_END_ALLOW_THREADS

	if( result != 1 )
	{
		pyregf_error_raise(
		 error,
		 PyExc_IOError,
		 "%s: unable to retrieve last written time.",
		 function );

		libcerror_error_free(
		 &error );

		return( NULL );
	}
	integer_object = pyregf_integer_unsigned_new_from_64bit(
	                  (uint64_t) filetime );

	return( integer_object );
}

/* Retrieves the number of sub keys
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyregf_key_get_number_of_sub_keys(
           pyregf_key_t *pyregf_key,
           PyObject *arguments PYREGF_ATTRIBUTE_UNUSED )
{
	libcerror_error_t *error = NULL;
	PyObject *integer_object = NULL;
	static char *function    = "pyregf_key_get_number_of_sub_keys";
	int number_of_sub_keys   = 0;
	int result               = 0;

	PYREGF_UNREFERENCED_PARAMETER( arguments )

	if( pyregf_key == NULL )
	{
		PyErr_Format(
		 PyExc_TypeError,
		 "%s: invalid key.",
		 function );

		return( NULL );
	}
	Py_BEGIN_ALLOW_THREADS

	result = libregf_key_get_number_of_sub_keys(
	          pyregf_key->key,
	          &number_of_sub_keys,
	          &error );

	Py_END_ALLOW_THREADS

	if( result != 1 )
	{
		pyregf_error_raise(
		 error,
		 PyExc_IOError,
		 "%s: unable to retrieve number of sub keys.",
		 function );

		libcerror_error_free(
		 &error );

		return( NULL );
	}
#if PY_MAJOR_VERSION >= 3
	integer_object = PyLong_FromLong(
	                  (long) number_of_sub_keys );
#else
	integer_object = PyInt_FromLong(
	                  (long) number_of_sub_keys );
#endif
	return( integer_object );
}

/* Retrieves a specific sub key by index
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyregf_key_get_sub_key_by_index(
           pyregf_key_t *pyregf_key,
           int sub_key_index )
{
	libcerror_error_t *error = NULL;
	libregf_key_t *sub_key   = NULL;
	PyObject *key_object     = NULL;
	static char *function    = "pyregf_key_get_sub_key_by_index";
	int result               = 0;

	if( pyregf_key == NULL )
	{
		PyErr_Format(
		 PyExc_TypeError,
		 "%s: invalid key.",
		 function );

		return( NULL );
	}
	Py_BEGIN_ALLOW_THREADS

	result = libregf_key_get_sub_key(
	          pyregf_key->key,
	          sub_key_index,
	          &sub_key,
	          &error );

	Py_END_ALLOW_THREADS

	if( result != 1 )
	{
		pyregf_error_raise(
		 error,
		 PyExc_IOError,
		 "%s: unable to retrieve sub key: %d.",
		 function,
		 sub_key_index );

		libcerror_error_free(
		 &error );

		goto on_error;
	}
	key_object = pyregf_key_new(
	              sub_key,
	              pyregf_key->file_object );

	if( key_object == NULL )
	{
		PyErr_Format(
		 PyExc_MemoryError,
		 "%s: unable to create key object.",
		 function );

		goto on_error;
	}
	return( key_object );

on_error:
	if( sub_key != NULL )
	{
		libregf_key_free(
		 &sub_key,
		 NULL );
	}
	return( NULL );
}

/* Retrieves a specific sub key
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyregf_key_get_sub_key(
           pyregf_key_t *pyregf_key,
           PyObject *arguments,
           PyObject *keywords )
{
	PyObject *key_object        = NULL;
	static char *keyword_list[] = { "sub_key_index", NULL };
	int sub_key_index           = 0;

	if( PyArg_ParseTupleAndKeywords(
	     arguments,
	     keywords,
	     "i",
	     keyword_list,
	     &sub_key_index ) == 0 )
	{
		return( NULL );
	}
	key_object = pyregf_key_get_sub_key_by_index(
	              pyregf_key,
	              sub_key_index );

	return( key_object );
}

/* Retrieves a keys sequence and iterator object for the sub keys
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyregf_key_get_sub_keys(
           pyregf_key_t *pyregf_key,
           PyObject *arguments PYREGF_ATTRIBUTE_UNUSED )
{
	libcerror_error_t *error  = NULL;
	PyObject *sub_keys_object = NULL;
	static char *function     = "pyregf_key_get_sub_keys";
	int number_of_sub_keys    = 0;
	int result                = 0;

	PYREGF_UNREFERENCED_PARAMETER( arguments )

	if( pyregf_key == NULL )
	{
		PyErr_Format(
		 PyExc_TypeError,
		 "%s: invalid key.",
		 function );

		return( NULL );
	}
	Py_BEGIN_ALLOW_THREADS

	result = libregf_key_get_number_of_sub_keys(
	          pyregf_key->key,
	          &number_of_sub_keys,
	          &error );

	Py_END_ALLOW_THREADS

	if( result != 1 )
	{
		pyregf_error_raise(
		 error,
		 PyExc_IOError,
		 "%s: unable to retrieve number of sub keys.",
		 function );

		libcerror_error_free(
		 &error );

		return( NULL );
	}
	sub_keys_object = pyregf_keys_new(
	                   pyregf_key,
	                   &pyregf_key_get_sub_key_by_index,
	                   number_of_sub_keys );

	if( sub_keys_object == NULL )
	{
		PyErr_Format(
		 PyExc_MemoryError,
		 "%s: unable to create sub keys object.",
		 function );

		return( NULL );
	}
	return( sub_keys_object );
}

/* Retrieves the sub key specified by the name
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyregf_key_get_sub_key_by_name(
           pyregf_key_t *pyregf_key,
           PyObject *arguments,
           PyObject *keywords )
{
	libcerror_error_t *error    = NULL;
	libregf_key_t *sub_key      = NULL;
	PyObject *key_object        = NULL;
	char *sub_key_name          = NULL;
	static char *keyword_list[] = { "sub_key_name", NULL };
	static char *function       = "pyregf_key_get_sub_key_by_name";
	size_t sub_key_name_length  = 0;
	int result                  = 0;

	if( pyregf_key == NULL )
	{
		PyErr_Format(
		 PyExc_TypeError,
		 "%s: invalid key.",
		 function );

		return( NULL );
	}
	if( PyArg_ParseTupleAndKeywords(
	     arguments,
	     keywords,
	     "s",
	     keyword_list,
	     &sub_key_name ) == 0 )
	{
		goto on_error;
	}
	sub_key_name_length = narrow_string_length(
	                       sub_key_name );

	Py_BEGIN_ALLOW_THREADS

	result = libregf_key_get_sub_key_by_utf8_name(
	           pyregf_key->key,
	           (uint8_t *) sub_key_name,
	           sub_key_name_length,
	           &sub_key,
	           &error );

	Py_END_ALLOW_THREADS

	if( result == -1 )
	{
		pyregf_error_raise(
		 error,
		 PyExc_IOError,
		 "%s: unable to retrieve sub key.",
		 function );

		libcerror_error_free(
		 &error );

		goto on_error;
	}
	/* Check if the sub key is present
	 */
	else if( result == 0 )
	{
		Py_IncRef(
		 Py_None );

		return( Py_None );
	}
	key_object = pyregf_key_new(
	              sub_key,
	              pyregf_key->file_object );

	if( key_object == NULL )
	{
		PyErr_Format(
		 PyExc_MemoryError,
		 "%s: unable to create key object.",
		 function );

		goto on_error;
	}
	return( key_object );

on_error:
	if( sub_key != NULL )
	{
		libregf_key_free(
		 &sub_key,
		 NULL );
	}
	return( NULL );
}

/* Retrieves the sub key specified by the path
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyregf_key_get_sub_key_by_path(
           pyregf_key_t *pyregf_key,
           PyObject *arguments,
           PyObject *keywords )
{
	libcerror_error_t *error    = NULL;
	libregf_key_t *sub_key      = NULL;
	PyObject *key_object        = NULL;
	char *sub_key_path          = NULL;
	static char *keyword_list[] = { "sub_key_path", NULL };
	static char *function       = "pyregf_key_get_sub_key_by_path";
	size_t sub_key_path_length  = 0;
	int result                  = 0;

	if( pyregf_key == NULL )
	{
		PyErr_Format(
		 PyExc_TypeError,
		 "%s: invalid key.",
		 function );

		return( NULL );
	}
	if( PyArg_ParseTupleAndKeywords(
	     arguments,
	     keywords,
	     "s",
	     keyword_list,
	     &sub_key_path ) == 0 )
	{
		goto on_error;
	}
	sub_key_path_length = narrow_string_length(
	                       sub_key_path );

	Py_BEGIN_ALLOW_THREADS

	result = libregf_key_get_sub_key_by_utf8_path(
	           pyregf_key->key,
	           (uint8_t *) sub_key_path,
	           sub_key_path_length,
	           &sub_key,
	           &error );

	Py_END_ALLOW_THREADS

	if( result == -1 )
	{
		pyregf_error_raise(
		 error,
		 PyExc_IOError,
		 "%s: unable to retrieve sub key.",
		 function );

		libcerror_error_free(
		 &error );

		goto on_error;
	}
	/* Check if the sub key is present
	 */
	else if( result == 0 )
	{
		Py_IncRef(
		 Py_None );

		return( Py_None );
	}
	key_object = pyregf_key_new(
	              sub_key,
	              pyregf_key->file_object );

	if( key_object == NULL )
	{
		PyErr_Format(
		 PyExc_MemoryError,
		 "%s: unable to create key object.",
		 function );

		goto on_error;
	}
	return( key_object );

on_error:
	if( sub_key != NULL )
	{
		libregf_key_free(
		 &sub_key,
		 NULL );
	}
	return( NULL );
}

/* Retrieves the number of values
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyregf_key_get_number_of_values(
           pyregf_key_t *pyregf_key,
           PyObject *arguments PYREGF_ATTRIBUTE_UNUSED )
{
	libcerror_error_t *error = NULL;
	PyObject *integer_object = NULL;
	static char *function    = "pyregf_key_get_number_of_values";
	int number_of_values     = 0;
	int result               = 0;

	PYREGF_UNREFERENCED_PARAMETER( arguments )

	if( pyregf_key == NULL )
	{
		PyErr_Format(
		 PyExc_TypeError,
		 "%s: invalid key.",
		 function );

		return( NULL );
	}
	Py_BEGIN_ALLOW_THREADS

	result = libregf_key_get_number_of_values(
	          pyregf_key->key,
	          &number_of_values,
	          &error );

	Py_END_ALLOW_THREADS

	if( result != 1 )
	{
		pyregf_error_raise(
		 error,
		 PyExc_IOError,
		 "%s: unable to retrieve number of values.",
		 function );

		libcerror_error_free(
		 &error );

		return( NULL );
	}
#if PY_MAJOR_VERSION >= 3
	integer_object = PyLong_FromLong(
	                  (long) number_of_values );
#else
	integer_object = PyInt_FromLong(
	                  (long) number_of_values );
#endif
	return( integer_object );
}

/* Retrieves a specific value by index
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyregf_key_get_value_by_index(
           pyregf_key_t *pyregf_key,
           int value_index )
{
	libcerror_error_t *error = NULL;
	libregf_value_t *value   = NULL;
	PyObject *value_object   = NULL;
	static char *function    = "pyregf_key_get_value";
	int result               = 0;

	if( pyregf_key == NULL )
	{
		PyErr_Format(
		 PyExc_TypeError,
		 "%s: invalid key.",
		 function );

		return( NULL );
	}
	Py_BEGIN_ALLOW_THREADS

	result = libregf_key_get_value(
	          pyregf_key->key,
	          value_index,
	          &value,
	          &error );

	Py_END_ALLOW_THREADS

	if( result != 1 )
	{
		pyregf_error_raise(
		 error,
		 PyExc_IOError,
		 "%s: unable to retrieve value: %d.",
		 function,
		 value_index );

		libcerror_error_free(
		 &error );

		goto on_error;
	}
	value_object = pyregf_value_new(
	                value,
	                pyregf_key->file_object );

	if( value_object == NULL )
	{
		PyErr_Format(
		 PyExc_MemoryError,
		 "%s: unable to create value object.",
		 function );

		goto on_error;
	}
	return( value_object );

on_error:
	if( value != NULL )
	{
		libregf_value_free(
		 &value,
		 NULL );
	}
	return( NULL );
}

/* Retrieves a specific value
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyregf_key_get_value(
           pyregf_key_t *pyregf_key,
           PyObject *arguments,
           PyObject *keywords )
{
	PyObject *value_object      = NULL;
	static char *keyword_list[] = { "value_index", NULL };
	int value_index             = 0;

	if( PyArg_ParseTupleAndKeywords(
	     arguments,
	     keywords,
	     "i",
	     keyword_list,
	     &value_index ) == 0 )
	{
		return( NULL );
	}
	value_object = pyregf_key_get_value_by_index(
	                pyregf_key,
	                value_index );

	return( value_object );
}

/* Retrieves a values sequence and iterator object for the values
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyregf_key_get_values(
           pyregf_key_t *pyregf_key,
           PyObject *arguments PYREGF_ATTRIBUTE_UNUSED )
{
	libcerror_error_t *error = NULL;
	PyObject *values_object  = NULL;
	static char *function    = "pyregf_key_get_values";
	int number_of_values     = 0;
	int result               = 0;

	PYREGF_UNREFERENCED_PARAMETER( arguments )

	if( pyregf_key == NULL )
	{
		PyErr_Format(
		 PyExc_TypeError,
		 "%s: invalid key.",
		 function );

		return( NULL );
	}
	Py_BEGIN_ALLOW_THREADS

	result = libregf_key_get_number_of_values(
	          pyregf_key->key,
	          &number_of_values,
	          &error );

	Py_END_ALLOW_THREADS

	if( result != 1 )
	{
		pyregf_error_raise(
		 error,
		 PyExc_IOError,
		 "%s: unable to retrieve number of values.",
		 function );

		libcerror_error_free(
		 &error );

		return( NULL );
	}
	values_object = pyregf_values_new(
	                 pyregf_key,
	                 &pyregf_key_get_value_by_index,
	                 number_of_values );

	if( values_object == NULL )
	{
		PyErr_Format(
		 PyExc_MemoryError,
		 "%s: unable to create values object.",
		 function );

		return( NULL );
	}
	return( values_object );
}

/* Retrieves the value specified by the name
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyregf_key_get_value_by_name(
           pyregf_key_t *pyregf_key,
           PyObject *arguments,
           PyObject *keywords )
{
	libcerror_error_t *error    = NULL;
	libregf_value_t *value      = NULL;
	PyObject *value_object      = NULL;
	char *value_name            = NULL;
	static char *keyword_list[] = { "value_name", NULL };
	static char *function       = "pyregf_key_get_value_by_name";
	size_t value_name_length    = 0;
	int result                  = 0;

	if( pyregf_key == NULL )
	{
		PyErr_Format(
		 PyExc_TypeError,
		 "%s: invalid key.",
		 function );

		return( NULL );
	}
	if( PyArg_ParseTupleAndKeywords(
	     arguments,
	     keywords,
	     "s",
	     keyword_list,
	     &value_name ) == 0 )
	{
		goto on_error;
	}
	value_name_length = narrow_string_length(
	                     value_name );

	if( value_name_length == 0 )
	{
		value_name = NULL;
	}
	Py_BEGIN_ALLOW_THREADS

	result = libregf_key_get_value_by_utf8_name(
	           pyregf_key->key,
	           (uint8_t *) value_name,
	           value_name_length,
	           &value,
	           &error );

	Py_END_ALLOW_THREADS

	if( result == -1 )
	{
		pyregf_error_raise(
		 error,
		 PyExc_IOError,
		 "%s: unable to retrieve value.",
		 function );

		libcerror_error_free(
		 &error );

		goto on_error;
	}
	/* Check if the value is present
	 */
	else if( result == 0 )
	{
		Py_IncRef(
		 Py_None );

		return( Py_None );
	}
	value_object = pyregf_value_new(
	                value,
	                pyregf_key->file_object );

	if( value_object == NULL )
	{
		PyErr_Format(
		 PyExc_MemoryError,
		 "%s: unable to create value object.",
		 function );

		goto on_error;
	}
	return( value_object );

on_error:
	if( value != NULL )
	{
		libregf_value_free(
		 &value,
		 NULL );
	}
	return( NULL );
}

