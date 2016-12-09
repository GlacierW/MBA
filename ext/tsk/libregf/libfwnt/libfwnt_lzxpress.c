/*
 * LZXPRESS (de)compression functions
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
#include <byte_stream.h>
#include <memory.h>
#include <types.h>

#include "libfwnt_bit_stream.h"
#include "libfwnt_libcerror.h"
#include "libfwnt_libcnotify.h"
#include "libfwnt_lzxpress.h"

/* Compresses data using LZXPRESS (LZ77 + DIRECT2) compression
 * Returns 1 on success or -1 on error
 */
int libfwnt_lzxpress_compress(
     const uint8_t *uncompressed_data,
     size_t uncompressed_data_size,
     uint8_t *compressed_data,
     size_t *compressed_data_size,
     libcerror_error_t **error )
{
	static char *function = "libfwnt_lzxpress_compress";

	if( uncompressed_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid uncompressed data.",
		 function );

		return( -1 );
	}
	if( uncompressed_data_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid uncompressed data size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( compressed_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid compressed data.",
		 function );

		return( -1 );
	}
	if( compressed_data_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid compressed data size.",
		 function );

		return( -1 );
	}
/* TODO implement */
	return( -1 );
}

/* Decompresses data using LZXPRESS (LZ77 + DIRECT2) compression
 * Return 1 on success or -1 on error
 */
int libfwnt_lzxpress_decompress(
     const uint8_t *compressed_data,
     size_t compressed_data_size,
     uint8_t *uncompressed_data,
     size_t *uncompressed_data_size,
     libcerror_error_t **error )
{
	static char *function                  = "libfwnt_lzxpress_decompress";
	size_t compressed_data_offset          = 0;
	size_t compression_index               = 0;
	size_t compression_shared_byte_index   = 0;
	size_t uncompressed_data_offset        = 0;
	uint32_t compression_indicator         = 0;
	uint32_t compression_indicator_bitmask = 0;
	uint16_t compression_tuple             = 0;
	uint16_t compression_tuple_size        = 0;
	int16_t compression_tuple_offset       = 0;

	if( compressed_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid compressed data.",
		 function );

		return( -1 );
	}
	if( compressed_data_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid compressed data size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( uncompressed_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid uncompressed data.",
		 function );

		return( -1 );
	}
	if( uncompressed_data_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid uncompressed data size.",
		 function );

		return( -1 );
	}
	if( compressed_data_size <= 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
		 "%s: compressed data size value too small.",
		 function );

		return( -1 );
	}
	while( compressed_data_offset < compressed_data_size )
	{
		if( uncompressed_data_offset >= *uncompressed_data_size )
		{
			break;
		}
		byte_stream_copy_to_uint32_little_endian(
		 &( compressed_data[ compressed_data_offset ] ),
		 compression_indicator );

#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: compressed data offset\t\t\t: %" PRIzd " (0x%08" PRIzx ")\n",
			 function,
			 compressed_data_offset,
			 compressed_data_offset );

			libcnotify_printf(
			 "%s: compression indicator\t\t\t: 0x%08" PRIx32 "\n",
			 function,
			 compression_indicator );

			libcnotify_printf(
			 "\n" );
		}
