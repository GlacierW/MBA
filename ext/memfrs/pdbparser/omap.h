/*
MBA PDB OMAP Stream Parser Header.

This file is part of MBA PDB Parser, which use to handle omap stream..
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
#ifndef OMAP_H
#define OMAP_H

void parse_omap_stream(void *stream, R_STREAM_FILE *stream_file);
void free_omap_stream(void *stream);
int omap_remap(void *stream, int address);

#endif // OMAP_H
