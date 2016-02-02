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
	dift_log( "B I %s[%d] <- %s[%d]", 
				REG_NAME[i],
				j,
				REG_NAME[i],
				l );
}
#endif

    dc->reg_dirty_tbl[i][j] = dc->reg_dirty_tbl[i][l];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
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
	dift_log( "B I %s[%d] <= %s[%d]", 
				REG_NAME[i],
				j,
				REG_NAME[i],
				l );
}
#endif

    dc->reg_dirty_tbl[i][j] |= dc->reg_dirty_tbl[i][l];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION 
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
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
	dift_log( "B O %s[%d] <- %s[%d]", 
				REG_NAME[i],
				k,
				REG_NAME[j],
				l );
}
#endif

    dc->reg_dirty_tbl[i][k] = dc->reg_dirty_tbl[j][l];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_reg_reg_oo_assign_mo16:
	i = rec.v1.r2r.dreg;
    j = rec.v1.r2r.sreg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "W O %s <- %s", 
				REG_NAME[i],
				REG_NAME[j] );
}
#endif

    *(uint16_t*)&dc->reg_dirty_tbl[i][0] = *(uint16_t*)&dc->reg_dirty_tbl[j][0];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_reg_reg_oo_assign_mo32:
	i = rec.v1.r2r.dreg;
    j = rec.v1.r2r.sreg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "D O %s <- %s", 
				REG_NAME[i],
				REG_NAME[j] );
}
#endif

    *(uint32_t*)&dc->reg_dirty_tbl[i][0] = *(uint32_t*)&dc->reg_dirty_tbl[j][0];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_reg_reg_oo_assign_mo64:
	i = rec.v1.r2r.dreg;
    j = rec.v1.r2r.sreg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "Q O %s <- %s", 
				REG_NAME[i],
				REG_NAME[j] );
}
#endif

    *(uint64_t*)&dc->reg_dirty_tbl[i][0] = *(uint64_t*)&dc->reg_dirty_tbl[j][0];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
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
	dift_log( "B O %s[%d] <= %s[%d]", 
				REG_NAME[i],
				k,
				REG_NAME[j],
				l );
}
#endif

    dc->reg_dirty_tbl[i][k] |= dc->reg_dirty_tbl[j][l];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_reg_reg_oo_append_mo16:
	i = rec.v1.r2r.dreg;
    j = rec.v1.r2r.sreg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "W O %s <= %s", 
				REG_NAME[i],
				REG_NAME[j] );
}
#endif

    *(uint16_t*)&dc->reg_dirty_tbl[i][0] |= *(uint16_t*)&dc->reg_dirty_tbl[j][0];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_reg_reg_oo_append_mo32:
	i = rec.v1.r2r.dreg;
    j = rec.v1.r2r.sreg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "D O %s <= %s", 
				REG_NAME[i],
				REG_NAME[j] );
}
#endif

    *(uint32_t*)&dc->reg_dirty_tbl[i][0] |= *(uint32_t*)&dc->reg_dirty_tbl[j][0];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_reg_reg_oo_append_mo64:
	i = rec.v1.r2r.dreg;
    j = rec.v1.r2r.sreg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "Q O %s <= %s", 
				REG_NAME[i],
				REG_NAME[j] );
}
#endif

    *(uint64_t*)&dc->reg_dirty_tbl[i][0] |= *(uint64_t*)&dc->reg_dirty_tbl[j][0];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_reg_reg_mix_append_mo16: 
    i = rec.v1.r2r.dreg;
	j = rec.v1.r2r.sreg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "W M %s <= %s", 
				REG_NAME[i],
				REG_NAME[j] );
}
#endif

    t = dc->reg_dirty_tbl[i][0] | dc->reg_dirty_tbl[i][1]
        | dc->reg_dirty_tbl[j][0] | dc->reg_dirty_tbl[j][1];

    dc->reg_dirty_tbl[i][0] |= t;
    dc->reg_dirty_tbl[i][1] |= t;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_reg_reg_mix_append_mo32:
    i = rec.v1.r2r.dreg;
	j = rec.v1.r2r.sreg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "D M %s <= %s", 
				REG_NAME[i],
				REG_NAME[j] );
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
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_reg_reg_mix_append_mo64:
    i = rec.v1.r2r.dreg;
	j = rec.v1.r2r.sreg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "Q M %s <- %s", 
				REG_NAME[i],
				REG_NAME[j] );
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
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();   

