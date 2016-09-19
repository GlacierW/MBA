/*
This file is part of MBA PDB GData Stream Parser.
MBA PDB Gdata Stream Parser Implementation.
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

#include "types.h"
#include "gdata.h"
#include "stream_file.h"
#include "tpi.h"
#include "utlist.h"

static int parse_global(char *data, int data_size, SGlobal *global) {
	unsigned int read_bytes = 2;

	READ(read_bytes, 4, data_size, global->symtype, data, unsigned int);
	READ(read_bytes, 4, data_size, global->offset, data, unsigned int);
	READ(read_bytes, 2, data_size, global->segment, data, unsigned short);
	if (global->leaf_type == 0x110E) {
		parse_sctring(&global->name, (unsigned char *)data, &read_bytes, data_size);
	} else {
		READ(read_bytes, 1, data_size, global->name.size, data, unsigned char);
		init_scstring(&global->name, global->name.size, data);
	}

	return read_bytes;
}

void parse_gdata_stream(void *stream, R_STREAM_FILE *stream_file) {
        unsigned short len = 0;
	unsigned short leaf_type = 0;
	char *data = 0;
	SGDATAStream *data_stream = (SGDATAStream *) stream;
	SGlobal *global = 0;

        data_stream->globals_list = NULL;
	while (1) {
		stream_file_read(stream_file, 2, (char *)&len);
		if (len == 0)
			break;
		data = (char *) malloc(len);
		if (!data) return;
		stream_file_read(stream_file, len, data);

		leaf_type = *(unsigned short *) (data);
		if ((leaf_type == 0x110E) || (leaf_type == 0x1009)) {
			global = (SGlobal *) malloc(sizeof(SGlobal));
			if (!global) {
				free (data);
				return;
			}
			global->leaf_type = leaf_type;
			parse_global(data + 2, len, global);
                        DL_APPEND(data_stream->globals_list, global);
		}
	}

}

void free_gdata_stream(void *stream)
{
}
