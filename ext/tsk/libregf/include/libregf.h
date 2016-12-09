/*
 * Library to access the Windows NT Registry File (REGF) format
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

#if !defined( _LIBREGF_H )
#define _LIBREGF_H

#include <libregf/codepage.h>
#include <libregf/definitions.h>
#include <libregf/error.h>
#include <libregf/extern.h>
#include <libregf/features.h>
#include <libregf/types.h>

#include <stdio.h>

#if defined( LIBREGF_HAVE_BFIO )
#include <libbfio.h>
#endif

#if defined( __cplusplus )
extern "C" {
#endif

/* -------------------------------------------------------------------------
 * Support functions
 * ------------------------------------------------------------------------- */

/* Returns the library version
 */
LIBREGF_EXTERN \
const char *libregf_get_version(
             void );

/* Returns the access flags for reading
 */
LIBREGF_EXTERN \
int libregf_get_access_flags_read(
     void );

/* Retrieves the narrow system string codepage
 * A value of 0 represents no codepage, UTF-8 encoding is used instead
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_get_codepage(
     int *codepage,
     libregf_error_t **error );

/* Sets the narrow system string codepage
 * A value of 0 represents no codepage, UTF-8 encoding is used instead
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_set_codepage(
     int codepage,
     libregf_error_t **error );

/* Determines if a file contains a REGF file signature
 * Returns 1 if true, 0 if not or -1 on error
 */
LIBREGF_EXTERN \
int libregf_check_file_signature(
     const char *filename,
     libregf_error_t **error );

#if defined( LIBREGF_HAVE_WIDE_CHARACTER_TYPE )

/* Determines if a file contains a REGF file signature
 * Returns 1 if true, 0 if not or -1 on error
 */
LIBREGF_EXTERN \
int libregf_check_file_signature_wide(
     const wchar_t *filename,
     libregf_error_t **error );

#endif /* defined( LIBREGF_HAVE_WIDE_CHARACTER_TYPE ) */

#if defined( LIBREGF_HAVE_BFIO )

/* Determines if a file contains a REGF file signature using a Basic File IO (bfio) handle
 * Returns 1 if true, 0 if not or -1 on error
 */
LIBREGF_EXTERN \
int libregf_check_file_signature_file_io_handle(
     libbfio_handle_t *bfio_handle,
     libregf_error_t **error );

#endif /* defined( LIBREGF_HAVE_BFIO ) */

/* -------------------------------------------------------------------------
 * Notify functions
 * ------------------------------------------------------------------------- */

/* Sets the verbose notification
 */
LIBREGF_EXTERN \
void libregf_notify_set_verbose(
      int verbose );

/* Sets the notification stream
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_notify_set_stream(
     FILE *stream,
     libregf_error_t **error );

/* Opens the notification stream using a filename
 * The stream is opened in append mode
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_notify_stream_open(
     const char *filename,
     libregf_error_t **error );

/* Closes the notification stream if opened using a filename
 * Returns 0 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_notify_stream_close(
     libregf_error_t **error );

/* -------------------------------------------------------------------------
 * Error functions
 * ------------------------------------------------------------------------- */

/* Frees an error
 */
LIBREGF_EXTERN \
void libregf_error_free(
      libregf_error_t **error );

/* Prints a descriptive string of the error to the stream
 * Returns the number of printed characters if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_error_fprint(
     libregf_error_t *error,
     FILE *stream );

/* Prints a descriptive string of the error to the string
 * Returns the number of printed characters if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_error_sprint(
     libregf_error_t *error,
     char *string,
     size_t size );

/* Prints a backtrace of the error to the stream
 * Returns the number of printed characters if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_error_backtrace_fprint(
     libregf_error_t *error,
     FILE *stream );

/* Prints a backtrace of the error to the string
 * Returns the number of printed characters if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_error_backtrace_sprint(
     libregf_error_t *error,
     char *string,
     size_t size );

/* -------------------------------------------------------------------------
 * File functions
 * ------------------------------------------------------------------------- */

