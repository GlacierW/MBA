/*
 *
 *  Taint ability of DBA
 *
 *  Copyright (c)    2017 JuiChien Jao
 *                   2016 ChiaWei Wang
 *
 * This library is free software you can redistribute it and/or
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

#include "dba_taint.h"

#include "qemu/thread.h"
#include "qmp-commands.h"
#include "include/utarray.h"
#include "include/exec/cpu-common.h"

// define Report Field Title for each taint analysis items
#define RFT_TAINTED_FILE       "Tainted File"
#define RFT_TAINTED_REGISTRY   "Tainted Registry"
#define RFT_BOOTCODE           "Tainted Bootcode"
#define RFT_TAINTED_PACKET     "Tainted Packet"

extern QemuMutex qemu_global_mutex;
extern hive_log logEntry[];  
extern int log_index;

UT_array* UT_SOFTWARE      = NULL;
UT_array* UT_SOFTWARE_LOG1 = NULL;
UT_array* UT_SOFTWARE_LOG2 = NULL;

UT_array* UT_SAM           = NULL;
UT_array* UT_SAM_LOG1      = NULL;
UT_array* UT_SAM_LOG2      = NULL;

UT_array* UT_SYSTEM        = NULL;
UT_array* UT_SYSTEM_LOG1   = NULL;
UT_array* UT_SYSTEM_LOG2   = NULL;

UT_array* UT_SECURITY      = NULL;
UT_array* UT_SECURITY_LOG1 = NULL;
UT_array* UT_SECURITY_LOG2 = NULL;

UT_array* UT_NTUSER        = NULL;
UT_array* UT_NTUSER_LOG1   = NULL;
UT_array* UT_NTUSER_LOG2   = NULL;

// XXX: currently use hardcoded block size & static function
// to get the device image file, These should be fixed by 
// improving TSK extension
#define GUEST_FS_BLOCKSIZE 4096
#define TEMP_SIZE 20

static int sort_string( const void* a, const void* b ) {
    
    const char* str_a = *(const char**)a;
    const char* str_b = *(const char**)b;
    return strcmp( str_a, str_b );
}

static const char* get_device_image( const char* dev ) {

    BlockInfoList* blk_list;
    BlockInfoList* info;

    blk_list = qmp_query_block( NULL );
    for( info = blk_list; info != NULL; info = info->next ) {
        if( info->value->has_inserted )
            if( strcmp(info->value->device, dev ) == 0 )
                return info->value->inserted->file;
    }
    return NULL;
}

// Print out the content in buf into json_packet_content if the length of json_packet_content is not full
// Return 0 if success, 1 for full json_packet_content or fail
static int json_packet_content_sprintf( char* json_packet_content, const char* fmt, ... ) {

    char    tmp[DBA_MAX_TAINT_PACKET_LENGTH];
    va_list args;
    
    va_start( args, fmt );
    vsprintf( tmp, fmt, args );
    va_end( args );

    if ( strlen( json_packet_content ) + strlen( tmp ) >= DBA_MAX_TAINT_PACKET_LENGTH )
        return 1;

    strncat( json_packet_content, tmp, strlen(tmp) );

    return 0;

}

// Print out the payload in hex and ascii format in one line
// Return none
static void print_taint_packet_line( char* json_packet_content, const u_char *payload, int len, int offset) {
    
    int i;
    const u_char *hex_head;
    const u_char *ascii_head;

    /* hex */
    hex_head = payload;
    for(i = 0; i < len; i++) {
        if( json_packet_content_sprintf( json_packet_content, "%02x ", hex_head[i] ) ) 
            return ;
    }
   
    /* To align the content */
    if ( len < 16 )
        for ( i = len; i < 16; i++ )
            if ( json_packet_content_sprintf( json_packet_content, "   ") )
                return ;

    if ( json_packet_content_sprintf( json_packet_content, "  ") )
        return ;

    /* print out ascii */
    ascii_head = payload;
    for(i = 0; i < len; i++) {
        if ( isprint(ascii_head[i]) ) {
            if ( json_packet_content_sprintf( json_packet_content, "%c", ascii_head[i]) )
                return ;
        }
        else {
            if ( json_packet_content_sprintf( json_packet_content, ".") )
                return ;
        }
    }

    json_packet_content_sprintf( json_packet_content, "\n");

    return;
}

