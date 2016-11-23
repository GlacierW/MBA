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

// re-define certain header in order to decouple the QEMU dependency
#include "../../../include/utlist.h"
#include "../../../include/uthash.h"

#include "test.h"

typedef uint64_t target_ulong;

struct hook_func {

    virtual ~hook_func() {};

    // library API
    virtual int cpu_memory_rw_debug(void *cpu, target_ulong addr,
                uint8_t *buf, int len, int is_write) = 0;
    virtual void cpu_physical_memory_read(hwaddr addr,
                void *buf, int len) = 0;
   

    // obhook functions/API
};

struct mock_func : hook_func {
    public:
        MOCK_METHOD3( cpu_physical_memory_read, void(hwaddr, void *, int));
        MOCK_METHOD5( cpu_memory_rw_debug, int(void *, target_ulong, uint8_t *, int, int));
} *mock_ptr;

#define GEN_MOCK_OBJECT(x)  mock_func x; mock_ptr = &x;

// mock the functions in the original code
#define cpu_memory_rw_debug(arg1, arg2, arg3, arg4, arg5)      mock_ptr->cpu_memory_rw_debug(arg1, arg2, arg3, arg4, arg5)
#define cpu_physical_memory_read(arg1, arg2, arg3)     mock_ptr->cpu_physical_memory_read(arg1, arg2, arg3)

#define monitor_printf(mon, fmt, ...)    printf( fmt, ##__VA_ARGS__)

extern "C" {
#include "../memfrs.c"
#include "../kernel.c"
#include "../kmod.c"
#include "../vad.c"
}

#undef cpu_memory_rw_debug
#undef cpu_physical_memory_read

using namespace ::testing;

class MemfrsContextInitialized : public Test {
protected:
    virtual void SetUp() {
        GEN_MOCK_OBJECT( mock );
        memfrs_load_structs("type_definition.json");
        memfrs_load_globalvar("global_variable.json");
    }
    virtual void TearDown() {
    }
};

TEST( MEMFRS_NOTEST, NormalCondition ) {
   
}

ACTION_P(StrCpyToArg2, str) { strcpy(reinterpret_cast<char *>(arg2) , str); } 

TEST( MEMFRS_VIRMEMTEST, GetVirMemContent) {
    //const uint8_t* a = "\xde\xad\xbe\xef"; 
    CPUState cpu;
    uint8_t b[100]; 

    GEN_MOCK_OBJECT( mock );   
    EXPECT_CALL( mock, cpu_memory_rw_debug(_,_,_,_,_) ).WillOnce( DoAll( StrCpyToArg2("hello"), Return(0) ));
    memfrs_get_virmem_content(&cpu, 0x1234, 0x1234, 4, b); 
    ASSERT_EQ( 0, memcmp(b, "hello", 6));
    //ASSERT_EQ( 1, memfrs_get_virmem_content(&cpu, 0x1234, 0x1234, 4, b));
} 


ACTION_P(VirMemReadWithIdx, str) { strcpy(reinterpret_cast<char *>(arg2) , str+arg1); }

TEST( MEMFRS_VIRMEMTEST, VirMemScan) {
    CPUState cpu;
    const char* mem = "abcabcabcabc";
    //uint8_t b[100];
    //uint64_t *p;

    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL( mock, cpu_memory_rw_debug(_,_,_,_,_) ).WillRepeatedly( DoAll( VirMemReadWithIdx( mem ), Return(0) ));
    UT_array *match_addr = memfrs_scan_virmem(&cpu, 0x0, strlen(mem), "bca", strlen("bca"));

    ASSERT_EQ( 1, *(utarray_eltptr( match_addr, 0)));
    ASSERT_EQ( 4, *(utarray_eltptr( match_addr, 1)));
    ASSERT_EQ( 7, *(utarray_eltptr( match_addr, 2)));
}

/*=======================================================================================*/

ACTION_P(PhyMemReadWithIdx, str) { strcpy(reinterpret_cast<char *>(arg1) , str+arg0); }

TEST( MEMFRS_PHYMEMTEST, PhyMemScan) {
    //CPUState cpu;
    const char* mem = "abcabcabcabc";
    //uint8_t b[100];
    //uint64_t *p;

    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL( mock, cpu_physical_memory_read(_,_,_) ).WillRepeatedly( PhyMemReadWithIdx( mem  ));
    UT_array *match_addr = memfrs_scan_phymem(0x0, strlen(mem), "bca", strlen("bca"));
    
    ASSERT_EQ( 1, *(utarray_eltptr( match_addr, 0)));
    ASSERT_EQ( 4, *(utarray_eltptr( match_addr, 1)));
    ASSERT_EQ( 7, *(utarray_eltptr( match_addr, 2)));
}

