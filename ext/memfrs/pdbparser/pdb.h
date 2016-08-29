
#include <stdio.h>



struct R_PDB7_ROOT_STREAM;

typedef struct pdb_streams_element {
    void *pdb_stream; // Pointer to the different type of stream
    struct pdb_streams_element *prev; /* needed for a doubly-linked list only */
    struct pdb_streams_element *next; /* needed for singly- or doubly-linked lists */
} pdb_streams_element;


typedef struct R_PDB {
	int (*pdb_parse)(struct R_PDB *pdb);
	void (*finish_pdb_parse)(struct R_PDB *pdb);
	void (*print_types)(struct R_PDB *pdb, int mode);
	FILE *fp;
	//PrintfCallback cb_printf;
	struct R_PDB7_ROOT_STREAM *root_stream;
	void *stream_map;
	//RList *pdb_streams;
        pdb_streams_element *pdb_streams;
	//RList *pdb_streams2;
	//RBuffer *buf; // mmap of file
//	int curr;

	//void (*print_gvars)(struct R_PDB *pdb, ut64 img_base, int format);
} R_PDB;

int init_pdb_parser(R_PDB *pdb, const char *filename);
void print_types(R_PDB *pdb, int mode); 
