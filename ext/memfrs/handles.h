/*
 *  MBA Virtual Machine Memory Introspection header
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

typedef enum PARSING_HANDLE_TYPE{
    PARSING_HANDLE_TYPE_ALL,
    PARSING_HANDLE_TYPE_CR3,
    PARSING_HANDLE_TYPE_EPROCESS,
    PARSING_HANDLE_TYPE_PID,
    PARSING_HANDLE_TYPE_TYPE,
    PARSING_HANDLE_TYPE_FULL_DETAIL,
    PARSING_HANDLE_TYPE_DETAIL
}PARSING_HANDLE_TYPE;

extern PARSING_HANDLE_TYPE parsing_handle_type;
