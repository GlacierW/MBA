#include "dba_taint.h"

#include "qemu/thread.h"
#include "qmp-commands.h"
#include "include/utarray.h"

// define Report Field Title for each taint analysis items
#define RFT_TAINTED_FILE       "Tainted File"
#define RFT_TAINTED_REGISTRY   "Tainted Registry"
#define RFT_BOOTCODE           "Tainted Bootcode"

extern QemuMutex qemu_global_mutex;

// XXX: currently use hardcoded block size & static function
// to get the device image file, These should be fixed by 
// improving TSK extension
#define GUEST_FS_BLOCKSIZE 4096

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

        // check if tainted address corresponds to a file
        fnames_part = tsk_get_filename_by_haddr( img, haddr );
        if( fnames_part == NULL )
            continue;

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

        fname_prev = fname;
    }
    utarray_free( fnames );

    return 0;
}

int enum_tainted_registry( dba_context* ctx ) {

    // TODO: tainted registry parsing
    return 1;
}