// Print out the payload to the json_packet_content
// Return none
static void print_taint_packet_payload( char* json_packet_content, const u_char *payload, int len ) {
    
    int len_remain = len;
    int line_width = 16;            /* number of bytes per line */
    int line_len;
    int offset = 0;                 /* zero-based offset counter */
    const u_char *pl_head = payload;

    if (len <= 0)
        return;

    if ( json_packet_content_sprintf( json_packet_content, "Tainted Packet Payload: \n" ) )
        return;

    /* data fits on one line */
    if (len <= line_width) {
        print_taint_packet_line(json_packet_content, pl_head, len, offset);
        return;
    }

    /* data spans multiple lines */
    for ( ;; ) {
        /* compute current line length */
        line_len = line_width % len_remain;
        /* print line */
        print_taint_packet_line(json_packet_content, pl_head, line_len, offset);
        /* compute total remaining */
        len_remain = len_remain - line_len;
        /* shift pointer to remaining bytes to print */
        pl_head = pl_head + line_len;
        /* add offset */
        offset = offset + line_width;
        /* check if we have line width chars or less */
        if (len_remain <= line_width) {
            /* print last line and get out */
            print_taint_packet_line(json_packet_content, pl_head, len_remain, offset);
            break;
        }
    }

    return;
}

int set_sample_tainted( dba_context* ctx ) {
    
    const char* img;

    UT_array* fblocks;

    TSK_DADDR_T  st_haddr, 
                 ed_haddr;

    TSK_DADDR_T* haddr_tuple; 

    UT_array* fnames; 
    char**    fname;

    img     = get_device_image( "ide0-hd0" );
    fblocks = tsk_find_haddr_by_filename( img, ctx->sample_gpath );

    qemu_mutex_lock( &qemu_global_mutex );

    for( haddr_tuple  = (TSK_DADDR_T*)utarray_front(fblocks);
         haddr_tuple != NULL;
         haddr_tuple  = (TSK_DADDR_T*)utarray_next(fblocks, haddr_tuple) ) {
        st_haddr = haddr_tuple[0];
        ed_haddr = haddr_tuple[1];

        fnames = tsk_get_filename_by_haddr( img, st_haddr );

        if( fnames == NULL )
            continue;

        for( fname  = (char**)utarray_front(fnames);
             fname != NULL;
             fname  = (char**)utarray_next(fnames, fname) ) {

            if( strcasecmp(*fname, ctx->sample_gpath) != 0 ) 
                continue;

            dift_contaminate_disk_or( st_haddr, ed_haddr - st_haddr + 1, ctx->taint.tag );
            break;
        }
        utarray_free( fnames );
    }
    utarray_free( fblocks );

    qemu_mutex_unlock( &qemu_global_mutex );

    return 0;
}

