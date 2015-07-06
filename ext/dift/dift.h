 /***********************************************************
  * Dynamic Information Flow Tracking(DIFT) Implementation
  * Originated from Chiwei Wang, 2009/02/16
  *
  * Upgraded by Chiawei Wang, 2015/04/23
  ***********************************************************/

#ifndef __DIFT_H__
#define __DIFT_H__


/* Config */
#define CONFIG_SIZE_OF_QUEUE    (16 * 1024 * 1024)
#define CONFIG_MAX_TB_ESTI      500000
#define CONFIG_IF_CODES_PER_TB  160

#define CONFIG_INDIRECT_TAINT
/* Advanced Feature for JIT & Packer Detection, yet to complete */
//#define CONFIG_JIT_IFCODE
//#define CONFIG_JIT_IFCODE_SIZE_PER_TB 300
//#define CONFIG_TX_CHECK

/* Constant Definition */
#define REC_END_SYMBOL  0x9a0a0a0000000000
#define CHUNK_AVAILABLE 0
#define CHUNK_FILLING   1
#define CHUNK_FILLED    2
#define CHUNK_CONSUMING 3

#define SIZE_OF_CHUNK       (1 << 12)
#define CHUNK_BITSIZEMASK	~(SIZE_OF_CHUNK - 1)
#define Q_CHUNKS_SIZE		(CONFIG_SIZE_OF_QUEUE / SIZE_OF_CHUNK)

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

#define R_RIP      			16
#define R_SEG_REG_BASE      17
#define R_DIFT_TMP          23
#define R_NONE              24

/* Type Definition */
#define CONTAMINATION_RECORD uint8_t

///
/// We use 2-layer page table to record the harddisk taint status
/// Since the initial image is 16GB size, the address need 36 bits to cover the addressing space
/// (same as PAE except that we use only 2-layer page translation)
#define HD_L1_INDEX_BITS    24
#define HD_L2_INDEX_BITS    12
#define HD_L1_SIZE          (1<<HD_L1_INDEX_BITS)
#define HD_L2_SIZE          (1<<HD_L2_INDEX_BITS)
#define HD_L1_INDEX_MASK    (~(((1 << HD_L1_INDEX_BITS) - 1) << HD_L2_INDEX_BITS))
#define HD_L2_INDEX_MASK    (~((1 << HD_L2_INDEX_BITS) - 1))
#define HD_L1_INDEX(addr)   (((addr) & ~HD_L1_INDEX_MASK) >> HD_L2_INDEX_BITS)
#define HD_L2_INDEX(addr)   ((addr) & ~HD_L2_INDEX_MASK)
#define HD_PAGE(addr)       ((hd_l1_dirty_tbl[HD_L1_INDEX(addr)]))


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

enum {

    /* Expression: DST_SRC_MAPPING_TYPE_BYTE */

    /* 0 */
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

    /* 10 */
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

    /* 20 */
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

    /* 30 */ 
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

    /* 40 */
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

    /* 50 */ 
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

    /* 60 */
    REC_CONTAMINATE_HD_OR,
    REC_CONTAMINATE_HD_AND,
    REC_CLEAR_MEM,
    REC_BEFORE_BLOCK_BEGIN,
    REC_BLOCK_BEGIN,

    REC_SYNC,
    REC_END
};

extern uint64_t phys_ram_base;
extern uint64_t phys_ram_size;

extern uint64_t  q_records[];
extern uint64_t* q_chunks_ptr[];
extern volatile uint64_t q_chunks_flag[];
extern volatile uint64_t *enqptr;
extern volatile uint64_t head, prev_head;

extern uint64_t last_mem_read_addr;
extern uint64_t last_mem_write_addr;
extern volatile uint64_t clean_source;
extern volatile uint64_t null_sink;

extern volatile int nic_rx_taint;
extern volatile int nic_tx_taintcheck;

extern volatile uint32_t dift_thread_ok_signal;

extern uint8_t* rt_get_next_enqptr;
extern uint8_t* rt_finish_curr_block;
extern uint8_t* rt_enqueue_one_rec;
extern uint8_t* rt_enqueue_raddr;
extern uint8_t* rt_enqueue_waddr;

