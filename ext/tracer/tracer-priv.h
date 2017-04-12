/*
 *  MBA Tracer header
 *
 *  Copyright (c)   2016 Chiawei Wang
 *                  2016 ChongKuan Chen
 *
 * This library is free software; you can redistribute it and/or
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

#ifndef __TRACER_PRIV_H__
#define __TRACER_PRIV_H__

/// bool tracer_is_kern_addr( uint64_t addr );
/// Check if the address is in kernel space
///
/// \param addr			the address to check        
/// 
/// return true if addr is in kernel, 0 otherwise
bool tracer_is_kern_addr( uint64_t addr );

/// void* default_callback(void* env_state, uint64_t pc_start, uint64_t pc_end );
/// the default callback as sample call back function
/// this callback will print out address, assembly and register value
///
/// \param env_state		the cpu state
/// \param pc_start		the start addess of block, or instruction
/// \param pa_end		the end address of block, or 0 for instruction tracer
void* default_callback(void* env_state, uint64_t pc_start, uint64_t pc_end );

/// void tracer_list_callback(void);
/// list all callback
void tracer_list_callback(void);


/// tracer_cb_record* get_tracer_in_list(tracer_cb_record* head, int uid);
/// Given the tracer list, check every tracer in the list and return the tracer it if id is match
///
/// \param head                 the head of tracer linked list
/// \param uid                  the target id
/// 
/// return NULL on error, tracer's pointer on success
tracer_cb_record* get_tracer_in_list(tracer_cb_record* head, int uid);

/// tracer_cb_record* get_tracer(int uid);
/// Return the pointer to tracer of specify uid
///
/// \param uid                  the target id
///
/// return NULL on error, tracer's pointer on success
tracer_cb_record* get_tracer(int uid);


int tracer_clean_up_by_list( tracer_cb_record* head );
#endif