int enum_tainted_file( dba_context* ctx ) {

    const char* img;

    uint64_t haddr;

    UT_array* fnames      = NULL;
    UT_array* fnames_part = NULL;
    UT_array* UT_blocks = NULL;
  
    char**    fname;
    char**    fname_prev;

    json_object* jo_taint_report;
    json_object* jo_tainted_farr;
    
    // get JSON object for taint result
    json_object_object_get_ex( ctx->result, DBA_JSON_KEY_TAINT, &jo_taint_report );

    // add array JSON object to store enumerated tainted files
    jo_tainted_farr = json_object_new_array();
    json_object_object_add( jo_taint_report, RFT_TAINTED_FILE, jo_tainted_farr );

    /// enumerate each disk blocks to search tainted blocks
    /// and recover the blocks to high-level file information
    img = get_device_image( "ide0-hd0" );
    for( haddr = 0; haddr < HD_MAX_SIZE; haddr += GUEST_FS_BLOCKSIZE ) {

        // check disk address is tainted
        if( (dift_get_disk_dirty(haddr) & ctx->taint.tag) == 0 )
            continue;
        TSK_DADDR_T *p;
        for ( p=(TSK_DADDR_T*)utarray_front(UT_SOFTWARE);
              p != NULL;
              p=(TSK_DADDR_T*)utarray_next(UT_SOFTWARE, p)) {
            if ( p[0] <= haddr && haddr < p[1] ) 
                haddr = p[1];
        } // for
        // check if tainted address corresponds to a file
        fnames_part = tsk_get_filename_by_haddr( img, haddr );
        if( fnames_part == NULL )
            continue;
 
        char ** tempPart = (char**)utarray_front(fnames_part);
        if ( tempPart == NULL )  
            continue;

        UT_blocks = tsk_find_haddr_by_filename( img, *tempPart );
        if ( UT_blocks != NULL ) {
            for ( p=(TSK_DADDR_T*)utarray_front(UT_blocks);
                  p != NULL;
                  p=(TSK_DADDR_T*)utarray_next(UT_blocks, p)) {
                if ( p[0]-1 <= haddr && haddr < p[1] ) 
                    haddr = p[1];
            } // for
        } // if
  
        if( fnames == NULL ) {
            fnames = fnames_part;
            continue;
        }
        
        utarray_concat( fnames, fnames_part );
        utarray_free( fnames_part );
    }

    // empty record, return
    if( fnames == NULL )
        return 0;

    /// add the found tainted file into DBA taint report
    /// and remove the duplicate records
    fname_prev = NULL;
    utarray_sort( fnames, sort_string );
    for( fname = (char**)utarray_front(fnames);
         fname != NULL;
         fname = (char**)utarray_next(fnames, fname) ) {

        if( fname_prev != NULL && strcmp( *fname_prev, *fname ) != 0 ) 
            json_object_array_add( jo_tainted_farr, json_object_new_string(*fname) );
        if ( fname_prev == NULL )
            json_object_array_add( jo_tainted_farr, json_object_new_string(*fname) );

        fname_prev = fname;
    }

    utarray_free( fnames );
    
    return 0;
}

static void get_registry_address(const char* img) {
    // ----SOFTWARE----

    UT_SOFTWARE = tsk_find_haddr_by_filename( img, "/Windows/System32/config/SOFTWARE"); 
    merge_continuous_address(UT_SOFTWARE);

    UT_SOFTWARE_LOG1 = tsk_find_haddr_by_filename( img, "/Windows/System32/config/SOFTWARE.LOG1");     
    merge_continuous_address(UT_SOFTWARE_LOG1);

    UT_SOFTWARE_LOG2 = tsk_find_haddr_by_filename( img, "/Windows/System32/config/SOFTWARE.LOG2");     
    merge_continuous_address(UT_SOFTWARE_LOG2);  

    // ------SAM-------

    UT_SAM = tsk_find_haddr_by_filename( img, "/Windows/System32/config/SAM");   
    merge_continuous_address(UT_SAM);

    UT_SAM_LOG1 = tsk_find_haddr_by_filename( img, "/Windows/System32/config/SAM.LOG1");   
    merge_continuous_address(UT_SAM_LOG1);

    UT_SAM_LOG2 = tsk_find_haddr_by_filename( img, "/Windows/System32/config/SAM.LOG2");   
    merge_continuous_address(UT_SAM_LOG2);

    // ----SECURITY----

    UT_SECURITY = tsk_find_haddr_by_filename( img, "/Windows/System32/config/SECURITY");
    merge_continuous_address(UT_SECURITY);
   
    UT_SECURITY_LOG1 = tsk_find_haddr_by_filename( img, "/Windows/System32/config/SECURITY.LOG1");
    merge_continuous_address(UT_SECURITY_LOG1);

    UT_SECURITY_LOG2 = tsk_find_haddr_by_filename( img, "/Windows/System32/config/SECURITY.LOG2");
    merge_continuous_address(UT_SECURITY_LOG2);

    // ----SYSTEM------

    UT_SYSTEM = tsk_find_haddr_by_filename( img, "/Windows/System32/config/SYSTEM");     
    merge_continuous_address(UT_SYSTEM); 

    UT_SYSTEM_LOG1 = tsk_find_haddr_by_filename( img, "/Windows/System32/config/SYSTEM.LOG1");     
    merge_continuous_address(UT_SYSTEM_LOG1);
 
    UT_SYSTEM_LOG2 = tsk_find_haddr_by_filename( img, "/Windows/System32/config/SYSTEM.LOG2");     
    merge_continuous_address(UT_SYSTEM_LOG2);

    // ----NTUSER------
   
    UT_NTUSER = tsk_find_haddr_by_filename( img, "/Users/dsns/NTUSER.DAT");     
    merge_continuous_address(UT_NTUSER); 

    UT_NTUSER_LOG1 = tsk_find_haddr_by_filename( img, "/Users/dsns/NTUSER.DAT.LOG1");     
    merge_continuous_address(UT_NTUSER_LOG1);
 
    UT_NTUSER_LOG2 = tsk_find_haddr_by_filename( img, "/Users/dsns/NTUSER.DAT.LOG2");     
    merge_continuous_address(UT_NTUSER_LOG2);    
}

