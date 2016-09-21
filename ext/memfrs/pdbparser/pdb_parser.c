/*
MBA PDB Parser Implementation.

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


#include <stdio.h>
#include <stdlib.h>
#include "pdb.h"

int main(int argc, char *argv[])
{
    if( argc != 2 ) {
        printf( "Usage: %s <PDB_FILE>\n\n", argv[0] );
        printf( "Generate two JSON file: \n\
    1. type_definition.json - data structure information\n\
    2. global_variable.json - global variable offset information\n" );
        exit( 1 );
    }

    R_PDB pdb = {0};
    init_pdb_parser(&pdb, argv[1]);
    printf("Start parsing pdb file\n");
    pdb7_parse(&pdb);
    printf("Dumping structure information\n");
    dump_json(&pdb);
    printf("Dumping global offset information\n");
    dump_gvar_json(&pdb);
}


