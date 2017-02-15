/*
 *  MBA Network scanning Introspection Implementation
 *
 *  Copyright (c)   2016 ELin Ho
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

#include "qemu-common.h"
#include "monitor/monitor.h"
#include "include/exec/cpu-common.h"
#include "exec/cpu-all.h"
#include "include/utarray.h"
#include "include/uthash.h"
#include "json-c/json.h"

#include "memfrs.h"
#include "memfrs-priv.h"
#include<stdlib.h>
#include<time.h>

#include "netscan.h"

#define SIZE_OF_POOL_HEADER 0x10
#define AF_INET 2
#define AF_INET6 23



/*********************************************************************************
static const char* map_state_to_str(uint32_t state)

mapping TCP state from state number to state string

INPUT:  uint32_t state          state number
OUTPUT: char*                   pointer to state string
**********************************************************************************/
static const char* map_state_to_str(uint32_t state)
{
    switch(state){
        case 0:  return "CLOSED"; break;
        case 1:  return "LISTENING"; break;
        case 2:  return "SYN_SENT"; break;
        case 3:  return "SYN_RCVD"; break;
        case 4:  return "ESTABLISHED"; break;
        case 5:  return "FIN_WAIT1";  break;
        case 6:  return "FIN_WAIT2";  break;
        case 7:  return "CLOSE_WAIT"; break;
        case 8:  return "CLOSING";  break;
        case 9:  return "LAST_ACK";  break;
        case 12: return "TIME_WAIT";  break;
        case 13: return "DELETE_TCP";  break;
        default:
                 return "UNKOWN"; break;
    }
    return "UNKOWN";
}



/*********************************************************************************
static void add_network_feild_to_structure(
        uint64_t offset, 
        const char* protocol, 
        uint64_t pmem, 
        uint64_t eprocess, 
        char* file_name, 
        uint64_t pid, 
        const char* state, 
        char* local_addr, 
        char* foreign_addr, 
        uint64_t time, 
        UT_array *network_list)

add a network data in UT_array structure network_state

INPUT:  uint64_t offset         offset
        const char* protocol    network protocol, including UDP/TCP and IPv4/IPv6
        uint64_t pmem           physical memory address of the pool tag address
        uint64_t eprocess       eprocess address in virtual memoey
        char* file_name         the file name of owner process
        uint64_t pid            the process id of owner process
        const char* state       TCP connect state
        char* local_addr        local address
        char* foreign_addr      foreign address
        char* time              windows timestamp
        UT_array *network_list  target network_state structure
OUTPUT: void                    nothing to output
**********************************************************************************/
static void add_network_feild_to_structure(
        uint64_t offset, 
        const char* protocol, 
        uint64_t pmem, 
        uint64_t eprocess, 
        char* file_name, 
        uint64_t pid, 
        const char* state, 
        char* local_addr, 
        char* foreign_addr, 
        char* time, 
        UT_array *network_list)
{
    network_state network_data;

    // offset
    network_data.offset = offset;

    // protocol
    network_data.protocol = protocol;

    // pgysical memory
    network_data.pmem = pmem;

    // eprocess
    network_data.eprocess = eprocess;

    // file name
    network_data.file_name = file_name;

    // process id
    network_data.pid = pid;

    // state of Tcp Endpoint
    network_data.state = state ? state : calloc(1, sizeof(char));

    // local address
    network_data.local_addr = local_addr;

    // foreign address
    network_data.foreign_addr = foreign_addr ;

    // createtime
    network_data.time = time;

    utarray_push_back(network_list, &network_data);
}



/*********************************************************************************
static char* port_convert(uint64_t time)

Convert windows time to timestamp

INPUT:  uint64_t port           windows time
OUTPUT: char*                   pointer to char array, stored timestamp
**********************************************************************************/
static char* windows_timestamp_convert(uint64_t time)
{
    float second=0;
    time_t windows_time=0;

    // convert time to seconds
    second = time/10000000.0;

    // convert NTP time to Windows time
    /*
        ntp time begin from 1601 but windows time begin from 1970
        11644473600 is number of seconds from 1601 to 1970
    */
    windows_time = second-11644473600;

    // convert seconds to timestamp
    return ctime(&windows_time);
}