rt_reg_mem_oo_assign_mo8:
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : phys_ram_size; 
    i = rec.v1.r2m_m2r_byte.reg;
    k = rec.v1.r2m_m2r_byte.hl;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "B O %s <- [%p]",
				REG_NAME[i],
				ra );			
}
#endif

#if defined(CONFIG_INDIRECT_TAINT)
    j = rec.v1.r2m_m2r_byte.reg_base;
    l = rec.v1.r2m_m2r_byte.reg_index;

	t = 0;
	t |= (j == R_NONE)? 0 : dc->reg_dirty_tbl[j][0];
	t |= (l == R_NONE)? 0 : dc->reg_dirty_tbl[l][0];

    dc->reg_dirty_tbl[i][k] = get_mem_dirty(dc, ra) | t;
#else
    dc->reg_dirty_tbl[i][k] = get_mem_dirty(dc, ra);
#endif

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_reg_mem_oo_assign_mo16:
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : phys_ram_size; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "W O %s <- [%p]",
				REG_NAME[i],
				ra );
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
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_reg_mem_oo_assign_mo32:
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : phys_ram_size; 
    i = rec.v1.r2m_m2r.reg;
#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "D O %s <- [%p]",
				REG_NAME[i],
				ra );
}
#endif

    *(uint32_t*)&dc->reg_dirty_tbl[i][0] = *(uint32_t*)&dc->mem_dirty_tbl[ra];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_reg_mem_oo_assign_mo64:
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : phys_ram_size; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "Q O %s <- [%p]",
				REG_NAME[i],
				ra );
}
#endif

    *(uint64_t*)&dc->reg_dirty_tbl[i][0] = *(uint64_t*)&dc->mem_dirty_tbl[ra];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_reg_mem_oo_append_mo8:
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : phys_ram_size; 
	i = rec.v1.r2m_m2r_byte.reg;
    k = rec.v1.r2m_m2r_byte.hl;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "B O %s <= [%p]",
				REG_NAME[i],
				ra );
}				
#endif

    t = get_mem_dirty(dc, ra);
    dc->reg_dirty_tbl[i][k] |= t;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION 
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_reg_mem_oo_append_mo16:
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : phys_ram_size; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "W O %s <= [%p]",
				REG_NAME[i],
				ra );
}
#endif

    t = get_mem_dirty(dc, ra);
    dc->reg_dirty_tbl[i][0] |= t;

    t = get_mem_dirty(dc, ra + 1);
    dc->reg_dirty_tbl[i][1] |= t;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_reg_mem_oo_append_mo32:
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : phys_ram_size; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "D O %s <= [%p]",
				REG_NAME[i],
				ra );
}
#endif

    *(uint32_t*)&dc->reg_dirty_tbl[i][0] |= *(uint32_t*)&dc->mem_dirty_tbl[ra];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_reg_mem_oo_append_mo64:
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : phys_ram_size; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "Q O %s <= [%p]",
				REG_NAME[i],
				ra );
}
#endif

    *(uint64_t*)&dc->reg_dirty_tbl[i][0] |= *(uint64_t*)&dc->mem_dirty_tbl[ra];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_reg_mem_mix_assign_mo16:
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : phys_ram_size; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "W M %s <- [%p]",
				REG_NAME[i],
				ra );
}
#endif

    t = get_mem_dirty(dc, ra) | get_mem_dirty(dc, ra + 1);
    dc->reg_dirty_tbl[i][0] = t;
    dc->reg_dirty_tbl[i][1] = t;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_reg_mem_mix_assign_mo32:
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : phys_ram_size; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "D M %s <- [%p]",
				REG_NAME[i],
				ra );
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
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_reg_mem_mix_assign_mo64:
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : phys_ram_size; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "Q M %s <- [%p]",
				REG_NAME[i],
				ra );
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
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_reg_mem_mix_append_mo16:
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : phys_ram_size; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "W M %s <= [%p]",
				REG_NAME[i],
				ra );
}
#endif

    t = get_mem_dirty(dc, ra) | get_mem_dirty(dc, ra + 1)
        | dc->reg_dirty_tbl[i][0] | dc->reg_dirty_tbl[i][1];
    dc->reg_dirty_tbl[i][0] |= t;
    dc->reg_dirty_tbl[i][1] |= t;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_reg_mem_mix_append_mo32:
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : phys_ram_size; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "D M %s <= [%p]",
				REG_NAME[i],
				ra );
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
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_reg_mem_mix_append_mo64:
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : phys_ram_size; 
    i = rec.v1.r2m_m2r.reg;
	
