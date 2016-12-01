#include "dba_taint.h"

#include "qmp-commands.h"
#include "include/utarray.h"

extern QemuMutex qemu_global_mutex;

// XXX: currently use hardcoded block size & static function
// to get the device image file, These should be fixed by 
// improving TSK extension
#define GUEST_FS_BLOCKSIZE 4096

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

    char**    fname;
    UT_array* fnames; 

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

    UT_array* fnames;
    char**    fname;

    img = get_device_image( "ide0-hd0" );

    for( haddr = 0; haddr < HD_MAX_SIZE; haddr += GUEST_FS_BLOCKSIZE ) {
        // check disk address is tainted
        if( dift_get_disk_dirty(haddr) != ctx->taint.tag )
            continue;

        // check if tainted address corresponds to a file
        fnames = tsk_get_filename_by_haddr( img, haddr );
        if( fnames == NULL )
            continue;

        for( fname = (char**)utarray_front(fnames);
             fname != NULL;
             fname = (char**)utarray_next(fnames, fname) ) {

            printf( "%s\n", *fname );
        }
        utarray_free( fnames );
    }

    return 0;
}

int enum_tainted_registry( dba_context* ctx ) {

    return 1;
}
