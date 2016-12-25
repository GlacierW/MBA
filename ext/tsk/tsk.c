/*
 *  mba sleuth kit extension implementation
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
#include <stdint.h>

#include "tsk/tsk_tools_i.h"
#include "tsk/fs/tsk_fs_i.h"
#include "tsk/fs/tsk_hfs.h"
#include "tsk/fs/tsk_ntfs.h"

#include "tsk.h"

//Hardcode image type to support qcow 
#define QCOW_IMG_TYPE "qcow" 

#define STRLEN 256

#define FIND_FS_CONT 0
#define FIND_FS_NOT_FOUND 1
#define FIND_FS_FOUND 2

#define OPEN_FAIL(x) (x == NULL)

typedef struct{
    int find_fs_stat;
    uint64_t target_offset;
    uint64_t found_fs_offset;
} find_fs_struct;

static UT_icd ut_tsk_daddr_tuple_icd = {sizeof(TSK_DADDR_T)*2, NULL, NULL,NULL};

typedef struct {
    TSK_INUM_T inode;
    uint8_t flags;
    uint8_t found;
    UT_array *filenames;
} MBA_FFIND_DATA;

typedef struct{
    const char *full_path;
    int file_found_in_partiton;
    int file_found;
    UT_array *offsets_in_filesystem;
    UT_array *offsets_in_disk;
}find_file_data;

typedef struct {
    TSK_DADDR_T block;          /* the block to find */
    TSK_FS_IFIND_FLAG_ENUM flags;
    uint8_t found;

    TSK_INUM_T curinode;        /* the inode being analyzed */
    uint32_t curtype;           /* the type currently being analyzed: NTFS */
    uint16_t curid;
} MBA_IFIND_DATA_DATA;;


/*
Functions to support finding address of a file with given file path 
*/

static int is_run_on_disk(const TSK_FS_BLOCK_FLAG_ENUM run_flags) {
    return (run_flags & TSK_FS_BLOCK_FLAG_SPARSE) == 0;
}

// file contains attributes, attributes contain runs, and runs are on disk.
static TSK_WALK_RET_ENUM
log_attr_offsets_in_filesystem(TSK_FS_FILE * fs_file, TSK_OFF_T a_off,
        TSK_DADDR_T addr, char *buf, size_t size, TSK_FS_BLOCK_FLAG_ENUM flags,
        void *ptr){
    if (is_run_on_disk(flags)){
        find_file_data                    *data = (find_file_data*)ptr;
        TSK_DADDR_T    offset_start_and_last[2] = {
            addr * fs_file->fs_info->block_size, 
            (addr+1) * fs_file->fs_info->block_size - 1
        };
        
        utarray_push_back(data->offsets_in_filesystem, &offset_start_and_last);
        data->file_found_in_partiton = 1;
    } 
    
    return TSK_WALK_CONT;
}

static int is_attr_on_disk(const TSK_FS_ATTR *attr) {
    return attr->flags & TSK_FS_ATTR_NONRES;
}

// file contains attributes, one attribute is the content.
static void 
log_file_offsets_in_filesystem(find_file_data *data, TSK_FS_FILE *file){

    int i;
    int cnt = tsk_fs_file_attr_getsize(file);
    TSK_FS_FILE_WALK_FLAG_ENUM 
        file_walk_flags = (TSK_FS_FILE_WALK_FLAG_ENUM)(
                    TSK_FS_FILE_WALK_FLAG_AONLY   // Provide callback with only addr
                    | TSK_FS_FILE_WALK_FLAG_SLACK // Include file's slack space
                    );
    
    for (i = 0; i < cnt; i++) {
        const TSK_FS_ATTR *attr = tsk_fs_file_attr_get_idx(file, i);
        if (attr && is_attr_on_disk(attr)) {
            tsk_fs_attr_walk(
                    attr,                           // file attribute to log offsets
                    file_walk_flags,                // walk options
                    log_attr_offsets_in_filesystem, // action for each attr
                    data                            // argument for action
                    );
        }
    }
}