extern void dift_enqueue(uint32_t);
extern void dift_sync(void);

///
/// DIFT Logging functions
extern FILE* dift_logfile;
extern void  dift_log( const char*, ... );

CONTAMINATION_RECORD get_valid_taint_mask(void);
int is_valid_taint(const CONTAMINATION_RECORD* taint);

///
/// Get the contamination status of a phsical memory byte.
/// The most significant bit is used for marking whether a code block contains tainted code.
///
extern CONTAMINATION_RECORD cpu_get_mem_dirty(uint64_t offset);
extern CONTAMINATION_RECORD cpu_get_hd_dirty(uint64_t hdaddr);

///
/// Get the taint status of virtual memory
/// For pages not in physical memory, it assumes that the pages are clear.
/// For each continuous tainted memory range of a same taint type in virtual memory,
/// it calls the 'callback' with the starting virtual address, ending virtual address(inclusive),
/// and the taint status.
/// It the callback function returns zero, it stops and return.
/// p.s. The reason why it doesn't use 'len' of 'end', is that if the whole virtual memory space is tainted with the same
/// type, it will cause problem, or the requested virtual memory range is the whole virtual memory space, it will cause
/// problem.
///
/// \param env         CPU state structure of the process
/// \param addr        starting virtual addr
/// \param end         ending virtual addr(inclusive)
/// \param filter      filter out the bits that should be ignored
/// \param callback    the callback function to be called
///
//extern void mem_dirty_iter(CPUState* env, target_ulong addr, target_ulong end, CONTAMINATION_RECORD filter, int(*callback)(target_ulong,target_ulong,CONTAMINATION_RECORD,void*), void*);

// Collect the taint status in a single CONTAMINATION_RECORD
// Usage:
//   CONTAMINATION_RECORD mytaint=0;
//   mem_dirty_iter(env,start,end,taint_collect,mytaint);
extern int collect_taint(uint64_t start, uint64_t end, CONTAMINATION_RECORD taint, void* arg);

extern CONTAMINATION_RECORD hd_dirty_tbl[];
extern uint64_t dift_code_buffer[];
extern uint32_t dift_code_top;
extern uint32_t dift_code_cntr;
extern uint32_t dift_code_loc;
extern uint32_t dift_code_off;
extern int label_or_helper_appeared;

extern int dift_start(void);

extern void    dift_rec_enqueue( uint64_t data_in );
extern uint8_t dift_rec_case_nb(uint8_t, uint8_t, uint8_t, uint8_t);

extern void dift_contaminate_memory_or(uint64_t, uint64_t, CONTAMINATION_RECORD);
extern void dift_contaminate_memory_and(uint64_t, uint64_t, CONTAMINATION_RECORD);
extern void dift_contaminate_hd_or(uint64_t, uint64_t, CONTAMINATION_RECORD);
extern void dift_contaminate_hd_and(uint64_t, uint64_t, CONTAMINATION_RECORD);


///
/// Flush the record queue if needed.
///
/// \param cnt   Number of sets of records that have already been enqueued. Each set of records
///              may be enqueued by several enqueue() call.
///
void record_queue_flush(size_t cnt);
extern void clear_memory(uint64_t, uint64_t);
#if defined(CONFIG_SET_BIT7_ON_DIRTY_INS_OUTPUT)
extern void clear_all_taint_bit7(void);
#endif

struct dift_context {

    uint64_t* deqptr;
    uint32_t  tail, prev_tail;
    CONTAMINATION_RECORD reg_dirty_tbl[24][8];
    CONTAMINATION_RECORD *mem_dirty_tbl;
    CONTAMINATION_RECORD **hd_l1_dirty_tbl;

//#if defined(CONFIG_DIFT_DEBUG)	
	uint64_t tb_rip;
	uint64_t tb_tc_ptr;
//#endif	

#if defined(CONFIG_TAINT_DIRTY_INS_OUTPUT)
	uint64_t tb_phyeip;
    uint32_t force_mem_dirty;
#endif
};

typedef struct dift_context dift_context;
typedef struct dift_record  dift_record;

extern dift_context dc[];

#endif