static UT_array* search_registry_log( int hive_type, UT_array* UT_registry, UT_array* UT_log, dba_context* ctx ) {
    TSK_DADDR_T *p, *hive_haddr;
    UT_array* fnames      = NULL;
    UT_array* fnames_part = NULL;

    uint64_t haddr, offset_total = 0;

    tsk_parse_registry(hive_type); 
    for ( p=(TSK_DADDR_T*)utarray_front(UT_log);
          p != NULL;
          p=(TSK_DADDR_T*)utarray_next(UT_log, p)) {
          for( haddr = p[0] ; haddr < p[1]; haddr += 1 ) {
            // check disk address is tainted
            if( (dift_get_disk_dirty(haddr) & ctx->taint.tag) == 0 )
                continue;
            uint64_t log_offset = haddr - p[0] + offset_total, taint_registry_haddr = 0;
            int search_address = 0, log_entry_index = 0, dirty_page_index = 0;
            for ( log_entry_index = 0 ; log_entry_index < log_index ; log_entry_index++ ) {
                for ( dirty_page_index = 0 ; dirty_page_index < logEntry[log_entry_index].dirtyPage_count ; dirty_page_index++ ) {
                    if ( log_offset >= logEntry[log_entry_index].page[dirty_page_index].position_begin && 
                         log_offset < logEntry[log_entry_index].page[dirty_page_index].position_end ) {
                        uint64_t cal_hive_offset = 0;
                        for ( hive_haddr = (TSK_DADDR_T*)utarray_front(UT_registry); 
                              hive_haddr != NULL;
                              hive_haddr = (TSK_DADDR_T*)utarray_next(UT_registry, hive_haddr )) { 
                             if ( cal_hive_offset + hive_haddr[1] - hive_haddr[0]
                                  >= 
                                  logEntry[log_entry_index].page[dirty_page_index].offset ) { 

                                taint_registry_haddr = hive_haddr[0] 
                                                     + logEntry[log_entry_index].page[dirty_page_index].offset
                                                     - cal_hive_offset 
                                                     + log_offset
                                                     - logEntry[log_entry_index].page[dirty_page_index].position_begin                      
                                                     + 0x1000;
                                uint64_t temp_haddr = haddr;
                                char *temp_to_uint64;
                                temp_to_uint64 = calloc( TEMP_SIZE, sizeof(char) );
                                sprintf( temp_to_uint64, "%"PRIu64, taint_registry_haddr );
                                fnames_part = tsk_get_registry_value_by_address( temp_to_uint64, UT_registry, &search_address, &temp_haddr, hive_type );

                                if( fnames_part == NULL ) {
                                    continue;
                                } // if

                                if( fnames == NULL ) {
                                    fnames = fnames_part;
                                    continue;
                                }
        
                                utarray_concat( fnames, fnames_part );
                                utarray_free( fnames_part ); 
                             } // if

                             cal_hive_offset += hive_haddr[1] - hive_haddr[0];
                        } // for
                    } // if
                } // for
            } // for
        } // for   

        offset_total = offset_total + p[1] - p[0];
    } // for    

    return fnames;
} 

