#include "qemu-common.h"
#include "monitor/monitor.h"

#include "ext/tsk/tsk-commands.h"
#include "ext/tsk/tsk.h"

#include "qmp-commands.h"

char* get_imagepath_by_block_id(const char* dev_id)
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

   for (p=(TSK_DADDR_T*)utarray_front(blocks);
        p != NULL;
        p=(TSK_DADDR_T*)utarray_next(blocks, p))
      {
            printf("%d - %lu\n", cnt++, *p);
      }
   return;
}


