// md_util.c - mountdir/utility functions
// ------
// Author: Nmlgc

#include	"compiler.h"
#include	"oemtext.h"
#include	<shlwapi.h>
#include	"cpucore.h"
#include	"md_dir.h"

/// Time conversion
/// ===============
UINT16 time2dos(const SYSTEMTIME *systime)	{
	return
		((systime->wSecond / 2) & 0x1f) +
		((systime->wMinute & 0x3f) << 5) + 
		((systime->wHour & 0x1f) << 11);
}

UINT16 date2dos(const SYSTEMTIME *systime)	{
	return
		(systime->wDay & 0x1f) + 
		((systime->wMonth & 0xf) << 5) +
		(((systime->wYear - 1980) & 0x7f) << 9);
}

BOOL dostime2wintime(FILETIME *win_time, const UINT16 dos_date, const UINT16 dos_time) {

	SYSTEMTIME systime;

	if(!win_time)	return FALSE;

	systime.wDay = dos_date & 0x1f;
	systime.wMonth = (dos_date >> 5) & 0xf;
	systime.wYear = ((dos_date >> 9) & 0x7f) + 1980;
	
	systime.wSecond = (dos_time & 0x1f) * 2;
	systime.wMinute = (dos_time >> 5) & 0x3f;
	systime.wHour = (dos_time >> 11) & 0x1f;

	return SystemTimeToFileTime(&systime, win_time);
}
/// ===============

/// String conversion
/// =================
TCHAR* sjis2ucs2(CHAR *str_raw, TCHAR *str_ucs2, UINT16 len)	{

#if defined(UNICODE)
	MultiByteToWideChar(932, MB_PRECOMPOSED, str_raw, -1, str_ucs2, len);
	return str_ucs2;
#else
	return str_raw;
#endif
}

TCHAR* mem_read_sjis2ucs2(CHAR *str_raw, TCHAR *str_ucs2, UINT32 seg4, UINT16 off, UINT16 len)	{
	
	MEML_READS(seg4 + off, str_raw, len);
	return sjis2ucs2(str_raw, str_ucs2, len);
}
/// =================

/// File name conversion
/// ====================
OEMCHAR* md_util_dos_fn(WIN32_FIND_DATA *w32fd) {

	OEMCHAR *fn = w32fd->cFileName;
	size_t fn_len = lstrlen(w32fd->cFileName);
	OEMCHAR *f_name = (OEMCHAR *)alloca((fn_len + 1) * sizeof(OEMCHAR));
	OEMCHAR *f_ext;

	lstrcpy(f_name, fn);
	f_ext = PathFindExtension(fn);
	PathRemoveExtension(f_name);

	if(lstrlen(f_name) > 8 || (lstrlen(f_ext) - 1) > 3) {
		fn = w32fd->cAlternateFileName;
	}
	return fn;
}

void md_util_fn_split(CHAR *f_name, CHAR *f_ext, const CHAR *asciiz_fn) {

	CHAR* ext = "\0";
	size_t ext_len = 0;

	memset(f_name, ' ', 8);
	memset(f_ext, ' ', 3);
	if(strcmp(asciiz_fn, ".") && strcmp(asciiz_fn, ".."))	{
		ext = PathFindExtensionA(asciiz_fn);
		ext_len = strlen(ext);
	}
	memcpy(f_name, asciiz_fn, strlen(asciiz_fn) - ext_len);
	if(ext[0] != '\0')	{
		memcpy(f_ext, ext + 1, strlen(ext) - 1);
	}
}

void md_util_fn_combine(CHAR *asciiz_fn, const CHAR *f_name, const CHAR *f_ext) {

	UINT8 name_len, ext_len;

	name_len = memchr_len(f_name, ' ', 8);
	ext_len = memchr_len(f_ext, ' ', 3);
	
	memcpy(asciiz_fn, f_name, name_len);
	memcpy(asciiz_fn + name_len + 1, f_ext, ext_len);
	asciiz_fn[name_len] = '.';
	asciiz_fn[name_len + 1 + ext_len] = '\0';
}
/// ====================
