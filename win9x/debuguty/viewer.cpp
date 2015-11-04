#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"break.h"
#include 	"dialogs.h"
#include	"viewer.h"
#include	"viewcmn.h"
#include	"viewmenu.h"
#include	"viewstat.h"
#include	"cpucore.h"
#include	"dialog.h"
#include	"ini.h"
#include	"debugsub.h"


static	const TCHAR		np2viewparentclass[] = _T("NP2-ViewWindow");
static	const TCHAR		np2viewclientclass[] = _T("NP2-ViewClient");
		HFONT    	np2viewfont;
		int         	np2viewfontwidth = 0;
		NP2VIEW_T		np2view[NP2VIEW_MAX];

		VIEWCFG	viewcfg = {
			 _T("MS Gothic"), 16,

			 // Colors
			 0xffffff, 0x909090, 0x400000, 0xff8000, 0x0000ff, 0x000000,
			 // Sizes
			 {0, 0}, {0, 0}, {0, 0}, {0, 0}
		};

static	const PFTBL viewerini[] = {
	PFSTR("fontname", PFTYPE_STR,	viewcfg.font_name),
	PFVAL("col_text", PFTYPE_HEX32, &viewcfg.color_text),
	PFVAL("col_dimm", PFTYPE_HEX32, &viewcfg.color_dim),
	PFVAL("col_back", PFTYPE_HEX32, &viewcfg.color_back),
	PFVAL("col_curs", PFTYPE_HEX32, &viewcfg.color_cursor),
	PFVAL("col_high", PFTYPE_HEX32, &viewcfg.color_hilite),
	PFVAL("col_actv", PFTYPE_HEX32, &viewcfg.color_active),

	PFVAL("sizeasmx", PFTYPE_UINT16, &viewcfg.size_asm.x),
	PFVAL("sizeasmy", PFTYPE_UINT16, &viewcfg.size_asm.y),
	PFVAL("sizememx", PFTYPE_UINT16, &viewcfg.size_mem.x),
	PFVAL("sizememy", PFTYPE_UINT16, &viewcfg.size_mem.y),
	PFVAL("sizestkx", PFTYPE_UINT16, &viewcfg.size_stk.x),
	PFVAL("sizestky", PFTYPE_UINT16, &viewcfg.size_stk.y),
	PFVAL("sizeregx", PFTYPE_UINT16, &viewcfg.size_reg.x),
	PFVAL("sizeregy", PFTYPE_UINT16, &viewcfg.size_reg.y),
	PFVAL("sizesndx", PFTYPE_UINT16, &viewcfg.size_snd.x),
	PFVAL("sizesndy", PFTYPE_UINT16, &viewcfg.size_snd.y),
};

static const OEMCHAR viewerapp[] = OEMTEXT("Debug Utility");

static void viewer_segmode(NP2VIEW_T *view, UINT8 type) {

	if ((view) && (view->type != type)) {
		viewcmn_setmode(view, view, type);
		viewcmn_setbank(view);
		viewcmn_setvscroll(view);
		InvalidateRect(view->clientwnd, NULL, TRUE);
	}
}

static UINT32 viewer_pageup(NP2VIEW_T *view) 	{

	if (view->pos > view->step) {
		return view->pos - view->step;
	} else {
		return 0;
	}
}

static UINT32 viewer_pagedown(NP2VIEW_T *view)	{

	UINT32 newpos = view->pos + view->step;
	if (newpos > (view->maxline - view->step)) {
		newpos = view->maxline - view->step;
	}
	return newpos;
}

void viewer_scroll_update(NP2VIEW_T *view, UINT32 newpos)	{

	if (view->pos != newpos) {
		view->pos = newpos;
		viewcmn_setvscroll(view);
		InvalidateRect(view->clientwnd, NULL, TRUE);
	}
}

void viewer_scroll_fit_line(NP2VIEW_T *view, LONG line)	{

	LONG scrolldiff;
	RECT rc;
	LONG viewlines;

	scrolldiff = line - view->pos;
	GetClientRect(view->clientwnd, &rc);
	viewlines = (rc.bottom / viewcfg.font_height) - 1;

	if(scrolldiff < 0)	{
		viewer_scroll_update(view, view->pos + scrolldiff);
	}
	scrolldiff = line - view->pos - viewlines;
	if(scrolldiff > 0)	{
		viewer_scroll_update(view, view->pos + scrolldiff);
	}
}

