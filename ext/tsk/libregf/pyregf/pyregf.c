/*
 * Python bindings module for libregf (pyregf)
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

#include "pyregf.h"
#include "pyregf_error.h"
#include "pyregf_file.h"
#include "pyregf_file_object_io_handle.h"
#include "pyregf_file_types.h"
#include "pyregf_key.h"
#include "pyregf_keys.h"
#include "pyregf_libcerror.h"
#include "pyregf_libregf.h"
#include "pyregf_python.h"
#include "pyregf_unused.h"
#include "pyregf_value.h"
#include "pyregf_value_types.h"
#include "pyregf_values.h"

#if !defined( LIBREGF_HAVE_BFIO )
LIBREGF_EXTERN \
int libregf_check_file_signature_file_io_handle(
     libbfio_handle_t *file_io_handle,
     libregf_error_t **error );
#endif

/* The pyregf module methods
 */
PyMethodDef pyregf_module_methods[] = {
	{ "get_version",
	  (PyCFunction) pyregf_get_version,
	  METH_NOARGS,
	  "get_version() -> String\n"
	  "\n"
	  "Retrieves the version." },

	{ "check_file_signature",
	  (PyCFunction) pyregf_check_file_signature,
	  METH_VARARGS | METH_KEYWORDS,
	  "check_file_signature(filename) -> Boolean\n"
	  "\n"
	  "Checks if a file has a Windows NT Registry File (REGF) signature." },

	{ "check_file_signature_file_object",
	  (PyCFunction) pyregf_check_file_signature_file_object,
	  METH_VARARGS | METH_KEYWORDS,
	  "check_file_signature_file_object(file_object) -> Boolean\n"
	  "\n"
	  "Checks if a file has a Windows NT Registry File (REGF) signature using a file-like object." },

	{ "open",
	  (PyCFunction) pyregf_file_new_open,
	  METH_VARARGS | METH_KEYWORDS,
	  "open(filename, mode='r') -> Object\n"
	  "\n"
	  "Opens a file." },

	{ "open_file_object",
	  (PyCFunction) pyregf_file_new_open_file_object,
	  METH_VARARGS | METH_KEYWORDS,
	  "open_file_object(file_boject, mode='r') -> Object\n"
	  "\n"
	  "Opens a file using a file-like object." },

	/* Sentinel */
	{ NULL,
	  NULL,
	  0,
	  NULL}
};

/* Retrieves the pyregf/libregf version
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyregf_get_version(
           PyObject *self PYREGF_ATTRIBUTE_UNUSED,
           PyObject *arguments PYREGF_ATTRIBUTE_UNUSED )
{
	const char *errors           = NULL;
	const char *version_string   = NULL;
	size_t version_string_length = 0;

	PYREGF_UNREFERENCED_PARAMETER( self )
	PYREGF_UNREFERENCED_PARAMETER( arguments )

	Py_BEGIN_ALLOW_THREADS

	version_string = libregf_get_version();

	Py_END_ALLOW_THREADS

	version_string_length = narrow_string_length(
	                         version_string );

	/* Pass the string length to PyUnicode_DecodeUTF8
	 * otherwise it makes the end of string character is part
	 * of the string
	 */
	return( PyUnicode_DecodeUTF8(
	         version_string,
	         (Py_ssize_t) version_string_length,
	         errors ) );
}

