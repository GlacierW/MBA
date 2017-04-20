/*
 *
 *  Taint ability of DBA
 *
 *  Copyright (c)    2017 JuiChien Jao
 *                   2016 ChiaWei Wang
 *
 * This library is free software you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */
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
