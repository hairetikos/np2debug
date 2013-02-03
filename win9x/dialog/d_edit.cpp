// d_edit.cpp - Memory edit and find dialog
// ------
// Author: Nmlgc

#include	"compiler.h"
#include	"resource.h"
#include	"milstr.h"
#include	"dialogs.h"


static const UINT16 edit_types[] = {
				IDC_EDIT_TYPE_HEX,
				IDC_EDIT_TYPE_SJIS, IDC_EDIT_TYPE_UTF8};

static const DWORD edit_codepages[] = {
				0,
				932, CP_UTF8};

           EDITDATA editdata = {
			   {0}, L"", 0, IDC_EDIT_TYPE_HEX
		   };

		   EDITDATA finddata = {
			   {0}, L"", 0, IDC_EDIT_TYPE_HEX
		   };


static void editdata_str_to_bytes(EDITDATA *ed, HWND hWnd, int item)	{

	GetDlgItemTextW(hWnd, item, ed->str, MAX_EDIT_STR);
	if(ed->codepage)	{
		// excluding the terminating null character
		ed->bytes_len = WideCharToMultiByte(ed->codepage, 0, ed->str, -1, (LPSTR)ed->bytes, MAX_EDIT_STR * 3, NULL, NULL) - 1;
	} else {
		ed->bytes_len = milstr_solveHEXbuffer(ed->bytes, MAX_EDIT_STR, ed->str);
	}
}

static void editdata_get_type(EDITDATA *ed, HWND hWnd)	{

	WORD i;

	for(i = 0; i < NELEMENTS(edit_types); i++)	{
		if(GetDlgItemCheck(hWnd, edit_types[i]))	{
			ed->type = edit_types[i];
			ed->codepage = edit_codepages[i];
		}
	}
}

static LRESULT CALLBACK EditDlgBaseProc(EDITDATA *ed, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg)	{
		case WM_INITDIALOG:
			SendDlgItemMessage(hWnd, IDC_EDIT_STR, EM_SETLIMITTEXT, MAX_EDIT_STR - 1, NULL);
			SetDlgItemCheck(hWnd, ed->type, BST_CHECKED);
			SetDlgItemTextW(hWnd, IDC_EDIT_STR, ed->str);
			SendDlgItemMessage(hWnd, IDC_EDIT_STR, EM_SETSEL, 0, -1);
			// needs to be at the bottom because it calls WM_COMMAND, which would clear our previous string
			SendMessage(hWnd, WM_NEXTDLGCTL, (WPARAM)GetDlgItem(hWnd, IDC_EDIT_STR), TRUE);
			break;

		case WM_COMMAND:
			editdata_get_type(ed, hWnd);
			editdata_str_to_bytes(ed, hWnd, IDC_EDIT_STR);
			switch (LOWORD(wParam))	{
				case IDOK:
					EndDialog(hWnd, (INT_PTR)ed);
					return TRUE;

				case IDCANCEL:
					EndDialog(hWnd, 0);
					return TRUE;
			}
			break;
	}
	return FALSE;
}

LRESULT CALLBACK EditDialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch(msg)	{
		case WM_INITDIALOG:
			SetDlgItemText(hWnd, IDC_EDIT_GROUP, _T("Edit"));
			return EditDlgBaseProc(&editdata, hWnd, msg, wParam, lParam);
		default:
			return EditDlgBaseProc(&editdata, hWnd, msg, wParam, lParam);
	}
	return FALSE;
}

LRESULT CALLBACK FindDialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch(msg)	{
		case WM_INITDIALOG:
			SetWindowText(hWnd, _T("Find..."));
			SetDlgItemText(hWnd, IDC_EDIT_GROUP, _T("Find"));
			return EditDlgBaseProc(&finddata, hWnd, msg, wParam, lParam);
		default:
			return EditDlgBaseProc(&finddata, hWnd, msg, wParam, lParam);
	}
	return FALSE;
}
