// md_dir.c - mountdir/directory handling
// ------
// Author: Nmlgc

#include	"compiler.h"
#include	"oemtext.h"
#include	<shlwapi.h>
#include	"cpucore.h"
#include	"pccore.h"
#include	"md_dir.h"
#include	"md_util.h"
#include	"mountdir.h"

/// DOS state
/// =========
INT8 cur_drive = 0;
OEMCHAR cur_dir[MAX_PATH];
/// =========

/// Drive handling
/// ==============
BOOL md_drive_set(UINT8 drive) {

	cur_drive = drive;
	SetCurrentDirectory(np2cfg.mountdir[cur_drive]);
	return FALSE;
}

OEMCHAR *md_drive_parse(OEMCHAR *new_dir) {

	OEMCHAR *root_str;
	int ret = PathGetDriveNumber(new_dir);
	if(ret != -1)	{
		md_drive_set(ret);
	}
	if(np2cfg.mountdir[cur_drive][0] == '\0')	{
		return new_dir;
	}
	root_str = PathSkipRoot(new_dir);
	if(root_str)	{
		// Absolute patch. Switch back to virtual root directory
		new_dir = root_str;
		SetCurrentDirectory(np2cfg.mountdir[cur_drive]);
	}
	return new_dir;
}

BOOL md_drive_free_space(UINT8 drive)	{

	DWORD sectors_per_cluster;
	DWORD bytes_per_sector;
	DWORD free_clusters;
	DWORD total_clusters;
	BOOL ret;

	ret = GetDiskFreeSpace(NULL, &sectors_per_cluster, &bytes_per_sector, &free_clusters, &total_clusters);
	if(ret)	{
		CPU_AX = min(sectors_per_cluster, 0xfffe); // 0xffff = fail code
		CPU_BX = min(free_clusters, 0xffff);
		CPU_CX = min(bytes_per_sector, 0xffff);
		CPU_DX = min(total_clusters, 0xffff);
	} else {
		CPU_AX = 0xffff;
	}
	return TRUE;
}
/// ==============

/// Directory handling
/// ==================
BOOL md_dir_set(UINT32 dir_seg4, UINT16 dir_off) {

	CHAR dir_raw[MAX_PATH];
	TCHAR dir_ucs2[MAX_PATH];
	OEMCHAR *dos_dir;
	OEMCHAR *real_dir;
	BOOL ret;

	dos_dir = mem_read_sjis2ucs2(dir_raw, dir_ucs2, dir_seg4, dir_off, MAX_PATH);
	real_dir = md_drive_parse(dos_dir);
	
	// Changing to an empty string would lead to an error, crashing DOS
	if(real_dir[0] == _T('\0'))	{
		ret = 1;
	} else {
		ret = SetCurrentDirectory(real_dir);
	}
	// Don't set errors here!
	// Changing the registers quickly leads to
	// DOS crashing inside its own DIR function
	if(ret) {
		if(PathIsRelative(dos_dir)) {
			OEMCHAR tmp[MAX_PATH];
			lstrcpy(tmp, cur_dir);

			PathAddBackslash(tmp);
			lstrcat(tmp, real_dir);
			PathCanonicalize(cur_dir, tmp);
		} else {
			lstrcpy(cur_dir, real_dir);
		}
	}
	// Don't override DOS' handling of this one
	return FALSE;
}

BOOL md_dir_create(UINT32 dir_seg4, UINT16 dir_off) {

	CHAR dir_raw[MAX_PATH];
	TCHAR dir_ucs2[MAX_PATH];
	OEMCHAR *dir_str;
	BOOL ret;

	dir_str = mem_read_sjis2ucs2(dir_raw, dir_ucs2, dir_seg4, dir_off, MAX_PATH);
	ret = CreateDirectory(dir_str, NULL);
	md_set_error(ret);
	return FALSE;
}

BOOL md_dir_remove(UINT32 dir_seg4, UINT16 dir_off) {

	CHAR dir_raw[MAX_PATH];
	TCHAR dir_ucs2[MAX_PATH];
	OEMCHAR *dir_str;
	BOOL ret;

	dir_str = mem_read_sjis2ucs2(dir_raw, dir_ucs2, dir_seg4, dir_off, MAX_PATH);
	ret = RemoveDirectory(dir_str);
	md_set_error(ret);
	return FALSE;
}
/// ==================

void md_reset_dir() {

	cur_drive = 0;
	cur_dir[0] = '\0';
}