/*=======================================================================================*/

int addFieldToStruct(json_object *parent_obj, char* field_name, char* field_type, int offset, int size, bool is_pointer)
{
    json_object *field_info;
    field_info = json_object_new_array();

    json_object_array_add( field_info, json_object_new_string(field_type));
    json_object_array_add( field_info, json_object_new_int(offset));
    json_object_array_add( field_info, json_object_new_int(size));
    json_object_array_add( field_info, json_object_new_boolean(is_pointer));

    json_object_object_add( parent_obj, field_name, field_info);

    return 0;
}


TEST( MEMFRS_DATASTRUCT, Q_Struct) {
    json_object *tyde_definition, *kpcr;

    tyde_definition = json_object_new_object();
    
    // =========== _KPCR =================================
    kpcr = json_object_new_object();

    //int addFieldToStruct(json_object *parent_obj, char* member_name, char* member_type, int offset, int size, bool is_pointer)
    //addFieldToStruct(kpcr, "Self", "_KPCR", 0x18, 8, true);
    //addFieldToStruct(kpcr, "Prcb", "_KPRCB", 0x180, 0x6900, false);
    json_object_object_add(tyde_definition, "_KPCR", kpcr);

    g_struct_info = tyde_definition;

    ASSERT_EQ( kpcr, memfrs_q_struct("_KPCR"));   
}

TEST( MEMFRS_DATASTRUCT, Q_Field) {
    json_object *tyde_definition, *kpcr;
    json_object *obj;
    field_info* field;

    tyde_definition = json_object_new_object();
    
    // =========== _KPCR =================================
    kpcr = json_object_new_object();

    //int addFieldToStruct(json_object *parent_obj, char* member_name, char* member_type, int offset, int size, bool is_pointer)
    addFieldToStruct(kpcr, (char*)"Self", (char*)"_KPCR", 0x18, 8, true);
    addFieldToStruct(kpcr, (char*)"Prcb", (char*)"_KPRCB", 0x180, 0x6900, false);
    json_object_object_add(tyde_definition, "_KPCR", kpcr);

    g_struct_info = tyde_definition;
    obj = memfrs_q_struct("_KPCR");
    ASSERT_EQ( kpcr, obj);

    field = memfrs_q_field(obj, "Self");
    ASSERT_EQ( field->offset, 0x18 );
    ASSERT_EQ( 0, strcmp(field->type_name, "_KPCR"));     
    ASSERT_EQ( field->jobject_type, kpcr); 
}

/* Un-implement feature
TEST( MEMFRS_DATASTRUCT, Q_Field_TypeSize) {
    json_object *tyde_definition, *kpcr;
    json_object *obj;
    field_info* field;

    tyde_definition = json_object_new_object();

    // =========== _KPCR =================================
    kpcr = json_object_new_object();

    //int addFieldToStruct(json_object *parent_obj, char* member_name, char* member_type, int offset, int size, bool is_pointer)
    addFieldToStruct(kpcr, "Self", "_KPCR", 0x18, 8, true);
    addFieldToStruct(kpcr, "Prcb", "_KPRCB", 0x180, 0x6900, false);
    json_object_object_add(tyde_definition, "_KPCR", kpcr);
    printf("%p\n", kpcr);

    g_struct_info = tyde_definition;
    obj = memfrs_q_struct("_KPCR");
    ASSERT_EQ( kpcr, obj);

    field = memfrs_q_field(obj, "Self");
    //ASSERT_EQ( field->offset, 0x18 );
    ASSERT_EQ( field->type_size, 8 ); 
    //ASSERT_EQ( 0, strcmp(field->type_name, "_KPCR"));    
}*/

/*    */
//TEST( MEMFRS_PROCLIST, )

ACTION_P( MEMREADKPCR, kpcr) { *(uint64_t*)arg2 = kpcr; }