#endif
		compressed_data_offset += 4;

		for( compression_indicator_bitmask = 0x80000000UL;
		     compression_indicator_bitmask > 0;
		     compression_indicator_bitmask >>= 1 )
		{
			if( uncompressed_data_offset >= *uncompressed_data_size )
			{
				break;
			}
			if( compressed_data_offset >= compressed_data_size )
			{
				break;
			}
			/* If the indicator bit is 0 the data is uncompressed
			 * or 1 if the data is compressed
			 */
			if( ( compression_indicator & compression_indicator_bitmask ) != 0 )
			{
				if( compressed_data_offset >= ( compressed_data_size - 1 ) )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
					 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
					 "%s: compressed data too small.",
					 function );

					return( -1 );
				}
		                byte_stream_copy_to_uint16_little_endian(
		                 &( compressed_data[ compressed_data_offset ] ),
		                 compression_tuple );

				compressed_data_offset += 2;

				/* The compression tuple contains:
				 * 0 - 2	the size
				 * 3 - 15	the offset - 1
				 */
				compression_tuple_size   = ( compression_tuple & 0x0007 );
				compression_tuple_offset = ( compression_tuple >> 3 ) + 1;

				/* Check for a first level extended size
				 * stored in the 4-bits of a shared extended compression tuple size byte
				 * the size is added to the previous size
				 */
				if( compression_tuple_size == 0x07 )
				{
					if( compression_shared_byte_index == 0 )
					{
						if( compressed_data_offset >= compressed_data_size )
						{
							libcerror_error_set(
							 error,
							 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
							 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
							 "%s: compressed data too small.",
							 function );

							return( -1 );
						}
						compression_tuple_size += compressed_data[ compressed_data_offset ] & 0x0f;

						compression_shared_byte_index = compressed_data_offset++;
					}
					else
					{
						compression_tuple_size += compressed_data[ compression_shared_byte_index ] >> 4;

						compression_shared_byte_index = 0;
					}
				}
				/* Check for a second level extended size
				 * stored in the 8-bits of the next byte
				 * the size is added to the previous size
				 */
				if( compression_tuple_size == ( 0x07 + 0x0f ) )
				{
					if( compressed_data_offset >= compressed_data_size )
					{
						libcerror_error_set(
						 error,
						 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
						 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
						 "%s: compressed data too small.",
						 function );

						return( -1 );
					}
					compression_tuple_size += compressed_data[ compressed_data_offset++ ];
				}
				/* Check for a third level extended size
				 * stored in the 16-bits of the next two bytes
				 * the previous size is ignored
				 */
				if( compression_tuple_size == ( 0x07 + 0x0f + 0xff ) )
				{
					if( compressed_data_offset >= ( compressed_data_size - 1 ) )
					{
						libcerror_error_set(
						 error,
						 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
						 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
						 "%s: compressed data too small.",
						 function );

						return( -1 );
					}
			                byte_stream_copy_to_uint16_little_endian(
			                 &( compressed_data[ compressed_data_offset ] ),
			                 compression_tuple_size );
		
					compressed_data_offset += 2;

				}
				/* The size value is stored as
				 * size - 3
				 */
				compression_tuple_size += 3;

#if defined( HAVE_DEBUG_OUTPUT )
				if( libcnotify_verbose != 0 )
				{
					libcnotify_printf(
					 "%s: compressed data offset\t\t\t: %" PRIzd " (0x%08" PRIzx ")\n",
					 function,
					 compressed_data_offset,
					 compressed_data_offset );

					libcnotify_printf(
					 "%s: compression tuple offset\t\t\t: %" PRIi16 "\n",
					 function,
					 compression_tuple_offset );

					libcnotify_printf(
					 "%s: compression tuple size\t\t\t: %" PRIu16 "\n",
					 function,
					 compression_tuple_size );

					libcnotify_printf(
					 "%s: uncompressed data offset\t\t\t: %" PRIzd "\n",
					 function,
					 uncompressed_data_offset );

					libcnotify_printf(
					 "\n" );
				}
#endif
				if( compression_tuple_size > 32771 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
					 "%s: compression tuple size value out of bounds.",
					 function );

					return( -1 );
				}
				compression_index = uncompressed_data_offset - compression_tuple_offset;

				while( compression_tuple_size > 0 )
				{
					if( compression_index > uncompressed_data_offset )
					{
						libcerror_error_set(
						 error,
						 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
						 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
						 "%s: invalid compressed data at offset: %" PRIzd " - compression index: %" PRIzd " out of range: %" PRIzd ".",
						 function,
						 compressed_data_offset,
						 compression_index,
						 uncompressed_data_offset );

						return( -1 );
					}
					if( uncompressed_data_offset > *uncompressed_data_size )
					{
						libcerror_error_set(
						 error,
						 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
						 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
						 "%s: uncompressed data too small.",
						 function );

						return( -1 );
					}
					uncompressed_data[ uncompressed_data_offset++ ] = uncompressed_data[ compression_index++ ];

					compression_tuple_size--;
				}
			}
			else
			{
				if( compressed_data_offset >= compressed_data_size )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
					 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
					 "%s: compressed data too small.",
					 function );

					return( -1 );
				}
				if( uncompressed_data_offset > *uncompressed_data_size )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
					 LIBCERROR_ARGUMENT_ERROR_VALUE_TOO_SMALL,
					 "%s: uncompressed data too small.",
					 function );

					return( -1 );
				}
				uncompressed_data[ uncompressed_data_offset++ ] = compressed_data[ compressed_data_offset++ ];
			}
		}
	}
	*uncompressed_data_size = uncompressed_data_offset;

	return( 1 );
}

