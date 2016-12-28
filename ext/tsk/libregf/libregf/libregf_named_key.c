/*
 * Named key functions
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

#include <common.h>
#include <byte_stream.h>
#include <memory.h>
#include <narrow_string.h>
#include <system_string.h>
#include <types.h>
#include <wide_string.h>

#if defined( HAVE_WCTYPE_H )
#include <wctype.h>
#endif

#include "libregf_debug.h"
#include "libregf_definitions.h"
#include "libregf_libcerror.h"
#include "libregf_libcnotify.h"
#include "libregf_libfdatetime.h"
#include "libregf_libuna.h"
#include "libregf_named_key.h"
#include "libregf_unused.h"

#include "regf_cell_values.h"

/* Creates a named key
 * Make sure the value named_key is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libregf_named_key_initialize(
     libregf_named_key_t **named_key,
     libcerror_error_t **error )
{
	static char *function = "libregf_named_key_initialize";

	if( named_key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid named key.",
		 function );

		return( -1 );
	}
	if( *named_key != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid named key value already set.",
		 function );

		return( -1 );
	}
	*named_key = memory_allocate_structure(
	              libregf_named_key_t );

	if( *named_key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create named key.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *named_key,
	     0,
	     sizeof( libregf_named_key_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear named key.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( *named_key != NULL )
	{
		memory_free(
		 *named_key );

		*named_key = NULL;
	}
	return( -1 );
}

/* Frees a named key
 * Returns 1 if successful or -1 on error
 */
int libregf_named_key_free(
     libregf_named_key_t **named_key,
     libcerror_error_t **error )
{
	static char *function = "libregf_named_key_free";

	if( named_key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid named key.",
		 function );

		return( -1 );
	}
	if( *named_key != NULL )
	{
		if( ( *named_key )->name != NULL )
		{
			memory_free(
			 ( *named_key )->name );
		}
		memory_free(
		 *named_key );

		*named_key = NULL;
	}
	return( 1 );
}

/* Reads a named key
 * Returns 1 if successful or -1 on error
 */
int libregf_named_key_read(
     libregf_named_key_t *named_key,
     const uint8_t *data,
     size_t data_size,
     uint32_t named_key_hash LIBREGF_ATTRIBUTE_UNUSED,
     int ascii_codepage LIBREGF_ATTRIBUTE_UNUSED,
     libcerror_error_t **error )
{
	static char *function                        = "libregf_named_key_read";
	libuna_unicode_character_t unicode_character = 0;
	size_t data_offset                           = 0;
	size_t name_index                            = 0;
	uint32_t class_name_offset                   = 0;
	uint32_t number_of_sub_keys                  = 0;
	uint32_t number_of_values_list_elements      = 0;
	uint32_t security_key_offset                 = 0;
	uint32_t sub_keys_list_offset                = 0;
	uint32_t values_list_offset                  = 0;
	uint16_t class_name_size                     = 0;
	int result                                   = 0;

#if defined( HAVE_DEBUG_OUTPUT )
	system_character_t filetime_string[ 32 ];

	system_character_t *value_string             = NULL;
	libfdatetime_filetime_t *filetime            = NULL;
	size_t value_string_size                     = 0;
	uint32_t value_32bit                         = 0;
#endif

	LIBREGF_UNREFERENCED_PARAMETER( named_key_hash )
	LIBREGF_UNREFERENCED_PARAMETER( ascii_codepage )

	if( named_key == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid named key.",
		 function );

		return( -1 );
	}
	if( data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid data.",
		 function );

		return( -1 );
	}
	if( data_size < sizeof( regf_named_key_t ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: invalid data size value too small.",
		 function );

		return( -1 );
	}
	if( data_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid data size value exceeds maximum.",
		 function );

		return( -1 );
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: named key data:\n",
		 function );
		libcnotify_print_data(
		 data,
		 data_size,
		 0 );
	}
