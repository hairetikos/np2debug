#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"viewer.h"
#include	"viewcmn.h"
#include	"viewmenu.h"
#include	"viewmem.h"
#include	"viewseg.h"
#include	"cpucore.h"


static void set_viewseg(HWND hwnd, NP2VIEW_T *view, UINT16 seg) {

	if (view->seg != seg) {
		view->seg = seg;
		InvalidateRect(hwnd, NULL, TRUE);
	}
}


static void viewseg_paint(NP2VIEW_T *view, RECT *rc, HDC hdc) {

	viewmem_paint(view, rc, hdc, ALLOCTYPE_SEG, 0x10000, 16, true);
}


LRESULT CALLBACK viewseg_proc(NP2VIEW_T *view, HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {

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
			viewcmn_paint(view, color_back, viewseg_paint);
			break;

	}
	return(0L);
}


// ---------------------------------------------------------------------------

void viewseg_init(NP2VIEW_T *dst, NP2VIEW_T *src) {

	if (src) {
		switch(src->type) {
			case VIEWMODE_SEG:
				dst->seg = dst->seg;
				break;

			case VIEWMODE_1MB:
				if (dst->pos < 0x10000) {
					dst->seg = (UINT16)dst->pos;
				}
				else {
					dst->seg = 0xffff;
				}
				break;

			case VIEWMODE_ASM:
				dst->seg = src->seg;
				break;

			default:
				src = NULL;
				break;
		}
	}
	if (!src) {
		dst->seg = 0;
	}
	dst->type = VIEWMODE_SEG;
	dst->maxline = 0x1000;
	dst->mul = 1;
	dst->pos = 0;
}

