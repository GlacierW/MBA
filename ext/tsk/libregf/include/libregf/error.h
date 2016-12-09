/*
 * The error code definitions for libregf
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

#if !defined( _LIBREGF_ERROR_H )
#define _LIBREGF_ERROR_H

#include <libregf/types.h>

/* External error type definition hides internal structure
 */
typedef intptr_t libregf_error_t;

/* The error domains
 */
enum LIBREGF_ERROR_DOMAINS
{
	LIBREGF_ERROR_DOMAIN_ARGUMENTS			= (int) 'a',
	LIBREGF_ERROR_DOMAIN_CONVERSION			= (int) 'c',
	LIBREGF_ERROR_DOMAIN_COMPRESSION		= (int) 'C',
	LIBREGF_ERROR_DOMAIN_IO				= (int) 'I',
	LIBREGF_ERROR_DOMAIN_INPUT			= (int) 'i',
	LIBREGF_ERROR_DOMAIN_MEMORY			= (int) 'm',
	LIBREGF_ERROR_DOMAIN_OUTPUT			= (int) 'o',
	LIBREGF_ERROR_DOMAIN_RUNTIME			= (int) 'r'
};

/* The argument error codes
 * to signify errors regarding arguments passed to a function
 */
enum LIBREGF_ARGUMENT_ERROR
{
	LIBREGF_ARGUMENT_ERROR_GENERIC			= 0,

	/* The argument contains an invalid value
	 */
	LIBREGF_ARGUMENT_ERROR_INVALID_VALUE		= 1,

	/* The argument contains a value less than zero
	 */
	LIBREGF_ARGUMENT_ERROR_VALUE_LESS_THAN_ZERO	= 2,

	/* The argument contains a value zero or less
	 */
	LIBREGF_ARGUMENT_ERROR_VALUE_ZERO_OR_LESS	= 3,

	/* The argument contains a value that exceeds the maximum
	 * for the specific type
	 */
	LIBREGF_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM	= 4,

	/* The argument contains a value that is too small
	 */
	LIBREGF_ARGUMENT_ERROR_VALUE_TOO_SMALL		= 5,

	/* The argument contains a value that is too large
	 */
	LIBREGF_ARGUMENT_ERROR_VALUE_TOO_LARGE		= 6,

	/* The argument contains a value that is out of bounds
	 */
	LIBREGF_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS	= 7,

	/* The argument contains a value that is not supported
	 */
	LIBREGF_ARGUMENT_ERROR_UNSUPPORTED_VALUE	= 8,

	/* The argument contains a value that conficts with another argument
	 */
	LIBREGF_ARGUMENT_ERROR_CONFLICTING_VALUE	= 9
};

/* The conversion error codes
 * to signify errors regarding conversions
 */
enum LIBREGF_CONVERSION_ERROR
{
	LIBREGF_CONVERSION_ERROR_GENERIC		= 0,

	/* The conversion failed on the input
	 */
	LIBREGF_CONVERSION_ERROR_INPUT_FAILED		= 1,

	/* The conversion failed on the output
	 */
	LIBREGF_CONVERSION_ERROR_OUTPUT_FAILED		= 2
};

/* The compression error codes
 * to signify errors regarding compression
 */
enum LIBREGF_COMPRESSION_ERROR
{
	LIBREGF_COMPRESSION_ERROR_GENERIC		= 0,

	/* The compression failed
	 */
	LIBREGF_COMPRESSION_ERROR_COMPRESS_FAILED	= 1,

	/* The decompression failed
	 */
	LIBREGF_COMPRESSION_ERROR_DECOMPRESS_FAILED	= 2
};

/* The input/output error codes
 * to signify errors regarding input/output
 */
enum LIBREGF_IO_ERROR
{
	LIBREGF_IO_ERROR_GENERIC			= 0,

	/* The open failed
	 */
	LIBREGF_IO_ERROR_OPEN_FAILED			= 1,

	/* The close failed
	 */
	LIBREGF_IO_ERROR_CLOSE_FAILED			= 2,

	/* The seek failed
	 */
	LIBREGF_IO_ERROR_SEEK_FAILED			= 3,

	/* The read failed
	 */
	LIBREGF_IO_ERROR_READ_FAILED			= 4,

	/* The write failed
	 */
	LIBREGF_IO_ERROR_WRITE_FAILED			= 5,