static UT_array* search_registry( int hive_type, UT_array* UT_registry, dba_context* ctx ) {
    TSK_DADDR_T *p;
    UT_array* fnames      = NULL;
    UT_array* fnames_part = NULL;
    uint64_t haddr;

    tsk_parse_registry(hive_type); 
    for ( p=(TSK_DADDR_T*)utarray_front(UT_registry);
          p != NULL;
          p=(TSK_DADDR_T*)utarray_next(UT_registry, p)) {
          for( haddr = p[0] ; haddr < p[1]; haddr += 1 ) {
            // check disk address is tainted
            if( (dift_get_disk_dirty(haddr) & ctx->taint.tag) == 0 )
                continue;
            int search_address = 0; 
            // check if tainted address corresponds to a file
            char *temp_to_uint64;
            temp_to_uint64 = calloc( TEMP_SIZE, sizeof(char) );
            sprintf( temp_to_uint64, "%"PRIu64, haddr );
            fnames_part = tsk_get_registry_value_by_address( temp_to_uint64, UT_registry, &search_address, &haddr , hive_type );
            if( fnames_part == NULL ) {
                continue;
            } // if

            if( fnames == NULL ) {
                fnames = fnames_part;
                continue;
            }
        
            utarray_concat( fnames, fnames_part );
            utarray_free( fnames_part );
        } // for   
    } // for    

    return fnames;
}
static void recovery_registry_by_log(dba_context* ctx ) {
    // For SAM.LOG
    if ( get_hive_file( "/Windows/System32/config/SAM.LOG1", "./SAM.LOG1") < 0 )
        monitor_printf( ctx->mon, "Download SAM.LOG1 failed\n");
    else 
        recovery_registry_log("./SAM.LOG1", "./SAM" );
    if ( get_hive_file( "/Windows/System32/config/SAM.LOG2", "./SAM.LOG2") < 0 )
        monitor_printf( ctx->mon, "Download SAM.LOG2 failed\n");
    else 
        recovery_registry_log("./SAM.LOG2", "./SAM" ); 

    // For SYSTEM.LOG
    if ( get_hive_file( "/Windows/System32/config/SYSTEM.LOG1", "./SYSTEM.LOG1") < 0 )
        monitor_printf( ctx->mon, "Download SYSTEM.LOG1 failed\n");
    else 
        recovery_registry_log("./SYSTEM.LOG1", "./SYSTEM" );
    if ( get_hive_file( "/Windows/System32/config/SYSTEM.LOG2", "./SYSTEM.LOG2") < 0 )
        monitor_printf( ctx->mon, "Download SYSTEM.LOG2 failed\n");
    else 
        recovery_registry_log("./SYSTEM.LOG2", "./SYSTEM" ); 

    // For SECURITY.LOG
    if ( get_hive_file( "/Windows/System32/config/SECURITY.LOG1", "./SECURITY.LOG1") < 0 )
        monitor_printf( ctx->mon, "Download SECURITY.LOG1 failed\n");
    else 
        recovery_registry_log("./SECURITY.LOG1", "./SECURITY" );
    if ( get_hive_file( "/Windows/System32/config/SECURITY.LOG2", "./SECURITY.LOG2") < 0 )
        monitor_printf( ctx->mon, "Download SECURITY.LOG2 failed\n");
    else 
        recovery_registry_log("./SECURITY.LOG2", "./SECURITY" ); 

    // For SOFTWARE.LOG
    if ( get_hive_file( "/Windows/System32/config/SOFTWARE.LOG1", "./SOFTWARE.LOG1") < 0 )
        monitor_printf( ctx->mon, "Download SOFTWARE.LOG1 failed\n");
    else 
        recovery_registry_log("./SOFTWARE.LOG1", "./SOFTWARE" );
    if ( get_hive_file( "/Windows/System32/config/SOFTWARE.LOG2", "./SOFTWARE.LOG2") < 0 )
        monitor_printf( ctx->mon, "Download SOFTWARE.LOG2 failed\n");
    else 
        recovery_registry_log("./SOFTWARE.LOG2", "./SOFTWARE" );
    /*
    // if recovery HKCU\NTUSER.DAT will fail. 
    // For NTUSER.LOG
    if ( get_hive_file( "/Users/dsns/NTUSER.DAT.LOG1", "./NTUSER.DAT.LOG1") < 0 )
        monitor_printf( ctx->mon, "Download NTUSER.DAT.LOG1 failed\n");
    else 
        recovery_registry_log("./NTUSER.DAT.LOG1", "./NTUSER.DAT" );
    if ( get_hive_file( "/Users/dsns/NTUSER.DAT.LOG2", "./NTUSER.DAT.LOG2") < 0 )
        monitor_printf( ctx->mon, "Download NTUSER.DAT.LOG2 failed\n");
    else 
        recovery_registry_log("./NTUSER.DAT.LOG2", "./NTUSER.DAT" ); 
    */
}
static void dba_download_registry( dba_context* ctx ) {
    if ( get_hive_file( "/Windows/System32/config/SAM", "./SAM") < 0 )    
        monitor_printf( ctx->mon, "Download SAM failed\n");
    if ( get_hive_file( "/Windows/System32/config/SYSTEM", "./SYSTEM") < 0 )
        monitor_printf( ctx->mon, "Download SYSTEM failed\n");
    if ( get_hive_file( "/Windows/System32/config/SECURITY", "./SECURITY") < 0 )
        monitor_printf( ctx->mon, "Download SECURITY failed\n");
    if ( get_hive_file( "/Windows/System32/config/SOFTWARE", "./SOFTWARE") < 0 )
        monitor_printf( ctx->mon, "Download SOFTWARE failed\n");
    if ( get_hive_file( "/Users/dsns/NTUSER.DAT", "./NTUSER.DAT") < 0 )
        monitor_printf( ctx->mon, "Download NTUSER.DAT failed\n");
} 
int enum_tainted_registry( dba_context* ctx ) {
    const char* img;

    UT_array* fnames      = NULL;
    UT_array* fnames_part = NULL;
  
    char**    fname;
    char**    fname_prev;

    json_object* jo_taint_report;
    json_object* jo_tainted_farr;
    
    // get JSON object for taint result
    json_object_object_get_ex( ctx->result, DBA_JSON_KEY_TAINT, &jo_taint_report );

    // add array JSON object to store enumerated tainted files
    jo_tainted_farr = json_object_new_array();
    json_object_object_add( jo_taint_report, RFT_TAINTED_REGISTRY, jo_tainted_farr );

    /// enumerate each disk blocks to search tainted blocks
    /// and recover the blocks to high-level file information
    img = get_device_image( "ide0-hd0" );
       
    dba_download_registry(ctx);
    get_registry_address(img);

    // printf("find SOFTWARE\n");
    fnames = search_registry( SOFTWARE, UT_SOFTWARE, ctx );

    // printf("find SAM\n");
    fnames_part = search_registry( SAM, UT_SAM, ctx );
    if ( fnames == NULL )
        fnames = fnames_part;
    else if ( fnames_part != NULL ) {   
        utarray_concat( fnames, fnames_part );
        utarray_free( fnames_part );
    } // else if

    
    // printf("find SYSTEM\n");
    fnames_part = search_registry( SYSTEM, UT_SYSTEM, ctx );
    if ( fnames == NULL )
        fnames = fnames_part;
    else if ( fnames_part != NULL ) {         
        utarray_concat( fnames, fnames_part );
        utarray_free( fnames_part );
    } // else if

    
    // printf("find SECURITY\n");
    fnames_part = search_registry( SECURITY, UT_SECURITY, ctx );
    if ( fnames == NULL )
        fnames = fnames_part;
    else if ( fnames_part != NULL ) {
        utarray_concat( fnames, fnames_part );        
        utarray_free( fnames_part );
    } // if

    // printf("find NTUSER\n");
    fnames_part = search_registry( NTUSER, UT_NTUSER, ctx );
    if ( fnames == NULL )
        fnames = fnames_part;
    else if ( fnames_part != NULL ) {
        utarray_concat( fnames, fnames_part );        
        utarray_free( fnames_part );
    } // if    

    recovery_registry_by_log(ctx);
    // printf("find SOFTWARE.LOG1\n");
    fnames_part = search_registry_log( SOFTWARE, UT_SOFTWARE, UT_SOFTWARE_LOG1, ctx );
    if ( fnames == NULL )
        fnames = fnames_part;
    else if ( fnames_part != NULL ) {
        utarray_concat( fnames, fnames_part ); 
        utarray_free( fnames_part );  
    } // else if

    // printf("find SOFTWARE.LOG2\n");
    fnames_part = search_registry_log( SOFTWARE, UT_SOFTWARE, UT_SOFTWARE_LOG2, ctx );
    if ( fnames == NULL )
        fnames = fnames_part;
    else if ( fnames_part != NULL ) {
        utarray_concat( fnames, fnames_part ); 
        utarray_free( fnames_part );  
    } // else if

    // printf("find SAM.LOG1\n");
    fnames_part = search_registry_log( SAM, UT_SAM, UT_SAM_LOG1, ctx );
    if ( fnames == NULL )
        fnames = fnames_part;
    else if ( fnames_part != NULL ) {
        utarray_concat( fnames, fnames_part ); 
        utarray_free( fnames_part );  
    } // else if

    // printf("find SAM.LOG2\n");
    fnames_part = search_registry_log( SAM, UT_SAM, UT_SAM_LOG2, ctx );
    if ( fnames == NULL )
        fnames = fnames_part;
    else if ( fnames_part != NULL ) {
        utarray_concat( fnames, fnames_part ); 
        utarray_free( fnames_part );  
    } // else if

    // printf("find SYSTEM.LOG1\n");
    fnames_part = search_registry_log( SYSTEM, UT_SYSTEM, UT_SYSTEM_LOG1, ctx );
    if ( fnames == NULL )
        fnames = fnames_part;
    else if ( fnames_part != NULL ) {
        utarray_concat( fnames, fnames_part ); 
        utarray_free( fnames_part );  
    } // else if

    // printf("find SYSTEM.LOG2\n");
    fnames_part = search_registry_log( SYSTEM, UT_SYSTEM, UT_SYSTEM_LOG2, ctx );
    if ( fnames == NULL )
        fnames = fnames_part;
    else if ( fnames_part != NULL ) {
        utarray_concat( fnames, fnames_part ); 
        utarray_free( fnames_part );  
    } // else if

    // printf("find SECURITY.LOG1\n");
    fnames_part = search_registry_log( SECURITY, UT_SECURITY, UT_SECURITY_LOG1, ctx );
    if ( fnames == NULL )
        fnames = fnames_part;
    else if ( fnames_part != NULL ) {
        utarray_concat( fnames, fnames_part ); 
        utarray_free( fnames_part );  
    } // else if

    // printf("find SECURITY.LOG2\n");
    fnames_part = search_registry_log( SECURITY, UT_SECURITY, UT_SECURITY_LOG2, ctx );
    if ( fnames == NULL )
        fnames = fnames_part;
    else if ( fnames_part != NULL ) {
        utarray_concat( fnames, fnames_part ); 
        utarray_free( fnames_part );  
    } // else if

    // empty record, return
    if( fnames == NULL ) {
        printf( "empty record\n");
        return 0;
    } // if

    /// add the found tainted file into DBA taint report
    /// and remove the duplicate records
    fname_prev = NULL;
    utarray_sort( fnames, sort_string );
    for( fname = (char**)utarray_front(fnames);
         fname != NULL;
         fname = (char**)utarray_next(fnames, fname) ) {

        if( fname_prev != NULL && strcmp( *fname_prev, *fname ) != 0 ) 
            json_object_array_add( jo_tainted_farr, json_object_new_string(*fname) );
        if ( fname_prev == NULL )
            json_object_array_add( jo_tainted_farr, json_object_new_string(*fname) );
        fname_prev = fname;
    }

    utarray_free( fnames );
    
    return 0;
}

