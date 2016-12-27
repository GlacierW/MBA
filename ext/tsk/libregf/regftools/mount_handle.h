/*
 * Mount handle
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

#if !defined( _MOUNT_HANDLE_H )
#define _MOUNT_HANDLE_H

#include <common.h>
#include <file_stream.h>
#include <types.h>

#include "regftools_libcerror.h"
#include "regftools_libregf.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct mount_handle mount_handle_t;

struct mount_handle
{
	/* The libregf input file
	 */
	libregf_file_t *input_file;

	/* The ascii codepage
	 */
	int ascii_codepage;

	/* The notification output stream
	 */
	FILE *notify_stream;
};

int mount_handle_initialize(
     mount_handle_t **mount_handle,
     libcerror_error_t **error );

int mount_handle_free(
     mount_handle_t **mount_handle,
     libcerror_error_t **error );

int mount_handle_signal_abort(
     mount_handle_t *mount_handle,
     libcerror_error_t **error );

int mount_handle_set_ascii_codepage(
     mount_handle_t *mount_handle,
     const system_character_t *string,
     libcerror_error_t **error );

int mount_handle_open_input(
     mount_handle_t *mount_handle,
     const system_character_t *filename,
     libcerror_error_t **error );

int mount_handle_close(
     mount_handle_t *mount_handle,
     libcerror_error_t **error );

int mount_handle_get_key_by_path(
     mount_handle_t *mount_handle,
     const system_character_t *path,
     size_t path_length,
     system_character_t path_separator,
     libregf_key_t **key,
     libcerror_error_t **error );

int mount_handle_get_value_by_path(
     mount_handle_t *mount_handle,
     const system_character_t *path,
     size_t path_length,
     system_character_t path_separator,
     libregf_key_t **key,
     libregf_value_t **value,
     libcerror_error_t **error );

int mount_handle_get_filename(
     mount_handle_t *mount_handle,
     const system_character_t *sanitized_name,
     size_t sanitized_name_size,
     system_character_t path_separator,
     system_character_t **name,
     size_t *name_size,
     size_t *last_path_seperator_index,
     libcerror_error_t **error );

int mount_handle_get_sanitized_filename(
     mount_handle_t *mount_handle,
     const system_character_t *name,
     size_t name_size,
     system_character_t path_separator,
     system_character_t **sanitized_name,
     size_t *sanitized_name_size,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

