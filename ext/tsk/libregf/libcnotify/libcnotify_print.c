/*
 * Notification print functions
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

#include <common.h>
#include <file_stream.h>
#include <memory.h>
#include <types.h>

#if defined( HAVE_STDARG_H ) || defined( WINAPI )
#include <stdarg.h>
#elif defined( HAVE_VARARGS_H )
#include <varargs.h>
#else
#error Missing headers stdarg.h and varargs.h
#endif

#include "libcnotify_definitions.h"
#include "libcnotify_libcerror.h"
#include "libcnotify_print.h"
#include "libcnotify_stream.h"

extern FILE *libcnotify_stream;

#if defined( HAVE_STDARG_H ) || defined( WINAPI )
#define VARARGS( function, type, argument ) \
	function( type argument, ... )
#define VASTART( argument_list, type, name ) \
	va_start( argument_list, name )
#define VAEND( argument_list ) \
	va_end( argument_list )

#elif defined( HAVE_VARARGS_H )
#define VARARGS( function, type, argument ) \
	function( va_alist ) va_dcl
#define VASTART( argument_list, type, name ) \
	{ type name; va_start( argument_list ); name = va_arg( argument_list, type )
#define VAEND( argument_list ) \
	va_end( argument_list ); }

#endif

/* Print a formatted string on the notify stream
 * Returns the number of printed characters if successful or -1 on error
 */
int VARARGS(
     libcnotify_printf,
     const char *,
     format )
{
	va_list argument_list;

	int print_count = 0;

	if( libcnotify_stream == NULL )
	{
		return( 0 );
	}
	VASTART(
	 argument_list,
	 char *,
	 format );

#if defined( HAVE_GLIB_H )
	g_logv(
	 G_LOG_DOMAIN,
	 G_LOG_LEVEL_MESSAGE,
	 format,
	 argument_list );

#else
	/* TODO handle narrow and wide streams
	 * for multi platform support
	 */
	print_count = file_stream_vfprintf(
		       libcnotify_stream,
		       format,
		       argument_list );
#endif

	VAEND(
	 argument_list );

	if( print_count <= -1 )
	{
		return( -1 );
	}
	return( print_count );
}

#undef VARARGS
#undef VASTART
#undef VAEND

/* Prints the data on the notify stream
 * Returns the number of printed characters if successful or -1 on error
 */
int libcnotify_print_data(
     const uint8_t *data,
     size_t data_size,
     uint8_t print_data_flags )
{
	size_t byte_iterator  = 0;
	size_t data_iterator  = 0;
	int in_group          = 0;
	int print_count       = 0;
	int total_print_count = 0;

	if( libcnotify_stream == NULL )
	{
		return( 0 );
	}
	if( data == NULL )
	{
		return( -1 );
	}
	while( data_iterator < data_size )
	{
		if( ( ( print_data_flags & LIBCNOTIFY_PRINT_DATA_FLAG_GROUP_DATA ) != 0 )
		 && ( data_size >= 32 )
		 && ( data_iterator >= 16 )
		 && ( data_iterator <= ( data_size - 32 ) ) )
		{
			if( ( memory_compare(
			       &( data[ data_iterator - 16 ] ),
			       &( data[ data_iterator ] ),
			       16 ) == 0 )
			 && ( memory_compare(
			       &( data[ data_iterator + 16 ] ),
			       &( data[ data_iterator ] ),
			       16 ) == 0 ) )
			{
				if( in_group == 0 )
				{
					print_count = libcnotify_printf(
						       "...\n" );

					if( print_count <= -1 )
					{
						return( -1 );
					}
					total_print_count += print_count;

					in_group = 1;
				}
				byte_iterator += 16;
				data_iterator += 16;

				continue;
			}
			in_group = 0;
		}
		while( byte_iterator < data_size )
		{
			if( byte_iterator % 16 == 0 )
			{
				print_count = libcnotify_printf(
					       "%.8" PRIzx ": ",
					       byte_iterator );

				if( print_count <= -1 )
				{
					return( -1 );
				}
				total_print_count += print_count;
			}
			print_count = libcnotify_printf(
				       "%.2" PRIx8 " ",
				       data[ byte_iterator++ ] );

			if( print_count <= -1 )
			{
				return( -1 );
			}
			total_print_count += print_count;

			if( byte_iterator % 16 == 0 )
			{
				break;
			}
			else if( byte_iterator % 8 == 0 )
			{
				print_count = libcnotify_printf(
					       " " );

				if( print_count <= -1 )
				{
					return( -1 );
				}
				total_print_count += print_count;
			}
		}
		while( byte_iterator % 16 != 0 )
		{
			byte_iterator++;

			print_count = libcnotify_printf(
				       "   " );

			if( print_count <= -1 )
			{
				return( -1 );
			}
			total_print_count += print_count;

			if( ( byte_iterator % 8 == 0 )
			 && ( byte_iterator % 16 != 0 ) )
			{
				print_count = libcnotify_printf(
					       " " );

				if( print_count <= -1 )
				{
					return( -1 );
				}
				total_print_count += print_count;
			}
		}
		print_count = libcnotify_printf(
			       "  " );

		if( print_count <= -1 )
		{
			return( -1 );
		}
		total_print_count += print_count;

		byte_iterator = data_iterator;

		while( byte_iterator < data_size )
		{
			if( ( data[ byte_iterator ] >= 0x20 )
			 && ( data[ byte_iterator ] <= 0x7e ) )
			{
				print_count = libcnotify_printf(
					       "%c",
					       (char) data[ byte_iterator ] );
			}
			else
			{
				print_count = libcnotify_printf(
					       "." );
			}
			if( print_count <= -1 )
			{
				return( -1 );
			}
			total_print_count += print_count;

			byte_iterator++;

			if( byte_iterator % 16 == 0 )
			{
				break;
			}
			else if( byte_iterator % 8 == 0 )
			{
				print_count = libcnotify_printf(
					       " " );

				if( print_count <= -1 )
				{
					return( -1 );
				}
				total_print_count += print_count;
			}
		}
		print_count = libcnotify_printf(
			       "\n" );

		if( print_count <= -1 )
		{
			return( -1 );
		}
		total_print_count += print_count;

		data_iterator = byte_iterator;
	}
	print_count = libcnotify_printf(
		       "\n" );

	if( print_count <= -1 )
	{
		return( -1 );
	}
	total_print_count += print_count;

	return( total_print_count );
}

/* Prints the backtrace of the error on the notify stream
 * Returns the number of printed characters if successful or -1 on error
 */
int libcnotify_print_error_backtrace(
     libcerror_error_t *error )
{
	int print_count = 0;

	if( libcnotify_stream == NULL )
	{
		return( 0 );
	}
	print_count = libcerror_error_backtrace_fprint(
	               error,
	               libcnotify_stream );

	return( print_count );
}

