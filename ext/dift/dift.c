/*
 *  De-coupled Information Flow Tracking (DIFT) implementation
 *
 *  Copyright (c) 2016 Chiawei Wang
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

#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <assert.h>
#include <stdint.h>

#if !defined(CONFIG_DIFT_GTEST)
#include <config.h>
#endif

/// we try to make DIFT as standalone as possible
//#include "../../target-i386/cpu.h"
//#include "../../tcg/tcg.h"

#include "dift.h"
//#include "ifcode-translator.h"

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

// NOTE!!! The following enum definition must be synchronized with the TCGMemOp of QEMU
enum {
    MO_8,
    MO_16,
    MO_32,
    MO_64
};

// Current implementation of 64-bits page table treat the bits 48 ~ 63 (16 bits) as sign extension.
// The page no. bit in use should be 12 ~ 47 (36 bits). The page offset remains 12 bits (in most case, for 4K page)
#define PAGESE_BITS   16
#define PAGENO_BITS   36
#define OFFSET_BITS   12
#define PAGE_MASK   (~(((1 << (PAGESE_BITS + PAGENO_BITS)) - 1) << OFFSET_BITS))
#define OFFSET_MASK   (~((1 << OFFSET_BITS) - 1))
#define PAGEOF(addr)  (((addr) & ~PAGE_MASK) >> OFFSET_BITS)
#define OFFSETOF(addr)  ((addr) & ~OFFSET_MASK)
#define NOT_LONG_SPANNING_PAGE(addr) (likely((~(uint32_t)(addr | ~PAGE_MASK)) > 2))
#define NOT_WORD_SPANNING_PAGE(addr) (likely( ~(uint32_t)(addr | ~PAGE_MASK)))

#define out8(p, x)  *(uint8_t*)p  = ((uint8_t)(x));  p++;
#define out32(p, x) *(uint32_t*)p = ((uint32_t)(x)); p += sizeof(uint32_t);
#define out64(p, x) *(uint64_t*)p = ((uint64_t)(x)); p += sizeof(uint64_t);

/* Global Variables */
uint64_t phys_ram_base = 0;
uint64_t phys_ram_size = 0;

#define DIFT_LOG "dift.log"
FILE* dift_logfile = NULL;

/* Emulator part */
uint8_t pre_generated_routine[4096] __attribute__((aligned(4096)));

uint8_t* rt_get_next_enqptr		= pre_generated_routine;
uint8_t* rt_finish_curr_block	= pre_generated_routine + 512;
uint8_t* rt_enqueue_one_rec		= pre_generated_routine + 1024;
uint8_t* rt_enqueue_raddr		= pre_generated_routine + 1536;
uint8_t* rt_enqueue_waddr		= pre_generated_routine + 2048;
uint8_t* rt_qemu_ld				= pre_generated_routine + 2560;
uint8_t* rt_qemu_st				= pre_generated_routine + 3072;

uint64_t last_mem_read_addr;
uint64_t last_mem_write_addr;

volatile uint64_t* enqptr __attribute__((aligned(4096)));
volatile uint64_t  head, prev_head;

volatile uint64_t clean_source;
volatile uint64_t null_sink;

volatile int nic_rx_taint;
volatile int nic_tx_taintcheck;

