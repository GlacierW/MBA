/*
 *  Tracer abilities of DBA
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
#ifndef __DBA_TRACER_H__
#define __DBA_TRACER_H__

#include "dba.h"

#define DBA_CALL_BACK_TAG    "DBA_TASK"
#define DBA_MAX_SZ_IMG_NAME  14         // The 15th character is '\0'

/// Get Kernel Base Address of guest
///
///     \param None
/// 
/// Return a non-zero value for success, 0 for fail 
extern target_ulong get_kernel_address_base( void );

/// Get Kernel Variable of guest 
/// 
///     \param  var_name        variable name
///     \param  out             address of kernel variable 
///
/// Return 0 for success, -1 for fail
extern int get_gvar_addr( const char *var_name, target_ulong *out);

/// Set up hook on MmCreatePeb to get the information of sample
/// 
///     \param  ctx             pointer to dba_context of task
///
/// Return 0 for success, -1 for fail
extern int set_obhook_on_mmcreatepeb( dba_context* ctx );

#endif
