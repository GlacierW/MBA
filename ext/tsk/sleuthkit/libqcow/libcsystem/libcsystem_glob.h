/*
 * Globbing functions
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

#if !defined( _LIBCSYSTEM_GLOB_H )
#define _LIBCSYSTEM_GLOB_H

#include <common.h>
#include <types.h>

#include "libcsystem_definitions.h"
#include "libcsystem_extern.h"
#include "libcsystem_libcerror.h"
#include "libcsystem_types.h"

#if defined( __cplusplus )
extern "C" {
#endif

#if !defined( HAVE_GLOB_H )

typedef struct libcsystem_internal_glob libcsystem_internal_glob_t;

struct libcsystem_internal_glob
{
	/* The number of globs resolved
	 */
	int number_of_results;

	/* The resolved globs
	 */
	system_character_t **results;
};

LIBCSYSTEM_EXTERN \
int libcsystem_glob_initialize(
     libcsystem_glob_t **glob,
     libcerror_error_t **error );

LIBCSYSTEM_EXTERN \
int libcsystem_glob_free(
     libcsystem_glob_t **glob,
     libcerror_error_t **error );

int libcsystem_glob_resize(
     libcsystem_glob_t *glob,
     int new_number_of_results,
     libcerror_error_t **error );

LIBCSYSTEM_EXTERN \
int libcsystem_glob_resolve(
     libcsystem_glob_t *glob,
     system_character_t * const patterns[],
     int number_of_patterns,
     libcerror_error_t **error );

LIBCSYSTEM_EXTERN \
int libcsystem_glob_get_results(
     libcsystem_glob_t *glob,
     int *number_of_results,
     system_character_t ***results,
     libcerror_error_t **error );

#endif /* !defined( HAVE_GLOB_H ) */

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBCSYSTEM_GLOB_H ) */