uint8_t case_mapping[1 << (sizeof(uint16_t) * 8)];
uint16_t case_list[] ={

    // encoding:
    // ((0xc0 | SRC_TYPE | DST_TYPE | OP_SIZE) << 8) | EFFECT)
    // The sequence should match the ENUM in dift.h
    ((0xc0 | OPT_REG << 4 | OPT_REG << 2 | MO_8) << 8) |
                        EFFECT_INSIDE_REG | EFFECT_ASSIGN,  // 0
    ((0xc0 | OPT_REG << 4 | OPT_REG << 2 | MO_8) << 8) |
                        EFFECT_INSIDE_REG | EFFECT_APPEND,  // 1
    ((0xc0 | OPT_REG << 4 | OPT_REG << 2 | MO_8) << 8) |
                        EFFECT_ONE_TO_ONE | EFFECT_ASSIGN,  // 2
    ((0xc0 | OPT_REG << 4 | OPT_REG << 2 | MO_16) << 8) |
                        EFFECT_ONE_TO_ONE | EFFECT_ASSIGN,  // 3
    ((0xc0 | OPT_REG << 4 | OPT_REG << 2 | MO_32) << 8) |
                        EFFECT_ONE_TO_ONE | EFFECT_ASSIGN,  // 4

    ((0xc0 | OPT_REG << 4 | OPT_REG << 2 | MO_64) << 8) |
                        EFFECT_ONE_TO_ONE | EFFECT_ASSIGN,  // 5
    ((0xc0 | OPT_REG << 4 | OPT_REG << 2 | MO_8) << 8) |
                        EFFECT_ONE_TO_ONE | EFFECT_APPEND,  // 6
    ((0xc0 | OPT_REG << 4 | OPT_REG << 2 | MO_16) << 8) |
                        EFFECT_ONE_TO_ONE | EFFECT_APPEND,  // 7
    ((0xc0 | OPT_REG << 4 | OPT_REG << 2 | MO_32) << 8) |
                        EFFECT_ONE_TO_ONE | EFFECT_APPEND,  // 8
    ((0xc0 | OPT_REG << 4 | OPT_REG << 2 | MO_64) << 8) |
                        EFFECT_ONE_TO_ONE | EFFECT_APPEND,  // 9

    ((0xc0 | OPT_REG << 4 | OPT_REG << 2 | MO_8) << 8) |
                        EFFECT_MIX | EFFECT_APPEND,         // 10    
    ((0xc0 | OPT_REG << 4 | OPT_REG << 2 | MO_16) << 8) |
                        EFFECT_MIX | EFFECT_APPEND,         // 11    
    ((0xc0 | OPT_REG << 4 | OPT_REG << 2 | MO_32) << 8) |
                        EFFECT_MIX | EFFECT_APPEND,         // 12
    ((0xc0 | OPT_REG << 4 | OPT_REG << 2 | MO_64) << 8) |
                        EFFECT_MIX | EFFECT_APPEND,         // 13
    ((0xc0 | OPT_MEM << 4 | OPT_REG << 2 | MO_8) << 8) |
                        EFFECT_ONE_TO_ONE | EFFECT_ASSIGN,  // 14

    ((0xc0 | OPT_MEM << 4 | OPT_REG << 2 | MO_16) << 8) |
                        EFFECT_ONE_TO_ONE | EFFECT_ASSIGN,  // 15
    ((0xc0 | OPT_MEM << 4 | OPT_REG << 2 | MO_32) << 8) |
                        EFFECT_ONE_TO_ONE | EFFECT_ASSIGN,  // 16
    ((0xc0 | OPT_MEM << 4 | OPT_REG << 2 | MO_64) << 8) |
                        EFFECT_ONE_TO_ONE | EFFECT_ASSIGN,  // 17
    ((0xc0 | OPT_MEM << 4 | OPT_REG << 2 | MO_8) << 8) |
                        EFFECT_ONE_TO_ONE | EFFECT_APPEND,  // 18
    ((0xc0 | OPT_MEM << 4 | OPT_REG << 2 | MO_16) << 8) |
                        EFFECT_ONE_TO_ONE | EFFECT_APPEND,  // 19

    ((0xc0 | OPT_MEM << 4 | OPT_REG << 2 | MO_32) << 8) |
                        EFFECT_ONE_TO_ONE | EFFECT_APPEND,  // 20
    ((0xc0 | OPT_MEM << 4 | OPT_REG << 2 | MO_64) << 8) |
                        EFFECT_ONE_TO_ONE | EFFECT_APPEND,  // 21
    ((0xc0 | OPT_MEM << 4 | OPT_REG << 2 | MO_16) << 8) |
                        EFFECT_MIX | EFFECT_ASSIGN,         // 22
    ((0xc0 | OPT_MEM << 4 | OPT_REG << 2 | MO_32) << 8) |
                        EFFECT_MIX | EFFECT_ASSIGN,         // 23
    ((0xc0 | OPT_MEM << 4 | OPT_REG << 2 | MO_64) << 8) |
                        EFFECT_MIX | EFFECT_ASSIGN,         // 24

    ((0xc0 | OPT_MEM << 4 | OPT_REG << 2 | MO_8) << 8) |
                        EFFECT_MIX | EFFECT_APPEND,         // 25
    ((0xc0 | OPT_MEM << 4 | OPT_REG << 2 | MO_16) << 8) |
                        EFFECT_MIX | EFFECT_APPEND,         // 26
    ((0xc0 | OPT_MEM << 4 | OPT_REG << 2 | MO_32) << 8) |
                        EFFECT_MIX | EFFECT_APPEND,         // 27
    ((0xc0 | OPT_MEM << 4 | OPT_REG << 2 | MO_64) << 8) |
                        EFFECT_MIX | EFFECT_APPEND,         // 28
    ((0xc0 | OPT_REG << 4 | OPT_MEM << 2 | MO_8) << 8) |
                        EFFECT_ONE_TO_ONE | EFFECT_ASSIGN,  // 29

    ((0xc0 | OPT_REG << 4 | OPT_MEM << 2 | MO_16) << 8) |
                        EFFECT_ONE_TO_ONE | EFFECT_ASSIGN,  // 30
    ((0xc0 | OPT_REG << 4 | OPT_MEM << 2 | MO_32) << 8) |
                        EFFECT_ONE_TO_ONE | EFFECT_ASSIGN,  // 31
    ((0xc0 | OPT_REG << 4 | OPT_MEM << 2 | MO_64) << 8) |
                        EFFECT_ONE_TO_ONE | EFFECT_ASSIGN,  // 32
    ((0xc0 | OPT_REG << 4 | OPT_MEM << 2 | MO_8) << 8) |
                        EFFECT_ONE_TO_ONE | EFFECT_APPEND,  // 33
    ((0xc0 | OPT_REG << 4 | OPT_MEM << 2 | MO_16) << 8) |
                        EFFECT_ONE_TO_ONE | EFFECT_APPEND,  // 34

    ((0xc0 | OPT_REG << 4 | OPT_MEM << 2 | MO_32) << 8) |
                        EFFECT_ONE_TO_ONE | EFFECT_APPEND,  // 35
    ((0xc0 | OPT_REG << 4 | OPT_MEM << 2 | MO_64) << 8) |
                        EFFECT_ONE_TO_ONE | EFFECT_APPEND,  // 36
    ((0xc0 | OPT_REG << 4 | OPT_MEM << 2 | MO_16) << 8) |
                        EFFECT_MIX | EFFECT_ASSIGN,         // 37
    ((0xc0 | OPT_REG << 4 | OPT_MEM << 2 | MO_32) << 8) |
                        EFFECT_MIX | EFFECT_ASSIGN,         // 38
    ((0xc0 | OPT_REG << 4 | OPT_MEM << 2 | MO_64) << 8) |
                        EFFECT_MIX | EFFECT_ASSIGN,         // 39

    ((0xc0 | OPT_REG << 4 | OPT_MEM << 2 | MO_8) << 8) |
                        EFFECT_MIX | EFFECT_APPEND,         // 40
    ((0xc0 | OPT_REG << 4 | OPT_MEM << 2 | MO_16) << 8) |
                        EFFECT_MIX | EFFECT_APPEND,         // 41
    ((0xc0 | OPT_REG << 4 | OPT_MEM << 2 | MO_32) << 8) |
                        EFFECT_MIX | EFFECT_APPEND,         // 42
    ((0xc0 | OPT_REG << 4 | OPT_MEM << 2 | MO_64) << 8) |
                        EFFECT_MIX | EFFECT_APPEND,         // 43
    ((0xc0 | OPT_MEM << 4 | OPT_MEM << 2 | MO_8) << 8) |
                        EFFECT_ONE_TO_ONE | EFFECT_ASSIGN,  // 44

    ((0xc0 | OPT_MEM << 4 | OPT_MEM << 2 | MO_16) << 8) |
                        EFFECT_ONE_TO_ONE | EFFECT_ASSIGN,  // 45
    ((0xc0 | OPT_MEM << 4 | OPT_MEM << 2 | MO_32) << 8) |
                        EFFECT_ONE_TO_ONE | EFFECT_ASSIGN,  // 46
    ((0xc0 | OPT_MEM << 4 | OPT_MEM << 2 | MO_64) << 8) |
                        EFFECT_ONE_TO_ONE | EFFECT_ASSIGN,  // 47
    ((0xc0 | OPT_IM << 4 | OPT_REG << 2 | MO_8) << 8) |
                        EFFECT_CLEAR,                       // 48
    ((0xc0 | OPT_IM << 4 | OPT_REG << 2 | MO_16) << 8) |
                        EFFECT_CLEAR,                       // 49

    ((0xc0 | OPT_IM << 4 | OPT_REG << 2 | MO_32) << 8) |
                        EFFECT_CLEAR,                       // 50
    ((0xc0 | OPT_IM << 4 | OPT_REG << 2 | MO_64) << 8) |
                        EFFECT_CLEAR,                       // 51
    ((0xc0 | OPT_IM << 4 | OPT_MEM << 2 | MO_8) << 8) |
                        EFFECT_CLEAR,                       // 52
    ((0xc0 | OPT_IM << 4 | OPT_MEM << 2 | MO_16) << 8) |
                        EFFECT_CLEAR,                       // 53
    ((0xc0 | OPT_IM << 4 | OPT_MEM << 2 | MO_32) << 8) |
                        EFFECT_CLEAR,                       // 54

    ((0xc0 | OPT_IM << 4 | OPT_MEM << 2 | MO_64) << 8) |
                        EFFECT_CLEAR,                       // 55
};

