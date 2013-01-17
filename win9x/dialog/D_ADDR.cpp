// D_ADDR.CPP - Address entering dialog
// ------
// Author: Nmlgc

#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"oemtext.h"
#include	"milstr.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"pccore.h"

#define SOURCE_SEG_OFF 0
#define SOURCE_REAL 1

static bool blockchange = false;
static UINT16 seg, off;

int validhex(OEMCHAR c)
{
	return
		(_T('0') <= c && c <= _T('9')) ||
		(_T('A') <= c && c <= _T('F')) ||
		(_T('a') <= c && c <= _T('f'));
}

static void calc_addr(HWND hWnd, UINT8 updatesource)
{
	OEMCHAR work[32];
	UINT32 real;

	blockchange = true;
	/// ... on second thought, we'd better not play around with the segment register ourselves...
	// if(updatesource == SOURCE_SEG_OFF)
	// {
		// seg:off -> real
		GetDlgItemText(hWnd, IDC_ADDR_SEG, work, 5);
		seg = (UINT16)milstr_solveHEX(work);
		GetDlgItemText(hWnd, IDC_ADDR_OFF, work, 5);
		off = (UINT16)milstr_solveHEX(work);
		real = (seg << 4) + off;
		wsprintf(work, _T("%04x"), real);
		SetDlgItemText(hWnd, IDC_ADDR_REAL, work);
	/* }
	else
	{
		// real -> seg:off
		GetDlgItemText(hWnd, IDC_ADDR_REAL, work, 6);
		real = milstr_solveHEX(work);
		seg = (real & 0xf0000) >> 4;
		off = (real & 0x0ffff);
		wsprintf(work, L"%04x", seg);
		SetDlgItemText(hWnd, IDC_ADDR_SEG, work);
		wsprintf(work, L"%04x", off);
		SetDlgItemText(hWnd, IDC_ADDR_OFF, work);
	} */
	blockchange = false;
}

LRESULT CALLBACK AddrDialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	OEMCHAR work[6];
	int i, len;
	bool changed = false;
	DWORD selstart, selend;

	if(blockchange)	return FALSE;

	switch (msg) {
		case WM_INITDIALOG:
			SetFocus(GetDlgItem(hWnd, IDC_ADDR_SEG));
			SendDlgItemMessage(hWnd, IDC_ADDR_SEG, EM_SETLIMITTEXT, 4, NULL);
			SendDlgItemMessage(hWnd, IDC_ADDR_OFF, EM_SETLIMITTEXT, 4, NULL);
			SendDlgItemMessage(hWnd, IDC_ADDR_REAL, EM_SETLIMITTEXT, 5, NULL);
			return FALSE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
					EndDialog(hWnd, (seg << 16) + off);
					return TRUE;

				case IDCANCEL:
					EndDialog(hWnd, -1);
					return TRUE;
			}

			switch (HIWORD(wParam)) {
				case EN_UPDATE:
					blockchange = true;
					GetDlgItemText(hWnd, LOWORD(wParam), work, 6);
					SendDlgItemMessage(hWnd, LOWORD(wParam), EM_GETSEL, (WPARAM)&selstart, (LPARAM)&selend);
					len = lstrlen(work);
					for(i = 0; i < len; i++)
					{
						OEMCHAR last = work[i];
						if(!validhex(work[i]))
						{
							memmove(&work[i], &work[i+1], sizeof(OEMCHAR) * (len - i));
							changed = true;
						}
					}
					if(changed)
					{
						SetDlgItemText(hWnd, LOWORD(wParam), work);
						selstart--;
						selend--;
						SendDlgItemMessage(hWnd, LOWORD(wParam), EM_SETSEL, (WPARAM)(selstart), (LPARAM) selend);
					}
					// Advance if seg was filled out
					if(!changed && len == 4 && LOWORD(wParam) == IDC_ADDR_SEG)
					{
						// http://blogs.msdn.com/b/oldnewthing/archive/2004/08/02/205624.aspx
						SendMessage(hWnd, WM_NEXTDLGCTL, (WPARAM)GetDlgItem(hWnd, IDC_ADDR_OFF), TRUE);
					}
					blockchange = false;
					break;

				case EN_CHANGE:
					if(!blockchange)
					{
						calc_addr(hWnd, LOWORD(wParam) == IDC_ADDR_REAL ? SOURCE_REAL : SOURCE_SEG_OFF);
						return TRUE;
					}
					else
						return FALSE;
			 }
	}
	return FALSE;
}