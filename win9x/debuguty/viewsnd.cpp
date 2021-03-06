#include	"compiler.h"
#include	"strres.h"
#include	"resource.h"
#include	"np2.h"
#include	"viewer.h"
#include	"viewcmn.h"
#include	"viewmenu.h"
#include	"viewsnd.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"sound.h"
#include	"fmboard.h"


typedef struct {
const TCHAR	*str;
	UINT16	reg;
	UINT16	mask;
} FMSNDTBL;

static const FMSNDTBL fmsndtbl[] = {
		{_T("Sound-Board I"), 0, 0},
		{NULL, 0x0000, 0xffff},
		{NULL, 0x0010, 0x3f07},
		{NULL, 0x0020, 0x07f6},
		{NULL, 0x0030, 0x7777},
		{NULL, 0x0040, 0x7777},
		{NULL, 0x0050, 0x7777},
		{NULL, 0x0060, 0x7777},
		{NULL, 0x0070, 0x7777},
		{NULL, 0x0080, 0x7777},
		{NULL, 0x0090, 0x7777},
		{NULL, 0x00a0, 0x7777},
		{NULL, 0x00b0, 0x0077},
		{str_null, 0, 0},
		{NULL, 0x0100, 0xffff},
		{NULL, 0x0110, 0x0001},
		{NULL, 0x0130, 0x7777},
		{NULL, 0x0140, 0x7777},
		{NULL, 0x0150, 0x7777},
		{NULL, 0x0160, 0x7777},
		{NULL, 0x0170, 0x7777},
		{NULL, 0x0180, 0x7777},
		{NULL, 0x0190, 0x7777},
		{NULL, 0x01a0, 0x7777},
		{NULL, 0x01b0, 0x0077},
#if 0
		{str_null, 0, 0},
		{_T("Sound-Board II"), 0, 0},
		{NULL, 0x0200, 0xffff},
		{NULL, 0x0220, 0x07e6},
		{NULL, 0x0230, 0x7777},
		{NULL, 0x0240, 0x7777},
		{NULL, 0x0250, 0x7777},
		{NULL, 0x0260, 0x7777},
		{NULL, 0x0270, 0x7777},
		{NULL, 0x0280, 0x7777},
		{NULL, 0x0290, 0x7777},
		{NULL, 0x02a0, 0x7777},
		{NULL, 0x02b0, 0x0077},
		{str_null, 0, 0},
		{NULL, 0x0230, 0x7777},
		{NULL, 0x0240, 0x7777},
		{NULL, 0x0250, 0x7777},
		{NULL, 0x0260, 0x7777},
		{NULL, 0x0270, 0x7777},
		{NULL, 0x0280, 0x7777},
		{NULL, 0x0290, 0x7777},
		{NULL, 0x02a0, 0x7777},
		{NULL, 0x02b0, 0x0077}
#endif
};

static void viewsnd_paint(NP2VIEW_T *view, RECT *rc, HDC hdc) {

	int		x;
	LONG	y;
	UINT	pos;
const UINT8	*p;
	TCHAR	str[16];
	UINT	reg;
	UINT16	mask;

	if (view->lock) {
		if (view->buf1.type != ALLOCTYPE_SND) {
			if (viewcmn_alloc(&view->buf1, 0x200)) {
				view->lock = FALSE;
				viewmenu_lock(view);
			}
			else {
				view->buf1.type = ALLOCTYPE_SND;
				CopyMemory(view->buf1.ptr, g_opna[0].s.reg, 0x200);
			}
			viewcmn_putcaption(view);
		}
	}

	pos = view->pos;
	for (y=0; (y<rc->bottom) && (pos<NELEMENTS(fmsndtbl)); y+=viewcfg.font_height, pos++) {
		if (fmsndtbl[pos].str) {
			TextOut(hdc, 0, y, fmsndtbl[pos].str, lstrlen(fmsndtbl[pos].str));
		}
		else {
			reg = fmsndtbl[pos].reg;
			mask = fmsndtbl[pos].mask;

			wsprintf(str, _T("%04x"), reg & 0x1ff);
			TextOut(hdc, 0, y, str, 4);

			if (view->lock) {
				p = (UINT8 *)view->buf1.ptr;
				p += reg;
			}
			else {
				p = p = g_opna[0].s.reg + reg;
			}
			for (x=0; x<16; x++) {
				if (mask & 1) {
					str[0] = viewcmn_hex[*p >> 4];
					str[1] = viewcmn_hex[*p & 15];
					str[2] = 0;
					TextOut(hdc, (6 + x*3) * 8, y, str, 2);
				}
				p++;
				mask >>= 1;
			}
		}
	}
}


LRESULT CALLBACK viewsnd_proc(NP2VIEW_T *view, HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
		case WM_PAINT:
			viewcmn_paint(view, viewsnd_paint);
			break;
	}
	return(0L);
}


// ---------------------------------------------------------------------------

void viewsnd_init(NP2VIEW_T *dst, NP2VIEW_T *src) {

	dst->type = VIEWMODE_SND;
	dst->maxline = NELEMENTS(fmsndtbl);
	dst->mul = 1;
	dst->pos = 0;
}