#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "Q M %s <= [%p]",
				REG_NAME[i],
				ra );
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
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_mem_reg_oo_assign_mo8:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : phys_ram_size + 1; 
	i = rec.v1.r2m_m2r_byte.reg;
    k = rec.v1.r2m_m2r_byte.hl;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "B O [%p] <- %s",
				wa,
				REG_NAME[i] );
}
#endif

    set_mem_dirty(dc, wa, dc->reg_dirty_tbl[i][k], 0);

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_mem_reg_oo_assign_mo16:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : phys_ram_size + 1; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "W O [%p] <- %s",
				wa,
				REG_NAME[i] );
}
#endif

    set_mem_dirty(dc, wa, dc->reg_dirty_tbl[i][0], 0);
    set_mem_dirty(dc, wa + 1, dc->reg_dirty_tbl[i][1], 0);

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_mem_reg_oo_assign_mo32:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : phys_ram_size + 1; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "D O [%p] <- %s",
				wa,
				REG_NAME[i] );
}
#endif

    *(uint32_t*)&dc->mem_dirty_tbl[wa] = *(uint32_t*)&dc->reg_dirty_tbl[i][0];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_mem_reg_oo_assign_mo64:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : phys_ram_size + 1; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "Q O [%p] <- %s",
				wa,
				REG_NAME[i] );
}
#endif

    *(uint64_t*)&dc->mem_dirty_tbl[wa] = *(uint64_t*)&dc->reg_dirty_tbl[i][0];

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_mem_reg_oo_append_mo8:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : phys_ram_size + 1; 
	i = rec.v1.r2m_m2r_byte.reg;
    k = rec.v1.r2m_m2r_byte.hl;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "B O [%p] <= %s",
				wa,
				REG_NAME[i] );
}
#endif

    set_mem_dirty(dc, wa, dc->reg_dirty_tbl[i][k], 1);

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_mem_reg_oo_append_mo16:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : phys_ram_size + 1; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "W O [%p] <= %s",
				wa,
				REG_NAME[i] );
}
#endif

    set_mem_dirty(dc, wa, dc->reg_dirty_tbl[i][0], 1);
    set_mem_dirty(dc, wa + 1, dc->reg_dirty_tbl[i][1], 1);

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_mem_reg_oo_append_mo32:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : phys_ram_size + 1; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "D O [%p] <= %s",
				wa,
				REG_NAME[i] );
}
#endif

    set_mem_dirty(dc, wa, dc->reg_dirty_tbl[i][0], 1);
    set_mem_dirty(dc, wa + 1, dc->reg_dirty_tbl[i][1], 1);
    set_mem_dirty(dc, wa + 2, dc->reg_dirty_tbl[i][2], 1);
    set_mem_dirty(dc, wa + 3, dc->reg_dirty_tbl[i][3], 1);

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_mem_reg_oo_append_mo64:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : phys_ram_size + 1; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "Q O [%p] <= %s",
				wa,
				REG_NAME[i] );
}
#endif

    set_mem_dirty(dc, wa,     dc->reg_dirty_tbl[i][0], 1);
    set_mem_dirty(dc, wa + 1, dc->reg_dirty_tbl[i][1], 1);
    set_mem_dirty(dc, wa + 2, dc->reg_dirty_tbl[i][2], 1);
    set_mem_dirty(dc, wa + 3, dc->reg_dirty_tbl[i][3], 1);
    set_mem_dirty(dc, wa + 4, dc->reg_dirty_tbl[i][4], 1);
    set_mem_dirty(dc, wa + 5, dc->reg_dirty_tbl[i][5], 1);
    set_mem_dirty(dc, wa + 6, dc->reg_dirty_tbl[i][6], 1);
    set_mem_dirty(dc, wa + 7, dc->reg_dirty_tbl[i][7], 1);

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_mem_reg_mix_assign_mo16:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : phys_ram_size + 1; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "W M [%p] <- %s",
				wa,
				REG_NAME[i] );
}
#endif

    t = dc->reg_dirty_tbl[i][0] | dc->reg_dirty_tbl[i][1];

    set_mem_dirty(dc, wa, t, 0);
    set_mem_dirty(dc, wa + 1, t, 0);

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_mem_reg_mix_assign_mo32:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : phys_ram_size + 1; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "D M [%p] <- %s",
				wa,
				REG_NAME[i] );
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
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_mem_reg_mix_assign_mo64:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : phys_ram_size + 1; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "Q M [%p] <- %s",
				wa,
				REG_NAME[i] );
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
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_mem_reg_mix_append_mo16:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : phys_ram_size + 1; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "W M [%p] <= %s",
				wa,
				REG_NAME[i] );
}
#endif

    t = dc->reg_dirty_tbl[i][0] | dc->reg_dirty_tbl[i][1]
        | get_mem_dirty(dc, wa) | get_mem_dirty(dc, wa + 1);

    set_mem_dirty(dc, wa, t, 1);
    set_mem_dirty(dc, wa + 1, t, 1);

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_mem_reg_mix_append_mo32:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : phys_ram_size + 1; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "D M [%p] <= %s",
				wa,
				REG_NAME[i] );
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
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_mem_reg_mix_append_mo64:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : phys_ram_size + 1; 
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "Q M [%p] <= %s",
				wa,
				REG_NAME[i] );
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
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_mem_mem_oo_assign_mo8:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : phys_ram_size + 1; 
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : phys_ram_size; 

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "B O [%p] <- [%p]",
				wa,
				ra );
}
#endif

    set_mem_dirty(dc, wa, get_mem_dirty(dc, ra), 0);

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_mem_mem_oo_assign_mo16:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : phys_ram_size + 1; 
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : phys_ram_size; 

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "W O [%p] <- [%p]",
				wa,
				ra );
}
#endif

    set_mem_dirty(dc, wa, get_mem_dirty(dc, ra), 0);
    set_mem_dirty(dc, wa + 1, get_mem_dirty(dc, ra + 1), 0);

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_mem_mem_oo_assign_mo32:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : phys_ram_size + 1; 
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : phys_ram_size; 

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "D O [%p] <- [%p]",
				wa,
				ra );
}
#endif

    set_mem_dirty(dc, wa, get_mem_dirty(dc, ra), 0);
    set_mem_dirty(dc, wa + 1, get_mem_dirty(dc, ra + 1), 0);
    set_mem_dirty(dc, wa + 2, get_mem_dirty(dc, ra + 2), 0);
    set_mem_dirty(dc, wa + 3, get_mem_dirty(dc, ra + 3), 0);

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_mem_mem_oo_assign_mo64:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : phys_ram_size + 1; 
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : phys_ram_size; 

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "Q O [%p] <- [%p]",
				wa,
				ra );
}
#endif

    set_mem_dirty(dc, wa,     get_mem_dirty(dc, ra),     0);
    set_mem_dirty(dc, wa + 1, get_mem_dirty(dc, ra + 1), 0);
    set_mem_dirty(dc, wa + 2, get_mem_dirty(dc, ra + 2), 0);
    set_mem_dirty(dc, wa + 3, get_mem_dirty(dc, ra + 3), 0);
    set_mem_dirty(dc, wa + 4, get_mem_dirty(dc, ra + 4), 0);
    set_mem_dirty(dc, wa + 5, get_mem_dirty(dc, ra + 5), 0);
    set_mem_dirty(dc, wa + 6, get_mem_dirty(dc, ra + 6), 0);
    set_mem_dirty(dc, wa + 7, get_mem_dirty(dc, ra + 7), 0);

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_reg_im_clear_mo8:
	i = rec.v1.r2m_m2r_byte.reg;
    k = rec.v1.r2m_m2r_byte.hl;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "B C: %s",
				REG_NAME[i] );
}
#endif

       dc->reg_dirty_tbl[i][k] = 0;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION 
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_reg_im_clear_mo16:
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "W C %s",
				REG_NAME[i] );
}
#endif

    *(uint16_t*)&dc->reg_dirty_tbl[i][0] = 0;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_reg_im_clear_mo32:
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "D C %s",
				REG_NAME[i] );
}
#endif

    *(uint32_t*)&dc->reg_dirty_tbl[i][0] = 0;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_reg_im_clear_mo64:
    i = rec.v1.r2m_m2r.reg;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "Q C %s",
				REG_NAME[i] );
}
#endif

    *(uint64_t*)&dc->reg_dirty_tbl[i][0] = 0;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_mem_im_clear_mo8:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : phys_ram_size + 1; 

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "B C [%p]",
				wa );
}
#endif

    set_mem_dirty(dc, wa, 0, 0);

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_mem_im_clear_mo16:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : phys_ram_size + 1; 

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "W C [%p]",
				wa );
}
#endif

    *(uint16_t*)&dc->mem_dirty_tbl[wa] = 0;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_mem_im_clear_mo32:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : phys_ram_size + 1; 

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "D C [%p]",
				wa );
}
#endif

    *(uint32_t*)&dc->mem_dirty_tbl[wa] = 0;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_mem_im_clear_mo64:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : phys_ram_size + 1; 

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( "Q C [%p]",
				wa );
}
#endif

    *(uint64_t*)&dc->mem_dirty_tbl[wa] = 0;

