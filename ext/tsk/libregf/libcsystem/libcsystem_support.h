/*
 * Support functions
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

#if !defined( _LIBCSYSTEM_SUPPORT_H )
#define _LIBCSYSTEM_SUPPORT_H

#include <common.h>
#include <types.h>

#include "libcsystem_extern.h"
#include "libcsystem_libcerror.h"

#if defined( __cplusplus )
extern "C" {
#endif

#if !defined( HAVE_LOCAL_LIBCSYSTEM )

LIBCSYSTEM_EXTERN \
const char *libcsystem_get_version(
             void );

#endif /* !defined( HAVE_LOCAL_LIBCSYSTEM ) */

LIBCSYSTEM_EXTERN \
int libcsystem_initialize(
     int stdio_mode,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBCSYSTEM_SUPPORT_H ) */