/* Copmares two Huffman code symbols
 * Returns 1 if the first symbol is larger than the second, 0 if equal or -1 if smaller
 */
int libfwnt_lzxpress_huffman_compare_symbols(
     libfwnt_lzxpress_huffman_code_symbol_t *first_symbol,
     libfwnt_lzxpress_huffman_code_symbol_t *second_symbol )
{
	if( first_symbol == NULL )
	{
		return( -1 );
	}
	if( second_symbol == NULL )
	{
		return( 1 );
	}
	if( first_symbol->code_size < second_symbol->code_size )
	{
		return( -1 );
	}
	else if( first_symbol->code_size > second_symbol->code_size )
	{
		return( 1 );
	}
	else if( first_symbol->symbol < second_symbol->symbol )
	{
		return( -1 );
	}
	else if( first_symbol->symbol > second_symbol->symbol )
	{
		return( 1 );
	}
	return( 0 );
}

/* Adds a leaf node to the Huffman tree
 * Returns 1 on success or -1 on error
 */
int libfwnt_lzxpress_huffman_tree_add_leaf(
     libfwnt_lzxpress_huffman_tree_node_t *tree_nodes,
     int tree_node_index,
     uint32_t bits,
     uint8_t number_of_bits )
{
	libfwnt_lzxpress_huffman_tree_node_t *tree_node = NULL;
	uint8_t sub_tree_node_index                     = 0;
	int next_tree_node_index                        = 0;

	tree_node            = &( tree_nodes[ 0 ] );
	next_tree_node_index = tree_node_index + 1;

	while( number_of_bits > 1 )
	{
		number_of_bits--;

		sub_tree_node_index = (uint8_t) ( bits >> number_of_bits ) & 0x00000001UL;

		if( tree_node->sub_tree_nodes[ sub_tree_node_index ] == NULL )
		{
			tree_node->sub_tree_nodes[ sub_tree_node_index ] = &( tree_nodes[ next_tree_node_index ] );
			tree_nodes[ next_tree_node_index ].is_leaf       = 0;

#if defined( HAVE_DEBUG_OUTPUT )
			tree_node->sub_tree_node_indexes[ sub_tree_node_index ] = next_tree_node_index;
#endif

			next_tree_node_index++;
		}
		tree_node = tree_node->sub_tree_nodes[ sub_tree_node_index ];
	}
/* TODO store index instead of pointer ? */
	tree_node->sub_tree_nodes[ bits & 0x00000001UL ] = &( tree_nodes[ tree_node_index ] );

#if defined( HAVE_DEBUG_OUTPUT )
	tree_node->sub_tree_node_indexes[ bits & 0x00000001UL ] = tree_node_index;
#endif

	return( next_tree_node_index );
}

/* Reads the Huffman tree
 * Returns 1 on success or -1 on error
 */
