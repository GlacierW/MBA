
#include "tsk/tsk_tools_i.h"
#include "mba_tsk_lib.h"

#define QCOW_IMG_TYPE "qcow" 

#define FIND_FS_CONT 0
#define FIND_FS_NOT_FOUND 1
#define FIND_FS_FOUND 2

typedef struct{
    int find_fs_stat;
    uint64_t target_offset; 
    uint64_t found_fs_offset; 
} find_fs_struct;


//
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
TSK_DADDR_T search_partition(TSK_VS_INFO *vs, uint64_t target_addr)
{
    int flags = TSK_VS_PART_FLAG_ALL;
    TSK_DADDR_T ret = 0;
  
    find_fs_struct* ffs_struct = (find_fs_struct*)malloc(sizeof(find_fs_struct));
    if(ffs_struct == NULL)
    { 
        printf("Cannot allocate memory!\n");
        return NULL;
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

UT_array* getFilenameByHaddr(char* imgname, uint64_t haddr_img_offset)
{
    TSK_IMG_INFO *img;
    TSK_VS_INFO *vs;
    TSK_FS_INFO *fs;
    uint16_t id = 0;
    uint8_t id_used = 0, type_used = 0;
    TSK_FS_ATTR_TYPE_ENUM type;

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
        return NULL;	
    }

    //open volume
    vs = tsk_vs_open(img, 0 , TSK_VS_TYPE_DETECT);
    if(vs==NULL)
    {
        printf("Volume Open Failed!!\n");
        return -1;
    }
    
    //calculate block address
    printf("Image sector size %d\n", img->sector_size);
    block_img_offset = haddr_img_offset/img->sector_size;
    printf("Block addr %"PRIuINUM"\n", block_img_offset);

    //search the partition contain the target block
    partition_offset = search_partition(vs, block_img_offset); 
    if(partition_offset == 0)
    {
        printf("Cannot found partition contains the target haddr\n");
        return NULL;
    }
    printf("partition off: %"PRIuINUM"\n", partition_offset); 
    
    //open the partition's file system
    fs = tsk_fs_open_img(img, partition_offset * img->sector_size, TSK_FS_TYPE_DETECT);
    if(fs==NULL)
    {
        printf("Cannot open file system\n");
        return NULL;
    }

    //calculate offset to the current partition
    part_byte_offset = haddr_img_offset - (partition_offset * img->sector_size);
    printf("block_size %d\n", fs->block_size);
    part_block_offset = part_byte_offset/fs->block_size;
    printf("byte offset to fs %"PRIuINUM"\n", part_byte_offset);
    printf("block count in current filesystem %"PRIuINUM"\n", fs->block_count);
    printf("block offset to current filesystem %"PRIuINUM"\n", part_block_offset);

    //find the inode of this block
    ifind_data = mba_tsk_fs_ifind_data(fs, (TSK_FS_IFIND_FLAG_ENUM) 0, part_block_offset);
    if(ifind_data->found!=1)
    {
        printf("Inode not found\n");
        return NULL; 
    }
    printf("Find inode data %"PRIuINUM"\n", ifind_data->curinode);

    //Find the inode's filename
    //Note: Do Not Know what to fill in variable type_used and id_used
    ffind_data =  mba_tsk_fs_ffind(fs, 0, ifind_data->curinode, ifind_data->curtype ,
            type_used, ifind_data->curid , id_used,
            (TSK_FS_DIR_WALK_FLAG_RECURSE | TSK_FS_DIR_WALK_FLAG_ALLOC | TSK_FS_DIR_WALK_FLAG_UNALLOC));

    if(ffind_data==NULL){
        printf("Cannot found fdata associate with inode\n");
        return NULL;
    } 
    
    //free data
    //reserve return data first 
    ret = ffind_data->filenames;

    free(ifind_data);
    free(ffind_data);

    return ffind_data->filenames;
}

int main(int argc, char* argv[])
{
    uint64_t  haddr_img_offset = atoll(argv[2]);
    char *name= argv[1];    
    char **p; 
    
    UT_array* filenames = getFilenameByHaddr(name, haddr_img_offset);
   
    p = NULL;
    while ( (p=(char**)utarray_next(filenames,p))) {
         printf(" %s\n",*p);
    }



    return 0;
}



