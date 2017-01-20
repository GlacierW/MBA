/*
 *  Windows in-VM network traffic monitor implementation
 *  Some source codes are referenced from www.tcpdump.org/sniffex.c
 *
 *  Copyright (c)    2017 JuiChien Jao
 *
 * This library is free software you can redistribute it and/or
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
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "nettramon.h"
#include "libpcap/pcap/pcap.h"

static MBA_NETTRAMON_PRO   protocol_type  = NETTRAMON_PRO_UNKNOWN;
static Monitor*            local_mon      = NULL;
static struct bpf_program* filter  = NULL;
static bool                status  = 0;

struct file_ptr {
    FILE* all_file;
    FILE* tcp_file;
    FILE* udp_file;
    FILE* icmp_file;
};
typedef struct file_ptr file_ptr;

struct path_buffer {
    bool has_set;
    char all_file_path_buf  [SZ_NETTRAMON_PATH_LENGTH];
    char tcp_file_path_buf  [SZ_NETTRAMON_PATH_LENGTH];
    char udp_file_path_buf  [SZ_NETTRAMON_PATH_LENGTH];
    char icmp_file_path_buf [SZ_NETTRAMON_PATH_LENGTH];
};
typedef struct path_buffer path_buffer;

file_ptr fp[1] = {};
path_buffer pb[1] = {};


/// Static Functions

// Reset file pointer
// Return None
static void nettramon_clear_filter( void ) {
    free( filter );
    filter = NULL;
}

// Reset file pointer
// Return None
static void nettramon_clear_file_path( void ) {
    bzero( pb, sizeof(path_buffer) );
}

// Clean up all the variables
// Return none
static void nettramon_cleanup ( void ) {

    local_mon = NULL;
    if ( fp->all_file != NULL )
        fclose( fp->all_file );
    if ( fp->tcp_file != NULL )
        fclose( fp->tcp_file );
    if ( fp->udp_file != NULL )
        fclose( fp->udp_file );
    if ( fp->icmp_file != NULL )
        fclose( fp->icmp_file );
    bzero( fp, sizeof( file_ptr ) );
}

// Resume written files
// Return 0 for success, 1 for file
static unsigned int nettramon_file_path_resume( void ) {
    
    if ( pb->all_file_path_buf[0] != '\0' ) {
        fp->all_file = fopen( pb->all_file_path_buf , "a" );
        if ( fp->all_file == NULL ) {
            nettramon_clear_file_path();
            return 1;
        }
    }
    if ( pb->tcp_file_path_buf[0] != '\0' ) {
        fp->tcp_file = fopen( pb->tcp_file_path_buf , "a" );
        if ( fp->tcp_file == NULL ) {
            nettramon_clear_file_path();
            return 1;
        }
    }
    if ( pb->udp_file_path_buf[0] != '\0' ) {
        fp->udp_file = fopen( pb->udp_file_path_buf , "a" );
        if ( fp->udp_file == NULL ) {
            nettramon_clear_file_path();
            return 1;
        }
    }
    if ( pb->icmp_file_path_buf[0] != '\0' ) {
        fp->icmp_file = fopen( pb->icmp_file_path_buf , "a" );
        if ( fp->icmp_file == NULL ) {
            nettramon_clear_file_path();
            return 1;
        }
    }
    return 0;
}

// Print on Monitor or into files
// Return none
static void nettramon_printf( const char* fmt, ... ) {
    
    va_list args;

    if ( local_mon != NULL && fp->all_file == NULL && fp->tcp_file == NULL && fp->udp_file == NULL && fp->icmp_file == NULL ) {
        va_start( args, fmt );
        monitor_vprintf( local_mon, fmt, args );
        va_end( args );
        return;
    }

    switch( protocol_type ) {
        case NETTRAMON_PRO_TCP :
            if ( fp->tcp_file != NULL ) {
                va_start( args, fmt );
                vfprintf( fp->tcp_file, fmt, args );
                va_end( args );
            }
            break;
        case NETTRAMON_PRO_UDP :
            if ( fp->udp_file != NULL ) {
                va_start( args, fmt );
                vfprintf( fp->udp_file, fmt, args );
                va_end( args );
            }
            break;
        case NETTRAMON_PRO_ICMP :
            if ( fp->icmp_file != NULL ) {
                va_start( args, fmt );
                vfprintf( fp->icmp_file, fmt, args );
                va_end( args );
            }
            break;
        case NETTRAMON_PRO_UNKNOWN:
        default :
            break;
    }

    if ( protocol_type != NETTRAMON_PRO_UNKNOWN && fp->all_file != NULL ) {
        va_start( args, fmt );
        vfprintf( fp->all_file, fmt, args );
        va_end( args );
    }

    return;
}

// Print out the payload in hex and ascii format in one line
// Return none
static void print_hex_ascii_line(const u_char *payload, int len, int offset) {
    
    int i;
    const u_char *hex_head;
    const u_char *ascii_head;

    /* offset */
    nettramon_printf("0x%04x   ", offset);
    
    /* hex */
    hex_head = payload;
    for(i = 0; i < len; i++) {
        nettramon_printf("%02x ", hex_head[i]);
    }
    
    if ( len < 16 )
        for ( i = len; i < 16; i++ )
            nettramon_printf("   ");

    nettramon_printf("  ");

    /* ascii */
    ascii_head = payload;
    for(i = 0; i < len; i++) {
        if (isprint(ascii_head[i]))
            nettramon_printf("%c", ascii_head[i]);
        else
            nettramon_printf(".");
    }

    nettramon_printf("\n");

    return;
}

