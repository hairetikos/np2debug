struct tagFindData;
typedef struct tagFindData			FINDDATA;

void viewstat_found(NP2VIEW_T *view, FINDDATA *fd);
void viewstat_notfound(NP2VIEW_T *view, FINDDATA *fd);
void viewstat_update(NP2VIEW_T *view);

#ifdef __cplusplus
extern "C" {
#endif

void viewstat_breakpoint(NP2VIEW_T *view, UINT8 type, UINT32 addr);
void viewstat_all_breakpoint(UINT8 type, UINT32 addr);

#ifdef __cplusplus
}
#endif

// -----------------------------------------------------------------------

void viewstat_open(NP2VIEW_T *view, HINSTANCE hInstance);
void viewstat_close(NP2VIEW_T *view);
