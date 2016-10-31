#ifndef CPU_COMMON_H
#define CPU_COMMON_H 1

#include "../../../include/utarray.h"
#include "../../../json-c/json.h"

#define X86_CPU(obj) (obj)

typedef uint64_t hwaddr;

typedef struct CPUState{
} CPUState;

typedef struct Monitor{
} Monitor;

typedef struct CPUX86State {
    ulong cr[5];
} CPUX86State;

typedef struct X86CPU{
    CPUX86State env;
} X86CPU;




void* current_cpu;


#endif
