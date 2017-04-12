/*
 *  MBA Mmemory Forensic unit testing
 *
 *  Copyright (c) 2016 Chong-kuan, Chen
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

#include "test.h"
typedef unsigned long target_ulong;

struct hook_func {

    virtual ~hook_func() {};

    // library API
    virtual void target_disas_inst_count(FILE *out, CPUX86State *env, target_ulong code,
		    target_ulong size, int num_inst, int flags) = 0; 

    // obhook functions/API
};

struct mock_func : hook_func {
    public:
        MOCK_METHOD6( target_disas_inst_count, void(FILE *, CPUX86State *, target_ulong , target_ulong , int , int ));
} *mock_ptr;

#define GEN_MOCK_OBJECT(x)  mock_func x; mock_ptr = &x;
#define target_disas_inst_count(arg1, arg2, arg3, arg4, arg5, arg6)    mock_ptr->target_disas_inst_count(arg1, arg2, arg3, arg4, arg5, arg6) 

extern "C" {
#include "../tracer.c"
}

#undef target_disas_inst_count

using namespace ::testing;

class TracerContextInitialized : public Test {
protected:
    virtual void SetUp() {
        GEN_MOCK_OBJECT( mock );
        tracer_ctx.serial_num = 0;
        tracer_ctx.process_tracer_head = NULL;
        tracer_ctx.universal_kernel_tracer_head = NULL;
        tracer_ctx.universal_tracer_head = NULL;
        tracer_ctx.universal_btracer_head = NULL;
        tracer_ctx.process_btracer_head = NULL;
        tracer_ctx.universal_kernel_btracer_head = NULL;

        //tracer_ctx.rwlock = PTHREAD_RWLOCK_INITIALIZER;
    }
    virtual void TearDown() {
    
    }
};


TEST( TRACER_NOTEST, NormalCondition ) {
   
}

class TRACER_INST_TRACER : public TracerContextInitialized{}; 
TEST( TRACER_INST_TRACER, ADD_PROC_TRACER ){
    int ret = tracer_add_inst_tracer( 0x331d0000, "test", false, NULL);
    ASSERT_EQ( ret, 1);
    //tracer_list_callback();
    ASSERT_FALSE( tracer_ctx.process_tracer_head == NULL );
    ASSERT_TRUE( tracer_ctx.universal_kernel_tracer_head == NULL);
    ASSERT_TRUE( tracer_ctx.universal_tracer_head == NULL );
   
    ASSERT_TRUE( tracer_ctx.universal_btracer_head == NULL );
    ASSERT_TRUE( tracer_ctx.process_btracer_head == NULL );
    ASSERT_TRUE( tracer_ctx.universal_kernel_btracer_head == NULL);      

    tracer_clean_up();
}

TEST( TRACER_INST_TRACER, ADD_PROC_TRACER2 ){
    int ret = tracer_add_inst_tracer( 0x331d0000, "test", true, NULL);
    ASSERT_EQ( ret, 1);
    //tracer_list_callback();
    ASSERT_FALSE( tracer_ctx.process_tracer_head == NULL );
    ASSERT_TRUE( tracer_ctx.universal_kernel_tracer_head == NULL);
    ASSERT_TRUE( tracer_ctx.universal_tracer_head == NULL );

    ASSERT_TRUE( tracer_ctx.universal_btracer_head == NULL );
    ASSERT_TRUE( tracer_ctx.process_btracer_head == NULL );
    ASSERT_TRUE( tracer_ctx.universal_kernel_btracer_head == NULL);

    tracer_clean_up();
}


TEST( TRACER_INST_TRACER, ADD_UNIV_TRACER ){
    int ret = tracer_add_inst_tracer( 0, "test", false, NULL);
    ASSERT_EQ( ret, 1);
    //tracer_list_callback();
    ASSERT_TRUE( tracer_ctx.process_tracer_head == NULL );
    ASSERT_TRUE( tracer_ctx.universal_kernel_tracer_head == NULL);
    ASSERT_TRUE( tracer_ctx.universal_tracer_head != NULL );

    ASSERT_TRUE( tracer_ctx.universal_btracer_head == NULL );
    ASSERT_TRUE( tracer_ctx.process_btracer_head == NULL );
    ASSERT_TRUE( tracer_ctx.universal_kernel_btracer_head == NULL);

    tracer_clean_up();
}

TEST( TRACER_INST_TRACER, ADD_NERNEL_UNIV_TRACER ){
    int ret = tracer_add_inst_tracer( 0, "test", true, NULL);
    ASSERT_EQ( ret, 1);
    //tracer_list_callback();
    ASSERT_TRUE( tracer_ctx.process_tracer_head == NULL );
    ASSERT_TRUE( tracer_ctx.universal_kernel_tracer_head != NULL);
    ASSERT_TRUE( tracer_ctx.universal_tracer_head == NULL );

    ASSERT_TRUE( tracer_ctx.universal_btracer_head == NULL );
    ASSERT_TRUE( tracer_ctx.process_btracer_head == NULL );
    ASSERT_TRUE( tracer_ctx.universal_kernel_btracer_head == NULL);
  
    tracer_clean_up();
}

TEST( TRACER_INST_TRACER, ADD_MAXMUN_TRACER ){
    int ret;
    for(int i = 0; i< UINT16_MAX ; i++)
    {
        ret = tracer_add_inst_tracer( 0, "test", true, NULL);
        ASSERT_EQ( ret, i+1);
        //printf("%d\n", i);
    }
    ret = tracer_add_inst_tracer( 0, "test", true, NULL);
    ASSERT_EQ(ret, -1);
    ASSERT_EQ(g_error_no, TRACER_MAX_TRACER_ID);

    tracer_clean_up();
}

TEST( TRACER_INST_TRACER, ENABLE_TRACER ){
    int ret = tracer_add_inst_tracer( 0, "test", true, NULL);
    int status;    

    ASSERT_TRUE( ret != NULL);

    status = tracer_get_tracer_status(ret);
    ASSERT_EQ(status, 0);

    tracer_enable_tracer(ret);
    status = tracer_get_tracer_status(ret);
    ASSERT_EQ(status, 1);

    tracer_clean_up();
}

TEST( TRACER_INST_TRACER, DISABLE_TRACER ){
    int ret = tracer_add_inst_tracer( 0, "test", true, NULL);
    int status;

    ASSERT_TRUE( ret != NULL);

    status = tracer_get_tracer_status(ret);
    ASSERT_EQ(status, 0);

    tracer_enable_tracer(ret);
    status = tracer_get_tracer_status(ret);
    ASSERT_EQ(status, 1);

    tracer_disable_tracer(ret);
    status = tracer_get_tracer_status(ret);
    ASSERT_EQ(status, 0);

    tracer_clean_up();
}

TEST( TRACER_INST_TRACER, GET_TRACER_LABEL ){
    const char* label_string = "test"; 
    int ret = tracer_add_inst_tracer( 0, label_string, true, NULL);
    char* label = tracer_get_tracer_label(ret);
    ASSERT_EQ( strcmp(label_string, label) , 0);

    tracer_clean_up();
}

TEST( TRACER_INST_TRACER, LONG_TRACER_LABEL ){
    const char* label_string = "testtesttestetsetestestestsetsestsetsestestsestsestestsetsest";
    int ret = tracer_add_inst_tracer( 0, label_string, true, NULL);
    char* label = tracer_get_tracer_label(ret);
    ASSERT_NE( strcmp(label_string, label) , 0);
    tracer_clean_up();
}

/****************************************************************************/
class TRACER_BLOCK_TRACER : public TracerContextInitialized{};
TEST( TRACER_BLOCK_TRACER, ADD_BLOCK_TRACER ){
    int ret = tracer_add_block_tracer( 0x331d0000, "test", false, NULL);
    ASSERT_EQ( ret, 1);
    //tracer_list_callback();
    ASSERT_TRUE( tracer_ctx.process_tracer_head == NULL );
    ASSERT_TRUE( tracer_ctx.universal_kernel_tracer_head == NULL);
    ASSERT_TRUE( tracer_ctx.universal_tracer_head == NULL );

    ASSERT_TRUE( tracer_ctx.process_btracer_head != NULL );
    ASSERT_TRUE( tracer_ctx.universal_btracer_head == NULL );
    ASSERT_TRUE( tracer_ctx.universal_kernel_btracer_head == NULL);

    tracer_clean_up();
}

