// md_fcb.c - mountdir/handling of anything that uses DOS' three-letter-acronym structures
// ------
// Author: Nmlgc

#include	"compiler.h"
#include	"oemtext.h"
#include	<shlwapi.h>
#include	"cpucore.h"
#include	"pccore.h"
#include	"md_dir.h"
#include	"md_fcb.h"
#include	"md_util.h"
#include	"mountdir.h"

/// DOS state
/// =========
// Disk Transfer Address
UINT32 dta_seg = 0;
UINT16 dta_off = 0;
DOS_DTA dta;
HANDLE hFind = NULL;
/// =========

/// Basic find functions
/// ====================
BOOL md_find_first_base(CHAR *mem_str_raw, UINT16 attrib)	{

	TCHAR mem_str_ucs2[MAX_PATH];
	OEMCHAR *mem_str;
	
	WIN32_FIND_DATA w32fd;

	FindClose(hFind);

	mem_str = sjis2ucs2(mem_str_raw, mem_str_ucs2, MAX_PATH);
	md_util_fn_split(dta.s_fn, dta.s_ext, PathFindFileNameA(mem_str_raw));
	mem_str = md_drive_parse(mem_str);

	hFind = FindFirstFile(mem_str, &w32fd);
	md_set_error(hFind != INVALID_HANDLE_VALUE);
	if(hFind != INVALID_HANDLE_VALUE)	{
		md_dta_write(&dta, &w32fd);
	}
	return TRUE;
}

BOOL md_find_first(UINT32 fn_seg4, UINT16 fn_off, UINT16 attrib)	{

	CHAR mem_str_raw[MAX_PATH];
	MEML_READS(fn_seg4 + fn_off, mem_str_raw, MAX_PATH);
	return md_find_first_base(mem_str_raw, attrib);
}

BOOL md_find_next()	{

	BOOL ret;
	WIN32_FIND_DATA w32fd;

	ret = FindNextFile(hFind, &w32fd);
	md_set_error(ret);
	if(ret)	{
		md_dta_write(&dta, &w32fd);
	}
	return TRUE;
}
/// ====================

/// Disk Transfer Area
/// ==================
BOOL md_dta_set(UINT32 addr_seg, UINT16 addr_off)	{
	
	dta_seg = addr_seg;
	dta_off = addr_off;
	return FALSE;
}

void md_dta_write(DOS_DTA *dta, WIN32_FIND_DATA *w32fd)	{

	SYSTEMTIME systime;

	oemtext_oemtosjis(dta->f_name, DOS_FN_LEN, md_util_dos_fn(w32fd), DOS_FN_LEN);

	FileTimeToSystemTime(&w32fd->ftLastWriteTime, &systime);

	dta->drive = cur_drive + 1;
	dta->f_attrib = w32fd->dwFileAttributes;
	dta->f_size = w32fd->nFileSizeLow;
	dta->f_time = time2dos(&systime);
	dta->f_date = date2dos(&systime);
	MEML_WRITES(dta_seg + dta_off, dta, sizeof(DOS_DTA));
}
/// ==================

/// File Control Block
/// ==================
BOOL md_fcb_rename(UINT32 fcb_seg4, UINT16 fcb_off)	{

	DOS_FCBREN fcbren;
	BOOL ret;
	CHAR fn_raw[2][DOS_FN_LEN];
	TCHAR fn_ucs2[2][DOS_FN_LEN];
	OEMCHAR *fn_str[2];

	MEML_READS(fcb_seg4 + fcb_off, &fcbren, sizeof(DOS_FCBREN));

	md_util_fn_combine(fn_raw[0], fcbren.old_name, fcbren.old_ext);
	md_util_fn_combine(fn_raw[1], fcbren.new_name, fcbren.new_ext);

	fn_str[0] = sjis2ucs2(fn_raw[0], fn_ucs2[0], DOS_FN_LEN);
	fn_str[1] = sjis2ucs2(fn_raw[1], fn_ucs2[1], DOS_FN_LEN);

	ret = MoveFile(fn_str[0], fn_str[1]);
	CPU_AL = ret ? 0x00 : 0xff;

	return TRUE;
}

