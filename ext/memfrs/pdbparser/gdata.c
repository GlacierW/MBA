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
                printf("globalname %s\n", global->name.name);
	} else {
		READ(read_bytes, 1, data_size, global->name.size, data, unsigned char);
		init_scstring(&global->name, global->name.size, data);
	}

	return read_bytes;
}

void parse_gdata_stream(void *stream, R_STREAM_FILE *stream_file) {
        //printf("test parse gdata stream!!!!!\n");
        unsigned short len = 0;
	unsigned short leaf_type = 0;
	char *data = 0;
	SGDATAStream *data_stream = (SGDATAStream *) stream;
	SGlobal *global = 0;

	//data_stream->globals_list = r_list_new();
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
			//r_list_append(data_stream->globals_list, global);
                        DL_APPEND(data_stream->globals_list, global);
		}
		//free(data);
	}

}

void free_gdata_stream(void *stream)
{
}
