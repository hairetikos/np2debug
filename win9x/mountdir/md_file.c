// md_file.c - mountdir/file handling
// ------
// Author: Nmlgc

#include	"compiler.h"
#include	"oemtext.h"
#include	<shlwapi.h>
#include	"cpucore.h"
#include	"pccore.h"
#include	"md_dir.h"
#include	"md_file.h"
#include	"md_util.h"
#include	"mountdir.h"

/// DOS state
/// =========
// Limit of both DOS and the C runtime
UINT16 dos_handle_max;
// DOS needs the bottom 5 for:
//  0: STDIN (CON)
//  1: STDOUT (CON)
//  2: STDERR (CON)
//  3: STDAUX (AUX)
//  4: STDPRN (PRN)
// And Turbo C++ needs 5 for... something
UINT16 dos_handle_start = 6;

HANDLE *file_handles = NULL;
/// =========

/// Handle... handling
/// ==================
HANDLE *md_handle_lookup(UINT16 h_dos)	{
	if(
		h_dos < dos_handle_start ||
		h_dos >= dos_handle_max ||
		!file_handles[h_dos]
	)	{
		return NULL;
	}
	return &file_handles[h_dos];
}

UINT16 md_handle_append(HANDLE h_w32) {

	size_t i;
	for(i = dos_handle_start; i < dos_handle_max; i++)	{
		if(!file_handles[i])	{
			file_handles[i] = h_w32;
			return i;
		}
	}
	return -1;
}

BOOL md_handle_set_num(UINT16 num_handles)	{

	dos_handle_max = num_handles;
	file_handles = (HANDLE *)realloc(file_handles, sizeof(HANDLE) * num_handles);

	md_set_error(file_handles != NULL);

	// Better don't override DOS' handling of this function here
	return FALSE;
}

BOOL md_handle_duplicate(UINT16 h_dos)	{

	DWORD ret;
	HANDLE *h_w32;
	HANDLE h_new;

	h_w32 = md_handle_lookup(h_dos);
	if(!h_w32)	{
		return FALSE;
	}

	ret = DuplicateHandle(GetCurrentProcess(), h_w32, GetCurrentProcess(), &h_new, 0, FALSE, DUPLICATE_SAME_ACCESS);
	md_set_error(ret);
	if(ret)	{
		CPU_AX = md_handle_append(h_new);
	}
	return TRUE;
}
/// ==================

/// File handling
/// =============
BOOL md_file_open(UINT32 fn_seg4, UINT16 fn_off, UINT8 flag_access, DWORD flag_create, DWORD flag_attrib)	{

	HANDLE ret;
	int drive;
	DWORD access = GENERIC_READ;

	CHAR mem_str_raw[MAX_PATH];
	TCHAR mem_str_ucs2[MAX_PATH];
	TCHAR *mem_str;

	TCHAR *mem_str_start;

	VERBOSE(("md_file_open(%08x:%04x)", fn_seg4, fn_off));

	mem_str = mem_read_sjis2ucs2(mem_str_raw, mem_str_ucs2, fn_seg4, fn_off, MAX_PATH);
		
	drive = PathGetDriveNumber(mem_str);
	if(drive != -1)	{
		md_drive_set(drive);
		mem_str_start = PathFindNextComponent(mem_str);
		if(mem_str_start[0] == _T('\0'))	{
			mem_str_start = mem_str;
		}
	}
	else {
		mem_str_start = mem_str;
	}
	if(mem_str[0] == _T('\\')) {
		md_drive_set(cur_drive);
		mem_str_start++;
	}

	if(
		!lstrcmp(mem_str_start, _T("CON")) ||
		!lstrcmp(mem_str_start, _T("AUX")) ||
		!lstrcmp(mem_str_start, _T("NUL")) ||
		!lstrcmp(mem_str_start, _T("PRN"))
		)	{
		return FALSE;
	}

	access |= (flag_access & 0x3) >= 0x01 ? GENERIC_WRITE : 0;

	ret = CreateFile(mem_str_start, access, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, flag_create, flag_attrib, NULL);
	md_set_error(ret != INVALID_HANDLE_VALUE);
	if(ret != INVALID_HANDLE_VALUE)	{
		CPU_AX = md_handle_append(ret);
	}
	return TRUE;
}

BOOL md_file_read(UINT16 h_dos, UINT32 buf_seg4, UINT16 buf_off, UINT16 buf_len)	{

	BYTE* buf = (BYTE*)alloca(buf_len);
	DWORD byte_ret;
	BOOL ret;
	HANDLE *h_w32;

	h_w32 = md_handle_lookup(h_dos);
	if(!h_w32)	{
		return FALSE;
	}

	ret = ReadFile(*h_w32, buf, buf_len, &byte_ret, NULL);

	md_set_error(ret);
	if(ret)	{
		MEML_WRITES(buf_seg4 + buf_off, buf, buf_len);
		CPU_AX = byte_ret;
	}
	return TRUE;
}

