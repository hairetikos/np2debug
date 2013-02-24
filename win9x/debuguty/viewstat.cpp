#include	"compiler.h"
#include	"resource.h"
#include	<CommCtrl.h>
#include	"dialogs.h"
#include	"viewer.h"
#include	"viewstat.h"
#include	"cpucore.h"
#include	"break.h"

/// -------
/// Helpers
static void print_addr(WCHAR *str, UINT32 seg, UINT32 addr, BOOL segmented)	{

	if(segmented)	{
		addr -= seg << 4;
		wsprintf(str, L"%04x:%04x", seg, addr);
	} else {
		wsprintf(str, L"%08x", addr);
	}
}

static WCHAR* edit_type_str(EDITDATA *ed)	{

	switch(ed->type)	{
		case IDC_EDIT_TYPE_HEX:
			return L"hex";
		case IDC_EDIT_TYPE_SJIS:
			return L"Shift-JIS";
		case IDC_EDIT_TYPE_UTF8:
			return L"UTF-8";
	}
	return L"unknown";
}
/// -------

/// ----
/// Find
static void viewstat_edit_base(NP2VIEW_T *view, EDITDATA *fd, WCHAR *msg)	{

	WCHAR str_msg[MAX_EDIT_STR * 2];
	WCHAR str_addr[32];

	print_addr(str_addr, view->seg, view->cursor, view->type != VIEWMODE_1MB);
	wsprintfW(str_msg, msg, fd->str, edit_type_str(fd), str_addr);
	SendMessage(view->statwnd, SB_SETTEXT, 2, (LPARAM)str_msg);
}

void viewstat_found(NP2VIEW_T *view, EDITDATA *fd)	{
	viewstat_edit_base(view, fd, L"Found '%s' (%s) at %s");
}
void viewstat_notfound(NP2VIEW_T *view, EDITDATA *fd)	{
	viewstat_edit_base(view, fd, L"'%s' (%s) not found");
}
/// ----

/// ----------
/// Breakpoint
void viewstat_breakpoint(NP2VIEW_T *view, UINT8 type, UINT32 addr)	{

	WCHAR str_msg[MAX_EDIT_STR * 2];
	WCHAR str_addr[32];
	const WCHAR* str_type;

	switch(type)	{
		case NP2BP_READ:
			str_type = L"memory read";
			break;
		case NP2BP_WRITE:
			str_type = L"memory write";
			break;			
		case NP2BP_EXECUTE:
			str_type = L"code";
			break;
	}

	print_addr(str_addr, 0, addr, FALSE);
	wsprintf(str_msg, L"Hit %s breakpoint at %s", str_type, str_addr);
	SendMessage(view->statwnd, SB_SETTEXT, 2, (LPARAM)str_msg);
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
/// ----------

/// --------------
/// Memory editing
void viewstat_memory_edit(NP2VIEW_T *view, EDITDATA *ed)	{
	
	if(ed->bytes_len)	{
		viewstat_edit_base(view, ed, L"Wrote '%s' (%s) at %s");
	} else {
		viewstat_edit_base(view, ed, L"'%s': Invalid %s string, wrote nothing");
	}
}
/// --------------

/// ---------------
/// Cursor position
void viewstat_update(NP2VIEW_T *view)	{

	WCHAR str[32];
	print_addr(str, view->seg, view->cursor, view->type != VIEWMODE_1MB);
	SendMessage(view->statwnd, SB_SETTEXT, 0, (LPARAM)str);
	SendMessage(view->statwnd, SB_SETTEXT, 1, (LPARAM)(CPU_STAT_PM ? _T("Protected Mode") : _T("Real Mode")));
}
/// ---------------


// -----------------------------------------------------------------------

void viewstat_open(NP2VIEW_T *view, HINSTANCE hInstance)	{

	int widths[] = {64, 160, -1};

	view->statwnd = CreateWindowEx(0, 
				STATUSCLASSNAME, _T(""),
				WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP | CCS_BOTTOM,
				0, 0,
				0, 0,
				view->hwnd, NULL, hInstance, NULL);

	SendMessage(view->statwnd, SB_SETPARTS, (WPARAM)NELEMENTS(widths), (LPARAM)widths);
}

void viewstat_close(NP2VIEW_T *view)	{

}
