/*
 *  mba sleuth kit extension of qemu command implementation
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
#include "qemu-common.h"
#include "monitor/monitor.h"

#include "ext/tsk/tsk-commands.h"
#include "ext/tsk/tsk.h"

#include "qmp-commands.h"

#include "ext/tsk/libregf/regftools/info_handle.h"
#include <common.h>
#include <system_string.h>
#include <memory.h>
#include <types.h>

#include "ext/tsk/libregf/regftools/regftools_libcerror.h"
#include "ext/tsk/libregf/regftools/regftools_libclocale.h"
#include "ext/tsk/libregf/regftools/regftools_libcnotify.h"
#include "ext/tsk/libregf/regftools/regftools_libcsystem.h"
#include "ext/tsk/libregf/regftools/regftools_libregf.h"


info_handle_t *regfinfo_info_handle = NULL;
Str registryPath[StringLength];
Str registrySource[CHECK_REGISTRY_PATH];
int registryPathLen;
extern key_offset_list key_list[];
extern int key_list_index;
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
static void merge_continuous_address(UT_array *arr);

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

    // sort_and_merge_continuous_address(blocks);
    merge_continuous_address(blocks);
    for (p=(TSK_DADDR_T*)utarray_front(blocks);
            p != NULL;
            p=(TSK_DADDR_T*)utarray_next(blocks, p))
    {
        printf("%d - %lu ~ %lu\n", cnt++, p[0], p[1]);
    }
    return;
}

void do_get_file(Monitor *mon, const QDict *qdict)
{
    const char* device_id = qdict_get_str(qdict, "dev");
    const char* file_path = qdict_get_str(qdict, "filename");
    const char* destiation = qdict_get_str(qdict, "destination");
   
    UT_array* blocks;
    int cnt = 0;
    TSK_DADDR_T *p = NULL;
    uint64_t haddr_img_offset = 0;


    //temporary hardcode image path
    const char* imgname = get_imagepath_by_block_id(device_id);;

    blocks = tsk_find_haddr_by_filename(imgname, file_path);
    if(blocks==NULL)
    {
        monitor_printf(mon, "No such file found\n");
        return;
    }

    sort_and_merge_continuous_address(blocks);
    for (p=(TSK_DADDR_T*)utarray_front(blocks);
            p != NULL;
            p=(TSK_DADDR_T*)utarray_next(blocks, p), cnt++ )
    {
        haddr_img_offset = p[0];
    }

    //temporary hardcode image path
    imgname = get_imagepath_by_block_id(device_id);

    if(imgname == NULL)
    {
        monitor_printf(mon, "Cannot get file path\n");
    }
 
    tsk_get_file(imgname, haddr_img_offset, file_path, destiation );
}

int get_hive_file( Monitor *mon, const char* sourcePath, const char* destination) {
    char* device_id;
    UT_array* blocks;
    int cnt = 0;
    TSK_DADDR_T *p = NULL;
    uint64_t haddr_img_offset = 0;
    //temporary hardcode image path   
    device_id = calloc( 10, sizeof(char*));
    strcpy( device_id, "ide0-hd0" );
    const char* imgname = get_imagepath_by_block_id(device_id);

    blocks = tsk_find_haddr_by_filename(imgname, sourcePath);
    if(blocks==NULL)
    {
        monitor_printf(mon, "No such file found\n");
        return -1;
    }

    sort_and_merge_continuous_address(blocks);
    for (p=(TSK_DADDR_T*)utarray_front(blocks);
            p != NULL;
            p=(TSK_DADDR_T*)utarray_next(blocks, p), cnt++ )
    {
        haddr_img_offset = p[0];
    }

    //temporary hardcode image path
    imgname = get_imagepath_by_block_id(device_id);
    if(imgname == NULL)
    {
        monitor_printf(mon, "Cannot get image by path\n");
        return -1;
    }
 
    tsk_get_file(imgname, haddr_img_offset, sourcePath, destination );
    return 1;
}

static char* get_imagepath_by_block_id(const char* dev_id)
{
    BlockInfoList *block_list, *info;
    block_list = qmp_query_block(NULL);

    for (info = block_list; info; info = info->next) {
        if(info->value->has_inserted)
        {
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
static void merge_continuous_address(UT_array *arr){
    int i;
    TSK_DADDR_T *prev;
    TSK_DADDR_T *p;
    int len;
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
void download_hive_to_tmp(Monitor *mon) {
    if ( get_hive_file(mon,"/Windows/System32/config/SAM", "./SAM") < 0 )
        monitor_printf(mon, "Download SAM to tmp failed\n"); 
    if ( get_hive_file(mon,"/Windows/System32/config/SYSTEM", "./SYSTEM") < 0 )
        monitor_printf(mon, "Download SYSTEM to tmp failed\n");
    if ( get_hive_file(mon,"/Windows/System32/config/SECURITY", "./SECURITY") < 0 )
        monitor_printf(mon, "Download SECURITY to tmp failed\n");
    if ( get_hive_file(mon,"/Windows/System32/config/SOFTWARE", "./SOFTWARE") < 0 )
        monitor_printf(mon, "Download SOFTWARE to tmp failed\n");
}
void do_search_registry_by_key(Monitor *mon, const QDict *qdict) {
    const char* key = qdict_get_str(qdict, "key"); 
    download_hive_to_tmp(mon);
    if( search_registry_by_key(key) != 0 )
        printf("print hive fail\n");
}  
int search_registry_by_key(const char* key) {
    info_handle_t *regfinfo_info_handle = NULL;
    libcerror_error_t *error                             = NULL;
    system_character_t *source                = NULL;
    system_character_t *keyName               = NULL;
    system_character_t *path                  = NULL;
    int verbose                                          = 0;
    int runRegistry                                      = 0;
    
    keyName = calloc( StringLength, sizeof(char*) );
    source = calloc( StringLength, sizeof(char*) );
    path = calloc( StringLength, sizeof(char*));

    libcnotify_stream_set(
            stderr,
            NULL );
    libcnotify_verbose_set(
            1 );

    if( libclocale_initialize(
            "regftools",
            &error ) != 1 )
    {
        fprintf(
                stderr,
                "Unable to initialize locale values.\n" );

        goto on_error;
    }
    if( libcsystem_initialize(
            _IONBF,
            &error ) != 1 )
    {
        fprintf(
                stderr,
                "Unable to initialize system values.\n" );

        goto on_error;
    }
    strcpy( keyName, key);
    for ( ; runRegistry < 4 ; runRegistry++ ) {
        if ( runRegistry == 0 ) {
            strcpy(source, "./SYSTEM");
        }
        else if ( runRegistry == 1 ) {
            strcpy( source, "./SAM");
        }
        else if ( runRegistry == 2 ) {
            strcpy( source, "./SOFTWARE");
        }
        else if ( runRegistry == 3 ) {
            strcpy( source, "./SECURITY");
        }

        libcnotify_verbose_set(
                verbose);
        libregf_notify_set_stream(
                stderr,
                NULL);
        libregf_notify_set_verbose(
                verbose);
        if (info_handle_initialize(
                &regfinfo_info_handle,
                &error) != 1) {
            fprintf(
                    stderr,
                    "Unable to initialize info handle.\n");

            goto on_error;
        }
        if (info_handle_open_input(
                regfinfo_info_handle,
                source,
                &error) != 1) {
            fprintf(
                    stderr,
                    "Unable to open: %"
            PRIs_SYSTEM
            ".\n",
                    source );

            goto on_error;
        }       
        if (info_handle_file_fprint_search_key(
                regfinfo_info_handle,
                keyName,
                path,
                &error) != 1) {
            fprintf(
                    stderr,
                    "Unable to print file information.\n");

            goto on_error;
        }
        if (info_handle_close_input(
                regfinfo_info_handle,
                &error) != 0) {
            fprintf(
                    stderr,
                    "Unable to close info handle.\n");

            goto on_error;
        }
        if (info_handle_free(
                &regfinfo_info_handle,
                &error) != 1) {
            fprintf(
                    stderr,
                    "Unable to free info handle.\n");

            goto on_error;
        }
    } // for

    return (EXIT_SUCCESS);

    on_error:
        if (error != NULL) {
            libcnotify_print_error_backtrace(
                    error);
            libcerror_error_free(
                    &error);
        }
        if (regfinfo_info_handle != NULL) {
            info_handle_free(
                    &regfinfo_info_handle,
                    NULL);
        }
        return (EXIT_FAILURE);

}

void pathHandle(const char* path) {
    int run = 0, len = strlen(path), counter = 0, hasLast = 0, initRun = 0;
    for ( ; initRun < StringLength ; initRun++ )
        strcpy( registryPath[initRun], "" );   
    strcpy( registrySource[0], "" );
    strcpy( registrySource[1], "" );
    for ( ; run < len ; run++ ) {
        if ( *(path+run) != '\\' ) {
           Str temp;
           sprintf( temp, "%c", *(path+run) );
           if ( counter <= 1 ) 
 	         strcat( registrySource[counter], temp );     
           else {
 	         strcat( registryPath[registryPathLen], temp );
               hasLast = 1;
           } // else
	} // if
	else if ( run != 0 && counter <= 1 )
            counter++;
        else if ( run != 0 && run != len - 1 )
	    registryPathLen++;
    } // for
 
    if ( hasLast == 1 )
        registryPathLen++;
}

void do_print_registry_by_path(Monitor *mon, const QDict *qdict) {
    const char* path = qdict_get_str(qdict, "path");
    download_hive_to_tmp(mon);
    if( print_registry_by_path(path) != 0 )
        printf("print hive fail\n");
}  
int print_registry_by_path(const char* path) {
    libcerror_error_t *error                             = NULL;
    system_character_t *source                           = NULL;
    int verbose                                          = 0;
    int runPrint                                         = 3;
    int multPrint                                        = 0;
    registryPathLen = 0;
    source = calloc( StringLength, sizeof(char*) );

    libcnotify_stream_set(
            stderr,
            NULL );
    libcnotify_verbose_set(
            1 );

    if( libclocale_initialize(
            "regftools",
            &error ) != 1 )
    {
        fprintf(
                stderr,
                "Unable to initialize locale values.\n" );

        goto on_error;
    }
    if( libcsystem_initialize(
            _IONBF,
            &error ) != 1 )
    {
        fprintf(
                stderr,
                "Unable to initialize system values.\n" );

        goto on_error;
    }

    pathHandle(path); 
    if ( strcmp( registrySource[0], "HKLM" ) == 0 &&  strcmp( registrySource[1], "SYSTEM" ) == 0 ) {
        strcpy( source, "./SYSTEM" );
    } // if
    else if ( strcmp( registrySource[0], "HKLM" ) == 0 &&  strcmp( registrySource[1], "SAM" ) == 0  ) {
        strcpy( source, "./SAM" );
    } // else if
    else if ( strcmp( registrySource[0], "HKLM" ) == 0 &&  strcmp( registrySource[1], "SECURITY" ) == 0  ) {
        strcpy( source, "./SECURITY" );     
    } // else if
    else if ( strcmp( registrySource[0], "HKLM" ) == 0 &&  strcmp( registrySource[1], "SOFTWARE" ) == 0  ) {
        strcpy( source, "./SOFTWARE" );     
    } // else if
    else if ( strcmp( registrySource[0], "HKLM" ) == 0 &&  strcmp( registrySource[1], "" ) == 0  ) {
        multPrint = 1, runPrint = 0;
    } // else if
    else {
        fprintf(
                stderr,
                "Key not found.\n" );

        return (EXIT_FAILURE);
    } // else
    for ( ; runPrint < 4 ; runPrint++ ) {
        if ( multPrint == 1 ) {
            if ( runPrint == 0 )
                strcpy( source, "./SYSTEM" );
            else if ( runPrint == 1 )
                strcpy( source, "./SAM" );
            else if ( runPrint == 2 )
                strcpy( source, "./SECURITY" );
            else if ( runPrint == 3 )
                strcpy( source, "./SOFTWARE" );
        } // if

	    libcnotify_verbose_set(
		        verbose);
	    libregf_notify_set_stream(
		        stderr,
		        NULL);
	    libregf_notify_set_verbose(
		        verbose);
	    if (info_handle_initialize(
		    &regfinfo_info_handle,
		    &error) != 1) {
		    fprintf(
		            stderr,
		            "Unable to initialize info handle.\n");

		    goto on_error;
	    }
	    if (info_handle_open_input(
		    regfinfo_info_handle,
		    source,
		    &error) != 1) {
		    fprintf(
		            stderr,
		            "Unable to open: %"
		            PRIs_SYSTEM
		            ".\n", source );

		    goto on_error;
	    }
	    if (info_handle_file_fprint_registry_path(
		    regfinfo_info_handle,
		    registryPath,
		    registryPathLen,
		    &error) != 1) {
		    fprintf(
		            stderr,
		            "Unable to print file information.\n");

		    goto on_error;
	    }
	    if (info_handle_close_input(
		    regfinfo_info_handle,
		    &error) != 0) {
		    fprintf(
		            stderr,
		            "Unable to close info handle.\n");

		    goto on_error;
	    }
	    if (info_handle_free(
		    &regfinfo_info_handle,
		    &error) != 1) {
		    fprintf(
		        stderr,
		        "Unable to free info handle.\n");

		    goto on_error;
	    }
    } // for

    return (EXIT_SUCCESS);

    on_error:
        if (error != NULL) {
            libcnotify_print_error_backtrace(
                    error);
            libcerror_error_free(
                    &error);
        }
        if (regfinfo_info_handle != NULL) {
            info_handle_free(
                    &regfinfo_info_handle,
                    NULL);
        }
        return (EXIT_FAILURE);
}

static uint64_t char_convert_uint64(const char *text)
{
    uint64_t number=0;

    for(; *text; text++)
    {
        char digit=*text-'0';           
        number=(number*10)+digit;
    }

    return number;
}
void do_print_registry_by_address(Monitor *mon, const QDict *qdict) {
    const char* address = qdict_get_str(qdict, "address");
    download_hive_to_tmp(mon);
    if( print_registry_by_address(address) == NULL )
        printf("print hive fail\n");
}  
UT_array* print_registry_by_address(const char* address) {
    const char* device_id = "ide0-hd0";
    char* filename = NULL;
    libcerror_error_t *error                             = NULL;
    system_character_t *source                           = NULL;
    int verbose = 0, runPrint = 0;
    UT_array* ret;

    filename = calloc( StringLength, sizeof(char*) );
    source = calloc( StringLength, sizeof(char*) );
    utarray_new( ret, &ut_str_icd );

    for ( ; runPrint < 1 ; runPrint++ ) {
        if ( runPrint == 0 ) {
            strcpy( filename, "/Windows/System32/config/SOFTWARE" );
            strcpy( source, "./SOFTWARE" );
        } // if
        else if ( runPrint == 1 ) {
            strcpy( filename, "/Windows/System32/config/SYSTEMM" );
            strcpy( source, "./SYSTEMM" );
        } // else if
        else if ( runPrint == 2 ) {     
            strcpy( filename, "/Windows/System32/config/SECURITY" );
            strcpy( source, "./SECURITY" );
        } // else if
        else if ( runPrint == 3 ) {            
            strcpy( filename, "/Windows/System32/config/SAM" );
            strcpy( source, "./SAM" );
        } // else if
        

    UT_array* blocks;
    int isRegistry = 0;
    uint64_t input_address = char_convert_uint64(address), offset_total = 0;
    TSK_DADDR_T *p = NULL;
    //extern key_offset_list key_list[800000];
    //temporary hardcode image path
    const char* tmp = get_imagepath_by_block_id(device_id);;

    // printf("Getting address of  %s, in image %s\n", filename, tmp);
    key_list_index = 0;
    blocks = tsk_find_haddr_by_filename(tmp, filename);
    if(blocks==NULL)
    {
        printf( "No such file found\n");
        return NULL;
    }
      
    merge_continuous_address(blocks);
    for ( p=(TSK_DADDR_T*)utarray_front(blocks);
          p != NULL;
          p=(TSK_DADDR_T*)utarray_next(blocks, p)) {
        if ( input_address >= p[0] && input_address < p[1] ) {
            break;
        } // if
        else if ((TSK_DADDR_T*)utarray_next(blocks, p) == NULL ) {
            isRegistry = 1;
            break;
        } // else if
    } // for

    if ( isRegistry == 1 )
        continue;
    printf("out\n");
    libcnotify_stream_set(
            stderr,
            NULL );
    libcnotify_verbose_set(
            1 );

    if( libclocale_initialize(
            "regftools",
            &error ) != 1 )
    {
        fprintf(
                stderr,
                "Unable to initialize locale values.\n" );

        goto on_error;
    }
    if( libcsystem_initialize(
            _IONBF,
            &error ) != 1 )
    {
        fprintf(
                stderr,
                "Unable to initialize system values.\n" );

        goto on_error;
    }

            
    libcnotify_verbose_set(verbose);
	    libregf_notify_set_stream(
		        stderr,
		        NULL);
	    libregf_notify_set_verbose(
		        verbose);
	    if (info_handle_initialize(
		    &regfinfo_info_handle,
		    &error) != 1) {
		    fprintf(
		            stderr,
		            "Unable to initialize info handle.\n");

		    goto on_error;
	    }
	    if (info_handle_open_input(
		    regfinfo_info_handle,
		    source,
		    &error) != 1) {
		    fprintf(
		            stderr,
		            "Unable to open: %"
		            PRIs_SYSTEM
		            ".\n", source );

		    goto on_error;
	    }
	    if (info_handle_file_print_by_address(
		    regfinfo_info_handle,
		    &error) != 1) {
		    fprintf(
		            stderr,
		            "Unable to print file information.\n");

		    goto on_error;
	    }
	    if (info_handle_close_input(
		    regfinfo_info_handle,
		    &error) != 0) {
		    fprintf(
		            stderr,
		            "Unable to close info handle.\n");

		    goto on_error;
	    }
	    if (info_handle_free(
		    &regfinfo_info_handle,
		    &error) != 1) {
		    fprintf(
		        stderr,
		        "Unable to free info handle.\n");

		    goto on_error;
	    }

            merge_continuous_address(blocks);
            for (p=(TSK_DADDR_T*)utarray_front(blocks);
                 p != NULL;
                 p=(TSK_DADDR_T*)utarray_next(blocks, p))
            {
                // printf("%d - %lu ~ %lu\n", cnt++, p[0], p[1]);
                if ( input_address >= p[0] && input_address < p[1] ) {
                    int search_address = 0;
                    for ( ; search_address < key_list_index ; search_address++ ) {
                       if ( offset_total + ( input_address - p[0] ) >= key_list[search_address].offset 
                         && offset_total + ( input_address - p[0] ) < key_list[search_address].offset + key_list[search_address].name_size + key_list[search_address].data_size + 40 ) {
                           const char *temp_to_push = key_list[search_address].key;
                           utarray_push_back( ret, &temp_to_push ); 
                           printf( "search key%s  key_list_index:%d\n", key_list[search_address].key, key_list_index );
                           break;
                       } // if
                    } // for 
                } // if
                else {
                    offset_total += p[1] - p[0];
                } // else
            } // for 
  
    } // for

    return ret;

    on_error:
        if (error != NULL) {
            libcnotify_print_error_backtrace(
                    error);
            libcerror_error_free(
                    &error);
        }
        if (regfinfo_info_handle != NULL) {
            info_handle_free(
                    &regfinfo_info_handle,
                    NULL);
        }
        return NULL;
}

