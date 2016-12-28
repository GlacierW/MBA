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


#include "utarray.h"
typedef uint64_t TSK_DADDR_T;
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
void tsk_get_file(const char* imgname, uint64_t haddr_img_offset,const char* file_path, const char* destination );
#endif
