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

#define ETHER_ADDR_LEN           6
#define SZ_NETTRAMON_PATH_LENGTH 64
#define SZ_ETHERNET_H            sizeof(struct sniff_ethernet) 
#define SZ_IP_H                  sizeof(struct sniff_ip)
#define SZ_TCP_H                 sizeof(struct sniff_tcp)
#define SZ_UDP_H                 sizeof(struct sniff_udp)
#define SZ_ICMP_H                sizeof(struct sniff_icmp)

// Protocol Types
enum MBA_NETTRAMON_PRO {
    NETTRAMON_PRO_TCP,
    NETTRAMON_PRO_UDP,
    NETTRAMON_PRO_ICMP,
    NETTRAMON_PRO_UNKNOWN,
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

typedef struct sniff_ethernet  ntm_ethernet;
typedef struct sniff_ip        ntm_ip;
typedef struct sniff_tcp       ntm_tcp;
typedef struct sniff_udp       ntm_udp;
typedef struct sniff_icmp      ntm_icmp;
typedef struct packet_info     packet_info;

/* ---- Public APIs ---- */
bool         nettramon_is_active                 ( void );
int          nettramon_parse_buffer              ( const u_char* , size_t, void(*)(u_char*, size_t) );
unsigned int nettramon_start                     ( Monitor* mon );
unsigned int nettramon_stop                      ( void );
unsigned int nettramon_set_file_path             ( const char*, const char*, const char*, const char* );
unsigned int nettramon_reset_file_path           ( void );
unsigned int nettramon_set_filter                ( const char* );
unsigned int nettramon_reset_filter              ( void );
struct       packet_info* nettramon_packet_parse ( const u_char*, size_t );

#endif