/*********************************************************************************
static char* port_convert(uint16_t port)

Convert port from Big Endian to Little Endian.

INPUT:  uint16_t port           port in Big Endian
OUTPUT: char*                   pointer to char array, stored port in Little Endian
**********************************************************************************/
static char* port_convert(uint16_t port)
{
    char* s_port;
    s_port = (char*)malloc(6);
    memset(s_port, 0, 6);

    snprintf(s_port, 6, "%d", (port%0x100)*256+(port>>8));

    return s_port;
}



/*********************************************************************************
static char* IPv6_to_str(uint64_t addr_ptr, uint16_t port, CPUState *cpu)

Parse IPv6 data from virtual address

INPUT:  uint64_t addr_ptr       an virtual memory address of IPv4 address 
        uint16_t port           port in Big Endian type
        CPUState *cpu           pointer to current cpu
OUTPUT: char*                   pointer to char array, stored IPv4 string
**********************************************************************************/
static char* IPv6_to_str(uint64_t addr_ptr, uint16_t port, CPUState *cpu)
{
    char* addr;
    uint64_t j;
    uint8_t addr_v6;
    addr = (char*)malloc(48);
    memset(addr, 0, 48);

    for(j=0;j<0x10;j=j+0x1){
        if(cpu_memory_rw_debug(cpu, addr_ptr+j, (uint8_t*)&addr_v6, 2, 0)!=0)
            return NULL;
        if(j%2==0)
            sprintf(addr, "%s%"PRIx16, addr, addr_v6);
        else
            sprintf(addr, "%s%"PRIx16":", addr, addr_v6);
    }

    sprintf(addr, "%s%s", addr, port_convert(port));

    return addr;
}



/*********************************************************************************
static char* IPv4_to_str(uint64_t addr_ptr, uint16_t port, CPUState *cpu)

Parse IPv4 data from virtual address

INPUT:  uint64_t addr_ptr       an virtual memory address of IPv4 address 
        uint16_t port           port in Big Endian type
        CPUState *cpu           pointer to current cpu
OUTPUT: char*                   pointer to char array, stored IPv4 string
**********************************************************************************/
static char* IPv4_to_str(uint64_t addr_ptr, uint16_t port, CPUState *cpu)
{
    char* addr;
    int i;
    uint32_t addr_v4;
    addr = (char*)malloc(22);
    memset(addr, 0, 22);

    if(cpu_memory_rw_debug(cpu, addr_ptr, (uint8_t*)&addr_v4, 4, 0)!=0)
        return NULL;
    for(i=0;i<4;i++){
        sprintf(addr, "%s%d.", addr, addr_v4%0x100);
        addr_v4=addr_v4>>8;
    }

    addr[strlen(addr)-1]=':';
    sprintf(addr, "%s%s", addr, port_convert(port));

    return addr;
}