void* tainted_packet_cb( size_t len, uint64_t packet_haddr, void* ctx ) {
    
    uint64_t        begin, end;
    u_char*         buf;
    char            json_packet_content[DBA_MAX_TAINT_PACKET_LENGTH] = {0};
    char            tmp[DBA_MAX_TAINT_PACKET_LENGTH] = {0};
    int             print_len;
    packet_info*    packet_ptr;

    json_object*    jo_taint_report;
    json_object*    jo_taint_packet;
    
    // get JSON object for taint result
    json_object_object_get_ex( ((dba_context*)ctx)->result, DBA_JSON_KEY_TAINT, &jo_taint_report );
    
    // get JSON object for tainted result
    // check if the JSON object is already in the tainted result
    if ( !json_object_object_get_ex( jo_taint_report, RFT_TAINTED_PACKET, &jo_taint_packet ) ) {
        // add array JSON object to store tainted packet
        jo_taint_packet = json_object_new_array();
        json_object_object_add( jo_taint_report, RFT_TAINTED_PACKET, jo_taint_packet );
    }

    begin = packet_haddr;
    end = begin + len;

    for(; begin < end; ++begin ) {
        if( dift_get_memory_dirty(begin) && ((dba_context*)ctx)->state == DBA_TASK_BUSY ) {
            buf = (u_char*)calloc( 1, len );
            packet_ptr = (packet_info*)calloc( 1, sizeof(packet_info) );
            cpu_physical_memory_read( packet_haddr, buf, len );
            if ( nettramon_packet_parse( buf, len, packet_ptr ) == 0 ) {
                switch( packet_ptr->packet_protocol ) {
                    // Notice that the inet_ntoa puts string to a static buffer and returns the pointer of the static buffer,
                    // which results in printing the same IP address if call it many times in the same sprintf.
                    // Separate the calling operations into multiple sprintf and get the right IP address.
                    case NETTRAMON_PRO_TCP:
                        print_len = sprintf( tmp, "------TCP Packet------\nFrom: %s\t\t : ",inet_ntoa(packet_ptr->ip_head->ip_src)  );
                        strncat( json_packet_content, tmp, print_len );
                        print_len = sprintf( tmp, "%d\nTo:   ",                             packet_ptr->tcp_head->th_sport          );
                        strncat( json_packet_content, tmp, print_len );
                        print_len = sprintf( tmp, "%s\t : ",                                inet_ntoa(packet_ptr->ip_head->ip_dst)  );
                        strncat( json_packet_content, tmp, print_len );
                        print_len = sprintf( tmp, "%d\n",                                   packet_ptr->tcp_head->th_dport          );
                        strncat( json_packet_content, tmp, print_len );
                        break; 
                    case NETTRAMON_PRO_UDP:
                        print_len = sprintf( tmp, "------UDP Packet------\nFrom: %s\t\t : ",inet_ntoa(packet_ptr->ip_head->ip_src)  );
                        strncat( json_packet_content, tmp, print_len );
                        print_len = sprintf( tmp, "%d\nTo:   ",                             packet_ptr->udp_head->uh_sport          );
                        strncat( json_packet_content, tmp, print_len );
                        print_len = sprintf( tmp, "%s\t : ",                                inet_ntoa(packet_ptr->ip_head->ip_dst)  );
                        strncat( json_packet_content, tmp, print_len );
                        print_len = sprintf( tmp, "%d\n",                                   packet_ptr->udp_head->uh_dport          );
                        strncat( json_packet_content, tmp, print_len );
                        break;
                    case NETTRAMON_PRO_ICMP:
                        print_len = sprintf( tmp, "------ICMP Packet-----\nFrom: %s \n",    inet_ntoa(packet_ptr->ip_head->ip_src)  );
                        strncat( json_packet_content, tmp, print_len );
                        print_len = sprintf( tmp, "To:   %s\n",                             inet_ntoa(packet_ptr->ip_head->ip_dst)  );
                        strncat( json_packet_content, tmp, print_len );
                        break;
                    case NETTRAMON_PRO_UNKNOWN:
                    default:
                        print_len = sprintf( tmp, "------UNKNOWN packet------" );
                        strncat( json_packet_content, tmp, print_len );
                        break;
                }
                print_taint_packet_payload( json_packet_content, packet_ptr->payload, packet_ptr->len );
                json_object_array_add( jo_taint_packet, json_object_new_string( json_packet_content ) );
            }
            break;   
        }
    }
    return NULL;
}

