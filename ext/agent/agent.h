#ifndef __AGENT_H__
#define __AGENT_H__
extern void create_agent_thread(Monitor *mon);
extern unsigned int agent_action;
extern unsigned int agent_file_size;
extern char* agent_srcpath;
extern char* agent_despath;
#endif
