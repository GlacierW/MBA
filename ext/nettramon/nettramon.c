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

Monitor* local_mon = NULL;

/// Private Functions

// Print in Monitor
// Return none
static void nettramon_mon_printf( const char* fmt, ... ) {
    
    va_list args;

    if( local_mon  == NULL )
        return;
    
    va_start( args, fmt );
    monitor_vprintf( local_mon, fmt, args );
    va_end( args );
}

// Print out the payload in hex and ascii format in one line
// Return none
static void print_hex_ascii_line(const u_char *payload, int len, int offset) {
    int i;
    int gap;
    const u_char *ch;

    /* offset */
    nettramon_mon_printf("%05d   ", offset);
    
    /* hex */
    ch = payload;
    for(i = 0; i < len; i++) {
        printf("%02x ", *ch);
        ch++;
        /* print extra space after 8th byte for visual aid */
        if (i == 7)
            nettramon_mon_printf(" ");
    }
    /* print space to handle line less than 8 bytes */
    if (len < 8)
        nettramon_mon_printf(" ");
    
    /* fill hex gap with spaces if not full line */
    if (len < 16) {
        gap = 16 - len;
        for (i = 0; i < gap; i++) {
            nettramon_mon_printf("   ");
        }
    }
    nettramon_mon_printf("   ");
    
    /* ascii (if printable) */
    ch = payload;
    for(i = 0; i < len; i++) {
        if (isprint(*ch))
            nettramon_mon_printf("%c", *ch);
        else
            nettramon_mon_printf(".");
        ch++;
    }

    nettramon_mon_printf("\n");

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

    nettramon_mon_printf("Payload: \n");

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

    nettramon_mon_printf("\n==========Protocol: TCP==========\n");
   
    /* define/compute ip header offset */
    packet_ip = (struct sniff_ip*)(buf + SZ_ETHERNET);
    size_ip = IP_HL(packet_ip)*4;
    if (size_ip < 20) {
        nettramon_mon_printf("Invalid IP header length: %u bytes\n", size_ip);
        return 1;
    }

    /* print source and destination IP addresses */
    nettramon_mon_printf(" From: %s\n", inet_ntoa(packet_ip->ip_src));
    nettramon_mon_printf(" To:   %s\n", inet_ntoa(packet_ip->ip_dst));

    /* define/compute tcp header offset */
    tcp = (struct sniff_tcp*)(buf + SZ_ETHERNET + size_ip);
    size_tcp = TH_OFF(tcp)*4;
    if (size_tcp < 20) {
        nettramon_mon_printf(" Invalid TCP header length: %u bytes\n", size_tcp);
        return 1;
    }
    
    /* define/compute tcp payload (segment) offset */
    payload = (char *)(buf + SZ_ETHERNET + size_ip + size_tcp);
    
    /* compute tcp payload (segment) size */
    size_payload = ntohs(packet_ip->ip_len) - (size_ip + size_tcp);
    if (size_payload <= 0) 
        nettramon_mon_printf(" No TCP packet payload\n");
    else
        print_payload( (const u_char *)payload, size_payload );

    nettramon_mon_printf("\n==========Protocol: TCP==========\n");

    return 0;
}

/// Publuc APIs

static void aaaaa( void ) {
    
    pcap_t *handle;

     handle = pcap_open_live("aaa", 1, 1, 1000, NULL);
     if (handle == NULL) {
         return;
     }
}

// Parser
int nettramon_parse_buffer( const char* buf, size_t len ) {
  aaaaa(); 
    if ( local_mon == NULL )
        return 0;

    MBA_NETTRAMON_PRO protocol_type;
    protocol_type = packet_type_parse( buf );

    /* determine protocol */    
    switch( protocol_type ) {
        case NETTRAMON_PRO_TCP:
            return tcp_parse( buf ); 
        case NETTRAMON_PRO_UDP:
            return 0;
        case NETTRAMON_PRO_ICMP:
            return 0;
        default:
            break;
    }

    return 0;    
}

// Start to print out the packet
unsigned int nettramon_start ( Monitor* mon ) {

    if ( mon != NULL ) {
        local_mon = mon;
        return 1;
    }
    return 0;
    
}

// Stop printing out the packet
unsigned int nettramon_stop ( void ) {

    local_mon = NULL;

    if ( local_mon == NULL )
        return 1;
    return 0;
    
}
