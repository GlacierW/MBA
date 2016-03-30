/*
 *  mba sleuth kit extension of qemu command implementation
 *
 *  Copyright (c)   2012 Chiwei Wang
 *                  2016 Chiawei Wang
 *                  2016 Chongkuan Chen
 *                  2016 Hao Li
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
#include "qemu-common.h"
#include "monitor/monitor.h"

#include "ext/tsk/tsk-commands.h"
#include "ext/tsk/tsk.h"

#include "qmp-commands.h"

static char* get_imagepath_by_block_id(const char* dev_id);

void do_get_filename_by_haddr(Monitor *mon, const QDict *qdict)
{
    const char* device_id = qdict_get_str(qdict, "dev");
    uint64_t target_addr = qdict_get_int(qdict, "addr");
    char **p; 
    UT_array* filenames;
    //temporary hardcode image path
    const char* tmp = get_imagepath_by_block_id(device_id);

    if(tmp == NULL)
    {
        monitor_printf(mon, "Cannot get file path\n");
    }

    monitor_printf(mon, "Getting imagepath of  %s, look for address %lu\n", tmp, target_addr);
    filenames = tsk_get_filename_by_haddr(tmp, target_addr);

    if(filenames==NULL)
    {
        monitor_printf(mon, "No file found\n");
        return;
    } 

    p = NULL;
    while ( (p=(char**)utarray_next(filenames,p))) {
        monitor_printf(mon, " %s\n",*p);
    }
    return;  
}

static int disk_offset_tuple_cmp(const void *a, const void *b);

static void sort_and_merge_continuous_address(UT_array *arr);

void do_get_haddr_by_filename(Monitor *mon, const QDict *qdict)
{
    const char* device_id = qdict_get_str(qdict, "dev");
    const char* filename = qdict_get_str(qdict, "filename");
    UT_array* blocks;
    int cnt = 0;
    TSK_DADDR_T *p = NULL;

    //temporary hardcode image path
    const char* tmp = get_imagepath_by_block_id(device_id);;

    monitor_printf(mon, "Getting address of  %s, in image %s\n", filename, tmp);
    blocks = tsk_find_haddr_by_filename(tmp, filename);
    if(blocks==NULL)
    {
        monitor_printf(mon, "No such file found\n");
        return;
    }

    sort_and_merge_continuous_address(blocks);
    for (p=(TSK_DADDR_T*)utarray_front(blocks);
            p != NULL;
            p=(TSK_DADDR_T*)utarray_next(blocks, p))
    {
        printf("%d - %lu ~ %lu\n", cnt++, p[0], p[1]);
    }
    return;
}


static char* get_imagepath_by_block_id(const char* dev_id)
{
    BlockInfoList *block_list, *info;

    block_list = qmp_query_block(NULL);

    for (info = block_list; info; info = info->next) {
        if(info->value->has_inserted)
        {
            printf("%s\n", info->value->device);
            printf("%s\n", info->value->inserted->file);
            if(strcmp(info->value->device, dev_id)==0)
                return info->value->inserted->file;
        }
    }
    return NULL;
}
static int disk_offset_tuple_cmp(const void *a, const void *b){
    TSK_DADDR_T *p = (TSK_DADDR_T*)a;
    TSK_DADDR_T *q = (TSK_DADDR_T*)b;

    if (*p < *q)
        return -1;
    else if (*p > *q)
        return 1;
    return 0;
}

static void sort_and_merge_continuous_address(UT_array *arr){
    int i;
    TSK_DADDR_T *prev;
    TSK_DADDR_T *p;
    int len;
    utarray_sort(arr, disk_offset_tuple_cmp);
    len = utarray_len(arr);
    
    for (i=1; i<len;)
    {
        prev = (TSK_DADDR_T*)utarray_eltptr(arr, i-1);
        p    = (TSK_DADDR_T*)utarray_eltptr(arr, i);
        if (prev[1]+1 == p[0]){
            prev[1] = p[1];
            utarray_erase(arr, i, 1);
            len--;
        }
        else {
            i++;
        }
    }
}