/*********************************************************************************
static void parse_TcpL(int offset_tag, uint64_t pmem, UT_array *network_list, CPUState *cpu)

Parse the whole Tcp Listening datas if found pool tag "TcpL" in physical memory

INPUT:  int offset_tag          offset from pool header to pool tag
        uint64_t pmem           physical address
        UT_array *network_list  pointer to network list, its a UT_array of structure "networt_state"
        CPUState *cpu           pointer to current cpu
OUTPUT: void                    nothing to return
**********************************************************************************/
static void parse_TcpL(int offset_tag, uint64_t pmem, UT_array *network_list, CPUState *cpu)
{
    uint64_t addr1, addr2;

    uint8_t AF;
    uint16_t port_local;
    uint64_t eprocess_ptr, processid, time;
    char *addr=NULL, *addr_foreign=NULL;
    uint8_t file_name_buf[16];
    char *file_name;

    // InetAF
    cpu_physical_memory_read( pmem-offset_tag+SIZE_OF_POOL_HEADER + 0x28 , &addr1, 8);
    if(cpu_memory_rw_debug(cpu, addr1+0x18, (uint8_t*)&AF, 1, 0)!=0)
        return;
    // Owner
    cpu_physical_memory_read( pmem-offset_tag+SIZE_OF_POOL_HEADER + 0x30 , &eprocess_ptr, 8);
    if(memfrs_get_virmem_struct_content( cpu, 0, (uint8_t*)&processid, sizeof(processid), eprocess_ptr, "_EPROCESS", 1, "#UniqueProcessId")!=0)
        return;
    if(memfrs_get_virmem_struct_content( cpu, 0, (uint8_t*)file_name_buf, sizeof(file_name_buf), eprocess_ptr, "_EPROCESS", 1, "#ImageFileName")!=0)
        return;
    file_name = (char*)malloc(16);
    snprintf(file_name, 16, "%s", file_name_buf);
    // CreateTime
    cpu_physical_memory_read( pmem-offset_tag+SIZE_OF_POOL_HEADER + 0x40 , &time, 8);
    // Local port
    cpu_physical_memory_read( pmem-offset_tag+SIZE_OF_POOL_HEADER + 0x72 , &port_local, 2);


    // is_valid
    if(eprocess_ptr<0xffff000000000000 || processid==0 || processid>65535 || port_local ==0x0000)
        return;


    // Address
    if(AF==AF_INET){
        cpu_physical_memory_read( pmem-offset_tag+SIZE_OF_POOL_HEADER + 0x60 , &addr1, 8);
        if(addr1==0x0000000000000000){
            addr = (char*)malloc(14);
            sprintf(addr, "0.0.0.0:%s", port_convert(port_local));
        }
        else if(cpu_memory_rw_debug(cpu, addr1+0x10, (uint8_t*)&addr2, 8, 0)!=0){
            return;
        }
        else{
            addr = IPv4_to_str(addr2+0x28, port_local, cpu);
            if(addr==NULL)
                return;
        }
        addr_foreign = (char*)malloc(10);
        memset(addr_foreign, 0, 10);
        sprintf(addr_foreign, "0.0.0.0:0");
        add_network_feild_to_structure(0, "TCPv4", pmem, eprocess_ptr, file_name, processid, "LISTENING", addr, addr_foreign, windows_timestamp_convert(time), network_list);
    }
    else if(AF==AF_INET6){
        cpu_physical_memory_read( pmem-offset_tag+SIZE_OF_POOL_HEADER + 0x60 , &addr1, 8);
        if(addr1==0x0000000000000000){
            addr = (char*)malloc(18);
            sprintf(addr, "0.0.0.0.0.0.0.0:0");
        }
        else if(cpu_memory_rw_debug(cpu, addr1+0x10, (uint8_t*)&addr2, 8, 0)!=0){
            return;
        }
        else{
            addr = IPv6_to_str(addr2+0x28, port_local, cpu);
            if(addr==NULL)
                return;
        }
        addr_foreign = (char*)malloc(10);
        memset(addr_foreign, 0, 10);
        sprintf(addr_foreign, "0.0.0.0:0");
        add_network_feild_to_structure(0, "TCPv6", pmem, eprocess_ptr, file_name, processid, "LISTENING", addr, addr_foreign, windows_timestamp_convert(time), network_list);
    }
    else{
        return;
    }
}



