#ifndef __AGENT_COMMANDS_H__
#define __AGENT_COMMANDS_H__

struct Monitor;
struct QDict;
void do_win_impo(struct Monitor *mon, const struct QDict *qdict);
void do_win_expo(struct Monitor *mon, const struct QDict *qdict);
void do_win_exec(struct Monitor *mon, const struct QDict *qdict);
void do_win_invo(struct Monitor *mon, const struct QDict *qdict);
void do_win_init(struct Monitor *mon, const struct QDict *qdict);
void do_win_status(struct Monitor *mon, const struct QDict *qdict);
void do_win_logf(struct Monitor *mon, const struct QDict *qdict);
#endif
