/*
 * The internal extern definition
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

#if !defined( _LIBCSPLIT_INTERNAL_EXTERN_H )
#define _LIBCSPLIT_INTERNAL_EXTERN_H

#include <common.h>

/* Define HAVE_LOCAL_LIBCSPLIT for local use of libcsplit
 */
#if !defined( HAVE_LOCAL_LIBCSPLIT )

/* If libtool DLL support is enabled set LIBCSPLIT_DLL_EXPORT
 * before including libcsplit/extern.h
 */
#if defined( _WIN32 ) && defined( DLL_EXPORT )
#define LIBCSPLIT_DLL_EXPORT
#endif

#include <libcsplit/extern.h>

#else
#define LIBCSPLIT_EXTERN	extern

#endif

#endif /* !defined( _LIBCSPLIT_INTERNAL_EXTERN_H ) */

