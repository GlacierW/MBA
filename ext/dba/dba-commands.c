#include "monitor/monitor.h"

#include "ext/dba/dba.h"
#include "ext/dba/dba-commands.h"
#include "ext/agent/agent-commands.h"

#define PROMPT_DBA_Q1 "DBA - Enable taint analysis? [y/n]: "
#define PROMPT_DBA_Q2 "DBA - Taint tag [1~127]: "
#define PROMPT_DBA_Q3 "DBA - Enable syscall tracer? [y/n]: "
#define PROMPT_DBA_Q4 "DBA - Start analysis? [y/n]: "

#define CONFIG_TAG_MODULE          "MODULE"
#define CONFIG_TAG_AGENT           "AGENT"
#define CONFIG_TAG_DIFT            "DIFT"
#define CONFIG_TAG_OBHOOK          "OBHOOK"
#define CONFIG_TAG_NETTRAMON       "NETTRAMON"
#define CONFIG_TAG_TAINT           "TAINT"
#define CONFIG_TAG_ENABLE          "ENABLE"
#define CONFIG_TAG_YES             "yes"
#define CONFIG_TAG_Y               "y"
#define CONFIG_TAG_NO              "no"
#define CONFIG_TAG_TAINT_TAG       "TAG"
#define CONFIG_TAG_SYSCALL_TRACER  "SYSCALL_TRACER"

static void cb_dba_confirm( void* mon, const char* yn, void* opaque );
static void cb_dba_set_syscall( void* mon, const char* yn, void* opaque );
static void cb_dba_set_taint_tag( void* mon, const char* yn, void* opaque );
static void cb_dba_set_taint( void* mon, const char* yn, void* opaque );

bool use_config_file = FALSE;

static void show_dba_context_info( Monitor* mon, FILE* fp, const dba_context* ctx ) {

    if( (fp == NULL && mon == NULL) || ctx == NULL )
        return;

    if ( fp != NULL ) {
        fprintf( fp, "Task ID:         %d\n", ctx->task_id );
        fprintf( fp, "Host sample:     %s\n", ctx->sample_hpath );
        fprintf( fp, "Guest sample:    %s\n", ctx->sample_gpath );
        fprintf( fp, "Taint analysis:  %s\n", (ctx->taint.is_enabled)? "Enabled" : "Disabled" );
        if( ctx->taint.is_enabled )
            fprintf( fp, "Taint tag:       %d\n", ctx->taint.tag );
        fprintf( fp, "Syscall tracer:  %s\n", (ctx->syscall.is_enabled)? "Enabled" : "Disabled" );
    }
    else {
        monitor_printf( mon, "Task ID:         %d\n", ctx->task_id );
        monitor_printf( mon, "Host sample:     %s\n", ctx->sample_hpath );
        monitor_printf( mon, "Guest sample:    %s\n", ctx->sample_gpath );
        monitor_printf( mon, "Taint analysis:  %s\n", (ctx->taint.is_enabled)? "Enabled" : "Disabled" );
        if( ctx->taint.is_enabled )
            monitor_printf( mon, "Taint tag:       %d\n", ctx->taint.tag );
        monitor_printf( mon, "Syscall tracer:  %s\n", (ctx->syscall.is_enabled)? "Enabled" : "Disabled" );
    }
}

static inline void show_dba_report_title( Monitor* mon, FILE* fp, const char* str ) {
    if ( ( mon == NULL && fp == NULL ) || str == NULL )
        return;
    if ( fp != NULL )
        fprintf( fp, "========== %s ==========\n", str );
    else
        monitor_printf( mon, "========== %s ==========\n", str );
}

