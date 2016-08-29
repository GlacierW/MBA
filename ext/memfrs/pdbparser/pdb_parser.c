#include <stdio.h>

#include "pdb.h"

//static int bin_pdb(RCore *core, int mode) 
int main(int argc, char *argv[])
{
    R_PDB pdb = {0};
    printf("pdb %p\n", &pdb);
    init_pdb_parser(&pdb, argv[1]);
    printf("pdbi2 %p\n", &pdb);
    pdb7_parse(&pdb);
    //print_types(&pdb, 'j');    
    dump_json(&pdb);
}


