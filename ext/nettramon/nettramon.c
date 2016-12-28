/*
 *
 *  Windows in-VM network traffic monitor implementation
 *  Some source codes are referenced from www.tcpdump.org/sniffex.c
 *
 *  Copyright (c)    2016 JuiChien Jao
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

MBA_NETTRAMON_PRO protocol_type  = NETTRAMON_PRO_UNKNOWN;
Monitor*          local_mon      = NULL;

struct file_ptr {
    FILE* all_file;
    FILE* tcp_file;
    FILE* udp_file;
    FILE* icmp_file;
};
typedef struct file_ptr file_ptr;

struct path_buffer {
    MBA_NETTRAMON_STATUS status;
    char all_file_path_buf  [SZ_NETTRAMON_PATH_LENGTH];
    char tcp_file_path_buf  [SZ_NETTRAMON_PATH_LENGTH];
    char udp_file_path_buf  [SZ_NETTRAMON_PATH_LENGTH];
    char icmp_file_path_buf [SZ_NETTRAMON_PATH_LENGTH];
};
typedef struct path_buffer path_buffer;

file_ptr fp[1] = {};
path_buffer pb[1] = {};

/// Private Functions

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

    fp->all_file = fopen( pb->all_file_path_buf , "a" );
    if ( fp->all_file == NULL ) {
        nettramon_clear_file_path();
        return 1;
    }
    fp->tcp_file = fopen( pb->tcp_file_path_buf , "a" );
    if ( fp->tcp_file == NULL ) {
        nettramon_clear_file_path();
        return 1;
    }
    fp->udp_file = fopen( pb->udp_file_path_buf , "a" );
    if ( fp->udp_file == NULL ) {
        nettramon_clear_file_path();
        return 1;
    }
    fp->icmp_file = fopen( pb->icmp_file_path_buf , "a" );
    if ( fp->icmp_file == NULL ) {
        nettramon_clear_file_path();
        return 1;
    }
    return 0;
}

// Print in Monitor
// Return none
static void nettramon_printf( const char* fmt, ... ) {
    
    va_list args;

    if ( fp->all_file != NULL && protocol_type != NETTRAMON_PRO_UNKNOWN ) {
        va_start( args, fmt );
        vfprintf( fp->all_file, fmt, args );
        va_end( args );
    }

    if ( fp->tcp_file != NULL && protocol_type == NETTRAMON_PRO_TCP ) {
        va_start( args, fmt );
        vfprintf( fp->tcp_file, fmt, args );
        va_end( args );
        return;
    }

    if ( fp->udp_file != NULL && protocol_type == NETTRAMON_PRO_UDP ) {
        va_start( args, fmt );
        vfprintf( fp->udp_file, fmt, args );
        va_end( args );
        return;
    }

    if ( fp->icmp_file != NULL && protocol_type == NETTRAMON_PRO_ICMP ) {
        va_start( args, fmt );
        vfprintf( fp->icmp_file, fmt, args );
        va_end( args );
        return;
    }

    if ( local_mon != NULL && fp->all_file == NULL && fp->tcp_file == NULL && fp->udp_file == NULL && fp->icmp_file == NULL ) {
        va_start( args, fmt );
        monitor_vprintf( local_mon, fmt, args );
        va_end( args );
    }
}

// Print out the payload in hex and ascii format in one line
// Return none
static void print_hex_ascii_line(const u_char *payload, int len, int offset) {
    
    int i;
    const u_char *hex_head;
    const u_char *ascii_head;

    /* offset */
    nettramon_printf("%05d   ", offset);
    
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
    
    int len_rem = len;
    int line_width = 16;            /* number of bytes per line */
    int line_len;
    int offset = 0;                 /* zero-based offset counter */
    const u_char *ch = payload;

    if (len <= 0)
        return;

    nettramon_printf("Payload: \n");

    /* data fits on one line */
    if (len <= line_width) {
        print_hex_ascii_line(ch, len, offset);
        return;
    }

    /* data spans multiple lines */
    for ( ;; ) {
        /* compute current line length */
        line_len = line_width % len_rem;
        /* print line */
        print_hex_ascii_line(ch, line_len, offset);
        /* compute total remaining */
        len_rem = len_rem - line_len;
        /* shift pointer to remaining bytes to print */
        ch = ch + line_len;
        /* add offset */
        offset = offset + line_width;
        /* check if we have line width chars or less */
        if (len_rem <= line_width) {
            /* print last line and get out */
            print_hex_ascii_line(ch, len_rem, offset);
            break;
        }
    }

    return;
}

