#include "monitor/monitor.h"

#include "ext/dba/dba.h"
#include "ext/dba/dba-commands.h"

#define PROMPT_DBA_Q1 "DBA - Enable taint analysis? [Y/N]: "
#define PROMPT_DBA_Q2 "DBA - Taint tag [1~127]: "
#define PROMPT_DBA_Q3 "DBA - Enable syscall tracer? [Y/N]: "
#define PROMPT_DBA_Q4 "DBA - Start analysis? [Y/N]: "

static void cb_dba_confirm( void* mon, const char* yn, void* opaque );
static void cb_dba_set_syscall( void* mon, const char* yn, void* opaque );
static void cb_dba_set_taint_tag( void* mon, const char* yn, void* opaque );
static void cb_dba_set_taint( void* mon, const char* yn, void* opaque );

static void show_dba_context_info( Monitor* mon, const dba_context* ctx ) {

    if( mon == NULL || ctx == NULL )
        return;

    monitor_printf( mon, "Task ID:         %d\n", ctx->task_id );
    monitor_printf( mon, "Host sample:     %s\n", ctx->sample_hpath );
    monitor_printf( mon, "Guest sample:    %s\n", ctx->sample_gpath );
    monitor_printf( mon, "Taint analysis:  %s\n", (ctx->taint.is_enabled)? "Enabled" : "Disabled" );
    if( ctx->taint.is_enabled )
        monitor_printf( mon, "Taint tag:       %d\n", ctx->taint.tag );
    monitor_printf( mon, "Syscall tracer:  %s\n", (ctx->syscall.is_enabled)? "Enabled" : "Disabled" );
}

static inline void show_dba_report_title( Monitor* mon, const char* str ) {
    monitor_printf( mon, "========== %s ==========\n", str );
}

// DBA - confirm ?
static void cb_dba_confirm( void* mon, const char* yn, void* opaque ) {

    // confirmed, start analysis
    if( strcasecmp( "y", yn ) == 0 || strcasecmp( "yes", yn ) == 0 ) {
        dba_start_analysis( (DBA_TID)opaque );
        monitor_read_command( mon, 1 );
        return;
    }

    // goto taint
    if( strcasecmp( "n", yn ) == 0 || strcasecmp( "no", yn ) == 0 ) {
        dba_delete_task( (DBA_TID)opaque );
        monitor_read_command( mon, 1 );
        return;
    }

    // stay confirm
    mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q4, 0, cb_dba_confirm, opaque );
    mba_readline_show_prompt( mon );
}

// DBA - syscall ?
static void cb_dba_set_syscall( void* mon, const char* yn, void* opaque ) {

    // goto confirm
    if( strcasecmp( "y", yn ) == 0 || strcasecmp( "yes", yn ) == 0 ) {
        dba_enable_syscall_trace( (DBA_TID)opaque );
        show_dba_context_info( mon, dba_get_task_context((DBA_TID)opaque) );
        mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q4, 0, cb_dba_confirm, opaque );
        mba_readline_show_prompt( mon );
        return;
    }

    // goto confirm
    if( strcasecmp( "n", yn ) == 0 || strcasecmp( "no", yn ) == 0 ) {
        dba_disable_syscall_trace( (DBA_TID)opaque );
        show_dba_context_info( mon, dba_get_task_context((DBA_TID)opaque) );
        mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q4, 0, cb_dba_confirm, opaque );
        mba_readline_show_prompt( mon );
        return;
    }

    // stay syscall
    mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q3, 0, cb_dba_set_syscall, opaque );
    mba_readline_show_prompt( mon );
}

// DBA - taint tag ?
static void cb_dba_set_taint_tag( void* mon, const char* tag_str, void* opaque ) {

    CONTAMINATION_RECORD tag;

    long int tmp;
    char* end;

    // invalidate taint tag
    tmp = strtol( tag_str, &end, 10 );
    if( *end != '\0' || tmp < 0x1 || tmp > 0x7f ) {
        // stay taint tag
        mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q2, 0, cb_dba_set_taint_tag, opaque );
        mba_readline_show_prompt( mon );
        return;
    }

    tag = (CONTAMINATION_RECORD)(tmp & 0x00000000000000ff);

    // goto syscall
    dba_enable_taint_analysis( (DBA_TID)opaque, tag );
    mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q3, 0, cb_dba_set_syscall, opaque );
    mba_readline_show_prompt( mon );
    return;
}

// DBA - taint ?
static void cb_dba_set_taint( void* mon, const char* yn, void* opaque ) {

    // goto taint tag
    if( strcasecmp( "y", yn ) == 0 || strcasecmp( "yes", yn ) == 0 ) {
        mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q2, 0, cb_dba_set_taint_tag, opaque );
        mba_readline_show_prompt( mon );
        return;
    }

    // goto syscall
    if( strcasecmp( "n", yn ) == 0 || strcasecmp( "no", yn ) == 0 ) {
        dba_disable_taint_analysis( (DBA_TID)opaque );
        mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q3, 0, cb_dba_set_syscall, opaque );
        mba_readline_show_prompt( mon );
        return;
    }
    
    // stay taint
    mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q1, 0, cb_dba_set_taint, opaque );
    mba_readline_show_prompt( mon );
}

