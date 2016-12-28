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

#if !defined( _LIBFWNT_LZXPRESS_H )
#define _LIBFWNT_LZXPRESS_H

#include <common.h>
#include <types.h>

#include "libfwnt_bit_stream.h"
#include "libfwnt_extern.h"
#include "libfwnt_libcerror.h"

#if defined( __cplusplus )
extern "C" {
#endif

typedef struct libfwnt_lzxpress_huffman_tree_node libfwnt_lzxpress_huffman_tree_node_t;

struct libfwnt_lzxpress_huffman_tree_node 
{
	/* The symbol
	 */
	uint16_t symbol;

	/* Value to indicate the tree node is a leaf node
	 */
	uint8_t is_leaf;

	/* The sub tree nodes
	 */
	libfwnt_lzxpress_huffman_tree_node_t *sub_tree_nodes[ 2 ];

#if defined( HAVE_DEBUG_OUTPUT )

	/* The sub tree node indexes
	 */
	uint16_t sub_tree_node_indexes[ 2 ];
#endif
};

typedef struct libfwnt_lzxpress_huffman_code_symbol libfwnt_lzxpress_huffman_code_symbol_t;

struct libfwnt_lzxpress_huffman_code_symbol
{
	/* The symbol
	 */
	uint16_t symbol;

	/* The code size 
	 */
	uint16_t code_size;
};

LIBFWNT_EXTERN \
int libfwnt_lzxpress_compress(
     const uint8_t *uncompressed_data,
     size_t uncompressed_data_size,
     uint8_t *compressed_data,
     size_t *compressed_data_size,
     libcerror_error_t **error );

LIBFWNT_EXTERN \
int libfwnt_lzxpress_decompress(
     const uint8_t *compressed_data,
     size_t compressed_data_size,
     uint8_t *uncompressed_data,
     size_t *uncompressed_data_size,
     libcerror_error_t **error );

int libfwnt_lzxpress_huffman_compare_symbols(
     libfwnt_lzxpress_huffman_code_symbol_t *first_symbol,
     libfwnt_lzxpress_huffman_code_symbol_t *second_symbol );

int libfwnt_lzxpress_huffman_tree_add_leaf(
     libfwnt_lzxpress_huffman_tree_node_t *tree_nodes,
     int tree_node_index,
     uint32_t bits,
     uint8_t number_of_bits );

int libfwnt_lzxpress_huffman_tree_read(
     libfwnt_lzxpress_huffman_tree_node_t *tree_nodes,
     const uint8_t *compressed_data,
     size_t compressed_data_size,
     size_t compressed_data_offset,
     libcerror_error_t **error );

int libfwnt_lzxpress_huffman_tree_read_symbol(
     libfwnt_lzxpress_huffman_tree_node_t *tree_nodes,
     libfwnt_bit_stream_t *compressed_data_bit_stream,
     uint16_t *symbol,
     libcerror_error_t **error );

int libfwnt_lzxpress_huffman_decompress_chunk(
     const uint8_t *compressed_data,
     size_t compressed_data_size,
     size_t *compressed_data_offset,
     uint8_t *uncompressed_data,
     size_t uncompressed_data_size,
     size_t *uncompressed_data_offset,
     libcerror_error_t **error );

LIBFWNT_EXTERN \
int libfwnt_lzxpress_huffman_decompress(
     const uint8_t *compressed_data,
     size_t compressed_data_size,
     uint8_t *uncompressed_data,
     size_t *uncompressed_data_size,
     libcerror_error_t **error );

#if defined( __cplusplus )
}
#endif

#endif /* !defined( _LIBFWNT_LZXPRESS_H ) */

