// break.h - Emulation activity control
// ------
// Author: Nmlgc

#ifdef __cplusplus
extern "C" {
#endif

// 16-bit address class

#if defined(__GNUC__)
typedef struct {
	UINT16	cs;
	UINT16	ip;
} __attribute__ ((packed)) CODEADDR16;
#else
#pragma pack(push, 1)
typedef struct {
	UINT16	cs;
	UINT16	ip;
} CODEADDR16;
#pragma pack(pop)
#endif

enum {
	NP2BREAK_RESUME		= 0x00,
	NP2BREAK_MAIN		= 0x01,
	NP2BREAK_DEBUG		= 0x02
};

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
/// --------

/// Breakpoints
/// -----------
LISTARRAY np2break_create();
BOOL np2break_toggle(UINT16 seg, UINT16 off);
CODEADDR16* np2break_is_set(UINT16 seg, UINT16 off);
void np2break_reset();
void np2break_destroy();
/// -----------

#ifdef __cplusplus
}
#endif
