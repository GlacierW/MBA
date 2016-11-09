/*
 * The internal definitions
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

#if !defined( _LIBCLOCALE_INTERNAL_DEFINITIONS_H )
#define _LIBCLOCALE_INTERNAL_DEFINITIONS_H

#include <common.h>

/* Define HAVE_LOCAL_LIBCLOCALE for local use of libclocale
 */
#if !defined( HAVE_LOCAL_LIBCLOCALE )
#include <libclocale/definitions.h>

/* The definitions in <libclocale/definitions.h> are copied here
 * for local use of libclocale
 */
#else

#define LIBCLOCALE_VERSION					20161106

/* The libclocale version string
 */
#define LIBCLOCALE_VERSION_STRING				"20161106"

/* The codepage feature flag definitions
 */
enum LIBCLOCALE_CODEPAGES_FEATURE_FLAGS
{
	LIBCLOCALE_CODEPAGE_FEATURE_FLAG_HAVE_ISO_8859		= 0x00000001UL,
	LIBCLOCALE_CODEPAGE_FEATURE_FLAG_HAVE_KOI8		= 0x00000002UL,
	LIBCLOCALE_CODEPAGE_FEATURE_FLAG_HAVE_WINDOWS		= 0x00000004UL,
};

/* The codepage set definitions
 */
enum LIBCLOCALE_CODEPAGES_SETS
{
	LIBCLOCALE_CODEPAGE_SET_GENERIC				= 1,
	LIBCLOCALE_CODEPAGE_SET_ISO_8859			= (int) 'i',
	LIBCLOCALE_CODEPAGE_SET_KOI8				= (int) 'k',
	LIBCLOCALE_CODEPAGE_SET_WINDOWS				= (int) 'w'
};

/* The codepage definitions
 */
enum LIBCLOCALE_CODEPAGESS
{
	LIBCLOCALE_CODEPAGE_ASCII				= 20127,

	LIBCLOCALE_CODEPAGE_ISO_8859_1				= 28591,
	LIBCLOCALE_CODEPAGE_ISO_8859_2				= 28592,
	LIBCLOCALE_CODEPAGE_ISO_8859_3				= 28593,
	LIBCLOCALE_CODEPAGE_ISO_8859_4				= 28594,
	LIBCLOCALE_CODEPAGE_ISO_8859_5				= 28595,
	LIBCLOCALE_CODEPAGE_ISO_8859_6				= 28596,
	LIBCLOCALE_CODEPAGE_ISO_8859_7				= 28597,
	LIBCLOCALE_CODEPAGE_ISO_8859_8				= 28598,
	LIBCLOCALE_CODEPAGE_ISO_8859_9				= 28599,
	LIBCLOCALE_CODEPAGE_ISO_8859_10				= 28600,
	LIBCLOCALE_CODEPAGE_ISO_8859_11				= 28601,
	LIBCLOCALE_CODEPAGE_ISO_8859_13				= 28603,
	LIBCLOCALE_CODEPAGE_ISO_8859_14				= 28604,
	LIBCLOCALE_CODEPAGE_ISO_8859_15				= 28605,
	LIBCLOCALE_CODEPAGE_ISO_8859_16				= 28606,

	LIBCLOCALE_CODEPAGE_KOI8_R				= 20866,
	LIBCLOCALE_CODEPAGE_KOI8_U				= 21866,

	LIBCLOCALE_CODEPAGE_WINDOWS_874				= 874,
	LIBCLOCALE_CODEPAGE_WINDOWS_932				= 932,
	LIBCLOCALE_CODEPAGE_WINDOWS_936				= 936,
	LIBCLOCALE_CODEPAGE_WINDOWS_949				= 949,
	LIBCLOCALE_CODEPAGE_WINDOWS_950				= 950,
	LIBCLOCALE_CODEPAGE_WINDOWS_1250			= 1250,
	LIBCLOCALE_CODEPAGE_WINDOWS_1251			= 1251,
	LIBCLOCALE_CODEPAGE_WINDOWS_1252			= 1252,
	LIBCLOCALE_CODEPAGE_WINDOWS_1253			= 1253,
	LIBCLOCALE_CODEPAGE_WINDOWS_1254			= 1254,
	LIBCLOCALE_CODEPAGE_WINDOWS_1255			= 1255,
	LIBCLOCALE_CODEPAGE_WINDOWS_1256			= 1256,
	LIBCLOCALE_CODEPAGE_WINDOWS_1257			= 1257,
	LIBCLOCALE_CODEPAGE_WINDOWS_1258			= 1258
};

#endif

#endif