int libfwnt_lzxpress_huffman_tree_read(
     libfwnt_lzxpress_huffman_tree_node_t *tree_nodes,
     const uint8_t *compressed_data,
     size_t compressed_data_size,
     size_t compressed_data_offset,
     libcerror_error_t **error )
{
	libfwnt_lzxpress_huffman_code_symbol_t code_symbols[ 512 ];

	static char *function  = "libfwnt_lzxpress_huffman_tree_read";
	size_t byte_index      = 0;
	uint32_t bits          = 0;
	uint16_t symbol_index  = 0;
	uint8_t byte_value     = 0;
	uint8_t number_of_bits = 0;
	int tree_node_index    = 0;

	if( compressed_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid compressed data.",
		 function );

		return( -1 );
	}
	if( compressed_data_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid compressed data size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( ( compressed_data_offset >= compressed_data_size )
	 || ( ( compressed_data_size - compressed_data_offset ) < 256 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: compressed data offset value out of bounds.",
		 function );

		return( -1 );
	}
	if( memory_set(
	     tree_nodes,
	     0,
	     sizeof( libfwnt_lzxpress_huffman_tree_node_t ) * 1024 ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear tree nodes.",
		 function );

		return( -1 );
	}
	/* The initial table contains a 4-bits code size per symbol
	 */
	for( byte_index = 0;
	     byte_index < 256;
	     byte_index++ )
	{
		byte_value = compressed_data[ compressed_data_offset++ ];

		code_symbols[ symbol_index ].symbol    = symbol_index;
		code_symbols[ symbol_index ].code_size = (uint16_t) ( byte_value & 0x0f );

		symbol_index++;

		byte_value >>= 4;

		code_symbols[ symbol_index ].symbol    = symbol_index;
		code_symbols[ symbol_index ].code_size = (uint16_t) ( byte_value & 0x0f );

		symbol_index++;
	}
	/* Sort the symbols
	 */
	qsort(
	 code_symbols,
	 512,
	 sizeof( libfwnt_lzxpress_huffman_code_symbol_t ),
	 (int (*)(const void *, const void *)) &libfwnt_lzxpress_huffman_compare_symbols );

	/* Find the first symbol with a code size > 0
	 */
	for( symbol_index = 0;
	     symbol_index < 512;
	     symbol_index++ )
	{
		if( code_symbols[ symbol_index ].code_size > 0 )
		{
			break;
		}
	}
	bits           = 0;
	number_of_bits = 1;

	tree_nodes[ tree_node_index ].is_leaf = 0;

	tree_node_index++;

	while( symbol_index < 512 )
	{
		tree_nodes[ tree_node_index ].symbol  = code_symbols[ symbol_index ].symbol;
		tree_nodes[ tree_node_index ].is_leaf = 1;

		bits         <<= code_symbols[ symbol_index ].code_size - number_of_bits;
		number_of_bits = (uint8_t) code_symbols[ symbol_index ].code_size;

		tree_node_index = libfwnt_lzxpress_huffman_tree_add_leaf(
		                   tree_nodes,
		                   tree_node_index,
		                   bits,
		                   number_of_bits );

		bits++;
		symbol_index++;
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		for( tree_node_index = 0;
		     tree_node_index < 1024;
		     tree_node_index++ )
		{
			libcnotify_printf(
			 "%s: tree node: %04d symbol\t\t: 0x%04" PRIx16 "\n",
			 function,
			 tree_node_index,
			 tree_nodes[ tree_node_index ].symbol );

			libcnotify_printf(
			 "%s: tree node: %04d is leaf\t\t: %" PRId8 "\n",
			 function,
			 tree_node_index,
			 tree_nodes[ tree_node_index ].is_leaf );

			libcnotify_printf(
			 "%s: tree node: %04d sub node: 0\t\t: %" PRId16 "\n",
			 function,
			 tree_node_index,
			 tree_nodes[ tree_node_index ].sub_tree_node_indexes[ 0 ] );

			libcnotify_printf(
			 "%s: tree node: %04d sub node: 1\t\t: %" PRId16 "\n",
			 function,
			 tree_node_index,
			 tree_nodes[ tree_node_index ].sub_tree_node_indexes[ 1 ] );
		}
		libcnotify_printf(
		 "\n" );

		for( symbol_index = 0;
		     symbol_index < 512;
		     symbol_index++ )
		{
			libcnotify_printf(
			 "%s: symbol: %03d symbol\t\t\t: 0x%04" PRIx16 "\n",
			 function,
			 symbol_index,
			 code_symbols[ symbol_index ].symbol );

			libcnotify_printf(
			 "%s: symbol: %03d code size\t\t: %" PRIu16 "\n",
			 function,
			 symbol_index,
			 code_symbols[ symbol_index ].code_size );
		}
		libcnotify_printf(
		 "\n" );
	}
#endif
	return( 1 );
}

/* Reads a Huffman tree symbol
 * Returns 1 on success or -1 on error
 */
