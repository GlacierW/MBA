/*
** ifind (inode find)
** The Sleuth Kit
**
** Given an image  and block number, identify which inode it is used by
** 
** Brian Carrier [carrier <at> sleuthkit [dot] org]
** Copyright (c) 2006-2011 Brian Carrier, Basis Technology.  All Rights reserved
** Copyright (c) 2003-2005 Brian Carrier.  All rights reserved
**
** TASK
** Copyright (c) 2002 Brian Carrier, @stake Inc.  All rights reserved
**
** TCTUTILs
** Copyright (c) 2001 Brian Carrier.  All rights reserved
**
**
** This software is distributed under the Common Public License 1.0
**
*/

/**
 * \file ifind_lib.c
 * Contains the library API functions used by the TSK ifind command
 * line tool.
 */

#include "tsk/fs/tsk_fs_i.h"
#include "tsk/fs/tsk_hfs.h"
#include "tsk/fs/tsk_ntfs.h"

#include "mba_tsk_lib.h"

#define STRLEN 256


/*******************************************************************************
 * Find an inode given a data unit
 */

/*
 * file_walk action for non-ntfs
 */
static TSK_WALK_RET_ENUM
ifind_data_file_act(TSK_FS_FILE * fs_file, TSK_OFF_T a_off,
    TSK_DADDR_T addr, char *buf, size_t size, TSK_FS_BLOCK_FLAG_ENUM flags,
    void *ptr)
{
    TSK_FS_INFO *fs = fs_file->fs_info;
    MBA_IFIND_DATA_DATA *data = (MBA_IFIND_DATA_DATA *) ptr;

    /* Ignore sparse blocks because they do not reside on disk */
    if (flags & TSK_FS_BLOCK_FLAG_SPARSE)
        return TSK_WALK_CONT;

    if (addr == data->block) {
        /*if (TSK_FS_TYPE_ISNTFS(fs->ftype))
            tsk_printf("[this is my target]%" PRIuINUM "-%" PRIu32 "-%" PRIu16 "\n",
                data->curinode, data->curtype, data->curid);
        else
            tsk_printf("%" PRIuINUM "\n", data->curinode);
        */
        data->found = 1;
        return TSK_WALK_STOP;
    }
    return TSK_WALK_CONT;
}


/*
** find_inode
**
** Callback action for inode_walk
*/
static TSK_WALK_RET_ENUM
ifind_data_act(TSK_FS_FILE * fs_file, void *ptr)
{
    MBA_IFIND_DATA_DATA *data = (MBA_IFIND_DATA_DATA *) ptr;
    int file_flags =
        (TSK_FS_FILE_WALK_FLAG_AONLY | TSK_FS_FILE_WALK_FLAG_SLACK);
    int i, cnt;
    
    data->curinode = fs_file->meta->addr;

    /* Search all attrributes */
    cnt = tsk_fs_file_attr_getsize(fs_file);
    for (i = 0; i < cnt; i++) {
        const TSK_FS_ATTR *fs_attr = tsk_fs_file_attr_get_idx(fs_file, i);
        if (!fs_attr)
            continue;

        data->curtype = fs_attr->type;
        data->curid = fs_attr->id;
        if (fs_attr->flags & TSK_FS_ATTR_NONRES) {
            if (tsk_fs_attr_walk(fs_attr,
                    file_flags, ifind_data_file_act, ptr)) {
                if (tsk_verbose)
                    tsk_fprintf(stderr,
                        "Error walking file %" PRIuINUM
                        " Attribute: %i", fs_file->meta->addr, i);

                /* Ignore these errors */
                tsk_error_reset();
            }

            // stop if we only want one hit
            if ((data->found) && (!(data->flags & TSK_FS_IFIND_ALL)))
                break;
        }
    }

    if ((data->found) && (!(data->flags & TSK_FS_IFIND_ALL)))
        return TSK_WALK_STOP;
    else
        return TSK_WALK_CONT;
}




/* 
 * Find the inode that has allocated block blk
 * Return 1 on error, 0 if no error */
MBA_IFIND_DATA_DATA* 
mba_tsk_fs_ifind_data(TSK_FS_INFO * fs, TSK_FS_IFIND_FLAG_ENUM lclflags,
    TSK_DADDR_T blk)
{
    MBA_IFIND_DATA_DATA* data;
    data = (MBA_IFIND_DATA_DATA*)malloc(sizeof(MBA_IFIND_DATA_DATA));
    if(data == NULL)
    {
        printf("Cannot allocate memory\n");
        return NULL;
    }

    memset(data, 0, sizeof(MBA_IFIND_DATA_DATA));
    data->flags = lclflags;
    data->block = blk;

    if (fs->inode_walk(fs, fs->first_inum, fs->last_inum,
            TSK_FS_META_FLAG_ALLOC | TSK_FS_META_FLAG_UNALLOC,
            ifind_data_act, data)) {
        free(data);
        return NULL;
    }

    /* If we did not find an inode yet, get the block's
     * flags so we can identify it as a meta data block */
    if (!data->found) {
        TSK_FS_BLOCK *fs_block;

        if ((fs_block = tsk_fs_block_get(fs, NULL, blk)) != NULL) {
            if (fs_block->flags & TSK_FS_BLOCK_FLAG_META) {
                tsk_printf("Meta Data\n");
                data->found = 1;
            }
            tsk_fs_block_free(fs_block);
        }
    }

    if (!data->found) {
        tsk_printf("Inode not found\n");
        free(data);
        return NULL;
    }
    return data;
}

