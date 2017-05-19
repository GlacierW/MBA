#include "monitor/monitor.h"

#include "ext/dba/dba.h"
#include "ext/dba/dba-commands.h"
#include "ext/agent/agent-commands.h"

#define PROMPT_DBA_Q1  "DBA - Enable taint analysis? [y/n]: "
#define PROMPT_DBA_Q2  "DBA - Enable tainted packet analysis? [y/n]: "
#define PROMPT_DBA_Q3  "DBA - Taint tag [1~127]: "
#define PROMPT_DBA_Q4  "DBA - Enable syscall tracer? [y/n]: "
#define PROMPT_DBA_Q5  "DBA - Enable Tracer? [y/n]: "
#define PROMPT_DBA_Q6  "DBA - The path to the strucure needed being loaded: "
#define PROMPT_DBA_Q7  "DBA - The path to the global variable needed being loaded: "
#define PROMPT_DBA_Q8  "DBA - Enable instruction tracer? [y/n]: "
#define PROMPT_DBA_Q9  "DBA - In kernel address? [y/n]: "
#define PROMPT_DBA_Q10 "DBA - Enable instruction block tracer? [y/n]: "
#define PROMPT_DBA_Q11 "DBA - In kernel address? [y/n]: "
#define PROMPT_DBA_Q12 "DBA - Start analysis? [y/n]: "

#define CONFIG_TAG_ENABLE          "ENABLE"
#define CONFIG_TAG_YES             "yes"
#define CONFIG_TAG_Y               "y"
#define CONFIG_TAG_NO              "no"
#define CONFIG_TAG_MODULE          "MODULE"
#define CONFIG_TAG_AGENT           "AGENT"
#define CONFIG_TAG_DIFT            "DIFT"
#define CONFIG_TAG_OBHOOK          "OBHOOK"
#define CONFIG_TAG_NETTRAMON       "NETTRAMON"
#define CONFIG_TAG_NTM_ALL         "ALL"
#define CONFIG_TAG_NTM_TCP         "TCP"
#define CONFIG_TAG_NTM_UDP         "UDP"
#define CONFIG_TAG_NTM_ICMP        "ICMP"
#define CONFIG_TAG_TAINT           "TAINT"
#define CONFIG_TAG_TAINT_PACKET    "PACKET"
#define CONFIG_TAG_TAINT_TAG       "TAG"
#define CONFIG_TAG_SYSCALL_TRACER  "SYSCALL_TRACER"
#define CONFIG_TAG_INSTR_TRACER    "INSTR_TRACER"
#define CONFIG_TAG_STRUC           "STRUCTURE"
#define CONFIG_TAG_GLOBV           "GLOBALVAR"
#define CONFIG_TAG_INSTR           "INSTR"
#define CONFIG_TAG_BLOCK           "BLOCK"
#define CONFIG_TAG_IS_KERNEL       "IS_KERNEL"

static void cb_dba_confirm( void* mon, const char* yn, void* opaque );
static void cb_dba_set_syscall( void* mon, const char* yn, void* opaque );
static void cb_dba_set_taint_tag( void* mon, const char* yn, void* opaque );
static void cb_dba_set_taint( void* mon, const char* yn, void* opaque );

