/*
 * The libfwnt header wrapper
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

#if !defined( _LIBREGF_LIBFWNT_H )
#define _LIBREGF_LIBFWNT_H

#include <common.h>

/* Define HAVE_LOCAL_LIBFWNT for local use of libfwnt
 */
#if defined( HAVE_LOCAL_LIBFWNT )

#include <libfwnt_access_control_entry.h>
#include <libfwnt_access_control_list.h>
#include <libfwnt_definitions.h>
#include <libfwnt_locale_identifier.h>
#include <libfwnt_lznt1.h>
#include <libfwnt_lzxpress.h>
#include <libfwnt_security_descriptor.h>
#include <libfwnt_security_identifier.h>
#include <libfwnt_types.h>

#else

/* If libtool DLL support is enabled set LIBFWNT_DLL_IMPORT
 * before including libfwnt.h
 */
#if defined( _WIN32 ) && defined( DLL_IMPORT )
#define LIBFWNT_DLL_IMPORT
#endif

#include <libfwnt.h>

#endif /* defined( HAVE_LOCAL_LIBFWNT ) */

#endif /* !defined( _LIBREGF_LIBFWNT_H ) */

