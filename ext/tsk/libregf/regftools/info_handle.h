/*
 * Info handle
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

#if !defined( _INFO_HANDLE_H )
#define _INFO_HANDLE_H

#include <common.h>
#include <file_stream.h>
#include <types.h>

#include "regftools_libcerror.h"
#include "regftools_libregf.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct info_handle info_handle_t;
typedef struct key_offset key_offset_list;
/*
 * MBA - Adding some definition of string array for map memory registry path
 */
#define StringLength 1000
typedef char Str[StringLength];
typedef Str StrArray[StringLength];

struct info_handle
{
	/* The libregf input file
	 */
	libregf_file_t *input_file;

	/* The notification output stream
	 */
	FILE *notify_stream;

	/* The ascii codepage
	 */
	int ascii_codepage;

	/* Value to indicate if abort was signalled
	 */
	int abort;
};

struct key_offset
{
    uint64_t offset;
    Str key;
    uint64_t name_size;
    uint64_t data_size;
};

void quicksort_key_list(int left, int right);

void swap_key_list(key_offset_list* a, key_offset_list* b);

int info_handle_initialize(
     info_handle_t **info_handle,
     libcerror_error_t **error );

int info_handle_free(
     info_handle_t **info_handle,
     libcerror_error_t **error );

int info_handle_signal_abort(
     info_handle_t *info_handle,
     libcerror_error_t **error );

int info_handle_set_ascii_codepage(
     info_handle_t *info_handle,
     const system_character_t *string,
     libcerror_error_t **error );

int info_handle_open_input(
     info_handle_t *info_handle,
     const system_character_t *filename,
     libcerror_error_t **error );

int info_handle_close_input(
     info_handle_t *info_handle,
     libcerror_error_t **error );

int info_handle_key_fprint(
     info_handle_t *info_handle,
     libregf_key_t *key,
     int indentation_level,
     libregf_error_t **error );

int info_handle_file_fprint(
     info_handle_t *info_handle,
     libcerror_error_t **error );


int info_handle_key_fprint_search_key(
		info_handle_t *info_handle,
		libregf_key_t *key,
		int indentation_level,
		system_character_t *keyName,
        system_character_t *findKeyPath,
		int hasKey,
		libregf_error_t **error );

int info_handle_file_fprint_search_key(
		info_handle_t *info_handle,
		system_character_t *keyName,
        system_character_t *findKeyPath,
		libcerror_error_t **error );

int info_handle_key_fprint_registry_path(
		info_handle_t *info_handle,
		libregf_key_t *key,
		int indentation_level,
		StrArray keyPath,
		int keyPathLen,
		libregf_error_t **error);

int info_handle_file_fprint_registry_path(
		info_handle_t *info_handle,
		StrArray keyPath,
		int keyPathLen,
		libcerror_error_t **error );
int info_handle_file_print_by_address(
                info_handle_t *info_handle,
                libcerror_error_t **error );
#if defined( __cplusplus )
}
#endif

#endif

