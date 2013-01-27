// D_FIND.CPP - Find dialog
// ------
// Author: Nmlgc

#include	"compiler.h"
#include	"resource.h"
#include	"milstr.h"
#include	"dialogs.h"


static const UINT16 findtypes[] = {
				IDC_FIND_TYPE_HEX,
				IDC_FIND_TYPE_SJIS, IDC_FIND_TYPE_UTF8};

static const DWORD codepages[] = {
				0,
				932, CP_UTF8};

           FINDDATA finddata = {
			   {0}, L"", 0, IDC_FIND_TYPE_HEX
		   };

void finddata_str_to_bytes(FINDDATA *fd, DWORD codepage)	{

	if(codepage)	{
		// excluding the terminating null character
		fd->bytes_len = WideCharToMultiByte(codepage, WC_COMPOSITECHECK, fd->str, -1, (LPSTR)fd->bytes, MAX_FIND_STR * 3, NULL, NULL) - 1;
	} else {
		fd->bytes_len = milstr_solveHEXbuffer(fd->bytes, MAX_FIND_STR, fd->str);
	}
}

LRESULT CALLBACK FindDialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	WORD i;
	DWORD cp;

	switch (msg)	{
		case WM_INITDIALOG:
			SetFocus(GetDlgItem(hWnd, IDC_FIND_STR));
			SendDlgItemMessage(hWnd, IDC_FIND_STR, EM_SETLIMITTEXT, MAX_FIND_STR - 1, NULL);
			SetDlgItemCheck(hWnd, finddata.type, BST_CHECKED);
			SetDlgItemText(hWnd, IDC_FIND_STR, finddata.str);
			SendDlgItemMessage(hWnd, IDC_FIND_STR, EM_SETSEL, 0, -1);
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))	{
				case IDOK:
					EndDialog(hWnd, (INT_PTR)&finddata);
					return TRUE;

				case IDCANCEL:
					EndDialog(hWnd, 0);
					return TRUE;
			}

			switch (HIWORD(wParam))	{
				case EN_UPDATE:
					for(i = 0; i < NELEMENTS(findtypes); i++)	{
						if(GetDlgItemCheck(hWnd, findtypes[i]))	{
							finddata.type = findtypes[i];
							cp = codepages[i];
						}
					}
					GetDlgItemTextW(hWnd, LOWORD(wParam), finddata.str, MAX_FIND_STR);
					finddata_str_to_bytes(&finddata, cp);
					break;
			}
			break;
	}
	return FALSE;
}