// Print out the whole payload
// Return none
static void print_payload( const u_char *payload, int len ) {
    
    int len_remain = len;
    int line_width = 16;            /* number of bytes per line */
    int line_len;
    int offset = 0;                 /* zero-based offset counter */
    const u_char *pl_head = payload;

    if (len <= 0)
        return;

    nettramon_printf("Payload: \n");

    /* data fits on one line */
    if (len <= line_width) {
        print_hex_ascii_line(pl_head, len, offset);
        return;
    }

    /* data spans multiple lines */
    for ( ;; ) {
        /* compute current line length */
        line_len = line_width % len_remain;
        /* print line */
        print_hex_ascii_line(pl_head, line_len, offset);
        /* compute total remaining */
        len_remain = len_remain - line_len;
        /* shift pointer to remaining bytes to print */
        pl_head = pl_head + line_len;
        /* add offset */
        offset = offset + line_width;
        /* check if we have line width chars or less */
        if (len_remain <= line_width) {
            /* print last line and get out */
            print_hex_ascii_line(pl_head, len_remain, offset);
            break;
        }
    }

    return;
}

// Print packet
// return 0 for success, 1 for fail
static int print_packet( packet_info* packet ) {
   
    protocol_type = packet->packet_protocol;

    switch( packet->packet_protocol ) {
        case NETTRAMON_PRO_TCP :
            nettramon_printf("\n==========Protocol: TCP==========\n");
            /* print source and destination IP addresses and used port */
            nettramon_printf("From: %s\t\t%d\n", inet_ntoa(packet->ip_head->ip_src), packet->tcp_head->th_sport);
            nettramon_printf("To:   %s\t\t%d\n", inet_ntoa(packet->ip_head->ip_dst), packet->tcp_head->th_dport);
            print_payload( (const u_char *)packet->payload, packet->len );
            nettramon_printf("\n==========Protocol: TCP==========\n");
            break;
        case NETTRAMON_PRO_UDP :
            nettramon_printf("\n==========Protocol: UDP==========\n");
            /* print source and destination IP addresses and used port */
            nettramon_printf("From: %s\t\t%d\n", inet_ntoa(packet->ip_head->ip_src), packet->udp_head->uh_sport);
            nettramon_printf("To:   %s\t\t%d\n", inet_ntoa(packet->ip_head->ip_dst), packet->udp_head->uh_dport);
            print_payload( (const u_char *)packet->payload, packet->len );
            nettramon_printf("\n==========Protocol: UDP==========\n");
            break;
        case NETTRAMON_PRO_ICMP :
            nettramon_printf("\n==========Protocol: ICMP==========\n");
            nettramon_printf("From: %s\n", inet_ntoa(packet->ip_head->ip_src));
            nettramon_printf("To:   %s\n", inet_ntoa(packet->ip_head->ip_dst));
            print_payload( (const u_char *)packet->payload, packet->len );
            nettramon_printf("\n==========Protocol: ICMP==========\n");
            break;
        case NETTRAMON_PRO_UNKNOWN :
            //nettramon_printf("\n==========Protocol: UNKNOWN==========\n");
            //nettramon_printf("\n==========Protocol: UNKNOWN==========\n");
        default :
            free(packet);
            return 1;
    }
    free(packet);
    return 0;
}

/// Publuc APIs

// NetTraMon is active or not
// Return boolean, True for positive and False for negative
bool nettramon_is_active( void ) {

    return status;

}

