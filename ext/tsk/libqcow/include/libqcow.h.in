/*
 * Library to access the QEMU Copy-On-Write (QCOW) image format
 *
 * Copyright (C) 2010-2016, Joachim Metz <joachim.metz@gmail.com>
 *
 * Refer to AUTHORS for acknowledgements.
 *
 * This software is free software: you can redistribute it and/or modisoy
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

#if !defined( _LIBQCOW_H )
#define _LIBQCOW_H

#include <libqcow/codepage.h>
#include <libqcow/definitions.h>
#include <libqcow/error.h>
#include <libqcow/extern.h>
#include <libqcow/features.h>
#include <libqcow/types.h>

#include <stdio.h>

#if defined( LIBQCOW_HAVE_BFIO )
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
LIBQCOW_EXTERN \
const char *libqcow_get_version(
             void );

/* Returns the access flags for reading
 */
LIBQCOW_EXTERN \
int libqcow_get_access_flags_read(
     void );

/* Returns the access flags for reading and writing
 */
LIBQCOW_EXTERN \
int libqcow_get_access_flags_read_write(
     void );

/* Returns the access flags for writing
 */
LIBQCOW_EXTERN \
int libqcow_get_access_flags_write(
     void );

/* Retrieves the narrow system string codepage
 * A value of 0 represents no codepage, UTF-8 encoding is used instead
 * Returns 1 if successful or -1 on error
 */
LIBQCOW_EXTERN \
int libqcow_get_codepage(
     int *codepage,
     libqcow_error_t **error );

/* Sets the narrow system string codepage
 * A value of 0 represents no codepage, UTF-8 encoding is used instead
 * Returns 1 if successful or -1 on error
 */
LIBQCOW_EXTERN \
int libqcow_set_codepage(
     int codepage,
     libqcow_error_t **error );

/* Determines if a file contains a QCOW file signature
 * Returns 1 if true, 0 if not or -1 on error
 */
LIBQCOW_EXTERN \
int libqcow_check_file_signature(
     const char *filename,
     libqcow_error_t **error );

#if defined( LIBQCOW_HAVE_WIDE_CHARACTER_TYPE )

/* Determines if a file contains a QCOW file signature
 * Returns 1 if true, 0 if not or -1 on error
 */
LIBQCOW_EXTERN \
int libqcow_check_file_signature_wide(
     const wchar_t *filename,
     libqcow_error_t **error );

#endif /* defined( LIBQCOW_HAVE_WIDE_CHARACTER_TYPE ) */

#if defined( LIBQCOW_HAVE_BFIO )

/* Determines if a file contains a QCOW file signature using a Basic File IO (bfio) handle
 * Returns 1 if true, 0 if not or -1 on error
 */
LIBQCOW_EXTERN \
int libqcow_check_file_signature_file_io_handle(
     libbfio_handle_t *bfio_handle,
     libqcow_error_t **error );

#endif /* defined( LIBQCOW_HAVE_BFIO ) */

/* -------------------------------------------------------------------------
 * Notify functions
 * ------------------------------------------------------------------------- */

/* Sets the verbose notification
 */
LIBQCOW_EXTERN \
void libqcow_notify_set_verbose(
      int verbose );

/* Sets the notification stream
 * Returns 1 if successful or -1 on error
 */
LIBQCOW_EXTERN \
int libqcow_notify_set_stream(
     FILE *stream,
     libqcow_error_t **error );

/* Opens the notification stream using a filename
 * The stream is opened in append mode
 * Returns 1 if successful or -1 on error
 */
LIBQCOW_EXTERN \
int libqcow_notify_stream_open(
     const char *filename,
     libqcow_error_t **error );

/* Closes the notification stream if opened using a filename
 * Returns 0 if successful or -1 on error
 */
LIBQCOW_EXTERN \
int libqcow_notify_stream_close(
     libqcow_error_t **error );

/* -------------------------------------------------------------------------
 * Error functions
 * ------------------------------------------------------------------------- */

/* Frees an error
 */
LIBQCOW_EXTERN \
void libqcow_error_free(
      libqcow_error_t **error );

/* Prints a descriptive string of the error to the stream
 * Returns the number of printed characters if successful or -1 on error
 */
LIBQCOW_EXTERN \
int libqcow_error_fprint(
     libqcow_error_t *error,
     FILE *stream );

/* Prints a descriptive string of the error to the string
 * The end-of-string character is not included in the return value
 * Returns the number of printed characters if successful or -1 on error
 */
LIBQCOW_EXTERN \
int libqcow_error_sprint(
     libqcow_error_t *error,
     char *string,
     size_t size );

/* Prints a backtrace of the error to the stream
 * Returns the number of printed characters if successful or -1 on error
 */
LIBQCOW_EXTERN \
int libqcow_error_backtrace_fprint(
     libqcow_error_t *error,
     FILE *stream );

/* Prints a backtrace of the error to the string
 * The end-of-string character is not included in the return value
 * Returns the number of printed characters if successful or -1 on error
 */
LIBQCOW_EXTERN \
int libqcow_error_backtrace_sprint(
     libqcow_error_t *error,
     char *string,
     size_t size );

/* -------------------------------------------------------------------------
 * File functions
 * ------------------------------------------------------------------------- */

/* Creates a file
 * Make sure the value file is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
LIBQCOW_EXTERN \
int libqcow_file_initialize(
     libqcow_file_t **file,
     libqcow_error_t **error );

/* Frees a file
 * Returns 1 if successful or -1 on error
 */
LIBQCOW_EXTERN \
int libqcow_file_free(
     libqcow_file_t **file,
     libqcow_error_t **error );

