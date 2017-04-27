#ifndef _QEMU_DISAS_H
#define _QEMU_DISAS_H

#include "qemu-common.h"

#ifdef NEED_CPU_H
/* Disassemble this for me please... (debugging). */
void disas(FILE *out, void *code, unsigned long size);
void target_disas(FILE *out, CPUArchState *env, target_ulong code,
                  target_ulong size, int flags);
int target_disas_inst_count(FILE *out, CPUArchState *env, target_ulong code,
                  target_ulong size, int num_inst, int flags);
void monitor_disas(Monitor *mon, CPUArchState *env,
                   target_ulong pc, int nb_insn, int is_physical, int flags);

/* Look up symbol for debugging purpose.  Returns "" if unknown. */
const char *lookup_symbol(target_ulong orig_addr);
#endif

struct syminfo;
struct elf32_sym;
struct elf64_sym;

#if defined(CONFIG_USER_ONLY)
typedef const char *(*lookup_symbol_t)(struct syminfo *s, target_ulong orig_addr);
#else
typedef const char *(*lookup_symbol_t)(struct syminfo *s, hwaddr orig_addr);
#endif

struct syminfo {
    lookup_symbol_t lookup_symbol;
    unsigned int disas_num_syms;
    union {
      struct elf32_sym *elf32;
      struct elf64_sym *elf64;
    } disas_symtab;
    const char *disas_strtab;
    struct syminfo *next;
};

/* Filled in by elfload.c.  Simplistic, but will do for now. */
extern struct syminfo *syminfos;

#endif /* _QEMU_DISAS_H */
