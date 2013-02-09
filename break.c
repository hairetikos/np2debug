// break.c - Emulation activity control
// -------
// Author: Nmlgc

#include "compiler.h"
#include "np2.h"
#include "mousemng.h"
#include "scrnmng.h"
#include "soundmng.h"
#include "sysmng.h"
#include "unasm.h"
#include "keystat.h"
#include "cpucore.h"
#include "break.h"
#include "unasmdef.tbl"
#include "viewer.h"
#include "viewmenu.h"
#include "viewstat.h"

/// Globals
/// =======
UINT8	np2stopemulate = 0;
BOOL 	np2singlestep = 0;
LISTARRAY np2breakpoints = NULL;
/// =======

void np2active_renewal(UINT8 breakflag) {										// ver0.30

	if (breakflag & (~NP2BREAK_MAIN)) {
		np2stopemulate = 2;
		soundmng_disable(SNDPROC_MASTER);
	}
	else if (breakflag & NP2BREAK_MAIN) {
		if (np2oscfg.background & 1) {
			np2stopemulate = 1;
		}
		else {
			np2stopemulate = 0;
		}
		if (np2oscfg.background) {
			soundmng_disable(SNDPROC_MASTER);
		}
		else {
			soundmng_enable(SNDPROC_MASTER);
		}
	}
	else {
		np2stopemulate = 0;
		soundmng_enable(SNDPROC_MASTER);
	}
	viewmenu_all_debug_toggle(!np2stopemulate);
	sysmng_updatecaption(0);
}

void np2active_set(int active)
{
	if (active) {
		scrnmng_update();
		keystat_allrelease();
		mousemng_enable(MOUSEPROC_BG);
	}
	else {
		mousemng_disable(MOUSEPROC_BG);
	}
	np2active_renewal(active ? NP2BREAK_RESUME : NP2BREAK_DEBUG);
}

void np2active_step()
{	
	if(!np2stopemulate)	{
		return;
	}
	np2active_set(1);
	np2singlestep = 1;
}

void np2active_step_over()	{

	UINT step;
	_UNASM una;

	if(!np2stopemulate)	{
		return;
	}
	step = unasm_next(&una);
	if(
		!strcmp(una.mnemonic, "call") ||
		!strcmp(una.mnemonic, "int")
	)	{
		np2break_toggle(CPU_CS, CPU_IP + step, NP2BP_EXECUTE | NP2BP_ONESHOT);
		np2active_set(1);
	} else {
		np2active_step();
	}
}

/// Breakpoints
/// -----------
LISTARRAY np2break_create()
{
	if(!np2breakpoints)	{
		np2breakpoints = listarray_new(sizeof(BREAKPOINT), 16);
	}
	return np2breakpoints;
}

static BOOL np2break_lookup(void *vpItem, void *vpArg) {
	BREAKPOINT *seek = (BREAKPOINT*)vpArg;
	BREAKPOINT *cur = (BREAKPOINT*)vpItem;
	return seek->addr == cur->addr;
}

BOOL np2break_toggle_real(UINT32 addr, UINT8 flag)
{
	BREAKPOINT* found;

	// LISTARRAY doesn't support element deletion.
	// We work around that here by zeroing out the elements that should be deleted.
	// Once a new element should be appended, we check for these zeroed entries
	// in order to keep the list from becoming larger and larger over time.
	found = np2break_is_set_real(addr);
	if(found)	{
		// Disable
		ZeroMemory(found, sizeof(BREAKPOINT));
		return(TRUE);
	}
	
	// Look for a zeroed element
	found = np2break_is_set_real(0);
	if(!found)	{
		// Nothing found, append a new one
		found = (BREAKPOINT*)listarray_append(np2breakpoints, NULL);
		if(!found)	{
			return(FALSE);
		}
	}
	// Set this element
	found->addr = addr;
	found->flag = flag;
	return(TRUE);
}

BOOL np2break_toggle(UINT16 seg, UINT16 off, UINT8 flag)	{
	return np2break_toggle_real((seg << 4) + off, flag);
}

