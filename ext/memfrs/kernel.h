

#ifndef __MEMFRS_KERNEL_H__
#define __MEMFRS_KERNEL_H__

#include "qom/cpu.h"


extern uint64_t memfrs_find_nt_kernel_base(CPUState* cpu);
extern uint64_t memfrs_get_nt_kernel_base(void);

#endif
