/*
 * The internal definitions
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

#if !defined( LIBCERROR_INTERNAL_DEFINITIONS_H )
#define LIBCERROR_INTERNAL_DEFINITIONS_H

#include <common.h>
#include <types.h>

/* Define HAVE_LOCAL_LIBCERROR for local use of libcerror
 */
#if !defined( HAVE_LOCAL_LIBCERROR )
#include <libcerror/definitions.h>

/* The definitions in <libcerror/definitions.h> are copied here
 * for local use of libcerror
 */
#else

#define LIBCERROR_VERSION				20161106

/* The libcerror version string
 */
#define LIBCERROR_VERSION_STRING			"20161106"

/* The error domains
 */
enum LIBCERROR_ERROR_DOMAINS
{
	LIBCERROR_ERROR_DOMAIN_ARGUMENTS		= (int) 'a',
	LIBCERROR_ERROR_DOMAIN_CONVERSION		= (int) 'c',
	LIBCERROR_ERROR_DOMAIN_COMPRESSION		= (int) 'C',
	LIBCERROR_ERROR_DOMAIN_ENCRYPTION		= (int) 'E',
	LIBCERROR_ERROR_DOMAIN_IO			= (int) 'I',
	LIBCERROR_ERROR_DOMAIN_INPUT			= (int) 'i',
	LIBCERROR_ERROR_DOMAIN_MEMORY			= (int) 'm',
	LIBCERROR_ERROR_DOMAIN_OUTPUT			= (int) 'o',
	LIBCERROR_ERROR_DOMAIN_RUNTIME			= (int) 'r',
};

/* The argument error codes
 * to signify errors regarding arguments passed to a function
 */
enum LIBCERROR_ARGUMENT_ERROR
{
	LIBCERROR_ARGUMENT_ERROR_GENERIC		= 0,

	/* The argument contains an invalid value
	 */
	LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE		= 1,

	/* The argument contains a value less than zero
	 */
	LIBCERROR_ARGUMENT_ERROR_VALUE_LESS_THAN_ZERO	= 2,

	/* The argument contains a value zero or less
	 */
	LIBCERROR_ARGUMENT_ERROR_VALUE_ZERO_OR_LESS	= 3,

	/* The argument contains a value that exceeds the maximum
	 * for the specific type
	 */
	LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM	= 4,

	/* The argument contains a value that is too small
	 */
	LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL	= 5,

	/* The argument contains a value that is too large
	 */
	LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_LARGE	= 6,

	/* The argument contains a value that is out of bound
	 */
	LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS	= 7,

	/* The argument contains a value that is not supported
	 */
	LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE	= 8,

	/* The argument contains a value that conficts with another argument
	 */
	LIBCERROR_ARGUMENT_ERROR_CONFLICTING_VALUE	= 9
};

/* The conversion error codes
 * to signify errors regarding conversions
 */
enum LIBCERROR_CONVERSION_ERROR
{
	LIBCERROR_CONVERSION_ERROR_GENERIC		= 0,

	/* The conversion failed on the input
	 */
	LIBCERROR_CONVERSION_ERROR_INPUT_FAILED		= 1,

	/* The conversion failed on the output
	 */
	LIBCERROR_CONVERSION_ERROR_OUTPUT_FAILED	= 2
};

/* The compression error codes
 * to signify errors regarding compression
 */
enum LIBCERROR_COMPRESSION_ERROR
{
	LIBCERROR_COMPRESSION_ERROR_GENERIC		= 0,

	/* The compression failed
	 */
	LIBCERROR_COMPRESSION_ERROR_COMPRESS_FAILED	= 1,

	/* The decompression failed
	 */
	LIBCERROR_COMPRESSION_ERROR_DECOMPRESS_FAILED	= 2
};

/* The encryption error codes
 * to signify errors regarding encryption
 */
enum LIBCERROR_ENCRYPTION_ERROR
{
	LIBCERROR_ENCRYPTION_ERROR_GENERIC		= 0,

	/* The encryption failed
	 */
	LIBCERROR_ENCRYPTION_ERROR_ENCRYPT_FAILED	= 1,

	/* The decryption failed
	 */
	LIBCERROR_ENCRYPTION_ERROR_DECRYPT_FAILED	= 2
};

/* The input/output error codes
 * to signify errors regarding input/output
 */
enum LIBCERROR_IO_ERROR
{
	LIBCERROR_IO_ERROR_GENERIC			= 0,

	/* The open failed
	 */
	LIBCERROR_IO_ERROR_OPEN_FAILED			= 1,

	/* The close failed
	 */
	LIBCERROR_IO_ERROR_CLOSE_FAILED			= 2,

