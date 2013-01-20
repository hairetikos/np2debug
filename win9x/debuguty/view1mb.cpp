#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"viewer.h"
#include	"viewcmn.h"
#include	"viewmenu.h"
#include	"viewmem.h"
#include	"view1mb.h"
#include	"cpucore.h"


static void set_viewseg(HWND hwnd, NP2VIEW_T *view, UINT16 seg) {

	UINT32	pos;

	pos = (UINT32)seg;
	if (view->pos != pos) {
		view->pos = pos;
		viewcmn_setvscroll(hwnd, view);
		InvalidateRect(hwnd, NULL, TRUE);
	}
}


static void view1mb_paint(NP2VIEW_T *view, RECT *rc, HDC hdc) {

	viewmem_paint(view, rc, hdc, ALLOCTYPE_1MB, 0x10fff0, false);
}


LRESULT CALLBACK view1mb_proc(NP2VIEW_T *view, HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
		case WM_COMMAND:
			switch(LOWORD(wp)) {
				case IDM_SEGCS:
					set_viewseg(hwnd, view, CPU_CS);
					break;

				case IDM_SEGDS:
					set_viewseg(hwnd, view, CPU_DS);
					break;

				case IDM_SEGES:
					set_viewseg(hwnd, view, CPU_ES);
					break;

				case IDM_SEGSS:
					set_viewseg(hwnd, view, CPU_SS);
					break;

				case IDM_SEGTEXT:
					set_viewseg(hwnd, view, 0xa000);
					break;

				case IDM_VIEWMODELOCK:
					view->lock ^= 1;
					viewmenu_lock(view);
					viewcmn_putcaption(view);
					InvalidateRect(hwnd, NULL, TRUE);
					break;
			}
			break;

		case WM_PAINT:
			viewcmn_paint(view, view1mb_paint);
	}
	return(viewmem_proc(view, hwnd, msg, wp, lp));
}


// ---------------------------------------------------------------------------

void view1mb_init(NP2VIEW_T *dst, NP2VIEW_T *src) {

	if (src) {
		switch(src->type) {
			case VIEWMODE_SEG:
				dst->pos = src->seg;
				break;

			case VIEWMODE_1MB:
				dst->pos = src->pos;
				break;

			case VIEWMODE_ASM:
				dst->pos = src->seg;
				break;

			default:
				src = NULL;
				break;
		}
	}
	if (!src) {
		dst->pos = 0;
	}
	dst->type = VIEWMODE_1MB;
	dst->maxline = 0x10fff;
	dst->mul = 2;
	dst->seg = 0;
}
