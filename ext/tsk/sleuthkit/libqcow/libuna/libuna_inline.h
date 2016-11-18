/*
 * The internal inline definition
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

#if !defined( _LIBUNA_INTERNAL_INLINE_H )
#define _LIBUNA_INTERNAL_INLINE_H

#include <common.h>

#if defined( _MSC_VER )
#define LIBUNA_INLINE _inline

#elif defined( __BORLANDC__ ) || defined( __clang__ ) || ( defined( __GNUC__ ) && __GNUC__ >= 5 )
#define LIBUNA_INLINE /* inline */

#else
#define LIBUNA_INLINE inline

#endif

#endif /* !defined( _LIBUNA_INTERNAL_INLINE_H ) */

