/*
 *  MBA VAD Introspection Implementation
 *
 *  Copyright (c)   2016 ChongKuan Chen
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
#include "ext/memfrs/memfrs.h"
#include "ext/memfrs/vad.h"

// Mapping between VAD type and it's name
const char *MI_VAD_TYPE_STR[] = {
    "VadNone",
    "VadDevicePhysicalMemory",
    "VadImageMap",
    "VadAwe",
    "VadWriteWatch",
    "VadLargePages",
    "VadRotatePhysical",
    "VadLargePageSection"
};

// Mapping between page permission and it's description
const char *PAGE_PERMISSION_STR[] = {
    "PAGE_NOACCESS",
    "PAGE_READONLY",
    "PAGE_EXECUTE",
    "PAGE_EXECUTE_READ",
    "PAGE_READWRITE",
    "PAGE_WRITECOPY",
    "PAGE_EXECUTE_READWRITE",
    "PAGE_EXECUTE_WRITECOPY",
    "PAGE_NOACCESS",
    "PAGE_NOCACHE | PAGE_READONLY",
    "PAGE_NOCACHE | PAGE_EXECUTE",
    "PAGE_NOCACHE | PAGE_EXECUTE_READ",
    "PAGE_NOCACHE | PAGE_READWRITE",
    "PAGE_NOCACHE | PAGE_WRITECOPY",
    "PAGE_NOCACHE | PAGE_EXECUTE_READWRITE",
    "PAGE_NOCACHE | PAGE_EXECUTE_WRITECOPY",
    "PAGE_NOACCESS",
    "PAGE_GUARD | PAGE_READONLY",
    "PAGE_GUARD | PAGE_EXECUTE",
    "PAGE_GUARD | PAGE_EXECUTE_READ",
    "PAGE_GUARD | PAGE_READWRITE",
    "PAGE_GUARD | PAGE_WRITECOPY",
    "PAGE_GUARD | PAGE_EXECUTE_READWRITE",
    "PAGE_GUARD | PAGE_EXECUTE_WRITECOPY",
    "PAGE_NOACCESS",
    "PAGE_WRITECOMBINE | PAGE_READONLY",
    "PAGE_WRITECOMBINE | PAGE_EXECUTE",
    "PAGE_WRITECOMBINE | PAGE_EXECUTE_READ",
    "PAGE_WRITECOMBINE | PAGE_READWRITE",
    "PAGE_WRITECOMBINE | PAGE_WRITECOPY",
    "PAGE_WRITECOMBINE | PAGE_EXECUTE_READWRITE",
    "PAGE_WRITECOMBINE | PAGE_EXECUTE_WRITECOPY",
};

/*********************************************************************************
int parse_mmvad_node(uint64_t mmvad_ptr, CPUState *cpu)

Parsing the vad node information by the given VAD node(MMVAD) virtual address.
Output is throw to stdout.

INPUT: uint64_t mmvad_ptr       the virtual address to the VAD node
       CPUState *cpu            pointer to current cpu

OUTPUT: int                     return the error state
**********************************************************************************/
int parse_mmvad_node(uint64_t mmvad_ptr, CPUState *cpu)
{
    json_object* jmmvad = NULL;
    json_object* jobj = NULL;
    field_info* f_info = NULL;
    int offset = 0;
    int vad_type, vad_protection;

    uint32_t starting_vpn, ending_vpn;
    uint8_t starting_vpn_high, ending_vpn_high;
    uint64_t start_viraddr, end_viraddr;
    uint64_t file_pointer_ptr;
    uint64_t subsection_ptr, control_area_ptr, file_name_unicode_ptr;
    
    // Hard code Unicode String Size
    // TODO: Add length support in ds query system
    uint32_t u;

    // Qery DS for VAD Virtual Address range
    jmmvad = memfrs_q_struct("_MMVAD_SHORT");

    f_info = memfrs_q_field(jmmvad, "StartingVpn");
    offset = f_info->offset;
    cpu_memory_rw_debug( cpu, mmvad_ptr+offset, (uint8_t*)&starting_vpn, sizeof(starting_vpn), 0 );
    memfrs_close_field(f_info);

    f_info = memfrs_q_field(jmmvad, "EndingVpn");
    offset = f_info->offset;
    cpu_memory_rw_debug( cpu, mmvad_ptr+offset, (uint8_t*)&ending_vpn, sizeof(ending_vpn), 0 );
    memfrs_close_field(f_info);

    f_info = memfrs_q_field(jmmvad, "StartingVpnHigh");
    offset = f_info->offset;
    cpu_memory_rw_debug( cpu, mmvad_ptr+offset, (uint8_t*)&starting_vpn_high, sizeof(starting_vpn_high), 0 );
    memfrs_close_field(f_info);

    f_info = memfrs_q_field(jmmvad, "EndingVpnHigh");
    offset = f_info->offset;
    cpu_memory_rw_debug( cpu, mmvad_ptr+offset, (uint8_t*)&ending_vpn_high, sizeof(ending_vpn_high), 0 );
    memfrs_close_field(f_info);

    start_viraddr = (( (uint64_t)starting_vpn_high << 32 ) + starting_vpn ) << 12;
    end_viraddr = ((( (uint64_t)ending_vpn_high << 32 ) + ending_vpn ) << 12 ) + 0xfff;
    printf("VAD vir range %" PRIx64 " <---------> %" PRIx64 "\n", start_viraddr, end_viraddr);

    // Query for VAD node metadata
    f_info = memfrs_q_field(jmmvad, "u");
    offset = f_info->offset;
    cpu_memory_rw_debug( cpu, mmvad_ptr+offset, (uint8_t*)&u, sizeof(u), 0 );
    memfrs_close_field(f_info);

    vad_type = u & 0b111;
    printf("VAD type: %s(%x)\n", MI_VAD_TYPE_STR[vad_type], vad_type);

    vad_protection =  ((u >> 3) & 0b11111);
    printf("Permission: %s(%x)\n", PAGE_PERMISSION_STR[vad_protection], vad_protection);

    // Check if mode is immage mapping
    if(vad_type != VadImageMap)
        return 0;

    // Quey image filename by following path 
    // _MMVAD->Subsection->ControlArea->FilePointer/_FILE_OBJECT->FileName
    jmmvad = memfrs_q_struct("_MMVAD");
    f_info = memfrs_q_field(jmmvad, "Subsection");
    offset = f_info->offset;
    cpu_memory_rw_debug( cpu, mmvad_ptr+offset, (uint8_t*)&subsection_ptr, sizeof(subsection_ptr), 0 );
    jobj = f_info->jobject_type;
    memfrs_close_field(f_info);

    f_info = memfrs_q_field( jobj, "ControlArea" ); //ControlArea
    offset = f_info->offset;
    cpu_memory_rw_debug( cpu, subsection_ptr+offset, (uint8_t*)&control_area_ptr, sizeof(control_area_ptr), 0 );
    jobj = f_info->jobject_type;
    memfrs_close_field(f_info);

    f_info = memfrs_q_field( jobj, "FilePointer" );
    offset = f_info->offset;
    cpu_memory_rw_debug( cpu, control_area_ptr+offset, (uint8_t*)&file_pointer_ptr, sizeof(file_pointer_ptr), 0 );
    jobj = f_info->jobject_type;
    memfrs_close_field(f_info);

    // _FILE_OBJECT is always aligned, mask out filepointer's lowest byte, which s used to save metadata 
    file_pointer_ptr &= 0xfffffffffffffff0;

    if(file_pointer_ptr==0)
        return 0;

    jobj = memfrs_q_struct("_FILE_OBJECT");
    f_info = memfrs_q_field( jobj, "FileName" );
    offset = f_info->offset;
    file_name_unicode_ptr = file_pointer_ptr+offset;
    jobj = f_info->jobject_type;
    memfrs_close_field(f_info);

    // Parse the unicode string
    parse_unicode_strptr(file_name_unicode_ptr, cpu);
    return 0;
}