void viewer_edit_dlg(NP2VIEW_T *view, HWND hWnd)	{

	EDITDATA *ed;

	winuienter();
	ed = (EDITDATA*)DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_EDIT), view->clientwnd, (DLGPROC)EditDialogProc);
	if(ed)	{
		if(ed->bytes_len > 0)	{
			viewstat_memory_edit(view, ed);
			MEML_WRITES(view->cursor, ed->bytes, ed->bytes_len);
			InvalidateRect(view->clientwnd, NULL, TRUE);
			// Invalidate secondary buffer, too.
			// Important for Unassemble mode, where the instruction offsets most likely change
			view->buf2.arg = NULL;
		}
	}
	winuileave();
}



static void vieweractive_renewal(void) {

	int  		i;
	UINT8		breakflag = NP2BREAK_RESUME;
	NP2VIEW_T	*view;

	view = np2view;
	breakflag &= ~NP2BREAK_DEBUG;
	for (i=0; i<NP2VIEW_MAX; i++, view++) {
		if ((view->alive) && (view->active)) {
			breakflag |= NP2BREAK_DEBUG;
			break;
		}
	}
	np2active_renewal(breakflag);
}


static void viewer_close(NP2VIEW_T *view) {

	viewstat_close(view);
	DestroyWindow(view->hwnd);
	view->alive = FALSE;
	viewcmn_free(&view->buf1);
	viewcmn_free(&view->buf2);
	np2active_set(1);
}


LRESULT CALLBACK ViewParentProc(HWND hParentWnd, UINT msg, WPARAM wp, LPARAM lp) {

	NP2VIEW_T *view = viewcmn_find(hParentWnd);
	UINT32 ret;
	UINT8 flag;

	switch (msg) {
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			break;

		case WM_COMMAND:
			switch(LOWORD(wp)) {
				case IDM_VIEWWINNEW:
					viewer_open(g_hInstance);
					break;

				case IDM_VIEWWINCLOSE:
					return(ViewParentProc(hParentWnd, WM_CLOSE, 0, 0));
					break;

				case IDM_VIEWWINALLCLOSE:
					viewer_allclose();
					break;

				case IDM_VIEWMODEREG:
				case IDM_VIEWMODESEG:
				case IDM_VIEWMODESTK:
				case IDM_VIEWMODE1MB:
				case IDM_VIEWMODEASM:
				case IDM_VIEWMODESND:
					viewer_segmode(view, LOWORD(wp) - IDM_VIEWMODEREG);
					break;

				case IDM_DEBUG_STEPINTO:
					np2active_step();
					break;

				case IDM_DEBUG_STEPOVER:
					np2active_step_over();
					break;

				case IDM_DEBUG_RUN:
				case IDM_DEBUG_STOP:
					flag = LOWORD(wp) == IDM_DEBUG_RUN;
					np2active_set(flag);
					break;

				case IDM_SETSEG:
					winuienter();
					ret = (UINT32)DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_ADDRESS), hParentWnd, (DLGPROC)AddrDialogProc);
					if(ret != -1)	{
						PostMessage(hParentWnd, WM_COMMAND, IDM_GOTO, ret);
					}
					winuileave();
					break;

				case IDM_MEMORYDUMP:
					debugsub_memorydump();
					break;

				case IDM_CPUSAVE:
					debugsub_status();
					break;

				/// Status
				case IDM_STATUS_FOUND:
					viewstat_found(view, (EDITDATA*)lp);
					break;

				case IDM_STATUS_NOTFOUND:
					viewstat_notfound(view, (EDITDATA*)lp);
					break;

				default:
					return(viewcmn_dispat(view->clientwnd, msg, wp, lp));
			}
			break;

		case WM_SIZE:
			if (view) {
				SendMessage(view->statwnd, WM_SIZE, 0, 0);
				SendMessage(view->clientwnd, WM_SIZE, 0, 0);
			}
			break;

		case WM_ENTERMENULOOP:
			viewcmn_setmenuseg(hParentWnd);
			// fall through
		case WM_ENTERSIZEMOVE:
			winuienter();
			break;

		case WM_EXITMENULOOP:
		case WM_EXITSIZEMOVE:
			winuileave();
			break;

		case WM_ACTIVATE:
			if (view) {
				if (LOWORD(wp) != WA_INACTIVE) {
					view->active = 1;
					InvalidateRect(view->clientwnd, NULL, TRUE);
					SetFocus(view->clientwnd);
				}
				else {
					view->active = 0;
				}
				// vieweractive_renewal();
			}
			break;

		case WM_CLOSE:
			viewcmn_dispat(view->clientwnd, msg, wp, lp);
			DestroyWindow(hParentWnd);
			if (view) {
				viewer_close(view);
				vieweractive_renewal();
			}
			break;
		
		default:
			return(DefWindowProc(hParentWnd, msg, wp, lp));
	}
	return(0);
}

