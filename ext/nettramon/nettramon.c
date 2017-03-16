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

ntm_context ntm[1] = { { 0, .rwlock=PTHREAD_RWLOCK_INITIALIZER } };

/// Static Functions

// Reset file pointer
// Return None
static void nettramon_clear_filter( void ) {
    free( ntm->filter );
    ntm->filter = NULL;
}

// Reset file pointer
// Return None
static void nettramon_clear_file_path( void ) {
    bzero( ntm->pb, sizeof(path_buffer) );
}

// Clean up all the variables
// Return none
static void nettramon_cleanup ( void ) {

    ntm->local_mon = NULL;
    if ( ntm->fp->all_file != NULL )
        fclose( ntm->fp->all_file );
    if ( ntm->fp->tcp_file != NULL )
        fclose( ntm->fp->tcp_file );
    if ( ntm->fp->udp_file != NULL )
        fclose( ntm->fp->udp_file );
    if ( ntm->fp->icmp_file != NULL )
        fclose( ntm->fp->icmp_file );
    bzero( ntm->fp, sizeof( file_ptr ) );
}

// Resume written files
// Return 0 for success, 1 for file
static unsigned int nettramon_file_path_resume( void ) {
    
    if ( ntm->pb->all_file_path_buf[0] != '\0' ) {
        ntm->fp->all_file = fopen( ntm->pb->all_file_path_buf , "a" );
        if ( ntm->fp->all_file == NULL ) {
            nettramon_clear_file_path();
            return 1;
        }
    }
    if ( ntm->pb->tcp_file_path_buf[0] != '\0' ) {
        ntm->fp->tcp_file = fopen( ntm->pb->tcp_file_path_buf , "a" );
        if ( ntm->fp->tcp_file == NULL ) {
            nettramon_clear_file_path();
            return 1;
        }
    }
    if ( ntm->pb->udp_file_path_buf[0] != '\0' ) {
        ntm->fp->udp_file = fopen( ntm->pb->udp_file_path_buf , "a" );
        if ( ntm->fp->udp_file == NULL ) {
            nettramon_clear_file_path();
            return 1;
        }
    }
    if ( ntm->pb->icmp_file_path_buf[0] != '\0' ) {
        ntm->fp->icmp_file = fopen( ntm->pb->icmp_file_path_buf , "a" );
        if ( ntm->fp->icmp_file == NULL ) {
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

    if ( ntm->local_mon != NULL && ntm->fp->all_file == NULL && ntm->fp->tcp_file == NULL && ntm->fp->udp_file == NULL && ntm->fp->icmp_file == NULL ) {
        va_start( args, fmt );
        monitor_vprintf( ntm->local_mon, fmt, args );
        va_end( args );
        return;
    }

    switch( ntm->protocol_type ) {
        case NETTRAMON_PRO_TCP :
            if ( ntm->fp->tcp_file != NULL ) {
                va_start( args, fmt );
                vfprintf( ntm->fp->tcp_file, fmt, args );
                va_end( args );
            }
            break;
        case NETTRAMON_PRO_UDP :
            if ( ntm->fp->udp_file != NULL ) {
                va_start( args, fmt );
                vfprintf( ntm->fp->udp_file, fmt, args );
                va_end( args );
            }
            break;
        case NETTRAMON_PRO_ICMP :
            if ( ntm->fp->icmp_file != NULL ) {
                va_start( args, fmt );
                vfprintf( ntm->fp->icmp_file, fmt, args );
                va_end( args );
            }
            break;
        case NETTRAMON_PRO_UNKNOWN:
        default :
            break;
    }

    if ( ntm->protocol_type != NETTRAMON_PRO_UNKNOWN && ntm->fp->all_file != NULL ) {
        va_start( args, fmt );
        vfprintf( ntm->fp->all_file, fmt, args );
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
   
    ntm->protocol_type = packet->packet_protocol;

    switch( packet->packet_protocol ) {
        case NETTRAMON_PRO_TCP :
            nettramon_printf("\n==========Protocol: TCP==========\n");
            /* print source and destination IP addresses and used port */
            nettramon_printf("From: %s\t\t",    inet_ntoa(packet->ip_head->ip_src));
            nettramon_printf("%d\n",            packet->tcp_head->th_sport);
            nettramon_printf("To:   %s\t",      inet_ntoa(packet->ip_head->ip_dst));
            nettramon_printf("%d\n",            packet->tcp_head->th_dport);
            /* print packet payload */
            print_payload( (const u_char*)packet->payload, packet->len );
            break;
        case NETTRAMON_PRO_UDP :
            nettramon_printf("\n==========Protocol: UDP==========\n");
            /* print source and destination IP addresses and used port */
            nettramon_printf("From: %s\t\t",    inet_ntoa(packet->ip_head->ip_src));
            nettramon_printf("%d\n",            packet->udp_head->uh_sport);
            nettramon_printf("To:   %s\t",      inet_ntoa(packet->ip_head->ip_dst));
            nettramon_printf("%d\n",            packet->udp_head->uh_dport);
            /* print packet payload */
            print_payload( (const u_char*)packet->payload, packet->len );
            break;
        case NETTRAMON_PRO_ICMP :
            nettramon_printf("\n==========Protocol: ICMP==========\n");
            nettramon_printf("From: %s\n",      inet_ntoa(packet->ip_head->ip_src));
            nettramon_printf("To:   %s\n",      inet_ntoa(packet->ip_head->ip_dst));
            /* print packet payload */
            print_payload( (const u_char*)packet->payload, packet->len );
            break;
        case NETTRAMON_PRO_UNKNOWN :
            //nettramon_printf("\n==========Protocol: UNKNOWN==========\n");
        default :
            free(packet);
            return 1;
    }
    free(packet);
    return 0;
}

// Clear input packet_info structure
// Return none
static void clear_packet_info ( packet_info* in ) {

    in->packet_protocol = NETTRAMON_PRO_UNKNOWN;
    in->eth_head = NULL;
    in->ip_head = NULL;
    in->tcp_head = NULL;
    in->udp_head = NULL;
    in->icmp_head = NULL;
    in->payload = NULL;
    in->len = 0;

}

static int get_nettramon_cb_uid ( void ) {
    
    int i;

    for ( i = 0; i < SZ_NETTRAMON_MAX_CB; i++ )
        if ( ntm->cb_arr[i] == NULL )
            break;

    if ( i == SZ_NETTRAMON_MAX_CB )
        return -1;

    return i;
}


/// Publuc APIs

// NetTraMon is active or not
// Return boolean, True for positive and False for negative
bool nettramon_is_active( void ) {

    return ntm->status;

}

// Set the call back function for further analysis according to the "action" argument
// Return 0 for success, 1 for fail
int nettramon_set_cb( void*(*cb)( size_t, haddr_t, void* ), void* cb_arg ) {

    pthread_rwlock_wrlock( &ntm->rwlock );
    
    int next_uid = get_nettramon_cb_uid();

    if ( cb == NULL || next_uid == -1 ) 
        return 1;

    nettramon_cb* tmp = (nettramon_cb*)calloc( 1, sizeof(nettramon_cb) );
    tmp->uid      = next_uid;
    tmp->enabled  = TRUE;
    tmp->user_cb  = cb;
    tmp->cb_arg   = cb_arg;
    tmp->next     = NULL;

    LL_APPEND( ntm->cb_list, tmp );

    // Add cb to array
    ntm->cb_arr[next_uid] = tmp;
    
    pthread_rwlock_unlock( &ntm->rwlock );

    return next_uid;
}
/* Delete the call back function with the input uid */
/* Return 0 for success or 1 for fail */
int nettramon_delete_cb( int uid ) {
    
    pthread_rwlock_wrlock( &ntm->rwlock );

    if ( ntm->cb_arr[uid] == NULL )
        return 1;

    nettramon_cb* target = NULL;
    nettramon_cb* tmp = NULL;

    // Find the target cb
    LL_FOREACH_SAFE( ntm->cb_list, target, tmp ) { 
        if ( target->uid == uid )
            LL_DELETE( ntm->cb_list, target );
    }

    pthread_rwlock_unlock( &ntm->rwlock );
    
    return 0;
}

/* Enable the call back function with the input uid */
/* Return 0 for success or 1 for fail */
int nettramon_enable_cb( int uid ) {
    
    pthread_rwlock_wrlock( &ntm->rwlock );

    if ( ntm->cb_arr[uid] == NULL )
        return 1;
    
    ntm->cb_arr[uid]->enabled = TRUE;

    pthread_rwlock_unlock( &ntm->rwlock );
    return 0;
}

/* Disable the call back function with the input uid */
/* Return 0 for success or 1 for fail */
int nettramon_disable_cb( int uid ) {
    
    pthread_rwlock_wrlock( &ntm->rwlock );

    if ( ntm->cb_arr[uid] == NULL )
        return 1;
    
    ntm->cb_arr[uid]->enabled = FALSE;

    pthread_rwlock_wrlock( &ntm->rwlock );
    return 0;
}

// Parser for printing result directly on the monitor or the files
// Return 0 for success, 1 for fail
int nettramon_packet_capture( const u_char* buf, size_t len, haddr_t packet_haddr ) {
    
    if ( !nettramon_is_active() )
        return 0;

    if ( ntm->filter != NULL )
        if ( !bpf_filter( ntm->filter->bf_insns, (const u_char*)buf, len, len ) )
            return 0;
 
    pthread_rwlock_wrlock( &ntm->rwlock );

    if ( ntm->cb_list != NULL ) {
        nettramon_cb* tmp = ntm->cb_list;
        while( tmp != NULL ) {
            tmp->user_cb( len, packet_haddr, tmp->cb_arg );
            tmp = tmp->next;
        }
    }

    pthread_rwlock_unlock( &ntm->rwlock );

    if ( ntm->local_mon != NULL || ntm->fp->all_file != NULL || ntm->fp->tcp_file != NULL || ntm->fp->udp_file != NULL || ntm->fp->icmp_file != NULL ) {
        packet_info* packet = (packet_info*)calloc( 1, sizeof(packet_info) );
        if ( nettramon_packet_parse( buf, len, packet ) != 0 )
            return 1;
        return print_packet( packet );
    }

    return 0;
}

// Start to print out the packet
// Return 0 for success, 1 for already setting written files, 2 for fail
unsigned int nettramon_start ( Monitor* mon ) {

    if ( nettramon_is_active() )
        return 0;

    if ( ntm->pb->has_set ) {
        if ( nettramon_file_path_resume() )
            return 1;
    }

    if ( ntm->fp->all_file == NULL && ntm->fp->tcp_file == NULL && ntm->fp->udp_file == NULL && ntm->fp->icmp_file == NULL ) {
        if ( mon != NULL || ntm->cb_list != NULL ) {
            ntm->local_mon = mon;
            ntm->status = TRUE;
            return 0;
        }
        return 2;
    }
    else {
        ntm->local_mon = NULL;
        ntm->status = TRUE;
        return 1;
    }
    
}

// Stop printing out the packet
// Return 0 for success, 1 for fail
unsigned int nettramon_stop ( void ) {

    nettramon_cleanup();
    ntm->status = 0;

    if ( ntm->local_mon == NULL )
        return 0;
    return 1;
    
}

// Set the file for writing captured packets
// Return 0 for success, 1 for fail
unsigned int nettramon_set_file_path ( const char* all_file_path, const char* tcp_file_path, const char* udp_file_path, const char* icmp_file_path ) {

    if ( !(strlen(all_file_path) == 1 && all_file_path[0] == 'N') ) {
        strncpy( ntm->pb->all_file_path_buf, all_file_path, SZ_NETTRAMON_PATH_LENGTH );
        ntm->fp->all_file = fopen( ntm->pb->all_file_path_buf , "w" );
        if ( ntm->fp->all_file == NULL ) {
            nettramon_clear_file_path();
            return 1;
        }
        ntm->pb->has_set = TRUE;
    }
    else
        ntm->fp->all_file = NULL;

    if ( !(strlen(tcp_file_path) == 1 && tcp_file_path[0] == 'N') ) {
        strncpy( ntm->pb->tcp_file_path_buf, tcp_file_path, SZ_NETTRAMON_PATH_LENGTH );
        ntm->fp->tcp_file = fopen( ntm->pb->tcp_file_path_buf , "w" );
        if ( ntm->fp->tcp_file == NULL ) {
            nettramon_clear_file_path();
            return 1;
        }
        ntm->pb->has_set = TRUE;
    }
    else
        ntm->fp->tcp_file = NULL;

    if ( !(strlen(udp_file_path) == 1 && udp_file_path[0] == 'N') ) {
        strncpy( ntm->pb->udp_file_path_buf, udp_file_path, SZ_NETTRAMON_PATH_LENGTH );
        ntm->fp->udp_file = fopen( ntm->pb->udp_file_path_buf , "w" );
        if ( ntm->fp->udp_file == NULL ) {
            nettramon_clear_file_path();
            return 1;
        }
        ntm->pb->has_set = TRUE;
    }
    else
        ntm->fp->udp_file = NULL;

    if ( !(strlen(icmp_file_path) == 1 && icmp_file_path[0] == 'N') ) {
        strncpy( ntm->pb->icmp_file_path_buf, icmp_file_path, SZ_NETTRAMON_PATH_LENGTH );
        ntm->fp->icmp_file = fopen( ntm->pb->icmp_file_path_buf , "w" );
        if ( ntm->fp->icmp_file == NULL ) {
            nettramon_clear_file_path();
            return 1;
        }
        ntm->pb->has_set = TRUE;
    }
    else
        ntm->fp->icmp_file = NULL;

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

    ntm->filter = ( struct bpf_program *)malloc( sizeof( struct bpf_program ) );

    pcap_compile_nopcap( 1500, DLT_EN10MB, ntm->filter, filter_string, 0, 0);
    if ( ntm->filter == NULL )
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
// Return 0 for success, 1 for fail
int nettramon_packet_parse ( const u_char* buf, size_t len, packet_info* user_info ) {

    if ( user_info == NULL )
        return 1;

    int size_ip, size_tcp;
    size_t check_len;
    check_len = len;
    
    /* ETHERNET HEAD */
    if ( check_len < SZ_ETHERNET_H ) {
        clear_packet_info( user_info );
        return 1;
    }
    user_info->eth_head = (ntm_ethernet*)(buf);

    /* Move to IP header */
    check_len -= SZ_ETHERNET_H;

    /* IP HEAD and PROTOCOL */
    if ( check_len < SZ_IP_H ) {
        clear_packet_info( user_info );
        return 1;
    }
    user_info->ip_head = (ntm_ip*)(buf + SZ_ETHERNET_H);
    size_ip = IP_HL(user_info->ip_head)*4;
    if ( size_ip < 20 ||  check_len < size_ip ) {
        clear_packet_info( user_info );
        return 1;
    }

    /* Move to next header */
    check_len -= size_ip;

    switch( user_info->ip_head->ip_p ) {
        case IPPROTO_TCP:
            if ( check_len < SZ_TCP_H ) {
                clear_packet_info( user_info );
                return 1;
            }
            user_info->tcp_head = (ntm_tcp*)(buf + SZ_ETHERNET_H + size_ip);
            size_tcp = TH_OFF(user_info->tcp_head)*4;
            if ( size_tcp < SZ_TCP_H || check_len < size_tcp ) {
                clear_packet_info( user_info );
                return 1;
            }
            user_info->packet_protocol = NETTRAMON_PRO_TCP;
            user_info->payload = (u_char*)(buf + SZ_ETHERNET_H + size_ip + size_tcp);
            check_len -= size_tcp;
            user_info->len = ntohs(user_info->ip_head->ip_len) - (size_ip + size_tcp);
            if ( check_len != user_info->len ) {
                clear_packet_info( user_info );
                return 1;
            }
            user_info->udp_head = NULL;
            user_info->icmp_head = NULL;
            return 0;
        case IPPROTO_UDP:
            if ( check_len < SZ_UDP_H ) {
                clear_packet_info( user_info );
                return 1;
            }
            user_info->udp_head = (ntm_udp*)(buf + SZ_ETHERNET_H + size_ip);
            if ( check_len != ntohs(user_info->udp_head->uh_leng) ) {
                clear_packet_info( user_info );
                return 1;
            }
            user_info->packet_protocol = NETTRAMON_PRO_UDP;
            user_info->payload = (u_char*)(buf + SZ_ETHERNET_H + size_ip + SZ_UDP_H);
            user_info->len = ntohs(user_info->ip_head->ip_len) - (size_ip + SZ_UDP_H);
            check_len -= SZ_UDP_H;
            user_info->len = ntohs(user_info->ip_head->ip_len) - (size_ip + SZ_UDP_H);
            if ( check_len != user_info->len ) {
                clear_packet_info( user_info );
                return 1;
            }
            user_info->tcp_head = NULL;
            user_info->icmp_head = NULL;
            return 0;
        case IPPROTO_ICMP:
            if ( check_len < SZ_ICMP_H ) {
                clear_packet_info( user_info );
                return 1;
            }
            user_info->icmp_head = (ntm_icmp*)(buf + SZ_ETHERNET_H + size_ip);
            user_info->packet_protocol = NETTRAMON_PRO_ICMP; 
            user_info->payload = (u_char*)(buf + SZ_ETHERNET_H + size_ip + SZ_ICMP_H);
            user_info->len = ntohs(user_info->ip_head->ip_len) - (size_ip + SZ_ICMP_H);
            check_len -= SZ_ICMP_H;
            if ( check_len != user_info->len ) {
                clear_packet_info( user_info );
                return 1;
            }
            user_info->tcp_head = NULL;
            user_info->udp_head = NULL;
            return 0;
        default:
            clear_packet_info( user_info );
            return 1;
    }

}
