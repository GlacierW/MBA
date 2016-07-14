/*
 *  MBA Virtual Machine Memory Introspection Self-defined Signature Generator
 *
 *  Copyright (c)  2016 ChongKuan Chen
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
 *
 * Using above command line to compile this file and execute
 * executable file to generate json file "type_definition.json":
 * gcc -I../../json-c/ gen_struct_def.c ../../json-c/.libs/libjson-c.a -o genjson && ./genjson
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include "json.h"

#ifdef TEST_FORMATTED
#define json_object_to_json_string(obj) json_object_to_json_string_ext(obj,sflags)
#endif

/*******************************************************************
* NAME :            int addFieldToStruct( parent_obj, field_name, field_type, offset, size, is_pointer)
* DESCRIPTION :     Add a field into data structure information
* INPUTS :
*           json_object* parent_obj        json object of target data structure
*           char*        field_name        name of field
*           char*        field_type        type of firld
*           int          offset            offset of this field in the structure
*           bool         is_pointer        indicator to show if this field is a pointer
*
* OUTPUTS :
*/
void add_field_to_struct(json_object *parent_obj, char* field_name, char* field_type, int offset, int size, bool is_pointer)
{
    json_object *field_info;
    field_info = json_object_new_array();

    json_object_array_add(field_info, json_object_new_string(field_type));
    json_object_array_add(field_info, json_object_new_int(offset));
    json_object_array_add(field_info, json_object_new_int(size));
    json_object_array_add(field_info, json_object_new_boolean(is_pointer));

    json_object_object_add(parent_obj, field_name, field_info);

    //printf("to_string()=%s\n", json_object_to_json_string(parent_obj));
}



