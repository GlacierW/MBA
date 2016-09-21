/*
MBA PDB PE File Stream Parser Implementation.
This file is part of MBA PDB Parser.
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
#include "stream_pe.h"
#include "stream_file.h"


UT_icd sctn_header_icd = {sizeof(SIMAGE_SECTION_HEADER), NULL, NULL, NULL};

///////////////////////////////////////////////////////////////////////////////
void parse_pe_stream(void *stream, R_STREAM_FILE *stream_file)
{
	int data_size = 0;
	char *data = 0, *ptmp = 0;
	int read_bytes = 0;
	SIMAGE_SECTION_HEADER *sctn_header = 0;
	SPEStream *pe_stream = (SPEStream *) stream;
	int sctn_header_size =0;

	stream_file_get_size(stream_file, &data_size);
	data = (char *) malloc(data_size);
	if (!data) return;
	stream_file_get_data(stream_file, data);

	sctn_header_size = sizeof(SIMAGE_SECTION_HEADER);
	ptmp = data;
        utarray_new(pe_stream->sections_hdrs, &sctn_header_icd);

	while (read_bytes < data_size) {
		sctn_header = (SIMAGE_SECTION_HEADER *) malloc (sctn_header_size);
		if (!sctn_header) break;
		memcpy(sctn_header, ptmp, sctn_header_size);
		ptmp += sctn_header_size;
                utarray_push_back(pe_stream->sections_hdrs, sctn_header);
		read_bytes += sctn_header_size;
	}

	free(data);
}

void free_pe_stream(void *stream)
{
}
