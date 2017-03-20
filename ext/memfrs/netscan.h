/*
 *  MBA Network scanning Introspection Header File
 *
 *  Copyright (c)   2016 ELin Ho
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

#include "qom/cpu.h"
#define MAXMEM 0x80000000

#define POOL_TAG_UDP_ENDPOINT "UdpA"
#define POOL_TAG_TCP_ENDPOINT "TcpE"
#define POOL_TAG_TCP_LISTENER "TcpL"

typedef enum TCP_STATE{
    TCP_CLOSED,
    TCP_LISTENING,
    TCP_SYN_SENT,
    TCP_SYN_RCVD,
    TCP_ESTABLISHED,
    TCP_FIN_WAIT1,
    TCP_FIN_WAIT2,
    TCP_CLOSE_WAIT,
    TCP_CLOSING,
    TCP_LAST_ACK,
    TCP_NO_STATE_10,
    TCP_NO_STATE_11,
    TCP_TIME_WAIT,
    TCP_DELETE_TCP
}TCP_STATE;

extern TCP_STATE tcp_state;


#define AF_INET 2
#define AF_INET6 23