/*********************************************************************************
static void parse_TcpE(int offset_tag, uint64_t pmem, UT_array *network_list, CPUState *cpu)

Parse the whole Tcp Endpoint datas if found pool tag "TcpE" in physical memory

INPUT:  int offset_tag          offset from pool header to pool tag
        uint64_t pmem           physical address
        UT_array *network_list  pointer to network list, its a UT_array of structure "networt_state"
        CPUState *cpu           pointer to current cpu
OUTPUT: void                    nothing to return
**********************************************************************************/
static void parse_TcpE(int offset_tag, uint64_t pmem, UT_array *network_list, CPUState *cpu)
{
    uint64_t addr1, addr2, addr3;
    uint8_t AF;
    uint32_t state=0;
    uint16_t port_local, port_foreign;
    uint64_t eprocess_ptr, processid, time;
    char *addr=NULL, *addr_foreign=NULL;
    const char* state_str;
    uint8_t file_name_buf[16];
    char *file_name;

    // InetAF
    cpu_physical_memory_read( pmem-offset_tag+SIZE_OF_POOL_HEADER + 0x10 , &addr1, 8);
    if(cpu_memory_rw_debug(cpu, addr1+0x18, (uint8_t*)&AF, 1, 0)!=0)
        return;
    // Owner
    cpu_physical_memory_read( pmem-offset_tag+SIZE_OF_POOL_HEADER + 0x258 , &eprocess_ptr, 8);
    if(memfrs_get_virmem_struct_content( cpu, 0, (uint8_t*)&processid, sizeof(processid), eprocess_ptr, "_EPROCESS", 1, "#UniqueProcessId")!=0)
        return;
    if(memfrs_get_virmem_struct_content( cpu, 0, (uint8_t*)file_name_buf, sizeof(file_name_buf), eprocess_ptr, "_EPROCESS", 1, "#ImageFileName")!=0)
        return;
    file_name = (char*)malloc(16);
    snprintf(file_name, 16, "%s", file_name_buf);
    // CreateTime
    cpu_physical_memory_read( pmem-offset_tag+SIZE_OF_POOL_HEADER + 0x268 , &time, 8);
    // Local port
    cpu_physical_memory_read( pmem-offset_tag+SIZE_OF_POOL_HEADER + 0x70 , &port_local, 2);
    // Local foreign
    cpu_physical_memory_read( pmem-offset_tag+SIZE_OF_POOL_HEADER + 0x74 , &port_foreign, 2);
    // state
    cpu_physical_memory_read( pmem-offset_tag+SIZE_OF_POOL_HEADER + 0x6c , &state, 4);


    // isvalid
    if(eprocess_ptr<=0xffff000000000000 || processid==0 || processid>65535 || port_local ==0x0000 || port_foreign==0x0000)
        return;


    // Address
    if(AF==AF_INET){
        state_str = map_state_to_str(state);

        // Local address
        cpu_physical_memory_read( pmem-offset_tag+SIZE_OF_POOL_HEADER + 0x18 , &addr1, 8);
        if(cpu_memory_rw_debug(cpu, addr1, (uint8_t*)&addr2, 8, 0)!=0)
            return;
        if(cpu_memory_rw_debug(cpu, addr2+0x10, (uint8_t*)&addr3, 8, 0)!=0)
            return;
        addr = IPv4_to_str(addr3+0x28, port_local, cpu);
        if(addr==NULL)
            return;

        // Foreign address
        cpu_physical_memory_read( pmem-offset_tag+SIZE_OF_POOL_HEADER + 0x18 , &addr1, 8);
        if(cpu_memory_rw_debug(cpu, addr1+0x10, (uint8_t*)&addr2, 8, 0)!=0)
            return;
        addr_foreign = IPv4_to_str(addr2, port_foreign, cpu);
        if(addr_foreign==NULL)
            return;
        add_network_feild_to_structure(0, "TCPv4", pmem, eprocess_ptr, file_name, processid, state_str, addr, addr_foreign, windows_timestamp_convert(time), network_list);
    }
    else if(AF==AF_INET6){
        state_str = map_state_to_str(state);

        // Local address
        cpu_physical_memory_read( pmem-offset_tag+SIZE_OF_POOL_HEADER + 0x18 , &addr1, 8);
        if(cpu_memory_rw_debug(cpu, addr1, (uint8_t*)&addr2, 8, 0)!=0)
            return;
        if(cpu_memory_rw_debug(cpu, addr2+0x10, (uint8_t*)&addr3, 8, 0)!=0)
            return;
        addr = IPv6_to_str(addr3+0x28, port_local, cpu);
        if(addr==NULL)
            return;

        // Foreign address
        cpu_physical_memory_read( pmem-offset_tag+SIZE_OF_POOL_HEADER + 0x18 , &addr1, 8);
        if(cpu_memory_rw_debug(cpu, addr1+0x10, (uint8_t*)&addr2, 8, 0)!=0)
            return;
        addr_foreign = IPv6_to_str(addr2, port_foreign, cpu);
        if(addr_foreign==NULL)
            return;
        add_network_feild_to_structure(0, "TCPv6", pmem, eprocess_ptr, file_name, processid, state_str, addr, addr_foreign, windows_timestamp_convert(time), network_list);
    }
    else{
        return;
    }
}