/* Checks if the file has a Windows NT Registry File (REGF) signature
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyregf_check_file_signature(
           PyObject *self PYREGF_ATTRIBUTE_UNUSED,
           PyObject *arguments,
           PyObject *keywords )
{
	PyObject *string_object      = NULL;
	libcerror_error_t *error     = NULL;
	static char *function        = "pyregf_check_file_signature";
	static char *keyword_list[]  = { "filename", NULL };
	const char *filename_narrow  = NULL;
	int result                   = 0;

#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
	const wchar_t *filename_wide = NULL;
#else
	PyObject *utf8_string_object = NULL;
#endif

	PYREGF_UNREFERENCED_PARAMETER( self )

	/* Note that PyArg_ParseTupleAndKeywords with "s" will force Unicode strings to be converted to narrow character string.
	 * On Windows the narrow character strings contains an extended ASCII string with a codepage. Hence we get a conversion
	 * exception. This will also fail if the default encoding is not set correctly. We cannot use "u" here either since that
	 * does not allow us to pass non Unicode string objects and Python (at least 2.7) does not seems to automatically upcast them.
	 */
	if( PyArg_ParseTupleAndKeywords(
	     arguments,
	     keywords,
	     "|O",
	     keyword_list,
	     &string_object ) == 0 )
	{
		return( NULL );
	}
	PyErr_Clear();

	result = PyObject_IsInstance(
	          string_object,
	          (PyObject *) &PyUnicode_Type );

	if( result == -1 )
	{
		pyregf_error_fetch_and_raise(
	         PyExc_RuntimeError,
		 "%s: unable to determine if string object is of type unicode.",
		 function );

		return( NULL );
	}
	else if( result != 0 )
	{
		PyErr_Clear();

#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
		filename_wide = (wchar_t *) PyUnicode_AsUnicode(
		                             string_object );
		Py_BEGIN_ALLOW_THREADS

		result = libregf_check_file_signature_wide(
		          filename_wide,
		          &error );

		Py_END_ALLOW_THREADS
#else
		utf8_string_object = PyUnicode_AsUTF8String(
		                      string_object );

		if( utf8_string_object == NULL )
		{
			pyregf_error_fetch_and_raise(
			 PyExc_RuntimeError,
			 "%s: unable to convert unicode string to UTF-8.",
			 function );

			return( NULL );
		}
#if PY_MAJOR_VERSION >= 3
		filename_narrow = PyBytes_AsString(
				   utf8_string_object );
#else
		filename_narrow = PyString_AsString(
				   utf8_string_object );
#endif
		Py_BEGIN_ALLOW_THREADS

		result = libregf_check_file_signature(
		          filename_narrow,
		          &error );

		Py_END_ALLOW_THREADS

		Py_DecRef(
		 utf8_string_object );
#endif
		if( result == -1 )
		{
			pyregf_error_raise(
			 error,
			 PyExc_IOError,
			 "%s: unable to check file signature.",
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
	PyErr_Clear();

#if PY_MAJOR_VERSION >= 3
	result = PyObject_IsInstance(
		  string_object,
		  (PyObject *) &PyBytes_Type );
#else
	result = PyObject_IsInstance(
		  string_object,
		  (PyObject *) &PyString_Type );
#endif
	if( result == -1 )
	{
		pyregf_error_fetch_and_raise(
	         PyExc_RuntimeError,
		 "%s: unable to determine if string object is of type string.",
		 function );

		return( NULL );
	}
	else if( result != 0 )
	{
		PyErr_Clear();

#if PY_MAJOR_VERSION >= 3
		filename_narrow = PyBytes_AsString(
				   string_object );
#else
		filename_narrow = PyString_AsString(
				   string_object );
#endif
		Py_BEGIN_ALLOW_THREADS

		result = libregf_check_file_signature(
		          filename_narrow,
		          &error );

		Py_END_ALLOW_THREADS

		if( result == -1 )
		{
			pyregf_error_raise(
			 error,
			 PyExc_IOError,
			 "%s: unable to check file signature.",
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
	PyErr_Format(
	 PyExc_TypeError,
	 "%s: unsupported string object type.",
	 function );

	return( NULL );
}

/* Checks if the file has a Windows NT Registry File (REGF) signature using a file-like object
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyregf_check_file_signature_file_object(
           PyObject *self PYREGF_ATTRIBUTE_UNUSED,
           PyObject *arguments,
           PyObject *keywords )
{
	libcerror_error_t *error         = NULL;
	libbfio_handle_t *file_io_handle = NULL;
	PyObject *file_object            = NULL;
	static char *function            = "pyregf_check_file_signature_file_object";
	static char *keyword_list[]      = { "file_object", NULL };
	int result                       = 0;

	PYREGF_UNREFERENCED_PARAMETER( self )

	if( PyArg_ParseTupleAndKeywords(
	     arguments,
	     keywords,
	     "|O",
	     keyword_list,
	     &file_object ) == 0 )
	{
		return( NULL );
	}
	if( pyregf_file_object_initialize(
	     &file_io_handle,
	     file_object,
	     &error ) != 1 )
	{
		pyregf_error_raise(
		 error,
		 PyExc_MemoryError,
		 "%s: unable to initialize file IO handle.",
		 function );

		libcerror_error_free(
		 &error );

		goto on_error;
	}
	Py_BEGIN_ALLOW_THREADS

	result = libregf_check_file_signature_file_io_handle(
	          file_io_handle,
	          &error );

	Py_END_ALLOW_THREADS

	if( result == -1 )
	{
		pyregf_error_raise(
		 error,
		 PyExc_IOError,
		 "%s: unable to check file signature.",
		 function );

		libcerror_error_free(
		 &error );

		goto on_error;
	}
	if( libbfio_handle_free(
	     &file_io_handle,
	     &error ) != 1 )
	{
		pyregf_error_raise(
		 error,
		 PyExc_MemoryError,
		 "%s: unable to free file IO handle.",
		 function );

		libcerror_error_free(
		 &error );

		goto on_error;
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

on_error:
	if( file_io_handle != NULL )
	{
		libbfio_handle_free(
		 &file_io_handle,
		 NULL );
	}
	return( NULL );
}

#if PY_MAJOR_VERSION >= 3

/* The pyregf module definition
 */
PyModuleDef pyregf_module_definition = {
	PyModuleDef_HEAD_INIT,

	/* m_name */
	"pyregf",
	/* m_doc */
	"Python libregf module (pyregf).",
	/* m_size */
	-1,
	/* m_methods */
	pyregf_module_methods,
	/* m_reload */
	NULL,
	/* m_traverse */
	NULL,
	/* m_clear */
	NULL,
	/* m_free */
	NULL,
};

#endif /* PY_MAJOR_VERSION >= 3 */

/* Initializes the pyregf module
 */
#if PY_MAJOR_VERSION >= 3
PyMODINIT_FUNC PyInit_pyregf(
                void )
#else
PyMODINIT_FUNC initpyregf(
                void )
#endif
{
	PyObject *module                      = NULL;
	PyTypeObject *file_type_object        = NULL;
	PyTypeObject *file_types_type_object  = NULL;
	PyTypeObject *key_type_object         = NULL;
	PyTypeObject *keys_type_object        = NULL;
	PyTypeObject *value_type_object       = NULL;
	PyTypeObject *value_types_type_object = NULL;
	PyTypeObject *values_type_object      = NULL;
	PyGILState_STATE gil_state            = 0;

#if defined( HAVE_DEBUG_OUTPUT )
	libregf_notify_set_stream(
	 stderr,
	 NULL );
	libregf_notify_set_verbose(
	 1 );
#endif

	/* Create the module
	 * This function must be called before grabbing the GIL
	 * otherwise the module will segfault on a version mismatch
	 */
#if PY_MAJOR_VERSION >= 3
	module = PyModule_Create(
	          &pyregf_module_definition );
#else
	module = Py_InitModule3(
	          "pyregf",
	          pyregf_module_methods,
	          "Python libregf module (pyregf)." );
#endif
	if( module == NULL )
	{
#if PY_MAJOR_VERSION >= 3
		return( NULL );
#else
		return;
#endif
	}
	PyEval_InitThreads();

	gil_state = PyGILState_Ensure();

	/* Setup the file type object
	 */
	pyregf_file_type_object.tp_new = PyType_GenericNew;

	if( PyType_Ready(
	     &pyregf_file_type_object ) < 0 )
	{
		goto on_error;
	}
	Py_IncRef(
	 (PyObject *) &pyregf_file_type_object );

	file_type_object = &pyregf_file_type_object;

	PyModule_AddObject(
	 module,
	 "file",
	 (PyObject *) file_type_object );

	/* Setup the keys type object
	 */
	pyregf_keys_type_object.tp_new = PyType_GenericNew;

	if( PyType_Ready(
	     &pyregf_keys_type_object ) < 0 )
	{
		goto on_error;
	}
	Py_IncRef(
	 (PyObject *) &pyregf_keys_type_object );

	keys_type_object = &pyregf_keys_type_object;

	PyModule_AddObject(
	 module,
	 "_keys",
	 (PyObject *) keys_type_object );

	/* Setup the key type object
	 */
	pyregf_key_type_object.tp_new = PyType_GenericNew;

	if( PyType_Ready(
	     &pyregf_key_type_object ) < 0 )
	{
		goto on_error;
	}
	Py_IncRef(
	 (PyObject *) &pyregf_key_type_object );

	key_type_object = &pyregf_key_type_object;

	PyModule_AddObject(
	 module,
	 "key",
	 (PyObject *) key_type_object );

	/* Setup the values type object
	 */
	pyregf_values_type_object.tp_new = PyType_GenericNew;

	if( PyType_Ready(
	     &pyregf_values_type_object ) < 0 )
	{
		goto on_error;
	}
	Py_IncRef(
	 (PyObject *) &pyregf_values_type_object );

	values_type_object = &pyregf_values_type_object;

	PyModule_AddObject(
	 module,
	 "_values",
	 (PyObject *) values_type_object );

	/* Setup the value type object
	 */
	pyregf_value_type_object.tp_new = PyType_GenericNew;

	if( PyType_Ready(
	     &pyregf_value_type_object ) < 0 )
	{
		goto on_error;
	}
	Py_IncRef(
	 (PyObject *) &pyregf_value_type_object );

	value_type_object = &pyregf_value_type_object;

	PyModule_AddObject(
	 module,
	 "value",
	 (PyObject *) value_type_object );

	/* Setup the file types type object
	 */
	pyregf_file_types_type_object.tp_new = PyType_GenericNew;

	if( pyregf_file_types_init_type(
	     &pyregf_file_types_type_object ) != 1 )
	{
		goto on_error;
	}
	if( PyType_Ready(
	     &pyregf_file_types_type_object ) < 0 )
	{
		goto on_error;
	}
	Py_IncRef(
	 (PyObject *) &pyregf_file_types_type_object );

	file_types_type_object = &pyregf_file_types_type_object;

	PyModule_AddObject(
	 module,
	 "file_types",
	 (PyObject *) file_types_type_object );

	/* Setup the value types type object
	 */
	pyregf_value_types_type_object.tp_new = PyType_GenericNew;

	if( pyregf_value_types_init_type(
	     &pyregf_value_types_type_object ) != 1 )
	{
		goto on_error;
	}
	if( PyType_Ready(
	     &pyregf_value_types_type_object ) < 0 )
	{
		goto on_error;
	}
	Py_IncRef(
	 (PyObject *) &pyregf_value_types_type_object );

	value_types_type_object = &pyregf_value_types_type_object;

	PyModule_AddObject(
	 module,
	 "value_types",
	 (PyObject *) value_types_type_object );

	PyGILState_Release(
	 gil_state );

#if PY_MAJOR_VERSION >= 3
	return( module );
#else
	return;
#endif

on_error:
	PyGILState_Release(
	 gil_state );

#if PY_MAJOR_VERSION >= 3
	return( NULL );
#else
	return;
#endif
}