int libfwnt_lzxpress_huffman_tree_read_symbol(
     libfwnt_lzxpress_huffman_tree_node_t *tree_nodes,
     libfwnt_bit_stream_t *compressed_data_bit_stream,
     uint16_t *symbol,
     libcerror_error_t **error )
{
	libfwnt_lzxpress_huffman_tree_node_t *tree_node = NULL;
	static char *function                           = "libfwnt_lzxpress_huffman_tree_read_symbol";
	uint16_t bits                                   = 0;
	uint8_t sub_tree_node_index                     = 0;

	if( compressed_data_bit_stream == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid compressed data bit stream.",
		 function );

		return( -1 );
	}
	if( symbol == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid symbol.",
		 function );

		return( -1 );
	}
	tree_node = &( tree_nodes[ 0 ] );

	do
	{
/* TODO add a read number of bits function ? */
		sub_tree_node_index = (uint8_t) ( ( compressed_data_bit_stream->bits >> 31 ) & 0x00000001UL );

		compressed_data_bit_stream->bits          <<= 1;
		compressed_data_bit_stream->number_of_bits -= 1;

#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: sub tree node index\t\t: 0x%02" PRIx8 "\n",
			 function,
			 sub_tree_node_index );
		}
#endif
		if( ( compressed_data_bit_stream->number_of_bits < 16 )
		 && ( compressed_data_bit_stream->byte_stream_offset <= ( compressed_data_bit_stream->byte_stream_size - 2 ) ) )
		{
			byte_stream_copy_to_uint16_little_endian(
			 &( compressed_data_bit_stream->byte_stream[ compressed_data_bit_stream->byte_stream_offset ] ),
			 bits );

			compressed_data_bit_stream->bits               |= bits << ( 16 - compressed_data_bit_stream->number_of_bits );
			compressed_data_bit_stream->byte_stream_offset += 2;
			compressed_data_bit_stream->number_of_bits     += 16;
		}
		tree_node = tree_node->sub_tree_nodes[ sub_tree_node_index ];

		if( tree_node == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing tree node value.",
			 function );

			return( -1 );
		}
	}
	while( tree_node->is_leaf == 0 );

	*symbol = tree_node->symbol;

	return( 1 );
}

/* Decompresses a LZXPRESS Huffman compressed chunk
 * Return 1 on success or -1 on error
 */
int libfwnt_lzxpress_huffman_decompress_chunk(
     const uint8_t *compressed_data,
     size_t compressed_data_size,
     size_t *compressed_data_offset,
     uint8_t *uncompressed_data,
     size_t uncompressed_data_size,
     size_t *uncompressed_data_offset,
     libcerror_error_t **error )
{
	libfwnt_lzxpress_huffman_tree_node_t tree_nodes[ 1024 ];

	libfwnt_bit_stream_t *compressed_data_bit_stream = NULL;
	static char *function                            = "libfwnt_lzxpress_huffman_decompress_chunk";
	size_t compression_uncompressed_data_offset      = 0;
	size_t next_chunk_uncompressed_data_offset       = 0;
	size_t safe_uncompressed_data_offset             = 0;
	uint32_t compression_offset                      = 0;
	uint16_t bits                                    = 0;
	uint16_t compression_size                        = 0;
	uint16_t symbol                                  = 0;

	if( compressed_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid compressed data.",
		 function );

		return( -1 );
	}
	if( compressed_data_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid compressed data size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( compressed_data_offset == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid compressed data offset.",
		 function );

		return( -1 );
	}
	if( ( *compressed_data_offset >= compressed_data_size )
	 || ( ( compressed_data_size - *compressed_data_offset ) < 260 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: compressed data offset value out of bounds.",
		 function );

		return( -1 );
	}
	if( uncompressed_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid uncompressed data.",
		 function );

		return( -1 );
	}
	if( uncompressed_data_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid uncompressed data size value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( uncompressed_data_offset == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid uncompressed data offset.",
		 function );

		return( -1 );
	}
	if( *uncompressed_data_offset > uncompressed_data_size )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: uncompressed data offset value out of bounds.",
		 function );

		return( -1 );
	}
	if( libfwnt_lzxpress_huffman_tree_read(
	     tree_nodes,
	     compressed_data,
	     compressed_data_size,
	     *compressed_data_offset,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to read Huffman tree.",
		 function );

		goto on_error;
	}
	if( libfwnt_bit_stream_initialize(
	     &compressed_data_bit_stream,
	     &( compressed_data[ *compressed_data_offset ] ),
	     compressed_data_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create compressed data bit stream.",
		 function );

		goto on_error;
	}
