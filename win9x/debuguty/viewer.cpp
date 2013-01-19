#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"viewer.h"
#include	"viewcmn.h"
#include	"cpucore.h"
#include	"break.h"
#include	"dialog.h"


static	const TCHAR		np2viewclass[] = _T("NP2-ViewWindow");
		const TCHAR		np2viewfont[] = _T("MS Gothic");
		const int   	np2viewfontheight = 12;
		NP2VIEW_T		np2view[NP2VIEW_MAX];

COLORREF color_back = 0x400000;
COLORREF color_text = 0xffffff;
COLORREF color_dim = 0x909090;
COLORREF color_cursor = 0x606060;
COLORREF color_hilite = 0x0000ff;
COLORREF color_active = 0x000000;


static void viewer_segmode(HWND hwnd, UINT8 type) {

	NP2VIEW_T	*view;

	view = viewcmn_find(hwnd);
	if ((view) && (view->type != type)) {
		viewcmn_setmode(view, view, type);
		viewcmn_setbank(view);
		viewcmn_setvscroll(hwnd, view);
		InvalidateRect(hwnd, NULL, TRUE);
	}
}

static UINT32 viewer_pageup(NP2VIEW_T *view, HWND hwnd) 	{
	if (view->pos > view->step) {
		return view->pos - view->step;
	}
	else {
		return 0;
	}
}

static UINT32 viewer_pagedown(NP2VIEW_T *view, HWND hwnd)	{
	UINT32 newpos = view->pos + view->step;
	if (newpos > (view->maxline - view->step)) {
		newpos = view->maxline - view->step;
	}
	return newpos;
}

void viewer_scroll_update(NP2VIEW_T *view, HWND hwnd, UINT32 newpos)	{
	if (view->pos != newpos) {
		view->pos = newpos;
		viewcmn_setvscroll(hwnd, view);
		InvalidateRect(hwnd, NULL, TRUE);
	}
}

void viewer_debug_menu_toggle(HMENU hmenu, BOOL running)
{
	DWORD flag;
	
	flag = running ? MF_GRAYED : MF_ENABLED;
	EnableMenuItem(hmenu, IDM_DEBUG_RUN, MF_BYCOMMAND | flag);
	EnableMenuItem(hmenu, IDM_DEBUG_STEPINTO, MF_BYCOMMAND | flag);
	EnableMenuItem(hmenu, IDM_DEBUG_STEPOVER, MF_BYCOMMAND | flag);

	flag = running ? MF_ENABLED : MF_GRAYED;
	EnableMenuItem(hmenu, IDM_DEBUG_STOP, MF_BYCOMMAND | flag);
}

static void vieweractive_renewal(void) {

	int			i;
	NP2VIEW_T	*view;

	view = np2view;
	np2break &= ~NP2BREAK_DEBUG;
	for (i=0; i<NP2VIEW_MAX; i++, view++) {
		if ((view->alive) && (view->active)) {
			np2break |= NP2BREAK_DEBUG;
			break;
		}
	}
	np2active_renewal();
}


static void viewer_close(NP2VIEW_T *view) {

	view->alive = FALSE;
	viewcmn_free(&view->buf1);
	viewcmn_free(&view->buf2);
	np2active_set(1);
}


