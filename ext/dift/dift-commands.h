#ifndef __MBA_COMMAND_H_
#define __MBA_COMMAND_H_

struct Monitor;
struct QDict;
void do_memory_contaminate(struct Monitor *mon, const struct QDict *qdict);
void do_show_memory_taint_map(struct Monitor *mon, const struct QDict *qdict);

#endif