/* TODO add a bit stream seek function ? */
	compressed_data_bit_stream->byte_stream_offset = 256;

	if( libfwnt_bit_stream_read(
	     compressed_data_bit_stream,
	     4,
	     error ) != 4 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read 32-bit from bit stream.",
		 function );

		goto on_error;
	}
	safe_uncompressed_data_offset = *uncompressed_data_offset;

	next_chunk_uncompressed_data_offset = safe_uncompressed_data_offset + 65536;

	if( next_chunk_uncompressed_data_offset > uncompressed_data_size )
	{
		next_chunk_uncompressed_data_offset = uncompressed_data_size;
	}
        while( ( compressed_data_bit_stream->byte_stream_offset < compressed_data_bit_stream->byte_stream_size )
            || ( compressed_data_bit_stream->number_of_bits > 0 ) )
	{
		if( safe_uncompressed_data_offset >= next_chunk_uncompressed_data_offset )
		{
			break;
		}
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: compressed data offset\t: %" PRIzd " (0x%08" PRIzx ")\n",
			 function,
			 compressed_data_bit_stream->byte_stream_offset,
			 compressed_data_bit_stream->byte_stream_offset );
		}
#endif
		if( libfwnt_lzxpress_huffman_tree_read_symbol(
		     tree_nodes,
		     compressed_data_bit_stream,
		     &symbol,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to read symbol.",
			 function );

			goto on_error;
		}
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: number of bits\t\t: %" PRId8 "\n",
			 function,
			 compressed_data_bit_stream->number_of_bits );

			libcnotify_printf(
			 "%s: huffman symbol\t\t: 0x%04" PRIx16 "\n",
			 function,
			 symbol );
		}
#endif
		if( ( compressed_data_bit_stream->number_of_bits < 16 )
		 && ( compressed_data_bit_stream->byte_stream_offset <= ( compressed_data_bit_stream->byte_stream_size - 2 ) ) )
		{
			byte_stream_copy_to_uint16_little_endian(
			 &( compressed_data_bit_stream->byte_stream[ compressed_data_bit_stream->byte_stream_offset ] ),
			 bits );

/* TODO bits read infront of the bit stream ? */
			compressed_data_bit_stream->bits               |= bits << ( 16 - compressed_data_bit_stream->number_of_bits );
			compressed_data_bit_stream->byte_stream_offset += 2;
			compressed_data_bit_stream->number_of_bits     += 16;
		}
		if( symbol < 256 )
		{
			uncompressed_data[ safe_uncompressed_data_offset++ ] = (uint8_t) symbol;
		}
		/* Check if we have an end-of-block marker (remaining bits are 0)
		 */
		if( ( compressed_data_bit_stream->bits == 0 )
		 && ( safe_uncompressed_data_offset >= uncompressed_data_size ) )
		{
			break;
		}
		if( symbol >= 256 )
		{
			symbol          -= 256;
			compression_size = symbol & 0x000f;
			symbol         >>= 4;

			if( symbol == 0 )
			{
				compression_offset = 0;
			}
			else
			{
/* TODO add a read number of bits function ? */
				compression_offset = (uint32_t) ( compressed_data_bit_stream->bits >> ( 32 - symbol ) );
			}
			compression_offset = (uint32_t) ( ( 1 << symbol ) | compression_offset );

			compressed_data_bit_stream->bits          <<= symbol;
			compressed_data_bit_stream->number_of_bits -= (uint8_t) symbol;

			/* Ignore any data beyond the uncompressed block size
			 */
			if( compression_size == 15 )
			{
				if( compressed_data_bit_stream->byte_stream_offset > ( compressed_data_bit_stream->byte_stream_size - 1 ) )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
					 "%s: compressed data size value too small.",
					 function );

					goto on_error;
				}
				compression_size = compressed_data_bit_stream->byte_stream[ compressed_data_bit_stream->byte_stream_offset ] + 15;

				compressed_data_bit_stream->byte_stream_offset += 1;

				if( compression_size == 270 )
				{
					if( compressed_data_bit_stream->byte_stream_offset > ( compressed_data_bit_stream->byte_stream_size - 2 ) )
					{
						libcerror_error_set(
						 error,
						 LIBCERROR_ERROR_DOMAIN_RUNTIME,
						 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
						 "%s: compressed data size value too small.",
						 function );

						goto on_error;
					}
					byte_stream_copy_to_uint16_little_endian(
					 &( compressed_data_bit_stream->byte_stream[ compressed_data_bit_stream->byte_stream_offset ] ),
					 compression_size );

					compressed_data_bit_stream->byte_stream_offset += 2;
				}
			}
			compression_size += 3;

