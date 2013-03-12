// md_util.h - mountdir/utility functions
// ------
// Author: Nmlgc

#ifdef __cplusplus
extern "C" {
#endif

/// Time conversion
/// ===============
UINT16 time2dos(const SYSTEMTIME *systime);
UINT16 date2dos(const SYSTEMTIME *systime);
BOOL dostime2wintime(FILETIME *win_time, const UINT16 dos_date, const UINT16 dos_time);
/// ===============

/// String conversion
/// =================
TCHAR* sjis2ucs2(CHAR *str_raw, TCHAR *str_ucs2, UINT16 len);
TCHAR *mem_read_sjis2ucs2(CHAR *str_raw, TCHAR *str_ucs2, UINT32 seg4, UINT16 off, UINT16 len);
/// =================

/// File name conversion
/// ====================
// Selects either 
OEMCHAR* md_util_dos_fn(WIN32_FIND_DATA *w32fd);
void md_util_fn_split(CHAR *f_name, CHAR *f_ext, const CHAR *asciiz_fn);
void md_util_fn_combine(CHAR *asciiz_fn, const CHAR *f_name, const CHAR *f_ext);
/// ====================

#ifdef __cplusplus
}
#endif
