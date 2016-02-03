/*
 *  DIFT taint propagation policy
 *
 *  Copyright (c) 2016 Chiawei Wang
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

rt_inside_reg_assign:
	i = rec.v1.inside_r.reg;
    j = rec.v1.inside_r.d_byte;
    l = rec.v1.inside_r.s_byte;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = dc->reg_dirty_tbl[i][j];
	st = dc->reg_dirty_tbl[i][l];

	dift_log( "B I %s%s[%d](%" PRIx64 ") %s%s %s%s[%d](%" PRIx64 ")", 
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				j,
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_ASSIGN,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				l,
				st );
}
#endif

    dc->reg_dirty_tbl[i][j] = dc->reg_dirty_tbl[i][l];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_inside_reg_append:
	i = rec.v1.inside_r.reg;
    j = rec.v1.inside_r.d_byte;
    l = rec.v1.inside_r.s_byte;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = dc->reg_dirty_tbl[i][j];
	st = dc->reg_dirty_tbl[i][l];

	dift_log( "B I %s%s[%d](%" PRIx64 ") %s%s %s%s[%d](%" PRIx64 ")", 
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				j,
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_APPEND,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				l,
				st );
}
#endif

    dc->reg_dirty_tbl[i][j] |= dc->reg_dirty_tbl[i][l];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION 
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_reg_reg_oo_assign_mo8:
	i = rec.v1.r2r_byte.dreg;
    k = rec.v1.r2r_byte.dreg_byte;
    j = rec.v1.r2r_byte.sreg;
    l = rec.v1.r2r_byte.sreg_byte;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = dc->reg_dirty_tbl[i][k];
	st = dc->reg_dirty_tbl[j][l];

	dift_log( "B O %s%s[%d](%" PRIx64 ") %s%s %s%s[%d](%" PRIx64 ")", 
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				k,
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_ASSIGN,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[j],
				l,
				st );
}
#endif

    dc->reg_dirty_tbl[i][k] = dc->reg_dirty_tbl[j][l];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_reg_reg_oo_assign_mo16:
	i = rec.v1.r2r.dreg;
    j = rec.v1.r2r.sreg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint16_t*)&dc->reg_dirty_tbl[i][0];
	st = *(uint16_t*)&dc->reg_dirty_tbl[j][0];

	dift_log( "W O %s%s(%" PRIx64 ") %s%s %s%s(%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_ASSIGN,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[j],
				st );
}
#endif

    *(uint16_t*)&dc->reg_dirty_tbl[i][0] = *(uint16_t*)&dc->reg_dirty_tbl[j][0];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_reg_reg_oo_assign_mo32:
	i = rec.v1.r2r.dreg;
    j = rec.v1.r2r.sreg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint32_t*)&dc->reg_dirty_tbl[i][0];
	st = *(uint32_t*)&dc->reg_dirty_tbl[j][0];

	dift_log( "D O %s%s(%" PRIx64 ") %s%s %s%s(%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_ASSIGN,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[j],
				st );
}
#endif

    *(uint32_t*)&dc->reg_dirty_tbl[i][0] = *(uint32_t*)&dc->reg_dirty_tbl[j][0];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_reg_reg_oo_assign_mo64:
	i = rec.v1.r2r.dreg;
    j = rec.v1.r2r.sreg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint64_t*)&dc->reg_dirty_tbl[i][0];
	st = *(uint64_t*)&dc->reg_dirty_tbl[j][0];

	dift_log( "Q O %s%s(%" PRIx64 ") %s%s %s%s(%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_ASSIGN,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[j],
				st );
}
#endif

    *(uint64_t*)&dc->reg_dirty_tbl[i][0] = *(uint64_t*)&dc->reg_dirty_tbl[j][0];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_reg_reg_oo_append_mo8:
    i = rec.v1.r2r_byte.dreg;
    k = rec.v1.r2r_byte.dreg_byte;
	j = rec.v1.r2r_byte.sreg;
    l = rec.v1.r2r_byte.sreg_byte;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = dc->reg_dirty_tbl[i][k];
	st = dc->reg_dirty_tbl[j][l];

	dift_log( "B O %s%s[%d](%" PRIx64 ") %s%s %s%s[%d](%" PRIx64 ")", 
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				k,
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_APPEND,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[j],
				l,
				st );
}
#endif

    dc->reg_dirty_tbl[i][k] |= dc->reg_dirty_tbl[j][l];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_reg_reg_oo_append_mo16:
	i = rec.v1.r2r.dreg;
    j = rec.v1.r2r.sreg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint16_t*)&dc->reg_dirty_tbl[i][0];
	st = *(uint16_t*)&dc->reg_dirty_tbl[j][0];

	dift_log( "W O %s%s(%" PRIx64 ") %s%s %s%s(%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_APPEND,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[j],
				st );
}
#endif

    *(uint16_t*)&dc->reg_dirty_tbl[i][0] |= *(uint16_t*)&dc->reg_dirty_tbl[j][0];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{	
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_reg_reg_oo_append_mo32:
	i = rec.v1.r2r.dreg;
    j = rec.v1.r2r.sreg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint32_t*)&dc->reg_dirty_tbl[i][0];
	st = *(uint32_t*)&dc->reg_dirty_tbl[j][0];

	dift_log( "D O %s%s(%" PRIx64 ") %s%s %s%s(%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_APPEND,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[j],
				st );
}
#endif

    *(uint32_t*)&dc->reg_dirty_tbl[i][0] |= *(uint32_t*)&dc->reg_dirty_tbl[j][0];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_reg_reg_oo_append_mo64:
	i = rec.v1.r2r.dreg;
    j = rec.v1.r2r.sreg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint64_t*)&dc->reg_dirty_tbl[i][0];
	st = *(uint64_t*)&dc->reg_dirty_tbl[j][0];

	dift_log( "Q O %s%s(%" PRIx64 ") %s%s %s%s(%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_APPEND,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[j],
				st );
}
#endif

    *(uint64_t*)&dc->reg_dirty_tbl[i][0] |= *(uint64_t*)&dc->reg_dirty_tbl[j][0];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_reg_reg_mix_append_mo16: 
    i = rec.v1.r2r.dreg;
	j = rec.v1.r2r.sreg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint16_t*)&dc->reg_dirty_tbl[i][0];
	st = *(uint16_t*)&dc->reg_dirty_tbl[j][0];

	dift_log( "W M %s%s(%" PRIx64 ") %s%s %s%s(%" PRIx64 ")", 
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_APPEND,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[j],
				st );
}
#endif

    t = dc->reg_dirty_tbl[i][0] | dc->reg_dirty_tbl[i][1]
        | dc->reg_dirty_tbl[j][0] | dc->reg_dirty_tbl[j][1];

    dc->reg_dirty_tbl[i][0] |= t;
    dc->reg_dirty_tbl[i][1] |= t;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_reg_reg_mix_append_mo32:
    i = rec.v1.r2r.dreg;
	j = rec.v1.r2r.sreg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint32_t*)&dc->reg_dirty_tbl[i][0];
	st = *(uint32_t*)&dc->reg_dirty_tbl[j][0];

	dift_log( "D M %s%s(%" PRIx64 ") %s%s %s%s(%" PRIx64 ")", 
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_APPEND,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[j],
				st );
}
#endif

    dc->reg_dirty_tbl[i][0] |=
        dc->reg_dirty_tbl[j][0] | dc->reg_dirty_tbl[j][1]
        | dc->reg_dirty_tbl[j][2] | dc->reg_dirty_tbl[j][3]
        | dc->reg_dirty_tbl[i][1] | dc->reg_dirty_tbl[i][2]
        | dc->reg_dirty_tbl[i][3];
    dc->reg_dirty_tbl[i][1] = dc->reg_dirty_tbl[i][0];
    dc->reg_dirty_tbl[i][2] = dc->reg_dirty_tbl[i][0];
    dc->reg_dirty_tbl[i][3] = dc->reg_dirty_tbl[i][0];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_reg_reg_mix_append_mo64:
    i = rec.v1.r2r.dreg;
	j = rec.v1.r2r.sreg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint64_t*)&dc->reg_dirty_tbl[i][0];
	st = *(uint64_t*)&dc->reg_dirty_tbl[j][0];

	dift_log( "Q M %s%s(%" PRIx64 ") %s%s %s%s(%" PRIx64 ")", 
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_APPEND,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[j],
				st );
}
#endif

    dc->reg_dirty_tbl[i][0] |=
          dc->reg_dirty_tbl[j][0] | dc->reg_dirty_tbl[j][1]
        | dc->reg_dirty_tbl[j][2] | dc->reg_dirty_tbl[j][3]
        | dc->reg_dirty_tbl[j][4] | dc->reg_dirty_tbl[j][5]
        | dc->reg_dirty_tbl[j][6] | dc->reg_dirty_tbl[j][7]
        | dc->reg_dirty_tbl[i][1] | dc->reg_dirty_tbl[i][2]
        | dc->reg_dirty_tbl[i][3] | dc->reg_dirty_tbl[i][4]
        | dc->reg_dirty_tbl[i][5] | dc->reg_dirty_tbl[i][6]
        | dc->reg_dirty_tbl[i][7];

    dc->reg_dirty_tbl[i][1] = dc->reg_dirty_tbl[i][0];
    dc->reg_dirty_tbl[i][2] = dc->reg_dirty_tbl[i][0];
    dc->reg_dirty_tbl[i][3] = dc->reg_dirty_tbl[i][0];
    dc->reg_dirty_tbl[i][4] = dc->reg_dirty_tbl[i][0];
    dc->reg_dirty_tbl[i][5] = dc->reg_dirty_tbl[i][0];
    dc->reg_dirty_tbl[i][6] = dc->reg_dirty_tbl[i][0];
    dc->reg_dirty_tbl[i][7] = dc->reg_dirty_tbl[i][0];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();   

rt_reg_mem_oo_assign_mo8:
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : clean_source; 
    i = rec.v1.r2m_m2r_byte.reg;
    k = rec.v1.r2m_m2r_byte.hl;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = dc->reg_dirty_tbl[i][k];
	st = dc->mem_dirty_tbl[ra];

	dift_log( "B O %s%s[%d](%" PRIx64 ") %s%s %s[%p](%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				k,
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_ASSIGN,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				ra,
				st );
}
#endif

#if defined(CONFIG_INDIRECT_TAINT)
    j = rec.v1.r2m_m2r_byte.reg_base;
    l = rec.v1.r2m_m2r_byte.reg_index;

	t = 0;
	t |= (j == R_NONE)? 0 : dc->reg_dirty_tbl[j][0];
	t |= (l == R_NONE)? 0 : dc->reg_dirty_tbl[l][0];

    dc->reg_dirty_tbl[i][k] = dc->mem_dirty_tbl[ra] | t;
#else
    dc->reg_dirty_tbl[i][k] = dc->mem_dirty_tbl[ra];
#endif

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_reg_mem_oo_assign_mo16:
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : clean_source; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint16_t*)&dc->reg_dirty_tbl[i][0];
	st = *(uint16_t*)&dc->mem_dirty_tbl[ra];

	dift_log( "W O %s%s(%" PRIx64 ") %s%s %s[%p](%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_ASSIGN,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				ra,
				st );
}
#endif

#if defined(CONFIG_INDIRECT_TAINT)
    j = rec.v1.r2m_m2r.reg_base;
    l = rec.v1.r2m_m2r.reg_index;

	t = 0;
	t |= (j == R_NONE)? 0 : (dc->reg_dirty_tbl[j][0] | dc->reg_dirty_tbl[j][1]);
	t |= (l == R_NONE)? 0 : (dc->reg_dirty_tbl[l][0] | dc->reg_dirty_tbl[l][1]);

    dc->reg_dirty_tbl[i][0] = get_mem_dirty(dc, ra) | t;
    dc->reg_dirty_tbl[i][1] = get_mem_dirty(dc, ra + 1) | t;
#else
    dc->reg_dirty_tbl[i][0] = get_mem_dirty(dc, ra);
    dc->reg_dirty_tbl[i][1] = get_mem_dirty(dc, ra + 1);
#endif

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_reg_mem_oo_assign_mo32:
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : clean_source; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint32_t*)&dc->reg_dirty_tbl[i][0];
	st = *(uint32_t*)&dc->mem_dirty_tbl[ra];

	dift_log( "D O %s%s(%" PRIx64 ") %s%s %s[%p](%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_ASSIGN,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				ra,
				st );
}
#endif

    *(uint32_t*)&dc->reg_dirty_tbl[i][0] = *(uint32_t*)&dc->mem_dirty_tbl[ra];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{	
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_reg_mem_oo_assign_mo64:
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : clean_source; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint64_t*)&dc->reg_dirty_tbl[i][0];
	st = *(uint64_t*)&dc->mem_dirty_tbl[ra];

	dift_log( "Q O %s%s(%" PRIx64 ") %s%s %s[%p](%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_ASSIGN,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				ra,
				st );
}
#endif

    *(uint64_t*)&dc->reg_dirty_tbl[i][0] = *(uint64_t*)&dc->mem_dirty_tbl[ra];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_reg_mem_oo_append_mo8:
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : clean_source; 
	i = rec.v1.r2m_m2r_byte.reg;
    k = rec.v1.r2m_m2r_byte.hl;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = dc->reg_dirty_tbl[i][k];
	st = dc->mem_dirty_tbl[ra];

	dift_log( "B O %s%s[%d](%" PRIx64 ") %s%s %s[%p](%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				k,
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_APPEND,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				ra,
				st );
}				
#endif

    t = get_mem_dirty(dc, ra);
    dc->reg_dirty_tbl[i][k] |= t;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION 
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_reg_mem_oo_append_mo16:
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : clean_source; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint16_t*)&dc->reg_dirty_tbl[i][0];
	st = *(uint16_t*)&dc->mem_dirty_tbl[ra];

	dift_log( "W O %s%s(%" PRIx64 ") %s%s %s[%p](%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_APPEND,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				ra,
				st );
}
#endif

    *(uint16_t*)&dc->reg_dirty_tbl[i][0] |= *(uint16_t*)&dc->mem_dirty_tbl[ra];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_reg_mem_oo_append_mo32:
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : clean_source; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint32_t*)&dc->reg_dirty_tbl[i][0];
	st = *(uint32_t*)&dc->mem_dirty_tbl[ra];

	dift_log( "D O %s%s(%" PRIx64 ") %s%s %s[%p](%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_APPEND,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				ra,
				st );
}
#endif

    *(uint32_t*)&dc->reg_dirty_tbl[i][0] |= *(uint32_t*)&dc->mem_dirty_tbl[ra];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_reg_mem_oo_append_mo64:
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : clean_source;
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint64_t*)&dc->reg_dirty_tbl[i][0];
	st = *(uint64_t*)&dc->mem_dirty_tbl[ra];

	dift_log( "Q O %s%s(%" PRIx64 ") %s%s %s[%p](%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_APPEND,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				ra,
				st );
}
#endif

    *(uint64_t*)&dc->reg_dirty_tbl[i][0] |= *(uint64_t*)&dc->mem_dirty_tbl[ra];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_reg_mem_mix_assign_mo16:
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : clean_source; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint16_t*)&dc->reg_dirty_tbl[i][0];
	st = *(uint16_t*)&dc->mem_dirty_tbl[ra];

	dift_log( "W M %s%s(%" PRIx64 ") %s%s %s[%p](%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_ASSIGN,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				ra,
				st );
}
#endif

    t = get_mem_dirty(dc, ra) | get_mem_dirty(dc, ra + 1);
    dc->reg_dirty_tbl[i][0] = t;
    dc->reg_dirty_tbl[i][1] = t;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_reg_mem_mix_assign_mo32:
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : clean_source; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint32_t*)&dc->reg_dirty_tbl[i][0];
	st = *(uint32_t*)&dc->mem_dirty_tbl[ra];

	dift_log( "D M %s%s(%" PRIx64 ") %s%s %s[%p](%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_ASSIGN,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				ra,
				st );
}
#endif

    t =   get_mem_dirty(dc, ra)     | get_mem_dirty(dc, ra + 1)
        | get_mem_dirty(dc, ra + 2) | get_mem_dirty(dc, ra + 3);

    dc->reg_dirty_tbl[i][0] = t;
    dc->reg_dirty_tbl[i][1] = t;
    dc->reg_dirty_tbl[i][2] = t;
    dc->reg_dirty_tbl[i][3] = t;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_reg_mem_mix_assign_mo64:
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : clean_source; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint64_t*)&dc->reg_dirty_tbl[i][0];
	st = *(uint64_t*)&dc->mem_dirty_tbl[ra];

	dift_log( "Q M %s%s(%" PRIx64 ") %s%s %s[%p](%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_ASSIGN,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				ra,
				st );
}
#endif

    t =   get_mem_dirty(dc, ra)     | get_mem_dirty(dc, ra + 1)
        | get_mem_dirty(dc, ra + 2) | get_mem_dirty(dc, ra + 3)
        | get_mem_dirty(dc, ra + 4) | get_mem_dirty(dc, ra + 5)
        | get_mem_dirty(dc, ra + 6) | get_mem_dirty(dc, ra + 7);

    dc->reg_dirty_tbl[i][0] = t;
    dc->reg_dirty_tbl[i][1] = t;
    dc->reg_dirty_tbl[i][2] = t;
    dc->reg_dirty_tbl[i][3] = t;
    dc->reg_dirty_tbl[i][4] = t;
    dc->reg_dirty_tbl[i][5] = t;
    dc->reg_dirty_tbl[i][6] = t;
    dc->reg_dirty_tbl[i][7] = t;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_reg_mem_mix_append_mo16:
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : clean_source; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint16_t*)&dc->reg_dirty_tbl[i][0];
	st = *(uint16_t*)&dc->mem_dirty_tbl[ra];

	dift_log( "W M %s%s(%" PRIx64 ") %s%s %s[%p](%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_APPEND,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				ra,
				st );
}
#endif

    t = get_mem_dirty(dc, ra) | get_mem_dirty(dc, ra + 1)
        | dc->reg_dirty_tbl[i][0] | dc->reg_dirty_tbl[i][1];
    dc->reg_dirty_tbl[i][0] |= t;
    dc->reg_dirty_tbl[i][1] |= t;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_reg_mem_mix_append_mo32:
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : clean_source; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint32_t*)&dc->reg_dirty_tbl[i][0];
	st = *(uint32_t*)&dc->mem_dirty_tbl[ra];

	dift_log( "D M %s%s(%" PRIx64 ") %s%s %s[%p](%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_APPEND,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				ra,
				st );
}
#endif

    s = *(uint32_t*)&dc->mem_dirty_tbl[ra] | *(uint32_t*)&dc->reg_dirty_tbl[i][0];
    t = *((uint8_t*)&s) | *(((uint8_t*)&s) + 1) | *(((uint8_t*)&s) + 2) | *(((uint8_t*)&s) + 3);
    dc->reg_dirty_tbl[i][0] |= t;
    dc->reg_dirty_tbl[i][1] |= t;
    dc->reg_dirty_tbl[i][2] |= t;
    dc->reg_dirty_tbl[i][3] |= t;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_reg_mem_mix_append_mo64:
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : clean_source; 
    i = rec.v1.r2m_m2r.reg;
	
#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint64_t*)&dc->reg_dirty_tbl[i][0];
	st = *(uint64_t*)&dc->mem_dirty_tbl[ra];

	dift_log( "Q M %s%s(%" PRIx64 ") %s%s %s[%p](%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_ASSIGN,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				ra,
				st );
}
#endif

    s = *(uint64_t*)&dc->mem_dirty_tbl[ra] | *(uint64_t*)&dc->reg_dirty_tbl[i][0];
    t =   *((uint8_t*)&s)       | *(((uint8_t*)&s) + 1)
        | *(((uint8_t*)&s) + 2) | *(((uint8_t*)&s) + 3)
        | *(((uint8_t*)&s) + 4) | *(((uint8_t*)&s) + 5)
        | *(((uint8_t*)&s) + 6) | *(((uint8_t*)&s) + 7);

    dc->reg_dirty_tbl[i][0] |= t;
    dc->reg_dirty_tbl[i][1] |= t;
    dc->reg_dirty_tbl[i][2] |= t;
    dc->reg_dirty_tbl[i][3] |= t;
    dc->reg_dirty_tbl[i][4] |= t;
    dc->reg_dirty_tbl[i][5] |= t;
    dc->reg_dirty_tbl[i][6] |= t;
    dc->reg_dirty_tbl[i][7] |= t;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_mem_reg_oo_assign_mo8:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : null_sink; 
	i = rec.v1.r2m_m2r_byte.reg;
    k = rec.v1.r2m_m2r_byte.hl;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = dc->mem_dirty_tbl[wa];
	st = dc->reg_dirty_tbl[i][k];

	dift_log( "B O %s[%p](%" PRIx64 ") %s%s %s%s[%d](%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				wa,
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_ASSIGN,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				k,
				st );
}
#endif

    set_mem_dirty(dc, wa, dc->reg_dirty_tbl[i][k], 0);

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_mem_reg_oo_assign_mo16:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : null_sink; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint16_t*)&dc->mem_dirty_tbl[wa];
	st = *(uint16_t*)&dc->reg_dirty_tbl[i][0];

	dift_log( "W O %s[%p](%" PRIx64 ") %s%s %s%s(%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				wa,	
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_ASSIGN,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				st );
}
#endif

    *(uint16_t*)&dc->mem_dirty_tbl[wa] = *(uint16_t*)&dc->reg_dirty_tbl[i][0];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_mem_reg_oo_assign_mo32:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : null_sink; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint32_t*)&dc->mem_dirty_tbl[wa];
	st = *(uint32_t*)&dc->reg_dirty_tbl[i][0];

	dift_log( "D O %s[%p](%" PRIx64 ") %s%s %s%s(%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				wa,	
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_ASSIGN,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				st );
}
#endif

    *(uint32_t*)&dc->mem_dirty_tbl[wa] = *(uint32_t*)&dc->reg_dirty_tbl[i][0];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_mem_reg_oo_assign_mo64:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : null_sink; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint64_t*)&dc->mem_dirty_tbl[wa];
	st = *(uint64_t*)&dc->reg_dirty_tbl[i][0];

	dift_log( "Q O %s[%p](%" PRIx64 ") %s%s %s%s(%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				wa,	
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_ASSIGN,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				st );
}
#endif

    *(uint64_t*)&dc->mem_dirty_tbl[wa] = *(uint64_t*)&dc->reg_dirty_tbl[i][0];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{	
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_mem_reg_oo_append_mo8:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : null_sink; 
	i = rec.v1.r2m_m2r_byte.reg;
    k = rec.v1.r2m_m2r_byte.hl;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = dc->mem_dirty_tbl[wa];
	st = dc->reg_dirty_tbl[i][k];

	dift_log( "B O %s[%p](%" PRIx64 ") %s%s %s%s[%d](%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				wa,
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_APPEND,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				k,
				st );
}
#endif

    set_mem_dirty(dc, wa, dc->reg_dirty_tbl[i][k], 1);

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_mem_reg_oo_append_mo16:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : null_sink; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint16_t*)&dc->mem_dirty_tbl[wa];
	st = *(uint16_t*)&dc->reg_dirty_tbl[i][0];

	dift_log( "W O %s[%p](%" PRIx64 ") %s%s %s%s(%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				wa,	
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_APPEND,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				st );
}
#endif

    *(uint16_t*)&dc->mem_dirty_tbl[wa] |= *(uint16_t*)&dc->reg_dirty_tbl[i][0];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_mem_reg_oo_append_mo32:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : null_sink; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint32_t*)&dc->mem_dirty_tbl[wa];
	st = *(uint32_t*)&dc->reg_dirty_tbl[i][0];

	dift_log( "D O %s[%p](%" PRIx64 ") %s%s %s%s(%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				wa,	
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_APPEND,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				st );
}
#endif

    *(uint32_t*)&dc->mem_dirty_tbl[wa] |= *(uint32_t*)&dc->reg_dirty_tbl[i][0];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{	
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_mem_reg_oo_append_mo64:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : null_sink; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint64_t*)&dc->mem_dirty_tbl[wa];
	st = *(uint64_t*)&dc->reg_dirty_tbl[i][0];

	dift_log( "Q O %s[%p](%" PRIx64 ") %s%s %s%s(%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				wa,	
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_APPEND,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				st );
}
#endif

    *(uint64_t*)&dc->mem_dirty_tbl[wa] |= *(uint64_t*)&dc->reg_dirty_tbl[i][0];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{	
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_mem_reg_mix_assign_mo16:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : null_sink; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint16_t*)&dc->mem_dirty_tbl[wa];
	st = *(uint16_t*)&dc->reg_dirty_tbl[i][0];

	dift_log( "W M %s[%p](%" PRIx64 ") %s%s %s%s(%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				wa,
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_ASSIGN,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				st );
}
#endif

    t = dc->reg_dirty_tbl[i][0] | dc->reg_dirty_tbl[i][1];

    set_mem_dirty(dc, wa, t, 0);
    set_mem_dirty(dc, wa + 1, t, 0);

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_mem_reg_mix_assign_mo32:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : null_sink; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint32_t*)&dc->mem_dirty_tbl[wa];
	st = *(uint32_t*)&dc->reg_dirty_tbl[i][0];

	dift_log( "D M %s[%p](%" PRIx64 ") %s%s %s%s(%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				wa,
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_ASSIGN,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				st );
}
#endif

    t =   dc->reg_dirty_tbl[i][0] | dc->reg_dirty_tbl[i][1]
        | dc->reg_dirty_tbl[i][2] | dc->reg_dirty_tbl[i][3];

    set_mem_dirty(dc, wa, t, 0);
    set_mem_dirty(dc, wa + 1, t, 0);
    set_mem_dirty(dc, wa + 2, t, 0);
    set_mem_dirty(dc, wa + 3, t, 0);

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{	
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_mem_reg_mix_assign_mo64:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : null_sink; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint64_t*)&dc->mem_dirty_tbl[wa];
	st = *(uint64_t*)&dc->reg_dirty_tbl[i][0];

	dift_log( "Q M %s[%p](%" PRIx64 ") %s%s %s%s(%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				wa,
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_ASSIGN,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				st );
}
#endif

    t =   dc->reg_dirty_tbl[i][0] | dc->reg_dirty_tbl[i][1]
        | dc->reg_dirty_tbl[i][2] | dc->reg_dirty_tbl[i][3]
        | dc->reg_dirty_tbl[i][4] | dc->reg_dirty_tbl[i][5]
        | dc->reg_dirty_tbl[i][6] | dc->reg_dirty_tbl[i][7];

    set_mem_dirty(dc, wa,     t, 0);
    set_mem_dirty(dc, wa + 1, t, 0);
    set_mem_dirty(dc, wa + 2, t, 0);
    set_mem_dirty(dc, wa + 3, t, 0);
    set_mem_dirty(dc, wa + 4, t, 0);
    set_mem_dirty(dc, wa + 5, t, 0);
    set_mem_dirty(dc, wa + 6, t, 0);
    set_mem_dirty(dc, wa + 7, t, 0);

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_mem_reg_mix_append_mo16:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : null_sink; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint16_t*)&dc->mem_dirty_tbl[wa];
	st = *(uint16_t*)&dc->reg_dirty_tbl[i][0];

	dift_log( "W M %s[%p](%" PRIx64 ") %s%s %s%s(%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				wa,
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_APPEND,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				st );
}
#endif

    t = dc->reg_dirty_tbl[i][0] | dc->reg_dirty_tbl[i][1]
        | get_mem_dirty(dc, wa) | get_mem_dirty(dc, wa + 1);

    set_mem_dirty(dc, wa, t, 1);
    set_mem_dirty(dc, wa + 1, t, 1);

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_mem_reg_mix_append_mo32:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : null_sink; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint32_t*)&dc->mem_dirty_tbl[wa];
	st = *(uint32_t*)&dc->reg_dirty_tbl[i][0];

	dift_log( "D M %s[%p](%" PRIx64 ") %s%s %s%s(%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				wa,
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_APPEND,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				st );
}
#endif

    t =   dc->reg_dirty_tbl[i][0] | dc->reg_dirty_tbl[i][1]
        | dc->reg_dirty_tbl[i][2] | dc->reg_dirty_tbl[i][3]
        | get_mem_dirty(dc, wa)     | get_mem_dirty(dc, wa + 1)
        | get_mem_dirty(dc, wa + 2) | get_mem_dirty(dc, wa + 3);

    set_mem_dirty(dc, wa,     t, 1);
    set_mem_dirty(dc, wa + 1, t, 1);
    set_mem_dirty(dc, wa + 2, t, 1);
    set_mem_dirty(dc, wa + 3, t, 1);

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_mem_reg_mix_append_mo64:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : null_sink; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint64_t*)&dc->mem_dirty_tbl[wa];
	st = *(uint64_t*)&dc->reg_dirty_tbl[i][0];

	dift_log( "Q M %s[%p](%" PRIx64 ") %s%s %s%s(%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				wa,
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_APPEND,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				st );
}
#endif

    t =   dc->reg_dirty_tbl[i][0] | dc->reg_dirty_tbl[i][1]
        | dc->reg_dirty_tbl[i][2] | dc->reg_dirty_tbl[i][3]
        | dc->reg_dirty_tbl[i][4] | dc->reg_dirty_tbl[i][5]
        | dc->reg_dirty_tbl[i][6] | dc->reg_dirty_tbl[i][7]
        | get_mem_dirty(dc, wa)     | get_mem_dirty(dc, wa + 1)
        | get_mem_dirty(dc, wa + 2) | get_mem_dirty(dc, wa + 3)
        | get_mem_dirty(dc, wa + 4) | get_mem_dirty(dc, wa + 5)
        | get_mem_dirty(dc, wa + 6) | get_mem_dirty(dc, wa + 7);

    set_mem_dirty(dc, wa,     t, 1);
    set_mem_dirty(dc, wa + 1, t, 1);
    set_mem_dirty(dc, wa + 2, t, 1);
    set_mem_dirty(dc, wa + 3, t, 1);
    set_mem_dirty(dc, wa + 4, t, 1);
    set_mem_dirty(dc, wa + 5, t, 1);
    set_mem_dirty(dc, wa + 6, t, 1);
    set_mem_dirty(dc, wa + 7, t, 1);

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_mem_mem_oo_assign_mo8:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : null_sink; 
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : clean_source; 

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = dc->mem_dirty_tbl[wa];
	st = dc->mem_dirty_tbl[ra];

	dift_log( "B O %s[%p](%" PRIx64 ") %s%s %s[%p](%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				wa,
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_ASSIGN,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				ra,
				st );
}
#endif

	dc->mem_dirty_tbl[wa] = dc->mem_dirty_tbl[ra];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{	
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_mem_mem_oo_assign_mo16:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : null_sink; 
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : clean_source; 

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint16_t*)&dc->mem_dirty_tbl[wa];
	st = *(uint16_t*)&dc->mem_dirty_tbl[ra];

	dift_log( "W O %s[%p](%" PRIx64 ") %s%s %s[%p](%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				wa,
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_ASSIGN,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				ra,
				st );
}
#endif

    *(uint16_t*)&dc->mem_dirty_tbl[wa] = *(uint16_t*)&dc->mem_dirty_tbl[ra];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_mem_mem_oo_assign_mo32:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : null_sink; 
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : clean_source; 

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint32_t*)&dc->mem_dirty_tbl[wa];
	st = *(uint32_t*)&dc->mem_dirty_tbl[ra];

	dift_log( "D O %s[%p](%" PRIx64 ") %s%s %s[%p](%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				wa,
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_ASSIGN,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				ra,
				st );
}
#endif

    *(uint32_t*)&dc->mem_dirty_tbl[wa] = *(uint32_t*)&dc->mem_dirty_tbl[ra];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_mem_mem_oo_assign_mo64:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : null_sink; 
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : clean_source; 

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint64_t*)&dc->mem_dirty_tbl[wa];
	st = *(uint64_t*)&dc->mem_dirty_tbl[ra];

	dift_log( "Q O %s[%p](%" PRIx64 ") %s%s %s[%p](%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				wa,
				dt,
				DIFT_DEBUG_COLOR_OPERATOR,
				DIFT_DEBUG_OP_ASSIGN,
				(st) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				ra,
				st );
}
#endif

    *(uint64_t*)&dc->mem_dirty_tbl[wa] = *(uint64_t*)&dc->mem_dirty_tbl[ra];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_reg_im_clear_mo8:
	i = rec.v1.r2m_m2r_byte.reg;
    k = rec.v1.r2m_m2r_byte.hl;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = dc->reg_dirty_tbl[i][k];

	dift_log( "B C %s%s[%d](%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				k,
				dt );
}
#endif

       dc->reg_dirty_tbl[i][k] = 0;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION 
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_reg_im_clear_mo16:
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint16_t*)&dc->reg_dirty_tbl[i][0];

	dift_log( "W C %s%s(%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				dt );
}
#endif

    *(uint16_t*)&dc->reg_dirty_tbl[i][0] = 0;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_reg_im_clear_mo32:
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint32_t*)&dc->reg_dirty_tbl[i][0];

	dift_log( "D C %s%s(%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				dt );
}
#endif

    *(uint32_t*)&dc->reg_dirty_tbl[i][0] = 0;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_reg_im_clear_mo64:
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint64_t*)&dc->reg_dirty_tbl[i][0];

	dift_log( "Q C %s%s(%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				REG_NAME[i],
				dt );
}
#endif

    *(uint64_t*)&dc->reg_dirty_tbl[i][0] = 0;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_mem_im_clear_mo8:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : null_sink; 

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = dc->mem_dirty_tbl[wa];

	dift_log( "B C %s[%p](%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				wa,
				dt );
}
#endif

	dc->mem_dirty_tbl[wa] = 0;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_mem_im_clear_mo16:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : null_sink; 

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint16_t*)&dc->mem_dirty_tbl[wa];

	dift_log( "W C %s[%p](%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				wa,
				dt );
}
#endif

    *(uint16_t*)&dc->mem_dirty_tbl[wa] = 0;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_mem_im_clear_mo32:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : null_sink; 

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint32_t*)&dc->mem_dirty_tbl[wa];

	dift_log( "D C %s[%p](%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				wa,
				dt );
}
#endif

    *(uint32_t*)&dc->mem_dirty_tbl[wa] = 0;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_mem_im_clear_mo64:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : null_sink; 

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dt = *(uint64_t*)&dc->mem_dirty_tbl[wa];

	dift_log( "Q C %s[%p](%" PRIx64 ")",
				(dt) ? DIFT_DEBUG_COLOR_TAINTED : DIFT_DEBUG_COLOR_CLEAN,
				wa,
				dt );
}
#endif

    *(uint64_t*)&dc->mem_dirty_tbl[wa] = 0;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_mem_hd:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : null_sink; 
    hdaddr = DEQ_FROM_ADDR();
    l = DEQ_FROM_ADDR();

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "M[%p] H[%p] %" PRIx64,
				wa,
				hdaddr,
				l );
}
#endif

    copy_contamination_mem_hd(dc, wa, hdaddr, l);

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{	
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_hd_mem:
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : clean_source; 
    hdaddr = DEQ_FROM_ADDR();
    l = DEQ_FROM_ADDR();

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "H[%p] M[%p] %" PRIx64,
				hdaddr,
				ra,
				l );
}
#endif

    copy_contamination_hd_mem(dc, hdaddr, ra, l);

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{	
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_rec_contaminate_mem_or:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : null_sink; 
    j = ((*(uint64_t*)&rec) & 0x000000000000ff00) >> 8;
    l = ((*(uint64_t*)&rec) & 0x0000ffffffff0000) >> 16;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{	
	dift_log( "T M[%p] %d %" PRIx64,
				wa,
				j,
				l );
}
#endif

    for( i = 0 ; i < l ; ++i )
        set_mem_dirty( dc, wa + i, j, 1 );

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{	
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_rec_contaminate_mem_and:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : null_sink; 
    j = ((*(uint64_t*)&rec) & 0x000000000000ff00) >> 8;
    l = ((*(uint64_t*)&rec) & 0x0000ffffffff0000) >> 16;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{	
	dift_log( "T M[%p] %d %" PRIx64,
				wa,
				j,
				l );
}
#endif

    for( i = 0 ; i < l ; ++i )
        and_mem_dirty( dc, wa + i, j );

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{	
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_rec_contaminate_hd_or:
    hdaddr = DEQ_FROM_ADDR();
    l      = DEQ_FROM_ADDR();
    j = ((*(uint64_t*)&rec) & 0x000000000000ff00) >> 8;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{	
	dift_log( "T H[%p] %d %" PRIx64,
				hdaddr,
				j,
				l );
}
#endif

    for(i = 0 ; i < l ; i++)
        set_hd_dirty_or(dc, hdaddr + i, j);

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{	
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_rec_contaminate_hd_and:
    hdaddr = DEQ_FROM_ADDR();
    l      = DEQ_FROM_ADDR();
    j = ((*(uint64_t*)&rec) & 0x000000000000ff00) >> 8;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{	
	dift_log( "T H[%p] %d %" PRIx64,
				hdaddr,
				j,
				l );
}
#endif

    for(i = 0 ; i < l ; i++)
        set_hd_dirty_and(dc, hdaddr + i, j);

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{	
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_rec_clear_mem:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : null_sink; 
    l  = DEQ_FROM_ADDR();

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "U M[%p] 0 %" PRIx64,
				wa,
				l );
}
#endif

    for(i = 0; i < l; i++)
        set_mem_dirty(dc, wa + i, 0, 0);

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{	
	dift_log( DIFT_DEBUG_COLOR_OPERATOR 
			  DIFT_DEBUG_OP_COMPLETE 
			  DIFT_DEBUG_COLOR_RESET );
}
#endif
    THREADED_DISPATCH();

rt_rec_block_begin:
#if defined(CONFIG_DIFT_DEBUG)
	dc->tb_rip = DEQ_FROM_CODE();
	dc->tb_tc_ptr = DEQ_FROM_CODE();
DIFT_DEBUG_CONDITION
{
	dift_log( "----------------\n" );
	dift_log( "TB: %p [%016lx]\n", dc->tb_tc_ptr, dc->tb_rip );
}
#endif
    THREADED_DISPATCH();