// DBA - confirm ?
static void cb_dba_confirm( void* mon, const char* yn, void* opaque ) {

    monitor_printf( mon, "========== Task Info ==========\n" );
    show_dba_context_info( mon, NULL, dba_get_task_context((DBA_TID)opaque) );
   
    // confirmed, start analysis
    if( strcasecmp( "y", yn ) == 0 || strcasecmp( "yes", yn ) == 0 ) {
        // initiate DBA analysis
        if( dba_start_analysis((DBA_TID)opaque) != 0 ) {

            monitor_printf( mon, "\n****** ERROR ******\n" );
            
            switch( dba_errno ) {
                case DBA_ERR_INVALID_TID:
                    monitor_printf( mon, "Invalid DBA task ID\n" );
                    break;

                case DBA_ERR_INVALID_TSTATE:
                    monitor_printf( mon, "Invalid DBA task state: Not a IDLE task\n" );
                    break;
                
                case DBA_ERR_INVALID_SAMPLE:
                    monitor_printf( mon, "Invalid sample for DBA analysis\n" );
                    break;

                case DBA_ERR_AGENT_NOTREADY:
                    monitor_printf( mon, "Agent is not ready\n" );
                    break;

                case DBA_ERR_DIFT_NOTREADY:
                    monitor_printf( mon, "DIFT is not ready\n" );
                    break;

                case DBA_ERR_FAIL:
                default:
                    monitor_printf( mon, "Failed to start the DBA task\n" );
                    break;
            }
            dba_delete_task( (DBA_TID)opaque );
        }
        else {
            monitor_printf( mon, "=== Please wait for finish message ===\n" );
        }
        monitor_read_command( mon, 1 );
        return;
    }
    else {
        monitor_read_command( mon, 1 );
        return;
    }

    // abort & delete the DBA task
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
        mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q4, 0, cb_dba_confirm, opaque );
        mba_readline_show_prompt( mon );
        return;
    }

    // goto confirm
    if( strcasecmp( "n", yn ) == 0 || strcasecmp( "no", yn ) == 0 ) {
        dba_disable_syscall_trace( (DBA_TID)opaque );
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

// Set dba enviroment via config file
// Return 0 for success, 1 for fail
static int dba_set_by_config ( Monitor* mon, const char* config_file_path, void* opaque ) {
   
    json_object*    jo_config;
    json_object*    jo_taint;
    json_object*    jo_syscall;
    json_object*    jo_module;

    if ( ( jo_config = json_object_from_file( config_file_path ) ) == NULL ) {
        monitor_printf( mon, "Read config file fail\n");
        return 1;
    }

    monitor_printf( mon, "====== Start to Enable Modules ======\n\n" );

    // Enable modules
    if ( json_object_object_get_ex( (json_object*)jo_config, CONFIG_TAG_MODULE, &jo_module ) ) {

        json_object*    jo_now_module;

        // Enable Agent
        if ( json_object_object_get_ex( jo_module, CONFIG_TAG_AGENT, &jo_now_module ) ) {
            if ( strcmp( json_object_get_string( jo_now_module ), CONFIG_TAG_YES ) == 0 || strcmp( json_object_get_string( jo_now_module ), CONFIG_TAG_Y ) == 0 ) {
                do_win_init( mon, NULL );
                monitor_printf( mon, "Enable Agent Successfully\n\n");
            }
            else {
                monitor_printf( mon, "Do nothing to Agent\n\n");
            }
        }
        // Enable DIFT
        if ( json_object_object_get_ex( jo_module, CONFIG_TAG_DIFT, &jo_now_module ) ) {
            if ( strcmp( json_object_get_string( jo_now_module ), CONFIG_TAG_YES ) == 0 || strcmp( json_object_get_string( jo_now_module ), CONFIG_TAG_Y ) == 0 ) {
                //dift_enable();
            }
            else {
                monitor_printf( mon, "Do nothing to Dift\n\n");
            }
        }
        // Enable Nettramon
        if ( json_object_object_get_ex( jo_module, CONFIG_TAG_NETTRAMON, &jo_now_module ) ) {
            if ( strcmp( json_object_get_string( jo_now_module ), CONFIG_TAG_YES ) == 0 || strcmp( json_object_get_string( jo_now_module ), CONFIG_TAG_Y ) == 0 ) {
                nettramon_start( NULL );
                monitor_printf( mon, "Enable Nettramon Successfully\n\n");
            }
            else {
                monitor_printf( mon, "Do nothing to Nettramon\n\n");
            }
        }

    }
    monitor_printf( mon, "====== Finish Enabling Modules ======\n" );

    // Set Taint
    if ( json_object_object_get_ex( (json_object*)jo_config, CONFIG_TAG_TAINT, &jo_taint ) ) {
        
        json_object* jo_taint_enable;
        json_object* jo_taint_tag;
        if ( json_object_object_get_ex( jo_taint, CONFIG_TAG_ENABLE, &jo_taint_enable ) ) {
            if ( ( strcmp( json_object_get_string( jo_taint_enable ), CONFIG_TAG_YES ) == 0 || strcmp( json_object_get_string( jo_taint_enable ), CONFIG_TAG_Y ) == 0 ) && json_object_object_get_ex( jo_taint, CONFIG_TAG_TAINT_TAG, &jo_taint_tag ) ) {
                cb_dba_set_taint_tag( mon, json_object_get_string( jo_taint_tag ), opaque );
            }
            else {
                if ( strcmp( json_object_get_string( jo_taint_enable ), CONFIG_TAG_NO ) != 0 ) {
                    monitor_printf( mon, "Taint set fail\n");
                    return 1;
                }
                else {
                    cb_dba_set_taint( mon, "n", opaque ); 
                }
            }
        }
        else {
            monitor_printf( mon, "Cannot get taint enable json\n");
            return 1;
        }
    }
    else {
        monitor_printf( mon, "Cannot get taint json\n");
        return 1;
    }

    // Set Syscall tracer
    if ( json_object_object_get_ex( (json_object*)jo_config, CONFIG_TAG_SYSCALL_TRACER, &jo_syscall ) ) {
        json_object* jo_syscall_enable;
        if ( json_object_object_get_ex( jo_syscall, CONFIG_TAG_ENABLE, &jo_syscall_enable ) ) {
            if ( ( strcmp( json_object_get_string( jo_syscall_enable ), CONFIG_TAG_YES ) == 0 || strcmp( json_object_get_string( jo_syscall_enable ), CONFIG_TAG_Y ) == 0 ) ) {
                cb_dba_set_syscall( NULL, json_object_get_string( jo_syscall_enable ), opaque );
            }
            else {
                if ( strcmp( json_object_get_string( jo_syscall_enable ), CONFIG_TAG_NO ) != 0 ) {
                    monitor_printf( mon, "Syscall set fail\n");
                    return 1;
                }
                else {
                    cb_dba_set_syscall( NULL, "n", opaque );
                }
            }
        }
        else {
            monitor_printf( mon, "Cannot get syscall enable json\n");
            return 1;
        }
    }
    else {
        monitor_printf( mon, "Cannot get syscall json\n");
        return 1;
    }

    return 0;
}

void do_start_dba_task( Monitor* mon, const QDict* qdict ) {

    DBA_TID tid;

    const char* sample = qdict_get_str( qdict, "sample" );
    uint32_t timer     = qdict_get_int( qdict, "timer" );
    const char* config_file_path = qdict_get_try_str( qdict, "config" );
    
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

    // Set monitor pointer to the task in order to print out the finish message
    if ( dba_set_monitor( tid, mon ) != 0 ) {
        monitor_printf( mon, "Set monitor pointer fail\n");
        return;
    }

    // Set by the config file
    if ( config_file_path != NULL ) {

        use_config_file = TRUE;
        monitor_printf( mon, "Set with config file\n" );
        if ( dba_set_by_config( mon, config_file_path, (void*)tid ) != 0 ) {
            monitor_printf( mon, "Set task by config file fail\n");
            use_config_file = FALSE;
        }
        else {
            use_config_file = FALSE;
            cb_dba_confirm( mon, "yes", (void*)tid );
        }
    }
    else {

        use_config_file = FALSE;
        monitor_printf( mon, "Set without config file\n" );

        // callback chain as questions to setup DBA context
        //  1. ask user for taint option
        //  2. if taint is enabled, ask taint tag
        //  3. ask user for syscall trace option
        //  4. final confirm & launch analysis
        mba_readline_start( (void*)mon, PROMPT_DBA_Q1, 0, cb_dba_set_taint, (void*)tid );
    }

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
    json_object* syscall_report;

    int i,
        tid = qdict_get_int( qdict, "tid" );
    const char* file_path = qdict_get_try_str( qdict, "file" );

    ctx = dba_get_task_context( tid );
    if( ctx == NULL ) {
        monitor_printf( mon, "No corresponding DBA context\n" );
        return;
    }

    if( ctx->state != DBA_TASK_DONE ) {
        monitor_printf( mon, "DBA task not done\n" );
        return;
    }
    
    if ( file_path == NULL ) {
        // show summary
        show_dba_report_title( mon, NULL, "SUMMARY" );
        show_dba_context_info( mon, NULL, ctx );

        // show taint result if enabled
        if( ctx->taint.is_enabled ) {
            json_object_object_get_ex( ctx->result, DBA_JSON_KEY_TAINT, &taint_report );
            show_dba_report_title( mon, NULL, DBA_JSON_KEY_TAINT );
            json_object_object_foreach( taint_report, taint_field, taint_records  ) {
                monitor_printf( mon, "%s:\n", taint_field );

                for( i = 0; i < json_object_array_length(taint_records); ++i ) {
                    monitor_printf( mon, "    %s\n",
                            json_object_get_string(json_object_array_get_idx(taint_records, i)) );
                }
            }
        }
        if (ctx->syscall.is_enabled) {
            json_object_object_get_ex( ctx->result, DBA_JSON_KEY_SYSCALL, &syscall_report );
            show_dba_report_title( mon, NULL, DBA_JSON_KEY_SYSCALL );
            json_object_object_foreach( syscall_report, syscall_field, syscall_info  ) {
                monitor_printf( mon, "%s:\n", syscall_field );

                for( i = 0; i < json_object_array_length(syscall_info); ++i ) {
                    monitor_printf( mon, "    %s\n",
                            json_object_get_string(json_object_array_get_idx(syscall_info, i)) );
                }
            }
        }
    }
    else {
        FILE* fp = fopen( file_path, "a" );

        // show summary
        show_dba_report_title( NULL, fp, "SUMMARY" );
        show_dba_context_info( NULL, fp, ctx );

        // show taint result if enabled
        if( ctx->taint.is_enabled ) {
            json_object_object_get_ex( ctx->result, DBA_JSON_KEY_TAINT, &taint_report );
            show_dba_report_title( NULL, fp, DBA_JSON_KEY_TAINT );
            json_object_object_foreach( taint_report, taint_field, taint_records  ) {
                fprintf( fp, "%s:\n", taint_field );

                for( i = 0; i < json_object_array_length(taint_records); ++i ) {
                    fprintf( fp, "    %s\n",
                            json_object_get_string(json_object_array_get_idx(taint_records, i)) );
                }
            }
        }
        if (ctx->syscall.is_enabled) {
            json_object_object_get_ex( ctx->result, DBA_JSON_KEY_SYSCALL, &syscall_report );
            show_dba_report_title( NULL, fp, DBA_JSON_KEY_SYSCALL );
            json_object_object_foreach( syscall_report, syscall_field, syscall_info  ) {
                fprintf( fp, "%s:\n", syscall_field );

                for( i = 0; i < json_object_array_length(syscall_info); ++i ) {
                    fprintf( fp, "    %s\n",
                            json_object_get_string(json_object_array_get_idx(syscall_info, i)) );
                }
            }
        }

        fclose( fp );
        monitor_printf( mon, "Finished writing result to targeted file\n" );
    }
}

// Used to check which setting mode is now
// Return TRUE for setting by config file, FALSE for command line setting
bool do_dba_config_file_setting ( void ) {

    return use_config_file ;

}
