/// Line functions
/// --------------
void viewpaint_line_fill(RECT *rc, HDC hdc, DWORD top, DWORD height, COLORREF col)	;
COLORREF viewpaint_line_set_colors(NP2VIEW_T *view, RECT *rc, HDC hdc, DWORD y, UINT16 off, UINT16 cpu_seg, UINT16 cpu_off);
/// --------------

/// Common stuff
/// ------------
void viewpaint_print_addr(HDC hdc, int x, int y, UINT16 seg, UINT16 off, BOOL segmented);
/// ------------