// Distinguish protocol
// Return packet type
static MBA_NETTRAMON_PRO packet_type_parse ( const char* buf ) {

    struct sniff_ip* packet_ip;              /* The IP header */

    packet_ip = (struct sniff_ip*)(buf + SZ_ETHERNET);

    switch( packet_ip->ip_p ) {
        case IPPROTO_TCP:
            return NETTRAMON_PRO_TCP; 
        case IPPROTO_UDP:
            return NETTRAMON_PRO_UDP; 
        case IPPROTO_ICMP:
            return NETTRAMON_PRO_ICMP; 
        default:
            return NETTRAMON_PRO_UNKNOWN;
    }

}

// Parse tcp packet
// return 0 for success, 1 for fail
static int tcp_parse( const char* buf ) {

    struct sniff_ip *packet_ip;              /* The IP header */
    struct sniff_tcp *tcp;            /* The TCP header */
    char *payload;                    /* Packet payload */

    int size_ip,
        size_tcp,
        size_payload; 

    nettramon_printf("\n==========Protocol: TCP==========\n");
   
    /* define/compute ip header offset */
    packet_ip = (struct sniff_ip*)(buf + SZ_ETHERNET);
    size_ip = IP_HL(packet_ip)*4;
    if (size_ip < 20) {
        nettramon_printf("Invalid IP header length: %u bytes\n", size_ip);
        return 1;
    }

    /* define/compute tcp header offset */
    tcp = (struct sniff_tcp*)(buf + SZ_ETHERNET + size_ip);
    size_tcp = TH_OFF(tcp)*4;
    if (size_tcp < 20) {
        nettramon_printf(" Invalid TCP header length: %u bytes\n", size_tcp);
        return 1;
    }
    
    /* print source and destination IP addresses */
    nettramon_printf(" From: %s\t%d\n", inet_ntoa(packet_ip->ip_src), tcp->th_sport);
    nettramon_printf(" To:   %s\t%d\n", inet_ntoa(packet_ip->ip_dst), tcp->th_dport);

    /* define/compute tcp payload (segment) offset */
    payload = (char *)(buf + SZ_ETHERNET + size_ip + size_tcp);
    
    /* compute tcp payload (segment) size */
    size_payload = ntohs(packet_ip->ip_len) - (size_ip + size_tcp);
    if (size_payload <= 0) 
        nettramon_printf(" No TCP packet payload\n");
    else
        print_payload( (const u_char *)payload, size_payload );

    nettramon_printf("==========Protocol: TCP==========\n");

    return 0;
}

// Parse udp packet
// return 0 for success, 1 for fail
static int udp_parse( const char* buf ) {

    struct sniff_ip *packet_ip;       /* The IP header */
    struct sniff_udp *udp;            /* The UDP header */
    char *payload;                    /* Packet payload */

    int size_ip,
        size_udp,
        size_payload; 

    nettramon_printf("\n==========Protocol: UDP==========\n");
   
    /* define/compute ip header offset */
    packet_ip = (struct sniff_ip*)(buf + SZ_ETHERNET);
    size_ip = IP_HL(packet_ip)*4;
    if (size_ip < 20) {
        nettramon_printf("Invalid IP header length: %u bytes\n", size_ip);
        return 1;
    }

    /* define/compute tcp header offset */
    udp = (struct sniff_udp*)(buf + SZ_ETHERNET + size_ip);
    size_udp = sizeof(udp);
    if (size_udp < (sizeof(u_int))*2) {
        nettramon_printf(" Invalid UDP header length: %u bytes\n", size_udp);
        return 1;
    }
    
    /* print source and destination IP addresses */
    nettramon_printf(" From: %s\t%d\n", inet_ntoa(packet_ip->ip_src), udp->uh_sport);
    nettramon_printf(" To:   %s\t%d\n", inet_ntoa(packet_ip->ip_dst), udp->uh_dport);

    /* define/compute tcp payload (segment) offset */
    payload = (char *)(buf + SZ_ETHERNET + size_ip + size_udp);
    
    /* compute tcp payload (segment) size */
    size_payload = ntohs(packet_ip->ip_len) - (size_ip + size_udp);
    if (size_payload <= 0) 
        nettramon_printf(" No UDP packet payload\n");
    else
        print_payload( (const u_char *)payload, size_payload );

    nettramon_printf("==========Protocol: UDP==========\n");

    return 0;
}

