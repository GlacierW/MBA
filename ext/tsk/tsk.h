#ifndef _TSK_H
#define _TSK_H

#include "tsk/tsk_tools_i.h"
#include "tsk/fs/tsk_fs_i.h"
#include "tsk/fs/tsk_hfs.h"
#include "tsk/fs/tsk_ntfs.h"

#include "utarray.h"

typedef struct {
    TSK_DADDR_T block;          /* the block to find */
    TSK_FS_IFIND_FLAG_ENUM flags;
    uint8_t found;

    TSK_INUM_T curinode;        /* the inode being analyzed */
    uint32_t curtype;           /* the type currently being analyzed: NTFS */
    uint16_t curid;
} MBA_IFIND_DATA_DATA;

extern MBA_IFIND_DATA_DATA* mba_tsk_fs_ifind_data(TSK_FS_INFO * fs, TSK_FS_IFIND_FLAG_ENUM lclflags, TSK_DADDR_T blk);


typedef struct {
    TSK_INUM_T inode;
    uint8_t flags;
    uint8_t found;
    UT_array *filenames;
} MBA_FFIND_DATA;

extern MBA_FFIND_DATA* mba_tsk_fs_ffind(TSK_FS_INFO * fs, TSK_FS_FFIND_FLAG_ENUM lclflags,TSK_INUM_T a_inode,
                                TSK_FS_ATTR_TYPE_ENUM type, uint8_t type_used,
                                uint16_t id, uint8_t id_used, TSK_FS_DIR_WALK_FLAG_ENUM flags);

typedef struct{
    const char *full_path;
    int file_found_in_partiton;
    int file_found;
    UT_array *offsets_in_filesystem;
    UT_array *offsets_in_disk;
}find_file_data;

extern UT_array* tsk_find_haddr_by_filename(const char* img_path, const char* file_path);
extern UT_array* tsk_get_filename_by_haddr(const char* imgname, uint64_t haddr_img_offset);

// Private function declare
int is_file_attribute_on_disk(const TSK_FS_ATTR *fs_attr);
void get_file_block_offset_from_filesystem(TSK_FS_FILE *file, find_file_data *data);
char* get_imagepath_by_block_id(const char* dev_id);
TSK_DADDR_T search_partition(TSK_VS_INFO *vs, uint64_t target_addr);
#endif
