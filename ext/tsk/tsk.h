/*
 *  mba sleuth kit extension header
 *
 *  Copyright (c)   2012 Chiwei Wang
 *                  2016 Chiawei Wang
 *                  2016 Chongkuan Chen
 *                  2016 Hao Li
 *                  2016 ChuanHua Cheng
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _TSK_H
#define _TSK_H
#define SOFTWARE 0
#define SYSTEM   1
#define SECURITY 2
#define SAM      3

#define REGISTRY_PATH_MAX_LENGTH 200
#define REGISTRY_VALUE_LEN 20

#include "utarray.h"
typedef uint64_t TSK_DADDR_T;

// --for registry log--
typedef struct dirtyPage {
	long int offset;
	int size;
	int position_begin;
	int position_end;
	unsigned char* text;
}dirtyPage;

typedef struct logEntry {
	int dirtyPage_count;
	int sequence_Number; 
	int size;
	dirtyPage page[1000];
}hive_log;

// --------------------


// find hard disk byte addresses of a file given its full path
// free the utarray after return value is no more used
// 
// \param img_path   the disk image file path
// \param file_path  the file full path in the filesystem stored in the image
// 
// return 0 if error, otherwise a UT_array of (start addr, last addr) tuples
extern UT_array* tsk_find_haddr_by_filename(const char* img_path, const char* file_path);

// find name of the file that contains the given byte on disk
// free the utarray and the names after used
// 
// \param imgname           the disk image file path
// \param haddr_img_offset  the byte that target file contains
// 
// return 0 if error, otherwise a UT_array of names (char*)
extern UT_array* tsk_get_filename_by_haddr(const char* imgname, uint64_t haddr_img_offset);

/*
Public API for disk forensic
*/

// Get the file of the specified harddisk byte offset
// 
// \param imgname           path of target image
// \param haddr_img_offset  the target harddisk byte address
// \param file_path         get file by the path from the image
// \param destination       store file into the given path 
// \param start_offset      offset to start reading from 
// \param read_file_len     number of bytes to read from the file 
extern void tsk_get_file(const char* imgname,uint64_t haddr_img_offset, const char* file_path, const char* destination, uint64_t start_offset, int read_file_len );

/*
Public API for disk forensic
*/

// Get the registry list to key_list[].
// 
// \param hive_file_type    registry sequence number.
extern void tsk_parse_registry(int hive_file_type);

/*
Public API for disk forensic
*/

// Get the registry path by disk address and return result by UT_array.
// 
// \param address           hive tainted of disk address.
// \param block             hive file of address in file system.
// \param search_address    search key_list number.
// \param haddr             will change next disk address for reduce loop number of time. 
// \param hive_file_type    registry sequence number.
extern UT_array* tsk_get_registry_value_by_address(const char* address, UT_array* block, int* search_address, uint64_t* haddr, int hive_type );

/*
Public API for disk forensic
*/

// Get the registry path by disk address.
// 
// \param address           hive tainted of disk address.
extern UT_array* print_registry_by_address(const char* address);

/*
Public API for disk forensic
*/

// Get the registry hive file to MBA folder.
// 
// \param log_path        transaction log file path on guest.
// \param hive_path       hive file path on host.
extern int recovery_registry_log(const char* log_path, const char* hive_path );


// Get the registry hive file to MBA folder.
// 
// \param sourcePath        hive file from path on guest.
// \param destination       hive file store path on host.
int get_hive_file( const char* sourcePath, const char* destination);

int disk_offset_tuple_cmp(const void *a, const void *b);
void merge_continuous_address(UT_array *arr);
#endif
