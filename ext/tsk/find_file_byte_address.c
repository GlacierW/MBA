#include <stdint.h>

#include "tsk/tsk_tools_i.h"

#define OPEN_FAIL(x) (x == NULL)

typedef struct{
    char *full_path;
    int64_t offset_from_filesystem;
    int64_t offset_from_disk_head;
}FindFileData;

int is_file_run_on_disk(const TSK_FS_BLOCK_FLAG_ENUM run_flags) {
    return run_flags & TSK_FS_BLOCK_FLAG_SPARSE;
}

static TSK_WALK_RET_ENUM
get_block_addr(TSK_FS_FILE * fs_file, TSK_OFF_T a_off,
        TSK_DADDR_T addr, char *buf, size_t size, TSK_FS_BLOCK_FLAG_ENUM flags,
        void *ptr){
    if (is_file_run_on_disk(flags))
        return TSK_WALK_CONT;

    ((FindFileData*)ptr)->offset_from_filesystem = addr;
    return TSK_WALK_STOP;
}

int is_file_attribute_on_disk(const TSK_FS_ATTR *fs_attr) {
    return fs_attr->flags & TSK_FS_ATTR_NONRES;
}

void get_file_block_offset_from_filesystem(TSK_FS_FILE *file, FindFileData *data){
    int i, cnt = tsk_fs_file_attr_getsize(file);
    TSK_FS_FILE_WALK_FLAG_ENUM file_flags = (TSK_FS_FILE_WALK_FLAG_ENUM)
            (TSK_FS_FILE_WALK_FLAG_AONLY | TSK_FS_FILE_WALK_FLAG_SLACK);
    
    for (i = 0; i < cnt; i++) {
        const TSK_FS_ATTR *fs_attr = tsk_fs_file_attr_get_idx(file, i);
        if (!fs_attr)
            continue;

        if (is_file_attribute_on_disk(fs_attr)) {
            tsk_fs_attr_walk(fs_attr, file_flags, get_block_addr, data);
            
            break;
        }
    }
}

static TSK_WALK_RET_ENUM find_file_in_part(TSK_VS_INFO * vs, const TSK_VS_PART_INFO * part, void *ptr)
{
    FindFileData *data = (FindFileData*)ptr;
    TSK_FS_INFO *fs = tsk_fs_open_vol(part, TSK_FS_TYPE_DETECT);
    if (OPEN_FAIL(fs))
        return TSK_WALK_CONT;

    char *file_path = data->full_path;
    TSK_FS_FILE *file = tsk_fs_file_open(fs, NULL, file_path);
    if (OPEN_FAIL(file))
        return TSK_WALK_CONT;
    
    data->offset_from_filesystem = fs->last_block + 1;
    get_file_block_offset_from_filesystem(file, data);
    tsk_fs_file_close(file);
    tsk_fs_close(fs);

    if (data->offset_from_filesystem <= fs->last_block) {
        data->offset_from_disk_head = data->offset_from_filesystem*fs->block_size + part->start*vs->img_info->sector_size;

        return TSK_WALK_STOP;
    }
    return TSK_WALK_CONT;
}

int main(int argc, char **argv)
{
    const char *img_path= argv[1];
    char *path= argv[2];

    TSK_IMG_TYPE_ENUM img_type ;
    TSK_IMG_INFO *img;
    TSK_VS_INFO *vs;
    TSK_VS_PART_FLAG_ENUM flags = (TSK_VS_PART_FLAG_ENUM)TSK_VS_PART_FLAG_ALL;

    img_type = tsk_img_type_toid("qcow");

    //XXX(misterlihao@gmail.com):need error handling.
    img = tsk_img_open_sing(img_path, img_type, 0);
    vs = tsk_vs_open(img, 0, TSK_VS_TYPE_DETECT);

    FindFileData data;
    data.full_path = path;
    data.offset_from_disk_head = img->size + 1;
    tsk_vs_part_walk(vs, 0, vs->part_count - 1, flags, find_file_in_part, &data);
    if (data.offset_from_disk_head > img->size) {
        printf("file found but block not found\n");
    }
    else {
        printf("%s found at byte %lu from disk head\n", path, data.offset_from_disk_head);
    }

    tsk_vs_close(vs);
    tsk_img_close(img);
    printf("finish\n");
}
