/*
 * Notification print functions
 *
 * Copyright (C) 2008-2016, Joachim Metz <joachim.metz@gmail.com>
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

#if !defined( _LIBCNOTIFY_PRINT_H )
#define _LIBCNOTIFY_PRINT_H

#include <common.h>
#include <types.h>

#include "libcnotify_extern.h"
#include "libcnotify_libcerror.h"

#if defined( __cplusplus )
extern "C" {
#endif

LIBCNOTIFY_EXTERN \
int libcnotify_printf(
     const char *format,
     ... );

LIBCNOTIFY_EXTERN \
int libcnotify_print_data(
     const uint8_t *data,
     size_t data_size,
     uint8_t print_data_flags );

LIBCNOTIFY_EXTERN \
int libcnotify_print_error_backtrace(
     libcerror_error_t *error );

#if defined( __cplusplus )
}
#endif

#endif

