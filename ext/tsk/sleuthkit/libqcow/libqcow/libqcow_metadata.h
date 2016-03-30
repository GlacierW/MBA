/*
 * Metadata functions
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

#if !defined( _LIBQCOW_METADATA_H )
#define _LIBQCOW_METADATA_H

#include <common.h>
#include <types.h>

#include "libqcow_extern.h"
#include "libqcow_libcerror.h"
#include "libqcow_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

LIBQCOW_EXTERN \
int libqcow_file_get_media_size(
     libqcow_file_t *file,
     size64_t *media_size,
     libcerror_error_t **error );

LIBQCOW_EXTERN \
int libqcow_file_get_format_version(
     libqcow_file_t *file,
     uint32_t *format_version,
     libcerror_error_t **error );

LIBQCOW_EXTERN \
int libqcow_file_get_encryption_method(
     libqcow_file_t *file,
     uint32_t *encryption_method,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

