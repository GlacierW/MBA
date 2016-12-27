/*
 * The internal libcerror header
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

#if !defined( _LIBFDATETIME_LIBCERROR_H )
#define _LIBFDATETIME_LIBCERROR_H

#include <common.h>

/* Define HAVE_LOCAL_LIBCERROR for local use of libcerror
 */
#if defined( HAVE_LOCAL_LIBCERROR )

#include <libcerror_definitions.h>
#include <libcerror_error.h>
#include <libcerror_system.h>
#include <libcerror_types.h>

#else

/* If libtool DLL support is enabled set LIBCERROR_DLL_IMPORT
 * before including libcerror.h
 */
#if defined( _WIN32 ) && defined( DLL_IMPORT )
#define LIBCERROR_DLL_IMPORT
#endif

#include <libcerror.h>

#endif /* defined( HAVE_LOCAL_LIBCERROR ) */

#endif /* !defined( _LIBFDATETIME_LIBCERROR_H ) */

