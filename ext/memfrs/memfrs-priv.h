#ifndef __MEMFRS_PRIV_H__
#define __MEMFRS_PRIV_H__

extern json_object *g_struct_info;
extern json_object *g_globalvar_info;
extern char* parse_unicode_strptr(uint64_t ustr_ptr, CPUState *cpu);
extern char* parse_unicode_str(uint8_t* ustr, CPUState *cpu);   
extern void hexdump(Monitor *mon, uint8_t* buf, size_t length);

#endif
