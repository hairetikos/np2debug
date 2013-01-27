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

void np2active_step_over()
{
	UINT8 ins[16];
	UINT step;
	UINT32 addr;
	_UNASM una;

	if(!np2stopemulate)	{
		return;
	}
	addr = (CPU_CS<<4)+CPU_IP;
	memp_reads(addr, ins, 16);
	step = unasm(&una, ins, 16, FALSE, addr);

	if(!strcmp(una.mnemonic, "call"))	{
		np2break_toggle(CPU_CS, CPU_IP + step, TRUE);
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

BOOL np2break_toggle(UINT16 seg, UINT16 off, BOOL oneshot)
{
	BREAKPOINT* found;

	// LISTARRAY doesn't support element deletion.
	// We work around that here by zeroing out the elements that should be deleted.
	// Once a new element should be appended, we check for these zeroed entries
	// in order to keep the list from becoming larger and larger over time.
	found = np2break_is_set(seg, off);
	if(found)	{
		// Disable
		ZeroMemory(found, sizeof(BREAKPOINT));
		return(TRUE);
	}
	
	// Look for a zeroed element
	found = np2break_is_set(0, 0);
	if(!found)	{
		// Nothing found, append a new one
		found = (BREAKPOINT*)listarray_append(np2breakpoints, NULL);
		if(!found)	{
			return(FALSE);
		}
	}
	// Set this element
	found->addr = (seg << 4) + off;
	found->oneshot = oneshot;
	return(TRUE);
}

BREAKPOINT* np2break_is_set(UINT16 seg, UINT16 off)
{
	BREAKPOINT lookup;
	lookup.addr = (seg << 4) + off;
	return (BREAKPOINT*)listarray_enum(np2breakpoints, np2break_lookup, &lookup);
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

