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
    //virtual void monitor_printf(Monitor *mon, const char *fmt, ...) = 0; 
   

    // obhook functions/API
};

struct mock_func : hook_func {
    public:
        MOCK_METHOD3( cpu_physical_memory_read, void(hwaddr, void *, int));
        MOCK_METHOD5( cpu_memory_rw_debug, int(void *, target_ulong, uint8_t *, int, int));
        //MOCK_METHOD3( monitor_printf, void(Monitor *, const char *, ##__VA_ARGS__));     
} *mock_ptr;

#define GEN_MOCK_OBJECT(x)  mock_func x; mock_ptr = &x;

// mock the functions in the original code
#define cpu_memory_rw_debug(arg1, arg2, arg3, arg4, arg5)      mock_ptr->cpu_memory_rw_debug(arg1, arg2, arg3, arg4, arg5)
#define cpu_physical_memory_read(arg1, arg2, arg3)     mock_ptr->cpu_physical_memory_read(arg1, arg2, arg3)

#define monitor_printf(mon, fmt, ...)    printf( fmt, ##__VA_ARGS__)

extern "C" {
#include "../memfrs.c"

}

#undef cpu_memory_rw_debug
#undef cpu_physical_memory_read

using namespace ::testing;

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
    //printf("%s", b);
    ASSERT_EQ( 0, memcmp(b, "hello", 6));
    //ASSERT_EQ( 1, memfrs_get_virmem_content(&cpu, 0x1234, 0x1234, 4, b));
} 

ACTION_P(VirMemReadWithIdx, str) { strcpy(reinterpret_cast<char *>(arg2) , str+arg1); }

TEST( MEMFRS_VIRMEMTEST, VirMemScan) {
    CPUState cpu;
    const char* mem = "abcabcabcabc";
    uint8_t b[100];
    uint64_t *p;

    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL( mock, cpu_memory_rw_debug(_,_,_,_,_) ).WillRepeatedly( DoAll( VirMemReadWithIdx( mem ), Return(0) ));
    UT_array *match_addr = memfrs_scan_virmem(&cpu, 0x0, strlen(mem), "bca");

    ASSERT_EQ( 1, *(utarray_eltptr( match_addr, 0)));
    ASSERT_EQ( 4, *(utarray_eltptr( match_addr, 1)));
    ASSERT_EQ( 7, *(utarray_eltptr( match_addr, 2)));
}

/*=======================================================================================*/

ACTION_P(PhyMemReadWithIdx, str) { strcpy(reinterpret_cast<char *>(arg1) , str+arg0); }

TEST( MEMFRS_PHYMEMTEST, PhyMemScan) {
CPUState cpu;
    const char* mem = "abcabcabcabc";
    uint8_t b[100];
    uint64_t *p;

    GEN_MOCK_OBJECT( mock );
    EXPECT_CALL( mock, cpu_physical_memory_read(_,_,_) ).WillRepeatedly( PhyMemReadWithIdx( mem  ));
    UT_array *match_addr = memfrs_scan_phymem(0x0, strlen(mem), "bca");
    
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

    printf("to_string()=%s\n", json_object_to_json_string(parent_obj));
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
    printf("%p\n", kpcr);

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
    addFieldToStruct(kpcr, "Self", "_KPCR", 0x18, 8, true);
    addFieldToStruct(kpcr, "Prcb", "_KPRCB", 0x180, 0x6900, false);
    json_object_object_add(tyde_definition, "_KPCR", kpcr);
    printf("%p\n", kpcr);

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