/* Creates a file
 * Make sure the value file is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_file_initialize(
     libregf_file_t **file,
     libregf_error_t **error );

/* Frees a file
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_file_free(
     libregf_file_t **file,
     libregf_error_t **error );

/* Signals the file to abort its current activity
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_file_signal_abort(
     libregf_file_t *file,
     libregf_error_t **error );

/* Opens a file
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_file_open(
     libregf_file_t *file,
     const char *filename,
     int access_flags,
     libregf_error_t **error );

#if defined( LIBREGF_HAVE_WIDE_CHARACTER_TYPE )

/* Opens a file
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_file_open_wide(
     libregf_file_t *file,
     const wchar_t *filename,
     int access_flags,
     libregf_error_t **error );

#endif /* defined( LIBREGF_HAVE_WIDE_CHARACTER_TYPE ) */

#if defined( LIBREGF_HAVE_BFIO )

/* Opens a file using a Basic File IO (bfio) handle
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_file_open_file_io_handle(
     libregf_file_t *file,
     libbfio_handle_t *file_io_handle,
     int access_flags,
     libregf_error_t **error );

#endif /* defined( LIBREGF_HAVE_BFIO ) */

/* Closes a file
 * Returns 0 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_file_close(
     libregf_file_t *file,
     libregf_error_t **error );

/* Determine if the file corrupted
 * Returns 1 if corrupted, 0 if not or -1 on error
 */
LIBREGF_EXTERN \
int libregf_file_is_corrupted(
     libregf_file_t *file,
     libregf_error_t **error );

/* Retrieves the file ASCII codepage
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_file_get_ascii_codepage(
     libregf_file_t *file,
     int *ascii_codepage,
     libregf_error_t **error );

/* Sets the file ASCII codepage
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_file_set_ascii_codepage(
     libregf_file_t *file,
     int ascii_codepage,
     libregf_error_t **error );

/* Retrieves the format version
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_file_get_format_version(
     libregf_file_t *file,
     uint32_t *major_version,
     uint32_t *minor_version,
     libregf_error_t **error );

/* Retrieves the file type
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_file_get_type(
     libregf_file_t *file,
     uint32_t *file_type,
     libregf_error_t **error );

/* Retrieves the root key
 * Returns 1 if successful, 0 if no such key or -1 on error
 */
LIBREGF_EXTERN \
int libregf_file_get_root_key(
     libregf_file_t *file,
     libregf_key_t **key,
     libregf_error_t **error );

/* Retrieves the key for the specific UTF-8 encoded path
 * The path separator is the \ character
 * Returns 1 if successful, 0 if no such key or -1 on error
 */
LIBREGF_EXTERN \
int libregf_file_get_key_by_utf8_path(
     libregf_file_t *file,
     const uint8_t *utf8_string,
     size_t utf8_string_length,
     libregf_key_t **key,
     libregf_error_t **error );

/* Retrieves the key for the specific UTF-16 encoded path
 * The path separator is the \ character
 * Returns 1 if successful, 0 if no such key or -1 on error
 */
LIBREGF_EXTERN \
int libregf_file_get_key_by_utf16_path(
     libregf_file_t *file,
     const uint16_t *utf16_string,
     size_t utf16_string_length,
     libregf_key_t **key,
     libregf_error_t **error );

/* -------------------------------------------------------------------------
 * Key functions
 * ------------------------------------------------------------------------- */

/* Frees a key
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_key_free(
     libregf_key_t **key,
     libregf_error_t **error );

/* Determine if the key corrupted
 * Returns 1 if corrupted, 0 if not or -1 on error
 */
LIBREGF_EXTERN \
int libregf_key_is_corrupted(
     libregf_key_t *key,
     libregf_error_t **error );