/*********************************************************************************
static void parse_UdpA(int offset_tag, uint64_t pmem, UT_array *network_list, CPUState *cpu)

Parse the whole Udp datas if found pool tag "UdpA" in physical memory

INPUT:  int offset_tag          offset from pool header to pool tag
        uint64_t pmem           physical address
        UT_array *network_list  pointer to network list, its a UT_array of structure "networt_state"
        CPUState *cpu           pointer to current cpu
OUTPUT: void                    nothing to return
**********************************************************************************/
static void parse_UdpA(int offset_tag, uint64_t pmem, UT_array *network_list, CPUState *cpu)
{
    uint64_t addr1, addr2;

    uint8_t AF;
    uint16_t port_local;
    uint64_t eprocess_ptr, processid, time;
    char* addr=NULL, *addr_foreign=NULL;
    uint8_t file_name_buf[16];
    char *file_name;

    // InetAF
    cpu_physical_memory_read( pmem-offset_tag+SIZE_OF_POOL_HEADER + 0x20 , &addr1, 8);
    if(cpu_memory_rw_debug(cpu, addr1+0x18, (uint8_t*)&AF, 1, 0)!=0)
        return;
    // Owner
    cpu_physical_memory_read( pmem-offset_tag+SIZE_OF_POOL_HEADER + 0x28 , &eprocess_ptr, 8);
    if(memfrs_get_virmem_struct_content( cpu, 0, (uint8_t*)&processid, sizeof(processid), eprocess_ptr, "_EPROCESS", 1, "#UniqueProcessId")!=0)
        return;
    if(memfrs_get_virmem_struct_content( cpu, 0, (uint8_t*)file_name_buf, sizeof(file_name_buf), eprocess_ptr, "_EPROCESS", 1, "#ImageFileName")!=0)
        return;
    file_name = (char*)malloc(16);
    snprintf(file_name, 16, "%s", file_name_buf);
    // CreateTime
    cpu_physical_memory_read( pmem-offset_tag+SIZE_OF_POOL_HEADER + 0x58 , &time, 8);
    // Local port
    cpu_physical_memory_read( pmem-offset_tag+SIZE_OF_POOL_HEADER + 0x78 , &port_local, 2);


    // is_valid
    if(eprocess_ptr<0xffff000000000000 || processid==0 || processid>65535 || port_local ==0x0000)
        return;


    // Address
    if(AF==AF_INET){
        cpu_physical_memory_read( pmem-offset_tag+SIZE_OF_POOL_HEADER + 0x80 , &addr1, 8);
        if(addr1==0x0000000000000000){
            addr = (char*)malloc(14);
            sprintf(addr, "0.0.0.0:%s", port_convert(port_local));
        }
        else if(cpu_memory_rw_debug(cpu, addr1, (uint8_t*)&addr2, 8, 0)!=0){
            return;
        }
        else{
            addr = IPv4_to_str(addr2, port_local, cpu);
            if(addr==NULL)
                return;
        }
        addr_foreign = (char*)malloc(4);
        memset(addr_foreign, 0, 4);
        sprintf(addr_foreign, "*:*");
        add_network_feild_to_structure(0, "UDPv4", pmem, eprocess_ptr, file_name, processid, NULL, addr, addr_foreign, windows_timestamp_convert(time), network_list);
    }
    else if(AF==AF_INET6){
        cpu_physical_memory_read( pmem-offset_tag+SIZE_OF_POOL_HEADER + 0x80 , &addr1, 8);
        if(addr1==0x0000000000000000){
            addr = (char*)malloc(18);
            sprintf(addr, "0.0.0.0.0.0.0.0:0");
        }
        else if(cpu_memory_rw_debug(cpu, addr1, (uint8_t*)&addr2, 8, 0)!=0){
            return;
        }
        else{
            addr = IPv6_to_str(addr2, port_local, cpu);
            if(addr==NULL)
                return;
        }
        addr_foreign = (char*)malloc(4);
        memset(addr_foreign, 0, 4);
        sprintf(addr_foreign, "*:*");
        add_network_feild_to_structure(0, "UDPv6", pmem, eprocess_ptr, file_name, processid, NULL, addr, addr_foreign, windows_timestamp_convert(time), network_list);
    }
    else{
        return;
    }
}



