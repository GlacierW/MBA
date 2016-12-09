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

#include <common.h>
#include <memory.h>
#include <types.h>

#include "libregf_data_type.h"

#if defined( HAVE_DEBUG_OUTPUT )

libregf_data_type_t libregf_data_types[ ] = {
	{ 0x00000000, "REG_NONE",			"Undefined" },
	{ 0x00000001, "REG_SZ",				"String" },
	{ 0x00000002, "REG_EXPAND_SZ",			"String with expandable variables" },
	{ 0x00000003, "REG_BINARY",			"Binary data" },
	{ 0x00000004, "REG_DWORD_LITTLE_ENDIAN",	"Integer 32-bit signed little-endian" },
	{ 0x00000005, "REG_DWORD_BIG_ENDIAN",		"Integer 32-bit signed big-endian" },
	{ 0x00000006, "REG_LINK",			"Link" },
	{ 0x00000007, "REG_MULTI_SZ",			"Array of strings" },
	{ 0x00000008, "REG_RESOURCE_LIST",		"Resource list" },
	{ 0x00000009, "REG_FULL_RESOURCE_DESCRIPTOR",	"Full resource descriptor" },
	{ 0x0000000a, "REG_RESOURCE_REQUIREMENTS_LIST",	"Resource requirements list" },
	{ 0x0000000b, "REG_QWORD_LITTLE_ENDIAN",	"Integer 64-bit signed little-endian" },

	{ (uint32_t) -1, "_UNKNOWN_", "Unknown" } };

/* Retrieves a string containing the data_type identifier
 */
const char *libregf_data_type_get_identifier(
             uint32_t data_type )
{
	int iterator = 0;

	while( ( libregf_data_types[ iterator ] ).data_type != (uint32_t) -1 )
	{
		if( ( libregf_data_types[ iterator ] ).data_type == data_type )
		{
			break;
		}
		iterator++;
	}
	return(
	 ( libregf_data_types[ iterator ] ).identifier );
}

/* Retrieves a string containing the data_type description
 */
const char *libregf_data_type_get_description(
             uint32_t data_type )
{
	int iterator = 0;

	while( ( libregf_data_types[ iterator ] ).data_type != (uint32_t) -1 )
	{
		if( ( libregf_data_types[ iterator ] ).data_type == data_type )
		{
			break;
		}
		iterator++;
	}
	return(
	 ( libregf_data_types[ iterator ] ).description );
}

#endif

