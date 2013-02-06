#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"dialogs.h"
#include	"viewer.h"
#include	"viewcmn.h"
#include	"viewmenu.h"
#include	"viewmem.h"
#include	"viewstat.h"
#include	"viewpaint.h"
#include	"cpucore.h"
#include	"dialog.h"
#include	"break.h"

const float	CHAR_SPACING = 1.5f;

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

void viewmem_lock_alloc(NP2VIEW_T *view, UINT32 alloctype)	{

	if (view->lock) {
		if ((view->buf1.type != alloctype) ||
			(view->buf1.arg != view->seg)) {
			if (viewcmn_alloc(&view->buf1, view->memsize)) {
				view->lock = FALSE;
				viewmenu_lock(view);
			}
			else {
				view->buf1.type = alloctype;
				view->buf1.arg = view->seg;
				viewmem_read(&view->dmem, view->buf1.arg << 4, (UINT8 *)view->buf1.ptr, view->memsize);
			}
			viewcmn_putcaption(view);
		}
	}
}

// ----


static COLORREF viewmem_set_bkcol(NP2VIEW_T *view, UINT32 addr)	{

	if(addr == view->cursor)	{
		return viewcfg.color_cursor;
	} else if(np2break_is_set_real(addr)) {
		return viewcfg.color_hilite;
	}
	return viewcfg.color_back;
}

void viewmem_paint(NP2VIEW_T *view, RECT *rc, HDC hdc, UINT32 alloctype, BOOL segmented) {

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
	buf = (UINT8*)alloca(view->bytesperline + 1);

	off = (view->pos) * view->bytesperline;
	mad = off;
	if(segmented)	{
		mad += (((UINT32)view->seg) << 4);
	}

	viewmem_lock_alloc(view, alloctype);

	for (y=0; y<rc->bottom && off<view->memsize; y+=viewcfg.font_height, off+=view->bytesperline) {
		x = 0;
		SetTextColor(hdc, viewcfg.color_text);
		SetBkColor(hdc, viewcfg.color_back);

		viewpaint_print_addr(hdc, x, y, view->seg, off, segmented);

		x = view->cp_bytes.left;

		if (view->lock) {
			p = (UINT8 *)view->buf1.ptr;
			p += off;
		}
		else {
			p = buf;
			viewmem_read(&view->dmem, mad, buf, view->bytesperline + 1);
		}
		for (i=0; i<view->bytesperline;i++) {
			str[0] = viewcmn_hex[*p >> 4];
			str[1] = viewcmn_hex[*p & 15];
			str[2] = 0;
			p++;

			SetBkColor(hdc, viewmem_set_bkcol(view, mad + i));
			TextOut(hdc, x, y, str, 2);
			x += view->cp_bytes.cell;
		}
		x = view->cp_chars.left;

		for(i=0, p = buf; i<view->bytesperline; i++, p++) {
			bkcol = viewmem_set_bkcol(view, mad + i);
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
				ret = MultiByteToWideChar(932, MB_PRECOMPOSED, (LPCSTR)p, charlen, text, 4);
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
			x += view->cp_chars.cell;
		}
		mad += view->bytesperline;
	}
}

LONG viewmem_mouse_to_cursor(NP2VIEW_T *view, HWND hwnd, POINTS mpos, VIEWCELLPOS* cp)	{

	LONG ret = -1;
	if(mpos.x > cp->left && cp->right ? (mpos.x < cp->right) : 1)	{
		ret = (view->pos + (mpos.y / viewcfg.font_height)) * view->bytesperline;
		if(cp->cell)	{
			ret += (mpos.x - cp->left) / cp->cell;
		}
	}
	return ret;
}

static LONG viewmem_find(NP2VIEW_T *view, EDITDATA *_fd)	{

	void *needle;
	UINT32 seg4 = view->seg << 4;
	LONG newcursor = view->cursor - seg4;

	static EDITDATA* fd = NULL;
	if(_fd)	{
		fd = _fd;
	}

	if(!fd)	{
		return -1;
	}
	needle = memmem(mem + view->cursor + 1, (view->maxline << 4) - newcursor - 1, fd->bytes, fd->bytes_len);
	if(!needle)	{
		// Wrap
		needle = memmem(mem, view->cursor, fd->bytes, fd->bytes_len);
	}
	
	if(needle)	{
		PostMessage(view->hwnd, WM_COMMAND, IDM_STATUS_FOUND, (LPARAM)fd);
		return (LONG)((UINT8*)needle - mem) - seg4;
	} else {
		PostMessage(view->hwnd, WM_COMMAND, IDM_STATUS_NOTFOUND, (LPARAM)fd);
		return -1;
	}
	
}

