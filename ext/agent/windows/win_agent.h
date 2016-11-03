/*
 * Windows in-VM agent of MBA header
 * This program is compiled under TDM-GCC 4.9.2 64 bit with the compile flag "-static-libgcc -lws2_32"
 * The IDE used is Orwell Dev-C++ 5.1.1
 *
 *  Copyright (c)   2016 E-lin Ho
 *                  2016 Chiawei Wang
 *                  2016 Juichien Jao
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

#ifndef __WIN_AGENT_H__
#define __WIN_AGENT_H__

#define DEFAULT_PORT         "8888"
#define DEFAULT_LOG_NAME     "agent_log.txt"
#define DEFAULT_SECTOR_SIZE  512

#define SZ_MAX_CMD  256          // max command size
#define SZ_MAX_LOG  256          // log message buffer size
#define SZ_MAX_L    256          // log message buffer size
#define SZ_MAX_FILECHUNK 8192    // maximum import/export file chunk

#define MSG_EXEC_READY  "EXEC_READY"
#define MSG_ACK_PREFIX  "System Receive : "
#define MSG_REC_SUCCESS "SUCCESS"
#define MSG_REC_FAIL    "CMDFAIL"

enum cmd_type {
    MBA_CMD_UNKNOWN,    // 0 : Unknow command from MBA
    MBA_CMD_EXEC,       // 1 : Command begging with 'exec' from MBA     // execute
    MBA_CMD_INVO,       // 2 : Command begging with 'invo' from MBA     // invoke
    MBA_CMD_EXPO,       // 3 : Command begging with 'expo' from MBA     // export
    MBA_CMD_IMPO,       // 4 : Command begging with 'impo' from MBA     // import
    MBA_CMD_LOGF,        // 5 : Command begging with 'logf' from MBA     // log file
    MBA_CMD_SYNC        // 6 : Command begging with 'sync' from MBA     // sync
};

//common return value
enum MBA_AGENT_RETURN{
    AGENT_RET_SUCCESS, 
    AGENT_RET_FAIL
};

typedef enum MBA_AGENT_RETURN MBA_AGENT_RETURN;

#endif

