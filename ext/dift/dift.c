 /***********************************************************
  * Dynamic Information Flow Tracking(DIFT) Implementation
  * Originated from Chiwei Wang, 2009/02/16
  *
  * Upgraded by Chiawei Wang, 2015/04/23
  ***********************************************************/

#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/mman.h>
#include <assert.h>
#include <stdint.h>


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
uint64_t phys_ram_base;
uint64_t phys_ram_size;

/* Emulator part */
uint8_t pre_generated_routine[4096] __attribute__((aligned(4096)));

uint8_t* rt_get_next_enqptr		= pre_generated_routine;
uint8_t* rt_finish_curr_block	= pre_generated_routine + 512;
uint8_t* rt_enqueue_one_rec		= pre_generated_routine + 1024;
uint8_t* rt_enqueue_raddr		= pre_generated_routine + 1536;
uint8_t* rt_enqueue_waddr		= pre_generated_routine + 2048;
uint8_t* rt_qemu_ld				= pre_generated_routine + 2560;
uint8_t* rt_qemu_st				= pre_generated_routine + 3072;

volatile uint64_t* enqptr __attribute__((aligned(4096)));
volatile uint64_t  head, prev_head;
uint64_t last_mem_read_addr;
uint64_t last_mem_write_addr;
volatile uint64_t clean_source;
volatile uint64_t null_sink;
volatile int nic_rx_taint;
volatile int nic_tx_taintcheck;
uint8_t case_mapping[1 << (sizeof(uint16_t) * 8)];

uint16_t case_list[] ={

    // encoding:
    // ((0xc0 | SRC_TYPE | DST_TYPE | OP_SIZE) << 8) | EFFECT)
    // The sequence shoud match the ENUM in dift.h
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

int dift_code_top  = 1;  // Loc 0 is reserved for the workaround in add_file_taint
int dift_code_cntr = 0;
int dift_code_loc;
int label_or_helper_appeared;

uint32_t dift_code_buffer[CONFIG_MAX_TB_ESTI * CONFIG_IF_CODES_PER_TB] __attribute__((aligned(4096)));    // The size of the allocation should be fixed

/// DIFT part 
dift_context dc[1] __attribute__((aligned(4096)));
static pthread_t dift_thread;

/// DIFT Communication
uint64_t          q_records[CONFIG_SIZE_OF_QUEUE / sizeof(uint64_t)] __attribute__((aligned(4096)));
uint64_t*         q_chunks_ptr[Q_CHUNKS_SIZE];
volatile uint64_t q_chunks_flag[Q_CHUNKS_SIZE];
volatile uint32_t dift_thread_ok_signal = 0;

int sleepness = 0;

/// Logging
FILE* fp_dift_log;

const char* REG_NAME[] = {
    "RAX", "RCX", "RDX", "RBX", "RSP", "RBP", "RSI", "RDI",
    "R8",  "R9",  "R10", "R11", "R12", "R13", "R14", "R15",
    "RIP", "ES",  "CS",  "SS",  "DS",  "FS",  "GS",  "TMP"
};

/// DIFT Private API
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

/// Pre-generate routine for TCG usage
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
    out8(s, 0xe0);
    out8(s, 0x03);      // shl rax, 0x3

    out8(s, 0x48);
    out8(s, 0xba);
    out64(s, q_chunks_flag);   // mov rdx, q_chunks_flag

    out8(s, 0x48);
    out8(s, 0x01);
    out8(s, 0xc2);      // add rdx, rax

    out8(s, 0x7c);
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
    out8(s, 0xb8);
    out64(s, phys_ram_base);    // mov rax, phys_ram_base

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
    out8(s, 0xb8);
    out64(s, phys_ram_base);    // mov rax, phys_ram_base

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
        q_chunks_ptr[i]  = &q_records[ (i * SIZE_OF_CHUNK)/sizeof(uint64_t) ];
    }
    
    // ask for next abailable chunk;
    q_chunks_flag[0] = CHUNK_FILLING;

    enqptr = (uint64_t*)q_chunks_ptr[0];
    head   = 1;
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
	
	dc->tail      = 0;
    dc->prev_tail = Q_CHUNKS_SIZE - 1;

    dc->deqptr = NULL;

    // allocation for memory taint
    dc->mem_dirty_tbl = (CONTAMINATION_RECORD*)calloc( phys_ram_size * sizeof(CONTAMINATION_RECORD) /*+ ((uint64_t)1 << PAGENO_BITS)*/, 1 );
    if( dc->mem_dirty_tbl == NULL ) {
        fprintf( stderr, "Not enough memory for mem_dirty_tbl, need %016lx bytes\n", phys_ram_size * sizeof(CONTAMINATION_RECORD) + ((uint64_t)1 << PAGENO_BITS) );
        exit( 1 );
    }

    clean_source = 0xFFFF000000000000;
    null_sink    = 0x0000FFFFFFFFFFFF;

    // allocation for harddisk taint
    dc->hd_l1_dirty_tbl = (CONTAMINATION_RECORD**)calloc( 1 << HD_L1_INDEX_BITS, 1 );
    if( dc->hd_l1_dirty_tbl == NULL ) {
        fprintf(stderr, "Not enough memory for hd_l1_dirty_tbl\n");
        exit(1);
    }

#if defined(CONFIG_TAINT_DIRTY_INS_OUTPUT)
    dc->force_mem_dirty = 0;
#endif
}

static void* dift_analysis_mainloop( void* arg ) {
    // do nothing
    return NULL;
}

/// DIFT PUBLIC API: All of APIs are named with the prefix "dift_"
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

void dift_sync( void ) {
    if( dift_code_loc != 0 ) {
        dift_rec_enqueue( REC_END_SYMBOL | REC_BEFORE_BLOCK_BEGIN );
        dift_rec_enqueue( dift_code_loc | dift_code_cntr );
    }

    dift_rec_enqueue( REC_END_SYMBOL | REC_SYNC );
    kick_enqptr();
    wait_dift_analysis();

    dift_code_loc  = 0;
    dift_code_cntr = 0;
    
    dift_thread_ok_signal = 0;
}

int dift_start() {
    
    int err;

    pre_generate_routine();

    init_queue();
    init_case_mapping();
    init_dift_context(dc);

    if( (err = pthread_create(&dift_thread, NULL, dift_analysis_mainloop, NULL)) )
        return -1;
    return 0;
}
