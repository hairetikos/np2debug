
void viewstk_reload(NP2VIEW_T *view);

LRESULT CALLBACK viewstk_proc(NP2VIEW_T *view,
								HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

void viewstk_init(NP2VIEW_T *dst, NP2VIEW_T *src);
