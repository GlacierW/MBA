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

extern bool agent_is_ready( void );
extern void agent_printf( const char* fmt, ... );
extern MBA_AGENT_RETURN agent_init( Monitor* mon, uint16_t server_fwd_port );
#endif

