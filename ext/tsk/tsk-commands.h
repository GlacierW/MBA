#ifndef __TSK_COMMAND_H_
#define __TSK_COMMAND_H_

struct Monitor;
struct QDict;
void do_get_filename_by_haddr(Monitor *mon, const QDict *qdict);
void do_get_haddr_by_filename(Monitor *mon, const QDict *qdict);
#endif
