
#include "qemu-common.h"
#include "ext/memfrs/kernel.h"

#define PE_HEADER "MZ"
#define RSDS_HEADER_SIG "RSDS"
#define RSDS_HEADER_SIZE 0x30
#define RSDS_HEADER_IMAGE_NAME_OFF 0x18

#define INIT_LOADER_MAP_LOW 0xFFFFF80000000000 
#define INIT_LOADER_MAP_HIGH 0xFFFFF87FFFFFFFFF

#define SEARCH_MAX 0x1000000

uint64_t nt_kernel_base = 0;

uint64_t find_nt_kernel_base(CPUState* cpu)
{
    
    uint64_t i, j, x;
    bool find = 0;
    uint8_t* buf = (uint8_t*)malloc(RSDS_HEADER_SIZE);
    const char* kernel_list[] = {"ntoskrnl.pdb", "ntkrnlpa.pdb", "ntkrnlmp.pdb", 0};

    for(i = INIT_LOADER_MAP_LOW; i < INIT_LOADER_MAP_HIGH; i+=0x1000)
    {
        cpu_memory_rw_debug(cpu, i, buf, strlen(PE_HEADER), 0);
        if(memcmp(buf, PE_HEADER, strlen(PE_HEADER))==0)
        {
            for(j=i; j< i + SEARCH_MAX; j++)
            {
                cpu_memory_rw_debug(cpu, j, buf, RSDS_HEADER_SIZE, 0);
                // TODO: Not use magic number, use ds query system instead
                if(memcmp(buf, RSDS_HEADER_SIG, strlen(RSDS_HEADER_SIG))==0)
                {
                    printf("\tRSDS Find at %"PRIx64"\n", i+j);
                    printf("\tImage name %s\n", (char*)(buf+RSDS_HEADER_IMAGE_NAME_OFF));
                    break;
                }
            }

            for(x=0; kernel_list[x]; x++)
            {
                if(memcmp(buf+RSDS_HEADER_IMAGE_NAME_OFF, kernel_list[x], strlen(kernel_list[x]))==0)
                {
                    printf("find %s in offset %"PRIx64"\n", (char*)(buf+RSDS_HEADER_IMAGE_NAME_OFF), i);
                    find = 1;
                    nt_kernel_base = i;
                }
            }
            
            if(find)break; 
        }
    }

    free(buf);
    return nt_kernel_base;
}

uint64_t get_nt_kernel_base(void)
{
    return nt_kernel_base;
}