	/* Access denied
	 */
	LIBREGF_IO_ERROR_ACCESS_DENIED			= 6,

	/* The resource is invalid i.e. a missing file
	 */
	LIBREGF_IO_ERROR_INVALID_RESOURCE		= 7,

	/* The ioctl failed
	 */
	LIBREGF_IO_ERROR_IOCTL_FAILED			= 8,

	/* The unlink failed
	 */
	LIBREGF_IO_ERROR_UNLINK_FAILED			= 9
};

/* The input error codes
 * to signify errors regarding handing input data
 */
enum LIBREGF_INPUT_ERROR
{
	LIBREGF_INPUT_ERROR_GENERIC			= 0,

	/* The input contains invalid data
	 */
	LIBREGF_INPUT_ERROR_INVALID_DATA		= 1,

	/* The input contains an unsupported signature
	 */
	LIBREGF_INPUT_ERROR_SIGNATURE_MISMATCH		= 2,

	/* A checksum in the input did not match
	 */
	LIBREGF_INPUT_ERROR_CHECKSUM_MISMATCH		= 3,

	/* A value in the input did not match a previously
	 * read value or calculated value
	 */
	LIBREGF_INPUT_ERROR_VALUE_MISMATCH		= 4
};

/* The memory error codes
 * to signify errors regarding memory
 */
enum LIBREGF_MEMORY_ERROR
{
	LIBREGF_MEMORY_ERROR_GENERIC			= 0,

	/* There is insufficient memory available
	 */
	LIBREGF_MEMORY_ERROR_INSUFFICIENT		= 1,

	/* The memory failed to be copied
	 */
	LIBREGF_MEMORY_ERROR_COPY_FAILED		= 2,

	/* The memory failed to be set
	 */
	LIBREGF_MEMORY_ERROR_SET_FAILED			= 3
};

/* The output error codes
 */
enum LIBREGF_OUTPUT_ERROR
{
	LIBREGF_OUTPUT_ERROR_GENERIC			= 0,

	/* There is insuficient space to write the output
	 */
	LIBREGF_OUTPUT_ERROR_INSUFFICIENT_SPACE		= 1
};

/* The runtime error codes
 * to signify errors regarding runtime processing
 */
enum LIBREGF_RUNTIME_ERROR
{
	LIBREGF_RUNTIME_ERROR_GENERIC			= 0,

	/* The value is missing
	 */
	LIBREGF_RUNTIME_ERROR_VALUE_MISSING		= 1,

	/* The value was already set
	 */
	LIBREGF_RUNTIME_ERROR_VALUE_ALREADY_SET		= 2,

	/* The creation and/or initialization of an internal structure failed
	 */
	LIBREGF_RUNTIME_ERROR_INITIALIZE_FAILED		= 3,

	/* The resize of an internal structure failed
	 */
	LIBREGF_RUNTIME_ERROR_RESIZE_FAILED		= 4,

	/* The free and/or finalization of an internal structure failed
	 */
	LIBREGF_RUNTIME_ERROR_FINALIZE_FAILED		= 5,

	/* The value could not be determined
	 */
	LIBREGF_RUNTIME_ERROR_GET_FAILED		= 6,

	/* The value could not be set
	 */
	LIBREGF_RUNTIME_ERROR_SET_FAILED		= 7,

	/* The value could not be appended/prepended
	 */
	LIBREGF_RUNTIME_ERROR_APPEND_FAILED		= 8,

	/* The value could not be copied
	 */
	LIBREGF_RUNTIME_ERROR_COPY_FAILED		= 9,

	/* The value could not be removed
	 */
	LIBREGF_RUNTIME_ERROR_REMOVE_FAILED		= 10,

	/* The value could not be printed
	 */
	LIBREGF_RUNTIME_ERROR_PRINT_FAILED		= 11,

	/* The value was out of bounds
	 */
	LIBREGF_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS	= 12,

	/* The value exceeds the maximum for its specific type
	 */
	LIBREGF_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM	= 13,

	/* The value is unsupported
	 */
	LIBREGF_RUNTIME_ERROR_UNSUPPORTED_VALUE		= 14,

	/* An abort was requested
	 */
	LIBREGF_RUNTIME_ERROR_ABORT_REQUESTED		= 15
};

#endif /* !defined( _LIBREGF_ERROR_H ) */