BOOL md_fcb_delete(UINT32 fcb_seg4, UINT16 fcb_off)	{

	DOS_FCB fcb;
	BOOL ret;
	CHAR fn_raw[DOS_FN_LEN];
	TCHAR fn_ucs2[DOS_FN_LEN];
	OEMCHAR *fn_str;

	MEML_READS(fcb_seg4 + fcb_off, &fcb, sizeof(DOS_FCB));

	md_util_fn_combine(fn_raw, fcb.f_name, fcb.f_ext);
	fn_str = sjis2ucs2(fn_raw, fn_ucs2, DOS_FN_LEN);

	ret = DeleteFile(fn_str);
	CPU_AL = ret ? 0x00 : 0xff;

	return TRUE;
}

BOOL md_fcb_find_first(UINT32 fcb_seg4, UINT16 fcb_off)	{

	DOS_FCBEXT fcbext;
	UINT8 attrib = 0;

	CHAR fn_raw[DOS_FN_LEN];
	TCHAR fn_ucs2[DOS_FN_LEN];
	OEMCHAR *fn_str;

	WIN32_FIND_DATA w32fd;

	FindClose(hFind);

	MEML_READS(fcb_seg4 + fcb_off, &fcbext, sizeof(DOS_FCBEXT));
	if(fcbext.extended != 0xff)	{
		MEML_READS(fcb_seg4 + fcb_off, &fcbext.fcb, sizeof(DOS_FCB));
	} else {
		attrib = fcbext.attrib;
	}

	md_util_fn_combine(fn_raw, fcbext.fcb.f_name, fcbext.fcb.f_ext);
	fn_str = sjis2ucs2(fn_raw, fn_ucs2, DOS_FN_LEN);

	hFind = FindFirstFile(fn_str, &w32fd);
	if(hFind != INVALID_HANDLE_VALUE)	{
		CPU_AL = 0x00;
		md_fcb_write_find(&w32fd);
	} else {
		CPU_AL = 0xff;
	}
	return TRUE;
}

BOOL md_fcb_find_next(UINT32 fcb_seg4, UINT16 fcb_off)	{

	DOS_FCB fcb;
	BOOL ret = 0;
	WIN32_FIND_DATA w32fd;

	MEML_READS(fcb_seg4 + fcb_off, &fcb, sizeof(DOS_FCB));

	// Do stuff to verify that it's actually the same object
	// ...

	if(hFind)	{
		ret = FindNextFile(hFind, &w32fd);
	}
	if(ret)	{
		CPU_AL = 0x00;
		md_fcb_write_find(&w32fd);
	} else {
		CPU_AL = 0xff;
	}
	return TRUE;
}

void md_fcb_write_find(WIN32_FIND_DATA *w32fd)	{

	SYSTEMTIME systime;
	DOS_FCBEXT fcbext;
	CHAR fn[DOS_FN_LEN];

	oemtext_oemtosjis(fn, DOS_FN_LEN, md_util_dos_fn(w32fd), DOS_FN_LEN);
	md_util_fn_split(fcbext.fcb.f_name, fcbext.fcb.f_ext, fn);

	FileTimeToSystemTime(&w32fd->ftLastWriteTime, &systime);

	fcbext.extended = 0xff;
	fcbext.fcb.drive = cur_drive;
	fcbext.attrib = 0x10;
	fcbext.fcb.attrib = w32fd->dwFileAttributes;
	fcbext.fcb.f_size = w32fd->nFileSizeLow;
	fcbext.fcb.f_time = time2dos(&systime);
	fcbext.fcb.f_date = date2dos(&systime);
	MEML_WRITES(dta_seg + dta_off, &fcbext, sizeof(DOS_FCBEXT));
}
/// ==================

void md_reset_fcb() {

	FindClose(hFind);
	hFind = NULL;
}
