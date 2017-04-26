#ifndef __SYSTRACE_COMMANDS_H__
#define __SYSTRACE_COMMANDS_H__

struct Monitor;
struct QDict;

void do_add_systrace(Monitor *mon, const QDict *qdict);
void do_delete_systrace(Monitor *mon, const QDict *qdict);

#endif