class MEMFRS_PROCLIST : public MemfrsContextInitialized{}; 
TEST_F( MEMFRS_PROCLIST, KPCR_SELF_CHECK) {
    GEN_MOCK_OBJECT( mock );
    uint64_t kpcr_ptr = 0xdeadbeef;
    bool kpcr_check;
    EXPECT_CALL( mock, cpu_memory_rw_debug(_,_,_,_,_) ).WillOnce( DoAll( MEMREADKPCR(kpcr_ptr), Return( 0 ) ));
    kpcr_check = memfrs_kpcr_self_check(kpcr_ptr);
    ASSERT_EQ(true, kpcr_check);
    ASSERT_EQ( g_kpcr_ptr, kpcr_ptr );
    json_object_put(g_struct_info);
    json_object_put(g_globalvar_info);
}

TEST_F( MEMFRS_PROCLIST, KPCR_SELF_CHECK_FAIL) {
    GEN_MOCK_OBJECT( mock );
    uint64_t kpcr_ptr = 0xdeadbeef;
    bool kpcr_check;
    EXPECT_CALL( mock, cpu_memory_rw_debug(_,_,_,_,_) ).WillOnce( DoAll( MEMREADKPCR(0), Return( 0 ) ));
    kpcr_check = memfrs_kpcr_self_check(kpcr_ptr);
    ASSERT_EQ(false, kpcr_check);
    ASSERT_EQ( g_kpcr_ptr, 0 );
    json_object_put(g_struct_info);
    json_object_put(g_globalvar_info);
}

TEST_F( MEMFRS_PROCLIST, ProcessList) {
    CPUState cpu;
    uint64_t kpcr_ptr =0xc0fe;
    int ret;
    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL( mock, cpu_memory_rw_debug(_,_,_,_,_) ).Times(5).
                  WillOnce(Return(0)).
                  WillOnce( DoAll( MEMREADKPCR(0x0), Return(0))).
                  WillOnce(Return(0)).
                  WillOnce(Return(0)).
                  WillOnce( DoAll( MEMREADKPCR(0x2f0), Return(0)));
    ret = memfrs_enum_proc_list( kpcr_ptr, &cpu);
    json_object_put(g_struct_info);
    json_object_put(g_globalvar_info);
    ASSERT_EQ(ret, 0);
}

TEST( MEMFRS_PROCLIST_NO_INIT, ProcessList_No_StructInfo) {
    CPUState cpu;
    uint64_t kpcr_ptr =0xc0fe;
    int ret;
    GEN_MOCK_OBJECT( mock );
    g_struct_info = NULL;

    ret = memfrs_enum_proc_list( kpcr_ptr, &cpu);
    ASSERT_EQ(ret, -1);
}

TEST_F( MEMFRS_PROCLIST, ProcessList_NoKPCR) {
    CPUState cpu;
    uint64_t kpcr_ptr =0;
    int ret;
    GEN_MOCK_OBJECT( mock );
    ret = memfrs_enum_proc_list( kpcr_ptr, &cpu);
    json_object_put(g_struct_info);
    json_object_put(g_globalvar_info);
    ASSERT_EQ(ret, -1);
}

TEST_F( MEMFRS_PROCLIST, ProcessList_NoCPU) {
    uint64_t kpcr_ptr =1234;
    int ret;
    GEN_MOCK_OBJECT( mock );
    ret = memfrs_enum_proc_list( kpcr_ptr, NULL);
    json_object_put(g_struct_info);
    json_object_put(g_globalvar_info);
    ASSERT_EQ(ret, -1);
}

// 
class MEMFRS_GLOBALVAR : public MemfrsContextInitialized{};
TEST_F( MEMFRS_GLOBALVAR, Not_Load_DB) {
    json_object* gvar =NULL;
    json_object_put(g_globalvar_info);
    g_globalvar_info = NULL;
    gvar = memfrs_q_globalvar("NtCreateFile");
    ASSERT_TRUE(gvar == NULL);
    json_object_put(g_struct_info);
} 

TEST_F( MEMFRS_GLOBALVAR, Sym_Not_Found) {
    json_object* gvar =NULL;
    g_globalvar_info = NULL;
    gvar = memfrs_q_globalvar("QQQQQQQQ");
    ASSERT_TRUE(gvar == NULL);
    json_object_put(g_struct_info);
    json_object_put(g_globalvar_info);
}


TEST_F( MEMFRS_GLOBALVAR, Q_Offset_No_Arg) {
    json_object* gvar =NULL;
    int64_t adr = 10;    

    adr = memfrs_gvar_offset(gvar);
    
    json_object_put(g_struct_info);
    json_object_put(g_globalvar_info);

    ASSERT_EQ(adr, -1);
}