int main(int argc, char **argv)
{
    // final target
    json_object *type_definition, *target;
    // pointer
    json_object *kpcr, *kthread, *eprocess, *parameter, *unicode_string;
    // nonpointer
    json_object *kprcb, *kprocess, *list_entry, *peb;

json_object *mmvad_short;
json_object *rtl_node;
    type_definition = json_object_new_object();

#ifdef TEST_FORMATTED
    int sflags = 0;
#endif

    MC_SET_DEBUG(1);

#ifdef TEST_FORMATTED
    sflags = parse_flags(argc, argv);
#endif

    /* =========== type of struct ========================
     * create a json object
     * add the field to struct
     * add the new json object to json object "type_definition"
    */

    // =========== _KPCR =================================
    kpcr = json_object_new_object();
    add_field_to_struct(kpcr, "Self", "_KPCR", 0x18, 0x8, true);
    add_field_to_struct(kpcr, "Prcb", "_KPRCB", 0x180, 0x6900, false);
    json_object_object_add(type_definition, "_KPCR", kpcr);

    // =========== _KPRCB =================================
    kprcb = json_object_new_object();
    add_field_to_struct(kprcb, "CurrentThread", "_KTHREAD", 0x8, 0x8, true);
    json_object_object_add(type_definition, "_KPRCB", kprcb);

    // =========== _KTHREAD ===============================
    kthread = json_object_new_object();
    add_field_to_struct(kthread, "PROCESS", "_EPROCESS", 0x220, 0x8, true);
    json_object_object_add(type_definition, "_KTHREAD", kthread);

    // =========== _EPROCESS ==============================
    eprocess = json_object_new_object();
    add_field_to_struct(eprocess, "Pcb", "_KPROCESS", 0x00, 0x2d8, false);
    add_field_to_struct(eprocess, "ActiveProcessLinks", "_LIST_ENTRY", 0x2f0, 0x10, false);
    add_field_to_struct(eprocess, "Peb", "_PEB", 0x3f8, 0x8, true);
    add_field_to_struct(eprocess, "ImageFileName", "UChar[15]", 0x448, 0xf, false);
    add_field_to_struct(eprocess, "VadRoot", "_RTL_AVL_TREE", 0x608, 0x8, true);
    json_object_object_add(type_definition, "_EPROCESS", eprocess);

    // =========== _KPROCESS ==============================
    kprocess = json_object_new_object();
    add_field_to_struct(kprocess, "DirectoryTableBase", "Uint8B", 0x28, 0x8, true);
    json_object_object_add(type_definition, "_KPROCESS", kprocess);

    // =========== _LIST_ENTRY =============================
    list_entry = json_object_new_object();
    add_field_to_struct(list_entry, "Flink", "_LIST_ENTRY", 0x0, 0x8, true);
    add_field_to_struct(list_entry, "Blink", "_LIST_ENTRY", 0x8, 0x8, true);
    json_object_object_add(type_definition, "_LIST_ENTRY", list_entry);

    // =========== _PEB ===================================
    peb = json_object_new_object();
    add_field_to_struct(peb, "ProcessParameters", "_RTL_USER_PROCESS_PARAMETERS", 0x20, 0x8, true);
    json_object_object_add(type_definition, "_PEB", peb);

    // =========== _RTL_USER_PROCESS_PARAMETERS ===========
    parameter = json_object_new_object();
    add_field_to_struct(parameter, "ImagePathName", "_UNICODE_STRING", 0x60, 0x10, true);
    json_object_object_add(type_definition, "_RTL_USER_PROCESS_PARAMETERS", parameter);

    // =========== "ImagePathName" ========================
    unicode_string = json_object_new_object();
    add_field_to_struct(unicode_string, "Length", "USHORT", 0x0, 0x2, false);
    add_field_to_struct(unicode_string, "Buffer", "PWSTR", 0x8, 0x8, true);
    json_object_object_add(type_definition, "_UNICODE_STRING", unicode_string);

    // =========== "_MMVAD_SHORT ==========================
    mmvad_short = json_object_new_object();
    add_field_to_struct( mmvad_short, "VadNode", "_RTL_BALANCED_NODE", 0x0, 0x18, false);
    add_field_to_struct( mmvad_short, "StartingVpn", "Uint4B", 0x18, 0x4, false);
    add_field_to_struct( mmvad_short, "EndingVpn", "Uint4B", 0x1c, 0x4, false);
    add_field_to_struct( mmvad_short, "StartingVpnHigh", "UChar", 0x20, 0x1, false);
    add_field_to_struct( mmvad_short, "EndingVpnHigh", "UChar", 0x21, 0x1, false);
    json_object_object_add(type_definition, "_MMVAD_SHORT", mmvad_short);


    json_object *mmvad = json_object_new_object();
    add_field_to_struct( mmvad, "Subsection", "_SUBSECTION", 0x48, 0x8, true);
    json_object_object_add(type_definition, "_MMVAD", mmvad);

    json_object *subsection = json_object_new_object();
    add_field_to_struct( mmvad, "ControlArea", "_CONTROL_AREA", 0x0, 0x8, true);
    json_object_object_add(type_definition, "_SUBSECTION", mmvad);

    json_object *control_area = json_object_new_object();
    add_field_to_struct( control_area, "FilePointer", "_EX_FAST_REF", 0x40, 0x8, true);
    json_object_object_add(type_definition, "_CONTROL_AREA", control_area);

    json_object *file_oject = json_object_new_object();
    add_field_to_struct( file_oject, "FileName", "_UNICODE_STRING", 0x58, 0x10, true);
    json_object_object_add(type_definition, "_FILE_OBJECT", file_oject);

    json_object *unicode_str = json_object_new_object();
    add_field_to_struct( unicode_str, "Length", "Uint2B", 0x0, 0x2, false);
    add_field_to_struct( unicode_str, "MaximumLength", "Uint2B", 0x0, 0x2, false);
    add_field_to_struct( unicode_str, "Buffer", "Ptr64 Uint2B", 0x8, 0x8, true);
    json_object_object_add(type_definition, "_UNICODE_STRING", unicode_str);

    json_object *pool_header = json_object_new_object();
    add_field_to_struct( pool_header, "PoolTag", "Uint4B",  0x4, 0x4, false);
    json_object_object_add(type_definition, "_POOL_HEADER", pool_header);

    json_object *ldr_data = json_object_new_object();
    add_field_to_struct( ldr_data, "FullDllName", "_UNICODE_STRING", 0x48, 0x10, false );
    add_field_to_struct( ldr_data, "BaseDllName", "_UNICODE_STRING", 0x58, 0x10, false );
    json_object_object_add(type_definition, "_LDR_DATA_TABLE_ENTRY", ldr_data);

    // ========== "_RTL_BALANCED_NODE ====================
    rtl_node = json_object_new_object();
    add_field_to_struct( rtl_node, "Left", "_RTL_BALANCED_NODE", 0x0, 0x8, true);
    add_field_to_struct( rtl_node, "Right", "_RTL_BALANCED_NODE", 0x8, 0x8, true);
    json_object_object_add(type_definition, "_RTL_BALANCED_NODE", rtl_node);

    // check and add to json file
    printf("my_object.to_string()=%s\n", json_object_to_json_string(type_definition));
    json_object_to_file("type_definition.json", type_definition);

    // free json object
    json_object_put(kpcr);
    json_object_put(type_definition);

    return 0;
}
