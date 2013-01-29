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

enum {
	NP2BP_READ		= 0x01,
	NP2BP_WRITE		= 0x02,
	NP2BP_EXECUTE		= 0x04,
	NP2BP_ONESHOT		= 0x08
};

typedef struct {
	UINT32	addr;
	UINT8 	flag;
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
BOOL np2break_toggle(UINT16 seg, UINT16 off, UINT8 flags);
BREAKPOINT* np2break_is_set_real(UINT32 addr);
BREAKPOINT* np2break_is_set(UINT16 seg, UINT16 off);
BREAKPOINT* np2break_is_exec(UINT16 seg, UINT16 off);
BREAKPOINT* np2break_is_read(UINT16 seg, UINT16 off);
BREAKPOINT* np2break_is_write(UINT16 seg, UINT16 off);
void np2break_reset();
void np2break_destroy();
/// -----------

#ifdef __cplusplus
}
#endif
