#ifndef __MEMFRS_PRIV_H__
#define __MEMFRS_PRIV_H__

extern json_object *g_struct_info;
extern char* parse_unicode_strptr(uint64_t ustr_ptr, CPUState *cpu);
extern char* parse_unicode_str(uint8_t* ustr, CPUState *cpu);   
extern void hexdump(Monitor *mon, uint8_t* buf, size_t length);
extern int memfrs_enum_proc_handles(uint64_t target_cr3, uint64_t kpcr_ptr, CPUState *cpu , json_object *handles);

#endif
