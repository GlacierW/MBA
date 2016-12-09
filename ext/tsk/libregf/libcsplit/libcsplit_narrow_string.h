/*
 * Narrow character string functions
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

#if !defined( _LIBCSPLIT_NARROW_STRING_H )
#define _LIBCSPLIT_NARROW_STRING_H

#include <common.h>
#include <types.h>

#include "libcsplit_extern.h"
#include "libcsplit_libcerror.h"
#include "libcsplit_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

LIBCSPLIT_EXTERN \
int libcsplit_narrow_string_split(
     const char *string,
     size_t string_size,
     char delimiter,
     libcsplit_narrow_split_string_t **split_string,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBCSPLIT_NARROW_STRING_H ) */