TEST( TRACER_BLOCK_TRACER, ADD_BLOCK_TRACER2 ){
    int ret = tracer_add_block_tracer( 0x331d0000, "test", true, NULL);
    ASSERT_EQ( ret, 1);
    //tracer_list_callback();
    ASSERT_TRUE( tracer_ctx.process_tracer_head == NULL );
    ASSERT_TRUE( tracer_ctx.universal_kernel_tracer_head == NULL);
    ASSERT_TRUE( tracer_ctx.universal_tracer_head == NULL );

    ASSERT_TRUE( tracer_ctx.process_btracer_head != NULL );
    ASSERT_TRUE( tracer_ctx.universal_btracer_head == NULL );
    ASSERT_TRUE( tracer_ctx.universal_kernel_btracer_head == NULL);

    tracer_clean_up();
}


TEST( TRACER_BLOCK_TRACER, ADD_UNIV_TRACER ){
    int ret = tracer_add_block_tracer( 0, "test", false, NULL);
    ASSERT_EQ( ret, 1);
    //tracer_list_callback();
    ASSERT_TRUE( tracer_ctx.process_tracer_head == NULL );
    ASSERT_TRUE( tracer_ctx.universal_kernel_tracer_head == NULL);
    ASSERT_TRUE( tracer_ctx.universal_tracer_head == NULL );

    ASSERT_TRUE( tracer_ctx.process_btracer_head == NULL );
    ASSERT_TRUE( tracer_ctx.universal_btracer_head != NULL );
    ASSERT_TRUE( tracer_ctx.universal_kernel_btracer_head == NULL);

    tracer_clean_up();
}

TEST( TRACER_BLOCK_TRACER, ADD_NERNEL_UNIV_TRACER ){
    int ret = tracer_add_block_tracer( 0, "test", true, NULL);
    ASSERT_EQ( ret, 1);
    //tracer_list_callback();
    ASSERT_TRUE( tracer_ctx.process_tracer_head == NULL );
    ASSERT_TRUE( tracer_ctx.universal_kernel_tracer_head == NULL);
    ASSERT_TRUE( tracer_ctx.universal_tracer_head == NULL );

    ASSERT_TRUE( tracer_ctx.universal_btracer_head == NULL );
    ASSERT_TRUE( tracer_ctx.process_btracer_head == NULL );
    ASSERT_TRUE( tracer_ctx.universal_kernel_btracer_head != NULL);

    tracer_clean_up();
}