// Parse icmp packet
// return 0 for success, 1 for fail
static int icmp_parse( const char* buf ) {

    struct sniff_ip *packet_ip;       /* The IP header */
    struct sniff_icmp *icmp;            /* The UDP header */
    char *payload;                    /* Packet payload */

    int size_ip,
        size_icmp,
        size_payload; 

    nettramon_printf("\n==========Protocol: ICMP==========\n");
   
    /* define/compute ip header offset */
    packet_ip = (struct sniff_ip*)(buf + SZ_ETHERNET);
    size_ip = IP_HL(packet_ip)*4;
    if (size_ip < 20) {
        nettramon_printf("Invalid IP header length: %u bytes\n", size_ip);
        return 1;
    }

    /* print source and destination IP addresses */
    nettramon_printf(" From: %s\n", inet_ntoa(packet_ip->ip_src));
    nettramon_printf(" To:   %s\n", inet_ntoa(packet_ip->ip_dst));

    /* define/compute tcp header offset */
    icmp = (struct sniff_icmp*)(buf + SZ_ETHERNET + size_ip);
    size_icmp = sizeof(icmp);
    if (size_icmp < 8) {
        nettramon_printf(" Invalid ICMP header length: %u bytes\n", size_icmp);
        return 1;
    }
    
    /* define/compute tcp payload (segment) offset */
    payload = (char *)(buf + SZ_ETHERNET + size_ip + size_icmp);
    
    /* compute tcp payload (segment) size */
    size_payload = ntohs(packet_ip->ip_len) - (size_ip + size_icmp);
    if (size_payload <= 0) 
        nettramon_printf(" No ICMP packet payload\n");
    else
        print_payload( (const u_char *)payload, size_payload );

    nettramon_printf("==========Protocol: ICMP==========\n");

    return 0;
}

/// Publuc APIs

// NetTraMon is active or not
bool nettramon_is_active( void ) {
    if ( local_mon == NULL ) {
        if ( fp->all_file == NULL && fp->tcp_file == NULL && fp->udp_file == NULL && fp->icmp_file == NULL )
            return FALSE;
        else
            return TRUE;
    }
    else
        return TRUE;
}

// Parser
int nettramon_parse_buffer( const char* buf, size_t len ) {
    
    if ( nettramon_is_active() == FALSE )
        return 0;

    protocol_type = packet_type_parse( buf );

    /* determine protocol */    
    switch( protocol_type ) {
        case NETTRAMON_PRO_TCP:
            return tcp_parse( buf ); 
        case NETTRAMON_PRO_UDP:
            return udp_parse( buf );
        case NETTRAMON_PRO_ICMP:
            return icmp_parse( buf );
        default:
            break;
    }

    return 0;    
}

// Start to print out the packet
// Return 0 for success, 1 for already setting written files, 2 for fail
unsigned int nettramon_start ( Monitor* mon ) {

    if ( pb->status == NETTRAMON_STA_PAUSE ) {
        nettramon_file_path_resume();
        pb->status = NETTRAMON_STA_BEGIN;
    }
        
    if ( fp->all_file == NULL && fp->tcp_file == NULL && fp->udp_file == NULL && fp->icmp_file == NULL ) {
        if ( mon != NULL ) {
            local_mon = mon;
            return 0;
        }
        return 2;
    }
    else {
        local_mon = NULL;
        return 1;
   }
    
}

// Stop printing out the packet
// Return 0 for success, 1 for fail
unsigned int nettramon_stop ( void ) {

    pb->status = NETTRAMON_STA_PAUSE;
    nettramon_cleanup();

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
