/*
 * ISO 8859-4 codepage (Baltic) function
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
#include <types.h>

#include "libuna_codepage_iso_8859_4.h"

/* Extended ASCII to Unicode character lookup table for ISO 8859-4 codepage
 * Unknown are filled with the Unicode replacement character 0xfffd
 */
const uint16_t libuna_codepage_iso_8859_4_byte_stream_to_unicode_base_0xa0[ 96 ] = {
	0x00a0, 0x0104, 0x0138, 0x0156, 0x00a4, 0x0128, 0x013b, 0x00a7,
	0x00a8, 0x0160, 0x0112, 0x0122, 0x0166, 0x00ad, 0x017d, 0x00af,
	0x00b0, 0x0105, 0x02db, 0x0157, 0x00b4, 0x0129, 0x013c, 0x02c7,
	0x00b8, 0x0161, 0x0113, 0x0123, 0x0167, 0x014a, 0x017e, 0x014b,
	0x0100, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x012e,
	0x010c, 0x00c9, 0x0118, 0x00cb, 0x0116, 0x00cd, 0x00ce, 0x012a,
	0x0110, 0x0145, 0x014c, 0x0136, 0x00d4, 0x00d5, 0x00d6, 0x00d7,
	0x00d8, 0x0172, 0x00da, 0x00db, 0x00dc, 0x0168, 0x016a, 0x00df,
	0x0101, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x012f,
	0x010d, 0x00e9, 0x0119, 0x00eb, 0x0117, 0x00ed, 0x00ee, 0x012b,
	0x0111, 0x0146, 0x014d, 0x0137, 0x00f4, 0x00f5, 0x00f6, 0x00f7,
	0x00f8, 0x0173, 0x00fa, 0x00fb, 0x00fc, 0x0169, 0x016b, 0x02d9
};

/* Unicode to ASCII character lookup table for ISO 8859-4 codepage
 * Unknown are filled with the ASCII replacement character 0x1a
 */
const uint8_t libuna_codepage_iso_8859_4_unicode_to_byte_stream_base_0x00a0[ 184 ] = {
	0xa0, 0x1a, 0x1a, 0x1a, 0xa4, 0x1a, 0x1a, 0xa7,
	0xa8, 0x1a, 0x1a, 0x1a, 0x1a, 0xad, 0x1a, 0xaf,
	0xb0, 0x1a, 0x1a, 0x1a, 0xb4, 0x1a, 0x1a, 0x1a,
	0xb8, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a,
	0x1a, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0x1a,
	0x1a, 0xc9, 0x1a, 0xcb, 0x1a, 0xcd, 0xce, 0x1a,
	0x1a, 0x1a, 0x1a, 0x1a, 0xd4, 0xd5, 0xd6, 0xd7,
	0xd8, 0x1a, 0xda, 0xdb, 0xdc, 0x1a, 0x1a, 0xdf,
	0x1a, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0x1a,
	0x1a, 0xe9, 0x1a, 0xeb, 0x1a, 0xed, 0xee, 0x1a,
	0x1a, 0x1a, 0x1a, 0x1a, 0xf4, 0xf5, 0xf6, 0xf7,
	0xf8, 0x1a, 0xfa, 0xfb, 0xfc, 0x1a, 0x1a, 0x1a,
	0xc0, 0xe0, 0x1a, 0x1a, 0xa1, 0xb1, 0x1a, 0x1a,
	0x1a, 0x1a, 0x1a, 0x1a, 0xc8, 0xe8, 0x1a, 0x1a,
	0xd0, 0xf0, 0xaa, 0xba, 0x1a, 0x1a, 0xcc, 0xec,
	0xca, 0xea, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a,
	0x1a, 0x1a, 0xab, 0xbb, 0x1a, 0x1a, 0x1a, 0x1a,
	0xa5, 0xb5, 0xcf, 0xef, 0x1a, 0x1a, 0xc7, 0xe7,
	0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0xd3, 0xf3,
	0xa2, 0x1a, 0x1a, 0xa6, 0xb6, 0x1a, 0x1a, 0x1a,
	0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0xd1, 0xf1, 0x1a,
	0x1a, 0x1a, 0xbd, 0xbf, 0xd2, 0xf2, 0x1a, 0x1a,
	0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0xa3, 0xb3
};

const uint8_t libuna_codepage_iso_8859_4_unicode_to_byte_stream_base_0x0160[ 32 ] = {
	0xa9, 0xb9, 0x1a, 0x1a, 0x1a, 0x1a, 0xac, 0xbc,
	0xdd, 0xfd, 0xde, 0xfe, 0x1a, 0x1a, 0x1a, 0x1a,
	0x1a, 0x1a, 0xd9, 0xf9, 0x1a, 0x1a, 0x1a, 0x1a,
	0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0xae, 0xbe, 0x1a
};