static void show_dba_context_info( Monitor* mon, FILE* fp, const dba_context* ctx ) {

    if( (fp == NULL && mon == NULL) || ctx == NULL )
        return;

    if ( fp != NULL ) {
        fprintf( fp, "Task ID:                          %d\n", ctx->task_id );
        fprintf( fp, "Host sample:                      %s\n", ctx->sample_hpath );
        fprintf( fp, "Guest sample:                     %s\n", ctx->sample_gpath );
        fprintf( fp, "Taint analysis:                   %s\n", (ctx->taint.is_enabled)? "Enabled" : "Disabled" );
        if( ctx->taint.is_enabled )
            fprintf( fp, "Taint tag:                        %d\n", ctx->taint.tag );
        fprintf( fp, "Syscall tracer:                   %s\n", (ctx->syscall.is_enabled)? "Enabled" : "Disabled" );
        fprintf( fp, "Instruction tracer:               %s\n", (ctx->instr_tracer.instr_enabled)? "Enabled" : "Disabled" );
        if( ctx->instr_tracer.instr_enabled )
            fprintf( fp, "Instr tracer in kernel:           %s\n", ctx->instr_tracer.instr_is_kernel? "Enabled" : "Disabled" );
        fprintf( fp, "Block tracer:                     %s\n", (ctx->instr_tracer.block_enabled)? "Enabled" : "Disabled" );
        if( ctx->instr_tracer.block_enabled )
            fprintf( fp, "Block tracer in kernel:           %s\n", ctx->instr_tracer.block_is_kernel? "Enabled" : "Disabled" );
    }
    else {
        monitor_printf( mon, "Task ID:                          %d\n", ctx->task_id );
        monitor_printf( mon, "Host sample:                      %s\n", ctx->sample_hpath );
        monitor_printf( mon, "Guest sample:                     %s\n", ctx->sample_gpath );
        monitor_printf( mon, "Taint analysis:                   %s\n", (ctx->taint.is_enabled)? "Enabled" : "Disabled" );
        if( ctx->taint.is_enabled )
            monitor_printf( mon, "Taint tag:                        %d\n", ctx->taint.tag );
        monitor_printf( mon, "Syscall tracer:                   %s\n", (ctx->syscall.is_enabled)? "Enabled" : "Disabled" );
        monitor_printf( mon, "Instruction tracer:               %s\n", (ctx->instr_tracer.instr_enabled)? "Enabled" : "Disabled" );
        if( ctx->instr_tracer.instr_enabled )
            monitor_printf( mon, "Instr tracer in kernel:           %s\n", ctx->instr_tracer.instr_is_kernel? "Enabled" : "Disabled" );
        monitor_printf( mon, "Block tracer:                     %s\n", (ctx->instr_tracer.block_enabled)? "Enabled" : "Disabled" );
        if( ctx->instr_tracer.block_enabled )
            monitor_printf( mon, "Block tracer in kernel:           %s\n", ctx->instr_tracer.block_is_kernel? "Enabled" : "Disabled" );
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
    mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q9, 0, cb_dba_confirm, opaque );
    mba_readline_show_prompt( mon );
}

// DBA - instruction tracer ?
static void cb_dba_set_block_is_kernel( void* mon, const char* yn, void* opaque ) {

    // goto confirm
    if( strcasecmp( "y", yn ) == 0 || strcasecmp( "yes", yn ) == 0 ) {
        dba_enable_block_tracer_is_kernel( (DBA_TID)opaque );
        mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q12, 0, cb_dba_confirm, opaque );
        mba_readline_show_prompt( mon );
        return;
    }

    // goto confirm
    if( strcasecmp( "n", yn ) == 0 || strcasecmp( "no", yn ) == 0 ) {
        dba_disable_block_tracer_is_kernel( (DBA_TID)opaque );
        mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q12, 0, cb_dba_confirm, opaque );
        mba_readline_show_prompt( mon );
        return;
    }

    // stay set in kernel
    mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q11, 0, cb_dba_set_block_is_kernel, opaque );
    mba_readline_show_prompt( mon );
}

// DBA - block tracer ?
static void cb_dba_set_block( void* mon, const char* yn, void* opaque ) {

    // goto set in kernel 
    if( strcasecmp( "y", yn ) == 0 || strcasecmp( "yes", yn ) == 0 ) {
        dba_enable_block_tracer( (DBA_TID)opaque );
        mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q11, 0, cb_dba_set_block_is_kernel, opaque );
        mba_readline_show_prompt( mon );
        return;
    }

    // goto confirm
    if( strcasecmp( "n", yn ) == 0 || strcasecmp( "no", yn ) == 0 ) {
        dba_disable_block_tracer( (DBA_TID)opaque );
        dba_disable_block_tracer_is_kernel( (DBA_TID)opaque );
        mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q12, 0, cb_dba_confirm, opaque );
        mba_readline_show_prompt( mon );
        return;
    }

    // stay set block
    mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q10, 0, cb_dba_set_block, opaque );
    mba_readline_show_prompt( mon );
}

