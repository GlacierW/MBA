/*
 *
 *  Windows in-VM network traffic monitor implementation
 *  The used structs are referenced from http://www.tcpdump.org/sniffex.c
 *
 *  Copyright (c)    2017 JuiChien Jao
 *                   2016 ChiaWei Wang
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
#ifndef __NETTRAMON_H__
#define __NETTRAMON_H__

#include <netinet/in.h>
#include <arpa/inet.h>
#include <monitor/monitor.h>
#include "utlist.h"

#define ETHER_ADDR_LEN           6
#define SZ_NETTRAMON_PATH_LENGTH 64
#define SZ_NETTRAMON_MAX_CB      65535
#define SZ_ETHERNET_H            sizeof(struct sniff_ethernet) 
#define SZ_IP_H                  sizeof(struct sniff_ip)
#define SZ_TCP_H                 sizeof(struct sniff_tcp)
#define SZ_UDP_H                 sizeof(struct sniff_udp)
#define SZ_ICMP_H                sizeof(struct sniff_icmp)

typedef uint64_t haddr_t;

/* Protocol Types */
enum MBA_NETTRAMON_PRO {
    NETTRAMON_PRO_UNKNOWN,
    NETTRAMON_PRO_TCP,
    NETTRAMON_PRO_UDP,
    NETTRAMON_PRO_ICMP,
};
typedef enum MBA_NETTRAMON_PRO MBA_NETTRAMON_PRO;

/* Ethernet header */
struct sniff_ethernet {
    u_char  ether_dhost[ETHER_ADDR_LEN];    /* destination host address */
    u_char  ether_shost[ETHER_ADDR_LEN];    /* source host address */
    u_short ether_type;                     /* IP, ARP, RARP,.etc */
};

/* IP header */
struct sniff_ip {
    u_char  ip_vhl;                 /* version << 4 | header length >> 2 */
    u_char  ip_tos;                 /* type of service */
    u_short ip_len;                 /* total length */
    u_short ip_id;                  /* identification */
    u_short ip_off;                 /* fragment offset field */
    u_char  ip_ttl;                 /* time to live */
    u_char  ip_p;                   /* protocol */
    u_short ip_sum;                 /* checksum */
    struct  in_addr ip_src,ip_dst;  /* source and dest address */
};
#define IP_HL(ip)               (((ip)->ip_vhl) & 0x0f)
#define IP_V(ip)                (((ip)->ip_vhl) >> 4)
#define IP_RF 0x8000                /* reserved fragment flag */
#define IP_DF 0x4000                /* dont fragment flag */
#define IP_MF 0x2000                /* more fragments flag */
#define IP_OFFMASK 0x1fff           /* mask for fragmenting bits */

/* TCP header */
struct sniff_tcp {
    u_short th_sport;               /* source port */
    u_short th_dport;               /* destination port */
    u_int   th_seq;                 /* sequence number */
    u_int   th_ack;                 /* acknowledgement number */
    u_char  th_offx2;               /* data offset, rsvd */
    u_char  th_flags;
    u_short th_win;                 /* window */
    u_short th_sum;                 /* checksum */
    u_short th_urp;                 /* urgent pointer */
};
#define TH_OFF(th)      (((th)->th_offx2 & 0xf0) >> 4)
#define TH_FIN          0x01
#define TH_SYN          0x02
#define TH_RST          0x04
#define TH_PUSH         0x08
#define TH_ACK          0x10
#define TH_URG          0x20
#define TH_ECE          0x40
#define TH_CWR          0x80
#define TH_FLAGS        (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)

/* UDP header */
struct sniff_udp {
    u_short uh_sport;               /* source port */
    u_short uh_dport;               /* destination port */
    u_short uh_leng;                /* length */
    u_short uh_sum;                 /* checksum */
};

/* ICMP header */
struct sniff_icmp {
    u_char  ih_type;                /* ICMP type */
    u_char  ih_code;                /* ICMP subtype */
    u_short ih_sum;                 /* checksum */
    u_int   ih_rest;                /* Rest of Header */
};

/* Packet Information Structure */
struct packet_info {
    MBA_NETTRAMON_PRO       packet_protocol;
    struct sniff_ethernet*  eth_head;
    struct sniff_ip*        ip_head;
    struct sniff_tcp*       tcp_head;
    struct sniff_udp*       udp_head;
    struct sniff_icmp*      icmp_head;
    u_char*                 payload;
    size_t                  len;
};

struct file_ptr {
    FILE* all_file;
    FILE* tcp_file;
    FILE* udp_file;
    FILE* icmp_file;
};

