/*
 * Codepage functions
 *
 * Copyright (C) 2010-2016, Joachim Metz <joachim.metz@gmail.com>
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
#include <narrow_string.h>
#include <wide_string.h>
#include <types.h>

#include "libclocale_codepage.h"
#include "libclocale_definitions.h"
#include "libclocale_libcerror.h"
#include "libclocale_wide_string.h"

/* If the codepage is not set strings are formatted using UTF-8
 */
#if defined( WINAPI )
int libclocale_codepage = LIBCLOCALE_CODEPAGE_WINDOWS_1252;
#else
int libclocale_codepage = 0;
#endif

/* Retrieves the narrow system string codepage
 * A value of 0 represents no codepage, UTF-8 encoding is used instead
 * Returns 1 if successful or -1 on error
 */
int libclocale_codepage_get(
     int *codepage,
     libcerror_error_t **error )
{
	static char *function = "libclocale_codepage_get";

	if( codepage == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid codepage.",
		 function );

		return( -1 );
	}
	*codepage = libclocale_codepage;

	return( 1 );
}

/* Sets the narrow system string codepage
 * A value of 0 represents no codepage, UTF-8 encoding is used instead
 * Returns 1 if successful or -1 on error
 */
int libclocale_codepage_set(
     int codepage,
     libcerror_error_t **error )
{
	static char *function = "libclocale_codepage_set";

	if( ( codepage != LIBCLOCALE_CODEPAGE_ASCII )
	 && ( codepage != LIBCLOCALE_CODEPAGE_ISO_8859_1 )
	 && ( codepage != LIBCLOCALE_CODEPAGE_ISO_8859_2 )
	 && ( codepage != LIBCLOCALE_CODEPAGE_ISO_8859_3 )
	 && ( codepage != LIBCLOCALE_CODEPAGE_ISO_8859_4 )
	 && ( codepage != LIBCLOCALE_CODEPAGE_ISO_8859_5 )
	 && ( codepage != LIBCLOCALE_CODEPAGE_ISO_8859_6 )
	 && ( codepage != LIBCLOCALE_CODEPAGE_ISO_8859_7 )
	 && ( codepage != LIBCLOCALE_CODEPAGE_ISO_8859_8 )
	 && ( codepage != LIBCLOCALE_CODEPAGE_ISO_8859_9 )
	 && ( codepage != LIBCLOCALE_CODEPAGE_ISO_8859_10 )
	 && ( codepage != LIBCLOCALE_CODEPAGE_ISO_8859_11 )
	 && ( codepage != LIBCLOCALE_CODEPAGE_ISO_8859_13 )
	 && ( codepage != LIBCLOCALE_CODEPAGE_ISO_8859_14 )
	 && ( codepage != LIBCLOCALE_CODEPAGE_ISO_8859_15 )
	 && ( codepage != LIBCLOCALE_CODEPAGE_ISO_8859_16 )
	 && ( codepage != LIBCLOCALE_CODEPAGE_KOI8_R )
	 && ( codepage != LIBCLOCALE_CODEPAGE_KOI8_U )
	 && ( codepage != LIBCLOCALE_CODEPAGE_WINDOWS_874 )
	 && ( codepage != LIBCLOCALE_CODEPAGE_WINDOWS_932 )
	 && ( codepage != LIBCLOCALE_CODEPAGE_WINDOWS_936 )
	 && ( codepage != LIBCLOCALE_CODEPAGE_WINDOWS_949 )
	 && ( codepage != LIBCLOCALE_CODEPAGE_WINDOWS_950 )
	 && ( codepage != LIBCLOCALE_CODEPAGE_WINDOWS_1250 )
	 && ( codepage != LIBCLOCALE_CODEPAGE_WINDOWS_1251 )
	 && ( codepage != LIBCLOCALE_CODEPAGE_WINDOWS_1252 )
	 && ( codepage != LIBCLOCALE_CODEPAGE_WINDOWS_1253 )
	 && ( codepage != LIBCLOCALE_CODEPAGE_WINDOWS_1254 )
	 && ( codepage != LIBCLOCALE_CODEPAGE_WINDOWS_1256 )
	 && ( codepage != LIBCLOCALE_CODEPAGE_WINDOWS_1257 )
	 && ( codepage != LIBCLOCALE_CODEPAGE_WINDOWS_1258 )
	 && ( codepage != 0 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported codepage.",
		 function );

		return( -1 );
	}
	libclocale_codepage = codepage;

	return( 1 );
}