LRESULT CALLBACK ViewClientProc(HWND hClientWnd, UINT msg, WPARAM wp, LPARAM lp) {

	NP2VIEW_T *view = viewcmn_find(hClientWnd);

	switch (msg) {
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			// Accelerators suck
			switch(wp)
			{
			case VK_PRIOR:
				viewer_scroll_update(view, viewer_pageup(view));
				return(viewcmn_dispat(hClientWnd, msg, wp, lp));

			case VK_NEXT:
				viewer_scroll_update(view, viewer_pagedown(view));
				return(viewcmn_dispat(hClientWnd, msg, wp, lp));

			case VK_F2:
				SendMessage(view->hwnd, WM_COMMAND, IDM_BREAK_TOGGLE, NULL);
				break;

			case VK_F3:
				SendMessage(view->hwnd, WM_COMMAND, IDM_FINDAGAIN, NULL);
				break;

			case VK_F7:
				SendMessage(view->hwnd, WM_COMMAND, IDM_DEBUG_STEPINTO, NULL);
				break;

			case VK_F8:
				SendMessage(view->hwnd, WM_COMMAND, IDM_DEBUG_STEPOVER, NULL);
				break;

			case VK_F9:
				SendMessage(view->hwnd, WM_COMMAND, IDM_DEBUG_RUN, NULL);
				break;

			case VK_F12:
				SendMessage(view->hwnd, WM_COMMAND, IDM_DEBUG_STOP, NULL);
				break;

			case 'E':
				SendMessage(view->hwnd, WM_COMMAND, IDM_EDIT_MEMORY, NULL);
				break;

			case 'F':
				if(GetKeyState(VK_CONTROL) < 0)
					SendMessage(view->hwnd, WM_COMMAND, IDM_FIND, NULL);
				break;

			case 'G':
				if(GetKeyState(VK_CONTROL) < 0)
					SendMessage(view->hwnd, WM_COMMAND, IDM_SETSEG, NULL);
				break;

			default:
				return(viewcmn_dispat(hClientWnd, msg, wp, lp));
			}
			break;

		case WM_LBUTTONDOWN:
			return(viewcmn_dispat(hClientWnd, msg, wp, lp));

		case WM_SIZE:
			if(view)
			{
				RECT rc_parent;
				RECT rc_client;
				RECT rc_status_abs;
				POINTS *save = NULL;
				int w, h;

				GetClientRect(view->hwnd, &rc_parent);
				GetClientRect(view->clientwnd, &rc_client);
				GetWindowRect(view->statwnd, &rc_status_abs);

				w = rc_parent.right;
				h = rc_parent.bottom - (rc_status_abs.bottom - rc_status_abs.top);

				view->step = (UINT16)(rc_client.bottom / viewcfg.font_height);
				viewcmn_setvscroll(view);
				SetWindowPos(view->clientwnd, NULL, 0, 0, w, h, 0);

				switch(view->type)	{
					case VIEWMODE_ASM:
						save = &viewcfg.size_asm;
						break;
					case VIEWMODE_1MB:
					case VIEWMODE_SEG:
						save = &viewcfg.size_mem;
						break;
					case VIEWMODE_STK:
						save = &viewcfg.size_stk;
						break;
					case VIEWMODE_REG:
						save = &viewcfg.size_reg;
						break;
					case VIEWMODE_SND:
						save = &viewcfg.size_snd;
						break;
				}
				if(save)	{
					RECT rc_parent_abs;
					GetWindowRect(view->hwnd, &rc_parent_abs);
					save->x = rc_parent_abs.right - rc_parent_abs.left;
					save->y = rc_parent_abs.bottom - rc_parent_abs.top;
				}
			}
			break;

		case WM_PAINT:
			viewstat_update(view);
			return(viewcmn_dispat(hClientWnd, msg, wp, lp));

		case WM_VSCROLL:
			if (view) {
				UINT32 newpos = view->pos;
				winuienter();
				switch(LOWORD(wp)) {
					case SB_LINEUP:
						if (newpos) {
							newpos--;
						}
						break;
					case SB_LINEDOWN:
						if (newpos < (view->maxline - view->step)) {
							newpos++;
						}
						break;
					case SB_PAGEUP:
						newpos = viewer_pageup(view);
						break;
					case SB_PAGEDOWN:
						newpos = viewer_pagedown(view);
						break;
					case SB_THUMBTRACK:
						newpos = HIWORD(wp) * (view->mul);
						break;
				}
				viewer_scroll_update(view, newpos);
				winuileave();
			}
			break;

		case WM_MOUSEWHEEL:
			if (view)
			{
				short delta = HIWORD(wp);
				if(
					(delta < 0 && view->pos < (view->maxline - view->step)) ||
					(delta > 0 && view->pos)
				)	{
					view->pos -= (delta / WHEEL_DELTA) * 4;
					viewcmn_setvscroll(view);
					InvalidateRect(view->clientwnd, NULL, TRUE);
				}
			}
			break;

		default:
			return(DefWindowProc(hClientWnd, msg, wp, lp));
	}
	return(0);
}


