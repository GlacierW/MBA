/*
 * Date and time functions
 *
 * Copyright (c) 2009-2019, Joachim Metz <joachim.metz@gmail.com>
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

#if !defined( _PYREGF_DATETIME_H )
#define _PYREGF_DATETIME_H

#include <common.h>
#include <types.h>

#include "pyregf_python.h"

#if defined( __cplusplus )
extern "C" {
#endif

PyObject *pyregf_datetime_new_from_fat_date_time(
           uint32_t fat_date_time );

PyObject *pyregf_datetime_new_from_filetime(
           uint64_t filetime );

PyObject *pyregf_datetime_new_from_posix_time(
           uint32_t posix_time );

#if defined( __cplusplus )
}
#endif

#endif