TEST_F( MEMFRS_GLOBALVAR, Q_Offset) {
    json_object* gvar =NULL;
    uint64_t adr = 10;

    gvar = memfrs_q_globalvar("NtCreateFile");
    ASSERT_TRUE(gvar != NULL);

    adr = memfrs_gvar_offset(gvar);

    json_object_put(g_struct_info);
    json_object_put(g_globalvar_info);
    ASSERT_NE(adr, -1);
}

//====================Testing for kernel.c ========================/

const char *fake_mem_content = "MZ........RSDS.FileNameInOffset24.ntkrnlpa.pdb";
const char *fake_mem_content2 = "..............................................";

class MEMFRS_KERNEL : public MemfrsContextInitialized{};
ACTION_P( MEMREAD_Kernel, memcontent) { 
    int i = arg1 - INIT_LOADER_MAP_LOW;
    memcpy(reinterpret_cast<char *>(arg2), memcontent+(i % strlen(memcontent)), arg3);
}

TEST_F( MEMFRS_KERNEL, Scan_Kernel ){
    CPUState cpu;
    uint64_t ret;
    GEN_MOCK_OBJECT( mock );

    EXPECT_CALL( mock, cpu_memory_rw_debug(_,_,_,_,_))
               .WillRepeatedly( DoAll( MEMREAD_Kernel( fake_mem_content ), Return(0) ));

    ret = memfrs_find_nt_kernel_base(&cpu);
    ASSERT_EQ(ret , INIT_LOADER_MAP_LOW);
    ASSERT_EQ(nt_kernel_base , INIT_LOADER_MAP_LOW);
 
    ret = memfrs_get_nt_kernel_base();
    ASSERT_EQ(ret , INIT_LOADER_MAP_LOW);
}


TEST_F( MEMFRS_KERNEL, Scan_Kernel_NoFound ){
    CPUState cpu;
    uint64_t ret;
    GEN_MOCK_OBJECT( mock );

    EXPECT_CALL( mock, cpu_memory_rw_debug(_,_,_,_,_))
               .WillRepeatedly( DoAll( MEMREAD_Kernel( fake_mem_content2 ), Return(0) ));
    
  
    ret = memfrs_find_nt_kernel_base(&cpu);
    ASSERT_EQ(ret , 0);
    ASSERT_EQ(nt_kernel_base , 0);

    ret = memfrs_get_nt_kernel_base();
    ASSERT_EQ(ret , 0);
}

TEST_F( MEMFRS_KERNEL, Scan_Kernel_NoStruct ){
    CPUState cpu;
    uint64_t ret;
    GEN_MOCK_OBJECT( mock );

    g_struct_info = NULL;
    EXPECT_CALL( mock, cpu_memory_rw_debug(_,_,_,_,_))
               .WillRepeatedly( DoAll( MEMREAD_Kernel( fake_mem_content ), Return(0) ));

    ret = memfrs_find_nt_kernel_base(&cpu);
    ASSERT_EQ(ret , 0);
    ASSERT_EQ(nt_kernel_base , 0);

    ret = memfrs_get_nt_kernel_base();
    ASSERT_EQ(ret , 0);
}

//================== Testing for kmod.c ===============================//


const char *fake_phymem_content = "MmLd..........";
class MEMFRS_KMOD : public MemfrsContextInitialized{};
ACTION_P( MEMREAD_FakeUstring, name) {
    strcpy(reinterpret_cast<char *>(arg2), name);
}


ACTION_P( PHYREAD_ModuleTag, content) {
    strcpy(reinterpret_cast<char *>(arg1), content);
}

char* pattern = (char*)"\x10\x00\x10\x00\x00\x00\x00\x00\x00\x00";
ACTION_P( PHYREAD_UnicodeString, Ustring) {
    memcpy(reinterpret_cast<char *>(arg1), Ustring, arg2);
}

