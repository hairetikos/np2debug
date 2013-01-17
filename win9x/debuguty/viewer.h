
#define	NP2VIEW_MAX	8

typedef struct {
	UINT8	vram;
	UINT8	itf;
	UINT8	A20;
} VIEWMEM_T;

enum {
	VIEWMODE_REG = 0,
	VIEWMODE_SEG,
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
	HWND		hwnd;
	VIEWMEMBUF	buf1;
	VIEWMEMBUF	buf2;
	UINT32		pos;
	UINT32		maxline;
	UINT16		step;
	UINT16		mul;
	UINT16		cursor;
	INT16		cursorline;
	UINT8		alive;
	UINT8		type;
	UINT8		lock;
	UINT8		active;
	UINT16		seg;
	UINT16		off;
	VIEWMEM_T	dmem;
	SCROLLINFO	si;
} NP2VIEW_T;

extern	const int	np2viewfontheight;
extern	const TCHAR		np2viewfont[];
extern	NP2VIEW_T		np2view[NP2VIEW_MAX];

// Colors
extern COLORREF color_back;
extern COLORREF color_text;
extern COLORREF color_cursor;
extern COLORREF color_hilite;
extern COLORREF color_active;

BOOL viewer_init(HINSTANCE hInstance);
void viewer_term(void);

void viewer_open(HINSTANCE hInstance);
void viewer_allclose(void);

void viewer_scroll_update(NP2VIEW_T *view, HWND hwnd, UINT32 newpos);

void viewer_allreload(BOOL force);