LRESULT CALLBACK viewmem_proc(NP2VIEW_T *view, HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {

	UINT32 seg4 = view->seg << 4;
	LONG oldcursor = view->cursor - seg4;
	LONG newcursor = oldcursor;
	EDITDATA *ed;
	POINTS mpos;

	switch (msg) {
		case WM_COMMAND:
			switch (LOWORD(wp))	{
				case IDM_EDIT_MEMORY:
					viewer_edit_dlg(view, hwnd);
					break;

				case IDM_FIND:
					winuienter();
					ed = (EDITDATA*)DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_EDIT), view->clientwnd, (DLGPROC)FindDialogProc);
					newcursor = viewmem_find(view, ed);
					if(newcursor != -1)	{
						EnableMenuItem(GetMenu(view->hwnd), IDM_FINDAGAIN, MF_BYCOMMAND | MF_ENABLED);
					}
					winuileave();
					break;

				case IDM_FINDAGAIN:
					newcursor = viewmem_find(view, NULL);
					break;

				case IDM_GOTO:
					{
						UINT16 seg = (lp >> 16);
						newcursor = (lp & 0xffff);
						if(view->type == VIEWMODE_1MB)	{
							newcursor += seg << 4;
						} else {
							view->seg = seg;
						}
					}
					break;

				case IDM_BREAK_TOGGLE:
					if(np2break_toggle_real(seg4 + newcursor, NP2BP_READ | NP2BP_WRITE))	{
						InvalidateRect(view->clientwnd, NULL, TRUE);
					}
					break;
			}
			break;

		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			// Vertical
			switch(wp)
			{
			case VK_UP:
				newcursor -= view->bytesperline;
				break;
			case VK_DOWN:
				newcursor += view->bytesperline;
				break;
			case VK_PRIOR:
				newcursor -= view->step * view->bytesperline;
				break;
			case VK_NEXT:
				newcursor += view->step * view->bytesperline;
				break;
			}
			// Horizontal
			if(view->cp_bytes.cell)	{
				switch(wp)
				{
				case VK_LEFT:
					newcursor--;
					break;
				case VK_RIGHT:
					newcursor++;
					break;
				case VK_HOME:
					newcursor = (newcursor / view->bytesperline) * view->bytesperline;
					break;
				case VK_END:
					newcursor = (((newcursor / view->bytesperline) + 1) * view->bytesperline) - 1;
					break;
				}
			}
			// Clamp
			if(newcursor < 0) {
				newcursor = 0;
			} else if(newcursor >= (LONG)(view->maxline * view->bytesperline)) {
				newcursor = (view->maxline * view->bytesperline);
				if(view->cp_bytes.cell)	{
					newcursor--;
				} else {
					newcursor -= view->bytesperline;
				}
			}
			break;

		case WM_LBUTTONDOWN:	
			mpos = MAKEPOINTS(lp);
			newcursor = viewmem_mouse_to_cursor(view, hwnd, mpos, &view->cp_bytes);
			if(newcursor < 0)	{
				newcursor = viewmem_mouse_to_cursor(view, hwnd, mpos, &view->cp_chars);
			}
			break;
	}
	if(newcursor != oldcursor && newcursor != -1)	{
		viewer_scroll_fit_line(view, (newcursor / view->bytesperline));
		view->cursor = newcursor + seg4;
		viewstat_update(view);
		InvalidateRect(hwnd, NULL, TRUE);
	}
	return(0L);
}


// ---------------------------------------------------------------------------

void viewmem_init(NP2VIEW_T *dst, NP2VIEW_T *src) {

	dst->cp_bytes.left = 12*np2viewfontwidth;
	dst->cp_bytes.cell = 2*CHAR_SPACING*np2viewfontwidth;
	dst->cp_bytes.right = dst->cp_bytes.left + dst->cp_bytes.cell*dst->bytesperline;

	dst->cp_chars.left = dst->cp_bytes.right + 3*np2viewfontwidth;
	dst->cp_chars.cell = np2viewfontwidth;
	dst->cp_chars.right = dst->cp_chars.left + dst->cp_chars.cell*dst->bytesperline;
	
	if (!src) {
		dst->pos = 0;
	}
	dst->maxline = dst->memsize / dst->bytesperline;
}