uint32_t dift_code_top =  1;  // Loc 0 is reserved for the workaround in add_file_taint
uint32_t dift_code_cntr = 0;
uint32_t dift_code_loc;
uint32_t dift_code_off;

// The size of the allocation should be fixed
uint64_t dift_code_buffer[CONFIG_MAX_TB_ESTI * CONFIG_IF_CODES_PER_TB] __attribute__((aligned(4096)));    

int label_or_helper_appeared = 0;

/// DIFT Communication
uint64_t          q_records[CONFIG_SIZE_OF_QUEUE / sizeof(uint64_t)] __attribute__((aligned(4096)));
uint64_t*         q_chunks_ptr[Q_CHUNKS_SIZE];
volatile uint64_t q_chunks_flag[Q_CHUNKS_SIZE];
volatile uint32_t dift_thread_ok_signal = 0;

int sleepness = 0;

/// DIFT Context
dift_context dc[1] __attribute__((aligned(4096)));
static pthread_t dift_thread;

const char* REG_NAME[] = {
    "RAX", "RCX", "RDX", "RBX", "RSP", "RBP", "RSI", "RDI",
    "R8",  "R9",  "R10", "R11", "R12", "R13", "R14", "R15",
    "RIP", "ES",  "CS",  "SS",  "DS",  "FS",  "GS",  "TMP",
	"NONE"
};


