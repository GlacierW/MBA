#ifndef __DBA_TAINT_H__
#define __DBA_TAINT_H__

#include "dba.h"

extern int set_sample_tainted( dba_context* ctx );
extern int check_mbr_tainted( dba_context* ctx );
extern int enum_tainted_file( dba_context* ctx );
extern int enum_tainted_registry( dba_context* ctx );

#endif