/* Copies the codepage from a string
 * Returns 1 if successful, 0 if unsupported value or -1 on error
 */
int libclocale_codepage_copy_from_string(
     int *codepage,
     const char *string,
     size_t string_length,
     uint32_t feature_flags,
     libcerror_error_t **error )
{
	static char *function    = "libclocale_codepage_copy_from_string";
	size_t string_index      = 0;
	uint32_t supported_flags = 0;
	int codepage_set         = 0;

	if( codepage == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid codepage.",
		 function );

		return( -1 );
	}
	if( string == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid string.",
		 function );

		return( -1 );
	}
	if( string_length > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid string length value exceeds maximum.",
		 function );

		return( -1 );
	}
	supported_flags = LIBCLOCALE_CODEPAGE_FEATURE_FLAG_HAVE_ISO_8859
	                | LIBCLOCALE_CODEPAGE_FEATURE_FLAG_HAVE_KOI8
	                | LIBCLOCALE_CODEPAGE_FEATURE_FLAG_HAVE_WINDOWS;

	if( ( feature_flags & ~( supported_flags ) ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported feature flags value: 0x%08" PRIx32 ".",
		 function,
		 feature_flags );

		return( -1 );
	}
	*codepage = -1;

	if( string_length == 5 )
	{
		if( narrow_string_compare_no_case(
		     string,
		     "ascii",
		     5 ) == 0 )
		{
			*codepage = LIBCLOCALE_CODEPAGE_ASCII;

			string_index = 5;
		}
	}
	if( ( feature_flags & LIBCLOCALE_CODEPAGE_FEATURE_FLAG_HAVE_ISO_8859 ) != 0 )
	{
		/* Supported string formats:
		 * iso8859-1, iso8859_1, iso-8859-1, iso-8859_1, iso_8859-1, iso_8859_1
		 */
		if( string_length >= 8 )
		{
			if( narrow_string_compare_no_case(
			     string,
			     "iso",
			     3 ) == 0 )
			{
				string_index = 3;

				if( ( string[ string_index ] == '-' )
				 || ( string[ string_index ] == '_' ) )
				{
					string_index++;
				}
				if( ( string[ string_index     ] == '8' )
				 && ( string[ string_index + 1 ] == '8' )
				 && ( string[ string_index + 2 ] == '5' )
				 && ( string[ string_index + 3 ] == '9' ) )
				{
					string_index += 4;

					if( string_index < string_length )
					{
						if( ( string[ string_index ] == '-' )
						 || ( string[ string_index ] == '_' ) )
						{
							string_index++;

							codepage_set = LIBCLOCALE_CODEPAGE_SET_ISO_8859;
						}
					}
				}
			}
		}
	}
	if( ( feature_flags & LIBCLOCALE_CODEPAGE_FEATURE_FLAG_HAVE_KOI8 ) != 0 )
	{
		/* Supported string formats:
		 * koi8, koi8-r, koi8_r
		 */
		if( string_length >= 4 )
		{
			if( narrow_string_compare_no_case(
			     string,
			     "koi8",
			     4 ) == 0 )
			{
				string_index = 4;

				codepage_set = LIBCLOCALE_CODEPAGE_SET_KOI8;

				if( string_index < string_length )
				{
					if( ( string[ string_index ] == '-' )
					 || ( string[ string_index ] == '_' ) )
					{
						string_index++;
					}
				}
			}
		}
	}
	if( ( feature_flags & LIBCLOCALE_CODEPAGE_FEATURE_FLAG_HAVE_WINDOWS ) != 0 )
	{
		/* Supported string formats:
		 * cp1252, ms1252, windows1252, windows-1252, windows_1252
		 */
		if( string_length >= 7 )
		{
			if( narrow_string_compare_no_case(
			     string,
			     "windows",
			     7 ) == 0 )
			{
				string_index = 7;

				codepage_set = LIBCLOCALE_CODEPAGE_SET_WINDOWS;

				if( string_index < string_length )
				{
					if( ( string[ string_index ] == '-' )
					 || ( string[ string_index ] == '_' ) )
					{
						string_index++;
					}
				}
			}
		}
		else if( string_length >= 2 )
		{
			if( narrow_string_compare_no_case(
			     string,
			     "cp",
			     2 ) == 0 )
			{
				string_index = 2;

				codepage_set = LIBCLOCALE_CODEPAGE_SET_GENERIC;
			}
			else if( narrow_string_compare_no_case(
			          string,
			          "ms",
			          2 ) == 0 )
			{
				string_index = 2;

				codepage_set = LIBCLOCALE_CODEPAGE_SET_WINDOWS;
			}
		}
	}
	if( codepage_set == LIBCLOCALE_CODEPAGE_SET_ISO_8859 )
	{
		if( ( string_index + 1 ) == string_length )
		{
			switch( string[ string_index ] )
			{
				case '1':
					*codepage = LIBCLOCALE_CODEPAGE_ISO_8859_1;
					break;

				case '2':
					*codepage = LIBCLOCALE_CODEPAGE_ISO_8859_2;
					break;

				case '3':
					*codepage = LIBCLOCALE_CODEPAGE_ISO_8859_3;
					break;

				case '4':
					*codepage = LIBCLOCALE_CODEPAGE_ISO_8859_4;
					break;

				case '5':
					*codepage = LIBCLOCALE_CODEPAGE_ISO_8859_5;
					break;

				case '6':
					*codepage = LIBCLOCALE_CODEPAGE_ISO_8859_6;
					break;

				case '7':
					*codepage = LIBCLOCALE_CODEPAGE_ISO_8859_7;
					break;

				case '8':
					*codepage = LIBCLOCALE_CODEPAGE_ISO_8859_8;
					break;

				case '9':
					*codepage = LIBCLOCALE_CODEPAGE_ISO_8859_9;
					break;

				default:
					break;
			}
		}
		else if( ( string_index + 2 ) == string_length )
		{
			if( string[ string_index ] == '1' )
			{
				string_index++;

				switch( string[ string_index ] )
				{
					case '0':
						*codepage = LIBCLOCALE_CODEPAGE_ISO_8859_10;
						break;

					case '1':
						*codepage = LIBCLOCALE_CODEPAGE_ISO_8859_11;
						break;

					case '3':
						*codepage = LIBCLOCALE_CODEPAGE_ISO_8859_13;
						break;

					case '4':
						*codepage = LIBCLOCALE_CODEPAGE_ISO_8859_14;
						break;

					case '5':
						*codepage = LIBCLOCALE_CODEPAGE_ISO_8859_15;
						break;

					case '6':
						*codepage = LIBCLOCALE_CODEPAGE_ISO_8859_16;
						break;

					default:
						break;
				}
			}
		}
	}
	if( codepage_set == LIBCLOCALE_CODEPAGE_SET_KOI8 )
	{
		if( ( string_index + 1 ) == string_length )
		{
			switch( string[ string_index ] )
			{
				case 'r':
				case 'R':
					*codepage = LIBCLOCALE_CODEPAGE_KOI8_R;
					break;

				case 'u':
				case 'U':
					*codepage = LIBCLOCALE_CODEPAGE_KOI8_U;
					break;

				default:
					break;
			}
		}
	}
	if( ( codepage_set == LIBCLOCALE_CODEPAGE_SET_GENERIC )
	 || ( codepage_set == LIBCLOCALE_CODEPAGE_SET_WINDOWS ) )
	{
		if( ( string_index + 3 ) == string_length )
		{
			switch( string[ string_index ] )
			{
				case '8':
					if( ( string[ string_index + 1 ] == '7' )
					 && ( string[ string_index + 2 ] == '4' ) )
					{
						*codepage = LIBCLOCALE_CODEPAGE_WINDOWS_874;
					}
					break;

				case '9':
					if( ( string[ string_index + 1 ] == '3' )
					 && ( string[ string_index + 2 ] == '2' ) )
					{
						*codepage = LIBCLOCALE_CODEPAGE_WINDOWS_932;
					}
					else if( ( string[ string_index + 1 ] == '3' )
					      && ( string[ string_index + 2 ] == '6' ) )
					{
						*codepage = LIBCLOCALE_CODEPAGE_WINDOWS_936;
					}
					else if( ( string[ string_index + 1 ] == '4' )
					      && ( string[ string_index + 2 ] == '9' ) )
					{
						*codepage = LIBCLOCALE_CODEPAGE_WINDOWS_949;
					}
					else if( ( string[ string_index + 1 ] == '5' )
					      && ( string[ string_index + 2 ] == '0' ) )
					{
						*codepage = LIBCLOCALE_CODEPAGE_WINDOWS_950;
					}
					break;

				default:
					break;
			}
		}
		else if( ( string_index + 4 ) == string_length )
		{
			if( ( string[ string_index     ] == '1' )
			 && ( string[ string_index + 1 ] == '2' )
			 && ( string[ string_index + 2 ] == '5' ) )
			{
				switch( string[ string_index + 3 ] )
				{
					case '0':
						*codepage = LIBCLOCALE_CODEPAGE_WINDOWS_1250;
						break;

					case '1':
						*codepage = LIBCLOCALE_CODEPAGE_WINDOWS_1251;
						break;

					case '2':
						*codepage = LIBCLOCALE_CODEPAGE_WINDOWS_1252;
						break;

					case '3':
						*codepage = LIBCLOCALE_CODEPAGE_WINDOWS_1253;
						break;

					case '4':
						*codepage = LIBCLOCALE_CODEPAGE_WINDOWS_1254;
						break;

					case '5':
						*codepage = LIBCLOCALE_CODEPAGE_WINDOWS_1255;
						break;

					case '6':
						*codepage = LIBCLOCALE_CODEPAGE_WINDOWS_1256;
						break;

					case '7':
						*codepage = LIBCLOCALE_CODEPAGE_WINDOWS_1257;
						break;

					case '8':
						*codepage = LIBCLOCALE_CODEPAGE_WINDOWS_1258;
						break;

					default:
						break;
				}
			}
		}
	}
	if( *codepage != -1 )
	{
		return( 1 );
	}
	return( 0 );
}