// Parser for printing result directly on the monitor or the files
// Return 0 for success, 1 for fail
int nettramon_parse_buffer( const u_char* buf, size_t len, void(*user_cb)(u_char*, size_t) ) {
    
    if ( !nettramon_is_active() )
        return 0;

    if ( filter != NULL )
        if ( !bpf_filter( filter->bf_insns, (const u_char*)buf, len, len ) )
            return 0;
    
    if ( user_cb != NULL ) {
        u_char* buf_cb = (u_char *)malloc( len * sizeof(u_char) );
        memcpy( buf_cb, buf, len );
        user_cb( buf_cb, len );
    }
    else {
        packet_info* packet;
        packet = nettramon_packet_parse( buf, len );
        if ( packet == NULL ) {
            return 1;
        }
        return print_packet( packet );
    }

    return 0;    
}

// Start to print out the packet
// Return 0 for success, 1 for already setting written files, 2 for fail
unsigned int nettramon_start ( Monitor* mon ) {

    if ( nettramon_is_active() )
        return 0;

    if ( pb->has_set ) {
        if ( nettramon_file_path_resume() )
            return 1;
    }

    if ( fp->all_file == NULL && fp->tcp_file == NULL && fp->udp_file == NULL && fp->icmp_file == NULL ) {
        if ( mon != NULL ) {
            local_mon = mon;
            status = TRUE;
            return 0;
        }
        return 2;
    }
    else {
        local_mon = NULL;
        status = TRUE;
        return 1;
    }
    
}

// Stop printing out the packet
// Return 0 for success, 1 for fail
unsigned int nettramon_stop ( void ) {

    nettramon_cleanup();
    status = 0;

    if ( local_mon == NULL )
        return 0;
    return 1;
    
}

// Set the file for writing captured packets
// Return 0 for success, 1 for fail
unsigned int nettramon_set_file_path ( const char* all_file_path, const char* tcp_file_path, const char* udp_file_path, const char* icmp_file_path ) {

    if ( !(strlen(all_file_path) == 1 && all_file_path[0] == 'N') ) {
        strncpy( pb->all_file_path_buf, all_file_path, SZ_NETTRAMON_PATH_LENGTH );
        fp->all_file = fopen( pb->all_file_path_buf , "w" );
        if ( fp->all_file == NULL ) {
            nettramon_clear_file_path();
            return 1;
        }
        pb->has_set = TRUE;
    }
    else
        fp->all_file = NULL;

    if ( !(strlen(tcp_file_path) == 1 && tcp_file_path[0] == 'N') ) {
        strncpy( pb->tcp_file_path_buf, tcp_file_path, SZ_NETTRAMON_PATH_LENGTH );
        fp->tcp_file = fopen( pb->tcp_file_path_buf , "w" );
        if ( fp->tcp_file == NULL ) {
            nettramon_clear_file_path();
            return 1;
        }
        pb->has_set = TRUE;
    }
    else
        fp->tcp_file = NULL;

    if ( !(strlen(udp_file_path) == 1 && udp_file_path[0] == 'N') ) {
        strncpy( pb->udp_file_path_buf, udp_file_path, SZ_NETTRAMON_PATH_LENGTH );
        fp->udp_file = fopen( pb->udp_file_path_buf , "w" );
        if ( fp->udp_file == NULL ) {
            nettramon_clear_file_path();
            return 1;
        }
        pb->has_set = TRUE;
    }
    else
        fp->udp_file = NULL;

    if ( !(strlen(icmp_file_path) == 1 && icmp_file_path[0] == 'N') ) {
        strncpy( pb->icmp_file_path_buf, icmp_file_path, SZ_NETTRAMON_PATH_LENGTH );
        fp->icmp_file = fopen( pb->icmp_file_path_buf , "w" );
        if ( fp->icmp_file == NULL ) {
            nettramon_clear_file_path();
            return 1;
        }
        pb->has_set = TRUE;
    }
    else
        fp->icmp_file = NULL;

    return 0;
    
}

// Set the file for writing captured packets
// Return 0 for success, 1 for fail
unsigned int nettramon_reset_file_path ( void ) {
    nettramon_clear_file_path();
    return 0;
}

// Set the filter string for pcap_compile_nopcap
// Return 0 for success, 1 for fail
unsigned int nettramon_set_filter ( const char* filter_string ) {

    filter = ( struct bpf_program *)malloc( sizeof( struct bpf_program ) );

    pcap_compile_nopcap( 1500, DLT_EN10MB, filter, filter_string, 0, 0);
    if ( filter == NULL )
        return 1;
    return 0;
}

