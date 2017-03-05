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

#ifndef __TRACER_H__
#define __TRACER_H__

#include <inttypes.h> 
#include "utlist.h"

#define MAX_SZ_TRACER_LABEL 16


enum TRACER_ERRNO {
    TRACER_ERR_FAIL,
    TRACER_INVALID_GRANULARITY,
    TRACER_INVALID_ID
};

enum TRACER_GRANULARITY {
    TRACER_GRANULARITY_INSTR,
    TRACER_GRANULARITY_CODEBLOCK
};


struct tracer_cb_record {

    // unique identifier for each hook
    uint16_t uid;

    int enabled;
    uint64_t cr3;
    bool is_universal;
    //TODO: Add support for kernel trace 
    bool is_kernel_trace;
    int trace_granularity;

    // user-friendly label string
    char label[MAX_SZ_TRACER_LABEL];

    void* (*cb_func) (void*,uint64_t, uint64_t);

    struct tracer_cb_record* next;
};

typedef struct tracer_cb_record tracer_cb_record;

extern tracer_cb_record *g_process_tracer_head;
extern tracer_cb_record *g_universal_kernel_tracer_head;
extern tracer_cb_record *g_universal_tracer_head;

extern tracer_cb_record *g_process_btracer_head;
extern tracer_cb_record *g_universal_kernel_btracer_head;
extern tracer_cb_record *g_universal_btracer_head;

//Public API
/// int tracer_enable_tracer(int uid)
/// Enable the tracer by id 
///
/// \param uid              	id of the target tracer
/// 
/// return -1 on error, 0 on success
extern int tracer_enable_tracer(int);

/// int tracer_disable_tracer(int);
/// Disable the tracer by id 
///
/// \param uid           	id of the target tracer
/// 
/// return -1 on error, 0 on success
extern int tracer_disable_tracer(int);

/// int tracer_add_inst_tracer( uint64_t cr3, const char* label, bool is_kernel, void*(*cb) (void*, uint64_t, uint64_t) );
/// Add a new instruction level tracer
///
/// \param cr3			the target process which want to monitor	
///				if the cr3 is 0, then the tracer monitor all the process
/// \param label		the name label for identifying the tracer, user can provide their own name
/// \param is_kernel		denote if we want to trace kernel instructions, 
///				1 for kernel, 0 for user-level 
/// \param cb			the callback function after the instrction executed.
///				cb must receive 3 argument, 1st is CPUX86State(user must convert it from void* to CPUX86State*) 
///				2nd is start address of instruction, 3rd is useless in 
///                             the default call back is used when cb == NULL
///				instruction tracer(always be 0), and the cb must return void*
/// 
/// return -1 on error, 0 on success
extern int tracer_add_inst_tracer( uint64_t cr3, const char* label, bool is_kernel, void*(*cb) (void*, uint64_t, uint64_t) );

/// int tracer_add_block_tracer( uint64_t cr3, const char* label, bool is_kernel, void*(*cb) (void*, uint64_t, uint64_t) );
/// Add a new block level tracer
///
/// \param cr3                  the target process which want to monitor        
///                             if the cr3 is 0, then the tracer monitor all the process
/// \param label                the name label for identifying the tracer, user can provide their own name
/// \param is_kernel            denote if we want to trace kernel instructions, 
///                             1 for kernel, 0 for user-level 
/// \param cb                   the callback function after the instrction executed.
///                             cb must receive 3 argument, 1st is CPUX86State(user must convert it from void* to CPUX86State*) 
///                             2nd is start address of block, 3rd is address of last ins of block 
///                              and the cb must return void*
///				the default call back is used when cb == NULL
/// 
/// return -1 on error, 0 on success
extern int tracer_add_block_tracer( uint64_t cr3, const char* label, bool is_kernel, void*(*cb) (void*, uint64_t, uint64_t) );

/// int get_error_no(void);
/// get the error number 
/// return the int of enum TRACER_ERRNO
extern int get_error_no(void);
#endif