static void network_dtor(void *_elt) {
    network_state *elt = (network_state*)_elt;

    if(elt->local_addr) free(elt->local_addr);
    if(elt->foreign_addr) free(elt->foreign_addr);
}

UT_icd network_icd = {sizeof(network_state), NULL, NULL, network_dtor };
/*********************************************************************************
UT_array memfrs_scan_network(CPUState *cpu)

Scan the whole physical memory for network pool tag, and list all the network state.

INPUT:  CPUState *cpu            pointer to current cpu
OUTPUT: UT_array*                return a UT_array with handles types
**********************************************************************************/
extern UT_array* memfrs_scan_network(CPUState *cpu)
{
    UT_array *network_list=NULL;

    uint64_t pmem;
    int offset_tag;
    uint8_t* pool_tag;
    printf("Scan for network patterns.\n" );


    // Size of pool tag "UdpA", "TcpL", "TcpE" is the same
    pool_tag = (uint8_t*)malloc(strlen(POOL_TAG_UDP_ENDPOINT));
    //pooltag = (uint8_t*)malloc(strlen(POOL_TAG_TCP_ENDPOINT));
    //pooltag = (uint8_t*)malloc(strlen(POOL_TAG_TCP_LISTENER));

    // Check if ds metadata is already loaded
    if( memfrs_check_struct_info() == 0){
        printf("Data structure information is not loaded\n");
        return NULL;
    }
    else{
        memfrs_get_nested_field_offset(&offset_tag, "_POOL_HEADER", 1, "PoolTag");
    }


    utarray_new( network_list, &network_icd);
    //Scan whole physical memory
    for(pmem = 0; pmem < MAXMEM-strlen(POOL_TAG_UDP_ENDPOINT); pmem++)
    {
        if(pmem%0x10000000==0x0)
            printf("Scan physical address: 0x%"PRIx64"\n", pmem);

        // Read tag
        cpu_physical_memory_read(pmem, pool_tag, strlen(POOL_TAG_UDP_ENDPOINT));

        // UdpA
        if(memcmp( pool_tag, POOL_TAG_UDP_ENDPOINT, strlen(POOL_TAG_UDP_ENDPOINT))==0)
            parse_UdpA(offset_tag, pmem, network_list, cpu);
        // TCP EndPoint
        else if(memcmp( pool_tag, POOL_TAG_TCP_ENDPOINT, strlen(POOL_TAG_TCP_ENDPOINT))==0)
            parse_TcpE(offset_tag, pmem, network_list, cpu);
        // TCP Listening
        else if(memcmp( pool_tag, POOL_TAG_TCP_LISTENER, strlen(POOL_TAG_TCP_LISTENER))==0)
            parse_TcpL(offset_tag, pmem, network_list, cpu);
        else
            continue;

        fflush(stdout);
    }

    return network_list;
}