TEST_F( MEMFRS_KMOD, Scan_Modules ){
    CPUState cpu;
    GEN_MOCK_OBJECT( mock );
    kernel_module* kmod = NULL;
    
    EXPECT_CALL( mock, cpu_memory_rw_debug(_,_,_,_,_))
               .WillOnce( DoAll( MEMREAD_FakeUstring("f.i.l.e.n.a.m.e."), Return(0) ) )
               .WillOnce( DoAll( MEMREAD_FakeUstring("f.i.l.e.n.a.m.e."), Return(0) ) );
 
    EXPECT_CALL( mock, cpu_physical_memory_read(_,_,_))
               .WillOnce( PHYREAD_ModuleTag(fake_phymem_content)) //;//.
               .WillOnce( PHYREAD_UnicodeString(pattern))
               .WillOnce( PHYREAD_UnicodeString(pattern))
               .WillRepeatedly( PHYREAD_ModuleTag(".......................") );

    UT_array *ret = memfrs_scan_module(&cpu);
    int length = utarray_len(ret);
    ASSERT_EQ(length, 1);
  
    kmod = (kernel_module*)utarray_eltptr(ret, 0);
    ASSERT_TRUE(kmod != NULL);
    ASSERT_STREQ( kmod->fullname, "filename" );
    ASSERT_STREQ( kmod->basename, "filename" );
    ASSERT_EQ( kmod->base, 0 );
}


TEST_F( MEMFRS_KMOD, Scan_Modules_No_Tag ){
    CPUState cpu;
    GEN_MOCK_OBJECT( mock );
    //kernel_module* kmod = NULL;

    EXPECT_CALL( mock, cpu_physical_memory_read(_,_,_))
               .WillRepeatedly( PHYREAD_ModuleTag(".......................") );

    UT_array *ret = memfrs_scan_module(&cpu);
    int length = utarray_len(ret);
    ASSERT_EQ(length, 0);
}

TEST_F( MEMFRS_KMOD, Scan_Modules_No_Struct ){
    CPUState cpu;
    GEN_MOCK_OBJECT( mock );
    //kernel_module* kmod = NULL;

    g_struct_info = NULL;
    UT_array *ret = memfrs_scan_module(&cpu);
    ASSERT_TRUE(ret ==  NULL);
}


class MEMFRS_VAD : public MemfrsContextInitialized{};

ACTION_P( MEMREAD_FakeUint8, type) {
    *((uint8_t *)(arg2)) = type;
}

ACTION_P( MEMREAD_FakeUint64, type) {
    *((uint64_t *)(arg2)) = type;
}

TEST_F( MEMFRS_VAD, VAD_PARSE ){
    CPUState cpu;
    vad_node* node =NULL;
    GEN_MOCK_OBJECT( mock );

    EXPECT_CALL( mock, cpu_memory_rw_debug(_,_,_,_,_))
                .WillOnce( DoAll( MEMREAD_FakeUint8(1), Return(0) ) )
                .WillOnce( DoAll( MEMREAD_FakeUint8(2), Return(0) ) )
                .WillOnce( DoAll( MEMREAD_FakeUint8(3), Return(0) ) )
                .WillOnce( DoAll( MEMREAD_FakeUint8(4), Return(0) ) )
                //Fake VAD type
                .WillOnce( DoAll( MEMREAD_FakeUint8(VadImageMap), Return(0) ))  //;
                .WillOnce( Return(0) )
                .WillOnce( Return(0) )
                .WillOnce( DoAll( MEMREAD_FakeUint64(0x1003), Return(0) ) )
                // Fake Unicode String
                .WillOnce( DoAll( MEMREAD_FakeUint8(40), Return(0) ) )
                .WillOnce( DoAll( MEMREAD_FakeUint8(20), Return(0) ) )
                .WillOnce( Return(0) )
                .WillOnce( DoAll( MEMREAD_FakeUstring("f.i.l.e.n.a.m.e."), Return(0) ) );
    node = parse_mmvad_node( 0x000, &cpu);
    ASSERT_EQ( 0, strcmp(node->filename, "filename"));
    //printf("%"PRIx64" %"PRIx64"", node->start_viraddr, node->end_viraddr);
    //printf("%lx %lx", node->start_viraddr, node->end_viraddr);
    //ASSERT_EQ( node->start_viraddr, 0x300000001000 );
    //ASSERT_EQ( node->end_viraddr , 0x400000002fff);
}

