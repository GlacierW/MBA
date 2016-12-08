#include "monitor/monitor.h"

#include "ext/dba/dba.h"
#include "ext/dba/dba-commands.h"

#define PROMPT_DBA_Q1 "DBA - Enable taint analysis? [Y/N]: "
#define PROMPT_DBA_Q2 "DBA - Taint tag [1~7]: "
#define PROMPT_DBA_Q3 "DBA - Enable syscall tracer? [Y/N]: "
#define PROMPT_DBA_Q4 "DBA - Start analysis? [Y/N]: "

static void cb_dba_confirm( void* mon, const char* yn, void* opaque );
static void cb_dba_set_syscall( void* mon, const char* yn, void* opaque );
static void cb_dba_set_taint_tag( void* mon, const char* yn, void* opaque );
static void cb_dba_set_taint( void* mon, const char* yn, void* opaque );

static void show_dba_context_info( Monitor* mon, dba_context* ctx ) {

    if( mon == NULL || ctx == NULL )
        return;

    monitor_printf( mon, "\n" );
    monitor_printf( mon, "Host sample:     %s\n", ctx->sample_hpath );
    monitor_printf( mon, "Guest sample:    %s\n", ctx->sample_gpath );
    monitor_printf( mon, "Taint analysis:  %s\n", (ctx->taint.is_enabled)? "Enabled" : "Disabled" );
    if( ctx->taint.is_enabled )
        monitor_printf( mon, "Taint tag:       %d\n", ctx->taint.tag );
    monitor_printf( mon, "Syscall tracer:  %s\n", (ctx->syscall.is_enabled)? "Enabled" : "Disabled" );
}

// DBA - confirm ?
static void cb_dba_confirm( void* mon, const char* yn, void* opaque ) {

    // confirmed, start analysis
    if( strcasecmp( "y", yn ) == 0 || strcasecmp( "yes", yn ) == 0 ) {
        dba_start_analysis( (dba_context*)opaque );
        monitor_read_command( mon, 1 );
        return;
    }

    // goto taint
    if( strcasecmp( "n", yn ) == 0 || strcasecmp( "no", yn ) == 0 ) {
        monitor_read_command( mon, 1 );
        return;
    }

    // stay confirm
    mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q4, 0, cb_dba_confirm, opaque );
    mba_readline_show_prompt( mon );
}

// DBA - syscall ?
static void cb_dba_set_syscall( void* mon, const char* yn, void* opaque ) {

    dba_context* ctx = (dba_context*)opaque;

    // goto confirm
    if( strcasecmp( "y", yn ) == 0 || strcasecmp( "yes", yn ) == 0 ) {
        dba_enable_syscall_trace( (dba_context*)opaque );
        show_dba_context_info( mon, ctx );
        mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q4, 0, cb_dba_confirm, opaque );
        mba_readline_show_prompt( mon );
        return;
    }

    // goto confirm
    if( strcasecmp( "n", yn ) == 0 || strcasecmp( "no", yn ) == 0 ) {
        dba_disable_syscall_trace( (dba_context*)opaque );
        show_dba_context_info( mon, ctx );
        mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q4, 0, cb_dba_confirm, opaque );
        mba_readline_show_prompt( mon );
        return;
    }

    // stay syscall
    mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q3, 0, cb_dba_set_syscall, opaque );
    mba_readline_show_prompt( mon );
}

// DBA - taint tag ?
static void cb_dba_set_taint_tag( void* mon, const char* tag, void* opaque ) {

    // goto syscall
    if( strlen(tag) == 1 && tag[0] > '0' && tag[0] < '8' ) {
        dba_enable_taint_analysis( (dba_context*)opaque, (CONTAMINATION_RECORD)(tag[0] - '0') );
        mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q3, 0, cb_dba_set_syscall, opaque );
        mba_readline_show_prompt( mon );
        return;
    }
    
    // stay taint tag
    mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q2, 0, cb_dba_set_taint_tag, opaque );
    mba_readline_show_prompt( mon );
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
        dba_disable_taint_analysis( (dba_context*)opaque );
        mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q3, 0, cb_dba_set_syscall, opaque );
        mba_readline_show_prompt( mon );
        return;
    }
    
    // stay taint
    mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q1, 0, cb_dba_set_taint, opaque );
    mba_readline_show_prompt( mon );
}

void do_start_dba_task( Monitor* mon, const QDict* qdict ) {

    const char* sample = qdict_get_str( qdict, "sample" );
    uint32_t timer     = qdict_get_int( qdict, "timer" );

    // allocate new DBA context
    dba_context* ctx = dba_alloc_context();
    if( ctx == NULL ) {
        monitor_printf( mon, "Fail to allocate DBA context\n" );
        return;
    }

    // setup sample to be analyzed
    if( dba_set_sample( ctx, sample ) != 0 ) {
        monitor_printf( mon, "Fail to set subject sample for DBA task\n" );
        dba_free_context( ctx );
        return;
    }

    // setup execution timer for the sample
    if( dba_set_timer( ctx, timer ) != 0 ) {
        monitor_printf( mon, "Fail to set execution timer for DBA task\n" );
        dba_free_context( ctx );
        return;
    }

    // callback chain as questions to setup DBA context
    //  1. ask user for taint option
    //  2. if taint is enabled, ask taint tag
    //  3. ask user for syscall trace option
    //  4. final confirm & launch analysis
    mba_readline_start( (void*)mon, PROMPT_DBA_Q1, 0, cb_dba_set_taint, (void*)ctx );
}

void do_list_dba_task( Monitor* mon, const QDict* qdict ) {

    int i;
    dba_context* ctx;

    monitor_printf( mon, "%8s%8s%12s%12s%12s\t%s -> %s\n", "TaskID", "State", "Taint(Tag)", "Syscall", "Timer", "HostSample", "GuestSample" );
    monitor_printf( mon, "===================================================================================================\n" );

    for( i = 0; i < DBA_MAX_TASKS; ++i ) {
        
        ctx = dba_tasks[i];
        if( ctx == NULL )
            break;

        monitor_printf( mon, "%8d%8s%7s(%3d)%12s%12zu\t%s -> %s\n",
                        i,
                        (ctx->state == DBA_TASK_BUSY)? "BUSY" : "DONE",
                        (ctx->taint.is_enabled)? "TRUE" : "FALSE",
                        (ctx->taint.is_enabled)? ctx->taint.tag : 0 ,
                        (ctx->syscall.is_enabled)? "TRUE" : "FALSE",
                        ctx->sample_timer,
                        ctx->sample_hpath,
                        ctx->sample_gpath );
    }
}
