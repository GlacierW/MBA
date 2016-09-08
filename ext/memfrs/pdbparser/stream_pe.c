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
        printf("start parse pe_stream\n");

	stream_file_get_size(stream_file, &data_size);
	data = (char *) malloc(data_size);
	if (!data) return;
	stream_file_get_data(stream_file, data);

	sctn_header_size = sizeof(SIMAGE_SECTION_HEADER);
	ptmp = data;
	//pe_stream->sections_hdrs = r_list_new();
        utarray_new(pe_stream->sections_hdrs, &sctn_header_icd);

	while (read_bytes < data_size) {
		sctn_header = (SIMAGE_SECTION_HEADER *) malloc (sctn_header_size);
		if (!sctn_header) break;
		memcpy(sctn_header, ptmp, sctn_header_size);
		ptmp += sctn_header_size;
		//r_list_append(pe_stream->sections_hdrs, sctn_header);
                utarray_push_back(pe_stream->sections_hdrs, sctn_header);
		read_bytes += sctn_header_size;
	}

	free(data);
}

void free_pe_stream(void *stream)
{
}
