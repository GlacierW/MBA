#ifndef __DIFT_COMMANDS_H__
#define __DIFT_COMMANDS_H__

struct Monitor;
struct QDict;
void do_memory_contaminate(struct Monitor *mon, const struct QDict *qdict);
void do_show_memory_taint_map(struct Monitor *mon, const struct QDict *qdict);
void do_get_physic_address(struct Monitor *mon, const struct QDict *qdict);
void do_enable_dift(struct Monitor *mon, const struct QDict *qdict);
void do_disable_dift(struct Monitor *mon, const struct QDict *qdict);
void do_dift_status(struct Monitor *mon, const struct QDict *qdict);
#endif