/// Pre-generate routine for DIFT TCG usage
static void gen_rt_finish_curr_block( void ) {
    
    uint8_t* s;

    s = rt_finish_curr_block;
	
    out8(s, 0x0f);
    out8(s, 0xae);
    out8(s, 0xf0);      // mfence

    out8(s, 0x48);
    out8(s, 0x31);
    out8(s, 0xc0);      // xor rax, rax

    out8(s, 0x48);
    out8(s, 0xa3);
    out64(s, &enqptr);  // mov [&enqptr], rax
     
    out8(s, 0xa1);
    out64(s, &prev_head);   // mov eax, [&prev_head]
    
    out8(s, 0x48);
    out8(s, 0xc1);
    out8(s, 0xe0);
    out8(s, 0x03);      // shl rax, 0x3

    out8(s, 0x48);
    out8(s, 0xba);
    out64(s, q_chunks_flag);   // mov rdx, q_chunks_flag

    out8(s, 0x48);
    out8(s, 0x01);
    out8(s, 0xc2);      // add rdx, rax

    out8(s, 0xc7);
    out8(s, 0x02);
    out32(s, CHUNK_FILLED); // mov [rdx], CHUNK_FILLED

    out8(s, 0xc3);
}

static void gen_rt_get_next_enqptr( void ) {

    uint8_t* s;

    s = rt_get_next_enqptr;

    out8(s, 0xf3);
    out8(s, 0x90);      // pause
    
    out8(s, 0x48);
    out8(s, 0xa1);
    out64(s, &head);    // mov rax, [&head]
    
    out8(s, 0x48);
    out8(s, 0xc1);
    out8(s, 0xe0);
    out8(s, 0x03);      // shl rax, 0x3

    out8(s, 0x48);
    out8(s, 0xba);
    out64(s, q_chunks_flag);   // mov rdx, q_chunks_flag

    out8(s, 0x48);
    out8(s, 0x01);
    out8(s, 0xc2);      // add rdx, rax

    out8(s, 0x48);
    out8(s, 0x8b);
    out8(s, 0x02);      // mov rax, [rdx]

    out8(s, 0x48);
    out8(s, 0x85);
    out8(s, 0xc0);      // test rax, rax

    out8(s, 0x75);
    out8(s, rt_get_next_enqptr - (s+1));    // jne rt_get_next_enqptr
    
    out8(s, 0xc7);
    out8(s, 0x02);
    out32(s, CHUNK_FILLING);    // mov [rdx], CHUNK_FILLING

    out8(s, 0x48);
    out8(s, 0xa1);
    out64(s, &head);    // mov rax, [&head]
    
    out8(s, 0x48);
    out8(s, 0xc1);
    out8(s, 0xe0);
    out8(s, 0x03);      // shl rax, 0x3

    out8(s, 0x48);
    out8(s, 0xba);
    out64(s, q_chunks_ptr);   // mov rdx, q_chunks_ptr

    out8(s, 0x48);
    out8(s, 0x01);
    out8(s, 0xc2);      // add rdx, rax

    out8(s, 0x48);
    out8(s, 0x8b);
    out8(s, 0x02);      // mov rax, [rdx]

    out8(s, 0x48);
    out8(s, 0xa3);
    out64(s, &enqptr);  // mov [&enqptr], rax
   
    out8(s, 0xa1);
    out64(s, &head);    //  mov eax, [&head]

    out8(s, 0xa3);
    out64(s, &prev_head);   // mov [&prev_head], eax

    out8(s, 0xff);
    out8(s, 0xc0);          // inc eax

    out8(s, 0x25);
    out32(s, Q_CHUNKS_SIZE - 1);    // and eax, Q_CHUNKS_SIZE
    
    out8(s, 0xa3);
    out64(s, &head);    // mov [&head], eax

    out8(s, 0x48);
    out8(s, 0xa1);
    out64(s, &enqptr);  // mov rax, [&enqptr]

    out8(s, 0xc3);
}

static void gen_rt_enqueue_one_rec( void ) {

    uint8_t* s;
    uint8_t* label2_ptr;

    s = rt_enqueue_one_rec;

    // The following assembly is denoted in Intel syntax
    out8(s, 0x48);
    out8(s, 0xa1);
    out64(s, &enqptr);  // mov rax, [&enqptr]

    out8(s, 0x48);
    out8(s, 0x89);
    out8(s, 0x10);      // mov [rax], rdx

    out8(s, 0x48);
    out8(s, 0x83);
    out8(s, 0xc0);
    out8(s, 0x08);      // add rax, 0x8

    out8(s, 0xa9);
    out32(s, ~CHUNK_BITSIZEMASK);      // test rax, ~CHUNK_BITSIZEMASK

    out8(s, 0x48);
    out8(s, 0xa3);
    out64(s, &enqptr);  // mov [&enqptr], rax

    out8(s, 0x74);
    label2_ptr = s;
    s++;                // je label2

    out8(s, 0xc3);      // ret

    *label2_ptr = s - label2_ptr - 1;   // label2

    out8(s, 0xe8);
    out32(s, rt_finish_curr_block - (s + 4));   // call rt_finish_curr_block

    out8(s, 0xe8);
    out32(s, rt_get_next_enqptr - (s + 4));     // call rt_get_next_enqptr

    out8(s, 0xc3);      // ret
}