// UT_array's eelement structure for vad address
UT_icd vad_adr_icd = {sizeof(uint64_t), NULL, NULL, NULL };

/*********************************************************************************
void traverse_vad_tree(uint64_t eprocess_ptr, CPUState *cpu)

1. Get vad root node by the eprocess_ptr
2. Traversal vad tree, which is AVL tree

INPUT: uint64_t eprocess_ptr    the virtual address to the eprocess structure
       CPUState *cpu            pointer to current cpu

OUTPUT: void
**********************************************************************************/
void traverse_vad_tree(uint64_t eprocess_ptr, CPUState *cpu)
{
    int offset_vadroot_to_eprocess = 0;
    int offset_left_to_vadnode = 0;
    int offset_right_to_vadnode = 0;

    UT_array *vad_node_queue;
    uint64_t vad_root, left, right;
    uint64_t* current_node;

    // Initialize vad_node_queue, use UT_arry as the queue 
    utarray_new(vad_node_queue, &vad_adr_icd);

    json_object* jeprocess = NULL;
    json_object* jvadnode = NULL;
    field_info* f_info = NULL;

    // Check if ds metadata is already loaded
    if( memfrs_check_struct_info() == 0)
    {
        printf("Data structure information is not loaded\n");
        return;
    }

    // Prepare some offset that will be used later
    // Find the vad root offset to the eprocess
    jeprocess = memfrs_q_struct("_EPROCESS");
    f_info = memfrs_q_field(jeprocess, "VadRoot");

    offset_vadroot_to_eprocess = f_info->offset;
    memfrs_close_field(f_info);

    // Find the pointer offset to the Left node
    jvadnode = memfrs_q_struct("_RTL_BALANCED_NODE");
    f_info = memfrs_q_field(jvadnode, "Left");
    offset_left_to_vadnode = f_info->offset;
    memfrs_close_field(f_info);

    // Find the pointer offset to the Right node
    f_info = memfrs_q_field(jvadnode, "Right");
    offset_right_to_vadnode = f_info->offset;
    memfrs_close_field(f_info);

    // Read vad root node from memory 
    cpu_memory_rw_debug( cpu, eprocess_ptr + offset_vadroot_to_eprocess, (uint8_t*)&vad_root, sizeof(vad_root), 0 );
    printf("vad root: %" PRIx64 "\n", vad_root);

    // Put vad root into queue as first element
    utarray_push_back(vad_node_queue, &vad_root);
   
    // Walk through the QUEUE
    while(utarray_len(vad_node_queue) != 0)
    {
        current_node = (uint64_t*)utarray_back(vad_node_queue);
        //printf("Find Node %" PRIx64 "\n", *current_node);

        // Parse the vad node   
        parse_mmvad_node(*current_node, cpu);

        // Read Left node
        cpu_memory_rw_debug( cpu, (*current_node)+offset_left_to_vadnode, (uint8_t*)&left, sizeof(left), 0 );

        // Read Right node
        cpu_memory_rw_debug( cpu, (*current_node)+offset_right_to_vadnode, (uint8_t*)&right, sizeof(right), 0 );

        // Find the next node
        utarray_pop_back(vad_node_queue);

        // Push node into queue if the node found
        if(left != 0)
            utarray_push_back(vad_node_queue, &left);
        if(right != 0)
            utarray_push_back(vad_node_queue, &right);
    }

    return ;
}

