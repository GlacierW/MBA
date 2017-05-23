#ifndef __SYSTRACE_PRIV_H__
#define __SYSTRACE_PRIV_H__

#include "cpu.h"

extern void systrace_on_syscall(CPUX86State *env);
extern void systrace_on_sysret(CPUX86State *env);
extern void cb_log_syscall_info( X86CPU *x86cpu, bool is_invoke, void *args );

#endif
