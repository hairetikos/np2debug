#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"viewer.h"
#include	"viewcmn.h"
#include	"viewmenu.h"
#include	"viewmem.h"
#include	"cpucore.h"

const float CHAR_SPACING = 1.5f;
	UINT16 bytesperline = 16;


void viewmem_read(VIEWMEM_T *cfg, UINT32 adrs, UINT8 *buf, UINT32 size) {

	if (!size) {
		return;
	}

	// Main Memory
	if (adrs < 0xa4000) {
		if ((adrs + size) <= 0xa4000) {
			CopyMemory(buf, mem + adrs, size);
			return;
		}
		else {
			UINT32 len;
			len = 0xa4000 - adrs;
			CopyMemory(buf, mem + adrs, len);
			buf += len;
			size -= len;
			adrs = 0xa4000;
		}
	}

	// CG-Windowは無視
	if (adrs < 0xa5000) {
		if ((adrs + size) <= 0xa5000) {
			ZeroMemory(buf, size);
			return;
		}
		else {
			UINT32 len;
			len = 0xa5000 - adrs;
			ZeroMemory(buf, len);
			buf += len;
			size -= len;
			adrs = 0xa5000;
		}
	}

	// Main Memory
	if (adrs < 0xa8000) {
		if ((adrs + size) <= 0xa8000) {
			CopyMemory(buf, mem + adrs, size);
			return;
		}
		else {
			UINT32 len;
			len = 0xa8000 - adrs;
			CopyMemory(buf, mem + adrs, len);
			buf += len;
			size -= len;
			adrs = 0xa8000;
		}
	}

	// Video Memory
	if (adrs < 0xc0000) {
		UINT32 page;
		page = ((cfg->vram)?VRAM_STEP:0);
		if ((adrs + size) <= 0xc0000) {
			CopyMemory(buf, mem + page + adrs, size);
			return;
		}
		else {
			UINT32 len;
			len = 0xc0000 - adrs;
			CopyMemory(buf, mem + page + adrs, len);
			buf += len;
			size -= len;
			adrs = 0xc0000;
		}
	}

	// Main Memory
	if (adrs < 0xe0000) {
		if ((adrs + size) <= 0xe0000) {
			CopyMemory(buf, mem + adrs, size);
			return;
		}
		else {
			UINT32 len;
			len = 0xe0000 - adrs;
			CopyMemory(buf, mem + adrs, len);
			buf += len;
			size -= len;
			adrs = 0xe0000;
		}
	}

	// Video Memory
	if (adrs < 0xe8000) {
		UINT32 page;
		page = ((cfg->vram)?VRAM_STEP:0);
		if ((adrs + size) <= 0xe8000) {
			CopyMemory(buf, mem + page + adrs, size);
			return;
		}
		else {
			UINT32 len;
			len = 0xe8000 - adrs;
			CopyMemory(buf, mem + page + adrs, len);
			buf += len;
			size -= len;
			adrs = 0xe8000;
		}
	}

	// BIOS
	if (adrs < 0x0f8000) {
		if ((adrs + size) <= 0x0f8000) {
			CopyMemory(buf, mem + adrs, size);
			return;
		}
		else {
			UINT32 len;
			len = 0x0f8000 - adrs;
			CopyMemory(buf, mem + adrs, len);
			buf += len;
			size -= len;
			adrs = 0x0f8000;
		}
	}

	// BIOS/ITF
	if (adrs < 0x100000) {
		UINT32 page;
		page = ((cfg->itf)?VRAM_STEP:0);
		if ((adrs + size) <= 0x100000) {
			CopyMemory(buf, mem + page + adrs, size);
			return;
		}
		else {
			UINT32 len;
			len = 0x100000 - adrs;
			CopyMemory(buf, mem + page + adrs, len);
			buf += len;
			size -= len;
			adrs = 0x100000;
		}
	}

	// HMA
	if (adrs < 0x10fff0) {
		UINT32 adrs2;
		adrs2 = adrs & 0xffff;
		adrs2 += ((cfg->A20)?VRAM_STEP:0);
		if ((adrs + size) <= 0x10fff0) {
			CopyMemory(buf, mem + adrs2, size);
			return;
		}
		else {
			UINT32 len;
			len = 0x10fff0 - adrs;
			CopyMemory(buf, mem + adrs2, len);
			buf += len;
			size -= len;
			adrs = 0x10fff0;
		}
	}
}