static TSK_WALK_RET_ENUM find_file_in_partiton(
        TSK_VS_INFO * vs, const TSK_VS_PART_INFO * part, void *ptr){

    find_file_data *data      = (find_file_data*)ptr;
    const char     *file_path = data->full_path;
    TSK_FS_INFO    *filesystem;
    TSK_FS_FILE    *file;

    filesystem = tsk_fs_open_vol(part, TSK_FS_TYPE_DETECT);
    if (OPEN_FAIL(filesystem))
        return TSK_WALK_CONT;

    file       = tsk_fs_file_open(filesystem, NULL, file_path);
    if (OPEN_FAIL(file))
        return TSK_WALK_CONT;

    data->file_found_in_partiton = 0;
    utarray_new(data->offsets_in_filesystem, &ut_tsk_daddr_tuple_icd);
    log_file_offsets_in_filesystem(data, file);

    if (data->file_found_in_partiton) {
        TSK_DADDR_T *p;
        for (p=(TSK_DADDR_T*)utarray_front(data->offsets_in_filesystem);
                p != NULL;
                p=(TSK_DADDR_T*)utarray_next(data->offsets_in_filesystem, p)) {

            TSK_DADDR_T   offset_start_and_last[2] = {
                p[0] + part->start*vs->img_info->sector_size,
                p[1] + part->start*vs->img_info->sector_size
            };

            utarray_push_back(data->offsets_in_disk, &offset_start_and_last);
        }
        data->file_found = 1;
    }
    utarray_free(data->offsets_in_filesystem);

    tsk_fs_file_close(file);
    tsk_fs_close(filesystem);
    return TSK_WALK_CONT;
}


/*******************************************************************************
 * Find an inode given a data unit
   This part is copy from TSK with little modify to make it return a data structre..
 */

/*
 * file_walk action for non-ntfs
 */
static TSK_WALK_RET_ENUM
ifind_data_file_act(TSK_FS_FILE * fs_file, TSK_OFF_T a_off,
    TSK_DADDR_T addr, char *buf, size_t size, TSK_FS_BLOCK_FLAG_ENUM flags,
    void *ptr)
{
    MBA_IFIND_DATA_DATA *data = (MBA_IFIND_DATA_DATA *) ptr;

    /* Ignore sparse blocks because they do not reside on disk */
    if (flags & TSK_FS_BLOCK_FLAG_SPARSE)
        return TSK_WALK_CONT;

    if (addr == data->block) {
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

// Find the inode that has allocated the specified block
// 
// \param fs        filesystem info structure defined in sleuthkit
// \param lclflags  traverse options defined in sleuthkit
// \param blk       the target block
// 
// Return 0 if error, otherwise a pointer
static MBA_IFIND_DATA_DATA*
fs_ifind_data(TSK_FS_INFO * fs, TSK_FS_IFIND_FLAG_ENUM lclflags,
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
        //tsk_printf("Inode not found\n");
        free(data);
        return NULL;
    }
    return data;
}


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

        if (!(data->flags & TSK_FS_FFIND_ALL)) {
            return TSK_WALK_STOP;
        }
    }
    return TSK_WALK_CONT;
}