static void gen_rt_enqueue_raddr( void ) {

    uint8_t* s;
    uint8_t* label2_ptr;

    s = rt_enqueue_raddr;
    
    // The following assembly is denoted in Intel syntax 
    out8(s, 0x48);
    out8(s, 0xba);
    out64(s, (uintptr_t)&last_mem_read_addr);    // mov rdx, &last_mem_read_addr

    out8(s, 0x48);
    out8(s, 0x8b);
    out8(s, 0x12);      // mov rdx, [rdx]

	out8(s, 0x48);
    out8(s, 0xa1);
    out64(s, &phys_ram_base);    // mov rax, [&phys_ram_base]

    out8(s, 0x48);
    out8(s, 0x29);
    out8(s, 0xc2);      // sub rdx, rax
    
    out8(s, 0x48);
    out8(s, 0xa1);
    out64(s, &enqptr);  // mov rax, [&enqptr]

    out8(s, 0x48);
    out8(s, 0x89);
    out8(s, 0x10);      // mov [rax], rdx

    out8(s, 0x48);
    out8(s, 0x83);
    out8(s, 0xc0);
    out8(s, 0x08);      // add rax, 0x8

    out8(s, 0xa9);
    out32(s, ~CHUNK_BITSIZEMASK);      // test eax, ~CHUNK_BITSIZEMASK

    out8(s, 0x48);
    out8(s, 0xa3);
    out64(s, &enqptr);  // mov [&enqptr], rax

    out8(s, 0x74);
    label2_ptr = s;
    s++;                // je label2

    out8(s, 0xc3);      // ret

    *label2_ptr = s - label2_ptr - 1;           // label2

    out8(s, 0xe8);
    out32(s, rt_finish_curr_block - (s + 4));   // call rt_finish_curr_block

    out8(s, 0xe8);
    out32(s, rt_get_next_enqptr - (s + 4));     // call rt_get_next_enqptr

    out8(s, 0xc3);      // ret
}

static void gen_rt_enqueue_waddr( void ) {
    
    uint8_t* s;
    uint8_t* label2_ptr;

    s = rt_enqueue_waddr;
    
    // The following assembly is denoted in Intel syntax
    out8(s, 0x48);
    out8(s, 0xba);
    out64(s, (uintptr_t)&last_mem_write_addr);    // mov rdx, &last_mem_write_addr

    out8(s, 0x48);
    out8(s, 0x8b);
    out8(s, 0x12);      // mov rdx, [rdx]

	out8(s, 0x48);
    out8(s, 0xa1);
    out64(s, &phys_ram_base);    // mov rax, [&phys_ram_base]

    out8(s, 0x48);
    out8(s, 0x29);
    out8(s, 0xc2);      // sub rdx, rax
    
    out8(s, 0x48);
    out8(s, 0xa1);
    out64(s, &enqptr);  // mov rax, [&enqptr]

    out8(s, 0x48);
    out8(s, 0x89);
    out8(s, 0x10);      // mov [rax], rdx

    out8(s, 0x48);
    out8(s, 0x83);
    out8(s, 0xc0);
    out8(s, 0x08);      // add rax, 0x8

    out8(s, 0xa9);
    out32(s, ~CHUNK_BITSIZEMASK);      // test eax, ~CHUNK_BITSIZEMASK

    out8(s, 0x48);
    out8(s, 0xa3);
    out64(s, &enqptr);  // mov [&enqptr], rax

    out8(s, 0x74);
    label2_ptr = s;
    s++;                // je label2

    out8(s, 0xc3);      // ret

    *label2_ptr = s - label2_ptr - 1;           // label2

    out8(s, 0xe8);
    out32(s, rt_finish_curr_block - (s + 4));   // call rt_finish_curr_block

    out8(s, 0xe8);
    out32(s, rt_get_next_enqptr - (s + 4));     // call rt_get_next_enqptr

    out8(s, 0xc3);      // ret
}

/// DIFT Initialization 
static void pre_generate_routine( void ) {

    uintptr_t start, end;
    uint64_t page_size;

    gen_rt_finish_curr_block();
    gen_rt_get_next_enqptr();
    gen_rt_enqueue_one_rec();
    gen_rt_enqueue_raddr();
    gen_rt_enqueue_waddr();

#if defined(CONFIG_DIFT_DEBUG)
	dift_log( "rt_finish_curr_block @ %16p\n", rt_finish_curr_block );
	dift_log( "rt_get_next_enqptr   @ %16p\n", rt_get_next_enqptr );
	dift_log( "rt_enqueue_one_rec   @ %16p\n", rt_enqueue_one_rec );
	dift_log( "rt_enqueue_raddr     @ %16p\n", rt_enqueue_raddr );
	dift_log( "rt_enqueue_waddr     @ %16p\n", rt_enqueue_waddr );
	dift_log( "-------------------------------------------------\n" );
#endif	

    /* map */
    page_size = getpagesize();
    start = (uintptr_t)pre_generated_routine;
    start &= ~(page_size - 1);

    end = (uintptr_t)pre_generated_routine + sizeof(pre_generated_routine);
    end += page_size - 1;
    end &= ~(page_size - 1);

    mprotect((void*)start, end - start,
            PROT_READ | PROT_WRITE | PROT_EXEC);
}

static void init_queue( void ) {

    int i;
    for( i = 0; i < Q_CHUNKS_SIZE; ++i ) {
        q_chunks_flag[i] = CHUNK_AVAILABLE;
        q_chunks_ptr[i] = &q_records[ (i * SIZE_OF_CHUNK)/sizeof(uint64_t) ];
    }
    
    // ask for next abailable chunk;
    q_chunks_flag[0] = CHUNK_FILLING;

    enqptr = (uint64_t*)q_chunks_ptr[0];
    head = 1;
    prev_head = 0;
}

