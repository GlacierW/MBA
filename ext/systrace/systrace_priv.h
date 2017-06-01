#ifndef __SYSTRACE_PRIV_H__
#define __SYSTRACE_PRIV_H__

#include "cpu.h"
#include "systrace.h"

/*
syscall entry callback dispatcher
This function should only called by QEMU system call handler
*/
extern void systrace_on_syscall(CPUX86State *env);
/*
syscall exit callback dispatcher
This function should only called by QEMU sysret handler
*/
extern void systrace_on_sysret(CPUX86State *env);
/*
default system call callback
*/
extern void cb_log_syscall_info( X86CPU *x86cpu, bool is_invoke, void *args, const syscall_context* syscall_info );

#endif
