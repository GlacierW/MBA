/*
This file is part of MBA PDB Parser, which used to parse DBI Stream.
MBA PDB DBI Parser Implementation.
This PDB parser is modified from RADARE2's repository.
Instead of using radare's API, we replace it with ut-serial library.

Copyright (c)   2016 ChongKuan Chen

This PDB Parser is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Foobar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h> 
#include <stdint.h>

#include "types.h"
#include "dbi.h"
#include "stream_file.h"
#include "tpi.h"
#include "utlist.h"

static void parse_dbi_header(SDBIHeader *dbi_header, R_STREAM_FILE *stream_file)
{
	stream_file_read(stream_file, sizeof(uint32_t), (char *)&dbi_header->magic);
	stream_file_read(stream_file, sizeof(uint32_t), (char *)&dbi_header->version);
	stream_file_read(stream_file, sizeof(uint32_t), (char *)&dbi_header->age);
	stream_file_read(stream_file, sizeof(uint16_t), (char *)&dbi_header->gssymStream);
	stream_file_read(stream_file, sizeof(uint16_t), (char *)&dbi_header->vers);
	stream_file_read(stream_file, sizeof(int16_t), (char *)&dbi_header->pssymStream);
	stream_file_read(stream_file, sizeof(uint16_t), (char *)&dbi_header->pdbver);
	stream_file_read(stream_file, sizeof(int16_t), (char *)&dbi_header->symrecStream);
	stream_file_read(stream_file, sizeof(uint16_t), (char *)&dbi_header->pdbver2);
	stream_file_read(stream_file, sizeof(uint32_t), (char *)&dbi_header->module_size);
	stream_file_read(stream_file, sizeof(uint32_t), (char *)&dbi_header->seccon_size);
	stream_file_read(stream_file, sizeof(uint32_t), (char *)&dbi_header->secmap_size);
	stream_file_read(stream_file, sizeof(uint32_t), (char *)&dbi_header->filinf_size);
	stream_file_read(stream_file, sizeof(uint32_t), (char *)&dbi_header->tsmap_size);
	stream_file_read(stream_file, sizeof(uint32_t), (char *)&dbi_header->mfc_index);
	stream_file_read(stream_file, sizeof(uint32_t), (char *)&dbi_header->dbghdr_size);
	stream_file_read(stream_file, sizeof(uint32_t), (char *)&dbi_header->ecinfo_size);
	stream_file_read(stream_file, sizeof(uint16_t), (char *)&dbi_header->flags);
	stream_file_read(stream_file, 2, (char *)&dbi_header->machine);
	stream_file_read(stream_file, sizeof(uint32_t), (char *)&dbi_header->resvd);
}

static int parse_ssymbol_range(char *data, int max_len, SSymbolRange *symbol_range)
{
	int read_bytes = 0;

	READ(read_bytes, 2, max_len, symbol_range->section, data, short);
	READ(read_bytes, 2, max_len, symbol_range->padding1, data, short);
	READ(read_bytes, 4, max_len, symbol_range->offset, data, int);
	READ(read_bytes, 4, max_len, symbol_range->size, data, int);
	READ(read_bytes, 4, max_len, symbol_range->flags, data, uint32_t);
	READ(read_bytes, 4, max_len, symbol_range->module, data, int);

// TODO: why not need to read this padding?
//	READ(read_bytes, 2, max_len, symbol_range->padding2, data, short);
	READ(read_bytes, 4, max_len, symbol_range->data_crc, data, uint32_t);
	READ(read_bytes, 4, max_len, symbol_range->reloc_crc, data, uint32_t);

	return read_bytes;
}

static int parse_dbi_ex_header(char *data, int max_len, SDBIExHeader *dbi_ex_header)
{
	uint32_t read_bytes = 0, before_read_bytes = 0;

	READ(read_bytes, 4, max_len, dbi_ex_header->opened, data, uint32_t);

	before_read_bytes = read_bytes;
	read_bytes += parse_ssymbol_range (data, max_len, &dbi_ex_header->range);
	data += (read_bytes - before_read_bytes);

	READ(read_bytes, 2, max_len, dbi_ex_header->flags, data, uint16_t);
	READ(read_bytes, 2, max_len, dbi_ex_header->stream, data, short);
	READ(read_bytes, 4, max_len, dbi_ex_header->symSize, data, uint32_t);
	READ(read_bytes, 4, max_len, dbi_ex_header->oldLineSize, data, uint32_t);
	READ(read_bytes, 4, max_len, dbi_ex_header->lineSize, data, uint32_t);
	READ(read_bytes, 2, max_len, dbi_ex_header->nSrcFiles, data, short);
	READ(read_bytes, 2, max_len, dbi_ex_header->padding1, data, short);
	READ(read_bytes, 4, max_len, dbi_ex_header->offsets, data, uint32_t);
	READ(read_bytes, 4, max_len, dbi_ex_header->niSource, data, uint32_t);
	READ(read_bytes, 4, max_len, dbi_ex_header->niCompiler, data, uint32_t);

	before_read_bytes = read_bytes;
	parse_sctring(&dbi_ex_header->modName, (unsigned char *)data, &read_bytes, max_len);
	data += (read_bytes - before_read_bytes);

	before_read_bytes = read_bytes;
	parse_sctring(&dbi_ex_header->objName, (unsigned char *)data, &read_bytes, max_len);
	data += (read_bytes - before_read_bytes);
	return read_bytes;
}

static void parse_dbg_header(SDbiDbgHeader *dbg_header, R_STREAM_FILE *stream_file)
{
	stream_file_read(stream_file, sizeof(short), (char *)&dbg_header->sn_fpo);
	stream_file_read(stream_file, sizeof(short), (char *)&dbg_header->sn_exception);
	stream_file_read(stream_file, sizeof(short), (char *)&dbg_header->sn_fixup);
	stream_file_read(stream_file, sizeof(short), (char *)&dbg_header->sn_omap_to_src);
	stream_file_read(stream_file, sizeof(short), (char *)&dbg_header->sn_omap_from_src);
	stream_file_read(stream_file, sizeof(short), (char *)&dbg_header->sn_section_hdr);
	stream_file_read(stream_file, sizeof(short), (char *)&dbg_header->sn_token_rid_map);
	stream_file_read(stream_file, sizeof(short), (char *)&dbg_header->sn_xdata);
	stream_file_read(stream_file, sizeof(short), (char *)&dbg_header->sn_pdata);
	stream_file_read(stream_file, sizeof(short), (char *)&dbg_header->sn_new_fpo);
	stream_file_read(stream_file, sizeof(short), (char *)&dbg_header->sn_section_hdr_orig);
}

UT_icd SDBIExHeader_icd = {sizeof(SDBIExHeader), NULL, NULL, NULL};
#define _ALIGN 4

void parse_dbi_stream(void *parsed_pdb_stream, R_STREAM_FILE *stream_file)
{
        SDbiStream *dbi_stream = (SDbiStream *) parsed_pdb_stream;
	SDBIExHeader *dbi_ex_header = 0;
	int pos = 0;
	char *dbiexhdr_data = 0, *p_tmp = 0;
	int size = 0, sz = 0;
	int i = 0;

	parse_dbi_header (&dbi_stream->dbi_header, stream_file);
	pos += sizeof (SDBIHeader) - 2;	// 2 because enum in C equal to 4, but
									// to read just 2;
	stream_file_seek (stream_file, pos, 0);

	size = dbi_stream->dbi_header.module_size;
	dbiexhdr_data = (char *) malloc(size);
	if (!dbiexhdr_data) return;
	stream_file_read (stream_file, size, dbiexhdr_data);

        dbi_stream->dbiexhdrs = NULL;
	p_tmp = dbiexhdr_data;
        
	while (i < size) {
		dbi_ex_header = (SDBIExHeader *) malloc (sizeof(SDBIExHeader));
		if (!dbi_ex_header) break;
		// TODO: rewrite for signature where can to do chech CAN_READ true?
		sz = parse_dbi_ex_header (p_tmp, size, dbi_ex_header);
                
                //temp add
                //break;             

		if ((sz % _ALIGN)) {
			sz = sz + (_ALIGN - (sz % _ALIGN));
		}
		i += sz;
		p_tmp += sz;
                //DL_APPEND(dbi_stream->dbiexhdrs, dbi_ex_header);
	}
        
	free (dbiexhdr_data);

	stream_file_seek(stream_file, dbi_stream->dbi_header.seccon_size, 1);
	stream_file_seek(stream_file, dbi_stream->dbi_header.secmap_size, 1);
	stream_file_seek(stream_file, dbi_stream->dbi_header.filinf_size, 1);
	stream_file_seek(stream_file, dbi_stream->dbi_header.tsmap_size, 1);
	stream_file_seek(stream_file, dbi_stream->dbi_header.ecinfo_size, 1);
	parse_dbg_header(&dbi_stream->dbg_header, stream_file);
        
}


void init_dbi_stream(SDbiStream *dbi_stream)
{
}

