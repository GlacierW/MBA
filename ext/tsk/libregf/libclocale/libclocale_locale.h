/*
 * Locale functions
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

#if !defined( _LIBCLOCALE_LOCALE_H )
#define _LIBCLOCALE_LOCALE_H

#include <common.h>
#include <types.h>

#include "libclocale_extern.h"
#include "libclocale_libcerror.h"

#if defined( __cplusplus )
extern "C" {
#endif

#if defined( WINAPI ) && ( WINVER < 0x0500 )

int libclocale_GetLocaleInfo(
     LCID locale_identifier,
     LCTYPE locale_type,
     LPSTR buffer,
     int size );

#endif /* defined( WINAPI ) && ( WINVER < 0x0500 ) */

LIBCLOCALE_EXTERN \
int libclocale_locale_get_codepage(
     int *codepage,
     libcerror_error_t **error );

LIBCLOCALE_EXTERN \
int libclocale_locale_get_decimal_point(
     int *decimal_point,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBCLOCALE_LOCALE_H ) */