void viewmem_paint(NP2VIEW_T *view, RECT *rc, HDC hdc, UINT32 alloctype, UINT32 totalsize, BOOL segmented) {

	UINT32	i;
	UINT32	x;
	LONG	y;
	UINT32	mad;
	UINT32	off;
	UINT8	*p;
	UINT8	*buf;
	TCHAR	str[16];
	WCHAR	text[4];
	BOOL	dbskip = false;
	UINT8	charlen;
	int	ret;
	COLORREF	bkcol;

	// Add 1 to accommodate double-byte characters starting at the end of a line
	buf = (UINT8*)alloca(bytesperline + 1);

	off = (view->pos) << 4;
	mad = off;
	if(segmented)	{
		mad += (((UINT32)view->seg) << 4);
	}

	if (view->lock) {
		if ((view->buf1.type != ALLOCTYPE_SEG) ||
			(view->buf1.arg != view->seg)) {
			if (viewcmn_alloc(&view->buf1, totalsize)) {
				view->lock = FALSE;
				viewmenu_lock(view);
			}
			else {
				view->buf1.type = ALLOCTYPE_SEG;
				view->buf1.arg = view->seg;
				viewmem_read(&view->dmem, view->buf1.arg << 4, (UINT8 *)view->buf1.ptr, totalsize);
			}
			viewcmn_putcaption(view);
		}
	}

	for (y=0; y<rc->bottom && off<totalsize; y+=viewcfg.font_height, off+=bytesperline) {
		x = 0;
		SetTextColor(hdc, viewcfg.color_text);
		SetBkColor(hdc, viewcfg.color_back);
		if(segmented)	{
			wsprintf(str, _T("%04x:%04x"), view->seg, off);
		} else {
			wsprintf(str, _T("%08x"), off);
		}

		TextOut(hdc, x, y, str, 9);
		x += 10 * np2viewfontwidth;

		if (view->lock) {
			p = (UINT8 *)view->buf1.ptr;
			p += off;
		}
		else {
			p = buf;
			viewmem_read(&view->dmem, mad, buf, bytesperline + 1);
		}
		for (i=0; i<bytesperline;i++) {
			str[0] = viewcmn_hex[*p >> 4];
			str[1] = viewcmn_hex[*p & 15];
			str[2] = 0;
			p++;

			if(mad + i == view->cursor)	{
				bkcol = viewcfg.color_cursor;
			} else {
				bkcol = viewcfg.color_back;
			}
			SetBkColor(hdc, bkcol);
			TextOut(hdc, x, y, str, 2);
			x += 2*CHAR_SPACING*np2viewfontwidth;
		}
		x += 2*np2viewfontwidth;

		for(i=0, p = buf; i<bytesperline; i++, p++) {
			if(mad + i == view->cursor)	{
				bkcol = viewcfg.color_cursor;
			} else {
				bkcol = viewcfg.color_back;
			}
			SetBkColor(hdc, bkcol);

			if(bkcol != viewcfg.color_back) {
				HBRUSH hbrush = CreateSolidBrush(bkcol);
				RECT rect;
				rect.left = x;
				rect.top = y;
				rect.right = rect.left + np2viewfontwidth;
				rect.bottom = rect.top + viewcfg.font_height;
				FillRect(hdc, &rect, hbrush);
				DeleteObject(hbrush);
			}
			if(!dbskip)	{
				dbskip = IsDBCSLeadByteEx(932, *p);
				charlen = dbskip + 1;
				ret = MultiByteToWideChar(932, MB_PRECOMPOSED, (LPCSTR)p, charlen, text, bytesperline);
				if(text[0] == 0 || !ret)	{
					lstrcpyW(text, L".");
					SetTextColor(hdc, viewcfg.color_dim);
				} else {
					SetTextColor(hdc, viewcfg.color_text);
				}
				TextOutW(hdc, x, y, text, 1);
			} else {
				dbskip = false;
			}
			x += np2viewfontwidth;
		}
		mad += bytesperline;
	}
}

LONG viewmem_mouse_to_cursor(NP2VIEW_T *view, HWND hwnd, POINTS mpos, LONG left, LONG cell, LONG right)	{

	LONG ret = -1;
	if(mpos.x > left && mpos.x < right)	{
		ret = (view->pos + (mpos.y / viewcfg.font_height)) * bytesperline;
		ret += (mpos.x - left) / cell;
	}
	return ret;
}

LRESULT CALLBACK viewmem_proc(NP2VIEW_T *view, HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {

	LONG newcursor = view->cursor - (view->seg << 4);

	switch (msg) {
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			switch(wp)
			{
			case VK_UP:
				newcursor -= bytesperline;
				break;
			case VK_DOWN:
				newcursor += bytesperline;
				break;
			case VK_LEFT:
				newcursor--;
				break;
			case VK_RIGHT:
				newcursor++;
				break;
			case VK_PRIOR:
				newcursor -= view->step * bytesperline;
				break;
			case VK_NEXT:
				newcursor += view->step * bytesperline;
				break;
			case VK_HOME:
				newcursor = (view->cursor / bytesperline) * bytesperline;
				break;
			case VK_END:
				newcursor = (((view->cursor / bytesperline) + 1) * bytesperline) - 1;
				break;
			}
			// Clamp
			if(newcursor < 0) {
				newcursor = 0;
			} else if(newcursor >= view->maxline * bytesperline) {
				newcursor = (view->maxline * bytesperline) - 1;
			}

			viewer_scroll_fit_line(view, hwnd, (newcursor / bytesperline));
			break;

		case WM_LBUTTONDOWN:	{
			POINTS mpos = MAKEPOINTS(lp);

			LONG bytes_left = 10*np2viewfontwidth;
			LONG bytes_cell = 2*CHAR_SPACING*np2viewfontwidth;
			LONG bytes_right = bytes_left + bytes_cell*bytesperline;

			LONG char_left = bytes_right + 2*np2viewfontwidth;
			LONG char_cell = np2viewfontwidth;
			LONG char_right = char_left + char_cell*bytesperline;

			newcursor = viewmem_mouse_to_cursor(view, hwnd, mpos, bytes_left, bytes_cell, bytes_right);
			if(newcursor < 0)	{
				newcursor = viewmem_mouse_to_cursor(view, hwnd, mpos, char_left, char_cell, char_right);
			}
		}
		break;
	}
	if(newcursor != view->cursor && newcursor != -1)	{
		view->cursor = newcursor + (view->seg << 4);
		InvalidateRect(hwnd, NULL, TRUE);
	}
	return(0L);
}
