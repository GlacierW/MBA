#include "stdlib.h"
#include "stdio.h"

#include "types.h"
#include "omap.h"
#include "stream_file.h"

void parse_omap_stream(void *stream, R_STREAM_FILE *stream_file)
{
        printf("[parse_omap_stream]\n");
}

void free_omap_stream(void *stream)
{
}

int omap_remap(void *stream, int address)
{
        SOmapStream *omap_stream = (SOmapStream *) stream;
	SOmapEntry *omap_entry = 0;
	//RListIter *it = 0;
	int i = 0;
	int pos = 0;
	int len = 0;

	if (!omap_stream) {
		return address;
	}

        /* Not yet implement
	len = r_list_length(omap_stream->omap_entries);

	if (omap_stream->froms == 0) {
		omap_stream->froms = (unsigned int *) malloc(4 * len);
		it = r_list_iterator(omap_stream->omap_entries);
		while (r_list_iter_next(it)) {
			omap_entry = (SOmapEntry *) r_list_iter_get(it);
			omap_stream->froms[i] = omap_entry->from;
			i++;
		}
	}

	// mb (len -1) ???
	pos = binary_search(omap_stream->froms, address, 0, (len));

	if (omap_stream->froms[pos] != address) {
		pos -= 1;
	}

	omap_entry = (SOmapEntry *) r_list_get_n(omap_stream->omap_entries, pos);
	if (!omap_entry) {
		return -1;
	}
	if (omap_entry->to == 0) {
		return omap_entry->to;
	} else {
		return omap_entry->to + (address - omap_entry->from);
	}
        */
}
 

