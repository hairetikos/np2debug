struct tagEditData;
typedef struct tagEditData			EDITDATA;

void viewstat_found(NP2VIEW_T *view, EDITDATA *fd);
void viewstat_notfound(NP2VIEW_T *view, EDITDATA *fd);
void viewstat_update(NP2VIEW_T *view);

#ifdef __cplusplus
extern "C" {
#endif

void viewstat_memory_edit(NP2VIEW_T *view, EDITDATA *ed);

void viewstat_breakpoint(NP2VIEW_T *view, np2break_t type, UINT32 addr);
void viewstat_all_breakpoint(np2break_t type, UINT32 addr);

#ifdef __cplusplus
}
#endif

// -----------------------------------------------------------------------

void viewstat_open(NP2VIEW_T *view, HINSTANCE hInstance);
void viewstat_close(NP2VIEW_T *view);
