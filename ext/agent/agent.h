#ifndef __AGENT_H__
#define __AGENT_H__

#define AGENT_GUEST_PORT 8888

#define SZ_MAX_COMMAND   256
#define SZ_MAX_FILEPATH  256
#define SZ_MAX_FILECHUNK 8192 // 8KB per I/O chunk

#define MSG_EXEC_READY "EXEC_READY"
#define MSG_ACK_PREFIX "System Receive : "

// performing agent action
enum MBA_AGENT_ACTION {
    AGENT_ACT_IDLE,
    AGENT_ACT_INIT,
    AGENT_ACT_IMPO,
    AGENT_ACT_EXPO,
    AGENT_ACT_EXEC,
    AGENT_ACT_INVO,
    AGENT_ACT_LOGF,
};

// common return value
enum MBA_AGENT_RETURN {
    AGENT_RET_SUCCESS,
    AGENT_RET_EFAIL,
    AGENT_RET_EBUSY,
    AGENT_RET_EINIT
};

typedef enum MBA_AGENT_ACTION MBA_AGENT_ACTION;
typedef enum MBA_AGENT_RETURN MBA_AGENT_RETURN;

/*
void create_agent_thread(Monitor *mon);
char* getRandomRedirPort(void);
void initial_string(char* word, int len);
void init_socket(void);
bool executeIsOver(char* readMessageSize);
void print_system_receive(Monitor* mon, int receiveLength);
void import_cmd(Monitor* mon);
void export_cmd(Monitor* mon);
void execute_cmd(Monitor* mon);
void invoke_cmd(Monitor* mon);
void status_cmd(Monitor* mon);
void logf_cmd(Monitor* mon);
void* agent_thread_main(void *monitor);
extern unsigned int agent_action;
extern unsigned int agent_thread;
extern char* agent_srcpath;
extern char* agent_despath;
extern char* redir_Port;
*/

/// The following functions are decalred as public API of the agent extension
/// They are exported with the "agent_xxx" naming convention

/// Check if the agent extension is ready to serve
///
///     no param
///
/// Return true if ready, false otherwise
extern bool agent_is_ready( void );

/// Print out the agent extension message
/// If a monitor of QEMU is assigned at the initialization
/// the output will be delivered to the monitor. Otherwise,
/// all of the output will be omitted
/// 
///
///     \param fmt  format string
///     \param ...  
///
/// Return none
extern void agent_printf( const char* fmt, ... );

/// Command the agent thread to perform 'import' action
/// 
///     \param dst_path     the fullpath of the source file of host
///     \param src_path     the fullpath of the destination file of guest
///
/// Return AGENT_RET_SUCCESS, no error occured
///        AGENT_RET_EBUSY, the agent thread is still busy dealing the previous command
///        AGENT_RET_EINIT, the agent thread is not initialized yet
///        AGENT_RET_EFAIL, general failure
extern MBA_AGENT_RETURN agent_import( const char* dst_path, const char* src_path );

/// Command the agent thread to perform 'export' action
/// 
///     \param dst_path     the fullpath of the destination file of host
///     \param src_path     the fullpath of the source file of guest
///
/// Return AGENT_RET_SUCCESS, no error occured
///        AGENT_RET_EBUSY, the agent thread is still busy dealing the previous command
///        AGENT_RET_EINIT, the agent thread is not initialized yet
///        AGENT_RET_EFAIL, general failure
extern MBA_AGENT_RETURN agent_export( const char* dst_path, const char* src_path );

/// Command the agent thread to perform 'logfile' action
/// 
///     \param dst_path     the fullpath of the destination of host to store the agent log
///
/// Return AGENT_RET_SUCCESS, no error occured
///        AGENT_RET_EBUSY, the agent thread is still busy dealing the previous command
///        AGENT_RET_EINIT, the agent thread is not initialized yet
///        AGENT_RET_EFAIL, general failure
extern MBA_AGENT_RETURN agent_logfile( const char* dst_path );

/// Initialize agent context & spawn the agent thread
/// This API should be called before any other 'agent_xxx' can be used
/// 
///     \param mon              monitor of QEMU, allowing agent extension to show message
///     \param server_fwd_port  the local port number to forward network connections to the in-VM agent
///
/// Return AGENT_RET_SUCCESS, no error occured
///        AGENT_RET_EBUSY, the agent thread is still busy dealing the initialization
///        AGENT_RET_EFAIL, general failure
extern MBA_AGENT_RETURN agent_init( Monitor* mon, uint16_t server_fwd_port );
#endif

