/*
 * The internal type definitions
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

#if !defined( _LIBUNA_INTERNAL_TYPES_H )
#define _LIBUNA_INTERNAL_TYPES_H

#include <common.h>
#include <types.h>

/* Define HAVE_LOCAL_LIBUNA for local use of libuna
 * The definitions in <libuna/types.h> are copied here
 * for local use of libuna
 */
#if defined( HAVE_LOCAL_LIBUNA )

/* Unicode character definitions
 */
typedef uint32_t libuna_unicode_character_t;

/* UTF-8 character definitions
 */
typedef uint8_t libuna_utf8_character_t;

/* UTF-16 character definitions
 */
typedef uint16_t libuna_utf16_character_t;

/* UTF-32 character definitions
 */
typedef uint32_t libuna_utf32_character_t;

#endif /* defined( HAVE_LOCAL_LIBUNA ) */
 
#endif /* !defined( _LIBUNA_INTERNAL_TYPES_H ) */

