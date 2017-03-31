#!/bin/bash
gcc -g \
    -I../../../include/ -I../../../json-c/ -I../../../json-c/tests/ \
    pdb_parser.c pdb.c  \
    stream_file.c tpi.c \
    dbi.c gdata.c stream_pe.c omap.c \
    ../../../json-c/.libs/libjson-c.a \
    -o pdb_parser
