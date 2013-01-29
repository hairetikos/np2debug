// D_ADDR.CPP - Address entering dialog
// ------
// Author: Nmlgc

#include	"compiler.h"
#include	"resource.h"
#include	"cpucore.h"

enum	{
	SOURCE_SEG_OFF,
	SOURCE_REAL
};

static bool blockchange = false;
static UINT16 seg, off;

static UINT32 solve_reg(OEMCHAR* in)	{

	size_t i;
	CHAR str[32];

#if defined(OSLANG_UCS2)
	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, in, -1, str, 32, NULL, NULL);
#else
	strcpy(str, work);
#endif

	for(i = 0; i < CPU_REG_NUM; i++)	{
		if(!stricmp(str, reg16_str[i]))	{
			return CPU_REGS_WORD(i);
		}
		if(!stricmp(str, reg32_str[i]))	{
			return CPU_REGS_DWORD(i);
		}
	}
	for(i = 0; i < CPU_SEGREG_NUM; i++)	{
		if(!stricmp(str, sreg_str[i]))	{
			return CPU_REGS_SREG(i);
		}
	}
	if(!stricmp(str, "ip"))	{
		return CPU_IP;
	} else if(!stricmp(str, "eip"))	{
		return CPU_EIP;
	}
	return milstr_solveHEX(in);
}

static void calc_addr(HWND hWnd, UINT8 updatesource)	{

	OEMCHAR work[32];
	UINT32 real;

	blockchange = true;
	/// ... on second thought, we'd better not play around with the segment register ourselves...
	// if(updatesource == SOURCE_SEG_OFF)
	// {
		// seg:off -> real
		GetDlgItemText(hWnd, IDC_ADDR_SEG, work, 5);
		seg = (UINT16)solve_reg(work);
		GetDlgItemText(hWnd, IDC_ADDR_OFF, work, 5);
		off = (UINT16)solve_reg(work);
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

// Not needed anymore...
static void assure_hex(HWND hWnd, int item) {

	OEMCHAR work[6];
	int i, len;
	bool changed = false;
	DWORD selstart, selend;

	GetDlgItemText(hWnd, item, work, 6);
	len = lstrlen(work);

	SendDlgItemMessage(hWnd, item, EM_GETSEL, (WPARAM)&selstart, (LPARAM)&selend);
	for(i = 0; i < len; i++)	{
		OEMCHAR last = work[i];
		if(!milstr_validhex(work[i]))	{
			memmove(&work[i], &work[i+1], sizeof(OEMCHAR) * (len - i));
			changed = true;
		}
	}
	if(changed)	{
		SetDlgItemText(hWnd, item, work);
		selstart--;
		selend--;
		SendDlgItemMessage(hWnd, item, EM_SETSEL, (WPARAM)(selstart), (LPARAM)selend);
	}
}

// Returns the address entered in a combined 32-bit value
// (high word = segment part, low word = offset part)
LRESULT CALLBACK AddrDialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	OEMCHAR work[6];
	int len;

	if(blockchange)	{
		return FALSE;
	}
	switch (msg) {
		case WM_INITDIALOG:
			SetFocus(GetDlgItem(hWnd, IDC_ADDR_SEG));
			SendDlgItemMessage(hWnd, IDC_ADDR_SEG, EM_SETLIMITTEXT, 4, NULL);
			SendDlgItemMessage(hWnd, IDC_ADDR_OFF, EM_SETLIMITTEXT, 4, NULL);
			SendDlgItemMessage(hWnd, IDC_ADDR_REAL, EM_SETLIMITTEXT, 5, NULL);
			return FALSE;

		case WM_COMMAND:
			switch (LOWORD(wParam))	{
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
					len = lstrlen(work);
					// Advance if seg was filled out
					if(len == 4 && LOWORD(wParam) == IDC_ADDR_SEG)	{
						// http://blogs.msdn.com/b/oldnewthing/archive/2004/08/02/205624.aspx
						SendMessage(hWnd, WM_NEXTDLGCTL, (WPARAM)GetDlgItem(hWnd, IDC_ADDR_OFF), TRUE);
					}
					blockchange = false;
					break;

				case EN_CHANGE:
					if(!blockchange)	{
						calc_addr(hWnd, LOWORD(wParam) == IDC_ADDR_REAL ? SOURCE_REAL : SOURCE_SEG_OFF);
						return TRUE;
					}
					else
						return FALSE;
			}
			break;
	}
	return FALSE;
}