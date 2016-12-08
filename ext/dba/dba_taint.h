#ifndef __DBA_TAINT_H__
#define __DBA_TAINT_H__

#include "dba.h"


int set_sample_tainted( dba_context* ctx );
int check_mbr_tainted( dba_context* ctx );
int enum_tainted_file( dba_context* ctx );
int enum_tainted_registry( dba_context* ctx );

#endif
