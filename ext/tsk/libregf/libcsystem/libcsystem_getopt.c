/*
 * GetOpt functions
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
#include <narrow_string.h>
#include <system_string.h>
#include <types.h>
#include <wide_string.h>

#if defined( HAVE_STDLIB_H ) || defined( WINAPI )
#include <stdlib.h>
#endif

#include "libcsystem_getopt.h"
#include "libcsystem_libcnotify.h"

#if !defined( HAVE_GETOPT )

/* The current option argument
 */
system_character_t *optarg = NULL;

/* The option index
 * Start with argument 1 (argument 0 is the program name)
 */
int optind = 1;

/* Value to indicate the current option
 */
system_integer_t optopt = 0;

/* Get the program options
 * Function for platforms that do not have the getopt function
 * Returns the option character processed, or -1 on error,
 * ? if the option was not in the options string, : if the option argument was missing
 */
system_integer_t libcsystem_getopt(
                  int argument_count,
                  system_character_t * const argument_values[],
                  const system_character_t *options_string )
{
	system_character_t *option_value   = NULL;
	system_character_t *argument_value = NULL;
	static char *function              = "libcsystem_getopt";
	size_t options_string_length       = 0;

	if( optind >= argument_count )
	{
		return( (system_integer_t) -1 );
	}
	argument_value = argument_values[ optind ];

	/* Check if the argument value is not an empty string
	 */
	if( *argument_value == 0 )
	{
		return( (system_integer_t) -1 );
	}
	/* Check if the first character is a option marker '-'
	 */
	if( *argument_value != (system_character_t) '-' )
	{
		return( (system_integer_t) -1 );
	}
	argument_value++;

	/* Check if long options are provided '--'
	 */
	if( *argument_value == (system_character_t) '-' )
	{
		optind++;

		return( (system_integer_t) -1 );
	}
	options_string_length = system_string_length(
	                         options_string );

	optopt       = *argument_value;
	option_value = system_string_search_character(
	                options_string,
	                optopt,
	                options_string_length );

	argument_value++;

	/* Check if an argument was specified or that the option was not found
	 * in the option string
	 */
	if( ( optopt == (system_integer_t) ':' )
	 || ( option_value == NULL ) )
	{
		if( *argument_value == (system_character_t) '\0' )
		{
			optind++;
		}
		if( ( *options_string != (system_character_t) ':' )
		 && ( optopt != (system_integer_t) '?' ) )
		{
			libcnotify_printf(
			 "%s: no such option: %" PRIc_SYSTEM ".\n",
			 function,
			 optopt );
		}
		return( (system_integer_t) '?' );
	}
	option_value++;

	/* Check if no option argument is required
	 */
	if( *option_value != (system_character_t) ':' )
	{
		optarg = NULL;

		if( *argument_value == (system_character_t) '\0' )
		{
			optind++;
		}
	}
	/* Check if the argument is right after the option flag with no space in between
	 */
	else if( *argument_value != (system_character_t) '\0' )
	{
		optarg = argument_value;

		optind++;
	}
	else
	{
		optind++;

		/* Check if the argument was provided as the next argument value
		 */
		if( argument_count <= optind )
		{
			if( *option_value == ':' )
			{
				return( (system_integer_t) ':' );
			}
			libcnotify_printf(
			 "%s: option: %" PRIc_SYSTEM " requires an argument.\n",
			 function,
			 optopt );

			return( (system_integer_t) '?' );
		}
		optarg = argument_values[ optind ];

		optind++;
	}
	return( optopt );
}

#endif /* !defined( HAVE_GETOPT ) */

