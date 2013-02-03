#include	"compiler.h"
#include	"resource.h"
#include	<CommCtrl.h>
#include	"dialogs.h"
#include	"viewer.h"
#include	"viewstat.h"
#include	"break.h"

static void print_addr(OEMCHAR *str, UINT32 seg, UINT32 addr, BOOL segmented)	{

	if(segmented)	{
		addr -= seg << 4;
		wsprintf(str, _T("%04x:%04x"), seg, addr);
	} else {
		wsprintf(str, _T("%08x"), addr);
	}
}

static void viewstat_found_base(NP2VIEW_T *view, FINDDATA *fd, WCHAR *msg)	{

	WCHAR str_msg[MAX_FIND_STR * 2];
	WCHAR str_addr[32];
	const OEMCHAR* str_type;

	switch(fd->type)	{
		case IDC_FIND_TYPE_HEX:
			str_type = L"hex";
			break;
		case IDC_FIND_TYPE_SJIS:
			str_type = L"Shift-JIS";
			break;			
		case IDC_FIND_TYPE_UTF8:
			str_type = L"UTF-8";
			break;
	}

	print_addr(str_addr, view->seg, view->cursor, view->type != VIEWMODE_1MB);
	wsprintfW(str_msg, msg, fd->str, str_type, str_addr);
	SendMessage(view->statwnd, SB_SETTEXT, 1, (LPARAM)str_msg);
}

void viewstat_found(NP2VIEW_T *view, FINDDATA *fd)	{
	viewstat_found_base(view, fd, L"Found '%s' (%s) at %s");
}
void viewstat_notfound(NP2VIEW_T *view, FINDDATA *fd)	{
	viewstat_found_base(view, fd, L"'%s' (%s) not found");
}

void viewstat_breakpoint(NP2VIEW_T *view, UINT8 type, UINT32 addr)	{

	OEMCHAR str_msg[MAX_FIND_STR * 2];
	OEMCHAR str_addr[32];
	const OEMCHAR* str_type;

	switch(type)	{
		case NP2BP_READ:
			str_type = _T("memory read");
			break;
		case NP2BP_WRITE:
			str_type = _T("memory write");
			break;			
		case NP2BP_EXECUTE:
			str_type = _T("code");
			break;
	}

	print_addr(str_addr, 0, addr, FALSE);
	wsprintf(str_msg, _T("Hit %s breakpoint at %s"), str_type, str_addr);
	SendMessage(view->statwnd, SB_SETTEXT, 1, (LPARAM)str_msg);
}

void viewstat_all_breakpoint(UINT8 type, UINT32 addr) {

	int			i;
	NP2VIEW_T	*view = np2view;
	for (i=0; i<NP2VIEW_MAX; i++, view++) {
		if ((view->alive)) {
			viewstat_breakpoint(view, type, addr);
		}
	}
}

void viewstat_update(NP2VIEW_T *view)	{

	OEMCHAR str[32];
	print_addr(str, view->seg, view->cursor, view->type != VIEWMODE_1MB);
	SendMessage(view->statwnd, SB_SETTEXT, 0, (LPARAM)str);
}


// -----------------------------------------------------------------------

void viewstat_open(NP2VIEW_T *view, HINSTANCE hInstance)	{

	int widths[] = {64, -1};

	view->statwnd = CreateWindowEx(0, 
				STATUSCLASSNAME, _T(""),
				WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP | CCS_BOTTOM,
				0, 0,
				0, 0,
				view->hwnd, NULL, hInstance, NULL);

	SendMessage(view->statwnd, SB_SETPARTS, (WPARAM)(sizeof(widths)/sizeof(int)), (LPARAM)widths);
}

void viewstat_close(NP2VIEW_T *view)	{

}