static void init_case_mapping( void ) {
    uint32_t j;
    uint16_t i;
    for(j = 0 ; j < sizeof(case_mapping) ; j++)
        case_mapping[j] = 0xff;
    for(i = 0 ; i < sizeof(case_list) / sizeof(uint16_t) ; i++)
        case_mapping[case_list[i]] = i;
}

static void init_dift_context( dift_context *dc ) {

	bzero(dc, sizeof(struct dift_context));
	
	dc->tail = 0;
    dc->prev_tail = Q_CHUNKS_SIZE - 1;

    dc->deqptr = NULL;

#if defined(CONFIG_DIFT_DEBUG)
	dift_log( "Physical RAM size: 0x%016lx\n", phys_ram_size );
#endif

    // allocation for memory taint
    dc->mem_dirty_tbl = (CONTAMINATION_RECORD*)calloc( phys_ram_size + getpagesize(), sizeof(CONTAMINATION_RECORD) );
    if( dc->mem_dirty_tbl == NULL ) {
        fprintf( stderr, "Not enough memory for mem_dirty_tbl, need %016lx bytes\n", phys_ram_size * sizeof(CONTAMINATION_RECORD) + ((uint64_t)1 << PAGENO_BITS) );
        exit( 1 );
    }

    clean_source = phys_ram_base + phys_ram_size + 0;
    null_sink    = phys_ram_base + phys_ram_size + 1;

    // allocation for harddisk taint
    dc->hd_l1_dirty_tbl = (CONTAMINATION_RECORD**)calloc( 1 << HD_L1_INDEX_BITS, sizeof(CONTAMINATION_RECORD*) );
    if( dc->hd_l1_dirty_tbl == NULL ) {
        fprintf( stderr, "Not enough memory for hd_l1_dirty_tbl\n" );
        exit( 1 );
    }
#if defined(CONFIG_TAINT_DIRTY_INS_OUTPUT)
    dc->force_mem_dirty = 0;
#endif
}

/// DIFT Private API - Queue manipulation
static uint64_t* next_block( dift_context *dc ) {

    q_chunks_flag[dc->prev_tail] = CHUNK_AVAILABLE;
    while(unlikely(q_chunks_flag[dc->tail] != CHUNK_FILLED))
    {   
        if( sleepness )
            usleep(1000);
        else
            asm volatile("pause");
    }
    q_chunks_flag[dc->tail] = CHUNK_CONSUMING;
    dc->deqptr = q_chunks_ptr[dc->tail];
    dc->prev_tail = dc->tail;
    dc->tail = (dc->tail + 1) % Q_CHUNKS_SIZE;
    return dc->deqptr;
}

static uint64_t rec_dequeue( dift_context* dc ) {

    register uint64_t ret;

    if( unlikely(((uint64_t)dc->deqptr & ~CHUNK_BITSIZEMASK) == 0) )
        next_block( dc );
    ret = *dc->deqptr;
    dc->deqptr++;

    return ret;
}

static void kick_enqptr(void) {

    if(((uint64_t)enqptr & ~CHUNK_BITSIZEMASK) == 0)
        return;

    asm volatile ("mfence":::"memory");
    q_chunks_flag[prev_head] = CHUNK_FILLED;

    // Ask for next available chunk
    while(q_chunks_flag[head] != CHUNK_AVAILABLE)
    {
        asm volatile("pause");
    }

    q_chunks_flag[head] = CHUNK_FILLING;
    enqptr = (uint64_t*)q_chunks_ptr[head];
    prev_head = head;
    head = (head + 1) % Q_CHUNKS_SIZE;
}

static void wait_dift_analysis( void ) { 
    while( !dift_thread_ok_signal );
}

/// DIFT Private API - Memory taint operation
static inline void set_mem_dirty( dift_context* dc, uint64_t addr, CONTAMINATION_RECORD contamination, int is_append ) {

    if( is_append )
        dc->mem_dirty_tbl[addr] |= contamination;
    else
        dc->mem_dirty_tbl[addr]  = contamination;
}

static inline void and_mem_dirty( dift_context* dc, uint64_t addr, CONTAMINATION_RECORD contamination ) {
    dc->mem_dirty_tbl[addr] &= contamination;
}

static inline CONTAMINATION_RECORD get_mem_dirty( dift_context* dc, uint64_t addr ) { 
    return dc->mem_dirty_tbl[addr];
}

/// DIFT Private API - Disk taint operation
static CONTAMINATION_RECORD* alloc_hd_dirty_page( void ) {
 
    CONTAMINATION_RECORD* rec = (CONTAMINATION_RECORD*)calloc( (1 << HD_L2_INDEX_BITS), sizeof(CONTAMINATION_RECORD) );

    if( rec == NULL ) {
        fprintf( stderr, "Not enough memory for alloc_hd_dirty_page\n" );
        exit( 1 );
    }

    return rec;
}

static void set_hd_dirty_or( dift_context* dc, uint64_t hdaddr, CONTAMINATION_RECORD contamination ) {

	if( contamination == 0 )
    	return;

    if( dc->hd_l1_dirty_tbl[HD_L1_INDEX(hdaddr)] == NULL ) 
        dc->hd_l1_dirty_tbl[HD_L1_INDEX(hdaddr)] = alloc_hd_dirty_page();

    dc->hd_l1_dirty_tbl[HD_L1_INDEX(hdaddr)][HD_L2_INDEX(hdaddr)] |= contamination;
}