/* Signals a file to abort its current activity
 * Returns 1 if successful or -1 on error
 */
LIBQCOW_EXTERN \
int libqcow_file_signal_abort(
     libqcow_file_t *file,
     libqcow_error_t **error );

/* Opens a file
 * Returns 1 if successful or -1 on error
 */
LIBQCOW_EXTERN \
int libqcow_file_open(
     libqcow_file_t *file,
     const char *filename,
     int access_flags,
     libqcow_error_t **error );

#if defined( LIBQCOW_HAVE_WIDE_CHARACTER_TYPE )

/* Opens a file
 * Returns 1 if successful or -1 on error
 */
LIBQCOW_EXTERN \
int libqcow_file_open_wide(
     libqcow_file_t *file,
     const wchar_t *filename,
     int access_flags,
     libqcow_error_t **error );

#endif /* defined( LIBQCOW_HAVE_WIDE_CHARACTER_TYPE ) */

#if defined( LIBQCOW_HAVE_BFIO )

/* Opens a file using a Basic File IO (bfio) handle
 * Returns 1 if successful or -1 on error
 */
LIBQCOW_EXTERN \
int libqcow_file_open_file_io_handle(
     libqcow_file_t *file,
     libbfio_handle_t *file_io_handle,
     int access_flags,
     libqcow_error_t **error );

#endif /* defined( LIBQCOW_HAVE_BFIO ) */

/* Closes a file
 * Returns 0 if successful or -1 on error
 */
LIBQCOW_EXTERN \
int libqcow_file_close(
     libqcow_file_t *file,
     libqcow_error_t **error );

/* Reads (media) data at the current offset
 * Returns the number of bytes read or -1 on error
 */
LIBQCOW_EXTERN \
ssize_t libqcow_file_read_buffer(
         libqcow_file_t *file,
         void *buffer,
         size_t buffer_size,
         libqcow_error_t **error );

/* Reads (media) data at a specific offset
 * Returns the number of bytes read or -1 on error
 */
LIBQCOW_EXTERN \
ssize_t libqcow_file_read_buffer_at_offset(
         libqcow_file_t *file,
         void *buffer,
         size_t buffer_size,
         off64_t offset,
         libqcow_error_t **error );

#ifdef TODO_WRITE_SUPPORT

/* Writes (media) data at the current offset
 * the necessary settings of the write values must have been made
 * Will initialize write if necessary
 * Returns the number of input bytes written, 0 when no longer bytes can be written or -1 on error
 */
LIBQCOW_EXTERN \
ssize_t libqcow_file_write_buffer(
         libqcow_file_t *file,
         const void *buffer,
         size_t buffer_size,
         libqcow_error_t **error );

/* Writes (media) data at a specific offset
 * the necessary settings of the write values must have been made
 * Will initialize write if necessary
 * Returns the number of input bytes written, 0 when no longer bytes can be written or -1 on error
 */
LIBQCOW_EXTERN \
ssize_t libqcow_file_write_buffer_at_offset(
         libqcow_file_t *file,
         const void *buffer,
         size_t buffer_size,
         off64_t offset,
         libqcow_error_t **error );

#endif /* TODO_WRITE_SUPPORT */

/* Seeks a certain offset of the (media) data
 * Returns the offset if seek is successful or -1 on error
 */
LIBQCOW_EXTERN \
off64_t libqcow_file_seek_offset(
         libqcow_file_t *file,
         off64_t offset,
         int whence,
         libqcow_error_t **error );

/* Retrieves the current offset of the (media) data
 * Returns the offset if successful or -1 on error
 */
LIBQCOW_EXTERN \
int libqcow_file_get_offset(
     libqcow_file_t *file,
     off64_t *offset,
     libqcow_error_t **error );

/* Retrieves the format version
 * Returns 1 if successful or -1 on error
 */
LIBQCOW_EXTERN \
int libqcow_file_get_format_version(
     libqcow_file_t *file,
     uint32_t *format_version,
     libqcow_error_t **error );

/* Retrieves the encryption method
 * Returns 1 if successful or -1 on error
 */
LIBQCOW_EXTERN \
int libqcow_file_get_encryption_method(
     libqcow_file_t *file,
     uint32_t *encryption_method,
     libqcow_error_t **error );

/* Sets the keys
 * This function needs to be used before one of the open functions
 * Returns 1 if successful or -1 on error
 */
LIBQCOW_EXTERN \
int libqcow_file_set_keys(
     libqcow_file_t *file,
     const uint8_t *key,
     size_t key_size,
     libqcow_error_t **error );

/* Sets an UTF-8 formatted password
 * This function needs to be used before one of the open functions
 * Returns 1 if successful, 0 if password is invalid or -1 on error
 */
LIBQCOW_EXTERN \
int libqcow_file_set_utf8_password(
     libqcow_file_t *file,
     const uint8_t *utf8_string,
     size_t utf8_string_length,
     libqcow_error_t **error );

/* Sets an UTF-16 formatted password
 * This function needs to be used before one of the open functions
 * Returns 1 if successful, 0 if password is invalid or -1 on error
 */
LIBQCOW_EXTERN \
int libqcow_file_set_utf16_password(
     libqcow_file_t *file,
     const uint16_t *utf16_string,
     size_t utf16_string_length,
     libqcow_error_t **error );

/* -------------------------------------------------------------------------
 * Meta data functions
 * ------------------------------------------------------------------------- */

/* Retrieves the media size
 * Returns the 1 if succesful or -1 on error
 */
LIBQCOW_EXTERN \
int libqcow_file_get_media_size(
     libqcow_file_t *file,
     size64_t *media_size,
     libqcow_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif

