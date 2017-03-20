#ifndef __DBA_TAINT_H__
#define __DBA_TAINT_H__

#include "dba.h"

/// Set the sample tainted by given taint tag
/// 
///     \param  ctx             current task's dba_context pointer
///
/// Return 0 for success
extern int set_sample_tainted( dba_context* ctx );

/// TODO:
///
/// Return
extern int check_mbr_tainted( dba_context* ctx );

/// Collect tainted files by the taint tag used in the task 
/// 
///     \param  ctx             current task's dba_context pointer
///
/// Return 0 for success
extern int enum_tainted_file( dba_context* ctx );

/// TODO: 
///
/// Return
extern int enum_tainted_registry( dba_context* ctx );

/// Check whether cpatured packet is tainted
/// 
///     \param  len             length of captured packet
///     \param  packet_haddr    the physical address of captured packet
///     \param  ctx             current task's dba_context pointer
///
/// Return None
extern void* tainted_packet_cb( size_t len, haddr_t packet_haddr, void* ctx );

#endif
