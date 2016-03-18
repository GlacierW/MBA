#ifndef _MBA_TSK_FS_I_H
#define _MBA_TSK_FS_I_H

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


#endif