/* Retrieves the offset of the key
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_key_get_offset(
     libregf_key_t *key,
     off64_t *offset,
     libregf_error_t **error );

/* Retrieves the key name size
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_key_get_name_size(
     libregf_key_t *key,
     size_t *name_size,
     libregf_error_t **error );

/* Retrieves the key name
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_key_get_name(
     libregf_key_t *key,
     uint8_t *name,
     size_t name_size,
     libregf_error_t **error );

/* Retrieves the UTF-8 string size of the key name
 * The returned size includes the end of string character
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_key_get_utf8_name_size(
     libregf_key_t *key,
     size_t *utf8_name_size,
     libregf_error_t **error );

/* Retrieves the UTF-8 string value of the key name
 * The function uses a codepage if necessary, it uses the codepage set for the library
 * The size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_key_get_utf8_name(
     libregf_key_t *key,
     uint8_t *utf8_name,
     size_t utf8_name_size,
     libregf_error_t **error );

/* Retrieves the UTF-16 string size of the key name
 * The returned size includes the end of string character
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_key_get_utf16_name_size(
     libregf_key_t *key,
     size_t *utf16_name_size,
     libregf_error_t **error );

/* Retrieves the UTF-16 string value of the key name
 * The function uses a codepage if necessary, it uses the codepage set for the library
 * The size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_key_get_utf16_name(
     libregf_key_t *key,
     uint16_t *utf16_name,
     size_t utf16_name_size,
     libregf_error_t **error );

/* Retrieves the class name size
 * Returns 1 if successful, 0 if no such value or -1 on error
 */
LIBREGF_EXTERN \
int libregf_key_get_class_name_size(
     libregf_key_t *key,
     size_t *class_name_size,
     libregf_error_t **error );

/* Retrieves the class name
 * Returns 1 if successful, 0 if no such value or -1 on error
 */
LIBREGF_EXTERN \
int libregf_key_get_class_name(
     libregf_key_t *key,
     uint8_t *class_name,
     size_t class_name_size,
     libregf_error_t **error );

/* Retrieves the UTF-8 string size of the class name
 * The returned size includes the end of string character
 * Returns 1 if successful, 0 if no such value or -1 on error
 */
LIBREGF_EXTERN \
int libregf_key_get_utf8_class_name_size(
     libregf_key_t *key,
     size_t *utf8_class_name_size,
     libregf_error_t **error );

/* Retrieves the UTF-8 string value of the class name
 * The function uses a codepage if necessary, it uses the codepage set for the library
 * The size should include the end of string character
 * Returns 1 if successful, 0 if no such value or -1 on error
 */
LIBREGF_EXTERN \
int libregf_key_get_utf8_class_name(
     libregf_key_t *key,
     uint8_t *utf8_class_name,
     size_t utf8_class_name_size,
     libregf_error_t **error );

/* Retrieves the UTF-16 string size of the class name
 * The returned size includes the end of string character
 * Returns 1 if successful, 0 if no such value or -1 on error
 */
LIBREGF_EXTERN \
int libregf_key_get_utf16_class_name_size(
     libregf_key_t *key,
     size_t *utf16_class_name_size,
     libregf_error_t **error );

/* Retrieves the UTF-16 string value of the class name
 * The function uses a codepage if necessary, it uses the codepage set for the library
 * The size should include the end of string character
 * Returns 1 if successful, 0 if no such value or -1 on error
 */
LIBREGF_EXTERN \
int libregf_key_get_utf16_class_name(
     libregf_key_t *key,
     uint16_t *utf16_class_name,
     size_t utf16_class_name_size,
     libregf_error_t **error );

/* Retrieves the last written date and time
 * The returned time is a 64-bit version of a filetime value
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_key_get_last_written_time(
     libregf_key_t *key,
     uint64_t *last_written_time,
     libregf_error_t **error );

/* Retrieves the security descriptor size
 * Returns 1 if successful, 0 if no such value or -1 on error
 */