TEST_F( MEMFRS_VAD, VAD_PARSE_NO_FILEPOINTER ){
    CPUState cpu;
    vad_node* node =NULL;
    GEN_MOCK_OBJECT( mock );


    EXPECT_CALL( mock, cpu_memory_rw_debug(_,_,_,_,_))
                .WillOnce( Return(0) )
                .WillOnce( Return(0) )
                .WillOnce( Return(0) )
                .WillOnce( Return(0) )
                //Fake VAD type
                .WillOnce( DoAll( MEMREAD_FakeUint8(VadImageMap), Return(0) ))  //;
                .WillOnce( Return(0) )
                .WillOnce( Return(0) )
                .WillOnce( DoAll( MEMREAD_FakeUint64(0x1), Return(0) ) );
   node = parse_mmvad_node( 0x000, &cpu);
   ASSERT_EQ( NULL, node->filename);
}

TEST_F( MEMFRS_VAD, VAD_PARSE_NO_FILELENGTH ){
    CPUState cpu;
    vad_node* node =NULL;
    GEN_MOCK_OBJECT( mock );
    

    EXPECT_CALL( mock, cpu_memory_rw_debug(_,_,_,_,_))
                .WillOnce( Return(0) )
                .WillOnce( Return(0) )
                .WillOnce( Return(0) )
                .WillOnce( Return(0) )
                //Fake VAD type
                .WillOnce( DoAll( MEMREAD_FakeUint8(VadImageMap), Return(0) ))  //;
                .WillOnce( Return(0) )
                .WillOnce( Return(0) )
                .WillOnce( DoAll( MEMREAD_FakeUint64(0x1000), Return(0) ) )
                // Fake Unicode String
                .WillOnce( DoAll( MEMREAD_FakeUint8(0), Return(0) ) )
                .WillOnce( DoAll( MEMREAD_FakeUint8(20), Return(0) ) );
   node = parse_mmvad_node( 0x000, &cpu);
   ASSERT_EQ( NULL, node->filename);
}

TEST_F( MEMFRS_VAD, VAD_PARSE_NO_FILELENGTH2 ){
    CPUState cpu;
    vad_node* node =NULL;
    GEN_MOCK_OBJECT( mock );
    

    EXPECT_CALL( mock, cpu_memory_rw_debug(_,_,_,_,_))
                .WillOnce( Return(0) )
                .WillOnce( Return(0) )
                .WillOnce( Return(0) )
                .WillOnce( Return(0) )
                //Fake VAD type
                .WillOnce( DoAll( MEMREAD_FakeUint8(VadImageMap), Return(0) ))  //;
                .WillOnce( Return(0) )
                .WillOnce( Return(0) )
                .WillOnce( DoAll( MEMREAD_FakeUint64(0x1000), Return(0) ) )
                // Fake Unicode String
                .WillOnce( DoAll( MEMREAD_FakeUint8(20), Return(0) ) )
                .WillOnce( DoAll( MEMREAD_FakeUint8(0), Return(0) ) );
   node = parse_mmvad_node( 0x000, &cpu);
   ASSERT_EQ( NULL, node->filename);
}

TEST_F( MEMFRS_VAD, VAD_PARSE_WRONG_TYPE ){
    CPUState cpu;
    GEN_MOCK_OBJECT( mock );
    vad_node* node =NULL;


    EXPECT_CALL( mock, cpu_memory_rw_debug(_,_,_,_,_))
                .WillOnce( Return(0) )
                .WillOnce( Return(0) )
                .WillOnce( Return(0) )
                .WillOnce( Return(0) )
                //Fake VAD type
                .WillOnce( DoAll( MEMREAD_FakeUint8(VadDevicePhysicalMemory), Return(0) ));
    node = parse_mmvad_node( 0x000, &cpu);
    ASSERT_EQ( NULL, node->filename);
    ASSERT_EQ( VadDevicePhysicalMemory, node->vad_type);
}