/***************************************************************
*  Find the path base on inode
*/

/** \internal
* Structure to store data for callbacks.
*/

static TSK_WALK_RET_ENUM
find_file_act(TSK_FS_FILE * fs_file, const char *a_path, void *ptr)
{
    char* filename= NULL;
    int path_len = 0;
    MBA_FFIND_DATA *data = (MBA_FFIND_DATA *) ptr;
    /* We found it! */
    if (fs_file->name->meta_addr == data->inode) {
        data->found = 1;

    
        path_len = strlen(a_path)+strlen(fs_file->name->name)+3;
        filename = (char*)malloc(path_len);
        if(filename==NULL)
        {
            printf("Can not allocate memory!\n");
            return TSK_WALK_CONT;
        }
        snprintf(filename, path_len-1, "/%s%s", a_path, fs_file->name->name);
        utarray_push_back(data->filenames, &filename); 

        //if (fs_file->name->flags & TSK_FS_NAME_FLAG_UNALLOC)
        //    tsk_printf("* ");


        //tsk_printf("/%s%s\n", a_path, fs_file->name->name);

        if (!(data->flags & TSK_FS_FFIND_ALL)) {
            return TSK_WALK_STOP;
        }
    }
    return TSK_WALK_CONT;
}

/* Return 0 on success and 1 on error */
MBA_FFIND_DATA* 
mba_tsk_fs_ffind(TSK_FS_INFO * fs, TSK_FS_FFIND_FLAG_ENUM lclflags,
    TSK_INUM_T a_inode,
    TSK_FS_ATTR_TYPE_ENUM type, uint8_t type_used,
    uint16_t id, uint8_t id_used, TSK_FS_DIR_WALK_FLAG_ENUM flags)
{
    MBA_FFIND_DATA* data=(MBA_FFIND_DATA*)malloc(sizeof(MBA_FFIND_DATA));
    char** p = NULL;
    char* orphan =NULL;
    int size = 0;
  
    if(data == NULL)
    { 
        printf("Cannot allocate memory\n");
        return NULL;
    }
    
    data->found = 0;
    data->flags = lclflags;
    data->inode = a_inode;
    utarray_new(data->filenames, &ut_str_icd);


    /* Since we start the walk on the root inode, then this will not show
     ** up in the above functions, so do it now
     */
    if (data->inode == fs->root_inum) {
        if (flags & TSK_FS_DIR_WALK_FLAG_ALLOC) {
            //tsk_printf("/\n");
            data->found = 1;

            if (!(lclflags & TSK_FS_FFIND_ALL))
                return data;
        }
    }

    if (TSK_FS_TYPE_ISNTFS(fs->ftype)) {
        if (ntfs_find_file(fs, data->inode, type, type_used, id, id_used,
                flags, find_file_act, data))
            return NULL;
    }
    else {
        if (tsk_fs_dir_walk(fs, fs->root_inum, flags, find_file_act,
                data))
            return NULL;
    }

    if (data->found == 0) {

        /* With FAT, we can at least give the name of the file and call
         * it orphan
         */
        if (TSK_FS_TYPE_ISFAT(fs->ftype)) {
            TSK_FS_FILE *fs_file =
                tsk_fs_file_open_meta(fs, NULL, data->inode);
            if ((fs_file != NULL) && (fs_file->meta != NULL)
                && (fs_file->meta->name2 != NULL)) {
                //if (fs_file->meta->flags & TSK_FS_META_FLAG_UNALLOC)
                //    tsk_printf("* ");
                //tsk_printf("%s/%s\n", TSK_FS_ORPHAN_STR,
                //    fs_file->meta->name2->name);

                //Added Ophan file into file list
                size = strlen(TSK_FS_ORPHAN_STR) + strlen(fs_file->meta->name2->name)+2; 
                orphan = (char*)malloc(size);
                if(orphan == NULL)
                {
                    printf("Cannot allocate memory\n"); 
                }else
                {
                    snprintf(orphan, size, "%s/%s", TSK_FS_ORPHAN_STR,fs_file->meta->name2->name);
                    utarray_push_back(data->filenames, orphan );
                }
            }
            if (fs_file)
                tsk_fs_file_close(fs_file);
        }
        else {
            tsk_printf("File name not found for inode\n");
        }
    }
    return data;
}




