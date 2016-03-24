/*
 *  De-coupled Information Flow Tracking (DIFT) header
 *
 *  Copyright (c)   2012 Chiwei Wang
 *                  2016 Chiawei Wang
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

#ifndef __DIFT_H__
#define __DIFT_H__

#include <stdbool.h>

// Config 
#define CONFIG_SIZE_OF_QUEUE    (16 * 1024 * 1024)
#define CONFIG_MAX_TB_ESTI      500000
#define CONFIG_IF_CODES_PER_TB  160
#define CONFIG_INDIRECT_TAINT

#if defined(CONFIG_DIFT_DEBUG)
// By default we generate color DIFT log. Comment this if unwanted
#define DIFT_DEBUG_COLOR

///
/// Herein you should define your condition to log when the debugging
/// is activated. By default the condition macro is empty. (no condition, lots of logs)
/// e.g.
///     #define DIFT_DEBUG_CONDITION if( dc->tb_rip == 0x1234567812345678 )
/// 
/// The DIFT log can be divided into three catogories and have their own logging formats: 
///     1. REG & MEM
///     2. MEM & DISK
///     3. DIRECT MEM/DISK TAINT
/// 
/// 1. REG & MEM
///     The general taint propagation between registers and memory is logged in the following context:
///     <DATATYPE> <MAPPING> <OPERAND_DST(TAINT_TAG_HEX)> <OPERATOR> <OPERAND_SRC_IF_ANY(TAINT_TAG_HEX)> <COMPLETE_OPERATOR>
///
///         DATATYPE => { B(yte), W(ord), D(ouble word), Q(uad) }
///         MAPPING  => { I(nside reg), O(ne-to-one), M(ix), C(lear) }
///         OPERAND  => { e.g. RAX(0), [0xdeadbeed](1) }
///         OPERATOR => { DIFT_DEBUG_OP_ASSIGN, DIFT_DEBUG_OP_APPEND }
///         COMPLETE_OPERATOR => DIFT_DEBUG_OP_COMPLETE
/// 
/// 2. MEM & DISK
///     The logging format is shown as follows:
///     <TYPE_DST[ADDRRESS]> <TYPE_SRC[ADDRESS]> <LENGTH> <COMPLETE_OPERATOR>
///         
///         TYPE => { H(arddisk), M(emory) }
///         COMPLETE_OPERATOR => DIFT_DEBUG_OP_COMPLETE
/// 
/// 3. DIRECT MEM/DISK TAINT/UNTAINT
///     When a user directly mark a register/memory/disk region as tainted, the operation is logged:
///     <TYPE> <DST> <TAINT_TAG_HEX> <LENGTH> <COMPLETE_OPERATOR>
///
///         TYPE => { T(aint), U(n-taint) }
///         DST  => { e.g. M[0x11223344], H[0x11223344] }
///         COMPLETE_OPERATOR => DIFT_DEBUG_OP_COMPLETE
///
/// Note that if the color log is enabled, a tainted operand will be shown in red or green otherwise.
///

#define DIFT_DEBUG_CONDITION

#define DIFT_DEBUG_OP_ASSIGN   "<-"
#define DIFT_DEBUG_OP_APPEND   "<="
#define DIFT_DEBUG_OP_COMPLETE " *\n"

#if defined(DIFT_DEBUG_COLOR)
#define DIFT_DEBUG_COLOR_TAINTED    "\x1b[1;31m"
#define DIFT_DEBUG_COLOR_CLEAN      "\x1b[1;32m"
#define DIFT_DEBUG_COLOR_OPERATOR   "\x1b[0m"
#define DIFT_DEBUG_COLOR_RESET      "\x1b[0m"
#else
#define DIFT_DEBUG_COLOR_TAINTED    ""
#define DIFT_DEBUG_COLOR_CLEAN      ""
#define DIFT_DEBUG_COLOR_OPERATOR   ""
#define DIFT_DEBUG_COLOR_RESET      ""
#endif

#define DIFT_DEBUG_COMPLETE_SIGN    " *\n"
#endif

// Constant Definition
#define REC_END_SYMBOL  0x9a0a0a0000000000
#define CHUNK_AVAILABLE 0
#define CHUNK_FILLING   1
#define CHUNK_FILLED    2
#define CHUNK_CONSUMING 3

#define SIZE_OF_CHUNK       (1 << 12)
#define CHUNK_BITSIZEMASK   ~(SIZE_OF_CHUNK - 1)
#define Q_CHUNKS_SIZE       (CONFIG_SIZE_OF_QUEUE / SIZE_OF_CHUNK)

#define EFFECT_CLEAR        (1 << 0)
#define EFFECT_ASSIGN       (1 << 1)
#define EFFECT_APPEND       (1 << 2)
#define EFFECT_ONE_TO_ONE   (1 << 3)
#define EFFECT_MIX          (1 << 4)
#define EFFECT_INSIDE_REG   (1 << 5)
#define EFFECT_LOG_END      (1 << 6)

#define OPT_REG  0
#define OPT_MEM  1
#define OPT_IM   2
#define OPT_HD   3

#define R_RIP               16
#define R_SEG_REG_BASE      17
#define R_DIFT_TMP          23
#define R_NONE              24

// Type Definition
#define CONTAMINATION_RECORD uint8_t

// Return Code of Public API
enum DIFT_RETURN_CODE {
    DIFT_SUCCESS,
    DIFT_ERR_OUTOFRANGE,
    DIFT_ERR_INVALIDTAG,
    DIFT_ERR_FAIL,
};

///
/// We use 2-layer page table to record the harddisk taint status
/// Since the initial image is 16GB size, the address need 36 bits to cover the addressing space
/// (same as PAE except that we use only 2-layer page translation)
///
#define HD_L1_INDEX_BITS    24
#define HD_L2_INDEX_BITS    12
#define HD_L1_SIZE          (1<<HD_L1_INDEX_BITS)
#define HD_L2_SIZE          (1<<HD_L2_INDEX_BITS)
#define HD_L1_INDEX_MASK    (~(((1 << HD_L1_INDEX_BITS) - 1) << HD_L2_INDEX_BITS))
#define HD_L2_INDEX_MASK    (~((1 << HD_L2_INDEX_BITS) - 1))
#define HD_L1_INDEX(addr)   (((addr) & ~HD_L1_INDEX_MASK) >> HD_L2_INDEX_BITS)
#define HD_L2_INDEX(addr)   ((addr) & ~HD_L2_INDEX_MASK)
#define HD_PAGE(addr)       ((hd_l1_dirty_tbl[HD_L1_INDEX(addr)]))

#define HD_MAX_SIZE 0x0000001000000000

enum {
    // Expression: DST_SRC_MAPPING_TYPE_BYTES 
    // 0
    INSIDE_REG_ASSIGN,
    INSIDE_REG_APPEND,
    REG_REG_OO_ASSIGN_MO8,
    REG_REG_OO_ASSIGN_MO16,
    REG_REG_OO_ASSIGN_MO32,

    REG_REG_OO_ASSIGN_MO64,
    REG_REG_OO_APPEND_MO8,
    REG_REG_OO_APPEND_MO16,
    REG_REG_OO_APPEND_MO32,
    REG_REG_OO_APPEND_MO64,

    // 10
    REG_REG_MIX_APPEND_MO8,
    REG_REG_MIX_APPEND_MO16,
    REG_REG_MIX_APPEND_MO32,
    REG_REG_MIX_APPEND_MO64,
    REG_MEM_OO_ASSIGN_MO8,

    REG_MEM_OO_ASSIGN_MO16,
    REG_MEM_OO_ASSIGN_MO32,
    REG_MEM_OO_ASSIGN_MO64,
    REG_MEM_OO_APPEND_MO8,
    REG_MEM_OO_APPEND_MO16,

    // 20 
    REG_MEM_OO_APPEND_MO32,
    REG_MEM_OO_APPEND_MO64,
    REG_MEM_MIX_ASSIGN_MO16,
    REG_MEM_MIX_ASSIGN_MO32,
    REG_MEM_MIX_ASSIGN_MO64,

    REG_MEM_MIX_APPEND_MO8,
    REG_MEM_MIX_APPEND_MO16,
    REG_MEM_MIX_APPEND_MO32,
    REG_MEM_MIX_APPEND_MO64,
    MEM_REG_OO_ASSIGN_MO8,

    // 30 
    MEM_REG_OO_ASSIGN_MO16,
    MEM_REG_OO_ASSIGN_MO32,
    MEM_REG_OO_ASSIGN_MO64,
    MEM_REG_OO_APPEND_MO8,
    MEM_REG_OO_APPEND_MO16,

    MEM_REG_OO_APPEND_MO32,
    MEM_REG_OO_APPEND_MO64,
    MEM_REG_MIX_ASSIGN_MO16,
    MEM_REG_MIX_ASSIGN_MO32,
    MEM_REG_MIX_ASSIGN_MO64,

    // 40 
    MEM_REG_MIX_APPEND_MO8,
    MEM_REG_MIX_APPEND_MO16,
    MEM_REG_MIX_APPEND_MO32,
    MEM_REG_MIX_APPEND_MO64,
    MEM_MEM_OO_ASSIGN_MO8,

    MEM_MEM_OO_ASSIGN_MO16,
    MEM_MEM_OO_ASSIGN_MO32,
    MEM_MEM_OO_ASSIGN_MO64,
    REG_IM_CLEAR_MO8,
    REG_IM_CLEAR_MO16,

    // 50
    REG_IM_CLEAR_MO32,
    REG_IM_CLEAR_MO64,
    MEM_IM_CLEAR_MO8,
    MEM_IM_CLEAR_MO16,
    MEM_IM_CLEAR_MO32,

    MEM_IM_CLEAR_MO64,
    MEM_HD,
    HD_MEM,
    REC_CONTAMINATE_MEM_OR,
    REC_CONTAMINATE_MEM_AND,

    // 60
    REC_CONTAMINATE_HD_OR,
    REC_CONTAMINATE_HD_AND,
    REC_CLEAR_MEM,
    REC_BEFORE_BLOCK_BEGIN,
    REC_BLOCK_BEGIN,

    REC_SYNC,
    REC_END
};

struct dift_record {

    uint8_t case_nb;

    union
    {
        struct
        {
            uint8_t reg;
            uint8_t d_byte;
            uint8_t s_byte;
        }inside_r;

        struct
        {
            uint8_t sreg;
            uint8_t dreg;
        }r2r;

        struct
        {
            uint8_t sreg;
            uint8_t dreg;
            uint8_t sreg_byte;
            uint8_t dreg_byte;
        }r2r_byte;

        struct
        {
            uint8_t reg;
#if defined(CONFIG_INDIRECT_TAINT)
            uint8_t reg_index;
            uint8_t reg_base;
#endif
        }r2m_m2r;

        struct
        {
            uint8_t reg;
            uint8_t hl;
#if defined(CONFIG_INDIRECT_TAINT)
            uint8_t reg_index;
            uint8_t reg_base;
#endif
        }r2m_m2r_byte;

        struct
        {
            uint8_t padding1;
            uint8_t padding2;
            uint8_t padding3;
            uint8_t padding4;
            uint8_t padding5;
            uint8_t padding6;
            uint8_t padding7;
        }padding;
    } v1;

} __attribute__((__packed__));

struct dift_context {

    uint64_t* deqptr;
    uint32_t  tail, prev_tail;
    CONTAMINATION_RECORD reg_dirty_tbl[24][8];
    CONTAMINATION_RECORD *mem_dirty_tbl;
    CONTAMINATION_RECORD **hd_l1_dirty_tbl;

#if defined(CONFIG_DIFT_DEBUG)  
    uint64_t tb_rip;
    uint64_t tb_tc_ptr;
#endif  
};


/// 
/// The following variables are declared as global variables for DIFT implementation
/// It is STRONGLY recommanded that you should AVOID accessing them directly.
/// Instead, DIFT-related features should be used via the exported "dift_xxx" APIs
///
typedef struct dift_context dift_context;
typedef struct dift_record  dift_record;

extern const dift_record DIFT_REC_EMPTY;

extern uint64_t phys_ram_base;
extern uint64_t phys_ram_size;

extern uint64_t  q_records[];
extern uint64_t* q_chunks_ptr[];
extern volatile uint64_t q_chunks_flag[];
extern volatile uint64_t *enqptr;
extern volatile uint64_t head, prev_head;

extern uint64_t last_mem_read_addr;
extern uint64_t last_mem_write_addr;
extern bool     last_blk_io_dma;

extern volatile uint64_t clean_source;
extern volatile uint64_t null_sink;

extern volatile int nic_rx_taint;
extern volatile int nic_tx_taintcheck;

extern volatile uint32_t dift_thread_ok_signal;

extern CONTAMINATION_RECORD hd_dirty_tbl[];
extern uint64_t dift_code_buffer[];
extern uint32_t dift_code_top;
extern uint32_t dift_code_cntr;
extern uint32_t dift_code_loc;
extern uint32_t dift_code_off;
extern int label_or_helper_appeared;

extern uint8_t* rt_get_next_enqptr;
extern uint8_t* rt_finish_curr_block;
extern uint8_t* rt_enqueue_one_rec;
extern uint8_t* rt_enqueue_raddr;
extern uint8_t* rt_enqueue_waddr;

/// Inform dift system and then block(spinning) until
/// dift system declares to accept more IFcodes.
/// Current Called before translating every code blocks.
///
/// no parameter
///
/// no return value
extern void dift_sync(void);

///
/// DIFT Logging functions
///
#define DIFT_LOG "dift.log"
extern FILE* dift_logfile;
extern int   dift_log( const char*, ... );


/// Initializes dift function.
/// Currently called in pc_init1() (in Pc_piix.c)
/// 
/// No parameter
/// 
/// Returns 1 if create dift thread successfully,0 otherwise.
extern int dift_start(void);

/// Pushes a "struct dift_record", or additional information(like addr) into the queue.
/// 
/// \param data_in  struct dift_record data_in, which is going to be enqueued
/// 
/// No return value
extern void dift_rec_enqueue( uint64_t data_in );

/// Enable DIFT taint propagation
///
/// no parameter
///
/// no return value
extern void dift_enable(void);

/// Disable DIFT taint propagation
///
/// no parameter
/// 
/// no return value
extern void dift_disable(void);

/// Retrieve the DIFT status (enabled or disabled)
/// 
/// no parameter
///
/// Return true if DIFT is enabled, false otherwise
extern bool dift_is_enabled(void);

/// Get the case number corresponding to specified arguments.
/// The case numbers are also the indices of array "dispatch",
/// which stores addresses for the IFcode handlers.
/// The case number is correspond to the field .case_nb of struct dift_record.
/// 
/// \param dst_type	destination type 
/// \param src_type	source type
/// \param ot		operand type(8,16,32,etc)
/// \param effect   effect type
///
/// No return value
extern uint8_t dift_rec_case_nb(uint8_t dst_type, uint8_t src_type, uint8_t ot, uint8_t effect);

/// Set the contaminate record to the result of or operation with tag.
/// 
/// \param addr     The starting memory address to be set
/// \param len      The length(in bytes) to be set
/// \param tag      The contaminate value
/// 
/// Returns DIFT_SUCCESS while success, DIFT_ERR_FAIL otherwise.
extern int dift_contaminate_memory_or(uint64_t addr, uint64_t len, CONTAMINATION_RECORD tag);

/// Set the contaminate record to the result of and operation with tag.
/// 
/// \param addr     The starting memory address to be set
/// \param len      The length(in bytes) to be set
/// \param tag      The contaminate value
/// 
/// Returns DIFT_SUCCESS while success, DIFT_ERR_FAIL otherwise.
extern int dift_contaminate_memory_and(uint64_t addr, uint64_t len, CONTAMINATION_RECORD tag);

/// Set the contaminate record to the result of or operation with tag.
/// 
/// \param addr     The starting disk address to be set
/// \param len      The length(in bytes) to be set
/// \param tag      The contaminate value
/// 
/// Returns DIFT_SUCCESS while success, DIFT_ERR_FAIL otherwise.
extern int dift_contaminate_disk_or(uint64_t addr, uint64_t len, CONTAMINATION_RECORD tag);

/// Set the contaminate record to the result of and operation with tag.
/// 
/// \param addr     The starting disk address to be set
/// \param len      The length(in bytes) to be set
/// \param tag      The contaminate value
/// 
/// Returns DIFT_SUCCESS while success, DIFT_ERR_FAIL otherwise.
extern int dift_contaminate_disk_and(uint64_t addr, uint64_t len, CONTAMINATION_RECORD tag);

/// Get the contamination status of a phsical memory byte.
/// The most significant bit is used for marking whether a code block contains tainted code.
/// 
/// \param offset   The address of the byte in memory to be checked
/// 
/// Returns CONTAMINATION_RECORD type.
extern CONTAMINATION_RECORD dift_get_memory_dirty(uint64_t offset);

/// Get the contamination status of a disk byte.
/// The most significant bit is used for marking whether a code block contains tainted code.
/// 
/// \param hdaddr   The address of the byte in disk to be checked
/// 
/// Returns CONTAMINATION_RECORD type.
extern CONTAMINATION_RECORD dift_get_disk_dirty(uint64_t hdaddr);

/// Flush the record queue if needed.
///
/// \param cnt   Number of sets of records that have already been enqueued. Each set of records
///              may be enqueued by several enqueue() call.
/// FIXME(misterlihao): Definition not found.
void record_queue_flush(size_t cnt);

/// FIXME(misterlihao): Definition not found.
extern void clear_memory(uint64_t, uint64_t);


// DIFT context
extern dift_context dc[];
#endif

