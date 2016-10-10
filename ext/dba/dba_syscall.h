#ifndef __DBA_SYSCALL_H__
#define __DBA_SYSCALL_H__
#include <stddef.h>
#include "dba.h"

#define RFT_SYSCALL_TRACE       "Syscall Trace"
#define RFT_NTCREATEFILE        "NtCreateFile"
#define RFT_NTCREATEKEY         "NtCreateKey"
#define RFT_NTOPENKEY           "NtOpenKey"
#define RFT_NTOPENKEYEX         "NtOpenKeyEx"
#define RFT_ERROR               "Error"

extern void init_syscall_analysis( dba_context *ctx );
extern void clean_syscall_analysis( dba_context *ctx );

#endif
