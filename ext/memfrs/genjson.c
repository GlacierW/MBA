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
#include "parse_flags.h"

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
        json_object  *eprocess, *list_entry, *peb, *parameter;
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

        //int addMemberToDS(json_object *parent_obj, char* member_name, char* member_type, int offset, int size, bool is_pointer)
        addMemberToDS(kpcr, "Self", "_KPCR", 0x18, 8, true);
        addMemberToDS(kpcr, "Prcb", "_KPRCB", 0x180, 0x6900, false);
        json_object_object_add(tyde_definition, "_KPCR", kpcr);

        // =========== _KPRCB =================================
        kprcb = json_object_new_object();
        addMemberToDS(kprcb, "CurrentThread", "_KTHREAD", 0x8, 0x8, true);        
        json_object_object_add(tyde_definition, "_KPRCB", kprcb);

        // =========== _KTHREAD ===============================
        kthread = json_object_new_object();
        addMemberToDS(kthread, "PROCESS", "_EPROCESS", 0x220, 0x8, true); 
        json_object_object_add(tyde_definition, "_KTHREAD", kthread);

        // =========== _EPROCESS ==============================

        eprocess = json_object_new_object();
        addMemberToDS(eprocess, "Pcb", "_KPROCESS", 0x00, 0x2d8,  false);
        addMemberToDS(eprocess, "ActiveProcessLinks", "_LIST_ENTRY", 0x2f0, 0x10,  false);
        addMemberToDS(eprocess, "Peb", "_PEB", 0x3f8, 0x8,  false);
        addMemberToDS(eprocess, "ImageFileName", "UChar[15]", 0x448, 0xf,  false);
        json_object_object_add(tyde_definition, "_EPROCESS", eprocess);

        // =========== _KPROCESS ==============================
        kprocess = json_object_new_object();
        addMemberToDS(kprocess, "DirectoryTableBase", "Uint8B", 0x28, 0x8, true);
        json_object_object_add(tyde_definition, "_KPROCESS", kprocess);

        // =========== _LIST_ENTRY =============================
        list_entry = json_object_new_object();
        addMemberToDS(list_entry, "Flink", "_LIST_ENTRY", 0x0, 0x8, true);
        addMemberToDS(list_entry, "Blink", "_LIST_ENTRY", 0x8, 0x8, true);
        json_object_object_add(tyde_definition, "_LIST_ENTRY", list_entry);

        // =========== _PEB ===================================
        peb = json_object_new_object();
        addMemberToDS(peb, "ProcessParameters", "_RTL_USER_PROCESS_PARAMETERS", 0x20, 0x8, true);
        json_object_object_add(tyde_definition, "_PEB", peb);

        // =========== "_RTL_USER_PROCESS_PARAMETERS" =========
        parameter = json_object_new_object();
        addMemberToDS(parameter, "")



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


