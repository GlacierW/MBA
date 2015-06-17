    int i, j, k, l;

    uint64_t s;
    uint64_t wa, ra, hdaddr;

    dift_record rec;

    CONTAMINATION_RECORD t = 0;

    static void* dispatch[128];
    static int dispatch_init = 0;

    if ( !dispatch_init ) {
        dispatch[INSIDE_REG_ASSIGN] = &&rt_inside_reg_assign;
        dispatch[INSIDE_REG_APPEND] = &&rt_inside_reg_append;

        dispatch[REG_REG_OO_ASSIGN_MO8] =  &&rt_reg_reg_oo_assign_mo8;
        dispatch[REG_REG_OO_ASSIGN_MO16] = &&rt_reg_reg_oo_assign_mo16;
        dispatch[REG_REG_OO_ASSIGN_MO32] = &&rt_reg_reg_oo_assign_mo32;    
        dispatch[REG_REG_OO_ASSIGN_MO64] = &&rt_reg_reg_oo_assign_mo64;

        dispatch[REG_REG_OO_APPEND_MO8] =  &&rt_reg_reg_oo_append_mo8;
        dispatch[REG_REG_OO_APPEND_MO16] = &&rt_reg_reg_oo_append_mo16;
        dispatch[REG_REG_OO_APPEND_MO32] = &&rt_reg_reg_oo_append_mo32;
        dispatch[REG_REG_OO_APPEND_MO64] = &&rt_reg_reg_oo_append_mo64;

        dispatch[REG_REG_MIX_APPEND_MO16] = &&rt_reg_reg_mix_append_mo16;
        dispatch[REG_REG_MIX_APPEND_MO32] = &&rt_reg_reg_mix_append_mo32;
        dispatch[REG_REG_MIX_APPEND_MO64] = &&rt_reg_reg_mix_append_mo64;

        dispatch[REG_MEM_OO_ASSIGN_MO8] =  &&rt_reg_mem_oo_assign_mo8;
        dispatch[REG_MEM_OO_ASSIGN_MO16] = &&rt_reg_mem_oo_assign_mo16;
        dispatch[REG_MEM_OO_ASSIGN_MO32] = &&rt_reg_mem_oo_assign_mo32;
        dispatch[REG_MEM_OO_ASSIGN_MO64] = &&rt_reg_mem_oo_assign_mo64;

        dispatch[REG_MEM_OO_APPEND_MO8] =  &&rt_reg_mem_oo_append_mo8;
        dispatch[REG_MEM_OO_APPEND_MO16] = &&rt_reg_mem_oo_append_mo16;
        dispatch[REG_MEM_OO_APPEND_MO32] = &&rt_reg_mem_oo_append_mo32;
        dispatch[REG_MEM_OO_APPEND_MO64] = &&rt_reg_mem_oo_append_mo64;

        dispatch[REG_MEM_MIX_ASSIGN_MO16] = &&rt_reg_mem_mix_assign_mo16;
        dispatch[REG_MEM_MIX_ASSIGN_MO32] = &&rt_reg_mem_mix_assign_mo32;
        dispatch[REG_MEM_MIX_ASSIGN_MO64] = &&rt_reg_mem_mix_assign_mo64;

        dispatch[REG_MEM_MIX_APPEND_MO16] = &&rt_reg_mem_mix_append_mo16;
        dispatch[REG_MEM_MIX_APPEND_MO32] = &&rt_reg_mem_mix_append_mo32;
        dispatch[REG_MEM_MIX_APPEND_MO64] = &&rt_reg_mem_mix_append_mo64;

        dispatch[MEM_REG_OO_ASSIGN_MO8] =  &&rt_mem_reg_oo_assign_mo8;
        dispatch[MEM_REG_OO_ASSIGN_MO16] = &&rt_mem_reg_oo_assign_mo16;
        dispatch[MEM_REG_OO_ASSIGN_MO32] = &&rt_mem_reg_oo_assign_mo32;
        dispatch[MEM_REG_OO_ASSIGN_MO64] = &&rt_mem_reg_oo_assign_mo64;

        dispatch[MEM_REG_OO_APPEND_MO8] =  &&rt_mem_reg_oo_append_mo8;
        dispatch[MEM_REG_OO_APPEND_MO16] = &&rt_mem_reg_oo_append_mo16;
        dispatch[MEM_REG_OO_APPEND_MO32] = &&rt_mem_reg_oo_append_mo32;
        dispatch[MEM_REG_OO_APPEND_MO64] = &&rt_mem_reg_oo_append_mo64;

        dispatch[MEM_REG_MIX_ASSIGN_MO16] = &&rt_mem_reg_mix_assign_mo16;
        dispatch[MEM_REG_MIX_ASSIGN_MO32] = &&rt_mem_reg_mix_assign_mo32;
        dispatch[MEM_REG_MIX_ASSIGN_MO64] = &&rt_mem_reg_mix_assign_mo64;

        dispatch[MEM_REG_MIX_APPEND_MO16] = &&rt_mem_reg_mix_append_mo16;
        dispatch[MEM_REG_MIX_APPEND_MO32] = &&rt_mem_reg_mix_append_mo32;
        dispatch[MEM_REG_MIX_APPEND_MO64] = &&rt_mem_reg_mix_append_mo64;

        dispatch[MEM_MEM_OO_ASSIGN_MO8] =  &&rt_mem_mem_oo_assign_mo8;
        dispatch[MEM_MEM_OO_ASSIGN_MO16] = &&rt_mem_mem_oo_assign_mo16;
        dispatch[MEM_MEM_OO_ASSIGN_MO32] = &&rt_mem_mem_oo_assign_mo32;
        dispatch[MEM_MEM_OO_ASSIGN_MO64] = &&rt_mem_mem_oo_assign_mo64;

        dispatch[REG_IM_CLEAR_MO8] =  &&rt_reg_im_clear_mo8;
        dispatch[REG_IM_CLEAR_MO16] = &&rt_reg_im_clear_mo16;
        dispatch[REG_IM_CLEAR_MO32] = &&rt_reg_im_clear_mo32;
        dispatch[REG_IM_CLEAR_MO64] = &&rt_reg_im_clear_mo64;

        dispatch[MEM_IM_CLEAR_MO8] =  &&rt_mem_im_clear_mo8;
        dispatch[MEM_IM_CLEAR_MO16] = &&rt_mem_im_clear_mo16;
        dispatch[MEM_IM_CLEAR_MO32] = &&rt_mem_im_clear_mo32;
        dispatch[MEM_IM_CLEAR_MO64] = &&rt_mem_im_clear_mo64;

        dispatch[MEM_HD] = &&rt_mem_hd;
        dispatch[HD_MEM] = &&rt_hd_mem;

        dispatch[REC_CONTAMINATE_MEM_OR] =  &&rt_rec_contaminate_mem_or;
        dispatch[REC_CONTAMINATE_MEM_AND] = &&rt_rec_contaminate_mem_and;

        dispatch[REC_CONTAMINATE_HD_OR] =  &&rt_rec_contaminate_hd_or;
        dispatch[REC_CONTAMINATE_HD_AND] = &&rt_rec_contaminate_hd_and;

        dispatch[REC_CLEAR_MEM] = &&rt_rec_clear_mem;

        dispatch[REC_BLOCK_BEGIN] = &&rt_rec_block_begin;
        dispatch[REC_END] =         &&rt_rec_end;

        dispatch_init = 1;
    }

    /*
    // The following coding style is supported by C99 but not by C++ (derived from C89/90)
    // Thereby, we replace the fancy style with the old fashion. Just in case...
    static void* dispatch[128] = {
        [INSIDE_REG_ASSIGN] = &&rt_inside_reg_assign;
        [INSIDE_REG_APPEND] =       &&rt_inside_reg_append,

        [REG_REG_OO_ASSIGN_MO8] =   &&rt_reg_reg_oo_assign_mo8,
        [REG_REG_OO_ASSIGN_MO16] =  &&rt_reg_reg_oo_assign_mo16,
        [REG_REG_OO_ASSIGN_MO32] =  &&rt_reg_reg_oo_assign_mo32,
        [REG_REG_OO_ASSIGN_MO64] =  &&rt_reg_reg_oo_assign_mo64,
        [REG_REG_OO_APPEND_MO8] =   &&rt_reg_reg_oo_append_mo8,
        [REG_REG_OO_APPEND_MO16] =  &&rt_reg_reg_oo_append_mo16,
        [REG_REG_OO_APPEND_MO32] =  &&rt_reg_reg_oo_append_mo32,
        [REG_REG_OO_APPEND_MO64] =  &&rt_reg_reg_oo_append_mo64,
        [REG_REG_MIX_APPEND_MO16] = &&rt_reg_reg_mix_append_mo16,
        [REG_REG_MIX_APPEND_MO32] = &&rt_reg_reg_mix_append_mo32,
        [REG_REG_MIX_APPEND_MO64] = &&rt_reg_reg_mix_append_mo64,

        [REG_MEM_OO_ASSIGN_MO8] =   &&rt_reg_mem_oo_assign_mo8,
        [REG_MEM_OO_ASSIGN_MO16] =  &&rt_reg_mem_oo_assign_mo16,
        [REG_MEM_OO_ASSIGN_MO32] =  &&rt_reg_mem_oo_assign_mo32,
        [REG_MEM_OO_ASSIGN_MO64] =  &&rt_reg_mem_oo_assign_mo64,
        [REG_MEM_OO_APPEND_MO8] =   &&rt_reg_mem_oo_append_mo8,
        [REG_MEM_OO_APPEND_MO16] =  &&rt_reg_mem_oo_append_mo16,
        [REG_MEM_OO_APPEND_MO32] =  &&rt_reg_mem_oo_append_mo32,
        [REG_MEM_OO_APPEND_MO64] =  &&rt_reg_mem_oo_append_mo64,
        [REG_MEM_MIX_ASSIGN_MO16] = &&rt_reg_mem_mix_assign_mo16,
        [REG_MEM_MIX_ASSIGN_MO32] = &&rt_reg_mem_mix_assign_mo32,
        [REG_MEM_MIX_ASSIGN_MO64] = &&rt_reg_mem_mix_assign_mo64,
        [REG_MEM_MIX_APPEND_MO16] = &&rt_reg_mem_mix_append_mo16,
        [REG_MEM_MIX_APPEND_MO32] = &&rt_reg_mem_mix_append_mo32,
        [REG_MEM_MIX_APPEND_MO64] = &&rt_reg_mem_mix_append_mo64,

        [MEM_REG_OO_ASSIGN_MO8] =   &&rt_mem_reg_oo_assign_mo8,
        [MEM_REG_OO_ASSIGN_MO16] =  &&rt_mem_reg_oo_assign_mo16,
        [MEM_REG_OO_ASSIGN_MO32] =  &&rt_mem_reg_oo_assign_mo32,
        [MEM_REG_OO_ASSIGN_MO64] =  &&rt_mem_reg_oo_assign_mo64,
        [MEM_REG_OO_APPEND_MO8] =   &&rt_mem_reg_oo_append_mo8,
        [MEM_REG_OO_APPEND_MO16] =  &&rt_mem_reg_oo_append_mo16,
        [MEM_REG_OO_APPEND_MO32] =  &&rt_mem_reg_oo_append_mo32,
        [MEM_REG_OO_APPEND_MO64] =  &&rt_mem_reg_oo_append_mo64, 
        [MEM_REG_MIX_ASSIGN_MO16] = &&rt_mem_reg_mix_assign_mo16,
        [MEM_REG_MIX_ASSIGN_MO32] = &&rt_mem_reg_mix_assign_mo32,
        [MEM_REG_MIX_ASSIGN_MO64] = &&rt_mem_reg_mix_assign_mo64,
        [MEM_REG_MIX_APPEND_MO16] = &&rt_mem_reg_mix_append_mo16,
        [MEM_REG_MIX_APPEND_MO32] = &&rt_mem_reg_mix_append_mo32,
        [MEM_REG_MIX_APPEND_MO64] = &&rt_mem_reg_mix_append_mo64,

        [MEM_MEM_OO_ASSIGN_MO8] =   &&rt_mem_mem_oo_assign_mo8,
        [MEM_MEM_OO_ASSIGN_MO16] =  &&rt_mem_mem_oo_assign_mo16,
        [MEM_MEM_OO_ASSIGN_MO32] =  &&rt_mem_mem_oo_assign_mo32,
        [MEM_MEM_OO_ASSIGN_MO64] =  &&rt_mem_mem_oo_assign_mo64,

        [REG_IM_CLEAR_MO8] =        &&rt_reg_im_clear_mo8,
        [REG_IM_CLEAR_MO16] =       &&rt_reg_im_clear_mo16,
        [REG_IM_CLEAR_MO32] =       &&rt_reg_im_clear_mo32,
        [REG_IM_CLEAR_MO64] =       &&rt_reg_im_clear_mo64,

        [MEM_IM_CLEAR_MO8] =        &&rt_mem_im_clear_mo8,
        [MEM_IM_CLEAR_MO16] =       &&rt_mem_im_clear_mo16,
        [MEM_IM_CLEAR_MO32] =       &&rt_mem_im_clear_mo32,
        [MEM_IM_CLEAR_MO64] =       &&rt_mem_im_clear_mo64,

        [MEM_HD] =                  &&rt_mem_hd,
        [HD_MEM] =                  &&rt_hd_mem,

        [REC_CONTAMINATE_MEM_OR] =  &&rt_rec_contaminate_mem_or,
        [REC_CONTAMINATE_MEM_AND] = &&rt_rec_contaminate_mem_and,
        [REC_CONTAMINATE_HD_OR] =   &&rt_rec_contaminate_hd_or,
        [REC_CONTAMINATE_HD_AND] =  &&rt_rec_contaminate_hd_and,
        [REC_CLEAR_MEM] =           &&rt_rec_clear_mem,
        [REC_BLOCK_BEGIN] =         &&rt_rec_block_begin,
        [REC_END] =                 &&rt_rec_end
    };*/
