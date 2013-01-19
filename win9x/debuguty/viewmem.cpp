#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"viewer.h"
#include	"viewcmn.h"
#include	"viewmenu.h"
#include	"viewmem.h"
#include	"cpucore.h"

const float CHAR_SPACING = 1.5f;


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

void viewmem_paint(NP2VIEW_T *view, RECT *rc, HDC hdc,
	            UINT32 alloctype, UINT32 totalsize, UINT32 bytesperline, BOOL segmented) {

	UINT32	i;
	UINT32	x;
	LONG	y;
	UINT32	mad;
	UINT32	off;
	UINT8	*p;
	UINT8	*buf;
	TCHAR	str[16];
	WCHAR	*text;
	HFONT	hfont;
	TEXTMETRIC tm;
	BOOL	dbskip = false;
	UINT8	charlen;
	int	ret;

	// Add 1 to compensate for double-byte characters starting at the end of a line
	buf = (UINT8*)alloca(bytesperline + 1);
	text = (WCHAR*)alloca((bytesperline + 1) * sizeof(WCHAR));

	hfont = CreateFont(np2viewfontheight, 0, 0, 0, 0, 0, 0, 0, 
					DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY, FIXED_PITCH, np2viewfont);
	SetTextColor(hdc, color_text);
	SetBkColor(hdc, color_back);
	hfont = (HFONT)SelectObject(hdc, hfont);
	GetTextMetrics(hdc, &tm);

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

	for (y=0; y<rc->bottom && off<totalsize; y+=np2viewfontheight, off+=bytesperline) {
		x = 0;
		SetTextColor(hdc, color_text);
		if(segmented)	{
			wsprintf(str, _T("%04x:%04x"), view->seg, off);
		} else {
			wsprintf(str, _T("%08x"), off);
		}
		TextOut(hdc, x, y, str, 9);
		x += 10 * tm.tmAveCharWidth;

		if (view->lock) {
			p = (UINT8 *)view->buf1.ptr;
			p += off;
		}
		else {
			p = buf;
			viewmem_read(&view->dmem, mad, buf, bytesperline + 1);
			mad += bytesperline;
		}
		for (i=0; i<bytesperline;i++) {
			str[0] = viewcmn_hex[*p >> 4];
			str[1] = viewcmn_hex[*p & 15];
			str[2] = 0;
			p++;
			TextOut(hdc, x, y, str, 2);
			x += 2*CHAR_SPACING*tm.tmAveCharWidth;
		}
		x += 2*tm.tmAveCharWidth;

		for(i=0, p = buf; i<bytesperline; i++, p++) {
			if(!dbskip)	{
				dbskip = IsDBCSLeadByteEx(932, *p);
				charlen = dbskip + 1;
				ret = MultiByteToWideChar(932, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, (LPCSTR)p, charlen, text, bytesperline);
				if(text[0] == 0 || !ret)	{
					lstrcpyW(text, L".");
					SetTextColor(hdc, color_dim);
				} else {
					SetTextColor(hdc, color_text);
				}
				TextOutW(hdc, x, y, text, charlen);
			} else {
				dbskip = false;
			}
			x += tm.tmAveCharWidth;
		}
	}
	DeleteObject(SelectObject(hdc, hfont));
}