#endif
	/* Check if the cell signature matches that of a named key: "nk"
	 */
	if( ( data[ 0 ] != (uint8_t) 'n' )
	 || ( data[ 1 ] != (uint8_t) 'k' ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported named key signature.",
		 function );

		goto on_error;
	}
	byte_stream_copy_to_uint16_little_endian(
	 ( (regf_named_key_t *) data )->flags,
	 named_key->flags );

	byte_stream_copy_to_uint64_little_endian(
	 ( (regf_named_key_t *) data )->last_written_time,
	 named_key->last_written_time );

	byte_stream_copy_to_uint32_little_endian(
	 ( (regf_named_key_t *) data )->number_of_sub_keys,
	 number_of_sub_keys );

	byte_stream_copy_to_uint32_little_endian(
	 ( (regf_named_key_t *) data )->sub_keys_list_offset,
	 sub_keys_list_offset );

	byte_stream_copy_to_uint32_little_endian(
	 ( (regf_named_key_t *) data )->number_of_values,
	 number_of_values_list_elements );

	byte_stream_copy_to_uint32_little_endian(
	 ( (regf_named_key_t *) data )->values_list_offset,
	 values_list_offset );

	byte_stream_copy_to_uint32_little_endian(
	 ( (regf_named_key_t *) data )->security_key_offset,
	 security_key_offset );

	byte_stream_copy_to_uint32_little_endian(
	 ( (regf_named_key_t *) data )->class_name_offset,
	 class_name_offset );

	byte_stream_copy_to_uint16_little_endian(
	 ( (regf_named_key_t *) data )->key_name_size,
	 named_key->name_size );

	byte_stream_copy_to_uint16_little_endian(
	 ( (regf_named_key_t *) data )->class_name_size,
	 class_name_size );

#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: signature\t\t\t\t: %c%c\n",
		 function,
		 ( (regf_named_key_t *) data )->signature[ 0 ],
		 ( (regf_named_key_t *) data )->signature[ 1 ] );

		libcnotify_printf(
		 "%s: flags\t\t\t\t\t: 0x%04" PRIx16 "\n",
		 function,
		 named_key->flags );
		libregf_debug_print_named_key_flags(
		 named_key->flags );

		if( libfdatetime_filetime_initialize(
		     &filetime,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create filetime.",
			 function );

			goto on_error;
		}
		if( libfdatetime_filetime_copy_from_64bit(
		     filetime,
		     named_key->last_written_time,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to copy filetime from 64-bit value.",
			 function );

			goto on_error;
		}
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
		result = libfdatetime_filetime_copy_to_utf16_string(
		          filetime,
		          (uint16_t *) filetime_string,
		          32,
		          LIBFDATETIME_STRING_FORMAT_TYPE_CTIME | LIBFDATETIME_STRING_FORMAT_FLAG_DATE_TIME_NANO_SECONDS,
		          error );
#else
		result = libfdatetime_filetime_copy_to_utf8_string(
		          filetime,
		          (uint8_t *) filetime_string,
		          32,
		          LIBFDATETIME_STRING_FORMAT_TYPE_CTIME | LIBFDATETIME_STRING_FORMAT_FLAG_DATE_TIME_NANO_SECONDS,
		          error );