struct path_buffer {
    bool has_set;
    char all_file_path_buf  [SZ_NETTRAMON_PATH_LENGTH];
    char tcp_file_path_buf  [SZ_NETTRAMON_PATH_LENGTH];
    char udp_file_path_buf  [SZ_NETTRAMON_PATH_LENGTH];
    char icmp_file_path_buf [SZ_NETTRAMON_PATH_LENGTH];
};

/* call back record structure */
struct nettramon_cb {
    // unique identifier for each call back function
    uint16_t uid;
    // tell if the call back should be called
    bool enabled;
    // call back function pointer
    void*(*user_cb) (size_t, haddr_t, void*);
    // argument for call back function
    void* cb_arg;
    // pointer to the next call back function
    struct nettramon_cb* next;
};

typedef struct sniff_ethernet  ntm_ethernet;
typedef struct sniff_ip        ntm_ip;
typedef struct sniff_tcp       ntm_tcp;
typedef struct sniff_udp       ntm_udp;
typedef struct sniff_icmp      ntm_icmp;
typedef struct packet_info     packet_info;
typedef struct file_ptr        file_ptr;
typedef struct path_buffer     path_buffer;
typedef struct nettramon_cb    nettramon_cb;

struct ntm_context {
    // The handled packet's protocol
    MBA_NETTRAMON_PRO   protocol_type;
    // QEMU's monitor pointer
    Monitor*            local_mon;
    // compiled packet filter
    struct bpf_program* filter;
    // nettramon's status
    bool                status;
    // file's pointer for writing
    file_ptr            fp[1];
    // file path's buffer for setting file pointer
    path_buffer         pb[1];
    // call back function list
    nettramon_cb*       cb_list;
    // call back functoin array
    nettramon_cb*       cb_arr[SZ_NETTRAMON_MAX_CB];
    // read-write lock to protect the internal data
    pthread_rwlock_t    rwlock;
};
typedef struct ntm_context ntm_context;


/* ---- Public APIs ---- */

/* Check if the network traffic monitor is active */
/* Return a boolean value as true if it's active, or false if it's not active */
bool         nettramon_is_active                 ( void );

/*------ The following APIs are for the call back function opertions ------*/
/* Set a call back function */
/* Return the uid of the registered call back function or -1 for error */
int          nettramon_set_cb                    ( void*(*)( size_t, haddr_t, void* ), void* );

/* Delete the call back function with the input uid */
/* Return 0 for success or 1 for fail */
int          nettramon_delete_cb                 ( int );

/* Enable the call back function with the input uid */
/* Return 0 for success or 1 for fail */
int          nettramon_enable_cb                 ( int );

/* Disable the call back function with the input uid */
/* Return 0 for success or 1 for fail */
int          nettramon_disable_cb                ( int );
/*------ The upper APIs are for the call back function opertions ------*/

/* Main function for network device to tranmit captured packets */
/* Should not be called directly */
int          nettramon_packet_capture            ( const u_char*, size_t, haddr_t );

/*------ The following APIs are for the nettramon module operations ------*/
/* Start to capture packets, and argument 'mon' should be NULL */
/* Return 0 for success, 1 for outputing to files, or 2 for fail */ 
unsigned int nettramon_start                     ( Monitor* );

/* Stop capturing packets */
/* Return 0 for success, or 1 for fail */
unsigned int nettramon_stop                      ( void );

/* Set the file for outputing the parsed packets' contents according to protocols*/
/* The arguments are the paths of files according to protocols in the sequence: all protocols, TCP, UDP, ICMP, NULL for no setting */
/* Return 0 for suceess, or 1 for fail */
unsigned int nettramon_set_file_path             ( const char*, const char*, const char*, const char* );

/* Clear the set file paths */
/* Return 0 for suceess, or 1 for fail */
unsigned int nettramon_reset_file_path           ( void );

/* Set the packet filter for capturing */
/* Return 0 for suceess, or 1 for fail */
unsigned int nettramon_set_filter                ( const char* );

/* Clear the set filter */
/* Return 0 for suceess, or 1 for fail */
unsigned int nettramon_reset_filter              ( void );
/*------ The upper APIs are for the nettramon module operations ------*/

/* Parse the input u_char array's content as a packet */
/* Argument packet_info* would be set with pointers of all parts in the u_char array if parsing successfully */
/* Return 0 for suceess, or 1 for fail */
int          nettramon_packet_parse              ( const u_char*, size_t, packet_info* );

#endif