// DBA - instruction tracer ?
static void cb_dba_set_instr_is_kernel( void* mon, const char* yn, void* opaque ) {

    // goto set block
    if( strcasecmp( "y", yn ) == 0 || strcasecmp( "yes", yn ) == 0 ) {
        dba_enable_instr_tracer_is_kernel( (DBA_TID)opaque );
        mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q10, 0, cb_dba_set_block, opaque );
        mba_readline_show_prompt( mon );
        return;
    }

    // goto set block
    if( strcasecmp( "n", yn ) == 0 || strcasecmp( "no", yn ) == 0 ) {
        dba_disable_instr_tracer_is_kernel( (DBA_TID)opaque );
        mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q10, 0, cb_dba_set_block, opaque );
        mba_readline_show_prompt( mon );
        return;
    }

    // stay set in kernel 
    mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q9, 0, cb_dba_set_instr_is_kernel, opaque );
    mba_readline_show_prompt( mon );
}

// DBA - instruction tracer ?
static void cb_dba_set_instr( void* mon, const char* yn, void* opaque ) {

    // goto set in kernel
    if( strcasecmp( "y", yn ) == 0 || strcasecmp( "yes", yn ) == 0 ) {
        dba_enable_instr_tracer( (DBA_TID)opaque );
        mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q9, 0, cb_dba_set_instr_is_kernel, opaque );
        mba_readline_show_prompt( mon );
        return;
    }

    // goto set block
    if( strcasecmp( "n", yn ) == 0 || strcasecmp( "no", yn ) == 0 ) {
        dba_disable_instr_tracer( (DBA_TID)opaque );
        dba_disable_instr_tracer_is_kernel( (DBA_TID)opaque );
        mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q10, 0, cb_dba_set_block, opaque );
        mba_readline_show_prompt( mon );
        return;
    }

    // stay instruction tracer
    mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q8, 0, cb_dba_set_instr, opaque );
    mba_readline_show_prompt( mon );
}

// DBA -load global variable 
static void cb_dba_set_global( void* mon, const char* yn, void* opaque ) {

    // goto instruction tracer
    if( strlen(yn) != 0 ) {
        dba_set_global( (DBA_TID)opaque, yn );
        mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q8, 0, cb_dba_set_instr, opaque );
        mba_readline_show_prompt( mon );
        return;
    }

    // stay set global 
    mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q7, 0, cb_dba_set_global, opaque );
    mba_readline_show_prompt( mon );
}

// DBA - set structure ?
static void cb_dba_set_structure( void* mon, const char* yn, void* opaque ) {

    // goto set global
    if( strlen( yn ) != 0 ) {
        dba_set_structure( (DBA_TID)opaque, yn );
        mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q7, 0, cb_dba_set_global, opaque );
        mba_readline_show_prompt( mon );
        return;
    }

    // stay set structure
    mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q6, 0, cb_dba_set_structure, opaque );
    mba_readline_show_prompt( mon );
}

// DBA - tracer ?
static void cb_dba_set_tracer( void* mon, const char* yn, void* opaque ) {

    // goto set structure 
    if( strcasecmp( "y", yn ) == 0 || strcasecmp( "yes", yn ) == 0 ) {
        mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q6, 0, cb_dba_set_structure, opaque );
        mba_readline_show_prompt( mon );
        return;
    }

    // goto confirm
    if( strcasecmp( "n", yn ) == 0 || strcasecmp( "no", yn ) == 0 ) {
        mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q12, 0, cb_dba_confirm, opaque );
        dba_disable_instr_tracer( (DBA_TID)opaque );
        dba_disable_instr_tracer_is_kernel( (DBA_TID)opaque );
        dba_disable_block_tracer( (DBA_TID)opaque );
        dba_disable_block_tracer_is_kernel( (DBA_TID)opaque );
        mba_readline_show_prompt( mon );
        return;
    }

    // stay tracer
    mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q5, 0, cb_dba_set_tracer, opaque );
    mba_readline_show_prompt( mon );
}