// Set the filter string for pcap_compile_nopcap
// Return 0 for success, 1 for fail
unsigned int nettramon_reset_filter ( void ) {
    nettramon_clear_filter();
    return 0;
}

// Packet Parse
// Return struct packet_info* for success, and NULL for fail to parse the packet buf
packet_info* nettramon_packet_parse ( const u_char* buf, size_t len ) {

    packet_info* packet;
    int size_ip, size_tcp;
    size_t check_len;

    packet = (packet_info*)calloc( 1, sizeof(packet_info) );
    check_len = len;
    
    /* ETHERNET HEAD */
    if ( check_len < SZ_ETHERNET_H ) {
        free(packet);
        return NULL;
    }
    packet->eth_head = (ntm_ethernet*)(buf);

    /* Move to IP header */
    check_len -= SZ_ETHERNET_H;

    /* IP HEAD and PROTOCOL */
    if ( check_len < SZ_IP_H ) {
        free(packet);
        return NULL;
    }
    packet->ip_head = (ntm_ip*)(buf + SZ_ETHERNET_H);
    size_ip = IP_HL(packet->ip_head)*4;
    if (size_ip < 20) {
        free(packet);
        return NULL;
    }
    if ( check_len < size_ip ) {
        free(packet);
        return NULL;
    }

    /* Move to next header */
    check_len -= size_ip;

    switch( packet->ip_head->ip_p ) {
        case IPPROTO_TCP:
            if ( check_len < SZ_TCP_H ) {
                free(packet);
                return NULL;
            }
            packet->tcp_head = (ntm_tcp*)(buf + SZ_ETHERNET_H + size_ip);
            size_tcp = TH_OFF(packet->tcp_head)*4;
            if ( size_tcp < SZ_TCP_H ) {
                free(packet);
                return NULL;
            }
            if ( check_len < size_tcp ) {
                free(packet);
                return NULL;
            }
            packet->packet_protocol = NETTRAMON_PRO_TCP;
            packet->payload = (u_char*)(buf + SZ_ETHERNET_H + size_ip + size_tcp);
            check_len -= size_tcp;
            packet->len = ntohs(packet->ip_head->ip_len) - (size_ip + size_tcp);
            if ( check_len != packet->len ) {
                free(packet);
                return NULL;
            }
            packet->udp_head = NULL;
            packet->icmp_head = NULL;
            return packet;
        case IPPROTO_UDP:
            if ( check_len < SZ_UDP_H ) {
                free(packet);
                return NULL;
            }
            packet->udp_head = (ntm_udp*)(buf + SZ_ETHERNET_H + size_ip);
            if ( check_len != ntohs(packet->udp_head->uh_leng) ) {
                free(packet);
                return NULL;
            }
            packet->packet_protocol = NETTRAMON_PRO_UDP;
            packet->payload = (u_char*)(buf + SZ_ETHERNET_H + size_ip + SZ_UDP_H);
            packet->len = ntohs(packet->ip_head->ip_len) - (size_ip + SZ_UDP_H);
            check_len -= SZ_UDP_H;
            packet->len = ntohs(packet->ip_head->ip_len) - (size_ip + SZ_UDP_H);
            if ( check_len != packet->len ) {
                free(packet);
                return NULL;
            }
            packet->tcp_head = NULL;
            packet->icmp_head = NULL;
            return packet;
        case IPPROTO_ICMP:
            if ( check_len < SZ_ICMP_H ) {
                free(packet);
                return NULL;
            }
            packet->icmp_head = (ntm_icmp*)(buf + SZ_ETHERNET_H + size_ip);
            packet->packet_protocol = NETTRAMON_PRO_ICMP; 
            packet->payload = (u_char*)(buf + SZ_ETHERNET_H + size_ip + SZ_ICMP_H);
            packet->len = ntohs(packet->ip_head->ip_len) - (size_ip + SZ_ICMP_H);
            check_len -= SZ_ICMP_H;
            if ( check_len != packet->len ) {
                free(packet);
                return NULL;
            }
            packet->tcp_head = NULL;
            packet->udp_head = NULL;
            return packet;
        default:
            packet->packet_protocol = NETTRAMON_PRO_UNKNOWN;
            packet->eth_head = NULL;
            packet->ip_head = NULL;
            packet->tcp_head = NULL;
            packet->udp_head = NULL;
            packet->icmp_head = NULL;
            packet->payload = NULL;
            packet->len = 0;
            return packet;
    }

}
