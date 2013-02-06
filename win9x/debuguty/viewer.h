
#define	NP2VIEW_MAX	8

typedef struct {
	UINT8	vram;
	UINT8	itf;
	UINT8	A20;
} VIEWMEM_T;

enum {
	VIEWMODE_REG = 0,
	VIEWMODE_SEG,
	VIEWMODE_STK,
	VIEWMODE_1MB,
	VIEWMODE_ASM,
	VIEWMODE_SND
};

enum {
	ALLOCTYPE_NONE = 0,
	ALLOCTYPE_REG,
	ALLOCTYPE_SEG,
	ALLOCTYPE_1MB,
	ALLOCTYPE_ASM,
	ALLOCTYPE_SND,

	ALLOCTYPE_ERROR = 0xffffffff
};

typedef struct {
	UINT32	type;
	UINT32	arg;
	UINT32	size;
	void	*ptr;
} VIEWMEMBUF;

typedef struct {
	LONG	left;
	LONG	cell;
	LONG	right;
} VIEWCELLPOS;

typedef struct {
	HWND		hwnd;
	HWND		clientwnd;
	HWND		helpwnd;
	HWND		statwnd;
	VIEWMEMBUF	buf1;
	VIEWMEMBUF	buf2;
	UINT32		pos;
	UINT32		memsize;
	UINT16		bytesperline;
	UINT32		maxline;
	UINT16		step;
	UINT16		mul;
	// in segmented modes, this always contains the absolute memory address at the cursor position
	LONG		cursor;
	INT16		cursorline;
	UINT8		alive;
	UINT8		type;
	UINT8		lock;
	UINT8		active;
	UINT16		seg;
	UINT16		off;
	VIEWMEM_T	dmem;
	VIEWCELLPOS	cp_bytes;
	VIEWCELLPOS	cp_chars;
	SCROLLINFO	si;
} NP2VIEW_T;

typedef struct {
	OEMCHAR	font_name[64];
	int	font_height;

	COLORREF color_text;
	COLORREF color_dim;
	COLORREF color_back;
	COLORREF color_cursor;
	COLORREF color_hilite;
	COLORREF color_active;

	POINTS size_asm;
	POINTS size_mem;
	POINTS size_stk;
	POINTS size_reg;
	POINTS size_snd;
} VIEWCFG;

extern	VIEWCFG viewcfg;

extern	const OEMCHAR viewerapp[];

extern	HFONT		np2viewfont;
extern	int         	np2viewfontwidth;
extern	NP2VIEW_T 	np2view[NP2VIEW_MAX];

BOOL viewer_init(HINSTANCE hInstance);
void viewer_term(void);

void viewer_open(HINSTANCE hInstance);
void viewer_allclose(void);

void viewer_scroll_update(NP2VIEW_T *view, UINT32 newpos);
void viewer_scroll_fit_line(NP2VIEW_T *view, LONG line);

void viewer_edit_dlg(NP2VIEW_T *view, HWND hWnd);

void viewer_allreload(BOOL force);

void viewer_readini(void);
void viewer_writeini(void);