#if defined( HAVE_WIDE_CHARACTER_TYPE )

/* Copies the codepage from a string
 * Returns 1 if successful, 0 if unsupported value or -1 on error
 */
int libclocale_codepage_copy_from_string_wide(
     int *codepage,
     const wchar_t *string,
     size_t string_length,
     uint32_t feature_flags,
     libcerror_error_t **error )
{
	static char *function    = "libclocale_codepage_copy_from_string_wide";
	size_t string_index      = 0;
	uint32_t supported_flags = 0;
	int codepage_set         = 0;

	if( codepage == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid codepage.",
		 function );

		return( -1 );
	}
	if( string == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid string.",
		 function );

		return( -1 );
	}
	if( string_length > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid string length value exceeds maximum.",
		 function );

		return( -1 );
	}
	supported_flags = LIBCLOCALE_CODEPAGE_FEATURE_FLAG_HAVE_ISO_8859
	                | LIBCLOCALE_CODEPAGE_FEATURE_FLAG_HAVE_KOI8
	                | LIBCLOCALE_CODEPAGE_FEATURE_FLAG_HAVE_WINDOWS;

	if( ( feature_flags & ~( supported_flags ) ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported feature flags value: 0x%08" PRIx32 ".",
		 function,
		 feature_flags );

		return( -1 );
	}
	*codepage = -1;

	if( string_length == 5 )
	{
		if( wide_string_compare_no_case(
		     string,
		     L"ascii",
		     5 ) == 0 )
		{
			*codepage = LIBCLOCALE_CODEPAGE_ASCII;

			string_index = 5;
		}
	}
	if( ( feature_flags & LIBCLOCALE_CODEPAGE_FEATURE_FLAG_HAVE_ISO_8859 ) != 0 )
	{
		/* Supported string formats:
		 * iso8859-1, iso8859_1, iso-8859-1, iso-8859_1, iso_8859-1, iso_8859_1
		 */
		if( string_length >= 8 )
		{
			if( wide_string_compare_no_case(
			     string,
			     L"iso",
			     3 ) == 0 )
			{
				string_index = 3;

				if( ( string[ string_index ] == (wchar_t) '-' )
				 || ( string[ string_index ] == (wchar_t) '_' ) )
				{
					string_index++;
				}
				if( ( string[ string_index     ] == (wchar_t) '8' )
				 && ( string[ string_index + 1 ] == (wchar_t) '8' )
				 && ( string[ string_index + 2 ] == (wchar_t) '5' )
				 && ( string[ string_index + 3 ] == (wchar_t) '9' ) )
				{
					string_index += 4;

					if( string_index < string_length )
					{
						if( ( string[ string_index ] == (wchar_t) '-' )
						 || ( string[ string_index ] == (wchar_t) '_' ) )
						{
							string_index++;

							codepage_set = LIBCLOCALE_CODEPAGE_SET_ISO_8859;
						}
					}
				}
			}
		}
	}
	if( ( feature_flags & LIBCLOCALE_CODEPAGE_FEATURE_FLAG_HAVE_KOI8 ) != 0 )
	{
		/* Supported string formats:
		 * koi8, koi8-r, koi8_r
		 */
		if( string_length >= 4 )
		{
			if( wide_string_compare_no_case(
			     string,
			     L"koi8",
			     4 ) == 0 )
			{
				string_index = 4;

				codepage_set = LIBCLOCALE_CODEPAGE_SET_KOI8;

				if( string_index < string_length )
				{
					if( ( string[ string_index ] == (wchar_t) '-' )
					 || ( string[ string_index ] == (wchar_t) '_' ) )
					{
						string_index++;
					}
				}
			}
		}
	}
	if( ( feature_flags & LIBCLOCALE_CODEPAGE_FEATURE_FLAG_HAVE_WINDOWS ) != 0 )
	{
		/* Supported string formats:
		 * cp1252, ms1252, windows1252, windows-1252, windows_1252
		 */
		if( string_length >= 7 )
		{
			if( wide_string_compare_no_case(
			     string,
			     L"windows",
			     7 ) == 0 )
			{
				string_index = 7;

				codepage_set = LIBCLOCALE_CODEPAGE_SET_WINDOWS;

				if( string_index < string_length )
				{
					if( ( string[ string_index ] == (wchar_t) '-' )
					 || ( string[ string_index ] == (wchar_t) '_' ) )
					{
						string_index++;
					}
				}
			}
		}
		else if( string_length >= 2 )
		{
			if( wide_string_compare_no_case(
			     string,
			     L"cp",
			     2 ) == 0 )
			{
				string_index = 2;

				codepage_set = LIBCLOCALE_CODEPAGE_SET_GENERIC;
			}
			else if( wide_string_compare_no_case(
			          string,
			          L"ms",
			          2 ) == 0 )
			{
				string_index = 2;

				codepage_set = LIBCLOCALE_CODEPAGE_SET_WINDOWS;
			}
		}
	}
	if( codepage_set == LIBCLOCALE_CODEPAGE_SET_ISO_8859 )
	{
		if( ( string_index + 1 ) == string_length )
		{
			switch( string[ string_index ] )
			{
				case '1':
					*codepage = LIBCLOCALE_CODEPAGE_ISO_8859_1;
					break;

				case '2':
					*codepage = LIBCLOCALE_CODEPAGE_ISO_8859_2;
					break;

				case '3':
					*codepage = LIBCLOCALE_CODEPAGE_ISO_8859_3;
					break;

				case '4':
					*codepage = LIBCLOCALE_CODEPAGE_ISO_8859_4;
					break;

				case '5':
					*codepage = LIBCLOCALE_CODEPAGE_ISO_8859_5;
					break;

				case '6':
					*codepage = LIBCLOCALE_CODEPAGE_ISO_8859_6;
					break;

				case '7':
					*codepage = LIBCLOCALE_CODEPAGE_ISO_8859_7;
					break;

				case '8':
					*codepage = LIBCLOCALE_CODEPAGE_ISO_8859_8;
					break;

				case '9':
					*codepage = LIBCLOCALE_CODEPAGE_ISO_8859_9;
					break;

				default:
					break;
			}
		}
		else if( ( string_index + 2 ) == string_length )
		{
			if( string[ string_index ] == (wchar_t) '1' )
			{
				string_index++;

				switch( string[ string_index ] )
				{
					case '0':
						*codepage = LIBCLOCALE_CODEPAGE_ISO_8859_10;
						break;

					case '1':
						*codepage = LIBCLOCALE_CODEPAGE_ISO_8859_11;
						break;

					case '3':
						*codepage = LIBCLOCALE_CODEPAGE_ISO_8859_13;
						break;

					case '4':
						*codepage = LIBCLOCALE_CODEPAGE_ISO_8859_14;
						break;

					case '5':
						*codepage = LIBCLOCALE_CODEPAGE_ISO_8859_15;
						break;

					case '6':
						*codepage = LIBCLOCALE_CODEPAGE_ISO_8859_16;
						break;

					default:
						break;
				}
			}
		}
	}
	if( codepage_set == LIBCLOCALE_CODEPAGE_SET_KOI8 )
	{
		if( ( string_index + 1 ) == string_length )
		{
			switch( string[ string_index ] )
			{
				case 'r':
				case 'R':
					*codepage = LIBCLOCALE_CODEPAGE_KOI8_R;
					break;

				case 'u':
				case 'U':
					*codepage = LIBCLOCALE_CODEPAGE_KOI8_U;
					break;

				default:
					break;
			}
		}
	}
	if( ( codepage_set == LIBCLOCALE_CODEPAGE_SET_GENERIC )
	 || ( codepage_set == LIBCLOCALE_CODEPAGE_SET_WINDOWS ) )
	{
		if( ( string_index + 3 ) == string_length )
		{
			switch( string[ string_index ] )
			{
				case '8':
					if( ( string[ string_index + 1 ] == (wchar_t) '7' )
					 && ( string[ string_index + 2 ] == (wchar_t) '4' ) )
					{
						*codepage = LIBCLOCALE_CODEPAGE_WINDOWS_874;
					}
					break;

				case '9':
					if( ( string[ string_index + 1 ] == (wchar_t) '3' )
					 && ( string[ string_index + 2 ] == (wchar_t) '2' ) )
					{
						*codepage = LIBCLOCALE_CODEPAGE_WINDOWS_932;
					}
					else if( ( string[ string_index + 1 ] == (wchar_t) '3' )
					      && ( string[ string_index + 2 ] == (wchar_t) '6' ) )
					{
						*codepage = LIBCLOCALE_CODEPAGE_WINDOWS_936;
					}
					else if( ( string[ string_index + 1 ] == (wchar_t) '4' )
					      && ( string[ string_index + 2 ] == (wchar_t) '9' ) )
					{
						*codepage = LIBCLOCALE_CODEPAGE_WINDOWS_949;
					}
					else if( ( string[ string_index + 1 ] == (wchar_t) '5' )
					      && ( string[ string_index + 2 ] == (wchar_t) '0' ) )
					{
						*codepage = LIBCLOCALE_CODEPAGE_WINDOWS_950;
					}
					break;

				default:
					break;
			}
		}
		else if( ( string_index + 4 ) == string_length )
		{
			if( ( string[ string_index     ] == (wchar_t) '1' )
			 && ( string[ string_index + 1 ] == (wchar_t) '2' )
			 && ( string[ string_index + 2 ] == (wchar_t) '5' ) )
			{
				switch( string[ string_index + 3 ] )
				{
					case '0':
						*codepage = LIBCLOCALE_CODEPAGE_WINDOWS_1250;
						break;

					case '1':
						*codepage = LIBCLOCALE_CODEPAGE_WINDOWS_1251;
						break;

					case '2':
						*codepage = LIBCLOCALE_CODEPAGE_WINDOWS_1252;
						break;

					case '3':
						*codepage = LIBCLOCALE_CODEPAGE_WINDOWS_1253;
						break;

					case '4':
						*codepage = LIBCLOCALE_CODEPAGE_WINDOWS_1254;
						break;

					case '5':
						*codepage = LIBCLOCALE_CODEPAGE_WINDOWS_1255;
						break;

					case '6':
						*codepage = LIBCLOCALE_CODEPAGE_WINDOWS_1256;
						break;

					case '7':
						*codepage = LIBCLOCALE_CODEPAGE_WINDOWS_1257;
						break;

					case '8':
						*codepage = LIBCLOCALE_CODEPAGE_WINDOWS_1258;
						break;

					default:
						break;
				}
			}
		}
	}
	if( *codepage != -1 )
	{
		return( 1 );
	}
	return( 0 );
}

#endif /* defined( HAVE_WIDE_CHARACTER_TYPE ) */

