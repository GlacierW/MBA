/*
 * Data type functions
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

#if !defined( _LIBREGF_DATA_TYPE_H )
#define _LIBREGF_DATA_TYPE_H

#include <common.h>
#include <types.h>

#if defined( __cplusplus )
extern "C" {
#endif

#if defined( HAVE_DEBUG_OUTPUT )

typedef struct libregf_data_type libregf_data_type_t;

struct libregf_data_type
{
	/* The data_type
	 */
	uint32_t data_type;

	/* The identifier
	 */
	const char *identifier;

	/* The description
	 */
	const char *description;
};

const char *libregf_data_type_get_identifier(
             uint32_t data_type );

const char *libregf_data_type_get_description(
             uint32_t data_type );

#endif

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBREGF_DATA_TYPE_H ) */