LIBREGF_EXTERN \
int libregf_key_get_security_descriptor_size(
     libregf_key_t *key,
     size_t *security_descriptor_size,
     libregf_error_t **error );

/* Retrieves the security descriptor
 * Returns 1 if successful, 0 if no such value or -1 on error
 */
LIBREGF_EXTERN \
int libregf_key_get_security_descriptor(
     libregf_key_t *key,
     uint8_t *security_descriptor,
     size_t security_descriptor_size,
     libregf_error_t **error );

/* Retrieves the number of values
 * All sets in a key contain the same number of values
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_key_get_number_of_values(
     libregf_key_t *key,
     int *number_of_values,
     libregf_error_t **error );

/* Retrieves the value
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_key_get_value(
     libregf_key_t *key,
     int value_index,
     libregf_value_t **value,
     libregf_error_t **error );

/* Retrieves the value for the specific UTF-8 encoded name
 * To retrieve the default value specify value name as NULL
 * and its length as 0
 * Returns 1 if successful, 0 if no such value or -1 on error
 */
LIBREGF_EXTERN \
int libregf_key_get_value_by_utf8_name(
     libregf_key_t *key,
     const uint8_t *utf8_string,
     size_t utf8_string_length,
     libregf_value_t **value,
     libregf_error_t **error );

/* Retrieves the value for the specific UTF-16 encoded name
 * To retrieve the default value specify value name as NULL
 * and its length as 0
 * Returns 1 if successful, 0 if no such value or -1 on error
 */
LIBREGF_EXTERN \
int libregf_key_get_value_by_utf16_name(
     libregf_key_t *key,
     const uint16_t *utf16_string,
     size_t utf16_string_length,
     libregf_value_t **value,
     libregf_error_t **error );

/* Retrieves the number of sub keys
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_key_get_number_of_sub_keys(
     libregf_key_t *key,
     int *number_of_sub_keys,
     libregf_error_t **error );

/* Retrieves the sub key for the specific index
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_key_get_sub_key(
     libregf_key_t *key,
     int sub_key_index,
     libregf_key_t **sub_key,
     libregf_error_t **error );

/* Retrieves the sub key for the specific UTF-8 encoded name
 * Returns 1 if successful, 0 if no such sub key or -1 on error
 */
LIBREGF_EXTERN \
int libregf_key_get_sub_key_by_utf8_name(
     libregf_key_t *key,
     const uint8_t *utf8_string,
     size_t utf8_string_length,
     libregf_key_t **sub_key,
     libregf_error_t **error );

/* Retrieves the sub key for the specific UTF-8 encoded path
 * The path separator is the \ character
 * Returns 1 if successful, 0 if no such sub key or -1 on error
 */
LIBREGF_EXTERN \
int libregf_key_get_sub_key_by_utf8_path(
     libregf_key_t *key,
     const uint8_t *utf8_string,
     size_t utf8_string_length,
     libregf_key_t **sub_key,
     libregf_error_t **error );

/* Retrieves the sub key for the specific UTF-16 encoded name
 * Returns 1 if successful, 0 if no such sub key or -1 on error
 */
LIBREGF_EXTERN \
int libregf_key_get_sub_key_by_utf16_name(
     libregf_key_t *key,
     const uint16_t *utf16_string,
     size_t utf16_string_length,
     libregf_key_t **sub_key,
     libregf_error_t **error );

/* Retrieves the sub key for the specific UTF-16 encoded path
 * The path separator is the \ character
 * Returns 1 if successful, 0 if no such sub key or -1 on error
 */
LIBREGF_EXTERN \
int libregf_key_get_sub_key_by_utf16_path(
     libregf_key_t *key,
     const uint16_t *utf16_string,
     size_t utf16_string_length,
     libregf_key_t **sub_key,
     libregf_error_t **error );

/* -------------------------------------------------------------------------
 * Value functions
 * ------------------------------------------------------------------------- */

