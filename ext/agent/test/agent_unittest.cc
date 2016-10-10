/*
 *  De-coupled Information Flow Tracking (DIFT) test cases
 *
 *  Copyright (c) 2016 JuiChien, Jao 
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

#include <gmock/gmock.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <set>
#include <inttypes.h>

struct dift_context;
struct hook_functions {
    virtual ~hook_functions() {};

};

struct mock_functions : hook_functions {

} *mock_ptr;

extern "C" {
#include "../agent.c"

}

#define GEN_MOCK_OBJECT( x )  mock_functions x; mock_ptr = &x;
#define GEN_NICEMOCK_OBJECT( x )  NiceMock<mock_functions> x; mock_ptr = &x;