#if defined( HAVE_DEBUG_OUTPUT )
			if( libcnotify_verbose != 0 )
			{
				libcnotify_printf(
				 "%s: compression offset\t\t: %" PRIi16 "\n",
				 function,
				 compression_offset );

				libcnotify_printf(
				 "%s: compression size\t\t: %" PRIu16 "\n",
				 function,
				 compression_size );

				libcnotify_printf(
				 "%s: uncompressed data offset\t: %" PRIzd "\n",
				 function,
				 safe_uncompressed_data_offset );
			}
#endif
			if( compression_offset > safe_uncompressed_data_offset )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: compression offset value out of bounds.",
				 function );

				goto on_error;
			}
			if( compression_size > ( uncompressed_data_size - safe_uncompressed_data_offset ) )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
				 "%s: compression size value out of bounds.",
				 function );

				goto on_error;
			}
			compression_uncompressed_data_offset = safe_uncompressed_data_offset - compression_offset;

			while( compression_size > 0 )
			{
				uncompressed_data[ safe_uncompressed_data_offset++ ] = uncompressed_data[ compression_uncompressed_data_offset++ ];

				compression_size--;
			}
			if( ( compressed_data_bit_stream->number_of_bits < 16 )
			 && ( compressed_data_bit_stream->byte_stream_offset <= ( compressed_data_bit_stream->byte_stream_size - 2 ) ) )
			{
				byte_stream_copy_to_uint16_little_endian(
				 &( compressed_data_bit_stream->byte_stream[ compressed_data_bit_stream->byte_stream_offset ] ),
				 bits );

/* TODO bits read infront of the bit stream ? */
				compressed_data_bit_stream->bits               |= bits << ( 16 - compressed_data_bit_stream->number_of_bits );
				compressed_data_bit_stream->byte_stream_offset += 2;
				compressed_data_bit_stream->number_of_bits     += 16;
			}
		}
#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "\n" );
		}
#endif
	}
	*compressed_data_offset += compressed_data_bit_stream->byte_stream_offset;

	if( libfwnt_bit_stream_free(
	     &compressed_data_bit_stream,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
		 "%s: unable to free compressed data bit stream.",
		 function );

		goto on_error;
	}
	*uncompressed_data_offset = safe_uncompressed_data_offset;

	return( 1 );

on_error:
	if( compressed_data_bit_stream != NULL )
	{
		libfwnt_bit_stream_free(
		 &compressed_data_bit_stream,
		 NULL );
	}
	return( -1 );
}

/* Decompresses data using LZXPRESS Huffman compression
 * Return 1 on success or -1 on error
 */
int libfwnt_lzxpress_huffman_decompress(
     const uint8_t *compressed_data,
     size_t compressed_data_size,
     uint8_t *uncompressed_data,
     size_t *uncompressed_data_size,
     libcerror_error_t **error )
{
	static char *function           = "libfwnt_lzxpress_huffman_decompress";
	size_t compressed_data_offset   = 0;
	size_t uncompressed_data_offset = 0;

	if( uncompressed_data_size == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid uncompressed data size.",
		 function );

		return( -1 );
	}
	while( compressed_data_offset < compressed_data_size )
	{
		if( uncompressed_data_offset >= *uncompressed_data_size )
		{
			break;
		}
		if( libfwnt_lzxpress_huffman_decompress_chunk(
		     compressed_data,
		     compressed_data_size,
		     &compressed_data_offset,
		     uncompressed_data,
		     *uncompressed_data_size,
		     &uncompressed_data_offset,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_COMPRESSION,
			 LIBCERROR_COMPRESSION_ERROR_DECOMPRESS_FAILED,
			 "%s: unable to decompress chunk.",
			 function );

			return( -1 );
		}
	}
	*uncompressed_data_size = uncompressed_data_offset;

	return( 1 );
}