/* Frees a value
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_value_free(
     libregf_value_t **value,
     libregf_error_t **error );

/* Determine if the value corrupted
 * Returns 1 if corrupted, 0 if not or -1 on error
 */
LIBREGF_EXTERN \
int libregf_value_is_corrupted(
     libregf_value_t *value,
     libregf_error_t **error );

/* Retrieves the offset of the value
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_value_get_offset(
     libregf_value_t *value,
     off64_t *offset,
     libregf_error_t **error );

/* Retrieves the value name size
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_value_get_name_size(
     libregf_value_t *value,
     size_t *name_size,
     libregf_error_t **error );

/* Retrieves the value name
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_value_get_name(
     libregf_value_t *value,
     uint8_t *name,
     size_t name_size,
     libregf_error_t **error );

/* Retrieves the UTF-8 string size of the value name
 * The returned size includes the end of string character
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_value_get_utf8_name_size(
     libregf_value_t *value,
     size_t *utf8_name_size,
     libregf_error_t **error );

/* Retrieves the UTF-8 string value of the value name
 * The function uses a codepage if necessary, it uses the codepage set for the library
 * The size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_value_get_utf8_name(
     libregf_value_t *value,
     uint8_t *utf8_name,
     size_t utf8_name_size,
     libregf_error_t **error );

/* Retrieves the UTF-16 string size of the value name
 * The returned size includes the end of string character
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_value_get_utf16_name_size(
     libregf_value_t *value,
     size_t *utf16_name_size,
     libregf_error_t **error );

/* Retrieves the UTF-16 string value of the value name
 * The function uses a codepage if necessary, it uses the codepage set for the library
 * The size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_value_get_utf16_name(
     libregf_value_t *value,
     uint16_t *utf16_name,
     size_t utf16_name_size,
     libregf_error_t **error );

/* Retrieves the value type
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_value_get_value_type(
     libregf_value_t *value,
     uint32_t *value_type,
     libregf_error_t **error );

/* Retrieves the value data size
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_value_get_value_data_size(
     libregf_value_t *value,
     size_t *value_data_size,
     libregf_error_t **error );

/* Retrieves the value data
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_value_get_value_data(
     libregf_value_t *value,
     uint8_t *value_data,
     size_t value_data_size,
     libregf_error_t **error );

/* Retrieves the 32-bit value
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_value_get_value_32bit(
     libregf_value_t *value,
     uint32_t *value_32bit,
     libregf_error_t **error );

/* Retrieves the 64-bit value
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_value_get_value_64bit(
     libregf_value_t *value,
     uint64_t *value_64bit,
     libregf_error_t **error );

/* Retrieves the UTF-8 string size
 * The returned size includes the end of string character
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_value_get_value_utf8_string_size(
     libregf_value_t *value,
     size_t *utf8_string_size,
     libregf_error_t **error );

/* Retrieves the UTF-8 string value
 * The function uses a codepage if necessary, it uses the codepage set for the library
 * The size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_value_get_value_utf8_string(
     libregf_value_t *value,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     libregf_error_t **error );

/* Retrieves the UTF-16 string size
 * The returned size includes the end of string character
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_value_get_value_utf16_string_size(
     libregf_value_t *value,
     size_t *utf16_string_size,
     libregf_error_t **error );

/* Retrieves the UTF-16 string value
 * The function uses a codepage if necessary, it uses the codepage set for the library
 * The size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_value_get_value_utf16_string(
     libregf_value_t *value,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     libregf_error_t **error );

/* Retrieves the size of a binary data value
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_value_get_value_binary_data_size(
     libregf_value_t *value,
     size_t *size,
     libregf_error_t **error );

/* Retrieves the binary data value
 * Returns 1 if successful or -1 on error
 */
LIBREGF_EXTERN \
int libregf_value_get_value_binary_data(
     libregf_value_t *value,
     uint8_t *binary_data,
     size_t size,
     libregf_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

