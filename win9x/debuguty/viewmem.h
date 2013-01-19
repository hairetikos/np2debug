
void viewmem_read(VIEWMEM_T *cfg, UINT32 adrs, UINT8 *buf, UINT32 size);
void viewmem_write(VIEWMEM_T *cfg, UINT32 adrs, UINT8 *buf, UINT32 size);

void viewmem_paint(NP2VIEW_T *view, RECT *rc, HDC hdc,
	            UINT32 alloctype, UINT32 totalsize, UINT32 bytesperline, BOOL segmented);
