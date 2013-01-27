extern const float CHAR_SPACING;
extern      UINT16 bytesperline;


void viewmem_read(VIEWMEM_T *cfg, UINT32 adrs, UINT8 *buf, UINT32 size);
void viewmem_write(VIEWMEM_T *cfg, UINT32 adrs, UINT8 *buf, UINT32 size);

void viewmem_paint(NP2VIEW_T *view, RECT *rc, HDC hdc, UINT32 alloctype, UINT32 totalsize, BOOL segmented);

LRESULT CALLBACK viewmem_proc(NP2VIEW_T *view, HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

void viewmem_init(NP2VIEW_T *dst, NP2VIEW_T *src);