static void set_hd_dirty_and( dift_context* dc, uint64_t hdaddr, CONTAMINATION_RECORD contamination ) {

    if( dc->hd_l1_dirty_tbl[HD_L1_INDEX(hdaddr)] == NULL ) 
        return;

    dc->hd_l1_dirty_tbl[HD_L1_INDEX(hdaddr)][HD_L2_INDEX(hdaddr)] &= contamination;
}

static CONTAMINATION_RECORD get_hd_dirty( dift_context* dc, uint64_t hdaddr ) {
    
    CONTAMINATION_RECORD* hd_l2_dirty_tbl = NULL;

    if( (hd_l2_dirty_tbl = dc->hd_l1_dirty_tbl[HD_L1_INDEX(hdaddr)]) == NULL )
        return 0;
    return hd_l2_dirty_tbl[HD_L2_INDEX(hdaddr)];
}

/// DIFT Private API - MEM <--> HD taint propogation (copy_contamination_DST_SRC)
static void copy_contamination_hd_mem( dift_context* dc, uint64_t ra, uint64_t hdaddr, uint64_t len ) {

    uint64_t progress = ((hdaddr + (1 << HD_L2_INDEX_BITS)) & HD_L2_INDEX_MASK) - hdaddr;

    while( len ) {

        progress = progress < len ? progress : len;

        if( dc->hd_l1_dirty_tbl[HD_L1_INDEX(hdaddr)] == NULL )
            dc->hd_l1_dirty_tbl[HD_L1_INDEX(hdaddr)] = alloc_hd_dirty_page();

        memcpy( 
            &dc->hd_l1_dirty_tbl[HD_L1_INDEX(hdaddr)][HD_L2_INDEX(hdaddr)],
            &dc->mem_dirty_tbl[ra],
            progress * sizeof(CONTAMINATION_RECORD) );

        len    -= progress;
        ra     += progress;
        hdaddr += progress;

        progress = ((hdaddr + (1 << HD_L2_INDEX_BITS)) & HD_L2_INDEX_MASK) - hdaddr;
    }
}

static void copy_contamination_mem_hd( dift_context* dc, uint64_t hdaddr, uint64_t wa, uint64_t len ) {
    
    uint64_t progress = ((hdaddr + (1 << HD_L2_INDEX_BITS)) & HD_L2_INDEX_MASK) - hdaddr;

    while( len ) {

        progress = progress < len ? progress : len;

        if( dc->hd_l1_dirty_tbl[HD_L1_INDEX(hdaddr)] == NULL )
            bzero( &dc->mem_dirty_tbl[wa], progress * sizeof(CONTAMINATION_RECORD) );
        else
            memcpy( 
                &dc->mem_dirty_tbl[wa],
                &dc->hd_l1_dirty_tbl[HD_L1_INDEX(hdaddr)][HD_L2_INDEX(hdaddr)],
                progress * sizeof(CONTAMINATION_RECORD) );

        len    -= progress;
        wa     += progress;
        hdaddr += progress;

        progress = ((hdaddr + (1 << HD_L2_INDEX_BITS)) & HD_L2_INDEX_MASK) - hdaddr;
    }
}

/// DIFT Private API - Taint code interpretation
#define DEQ_FROM_CODE() (*(ptr_code++))
#define DEQ_FROM_ADDR() (*(ptr_code++))

#define THREADED_DISPATCH() \
{ \
   *(uint64_t*)&rec = DEQ_FROM_CODE(); \
    goto *dispatch[rec.case_nb]; \
}

static void interpret_dift_codes_rest( dift_context* dc, uint64_t* ptr_code ) {
#include "dift-policy-head.h"
    THREADED_DISPATCH();
#include "dift-policy.h"

rt_rec_end:
    return;
}

#undef DEQ_FROM_CODE
#undef DEQ_FROM_ADDR
#undef THREADED_DISPATCH

#define DEQ_FROM_CODE() (*(ptr_code++))
#define DEQ_FROM_ADDR() (*(ptr_addr++))

#define THREADED_DISPATCH() \
{ \
   *(uint64_t*)&rec = DEQ_FROM_CODE(); \
    goto *dispatch[rec.case_nb]; \
}

static void interpret_dift_codes( dift_context* dc, uint64_t* ptr_addr ) {
#include "dift-policy-head.h"
    uint64_t* ptr_code;
    uint64_t* ptr_code_end;

    uint64_t end_backup;
    uint64_t loc_cntr_off;
    uint64_t loc, cntr, off;
	
    loc_cntr_off = rec_dequeue( dc );

    loc  = (loc_cntr_off & 0xffffffff00000000) >> 32;
    cntr = (loc_cntr_off & 0x00000000ffff0000) >> 16;
	off  =  loc_cntr_off & 0x000000000000ffff;

	if( cntr < off )
		off = 0;
		
    ptr_code = dift_code_buffer + ((loc * CONFIG_IF_CODES_PER_TB) + off);
    ptr_code_end = ptr_code + (cntr - off);
    end_backup = *ptr_code_end;
    *ptr_code_end = REC_END;
    THREADED_DISPATCH();
#include "dift-policy.h"

rt_rec_end:
    *ptr_code_end = end_backup;
    interpret_dift_codes_rest( dc, ptr_addr );
}

