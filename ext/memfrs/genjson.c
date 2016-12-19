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
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include "json.h"
//#include "parse_flags.h"

#ifdef TEST_FORMATTED
#define json_object_to_json_string(obj) json_object_to_json_string_ext(obj,sflags)
#else
/* no special define */
#endif

/*******************************************************************
* NAME :            int addFieldToStruct( parent_obj, field_name, field_type, offset, size, is_pointer)
* DESCRIPTION :     Add a field into data structure information
* INPUTS :
*           json_object* parent_obj         json object of target data structure
*           char*        field_name         name of field
*           char*        field_type         type of firld
*           int          offset             offset of this field in the structure
*           bool         is_pointer         indicator to show if this field is a pointer     
*
* OUTPUTS :
*/
int addFieldToStruct(json_object *parent_obj, char* field_name, char* field_type, int offset, int size, bool is_pointer)
{
    json_object *field_info;
    field_info = json_object_new_array();

    json_object_array_add( field_info, json_object_new_string(field_type));
    json_object_array_add( field_info, json_object_new_int(offset));
    json_object_array_add( field_info, json_object_new_int(size));
    json_object_array_add( field_info, json_object_new_boolean(is_pointer));

    json_object_object_add( parent_obj, field_name, field_info);

    printf("to_string()=%s\n", json_object_to_json_string(parent_obj));
}



