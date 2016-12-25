#ifndef MONITOR_H
#define MONITOR_H

#include "qemu-common.h"
#include "qapi/qmp/qerror.h"
#include "qapi/qmp/qdict.h"
#include "block/block.h"
#include "qemu/readline.h"

extern Monitor *cur_mon;
extern Monitor *default_mon;

/* flags for monitor_init */
#define MONITOR_IS_DEFAULT    0x01
#define MONITOR_USE_READLINE  0x02
#define MONITOR_USE_CONTROL   0x04
#define MONITOR_USE_PRETTY    0x08

/* flags for monitor commands */
#define MONITOR_CMD_ASYNC       0x0001

int monitor_cur_is_qmp(void);

void monitor_init(CharDriverState *chr, int flags);

int monitor_suspend(Monitor *mon);
void monitor_resume(Monitor *mon);

int monitor_read_bdrv_key_start(Monitor *mon, BlockDriverState *bs,
                                BlockCompletionFunc *completion_cb,
                                void *opaque);
int monitor_read_block_device_key(Monitor *mon, const char *device,
                                  BlockCompletionFunc *completion_cb,
                                  void *opaque);

int monitor_get_fd(Monitor *mon, const char *fdname, Error **errp);
int monitor_fd_param(Monitor *mon, const char *fdname, Error **errp);

void monitor_vprintf(Monitor *mon, const char *fmt, va_list ap)
    GCC_FMT_ATTR(2, 0);
void monitor_printf(Monitor *mon, const char *fmt, ...) GCC_FMT_ATTR(2, 3);
void monitor_flush(Monitor *mon);
int monitor_set_cpu(int cpu_index);
int monitor_get_cpu_index(void);

/* MBA extension--- */
/// Returns CPUArchState (not determined here) pointer.
void* mba_mon_get_cpu(void);
void  mba_readline_start( Monitor* mon, const char *prompt, int read_password,
                          ReadLineFunc *readline_func, void *opaque );
void  mba_readline_show_prompt( Monitor* mon );

/* ---MBA extension */


typedef void (MonitorCompletion)(void *opaque, QObject *ret_data);

void monitor_set_error(Monitor *mon, QError *qerror);
void monitor_read_command(Monitor *mon, int show_prompt);
int monitor_read_password(Monitor *mon, ReadLineFunc *readline_func,
                          void *opaque);

int qmp_qom_set(Monitor *mon, const QDict *qdict, QObject **ret);

int qmp_qom_get(Monitor *mon, const QDict *qdict, QObject **ret);
int qmp_object_add(Monitor *mon, const QDict *qdict, QObject **ret);
void object_add(const char *type, const char *id, const QDict *qdict,
                Visitor *v, Error **errp);

AddfdInfo *monitor_fdset_add_fd(int fd, bool has_fdset_id, int64_t fdset_id,
                                bool has_opaque, const char *opaque,
                                Error **errp);
int monitor_fdset_get_fd(int64_t fdset_id, int flags);
int monitor_fdset_dup_fd_add(int64_t fdset_id, int dup_fd);
void monitor_fdset_dup_fd_remove(int dup_fd);
int monitor_fdset_dup_fd_find(int dup_fd);

#endif /* !MONITOR_H */
