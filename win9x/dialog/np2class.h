
// 猫用、拡張クラス。


extern const TCHAR np2dlgclass[];

void np2class_initialize(HINSTANCE hinst);
void np2class_deinitialize(void);

void np2class_move(HWND hWnd, int posx, int posy, int cx, int cy);
int CALLBACK np2class_propetysheet(HWND hWndDlg, UINT uMsg, LPARAM lParam);


// ---- class

#define	NP2GWLP_HMENU	(0 * sizeof(LONG_PTR))
#define	NP2GWLP_SIZE	(1 * sizeof(LONG_PTR))

void np2class_wmcreate(HWND hWnd);
void np2class_wmdestroy(HWND hWnd);
void np2class_enablemenu(HWND hWnd, BOOL enable);
void np2class_windowtype(HWND hWnd, UINT8 type);
void np2class_frametype(HWND hWnd, UINT8 thick);
HMENU np2class_gethmenu(HWND hWnd);

WINLOCEX np2class_entersizemove(HWND hWnd, WINLOCEX in);
WINLOCEX np2class_exitsizemove(HWND hWnd, WINLOCEX in);
LRESULT CALLBACK np2class_wndproc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);