// DBA - syscall ?
static void cb_dba_set_syscall( void* mon, const char* yn, void* opaque ) {

    // goto instruction tracer
    if( strcasecmp( "y", yn ) == 0 || strcasecmp( "yes", yn ) == 0 ) {
        dba_enable_syscall_trace( (DBA_TID)opaque );
        mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q5, 0, cb_dba_set_tracer, opaque );
        mba_readline_show_prompt( mon );
        return;
    }

    // goto next instruction tracer
    if( strcasecmp( "n", yn ) == 0 || strcasecmp( "no", yn ) == 0 ) {
        dba_disable_syscall_trace( (DBA_TID)opaque );
        mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q5, 0, cb_dba_set_tracer, opaque );
        mba_readline_show_prompt( mon );
        return;
    }

    // stay syscall
    mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q4, 0, cb_dba_set_syscall, opaque );
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
        mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q3, 0, cb_dba_set_taint_tag, opaque );
        mba_readline_show_prompt( mon );
        return;
    }

    tag = (CONTAMINATION_RECORD)(tmp & 0x00000000000000ff);

    // goto syscall
    dba_enable_taint_analysis( (DBA_TID)opaque, tag );
    mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q4, 0, cb_dba_set_syscall, opaque );
    mba_readline_show_prompt( mon );
    return;
}

// DBA - taint packet ?
static void cb_dba_set_taint_packet( void* mon, const char* yn, void* opaque ) {

    // goto taint tag
    if( strcasecmp( "y", yn ) == 0 || strcasecmp( "yes", yn ) == 0 ) {
        dba_enable_taint_packet( (DBA_TID)opaque );
        mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q3, 0, cb_dba_set_taint_tag, opaque );
        mba_readline_show_prompt( mon );
        return;
    }

    // goto taint tag
    if( strcasecmp( "n", yn ) == 0 || strcasecmp( "no", yn ) == 0 ) {
        dba_disable_taint_packet( (DBA_TID)opaque );
        mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q3, 0, cb_dba_set_taint_tag, opaque );
        mba_readline_show_prompt( mon );
        return;
    }
    
    // stay taint
    mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q2, 0, cb_dba_set_taint_packet, opaque );
    mba_readline_show_prompt( mon );
}

// DBA - taint ?
static void cb_dba_set_taint( void* mon, const char* yn, void* opaque ) {

    // goto taint packet
    if( strcasecmp( "y", yn ) == 0 || strcasecmp( "yes", yn ) == 0 ) {
        mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q2, 0, cb_dba_set_taint_packet, opaque );
        mba_readline_show_prompt( mon );
        return;
    }

    // goto syscall
    if( strcasecmp( "n", yn ) == 0 || strcasecmp( "no", yn ) == 0 ) {
        dba_disable_taint_analysis( (DBA_TID)opaque );
        mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q4, 0, cb_dba_set_syscall, opaque );
        mba_readline_show_prompt( mon );
        return;
    }
    
    // stay taint
    mba_readline_start( (Monitor*)mon, PROMPT_DBA_Q1, 0, cb_dba_set_taint, opaque );
    mba_readline_show_prompt( mon );
}

