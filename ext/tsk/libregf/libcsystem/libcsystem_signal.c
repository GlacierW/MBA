/*
 * Signal handling functions
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

#if defined( HAVE_SIGNAL_H )
#include <signal.h>
#endif

#if defined( _MSC_VER )
#include <crtdbg.h>
#endif

#include "libcsystem_libcerror.h"
#include "libcsystem_signal.h"
#include "libcsystem_types.h"

#if defined( WINAPI )

void (*libcsystem_signal_signal_handler)( libcsystem_signal_t ) = NULL;

/* Signal handler for Ctrl+C or Ctrl+Break signals
 */
BOOL WINAPI libcsystem_signal_handler(
             unsigned long signal )
{
	static char *function = "libcsystem_signal_handler";

	switch( signal )
	{
		/* use Ctrl+C or Ctrl+Break to simulate SERVICE_CONTROL_STOP in debug mode
		 */
		case CTRL_BREAK_EVENT:
		case CTRL_C_EVENT:
			if( libcsystem_signal_signal_handler != NULL )
			{
				libcsystem_signal_signal_handler(
				 signal );
			}
			return( TRUE );

		default:
			break;
	}
	return( FALSE );
}

#if defined( _MSC_VER )

/* Initialize memory usage and leakage debugging
 */
void libcsystem_signal_initialize_memory_debug(
      void )
{
	int flag = 0;

	/* Get the current state of the flag and store it in a temporary variable
	 */
	flag = _CrtSetDbgFlag(
	         _CRTDBG_REPORT_FLAG );

	/* Turn on client block identifiers and automatic leak detection
	 */
	flag |= ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	/* Set the new state for the flag
	 */
	_CrtSetDbgFlag(
	 flag );
}

#endif

/* Attaches a signal handler for Ctrl+C or Ctrl+Break signals
 * Returns 1 if successful or -1 on error
 */
int libcsystem_signal_attach(
     void (*signal_handler)( libcsystem_signal_t ),
     libcerror_error_t **error )
{
	static char *function = "libcsystem_signal_attach";

	if( signal_handler == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid signal handler.",
		 function );

		return( -1 );
	}
	libcsystem_signal_signal_handler = signal_handler;

	if( SetConsoleCtrlHandler(
	     libcsystem_signal_handler,
	     TRUE ) == 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to attach signal handler.",
		 function );

		return( -1 );
	}
	if( SetConsoleCtrlHandler(
	     NULL,
	     FALSE ) == 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to attach break signal.",
		 function );

		return( -1 );
	}
#if defined( _MSC_VER )
	libcsystem_signal_initialize_memory_debug();
#endif

	SetErrorMode(
	 SEM_FAILCRITICALERRORS );

#if defined( LOCALE_SUPPORT )
	/* Allow subsequent threads to have their own locale.
	 * The current application is single threaded so this
	 * call has no practical effect here.
	 */
	_configthreadlocale(
	  _ENABLE_PER_THREAD_LOCALE );

	/* Set the current thread locale to the user default
	 * ANSI code page.
	 */
	setlocale(
	 LC_ALL,
	 "" );

	/* Set the the code page used by multibyte functions
	 * to use the same code page as the previous call to setlocale.
	 */
	_setmbcp(
	  _MB_CP_LOCALE );
#endif

	return( 1 );
}

/* Detaches a signal handler for Ctrl+C or Ctrl+Break signals
 * Returns 1 if successful or -1 on error
 */
int libcsystem_signal_detach(
     libcerror_error_t **error )
{
	static char *function = "libcsystem_signal_detach";

	if( SetConsoleCtrlHandler(
	     libcsystem_signal_handler,
	     FALSE ) == 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to detach signal handler.",
		 function );

		return( -1 );
	}
	libcsystem_signal_signal_handler = NULL;

	return( 1 );
}

#elif defined( HAVE_SIGNAL_H )

/* Attaches a signal handler for SIGINT
 * Returns 1 if successful or -1 on error
 */
int libcsystem_signal_attach(
     void (*signal_handler)( libcsystem_signal_t ),
     libcerror_error_t **error )
{
	static char *function = "libcsystem_signal_attach";

	if( signal_handler == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid signal handler.",
		 function );

		return( -1 );
	}
	if( signal(
	     SIGINT,
	     signal_handler ) == SIG_ERR )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to attach signal handler.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Detaches a signal handler for SIGINT
 * Returns 1 if successful or -1 on error
 */
int libcsystem_signal_detach(
     libcerror_error_t **error )
{
	static char *function = "libcsystem_signal_detach";

	if( signal(
	     SIGINT,
	     SIG_DFL ) == SIG_ERR )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to detach signal handler.",
		 function );

		return( -1 );
	}
	return( 1 );
}

#endif