void do_start_dba_task( Monitor* mon, const QDict* qdict ) {

    DBA_TID tid;

    const char* sample = qdict_get_str( qdict, "sample" );
    uint32_t timer     = qdict_get_int( qdict, "timer" );

    // allocate new DBA context
    tid = dba_new_task();
    if( tid == -1 ) {

        monitor_printf( mon, "Fail to new a DBA task: " );
        switch( dba_errno ) {
            case DBA_ERR_TASK_FULL:
                monitor_printf( mon, "Full task\n" );
                break;
            default:
                monitor_printf( mon, "General failure\n" );
                break;
        }

        return;
    }

    // setup sample to be analyzed
    if( dba_set_sample( tid, sample ) != 0 ) {

        monitor_printf( mon, "Fail to set subject sample for DBA task: " );
        switch( dba_errno ) {
            case DBA_ERR_INVALID_TID:
                monitor_printf( mon, "Invalid task ID\n" );
                break;
            case DBA_ERR_INVALID_TSTATE:
                monitor_printf( mon, "Task unconfigurable\n" );
                break;
            case DBA_ERR_INVALID_SAMPLE:
                monitor_printf( mon, "Invalid sample\n" );
                break;
            default:
                monitor_printf( mon, "General failure\n" );
                break;
        }

        dba_delete_task( tid );
        return;
    }

    // setup execution timer for the sample
    if( dba_set_timer( tid, timer ) != 0 ) {

        monitor_printf( mon, "Fail to set execution timer for DBA task\n" );
        switch( dba_errno ) {
            case DBA_ERR_INVALID_TID:
                monitor_printf( mon, "Invalid task ID\n" );
                break;
            case DBA_ERR_INVALID_TSTATE:
                monitor_printf( mon, "Task unconfigurable\n" );
                break;
            default:
                monitor_printf( mon, "General failure\n" );
                break;
        }
        dba_delete_task( tid );
        return;
    }

    // callback chain as questions to setup DBA context
    //  1. ask user for taint option
    //  2. if taint is enabled, ask taint tag
    //  3. ask user for syscall trace option
    //  4. final confirm & launch analysis
    mba_readline_start( (void*)mon, PROMPT_DBA_Q1, 0, cb_dba_set_taint, (void*)tid );
}

void do_list_dba_task( Monitor* mon, const QDict* qdict ) {

    const dba_context* ctx;

    int i,
        tid = qdict_get_try_int( qdict, "tid", -1 );

    monitor_printf( mon, "%8s%8s%12s%12s%12s\t%s -> %s\n", "TaskID", "State", "Taint(Tag)", "Syscall", "Timer", "HostSample", "GuestSample" );
    monitor_printf( mon, "===================================================================================================\n" );

    // show targeted task
    if( tid != -1 ) {
        ctx = dba_get_task_context( tid );
        if( ctx == NULL )
            return;

        monitor_printf( mon, "%8d%8s%7s(%3d)%12s%12zu\t%s -> %s\n",
                        tid,
                        (ctx->state == DBA_TASK_BUSY)? "BUSY" : (ctx->state == DBA_TASK_DONE)? "DONE" : "IDLE",
                        (ctx->taint.is_enabled)? "TRUE" : "FALSE",
                        (ctx->taint.is_enabled)? ctx->taint.tag : 0 ,
                        (ctx->syscall.is_enabled)? "TRUE" : "FALSE",
                        ctx->sample_timer,
                        ctx->sample_hpath,
                        ctx->sample_gpath );
        return;
    }

    // enumerate
    for( i = 0; i < DBA_MAX_TASKS; ++i ) {
        
        ctx = dba_get_task_context( i );
        if( ctx == NULL )
            continue;

        monitor_printf( mon, "%8d%8s%7s(%3d)%12s%12zu\t%s -> %s\n",
                        i,
                        (ctx->state == DBA_TASK_BUSY)? "BUSY" : (ctx->state == DBA_TASK_DONE)? "DONE" : "IDLE",
                        (ctx->taint.is_enabled)? "TRUE" : "FALSE",
                        (ctx->taint.is_enabled)? ctx->taint.tag : 0 ,
                        (ctx->syscall.is_enabled)? "TRUE" : "FALSE",
                        ctx->sample_timer,
                        ctx->sample_hpath,
                        ctx->sample_gpath );
    }
}

void do_delete_dba_task( Monitor* mon, const QDict* qdict ) {
    
    int tid = qdict_get_int( qdict, "tid" );

    if( dba_delete_task(tid) == 0 )
        return;

    monitor_printf( mon, "Fail to delete the DBA task: " );
    switch( dba_errno ) {
        case DBA_ERR_INVALID_TID:
            monitor_printf( mon, "Invalid task ID\n" );
            break;
        case DBA_ERR_INVALID_TSTATE:
            monitor_printf( mon, "Invalid task state, only IDLE/DONE task can be deleted\n" );
            break;
        default:
            monitor_printf( mon, "General failure\n" );
            break;
    }
}

void do_show_dba_result( Monitor* mon, const QDict* qdict ) {

    const dba_context* ctx;

    json_object* taint_report;

    int i,
        tid = qdict_get_int( qdict, "tid" );

    ctx = dba_get_task_context( tid );
    if( ctx == NULL ) {
        monitor_printf( mon, "No corresponding DBA context\n" );
        return;
    }

    if( ctx->state != DBA_TASK_DONE ) {
        monitor_printf( mon, "DBA task not done\n" );
        return;
    }

    // show summary
    show_dba_report_title( mon, "SUMMARY" );
    show_dba_context_info( mon, ctx );

    // show taint result if enabled
    if( ctx->taint.is_enabled ) {
        json_object_object_get_ex( ctx->result, DBA_JSON_KEY_TAINT, &taint_report );
        show_dba_report_title( mon, DBA_JSON_KEY_TAINT );
        json_object_object_foreach( taint_report, taint_field, taint_records  ) {
            monitor_printf( mon, "%s:\n", taint_field );

            for( i = 0; i < json_object_array_length(taint_records); ++i ) {
                monitor_printf( mon, "    %s\n",
                        json_object_get_string(json_object_array_get_idx(taint_records, i)) );
            }
        }
    }
}