BREAKPOINT* np2break_is_set_real(UINT32 addr)
{
	BREAKPOINT lookup;
	lookup.addr = addr;
	return (BREAKPOINT*)listarray_enum(np2breakpoints, np2break_lookup, &lookup);
}

BREAKPOINT* np2break_is_set(UINT16 seg, UINT16 off)	{
	return np2break_is_set_real((seg << 4) + off);
}

static BREAKPOINT* np2break_is_flag(UINT16 seg, UINT16 off, UINT8 flag)
{
	BREAKPOINT* lookup = np2break_is_set(seg, off);
	if(lookup && lookup->flag & flag)	{
		return lookup;
	}
	return NULL;
}

BREAKPOINT* np2break_is_exec(UINT16 seg, UINT16 off)	{
	return np2break_is_flag(seg, off, NP2BP_EXECUTE);
}
BREAKPOINT* np2break_is_read(UINT16 seg, UINT16 off)	{
	return np2break_is_flag(seg, off, NP2BP_READ);
}
BREAKPOINT* np2break_is_write(UINT16 seg, UINT16 off)	{
	return np2break_is_flag(seg, off, NP2BP_WRITE);
}

static UINT8 is_mem_type(const UINT8 type)	{

	switch(type)	{
		case OP_MEM:
		case OP_EA:
		case OP_PEA:
			return TRUE;
	}
	return FALSE;
}

// This is kept in here to catch edge cases not yet covered by unasm
static UINT32 np2break_memory_write_naive()	{

	static UINT bp_num = 0;
	static UINT8* probe = NULL;
	UINT bp_num_new = listarray_getitems(np2breakpoints);
	UINT i;
	if(bp_num_new > bp_num)	{
		probe = (UINT8*)realloc(probe, bp_num_new);
		bp_num = bp_num_new;
	}
	for(i = 0; i < bp_num; i++)	{
		BREAKPOINT* bp = (BREAKPOINT*)listarray_getitem(np2breakpoints, i);
		if(bp->addr && bp->flag & NP2BP_WRITE)	{
			UINT8 cur_val = mem[bp->addr];
			if(probe[i] != cur_val)	{
				probe[i] = cur_val;
				return bp->addr;
			}
		}
	}
	return 0;
}

UINT32 np2break_is_next()	{

	_UNASM una;
	BREAKPOINT* bp = NULL;
	UINT32 ret = 0;
	UINT8 type;

#ifdef DEBUG
	ret = np2break_memory_write_naive();
	if(ret)	{
		viewstat_all_breakpoint(NP2BP_WRITE, ret);
		return ret;
	}
#endif
	if(!listarray_getitems(np2breakpoints))	{
		return 0;
	}

	unasm_next(&una);
	if(
		una.off && 
		(stricmp(una.mnemonic, "lea") && stricmp(una.mnemonic, "les"))
	  )	{
		if(is_mem_type(una.type_oper))	{
			bp = np2break_is_read(una.seg, una.off);
			type = NP2BP_READ;
#ifndef DEBUG
		} else if(is_mem_type(una.type_targ))	{
			bp = np2break_is_write(una.seg, una.off);
			type = NP2BP_WRITE;
#endif
		}
	}
	if(!bp)	{
		bp = np2break_is_exec(CPU_CS, CPU_EIP);
		type = NP2BP_EXECUTE;
	}
	if(bp)	{
		ret = bp->addr;
		if(bp->flag & NP2BP_ONESHOT)	{
			bp->addr = 0;
		}
		viewstat_all_breakpoint(type, ret);
	}
	return ret;
}

void np2break_reset()
{
	listarray_clr(np2breakpoints);
}

void np2break_destroy()
{
	listarray_destroy(np2breakpoints);
}
/// -----------

/// Helper
/// ------
UINT unasm_next(UNASM una)	{

	UINT32 addr;
	UINT8 ins[16];

	addr = (CPU_CS<<4)+CPU_IP;
	memp_reads(addr, ins, 16);
	return unasm(una, ins, 16, FALSE, addr);
}
/// ------
