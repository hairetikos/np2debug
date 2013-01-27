struct tagFindData;
typedef struct tagFindData			FINDDATA;

void viewstat_found(NP2VIEW_T *view, FINDDATA *fd);
void viewstat_notfound(NP2VIEW_T *view, FINDDATA *fd);
void viewstat_update(NP2VIEW_T *view);

// -----------------------------------------------------------------------

void viewstat_open(NP2VIEW_T *view, HINSTANCE hInstance);
void viewstat_close(NP2VIEW_T *view);