// XXX(misterlihao@gmail.com): had better fix the comment
// find file of the given inode
// 
// \param fs         filesystem info structure defined in sleuthkit
// \param lclflags   traverse options defined in sleuthkit
// \param a_inode    the inode of the file
// \param type       filesystem type number
// \param type_used  don't know what it is, just give a 0(XXX)
// \param id         the id of file attribute that the inode refers
// \param id_used    don't know what it is, just give a 0(XXX)
// \param flags      traverse options difned in sleuthkit
// 
// Return 0 if error, otherwise a pointer
static MBA_FFIND_DATA*
fs_ffind(TSK_FS_INFO * fs, TSK_FS_FFIND_FLAG_ENUM lclflags,
    TSK_INUM_T a_inode,
    TSK_FS_ATTR_TYPE_ENUM type, uint8_t type_used,
    uint16_t id, uint8_t id_used, TSK_FS_DIR_WALK_FLAG_ENUM flags)
{
    MBA_FFIND_DATA* data=(MBA_FFIND_DATA*)malloc(sizeof(MBA_FFIND_DATA));
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


static TSK_WALK_RET_ENUM part_act(TSK_VS_INFO * vs, const TSK_VS_PART_INFO * part, void *found_fs_struct)
{

    //search for if the target block is in the range of target block
    if(
       ((find_fs_struct*)found_fs_struct)->target_offset >= part->start &&
       ((find_fs_struct*)found_fs_struct)->target_offset <  part->start +  part->len
      )
    {
        //change state as FIND_FS_FOUND
        ((find_fs_struct*)found_fs_struct)->find_fs_stat = FIND_FS_FOUND;
        ((find_fs_struct*)found_fs_struct)->found_fs_offset = part->start;
    }
    return TSK_WALK_CONT;
}

//return the offset to the file system which contain our target_addr
static TSK_DADDR_T search_partition(TSK_VS_INFO *vs, uint64_t target_addr)
{
    int flags = TSK_VS_PART_FLAG_ALL;
    TSK_DADDR_T ret = 0;

    find_fs_struct* ffs_struct = (find_fs_struct*)malloc(sizeof(find_fs_struct));
    if(ffs_struct == NULL)
    {
        printf("Cannot allocate memory!\n");
        return 0;
    }
    ffs_struct->find_fs_stat = FIND_FS_CONT;
    ffs_struct->target_offset = target_addr;
    ffs_struct->found_fs_offset = 0;

    tsk_vs_part_walk(vs, 0, vs->part_count - 1, (TSK_VS_PART_FLAG_ENUM) flags, part_act, (void*)ffs_struct);

    if(ffs_struct->find_fs_stat == FIND_FS_FOUND)
    {
        ret = ffs_struct->found_fs_offset;
    }else
    {
        ret = 0;
    }
    free(ffs_struct);
    return ret;
}





/*
Public API for disk forensic
*/

// Find the filename of the specified harddisk byte offset
// 
// \param imgname           path of target image
// \param haddr_img_offset  the target harddisk byte address
// 
// Return NULL if error, otherwise a UT_array of filename 
UT_array* tsk_get_filename_by_haddr(const char* imgname, uint64_t haddr_img_offset)
{
    TSK_IMG_INFO *img;
    TSK_VS_INFO *vs;
    TSK_FS_INFO *fs;
    uint8_t id_used = 0, type_used = 0;

    TSK_DADDR_T partition_offset = 0;
    TSK_DADDR_T block_img_offset = 0;
    TSK_DADDR_T part_byte_offset = 0;
    TSK_DADDR_T part_block_offset = 0;

    MBA_IFIND_DATA_DATA* ifind_data;
    TSK_IMG_TYPE_ENUM imgtype;

    MBA_FFIND_DATA* ffind_data;

    UT_array* ret = NULL;

    //open image
    imgtype = tsk_img_type_toid(QCOW_IMG_TYPE);
    img = tsk_img_open_sing(imgname, imgtype, 0);
    if(img == NULL)
    {
         printf("Image Open Failed!!\n");
         return NULL;
    }

    if(haddr_img_offset >= img->size)
    {
        printf("Request haddr is larger than image size\n");
        tsk_img_close(img);
        return NULL;
    }

    //open volume
    vs = tsk_vs_open(img, 0 , TSK_VS_TYPE_DETECT);
    if(vs==NULL)
    {
        printf("Volume Open Failed!!\n");
        tsk_img_close(img);
        return NULL;
    }

    //calculate block address
    block_img_offset = haddr_img_offset/img->sector_size;

    //search the partition contain the target block
    partition_offset = search_partition(vs, block_img_offset);
    if(partition_offset == 0)
    {
        tsk_img_close(img);
        tsk_vs_close(vs);
        return NULL;
    }

    //open the partition's file system
    fs = tsk_fs_open_img(img, partition_offset * img->sector_size, TSK_FS_TYPE_DETECT);
    if(fs==NULL)
    {
        printf("Cannot open file system\n");
        tsk_img_close(img);
        tsk_vs_close(vs);
        return NULL;
    }

    //calculate offset to the current partition
    part_byte_offset = haddr_img_offset - (partition_offset * img->sector_size);
    part_block_offset = part_byte_offset/fs->block_size;

    //find the inode of this block
    ifind_data = fs_ifind_data(fs, (TSK_FS_IFIND_FLAG_ENUM) 0, part_block_offset);
    if(ifind_data == NULL)
    {
        tsk_img_close(img);
        tsk_vs_close(vs);
        return NULL; 
    }    

    if(ifind_data->found!=1)
    {
        tsk_img_close(img);
        tsk_vs_close(vs);
        return NULL;
    }

    //Find the inode's filename
    //Note: Do Not Know what to fill in variable type_used and id_used
    ffind_data =  fs_ffind(fs, 0, ifind_data->curinode, ifind_data->curtype ,
            type_used, ifind_data->curid , id_used,
            (TSK_FS_DIR_WALK_FLAG_RECURSE | TSK_FS_DIR_WALK_FLAG_ALLOC | TSK_FS_DIR_WALK_FLAG_UNALLOC));

    if(ffind_data==NULL){
        printf("Cannot found fdata associate with inode\n");
        tsk_img_close(img);
        tsk_vs_close(vs);
        return NULL;
    }

    //free data
    //reserve return data first 
    ret = ffind_data->filenames;

    free(ifind_data);
    free(ffind_data);
    tsk_vs_close(vs);
    tsk_img_close(img);

    return ret;
}

// Find all the harddisk address corresponding to target file
// 
// \param img_path          path of target image
// \param file_path         the target filename
// 
// Return NULL if error, otherwise a UT_array of (start, end) tuple will returned, where start and end are the harddisk byte addrsesses
UT_array* tsk_find_haddr_by_filename(const char* img_path, const char* file_path)
{
    TSK_IMG_TYPE_ENUM img_type ;
    TSK_IMG_INFO *img;
    TSK_VS_INFO *vs;
 
    find_file_data data;
    UT_array* ret = NULL;

    img_type = tsk_img_type_toid(QCOW_IMG_TYPE);

    //XXX(misterlihao@gmail.com):need error handling.
    img = tsk_img_open_sing(img_path, img_type, 0);
    if(img == NULL)
    {
         printf("Image Open Failed!!\n");
         return NULL;
    }

    vs = tsk_vs_open(img, 0, TSK_VS_TYPE_DETECT);
    if(vs==NULL)
    {
        printf("Volume Open Failed!!\n");
        tsk_img_close(img);
        return NULL;
    }

    data.file_found = 0;
    data.full_path  = file_path;
    utarray_new(data.offsets_in_disk, &ut_tsk_daddr_tuple_icd);
    tsk_vs_part_walk(
            vs,                 // the volume system 
            0,                             // start from first partiton 
            vs->part_count - 1, // walk to last partiton
            TSK_VS_PART_FLAG_ALLOC,        // traverse all allocated partiton
            find_file_in_partiton,         // action for each partition
            &data                          // argument for action
            );

    if (data.file_found) {
        ret = data.offsets_in_disk;
    }
    else {
        printf("file block not found\n");
        tsk_vs_close(vs);
        tsk_img_close(img);
        return NULL;
    }

    tsk_vs_close(vs);
    tsk_img_close(img);

    return ret;
}

/*
Public API for disk forensic
*/

// Get the file of the specified harddisk byte offset
// 
// \param imgname           path of target image
// \param haddr_img_offset  the target harddisk byte address
// \param file_path         get file by the path from the image
// \param destination       store file into the given path 
// 
// Return NULL if error, otherwise a UT_array of filename 

void tsk_get_file(const char* imgname,uint64_t haddr_img_offset, const char* file_path, const char* destination )
{
    TSK_IMG_INFO *img;
    TSK_VS_INFO *vs;
    TSK_FS_INFO *fs;
    uint8_t id_used = 0, type_used = 0;

    TSK_DADDR_T partition_offset = 0;
    TSK_DADDR_T block_img_offset = 0;
    TSK_DADDR_T part_byte_offset = 0;
    TSK_DADDR_T part_block_offset = 0;

    MBA_IFIND_DATA_DATA* ifind_data;
    TSK_IMG_TYPE_ENUM imgtype;
    MBA_FFIND_DATA* ffind_data;
    TSK_FS_FILE *file;
    FILE* writeHive;
    char *temp;

    //open image
    imgtype = tsk_img_type_toid(QCOW_IMG_TYPE);
    img = tsk_img_open_sing(imgname, imgtype, 0);
    if(img == NULL)
    {
         printf("Image Open Failed!!\n");
         return;
    }

    if(haddr_img_offset >= img->size)
    {
        printf("Request haddr is larger than image size\n");
        return;
    }

    //open volume
    vs = tsk_vs_open(img, 0 , TSK_VS_TYPE_DETECT);
    if(vs==NULL)
    {
        printf("Volume Open Failed!!\n");
        return;
    }

    //calculate block address
    block_img_offset = haddr_img_offset/img->sector_size;

    //search the partition contain the target block
    partition_offset = search_partition(vs, block_img_offset);
    if(partition_offset == 0)
    {
        printf("Cannot found partition contains the target haddr\n");
        return;
    }

    //open the partition's file system
    fs = tsk_fs_open_img(img, partition_offset * img->sector_size, TSK_FS_TYPE_DETECT);
    if(fs==NULL)
    {
        printf("Cannot open file system\n");
        return;
    }

    //calculate offset to the current partition
    part_byte_offset = haddr_img_offset - (partition_offset * img->sector_size);
    part_block_offset = part_byte_offset/fs->block_size;

    file = tsk_fs_file_open( fs, NULL, file_path);
    if ( OPEN_FAIL(file) ) 
        printf("open file fail\n\n");

    TSK_OFF_T readFileOff = 0;
    size_t fileSize = 150000000;
    temp = calloc( fileSize, sizeof(char));
    int size = tsk_fs_file_read( file,
                                 readFileOff,
                                 temp,
                                 fileSize,
                                 TSK_FS_FILE_READ_FLAG_NONE );
    tsk_fs_file_close(file);
    writeHive = fopen( destination, "w" );
    if ( writeHive == NULL )
        printf("Open fail");
    else {
        fwrite( temp, size, sizeof(char), writeHive );
    } // else

    free(temp);
    fclose(writeHive);

    //find the inode of this block
    ifind_data = fs_ifind_data(fs, (TSK_FS_IFIND_FLAG_ENUM) 0, part_block_offset);
    if(ifind_data == NULL)
    {
        return; 
    }    

    if(ifind_data->found!=1)
    {
        printf("Inode not found\n");
        return;
    }

    //Find the inode's filename
    //Note: Do Not Know what to fill in variable type_used and id_used
    ffind_data =  fs_ffind(fs, 0, ifind_data->curinode, ifind_data->curtype ,
            type_used, ifind_data->curid , id_used,
            (TSK_FS_DIR_WALK_FLAG_RECURSE | TSK_FS_DIR_WALK_FLAG_ALLOC | TSK_FS_DIR_WALK_FLAG_UNALLOC));

    if(ffind_data==NULL){
        printf("Cannot found fdata associate with inode\n");
        return;
    }

    free(ifind_data);
    return;
}



