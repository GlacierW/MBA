/*
 * Macros for testing
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

#if !defined( _REGF_TEST_MACROS_H )
#define _REGF_TEST_MACROS_H

#include <common.h>
#include <file_stream.h>

#if defined( HAVE_STDLIB_H ) || defined( WINAPI )
#include <stdlib.h>
#endif

#define REGF_TEST_ASSERT_EQUAL_INT( name, value, expected_value ) \
	if( value != expected_value ) \
	{ \
		fprintf( stdout, "%s:%d %s != %d\n", __FILE__, __LINE__, name, expected_value ); \
		goto on_error; \
	}

#define REGF_TEST_ASSERT_NOT_EQUAL_INT( name, value, expected_value ) \
	if( value == expected_value ) \
	{ \
		fprintf( stdout, "%s:%d %s == %d\n", __FILE__, __LINE__, name, expected_value ); \
		goto on_error; \
	}

#define REGF_TEST_ASSERT_GREATER_THAN_INT( name, value, expected_value ) \
	if( value <= expected_value ) \
	{ \
		fprintf( stdout, "%s:%d %s <= %d\n", __FILE__, __LINE__, name, expected_value ); \
		goto on_error; \
	}

#define REGF_TEST_ASSERT_LESS_THAN_INT( name, value, expected_value ) \
	if( value >= expected_value ) \
	{ \
		fprintf( stdout, "%s:%d %s >= %d\n", __FILE__, __LINE__, name, expected_value ); \
		goto on_error; \
	}

#define REGF_TEST_ASSERT_EQUAL_SIZE( name, value, expected_value ) \
	if( value != expected_value ) \
	{ \
		fprintf( stdout, "%s:%d %s != %" PRIzd "\n", __FILE__, __LINE__, name, expected_value ); \
		goto on_error; \
	}

#define REGF_TEST_ASSERT_EQUAL_SSIZE( name, value, expected_value ) \
	if( value != expected_value ) \
	{ \
		fprintf( stdout, "%s:%d %s != %" PRIzd "\n", __FILE__, __LINE__, name, expected_value ); \
		goto on_error; \
	}

#define REGF_TEST_ASSERT_EQUAL_INT32( name, value, expected_value ) \
	if( value != expected_value ) \
	{ \
		fprintf( stdout, "%s:%d %s != %" PRIi32 "\n", __FILE__, __LINE__, name, expected_value ); \
		goto on_error; \
	}

#define REGF_TEST_ASSERT_EQUAL_UINT32( name, value, expected_value ) \
	if( value != expected_value ) \
	{ \
		fprintf( stdout, "%s:%d %s != %" PRIu32 "\n", __FILE__, __LINE__, name, expected_value ); \
		goto on_error; \
	}

#define REGF_TEST_ASSERT_LESS_THAN_UINT32( name, value, expected_value ) \
	if( value >= expected_value ) \
	{ \
		fprintf( stdout, "%s:%d %s >= %" PRIu32 "\n", __FILE__, __LINE__, name, expected_value ); \
		goto on_error; \
	}

#define REGF_TEST_ASSERT_EQUAL_INT64( name, value, expected_value ) \
	if( value != expected_value ) \
	{ \
		fprintf( stdout, "%s:%d %s != %" PRIi64 "\n", __FILE__, __LINE__, name, expected_value ); \
		goto on_error; \
	}

#define REGF_TEST_ASSERT_EQUAL_UINT64( name, value, expected_value ) \
	if( value != expected_value ) \
	{ \
		fprintf( stdout, "%s:%d %s != %" PRIu64 "\n", __FILE__, __LINE__, name, expected_value ); \
		goto on_error; \
	}

#define REGF_TEST_ASSERT_LESS_THAN_UINT64( name, value, expected_value ) \
	if( value >= expected_value ) \
	{ \
		fprintf( stdout, "%s:%d %s >= %" PRIu64 "\n", __FILE__, __LINE__, name, expected_value ); \
		goto on_error; \
	}

#define REGF_TEST_ASSERT_IS_NOT_NULL( name, value ) \
	if( value == NULL ) \
	{ \
		fprintf( stdout, "%s:%d %s == NULL\n", __FILE__, __LINE__, name ); \
		goto on_error; \
	}

#define REGF_TEST_ASSERT_IS_NULL( name, value ) \
	if( value != NULL ) \
	{ \
		fprintf( stdout, "%s:%d %s != NULL\n", __FILE__, __LINE__, name ); \
		goto on_error; \
	}

#define REGF_TEST_RUN( name, function ) \
	if( function() != 1 ) \
	{ \
		fprintf( stdout, "Unable to run test: %s\n", name ); \
		goto on_error; \
	}

#if !defined( __BORLANDC__ ) || ( __BORLANDC__ >= 0x0560 )

#define REGF_TEST_RUN_WITH_ARGS( name, function, ... ) \
	if( function( __VA_ARGS__ ) != 1 ) \
	{ \
		fprintf( stdout, "Unable to run test: %s\n", name ); \
		goto on_error; \
	}

#endif /* !defined( __BORLANDC__ ) || ( __BORLANDC__ >= 0x0560 ) */

#define REGF_TEST_FPRINT_ERROR( error ) \
	libcerror_error_backtrace_fprint( error, stdout );

#endif /* !defined( _REGF_TEST_MACROS_H ) */