	/* The seek failed
	 */
	LIBCERROR_IO_ERROR_SEEK_FAILED			= 3,

	/* The read failed
	 */
	LIBCERROR_IO_ERROR_READ_FAILED			= 4,

	/* The write failed
	 */
	LIBCERROR_IO_ERROR_WRITE_FAILED			= 5,

	/* Access denied
	 */
	LIBCERROR_IO_ERROR_ACCESS_DENIED		= 6,

	/* The resource is invalid i.e. a missing file
	 */
	LIBCERROR_IO_ERROR_INVALID_RESOURCE		= 7,

	/* The ioctl failed
	 */
	LIBCERROR_IO_ERROR_IOCTL_FAILED			= 8,

	/* The unlink failed
	 */
	LIBCERROR_IO_ERROR_UNLINK_FAILED		= 9
};

/* The input error codes
 * to signify errors regarding handing input data
 */
enum LIBCERROR_INPUT_ERROR
{
	LIBCERROR_INPUT_ERROR_GENERIC			= 0,

	/* The input contains invalid data
	 */
	LIBCERROR_INPUT_ERROR_INVALID_DATA		= 1,

	/* The input contains an unsupported signature
	 */
	LIBCERROR_INPUT_ERROR_SIGNATURE_MISMATCH	= 2,

	/* A checksum in the input did not match
	 */
	LIBCERROR_INPUT_ERROR_CHECKSUM_MISMATCH		= 3,

	/* A value in the input did not match a previously
	 * read value or calculated value
	 */
	LIBCERROR_INPUT_ERROR_VALUE_MISMATCH		= 4
};

/* The memory error codes
 * to signify errors regarding memory
 */
enum LIBCERROR_MEMORY_ERROR
{
	LIBCERROR_MEMORY_ERROR_GENERIC			= 0,

	/* There is insufficient memory available
	 */
	LIBCERROR_MEMORY_ERROR_INSUFFICIENT		= 1,

	/* The memory failed to be copied
	 */
	LIBCERROR_MEMORY_ERROR_COPY_FAILED		= 2,

	/* The memory failed to be set
	 */
	LIBCERROR_MEMORY_ERROR_SET_FAILED		= 3
};

/* The output error codes
 */
enum LIBCERROR_OUTPUT_ERROR
{
	LIBCERROR_OUTPUT_ERROR_GENERIC			= 0,

	/* There is insuficient space to write the output
	 */
	LIBCERROR_OUTPUT_ERROR_INSUFFICIENT_SPACE	= 1
};

/* The runtime error codes
 * to signify errors regarding runtime processing
 */
enum LIBCERROR_RUNTIME_ERROR
{
	LIBCERROR_RUNTIME_ERROR_GENERIC			= 0,

	/* The value is missing
	 */
	LIBCERROR_RUNTIME_ERROR_VALUE_MISSING		= 1,

	/* The value was already set
	 */
	LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET	= 2,

	/* The creation and/or initialization of an internal structure failed
	 */
	LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED	= 3,

	/* The resize of an internal structure failed
	 */
	LIBCERROR_RUNTIME_ERROR_RESIZE_FAILED		= 4,

	/* The free and/or finalization of an internal structure failed
	 */
	LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED		= 5,

	/* The value could not be determined
	 */
	LIBCERROR_RUNTIME_ERROR_GET_FAILED		= 6,

	/* The value could not be set
	 */
	LIBCERROR_RUNTIME_ERROR_SET_FAILED		= 7,

	/* The value could not be appended/prepended
	 */
	LIBCERROR_RUNTIME_ERROR_APPEND_FAILED		= 8,

	/* The value could not be copied
	 */
	LIBCERROR_RUNTIME_ERROR_COPY_FAILED		= 9,

	/* The value could not be removed
	 */
	LIBCERROR_RUNTIME_ERROR_REMOVE_FAILED		= 10,

	/* The value could not be printed
	 */
	LIBCERROR_RUNTIME_ERROR_PRINT_FAILED		= 11,

	/* The value was out of bounds
	 */
	LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS	= 12,

	/* The value exceeds the maximum for its specific type
	 */
	LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM	= 13,

	/* The value is unsupported
	 */
	LIBCERROR_RUNTIME_ERROR_UNSUPPORTED_VALUE	= 14,

	/* An aborted was requested
	 */
	LIBCERROR_RUNTIME_ERROR_ABORT_REQUESTED		= 15
};

#endif /* !defined( HAVE_LOCAL_LIBCERROR ) */

#endif /* !defined( LIBCERROR_INTERNAL_DEFINITIONS_H ) */

