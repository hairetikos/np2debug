// break.c - Emulation activity control
// -------
// Author: Nmlgc

#include "compiler.h"
#include "np2.h"
#include "mousemng.h"
#include "scrnmng.h"
#include "soundmng.h"
#include "sysmng.h"
#include "keystat.h"
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
	if(np2stopemulate)
	{
		np2active_set(1);
		np2singlestep = 1;
	}
}

/// Breakpoints
/// -----------
LISTARRAY np2break_create()
{
	if(!np2breakpoints)	{
		np2breakpoints = listarray_new(sizeof(UINT32), 16);
	}
	return np2breakpoints;
}

static BOOL np2break_lookup(void *vpItem, void *vpArg) {
	CODEADDR16* seek = (CODEADDR16*)vpArg;
	CODEADDR16* cur = (CODEADDR16*)vpItem;
	UINT32 seek_real = (seek->cs << 4) + seek->ip;
	UINT32 cur_real = (cur->cs << 4) + cur->ip;
	return seek_real == cur_real;
}

BOOL np2break_toggle(UINT16 seg, UINT16 off)
{
	CODEADDR16* found_addr;

	// LISTARRAY doesn't support element deletion.
	// We work around that here by zeroing out the elements that should be deleted.
	// Once a new element should be appended, we check for these zeroed entries
	// in order to keep the list from becoming larger and larger over time.
	found_addr = np2break_is_set(seg, off);
	if(found_addr)	{
		// Disable
		ZeroMemory(found_addr, sizeof(CODEADDR16));
		return(TRUE);
	}
	
	// Look for a zeroed element
	found_addr = np2break_is_set(0, 0);
	if(!found_addr)	{
		// Nothing found, append a new one
		found_addr = (CODEADDR16*)listarray_append(np2breakpoints, NULL);
		if(!found_addr)	{
			return(FALSE);
		}
	}
	// Set this element
	found_addr->cs = seg;
	found_addr->ip = off;
	return(TRUE);
}

CODEADDR16* np2break_is_set(UINT16 seg, UINT16 off)
{
	CODEADDR16 lookup = {seg, off};
	return (CODEADDR16*)listarray_enum(np2breakpoints, np2break_lookup, &lookup);
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

