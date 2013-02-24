#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"viewer.h"
#include	"viewcmn.h"
#include	"viewseg.h"
#include	"viewmem.h"
#include	"viewstk.h"
#include	"viewpaint.h"
#include	"cpucore.h"


void viewstk_reload(NP2VIEW_T *view)	{

	view->seg = CPU_SS;
	view->off = 0;
	view->pos = CPU_SP / view->bytesperline;
	view->cursor = seg_to_real(view->seg) + CPU_SP;
	viewcmn_setvscroll(view);
}

void viewstk_paint(NP2VIEW_T *view, RECT *rc, HDC hdc) {

	UINT32	x;
	LONG	y;
	UINT32	off;
	TCHAR	str[16];
	
	off = (view->pos) * view->bytesperline;

	viewmem_lock_alloc(view, ALLOCTYPE_SEG);

	for (y=0; y<rc->bottom && off<view->memsize; y+=viewcfg.font_height, off+=view->bytesperline) {
		UINT16 val;
		COLORREF	bkcol;

		x = 0;
		bkcol = viewpaint_line_set_colors(view, rc, hdc, y, off, CPU_SS, CPU_SP);

		// Draw a line at BP
		if(view->seg == CPU_SS && off == CPU_BP)	{
			viewpaint_line_fill(rc, hdc, y + viewcfg.font_height - 2, 2, viewcfg.color_text);
		}

		viewpaint_print_addr(hdc, x, y, view->seg, off, TRUE);
		x += 11 * np2viewfontwidth;

		if (view->lock) {
			UINT8 *p = (UINT8 *)view->buf1.ptr + off;
			val = *(UINT16 *)p;
		}
		else {
			viewmem_read(&view->dmem, seg_to_real(view->seg) + off, (UINT8 *)&val, sizeof(UINT16));
		}
		wsprintf(str, _T("%04x"), val);
		TextOut(hdc, x, y, str, 4);
	}
}

LRESULT CALLBACK viewstk_proc(NP2VIEW_T *view, HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
		case WM_PAINT:
			viewcmn_paint(view, viewstk_paint);
			break;
	}
	return(viewmem_proc(view, hwnd, msg, wp, lp));
}


// ---------------------------------------------------------------------------

void viewstk_init(NP2VIEW_T *dst, NP2VIEW_T *src) {

	dst->type = VIEWMODE_STK;
	dst->bytesperline = 2;
	dst->mul = 1;
	dst->memsize = 0x10000;
	dst->maxline = dst->memsize / dst->bytesperline;

	ZeroMemory(&dst->cp_bytes, sizeof(VIEWCELLPOS));
	ZeroMemory(&dst->cp_chars, sizeof(VIEWCELLPOS));	
}
