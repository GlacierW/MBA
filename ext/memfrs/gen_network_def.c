/*
 *  MBA Virtual Machine Memory Introspection Self-defined Signature Generator
 *
 *  Copyright (c)  2017 ELin Ho
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
 * executable file to generate json file "network_type_definition.json":
 * gcc -I../../json-c/ gen_network_def.c ../../json-c/.libs/libjson-c.a -o genjson_network && ./genjson_network
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
    json_object *network_type_definition;
    // TCP / UDP object
    json_object *_tcp_listener, *_tcp_endpoint, *_udp_endpoint;
    // pointer
    json_object *_in_addr, *_in_addr_win10_tcpe, *_inetaf, *_local_address_win10_tcpe, *_local_address_win10_udp, *_local_address, *_addr_info;

    network_type_definition = json_object_new_object();



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
     * add the new json object to json object "network_type_definition"
     */

    // =========== _TCP_LISTENER ==========================
    _tcp_listener = json_object_new_object();
    add_field_to_struct(_tcp_listener, "Owner", "_EPROCESS", 0x30, 0x8, true);
    add_field_to_struct(_tcp_listener, "CreateTime", "WinTimeStamp", 0x40, 0x8, false);
    add_field_to_struct(_tcp_listener, "LocalAddr", "_LOCAL_ADDRESS", 0x60, 0x8, true);
    add_field_to_struct(_tcp_listener, "InetAF", "_INETAF", 0x28, 0x8, true);
    add_field_to_struct(_tcp_listener, "Port", "UINT2B", 0x72, 0x2, false);
    json_object_object_add(network_type_definition, "_TCP_LISTENER", _tcp_listener);

    // =========== _TCP_ENDPOINT ==========================
    _tcp_endpoint = json_object_new_object();
    add_field_to_struct(_tcp_endpoint, "InetAF", "_INETAF", 0x10, 0x8, true);
    add_field_to_struct(_tcp_endpoint, "AddrInfo", "_ADDRINFO", 0x18, 0x8, true);
    add_field_to_struct(_tcp_endpoint, "State", "UINT4B", 0x6c, 0x4, false);
    add_field_to_struct(_tcp_endpoint, "LocalPort", "UINT2B", 0x70, 0x2, false);
    add_field_to_struct(_tcp_endpoint, "RemotePort", "UINT2B", 0x72, 0x2, false);
    add_field_to_struct(_tcp_endpoint, "Owner", "_EPROCESS", 0x258, 0x8, true);
    add_field_to_struct(_tcp_endpoint, "CreateTime", "WinTimeStamp", 0x268, 0x8, false);
    json_object_object_add(network_type_definition, "_TCP_ENDPOINT", _tcp_endpoint);

    // =========== _UDP_ENDPOINT ==========================
    _udp_endpoint = json_object_new_object();
    add_field_to_struct(_udp_endpoint, "Owner", "_EPROCESS", 0x28, 0x8, true);
    add_field_to_struct(_udp_endpoint, "CreateTime", "WinTimeStamp", 0x58, 0x8, true);
    add_field_to_struct(_udp_endpoint, "LocalAddr", "_LOCAL_ADDRESS_WIN10_UDP", 0x80, 0x8, true);
    add_field_to_struct(_udp_endpoint, "InetAF", "_INETAF", 0x20, 0x8, true);
    add_field_to_struct(_udp_endpoint, "Port", "UINT2B", 0x78, 0x2, false);
    json_object_object_add(network_type_definition, "_UDP_ENDPOINT", _udp_endpoint);

    // =========== _INETAF ================================
    _inetaf = json_object_new_object();
    add_field_to_struct(_inetaf, "AddressFamily", "UINT2B", 0x18, 0x2, false);
    json_object_object_add(network_type_definition, "_INETAF", _inetaf);

    // =========== _ADDRINFO ==============================
    _addr_info = json_object_new_object();
    add_field_to_struct(_addr_info, "Local", "_LOCAL_ADDRESS", 0x00, 0x8, true);
    add_field_to_struct(_addr_info, "Remote", "_IN_ADDR", 0x10, 0x8, true);
    json_object_object_add(network_type_definition, "_ADDRINFO", _addr_info);

    // =========== _LOCAL_ADDRESS =========================
    _local_address = json_object_new_object();
    add_field_to_struct(_local_address, "pData", "_IN_ADDR", 0x10, 0x8, true);
    json_object_object_add(network_type_definition, "_LOCAL_ADDRESS", _local_address);

    // =========== _LOCAL_ADDRESS_WIN10_TCPE =========================
//    _local_address_win10_tcpe = json_object_new_object();
//    add_field_to_struct(_local_address_win10_tcpe, "pData", "_IN_ADDR_WIN10_TCPE", 0x10, 0x8, true);
//    json_object_object_add(network_type_definition, "_LOCAL_ADDRESS_WIN10_TCPE", _local_address_win10_tcpe);

    // =========== _LOCAL_ADDRESS_WIN10_UDP ===============
    _local_address_win10_udp = json_object_new_object();
    add_field_to_struct(_local_address_win10_udp, "pData", "_IN_ADDR", 0x00, 0x8, true);
    json_object_object_add(network_type_definition, "_LOCAL_ADDRESS_WIN10_UDP", _local_address_win10_udp);

    // =========== _IN_ADDR ===============================
    _in_addr = json_object_new_object();
    add_field_to_struct(_in_addr, "addr4", "_IpAddress", 0x00, 0x4, false);
    add_field_to_struct(_in_addr, "addr6", "_Ipv6Address", 0x00, 0x10, false);
    json_object_object_add(network_type_definition, "_IN_ADDR", _in_addr);

    // =========== _IN_ADDR_WIN10_TCPE ===============================
    _in_addr_win10_tcpe = json_object_new_object();
    add_field_to_struct(_in_addr_win10_tcpe, "addr4", "_IpAddress", 0x28, 0x4, false);
    add_field_to_struct(_in_addr_win10_tcpe, "addr6", "_Ipv6Address", 0x28, 0x10, false);
    json_object_object_add(network_type_definition, "_IN_ADDR_WIN10_TCPE", _in_addr_win10_tcpe);


    // check and add to json file
    printf("my_object.to_string()=%s\n", json_object_to_json_string(network_type_definition));
    json_object_to_file("network_type_definition.json", network_type_definition);

    // free json object
    json_object_put(network_type_definition);

    return 0;
}
