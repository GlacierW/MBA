#ifndef __AGENT_H__
#define __AGENT_H__
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
#endif

