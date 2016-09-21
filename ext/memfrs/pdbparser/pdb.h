/*
This file is part of MBA PDB Parser.
MBA PDB Parser Header.
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
#include <stdio.h>



struct R_PDB7_ROOT_STREAM;

typedef struct pdb_streams_element {
    void *pdb_stream; // Pointer to the different type of stream
    struct pdb_streams_element *prev; /* needed for a doubly-linked list only */
    struct pdb_streams_element *next; /* needed for singly- or doubly-linked lists */
} pdb_streams_element;


//TODO: Use one structure to wrap all utlist
// SStreamParseFunc
typedef struct sstream_parse_func_element {
    void *sstream_parse_func; // Pointer to the different type of stream
    struct sstream_parse_func_element *prev; /* needed for a doubly-linked list only */
    struct sstream_parse_func_element *next; /* needed for singly- or doubly-linked lists */
} sstream_parse_func_element;

typedef struct R_PDB {
	int (*pdb_parse)(struct R_PDB *pdb);
	void (*finish_pdb_parse)(struct R_PDB *pdb);
	void (*print_types)(struct R_PDB *pdb, int mode);
	FILE *fp;
	//PrintfCallback cb_printf;
	struct R_PDB7_ROOT_STREAM *root_stream;
	void *stream_map;
        pdb_streams_element *pdb_streams;
        sstream_parse_func_element *pdb_streams2;
	//RBuffer *buf; // mmap of file
//	int curr;

	//void (*print_gvars)(struct R_PDB *pdb, ut64 img_base, int format);
} R_PDB;

int init_pdb_parser(R_PDB *pdb, const char *filename);
void print_types(R_PDB *pdb, int mode); 