// Set dba enviroment via config file
// Return 0 for success, 1 for fail
static int dba_set_by_config ( Monitor* mon, const char* config_file_path, DBA_TID tid ) {
   
    json_object*    jo_config;
    json_object*    jo_object;
    json_object*    jo_content;

    if ( ( jo_config = json_object_from_file( config_file_path ) ) == NULL ) {
        monitor_printf( mon, "Read config file fail\n");
        return 1;
    }

    monitor_printf( mon, "=====================================\n" );
    monitor_printf( mon, "====== Start to Enable Modules ======\n" );
    monitor_printf( mon, "=====================================\n" );

    // Enable modules
    if ( json_object_object_get_ex( (json_object*)jo_config, CONFIG_TAG_MODULE, &jo_object ) ) {

        // Enable Agent
        if ( json_object_object_get_ex( jo_object, CONFIG_TAG_AGENT, &jo_content ) ) {
            if ( strcmp( json_object_get_string( jo_content ), CONFIG_TAG_YES ) == 0 || strcmp( json_object_get_string( jo_content ), CONFIG_TAG_Y ) == 0 ) {
                do_win_init( mon, NULL );
                monitor_printf( mon, "Enable Agent Successfully\n");
            }
            else {
                monitor_printf( mon, "Do nothing to Agent\n");
            }
        }
        // Enable DIFT
        if ( json_object_object_get_ex( jo_object, CONFIG_TAG_DIFT, &jo_content ) ) {
            if ( strcmp( json_object_get_string( jo_content ), CONFIG_TAG_YES ) == 0 || strcmp( json_object_get_string( jo_content ), CONFIG_TAG_Y ) == 0 ) {
                //dift_enable();
            }
            else {
                monitor_printf( mon, "Do nothing to Dift\n");
            }
        }
        // Enable Nettramon
        if ( json_object_object_get_ex( jo_object, CONFIG_TAG_NETTRAMON, &jo_content ) ) {

            json_object* jo_ntm;
            json_object* jo_all;
            json_object* jo_tcp;
            json_object* jo_udp;
            json_object* jo_icmp;
            char all_file[256];
            char tcp_file[256];
            char udp_file[256];
            char icmp_file[256];

            if ( json_object_object_get_ex( jo_content, CONFIG_TAG_ENABLE, &jo_ntm ) ) { 
                if ( strcmp( json_object_get_string( jo_ntm ), CONFIG_TAG_YES ) == 0 || strcmp( json_object_get_string( jo_ntm ), CONFIG_TAG_Y ) == 0 ) {
                    if ( json_object_object_get_ex( jo_content, CONFIG_TAG_NTM_ALL, &jo_all ) && json_object_object_get_ex( jo_content, CONFIG_TAG_NTM_TCP, &jo_tcp ) &&
                         json_object_object_get_ex( jo_content, CONFIG_TAG_NTM_UDP, &jo_udp ) && json_object_object_get_ex( jo_content, CONFIG_TAG_NTM_ICMP, &jo_icmp ) ) {
                        
                        if ( json_object_get_string_len( jo_all ) != 0 ) {
                            strcpy( all_file, json_object_get_string( jo_all ) );
                        }
                        else {
                            strcpy( all_file, "N" ); 
                        }
                        if ( json_object_get_string_len( jo_tcp ) != 0  ) {
                            strcpy( tcp_file, json_object_get_string( jo_tcp ) );
                        }
                        else {
                            strcpy( tcp_file, "N" ); 
                        }
                        if ( json_object_get_string_len( jo_udp ) != 0  ) {
                            strcpy( udp_file, json_object_get_string( jo_udp ) );
                        }
                        else {
                            strcpy( udp_file, "N" ); 
                        }
                        if ( json_object_get_string_len( jo_icmp ) != 0  ) {
                            strcpy( icmp_file, json_object_get_string( jo_icmp ) );
                        }
                        else {
                            strcpy( icmp_file, "N" ); 
                        }
                        if ( nettramon_set_file_path( all_file, tcp_file, udp_file, icmp_file ) != 0 ) {
                            monitor_printf( mon, "Set NTM files failed\n");
                        }
                        else {
                            monitor_printf( mon, "Set files of NTM successfully\n");
                        }
                    }
                    else {
                        monitor_printf( mon, "Cannot get all the NTM files' tags, set with printing on the monitor\n");
                    }
                    nettramon_start( NULL );
                    monitor_printf( mon, "Enable Nettramon Successfully\n");
                }
                else {
                    monitor_printf( mon, "Do nothing to NTM\n");
                }
            }
            else {
                monitor_printf( mon, "Cannot get enable tag of NTM\n");
                return 1;
            }
        }

    }
    monitor_printf( mon, "=====================================\n" );
    monitor_printf( mon, "====== Finish Enabling Modules ======\n" );
    monitor_printf( mon, "=====================================\n\n" );
    // Modules

    // DBA Abilities
    monitor_printf( mon, "=====================================\n" );
    monitor_printf( mon, "====== Start to Set Up DBA ==========\n" );
    monitor_printf( mon, "=====================================\n" );

    // Set Taint
    if ( json_object_object_get_ex( (json_object*)jo_config, CONFIG_TAG_TAINT, &jo_object ) ) {
        
        if ( json_object_object_get_ex( jo_object, CONFIG_TAG_ENABLE, &jo_content ) ) {
            if ( ( strcmp( json_object_get_string( jo_content ), CONFIG_TAG_YES ) == 0 || strcmp( json_object_get_string( jo_content ), CONFIG_TAG_Y ) == 0 ) ) {
                // Get the taint tag
                if ( json_object_object_get_ex( jo_object, CONFIG_TAG_TAINT_TAG, &jo_content ) ) {

                    CONTAMINATION_RECORD tag;
                    long int tmp;
                    char* end;
                    char tag_str[10];

                    strcpy( tag_str, json_object_get_string( jo_content ) );

                    // invalidate taint tag
                    tmp = strtol( tag_str, &end, 10 );
                    if( *end != '\0' || tmp < 0x1 || tmp > 0x7f ) {
                        monitor_printf( mon, "Invalid taint tag\n" );
                        return 1;
                    }

                    tag = (CONTAMINATION_RECORD)(tmp & 0x00000000000000ff);

                    if ( dba_enable_taint_analysis( tid, tag )!= 0 ) {
                        monitor_printf( mon, "Set taint tag failed\n" );   
                    }
                    monitor_printf( mon, "Set taint tag %s successfully\n", json_object_get_string( jo_content ) );
                }
                else {
                    monitor_printf( mon, "Cannot get tag of taint tag\n");
                    return 1;
                }
                // Get the taint packet tag
                if ( json_object_object_get_ex( jo_object, CONFIG_TAG_TAINT_PACKET, &jo_content ) ) {
                     if ( ( strcmp( json_object_get_string( jo_content ), CONFIG_TAG_YES ) == 0 || strcmp( json_object_get_string( jo_content ), CONFIG_TAG_Y ) == 0 ) ) {
                        if ( dba_enable_taint_packet( tid ) != 0 ) {
                            monitor_printf( mon, "Enable taint packet failed\n" );
                            return 1;
                        }
                        monitor_printf( mon, "Set taint packet successfully\n" );
                     }
                     else {
                        if ( dba_disable_taint_packet( tid ) != 0 ) {
                            monitor_printf( mon, "Disable taint packet failed\n" );
                            return 1;
                        }
                        monitor_printf( mon, "No taint packet\n" );
                     }
                }
                else {
                    monitor_printf( mon, "Cannot get taint packet tag\n");
                    return 1;
                }
            }
            else {
                if ( dba_disable_taint_analysis( tid ) != 0 ) {
                    monitor_printf( mon, "Disable Taint ablility failed\n" );
                    return 1;
                }
                monitor_printf( mon, "No Taint ablility\n" );
            }
        }
        else {
            monitor_printf( mon, "Cannot get taint enable tag\n");
            return 1;
        }
    }
    else {
        monitor_printf( mon, "Cannot get taint json tag\n");
        return 1;
    }
    // Taint

    // Set Syscall tracer
    if ( json_object_object_get_ex( (json_object*)jo_config, CONFIG_TAG_SYSCALL_TRACER, &jo_object ) ) {
        if ( json_object_object_get_ex( jo_object, CONFIG_TAG_ENABLE, &jo_content ) ) {
            if ( ( strcmp( json_object_get_string( jo_content ), CONFIG_TAG_YES ) == 0 || strcmp( json_object_get_string( jo_content ), CONFIG_TAG_Y ) == 0 ) ) {
                if ( dba_enable_syscall_trace( tid ) != 0 ) {
                    monitor_printf( mon, "Enable Syscall ability Failed\n");
                    return 1;
                }
                monitor_printf( mon, "Set Syscall ability successfully\n");
            }
            else {
                if ( dba_disable_syscall_trace( tid ) != 0 ) {
                    monitor_printf( mon, "Disable Syscall ability Failed\n");
                    return 1;
                }
                monitor_printf( mon, "No Syscall ability\n");
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
    // Syscall tracer

    // Set Intruction tracer
    if ( json_object_object_get_ex( (json_object*)jo_config, CONFIG_TAG_INSTR_TRACER, &jo_object ) ) {

        if ( json_object_object_get_ex( jo_object, CONFIG_TAG_ENABLE, &jo_content ) ) { 
            if ( ( strcmp( json_object_get_string( jo_content ), CONFIG_TAG_YES ) == 0 || strcmp( json_object_get_string( jo_content ), CONFIG_TAG_Y ) == 0 ) ) {
                
                // Load structure
                if ( json_object_object_get_ex( jo_object, CONFIG_TAG_STRUC, &jo_content ) ) {
                    if ( dba_set_structure( tid, json_object_get_string( jo_content ) ) != 0 ) {
                        monitor_printf( mon, "Set with given structure json file failed\n");
                        return 1;
                    }
                    monitor_printf( mon, "Set tracer with given structure json file successfully\n");
                }
                else {
                    monitor_printf( mon, "Set tracer without giving structure json file\n");
                    return 1;
                }

                // Load global variable
                if ( json_object_object_get_ex( jo_object, CONFIG_TAG_GLOBV, &jo_content ) ) {
                    if ( dba_set_global( tid, json_object_get_string( jo_content ) ) != 0 ) {
                        monitor_printf( mon, "Set with given global variable json file failed\n");
                        return 1;
                    }
                    monitor_printf( mon, "Set tracer with given global variable json file successfully\n");
                }
                else {
                    monitor_printf( mon, "set tracer without giving global variable json file\n");
                    return 1;
                }
                
                // instruction
                if ( json_object_object_get_ex( jo_object, CONFIG_TAG_INSTR, &jo_content ) ) {
                    
                    json_object* jo_enable;
                    json_object* jo_is_kernel;

                    // Make sure the "enable" and "is_kernel" tag in the json object
                    if ( json_object_object_get_ex( jo_content, CONFIG_TAG_ENABLE, &jo_enable ) && json_object_object_get_ex( jo_content, CONFIG_TAG_IS_KERNEL, &jo_is_kernel ) ) {
                        // Deal with the enable tag
                        if ( ( strcmp( json_object_get_string( jo_enable ), CONFIG_TAG_YES ) == 0 || strcmp( json_object_get_string( jo_enable ), CONFIG_TAG_Y ) == 0 ) ) {
                            if ( dba_enable_instr_tracer( tid ) != 0 ) {
                                monitor_printf( mon, "set instruction tracer failed\n");
                                return 1;
                            }
                            monitor_printf( mon, "set instruction tracer successfully\n");
                            // Deal with the is_kernel tag
                            if ( ( strcmp( json_object_get_string( jo_is_kernel ), CONFIG_TAG_YES ) == 0 || strcmp( json_object_get_string( jo_is_kernel ), CONFIG_TAG_Y ) == 0 ) ) {
                                if ( dba_enable_instr_tracer_is_kernel( tid ) != 0 ) {
                                    monitor_printf( mon, "set instruction tracer in kernel level failed\n");
                                    return 1;
                                }
                                monitor_printf( mon, "set instruction tracer in kernel level successfully\n");
                            }
                            else {
                                if ( dba_disable_instr_tracer_is_kernel( tid ) != 0 ) {
                                    monitor_printf( mon, "set instruction tracer in user level failed\n");
                                    return 1;
                                }
                                monitor_printf( mon, "set instruction tracer in user level successfully\n");
                            }
                        }
                        else {
                            if ( dba_disable_instr_tracer( tid ) != 0 ) {
                                monitor_printf( mon, "Disable instruction tracer ability failed\n");
                                return 1;
                            }
                            monitor_printf( mon, "No instruction tracer ability\n");
                        }
                    }
                    else {
                        monitor_printf( mon, "Instruction Tracer cannot get \"enable\" or \"is_kernel\" tag\n");
                        return 1;
                    }
                }
                else {
                    monitor_printf( mon, "Cannot get instruction tracer tag\n");
                    return 1;
                }
                // instruction
                // block
                if ( json_object_object_get_ex( jo_object, CONFIG_TAG_BLOCK, &jo_content ) ) {
                    
                    json_object* jo_enable;
                    json_object* jo_is_kernel;
                    
                    // Make sure the "enable" and "is_kernel" tag in the json object
                    if ( json_object_object_get_ex( jo_content, CONFIG_TAG_ENABLE, &jo_enable ) && json_object_object_get_ex( jo_content, CONFIG_TAG_IS_KERNEL, &jo_is_kernel ) ) {
                        // Deal with the enable tag
                        if ( ( strcmp( json_object_get_string( jo_enable ), CONFIG_TAG_YES ) == 0 || strcmp( json_object_get_string( jo_enable ), CONFIG_TAG_Y ) == 0 ) ) {
                            if ( dba_enable_block_tracer( tid ) != 0 ) {
                                monitor_printf( mon, "Enable block tracer ablility failed\n");
                                return 1;
                            }
                            monitor_printf( mon, "Enable block tracer successfully\n");
                            // Deal with the is_kernel tag
                            if ( ( strcmp( json_object_get_string( jo_is_kernel ), CONFIG_TAG_YES ) == 0 || strcmp( json_object_get_string( jo_is_kernel ), CONFIG_TAG_Y ) == 0 ) ) {
                                if ( dba_enable_block_tracer_is_kernel( tid ) != 0 ) {
                                    monitor_printf( mon, "Set block tracer in kernel level failed\n");
                                    return 1;
                                }
                                monitor_printf( mon, "Set block tracer in kernel level successfully\n\n");
                            }
                            else {
                                if ( dba_disable_block_tracer_is_kernel( tid ) != 0 ) {
                                    monitor_printf( mon, "Set block tracer in user level failed\n");
                                    return 1;
                                }
                                monitor_printf( mon, "Set block tracer in user level successfully\n");
                            }
                        }
                        else {
                            if ( dba_disable_block_tracer( tid ) != 0 ) {
                                monitor_printf( mon, "Disable block tracer ability failed\n");
                                return 1;
                            }
                            monitor_printf( mon, "No block tracer ability\n");
                        }
                    }
                    else {
                        monitor_printf( mon, "Block Tracer cannot get \"enable\" or \"is_kernel\" tag\n");
                        return 1;
                    }
                }
                else {
                    monitor_printf( mon, "Cannot get block tracer tag\n");
                    return 1;
                }
                // block
            }
            else {
                // Disable all the Tracer ability
                if ( dba_disable_instr_tracer( tid ) != 0 || dba_disable_instr_tracer_is_kernel( tid ) != 0 ||
                     dba_disable_block_tracer( tid ) != 0 || dba_disable_block_tracer_is_kernel( tid ) != 0   ){
                     monitor_printf( mon, "Set without tracer ability failed\n");
                     return 1;
                }
                monitor_printf( mon, "Set without tracer ability\n");
            }
        }
        else {
            monitor_printf( mon, "Cannot get tracer enable tag\n");
            return 1;
        }
    }
    else {
        monitor_printf( mon, "Cannot get tracer tag\n");
        return 1;
    }
    // Instrution Tracer

    monitor_printf( mon, "=====================================\n" );
    monitor_printf( mon, "====== Finishing Setting Up DBA =====\n" );
    monitor_printf( mon, "=====================================\n\n" );
    // DBA Abilities

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

    // Set by the config file
    if ( config_file_path != NULL ) {

        monitor_printf( mon, "* * * * * * * * * * * * * *\n" );
        monitor_printf( mon, "* Set up with config file *\n" );
        monitor_printf( mon, "* * * * * * * * * * * * * *\n" );

        if ( dba_set_by_config( mon, config_file_path, tid ) != 0 ) {
            monitor_printf( mon, "Set task by config file fail\n");
        }
        else {
            cb_dba_confirm( mon, "yes", (void*)tid );
        }
    }
    else {

        monitor_printf( mon, "* * * * * * * * * * * * *\n" );
        monitor_printf( mon, "* Set up with interface *\n" );
        monitor_printf( mon, "* * * * * * * * * * * * *\n" );

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
    json_object* tracer_report;

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

        // show tracer result if enabled
        if( ctx->instr_tracer.instr_enabled ) {
            json_object_object_get_ex( ctx->result, DBA_JSON_KEY_TRACER, &tracer_report );
            show_dba_report_title( mon, NULL, DBA_JSON_KEY_TRACER );
            json_object_object_foreach( tracer_report, tracer_field, tracer_records  ) {
                monitor_printf( mon, "%s:\n", tracer_field );

                for( i = 0; i < json_object_array_length(tracer_records); ++i ) {
                    monitor_printf( mon, "    %s\n",
                            json_object_get_string(json_object_array_get_idx(tracer_records, i)) );
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

        // show tracer result if enabled
        if( ctx->instr_tracer.instr_enabled ) {
            json_object_object_get_ex( ctx->result, DBA_JSON_KEY_TRACER, &tracer_report );
            show_dba_report_title( mon, fp, DBA_JSON_KEY_TRACER );
            json_object_object_foreach( tracer_report, tracer_field, tracer_records  ) {
                fprintf( fp, "%s:\n", tracer_field );

                for( i = 0; i < json_object_array_length(tracer_records); ++i ) {
                    fprintf( fp, "    %s\n",
                            json_object_get_string(json_object_array_get_idx(tracer_records, i)) );
                }
            }
        }

        fclose( fp );
        monitor_printf( mon, "Finished writing result to targeted file\n" );
    }
}