static void* analysis_mainloop( void* arg ) {

    sigset_t signal_mask;
    uint64_t data;
    uint64_t  q_addrs[4096];
    uint64_t* q_addrs_top = q_addrs;

    sigfillset( &signal_mask );
    sigprocmask( SIG_BLOCK, &signal_mask, NULL );

    int cntr = 0;
    int dist;
    int idleness = 0;
    while(1) {
        if( (cntr++ & 0xfffff) == 0 ) {
            dist = head - dc->tail;
            if(dist < 0)
                dist += Q_CHUNKS_SIZE;

            sleepness = 0;
            if( dist < 100 )
                idleness++;
            else
                idleness = 0;
            if( idleness > 12 )
                sleepness = 1;
        }

        data = rec_dequeue( dc );
        if( unlikely(data == (REC_END_SYMBOL | REC_SYNC)) ) {
            // A SYNC indicates that:
            // 1. It is preceded by a BEFORE_BLOCK_BEGIN, so all IF-codes had been processed.
            // 2. The emulation thread is NOT in the execution of any code blocks.
            // 3. dift_thread_ok_signal has been set to zero
            // 3. Emuation thread is spinning on dift_thread_ok_signal (waiting for 1)

            dift_thread_ok_signal = 1;
            dc->deqptr = NULL;
        } else if( unlikely(data == (REC_END_SYMBOL | REC_BEFORE_BLOCK_BEGIN)) ) {
            *(q_addrs_top++) = REC_END;
            interpret_dift_codes( dc, q_addrs );
            q_addrs_top = q_addrs;
        } else {
            *( q_addrs_top++ ) = data;
        }
    }

    return NULL;
}

/// DIFT Public API - All of the public APIs are named with the prefix "dift_"
void dift_log( const char* fmt, ... ) {
	
	va_list ap;
	
	va_start( ap, fmt );
	if( dift_logfile ) {
		vfprintf( dift_logfile, fmt, ap );
	}
	va_end( ap );
}

void dift_rec_enqueue( uint64_t data_in ) {

    *(enqptr++) = data_in;

    if( ((uint64_t)enqptr & ~CHUNK_BITSIZEMASK) == 0 ) {

        // end of chunk
        asm volatile("mfence":::"memory");
        q_chunks_flag[prev_head] = CHUNK_FILLED;

        // ask for next available chunk
        while( q_chunks_flag[head] != CHUNK_AVAILABLE )
            asm volatile("pause");

        q_chunks_flag[head] = CHUNK_FILLING;
        enqptr = (uint64_t*)q_chunks_ptr[head];

        prev_head = head;
        head = (head + 1) % Q_CHUNKS_SIZE;
    }
}

uint8_t dift_rec_case_nb( uint8_t dst_type, uint8_t src_type, uint8_t ot, uint8_t effect ) {

    uint8_t ret = case_mapping[((0xc0 | src_type << 4 | dst_type << 2 | ot) << 8) | effect];
    if( ret == 0xff ) { 
        fprintf( stderr, "No matching DIFT record case: \n" );
        fprintf( stderr, "dst_type : %d, src_type : %d, ot : %d, effect : %d\n", dst_type, src_type, ot, effect );
        exit( 1 );
    }
    return ret;
}

void dift_sync( void ) {

	uint64_t rec = 0;

    if( dift_code_loc != 0 ) {

		rec |=  dift_code_loc;
		rec <<= 0x10;

		rec |=  dift_code_cntr;
		rec <<= 0x10;

        dift_rec_enqueue( REC_END_SYMBOL | REC_BEFORE_BLOCK_BEGIN );
        dift_rec_enqueue( rec );
    }

    dift_rec_enqueue( REC_END_SYMBOL | REC_SYNC );
    kick_enqptr();
    wait_dift_analysis();

    dift_code_loc = 0;
    dift_code_cntr = 0;
    
    dift_thread_ok_signal = 0;
}

int dift_start( void ) {
    
    int err;
	
	dift_logfile = fopen( DIFT_LOG, "w+" );
	if( dift_logfile == NULL ) {
		fprintf( stderr, "Fail to create DIFT log file\n" );
		return -1;
	}

    pre_generate_routine();

    init_queue();
    init_case_mapping();
    init_dift_context(dc);

    if( (err = pthread_create(&dift_thread, NULL, analysis_mainloop, NULL)) ) {
		fprintf( stderr, "Fail to startup DIFT analysis thread\n" );
        return -1;
	}
    return 0;
}

void dift_contaminate_memory_or( uint64_t addr, uint64_t len, CONTAMINATION_RECORD tag ) {

	uint64_t max_fragment = 0xffffffff;
	uint64_t fragment     = len;

	while( len > 0 ) {

		
	}
}

void dift_contaminate_memory_and( uint64_t addr, uint64_t len, CONTAMINATION_RECORD tag ) {
}

void dift_contaminate_disk_or( uint64_t haddr, uint64_t len, CONTAMINATION_RECORD tag ) {
}

void dift_contaminate_disk_and( uint64_t haddr, uint64_t len, CONTAMINATION_RECORD tag ) {
}
