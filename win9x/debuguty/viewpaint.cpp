#include	"compiler.h"
#include	"viewer.h"
#include	"viewpaint.h"
#include	"cpucore.h"
#include	"break.h"


void viewpaint_line_fill(RECT *rc, HDC hdc, DWORD top, DWORD height, COLORREF col)	{

	HBRUSH hbrush = CreateSolidBrush(col);
	RECT rect;
	rect.left = 0;
	rect.top = top;
	rect.right = rc->right;
	rect.bottom = top + height;
	FillRect(hdc, &rect, hbrush);
	DeleteObject(hbrush);
}

COLORREF viewpaint_line_set_colors(NP2VIEW_T *view, RECT *rc, HDC hdc, DWORD y, UINT16 off, UINT16 cpu_seg, UINT16 cpu_off)	{

	COLORREF bkcol = viewcfg.color_back;
	UINT32 addr = seg_to_real(view->seg) + off;
	// Current registers?
	if(view->seg == cpu_seg && off == cpu_off)	{
		viewpaint_line_fill(rc, hdc, y, viewcfg.font_height, viewcfg.color_text);
		bkcol = viewcfg.color_text;
		SetTextColor(hdc, viewcfg.color_active);
	}
	else		{
		SetTextColor(hdc, viewcfg.color_text);
	}
	// Cursor?
	if(addr == view->cursor)	{
		viewpaint_line_fill(rc, hdc, y, viewcfg.font_height, viewcfg.color_cursor);
		bkcol = viewcfg.color_cursor;
	}

	// Breakpoint?
	if(*np2break_lookup(NULL, view->seg, off) != NP2BP_NONE) {
		SetBkColor(hdc, viewcfg.color_hilite);
	}
	else {
		SetBkColor(hdc, bkcol);
	}
	return bkcol;
}

void viewpaint_print_addr(HDC hdc, int x, int y, UINT16 seg, UINT32 off, BOOL segmented)	{

	TCHAR	str[16];

	if(segmented)	{
		wsprintf(str, _T("%04x:%04x"), seg, off);
	} else {
		wsprintf(str, _T("%08x"), off);
	}

	TextOut(hdc, x, y, str, 9);
}