TEST_F( MEMFRS_VAD, VAD_TRAVERSE ){
    CPUState cpu;
    GEN_MOCK_OBJECT( mock );
    UT_array *vad_list;
    char* filename;

    EXPECT_CALL( mock, cpu_memory_rw_debug(_,_,_,_,_))
                .WillOnce( DoAll( MEMREAD_FakeUint64(0x0001), Return(0) ) )
                //parse first level
                .WillOnce( Return(0) )
                .WillOnce( Return(0) )
                .WillOnce( Return(0) )
                .WillOnce( Return(0) )
                //Fake VAD type
                .WillOnce( DoAll( MEMREAD_FakeUint8(VadImageMap), Return(0) ))  //;
                .WillOnce( Return(0) )
                .WillOnce( Return(0) )
                .WillOnce( DoAll( MEMREAD_FakeUint64(0x1003), Return(0) ) )
                // Fake Unicode String
                .WillOnce( DoAll( MEMREAD_FakeUint8(40), Return(0) ) )
                .WillOnce( DoAll( MEMREAD_FakeUint8(20), Return(0) ) )
                .WillOnce( Return(0) )
                .WillOnce( DoAll( MEMREAD_FakeUstring("f.i.l.e.n.a.m.e."), Return(0) ) )
                //end of first level code
                .WillOnce( DoAll( MEMREAD_FakeUint64(0x0002), Return(0) ) )
                .WillOnce( DoAll( MEMREAD_FakeUint64(0x0003), Return(0) ) )
                // 2nd node
                .WillOnce( Return(0) )
                .WillOnce( Return(0) )
                .WillOnce( Return(0) )
                .WillOnce( Return(0) )
                //Fake VAD type
                .WillOnce( DoAll( MEMREAD_FakeUint8(VadImageMap), Return(0) ))  //;
                .WillOnce( Return(0) )
                .WillOnce( Return(0) )
                .WillOnce( DoAll( MEMREAD_FakeUint64(0x1003), Return(0) ))
                // Fake Unicode String
                .WillOnce( DoAll( MEMREAD_FakeUint8(40), Return(0) ) )
                .WillOnce( DoAll( MEMREAD_FakeUint8(20), Return(0) ) )
                .WillOnce( Return(0) )
                .WillOnce( DoAll( MEMREAD_FakeUstring("f.i.l.e.n.a.m.e.2."), Return(0) ) )
                //end 2nd
                .WillOnce( DoAll( MEMREAD_FakeUint64(0x0000), Return(0) ) )
                .WillOnce( DoAll( MEMREAD_FakeUint64(0x0000), Return(0) ) )
                // 3rd node
                .WillOnce( Return(0) )
                .WillOnce( Return(0) )
                .WillOnce( Return(0) )
                .WillOnce( Return(0) )
                //Fake VAD type
                .WillOnce( DoAll( MEMREAD_FakeUint8(VadImageMap), Return(0) ))  //;
                .WillOnce( Return(0) )
                .WillOnce( Return(0) )
                .WillOnce( DoAll( MEMREAD_FakeUint64(0x1003), Return(0) ) )
                // Fake Unicode String
                .WillOnce( DoAll( MEMREAD_FakeUint8(40), Return(0) ) )
                .WillOnce( DoAll( MEMREAD_FakeUint8(20), Return(0) ) )
                .WillOnce( Return(0) )
                .WillOnce( DoAll( MEMREAD_FakeUstring("f.i.l.e.n.a.m.e.3."), Return(0) ) )
                .WillOnce( DoAll( MEMREAD_FakeUint64(0x0000), Return(0) ) )
                .WillOnce( DoAll( MEMREAD_FakeUint64(0x0000), Return(0) ) );
    vad_list = memfrs_traverse_vad_tree(0x123,  &cpu);
    filename = ((vad_node*)utarray_eltptr( vad_list, 0))->filename;
    ASSERT_EQ( 0, strcmp( "filename", filename) );     
    filename = ((vad_node*)utarray_eltptr( vad_list, 1))->filename;
    ASSERT_EQ( 0, strcmp( "filename2", filename) );
    filename = ((vad_node*)utarray_eltptr( vad_list, 2))->filename;
    ASSERT_EQ( 0, strcmp( "filename3", filename) );
}

TEST_F( MEMFRS_VAD, VAD_TRAVERSE_NO_Node ){
    CPUState cpu;
    UT_array *vad_list;
    GEN_MOCK_OBJECT( mock );

    EXPECT_CALL( mock, cpu_memory_rw_debug(_,_,_,_,_))
               .WillOnce( DoAll( MEMREAD_FakeUint64(0x0000), Return(0) ) );
    vad_list = memfrs_traverse_vad_tree(0x123,  &cpu);
    ASSERT_EQ( 0, utarray_len(vad_list));
}

TEST_F( MEMFRS_VAD, VAD_TRAVERSE_NO_Struct_Info ){
    CPUState cpu;
    UT_array *vad_list;
    GEN_MOCK_OBJECT( mock );
    g_struct_info = NULL;
    vad_list = memfrs_traverse_vad_tree(0x123,  &cpu);
    ASSERT_EQ( NULL, vad_list);
}


