

#ifndef __MEMFRS_KERNEL_H__
#define __MEMFRS_KERNEL_H__

#if !defined(CONFIG_MEMFRS_TEST)
#include "qom/cpu.h"
#endif
//Leave for private APIs

typedef struct win_kernel_module{
    char name[256];
    uint64_t base;
    char guid[80];
} win_kernel_module;



#endif