// -----------------------------------------------------------------------

BOOL viewer_init(HINSTANCE hInstance) {

	WNDCLASS	np2vc;
	HDC htempdc;
	TEXTMETRIC tm;

	viewer_readini();

	ZeroMemory(np2view, sizeof(np2view));

	np2vc.style = CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW;
	np2vc.lpfnWndProc = ViewParentProc;
	np2vc.cbClsExtra = 0;
	np2vc.cbWndExtra = 0;
	np2vc.hInstance = hInstance;
	np2vc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	np2vc.hCursor = NULL;
	np2vc.hbrBackground = (HBRUSH)0;
	np2vc.lpszMenuName = MAKEINTRESOURCE(IDR_VIEW);
	np2vc.lpszClassName = np2viewparentclass;
	if (!RegisterClass(&np2vc)) {
		return(FAILURE);
	}

	np2vc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW;
	np2vc.lpfnWndProc = ViewClientProc;
	np2vc.cbClsExtra = 0;
	np2vc.cbWndExtra = 0;
	np2vc.hInstance = hInstance;
	np2vc.hIcon = NULL;
	np2vc.hCursor = LoadCursor(NULL, IDC_ARROW);
	np2vc.hbrBackground = (HBRUSH)0;
	np2vc.lpszMenuName = NULL;
	np2vc.lpszClassName = np2viewclientclass;
	if (!RegisterClass(&np2vc)) {
		return(FAILURE);
	}

	np2viewfont = CreateFont(viewcfg.font_height, 0, 0, 0, 0, 0, 0, 0,
					DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY, FIXED_PITCH, viewcfg.font_name);

	htempdc = CreateCompatibleDC(NULL);
	SelectObject(htempdc, np2viewfont);
	GetTextMetrics(htempdc, &tm);
	np2viewfontwidth = tm.tmMaxCharWidth / 2;
	DeleteDC(htempdc);
	return(SUCCESS);
}


void viewer_term(void) {

	DeleteObject(np2viewfont);
	viewer_writeini();
}


void viewer_open(HINSTANCE hInstance) {

	int			i;
	NP2VIEW_T	*view;

	view = np2view;
	for (i=0; i<NP2VIEW_MAX; i++, view++) {
		if (!view->alive) {
			TCHAR buf[256];
			
			viewcmn_caption(view, buf);
			ZeroMemory(view, sizeof(NP2VIEW_T));
			view->alive = TRUE;
			view->hwnd = CreateWindowEx(0,
							np2viewparentclass, buf,
							WS_OVERLAPPEDWINDOW,
							CW_USEDEFAULT, CW_USEDEFAULT,
							480, 640,
							NULL, NULL, hInstance, NULL);
			viewstat_open(view, hInstance);

			view->clientwnd = CreateWindowEx(0,
							np2viewclientclass, buf,
							WS_CHILD | WS_VSCROLL | WS_VISIBLE,
							CW_USEDEFAULT, CW_USEDEFAULT,
							CW_USEDEFAULT, CW_USEDEFAULT,
							view->hwnd, NULL, hInstance, NULL);
			viewcmn_setmode(view, NULL, VIEWMODE_REG);
			ShowWindow(view->hwnd, SW_SHOWNORMAL);
			UpdateWindow(view->hwnd);
			break;
		}
	}
}


void viewer_allclose(void) {

	int			i;
	NP2VIEW_T	*view;

	view = np2view;
	for (i=0; i<NP2VIEW_MAX; i++, view++) {
		if (view->alive) {
			viewer_close(view);
		}
	}
	vieweractive_renewal();
}


void viewer_allreload(BOOL force) {

static UINT32	last = 0;
	UINT32		now;

	now = GetTickCount();
	if ((force) || ((now - last) >= 200)) {
		int			i;
		NP2VIEW_T	*view;

		last = now;
		view = np2view;
		for (i=0; i<NP2VIEW_MAX; i++, view++) {
			if ((view->alive) && (!view->lock)) {
				viewcmn_reload(view);
				viewcmn_setbank(view);
				InvalidateRect(view->clientwnd, NULL, TRUE);
			}
		}
	}
}

void viewer_readini(void) {

	OEMCHAR	path[MAX_PATH];

	initgetfile(path, NELEMENTS(path));
	ini_read(path, viewerapp, viewerini, NELEMENTS(viewerini));
}

void viewer_writeini(void) {

	OEMCHAR	path[MAX_PATH];

	initgetfile(path, NELEMENTS(path));
	ini_write(path, viewerapp, viewerini, NELEMENTS(viewerini));
}