#if defined(CONFIG_DIFT_DEBUG)
DIFT_DEBUG_CONDITION
{
	dift_log( DIFT_DEBUG_COMPLETE_SIGN );
}
#endif
    THREADED_DISPATCH();

rt_mem_hd:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : phys_ram_size + 1; 
    hdaddr = DEQ_FROM_ADDR();
    l = DEQ_FROM_ADDR();
    copy_contamination_mem_hd(dc, wa, hdaddr, l);
    THREADED_DISPATCH();

rt_hd_mem:
    ra = DEQ_FROM_ADDR();
	ra = (ra < phys_ram_size) ? ra : phys_ram_size; 
    hdaddr = DEQ_FROM_ADDR();
    l = DEQ_FROM_ADDR();
    copy_contamination_hd_mem(dc, hdaddr, ra, l);
    THREADED_DISPATCH();

rt_rec_contaminate_mem_or:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : phys_ram_size + 1; 
    j = ((*(uint64_t*)&rec) & 0x000000000000ff00) >> 8;
    l = ((*(uint64_t*)&rec) & 0x0000ffffffff0000) >> 16;
    for(i = 0 ; i < l ; i++)
        set_mem_dirty(dc, wa + i, j, 1);
	dift_log( "rec_contaminate_mem_or\n" );
    THREADED_DISPATCH();

