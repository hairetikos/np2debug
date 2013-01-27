#include	"compiler.h"
#include	"resource.h"
#include	"viewer.h"
#include	"viewcmn.h"
#include	"viewmenu.h"

#define	MFCHECK(a) ((a)?MF_CHECKED:MF_UNCHECKED)


void viewmenu_mode(NP2VIEW_T *view) {

	HMENU	hmenu;

	hmenu = GetMenu(view->hwnd);
	CheckMenuItem(hmenu, IDM_VIEWMODEREG, MFCHECK(view->type == VIEWMODE_REG));
	CheckMenuItem(hmenu, IDM_VIEWMODESEG, MFCHECK(view->type == VIEWMODE_SEG));
	CheckMenuItem(hmenu, IDM_VIEWMODE1MB, MFCHECK(view->type == VIEWMODE_1MB));
	CheckMenuItem(hmenu, IDM_VIEWMODEASM, MFCHECK(view->type == VIEWMODE_ASM));
	CheckMenuItem(hmenu, IDM_VIEWMODESND, MFCHECK(view->type == VIEWMODE_SND));
}


void viewmenu_lock(NP2VIEW_T *view) {

	HMENU	hmenu;

	hmenu = GetMenu(view->hwnd);
	CheckMenuItem(hmenu, IDM_VIEWMODELOCK, MFCHECK(view->lock));
}


void viewmenu_debug_toggle(NP2VIEW_T *view, BOOL running)
{
	HMENU	hmenu;
	DWORD	flag;

	hmenu = GetMenu(view->hwnd);
	
	flag = running ? MF_GRAYED : MF_ENABLED;
	EnableMenuItem(hmenu, IDM_DEBUG_RUN, MF_BYCOMMAND | flag);
	EnableMenuItem(hmenu, IDM_DEBUG_STEPINTO, MF_BYCOMMAND | flag);
	EnableMenuItem(hmenu, IDM_DEBUG_STEPOVER, MF_BYCOMMAND | flag);

	flag = running ? MF_ENABLED : MF_GRAYED;
	EnableMenuItem(hmenu, IDM_DEBUG_STOP, MF_BYCOMMAND | flag);
}