#endif
		if( result != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to copy filetime to string.",
			 function );

			goto on_error;
		}
		libcnotify_printf(
		 "%s: last written time\t\t\t: %" PRIs_SYSTEM " UTC\n",
		 function,
		 filetime_string );

		if( libfdatetime_filetime_free(
		     &filetime,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free filetime.",
			 function );

			goto on_error;
		}
		byte_stream_copy_to_uint32_little_endian(
		 ( (regf_named_key_t *) data )->unknown1,
		 value_32bit );
		libcnotify_printf(
		 "%s: unknown1\t\t\t\t: 0x%08" PRIx32 " (%" PRIu32 ")\n",
		 function,
		 value_32bit,
		 value_32bit );

		byte_stream_copy_to_uint32_little_endian(
		 ( (regf_named_key_t *) data )->parent_key_offset,
		 value_32bit );
		libcnotify_printf(
		 "%s: parent key offset\t\t\t: 0x%08" PRIx32 "\n",
		 function,
		 value_32bit );

		libcnotify_printf(
		 "%s: number of sub keys\t\t\t: %" PRIu32 "\n",
		 function,
		 number_of_sub_keys );

		byte_stream_copy_to_uint32_little_endian(
		 ( (regf_named_key_t *) data )->number_of_volatile_sub_keys,
		 value_32bit );
		libcnotify_printf(
		 "%s: number of volatile sub keys\t\t: %" PRIu32 "\n",
		 function,
		 value_32bit );

		libcnotify_printf(
		 "%s: sub keys list offset\t\t\t: 0x%08" PRIx32 "\n",
		 function,
		 sub_keys_list_offset );

		byte_stream_copy_to_uint32_little_endian(
		 ( (regf_named_key_t *) data )->volatile_sub_keys_list_offset,
		 value_32bit );
		libcnotify_printf(
		 "%s: volatile sub keys list offset\t\t: 0x%08" PRIx32 "\n",
		 function,
		 value_32bit );

		libcnotify_printf(
		 "%s: number of values\t\t\t: %" PRIu32 "\n",
		 function,
		 number_of_values_list_elements );

		libcnotify_printf(
		 "%s: values list offset\t\t\t: 0x%08" PRIx32 "\n",
		 function,
		 values_list_offset );

		libcnotify_printf(
		 "%s: security key offset\t\t\t: 0x%08" PRIx32 "\n",
		 function,
		 security_key_offset );

		libcnotify_printf(
		 "%s: class name offset\t\t\t: 0x%08" PRIx32 "\n",
		 function,
		 class_name_offset );

		byte_stream_copy_to_uint32_little_endian(
		 ( (regf_named_key_t *) data )->largest_sub_key_name_size,
		 value_32bit );
		libcnotify_printf(
		 "%s: largest sub key name size\t\t: 0x%08" PRIx32 " (%" PRIu32 ")\n",
		 function,
		 value_32bit,
		 value_32bit );

		byte_stream_copy_to_uint32_little_endian(
		 ( (regf_named_key_t *) data )->largest_sub_key_class_name_size,
		 value_32bit );
		libcnotify_printf(
		 "%s: largest sub key class name size\t: 0x%08" PRIx32 " (%" PRIu32 ")\n",
		 function,
		 value_32bit,
		 value_32bit );

		byte_stream_copy_to_uint32_little_endian(
		 ( (regf_named_key_t *) data )->largest_value_name_size,
		 value_32bit );
		libcnotify_printf(
		 "%s: largest value name size\t\t: 0x%08" PRIx32 " (%" PRIu32 ")\n",
		 function,
		 value_32bit,
		 value_32bit );

		byte_stream_copy_to_uint32_little_endian(
		 ( (regf_named_key_t *) data )->largest_value_data_size,
		 value_32bit );
		libcnotify_printf(
		 "%s: largest value data size\t\t: 0x%08" PRIx32 " (%" PRIu32 ")\n",
		 function,
		 value_32bit,
		 value_32bit );

		byte_stream_copy_to_uint32_little_endian(
		 ( (regf_named_key_t *) data )->unknown6,
		 value_32bit );
		libcnotify_printf(
		 "%s: unknown6\t\t\t\t: 0x%08" PRIx32 " (%" PRIu32 ")\n",
		 function,
		 value_32bit,
		 value_32bit );

		libcnotify_printf(
		 "%s: key name size\t\t\t\t: %" PRIu16 "\n",
		 function,
		 named_key->name_size );

		libcnotify_printf(
		 "%s: class name size\t\t\t: %" PRIu16 "\n",
		 function,
		 class_name_size );
	}