BOOL md_file_write(UINT16 h_dos, UINT32 buf_seg4, UINT16 buf_off, UINT16 buf_len)	{

	BYTE* buf = (BYTE*)alloca(buf_len);
	DWORD byte_ret;
	BOOL ret;
	HANDLE *h_w32;

	h_w32 = md_handle_lookup(h_dos);
	if(!h_w32)	{
		return FALSE;
	}
	MEML_READS(buf_seg4 + buf_off, buf, buf_len);
	ret = WriteFile(*h_w32, buf, buf_len, &byte_ret, NULL);

	md_set_error(ret);
	if(ret)	{
		CPU_AX = byte_ret;
	}
	return TRUE;
}

BOOL md_file_seek(UINT16 h_dos, UINT16 offset_hi, UINT16 offset_lo, UINT8 origin)	{
	
	DWORD ret;
	HANDLE *h_w32;

	h_w32 = md_handle_lookup(h_dos);
	if(!h_w32)	{
		return FALSE;
	}
	ret = SetFilePointer(*h_w32, (offset_hi << 16) + offset_lo, NULL, origin);

	md_set_error(ret != 0xffffffff);
	if(ret != 0xffffffff)	{
		CPU_DX = ret >> 16;
		CPU_AX = ret & 0xffff;
	}
	return TRUE;
}

BOOL md_file_attrib(UINT8 mode, UINT32 fn_seg4, UINT16 fn_off, UINT16 *attrib)	{

	CHAR fn_raw[MAX_PATH];
	TCHAR fn_ucs2[MAX_PATH];
	OEMCHAR *fn_str;
	DWORD ret;

	fn_str = mem_read_sjis2ucs2(fn_raw, fn_ucs2, fn_seg4, fn_off, MAX_PATH);
	if(mode)	{
		ret = SetFileAttributes(fn_str, *attrib);
		md_set_error(ret);
	} else {
		ret = GetFileAttributes(fn_str);
		md_set_error(ret != 0xffffffff);
		if(ret != 0xffffffff)	{
			*attrib = ret;
		}
	}
	return TRUE;
}

BOOL md_file_time(UINT8 func, UINT16 h_dos, UINT16 *dos_date, UINT16 *dos_time) {

	HANDLE *h_w32;
	FILETIME win_time;
	BOOL ret;
	
	h_w32 = md_handle_lookup(h_dos);
	if(!h_w32)	{
		return FALSE;
	}

	if(func == 1)	{
		// Set
		dostime2wintime(&win_time, *dos_date, *dos_time);
		ret = SetFileTime(*h_w32, NULL, NULL, &win_time);
	} else {
		// Get
		ret = GetFileTime(*h_w32, NULL, NULL, &win_time);
		if(ret)	{
			SYSTEMTIME systime;
			FileTimeToSystemTime(&win_time, &systime);
			*dos_date = date2dos(&systime);
			*dos_time = time2dos(&systime);
		}
	}
	md_set_error(ret);
	return TRUE;
}

BOOL md_file_close(UINT16 h_dos)	{

	DWORD ret;
	HANDLE *h_w32;

	h_w32 = md_handle_lookup(h_dos);
	if(!h_w32)	{
		return FALSE;
	}
	ret = CloseHandle(*h_w32);
	*h_w32 = NULL;
	md_set_error(ret);
	return TRUE;
}

BOOL md_file_delete(UINT32 fn_seg4, UINT16 fn_off) {

	CHAR fn_raw[MAX_PATH];
	TCHAR fn_ucs2[MAX_PATH];
	OEMCHAR *fn_str;
	DWORD ret;

	fn_str = mem_read_sjis2ucs2(fn_raw, fn_ucs2, fn_seg4, fn_off, MAX_PATH);
	ret = DeleteFile(fn_str);
	md_set_error(ret);
	return TRUE;
}
/// =============

/// IOCTL
/// =====
BOOL md_ioctl_info_get(UINT16 h_dos)	{

	DWORD size, pos;
	HANDLE *h_w32;

	h_w32 = md_handle_lookup(h_dos);
	if(!h_w32)	{
		return FALSE;
	}
	CPU_DX = 0;

	// EOF check
	size = GetFileSize(*h_w32, NULL);
	if(size == 0xffffffff)	{
		md_set_error(size != 0xffffffff);
		return TRUE;
	}
	pos = SetFilePointer(*h_w32, 0, NULL, SEEK_CUR);
	if(pos == 0xffffffff)	{
		md_set_error(pos != 0xffffffff);
		return TRUE;
	}

	if(pos != size)	{
		CPU_DX |= 0x40;
	}
	CPU_DX |= cur_drive;
	md_set_error(1);
	return TRUE;
}

BOOL md_ioctl(UINT8 func, UINT16 h_dos, UINT16 len, UINT32 buf_seg4, UINT16 buf_off)	{
	
	if(h_dos < dos_handle_start)	{
		return FALSE;
	}
	switch(func)	{
		case 0x00:	return md_ioctl_info_get(h_dos);
	}
	return FALSE;
}
/// =====

void md_reset_file() {

	ZeroMemory(file_handles, sizeof(HANDLE) * dos_handle_max);
}

void md_destroy_file() {

	free(file_handles);
	file_handles = NULL;
}