int main(int argc, char **argv)
{
	json_object *tyde_definition, *kpcr, *kthread, *kprocess, *kprcb;
        json_object  *eprocess, *list_entry, *peb, *parameter, *mmvad_short;
        json_object *rtl_node;
#ifdef TEST_FORMATTED
	int sflags = 0;
#endif

	MC_SET_DEBUG(1);

#ifdef TEST_FORMATTED
	sflags = parse_flags(argc, argv);
#endif

        tyde_definition = json_object_new_object();

        // =========== _KPCR =================================
        kpcr = json_object_new_object();

        //int addFieldToStruct(json_object *parent_obj, char* member_name, char* member_type, int offset, int size, bool is_pointer)
        addFieldToStruct(kpcr, "Self", "_KPCR", 0x18, 8, true);
        addFieldToStruct(kpcr, "Prcb", "_KPRCB", 0x180, 0x6900, false);
        json_object_object_add(tyde_definition, "_KPCR", kpcr);

        // =========== _KPRCB =================================
        kprcb = json_object_new_object();
        addFieldToStruct(kprcb, "CurrentThread", "_KTHREAD", 0x8, 0x8, true);        
        json_object_object_add(tyde_definition, "_KPRCB", kprcb);

        // =========== _KTHREAD ===============================
        kthread = json_object_new_object();
        addFieldToStruct(kthread, "Process", "_EPROCESS", 0x220, 0x8, true); 
        json_object_object_add(tyde_definition, "_KTHREAD", kthread);

        // =========== _EPROCESS ==============================

        eprocess = json_object_new_object();
        addFieldToStruct(eprocess, "Pcb", "_KPROCESS", 0x00, 0x2d8,  false);
        addFieldToStruct(eprocess, "ActiveProcessLinks", "_LIST_ENTRY", 0x2f0, 0x10,  false);
        addFieldToStruct(eprocess, "Peb", "_PEB", 0x3f8, 0x8,  true);
        addFieldToStruct(eprocess, "ImageFileName", "UChar[15]", 0x448, 0xf,  false);
        addFieldToStruct(eprocess, "VadRoot", "_RTL_AVL_TREE", 0x608, 0x8,  true);
        json_object_object_add(tyde_definition, "_EPROCESS", eprocess);

        // =========== _KPROCESS ==============================
        kprocess = json_object_new_object();
        addFieldToStruct(kprocess, "DirectoryTableBase", "Uint8B", 0x28, 0x8, true);
        json_object_object_add(tyde_definition, "_KPROCESS", kprocess);

        // =========== _LIST_ENTRY =============================
        list_entry = json_object_new_object();
        addFieldToStruct(list_entry, "Flink", "_LIST_ENTRY", 0x0, 0x8, true);
        addFieldToStruct(list_entry, "Blink", "_LIST_ENTRY", 0x8, 0x8, true);
        json_object_object_add(tyde_definition, "_LIST_ENTRY", list_entry);

        // =========== _PEB ===================================
        peb = json_object_new_object();
        addFieldToStruct(peb, "ProcessParameters", "_RTL_USER_PROCESS_PARAMETERS", 0x20, 0x8, true);
        json_object_object_add(tyde_definition, "_PEB", peb);

        // =========== "_MMVAD_SHORT ==========================
        mmvad_short = json_object_new_object();
        addFieldToStruct( mmvad_short, "VadNode", "_RTL_BALANCED_NODE", 0x0, 0x18, false);
        addFieldToStruct( mmvad_short, "StartingVpn", "Uint4B", 0x18, 0x4, false);
        addFieldToStruct( mmvad_short, "EndingVpn", "Uint4B", 0x1c, 0x4, false);
        addFieldToStruct( mmvad_short, "StartingVpnHigh", "UChar", 0x20, 0x1, false);
        addFieldToStruct( mmvad_short, "EndingVpnHigh", "UChar", 0x21, 0x1, false);
        addFieldToStruct( mmvad_short, "u", "_MMVAD_FLAGS", 0x30, 0x4, false);
        json_object_object_add(tyde_definition, "_MMVAD_SHORT", mmvad_short);  

        //json_object *mmvad_flag = json_object_new_object();
        

  
        json_object *mmvad = json_object_new_object();
        addFieldToStruct( mmvad, "Subsection", "_SUBSECTION", 0x48, 0x8, true);
        json_object_object_add(tyde_definition, "_MMVAD", mmvad);

        json_object *subsection = json_object_new_object();
        addFieldToStruct( subsection, "ControlArea", "_CONTROL_AREA", 0x0, 0x8, true);
        json_object_object_add(tyde_definition, "_SUBSECTION", subsection);
        
        json_object *control_area = json_object_new_object();
        addFieldToStruct( control_area, "FilePointer", "_EX_FAST_REF", 0x40, 0x8, true);
        json_object_object_add(tyde_definition, "_CONTROL_AREA", control_area);

        json_object *file_oject = json_object_new_object();
        addFieldToStruct( file_oject, "FileName", "_UNICODE_STRING", 0x58, 0x10, true);
        json_object_object_add(tyde_definition, "_FILE_OBJECT", file_oject);

        json_object *unicode_str = json_object_new_object();
        addFieldToStruct( unicode_str, "Length", "Uint2B", 0x0, 0x2, false);
        addFieldToStruct( unicode_str, "MaximumLength", "Uint2B", 0x0, 0x2, false);
        addFieldToStruct( unicode_str, "Buffer", "Ptr64 Uint2B", 0x8, 0x8, true);
        json_object_object_add(tyde_definition, "_UNICODE_STRING", unicode_str);
       
        json_object *pool_header = json_object_new_object();
        addFieldToStruct( pool_header, "PoolTag", "Uint4B",  0x4, 0x4, false);
        json_object_object_add(tyde_definition, "_POOL_HEADER", pool_header);
    
        json_object *ldr_data = json_object_new_object();
        addFieldToStruct( ldr_data, "FullDllName", "_UNICODE_STRING", 0x48, 0x10, false );
        addFieldToStruct( ldr_data, "BaseDllName", "_UNICODE_STRING", 0x58, 0x10, false );
        json_object_object_add(tyde_definition, "_LDR_DATA_TABLE_ENTRY", ldr_data);

        // ========== "_RTL_BALANCED_NODE ==================== 
        rtl_node = json_object_new_object();
        addFieldToStruct( rtl_node, "Left", "_RTL_BALANCED_NODE", 0x0, 0x8, true);
        addFieldToStruct( rtl_node, "Right", "_RTL_BALANCED_NODE", 0x8, 0x8, true);
        json_object_object_add(tyde_definition, "_RTL_BALANCED_NODE", rtl_node);

        printf("my_object.to_string()=%s\n", json_object_to_json_string(tyde_definition));

        json_object_to_file("type_definition.json", tyde_definition);
        printf("==========================================================");

        json_object *target;
        json_object_object_get_ex(tyde_definition, "_KPCR", &target);
        printf("my_object.to_string()=%s\n", json_object_to_json_string(target));

        json_object_put(kpcr);
        
        json_object_put(tyde_definition);

	return 0;
}


