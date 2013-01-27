// break.h - Emulation activity control
// ------
// Author: Nmlgc

#ifdef __cplusplus
extern "C" {
#endif

enum {
	NP2BREAK_RESUME		= 0x00,
	NP2BREAK_MAIN		= 0x01,
	NP2BREAK_DEBUG		= 0x02
};

typedef struct {
	UINT32	addr;
	BOOL  	oneshot;
} BREAKPOINT;

/// Globals
/// =======
extern	UINT8	np2stopemulate;
extern	BOOL 	np2singlestep;
extern	LISTARRAY np2breakpoints;
/// =======

/// Activity
/// --------
void np2active_renewal(UINT8 breakflag);
void np2active_set(int active);
// Sets the single-step flag and resumes emulation.
// Next CPU loop will perform one instruction only, then pause the emulation again
void np2active_step();
void np2active_step_over();
/// --------

/// Breakpoints
/// -----------
LISTARRAY np2break_create();
BOOL np2break_toggle(UINT16 seg, UINT16 off, BOOL oneshot);
BREAKPOINT* np2break_is_set(UINT16 seg, UINT16 off);
void np2break_reset();
void np2break_destroy();
/// -----------

#ifdef __cplusplus
}
#endif