#endif
	data_offset = sizeof( regf_named_key_t );

	if( named_key->name_size == 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: invalid key name size.",
		 function );

		goto on_error;
	}
	named_key->name = (uint8_t *) memory_allocate(
	                               sizeof( uint8_t ) * (size_t) named_key->name_size );

	if( named_key->name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create key name.",
		 function );

		goto on_error;
	}
	if( memory_copy(
	     named_key->name,
	     &( data[ data_offset ] ),
	     (size_t) named_key->name_size ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
		 "%s: unable to copy key name.",
		 function );

		goto on_error;
	}
	named_key->name_hash = 0;

	while( name_index < (size_t) named_key->name_size )
	{
		if( ( named_key->flags & LIBREGF_NAMED_KEY_FLAG_NAME_IS_ASCII ) != 0 )
		{
			result = libuna_unicode_character_copy_from_byte_stream(
				  &unicode_character,
				  named_key->name,
				  (size_t) named_key->name_size,
				  &name_index,
				  ascii_codepage,
				  error );
		}
		else
		{
			result = libuna_unicode_character_copy_from_utf16_stream(
				  &unicode_character,
				  named_key->name,
				  (size_t) named_key->name_size,
				  &name_index,
				  LIBUNA_ENDIAN_LITTLE,
				  error );
		}
		if( result != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
			 "%s: unable to copy key name to Unicode character.",
			 function );

			goto on_error;
		}
		named_key->name_hash *= 37;
		named_key->name_hash += (uint32_t) towupper( (wint_t) unicode_character );
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		if( ( named_key->flags & LIBREGF_NAMED_KEY_FLAG_NAME_IS_ASCII ) != 0 )
		{
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
			result = libuna_utf16_string_size_from_byte_stream(
				  named_key->name,
				  (size_t) named_key->name_size,
				  ascii_codepage,
				  &value_string_size,
				  error );
#else
			result = libuna_utf8_string_size_from_byte_stream(
				  named_key->name,
				  (size_t) named_key->name_size,
				  ascii_codepage,
				  &value_string_size,
				  error );
#endif
		}
		else
		{
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
			result = libuna_utf16_string_size_from_utf16_stream(
				  named_key->name,
				  (size_t) named_key->name_size,
				  LIBUNA_ENDIAN_LITTLE,
				  &value_string_size,
				  error );
#else
			result = libuna_utf8_string_size_from_utf16_stream(
				  named_key->name,
				  (size_t) named_key->name_size,
				  LIBUNA_ENDIAN_LITTLE,
				  &value_string_size,
				  error );
#endif
		}
		if( result != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve key name string size.",
			 function );

			goto on_error;
		}
		value_string = system_string_allocate(
		                value_string_size );

		if( value_string == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_MEMORY,
			 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to create key name string.",
			 function );

			goto on_error;
		}
		if( ( named_key->flags & LIBREGF_NAMED_KEY_FLAG_NAME_IS_ASCII ) != 0 )
		{
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
			result = libuna_utf16_string_copy_from_byte_stream(
				  (libuna_utf16_character_t *) value_string,
				  value_string_size,
				  named_key->name,
				  (size_t) named_key->name_size,
				  ascii_codepage,
				  error );
#else
			result = libuna_utf8_string_copy_from_byte_stream(
				  (libuna_utf8_character_t *) value_string,
				  value_string_size,
				  named_key->name,
				  (size_t) named_key->name_size,
				  ascii_codepage,
				  error );
#endif
		}
		else
		{
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
			result = libuna_utf16_string_copy_from_utf16_stream(
				  (libuna_utf16_character_t *) value_string,
				  value_string_size,
				  named_key->name,
				  (size_t) named_key->name_size,
				  LIBUNA_ENDIAN_LITTLE,
				  error );
#else
			result = libuna_utf8_string_copy_from_utf16_stream(
				  (libuna_utf8_character_t *) value_string,
				  value_string_size,
				  named_key->name,
				  (size_t) named_key->name_size,
				  LIBUNA_ENDIAN_LITTLE,
				  error );
#endif
		}
		if( result != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve key name string.",
			 function );

			goto on_error;
		}
		libcnotify_printf(
		 "%s: key name\t\t\t\t: %" PRIs_SYSTEM "\n",
		 function,
		 value_string );

		libcnotify_printf(
		 "%s: key name hash\t\t\t\t: 0x%08" PRIx32 "\n",
		 function,
		 named_key->name_hash );

		memory_free(
		 value_string );

		value_string = NULL;
	}
#endif
	data_offset += named_key->name_size;

#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		if( ( named_key_hash != 0 )
		 && ( named_key_hash != named_key->name_hash ) )
		{
			libcnotify_printf(
			 "%s: mismatch in name hash ( 0x%08" PRIx32 " != 0x%08" PRIx32 " ).\n",
			 function,
			 named_key_hash,
			 named_key->name_hash );
		}
		if( data_offset < data_size )
		{
			libcnotify_printf(
			 "%s: padding:\n",
			 function );
			libcnotify_print_data(
			 &( data[ data_offset ] ),
			 data_size - data_offset,
			 0 );
		}
		else
		{
			libcnotify_printf(
			 "\n" );
		}
	}
#endif
	return( 1 );

on_error:
#if defined( HAVE_DEBUG_OUTPUT )
	if( filetime != NULL )
	{
		libfdatetime_filetime_free(
		 &filetime,
		 NULL );
	}
	if( value_string != NULL )
	{
		memory_free(
		 value_string );
	}
#endif
	if( named_key->name != NULL )
	{
		memory_free(
		 named_key->name );

		named_key->name = NULL;
	}
	named_key->name_size = 0;

	return( -1 );
}