rt_rec_contaminate_mem_and:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : phys_ram_size + 1; 
    j = ((*(uint64_t*)&rec) & 0x000000000000ff00) >> 8;
    l = ((*(uint64_t*)&rec) & 0x0000ffffffff0000) >> 16;
    for(i = 0 ; i < l ; i++)
        and_mem_dirty(dc, wa + i, j);
	dift_log( "rec_contaminate_mem_and\n" );
    THREADED_DISPATCH();

rt_rec_contaminate_hd_or:
    hdaddr = DEQ_FROM_ADDR();
    l      = DEQ_FROM_ADDR();

    j = ((*(uint64_t*)&rec) & 0x000000000000ff00) >> 8;
    for(i = 0 ; i < l ; i++)
        set_hd_dirty_or(dc, hdaddr + i, j);
	dift_log( "rec_contaminate_hd_or\n" );
    THREADED_DISPATCH();

rt_rec_contaminate_hd_and:
    hdaddr = DEQ_FROM_ADDR();
    l      = DEQ_FROM_ADDR();

    j = ((*(uint64_t*)&rec) & 0x000000000000ff00) >> 8;
    for(i = 0 ; i < l ; i++)
        set_hd_dirty_and(dc, hdaddr + i, j);
	dift_log( "rec_contaminate_hd_and\n" );
    THREADED_DISPATCH();

rt_rec_clear_mem:
    wa = DEQ_FROM_ADDR();
	wa = (wa < phys_ram_size) ? wa : phys_ram_size + 1; 
    l  = DEQ_FROM_ADDR();

    for(i = 0; i < l; i++)
        set_mem_dirty(dc, wa + i, 0, 0);
	dift_log( "rec_clear_mem\n" );
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