LRESULT CALLBACK ViewProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {

	NP2VIEW_T *view = viewcmn_find(hwnd);
	UINT32 ret;

	switch (msg) {
		case WM_CREATE:
			break;

		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			// Accelerators suck
			switch(wp)
			{
			case VK_PRIOR:
				viewer_scroll_update(view, hwnd, viewer_pageup(view, hwnd));
				return(viewcmn_dispat(hwnd, msg, wp, lp));

			case VK_NEXT:
				viewer_scroll_update(view, hwnd, viewer_pagedown(view, hwnd));
				return(viewcmn_dispat(hwnd, msg, wp, lp));

			case VK_F2:
				SendMessage(hwnd, WM_COMMAND, IDM_BREAK_TOGGLE, NULL);
				break;

			case VK_F7:
				SendMessage(hwnd, WM_COMMAND, IDM_DEBUG_STEPINTO, NULL);
				break;

			case VK_F8:
				SendMessage(hwnd, WM_COMMAND, IDM_DEBUG_STEPOVER, NULL);
				break;

			case VK_F9:
				SendMessage(hwnd, WM_COMMAND, IDM_DEBUG_RUN, NULL);
				break;

			case VK_F12:
				SendMessage(hwnd, WM_COMMAND, IDM_DEBUG_STOP, NULL);
				break;

			case 'G':
				if(GetKeyState(VK_CONTROL) < 0)
					SendMessage(hwnd, WM_COMMAND, IDM_SETSEG, NULL);
				break;

			default:
				return(viewcmn_dispat(hwnd, msg, wp, lp));
			}
			break;

		case WM_COMMAND:
			switch(LOWORD(wp)) {
				case IDM_VIEWWINNEW:
					viewer_open(g_hInstance);
					break;

				case IDM_VIEWWINCLOSE:
					return(ViewProc(hwnd, WM_CLOSE, 0, 0));
					break;

				case IDM_VIEWWINALLCLOSE:
					viewer_allclose();
					break;

				case IDM_VIEWMODEREG:
					viewer_segmode(hwnd, VIEWMODE_REG);
					break;

				case IDM_VIEWMODESEG:
					viewer_segmode(hwnd, VIEWMODE_SEG);
					break;

				case IDM_VIEWMODE1MB:
					viewer_segmode(hwnd, VIEWMODE_1MB);
					break;

				case IDM_VIEWMODEASM:
					viewer_segmode(hwnd, VIEWMODE_ASM);
					break;

				case IDM_VIEWMODESND:
					viewer_segmode(hwnd, VIEWMODE_SND);
					break;

				case IDM_DEBUG_STEPINTO:
					np2active_step();
					break;

				case IDM_DEBUG_RUN:
					np2active_set(1);
					viewer_debug_menu_toggle(GetMenu(hwnd), 1);
					break;

				case IDM_DEBUG_STOP:
					np2active_set(0);
					viewer_debug_menu_toggle(GetMenu(hwnd), 0);
					break;

				case IDM_SETSEG:
					ret = (UINT32)DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_ADDRESS), hwnd, (DLGPROC)AddrDialogProc);

					if(ret != -1 && view)	{
						view->seg = ret >> 16;
						view->off = ret & CPU_ADRSMASK;
					}
					break;

				default:
					return(viewcmn_dispat(hwnd, msg, wp, lp));
			}
			break;

		case WM_LBUTTONDOWN:
			return(viewcmn_dispat(hwnd, msg, wp, lp));

		case WM_PAINT:
			return(viewcmn_dispat(hwnd, msg, wp, lp));

		case WM_SIZE:
			if (view) {
				RECT rc;
				GetClientRect(hwnd, &rc);
				view->step = (UINT16)(rc.bottom / 16);
				viewcmn_setvscroll(hwnd, view);
			}
			break;

		case WM_VSCROLL:
			if (view) {
				UINT32 newpos = view->pos;
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
						newpos = viewer_pageup(view, hwnd);
						break;
					case SB_PAGEDOWN:
						newpos = viewer_pagedown(view, hwnd);
						break;
					case SB_THUMBTRACK:
						newpos = HIWORD(wp) * (view->mul);
						break;
				}
				viewer_scroll_update(view, hwnd, newpos);
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
					viewcmn_setvscroll(hwnd, view);
					InvalidateRect(hwnd, NULL, TRUE);
				}
			}
			break;

		case WM_ENTERMENULOOP:
			viewcmn_setmenuseg(hwnd);
			break;

		case WM_ACTIVATE:
			if (view) {
				if (LOWORD(wp) != WA_INACTIVE) {
					view->active = 1;
					InvalidateRect(hwnd, NULL, TRUE);
				}
				else {
					view->active = 0;
				}
				// vieweractive_renewal();
			}
			break;

		case WM_CLOSE:
			DestroyWindow(hwnd);
			if (view) {
				viewer_close(view);
				vieweractive_renewal();
			}
			break;

		default:
			return(DefWindowProc(hwnd, msg, wp, lp));
	}
	return(0L);
}


// -----------------------------------------------------------------------

BOOL viewer_init(HINSTANCE hInstance) {

	WNDCLASS	np2vc;

	ZeroMemory(np2view, sizeof(np2view));

	np2vc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW;
	np2vc.lpfnWndProc = ViewProc;
	np2vc.cbClsExtra = 0;
	np2vc.cbWndExtra = 0;
	np2vc.hInstance = hInstance;
	np2vc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	np2vc.hCursor = LoadCursor(NULL, IDC_ARROW);
	np2vc.hbrBackground = (HBRUSH)0;
	np2vc.lpszMenuName = MAKEINTRESOURCE(IDR_VIEW);
	np2vc.lpszClassName = np2viewclass;
	if (!RegisterClass(&np2vc)) {
		return(FAILURE);
	}
	return(SUCCESS);
}


void viewer_term(void) {

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
							np2viewclass, buf,
							WS_OVERLAPPEDWINDOW | WS_VSCROLL,
							CW_USEDEFAULT, CW_USEDEFAULT,
							480, 640,
							NULL, NULL, hInstance, NULL);
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
			DestroyWindow(view->hwnd);
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
				if (view->type == VIEWMODE_ASM) {
					view->seg = CPU_CS;
					view->off = CPU_IP;
					view->pos = 0;
					viewcmn_setvscroll(view->hwnd, view);
				}
				viewcmn_setbank(view);
				InvalidateRect(view->hwnd, NULL, TRUE);
			}
		}
	}
}

