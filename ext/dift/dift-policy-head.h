 /*
 *  DIFT taint propagation policy header
 *
 *  Copyright (c)   2012 Chiwei Wang
 *                  2016 Chiawei Wang
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

    int i, j, k, l;

    uint64_t s;
    uint64_t wa, ra, hdaddr;

    dift_record rec;

    CONTAMINATION_RECORD t = 0;

#if defined(CONFIG_DIFT_DEBUG)
    uint64_t dt[2], st[2];
#endif

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

        dispatch[REG_REG_MIX_APPEND_MO8] =  &&rt_reg_reg_oo_append_mo8;
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

        dispatch[REG_MEM_MIX_APPEND_MO8] =  &&rt_reg_mem_oo_append_mo8;
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

        dispatch[MEM_REG_MIX_APPEND_MO8] = &&rt_mem_reg_oo_append_mo8;
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

        // SSE
        dispatch[XMM_XMM_OO_ASSIGN_MO32] = &&rt_xmm_xmm_oo_assign_mo32;
        dispatch[XMM_XMM_OO_ASSIGN_MO64] = &&rt_xmm_xmm_oo_assign_mo64;
        dispatch[XMM_XMM_OO_ASSIGN_MO64LH] = &&rt_xmm_xmm_oo_assign_mo64lh;
        dispatch[XMM_XMM_OO_ASSIGN_MO64HL] = &&rt_xmm_xmm_oo_assign_mo64hl;
        dispatch[XMM_XMM_OO_ASSIGN_MO128] = &&rt_xmm_xmm_oo_assign_mo128;

        dispatch[XMM_REG_OO_ASSIGN_MO32] = &&rt_xmm_reg_oo_assign_mo32;
        dispatch[XMM_REG_OO_ASSIGN_MO64] = &&rt_xmm_reg_oo_assign_mo64;

        dispatch[REG_XMM_OO_ASSIGN_MO32] = &&rt_reg_xmm_oo_assign_mo32;
        dispatch[REG_XMM_OO_ASSIGN_MO64] = &&rt_reg_xmm_oo_assign_mo64;

        dispatch[XMM_MEM_OO_ASSIGN_MO32] = &&rt_xmm_mem_oo_assign_mo32;
        dispatch[XMM_MEM_OO_ASSIGN_MO64] = &&rt_xmm_mem_oo_assign_mo64;
        dispatch[XMM_MEM_OO_ASSIGN_MO64H] = &&rt_xmm_mem_oo_assign_mo64h;
        dispatch[XMM_MEM_OO_ASSIGN_MO128] = &&rt_xmm_mem_oo_assign_mo128;

        dispatch[MEM_XMM_OO_ASSIGN_MO32] = &&rt_mem_xmm_oo_assign_mo32;
        dispatch[MEM_XMM_OO_ASSIGN_MO64] = &&rt_mem_xmm_oo_assign_mo64;
        dispatch[MEM_XMM_OO_ASSIGN_MO64H] = &&rt_mem_xmm_oo_assign_mo64h;
        dispatch[MEM_XMM_OO_ASSIGN_MO128] = &&rt_mem_xmm_oo_assign_mo128;

        dispatch[XMM_XMM_OO_ASSIGN_DUP_MO32L] = &&rt_xmm_xmm_oo_assign_dup_mo32l;
        dispatch[XMM_XMM_OO_ASSIGN_DUP_MO32H] = &&rt_xmm_xmm_oo_assign_dup_mo32h;
        dispatch[XMM_XMM_OO_ASSIGN_DUP_MO64] = &&rt_xmm_xmm_oo_assign_dup_mo64;

        dispatch[XMM_MEM_OO_ASSIGN_DUP_MO32L] = &&rt_xmm_mem_oo_assign_dup_mo32l;
        dispatch[XMM_MEM_OO_ASSIGN_DUP_MO32H] = &&rt_xmm_mem_oo_assign_dup_mo32h;
        dispatch[XMM_MEM_OO_ASSIGN_DUP_MO64] = &&rt_xmm_mem_oo_assign_dup_mo64;

        dispatch[XMM_IM_CLEAR] = &&rt_xmm_im_clear;

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
