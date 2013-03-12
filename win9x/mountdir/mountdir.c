// mountdir.c - mountdir/root functions
// ------
// Author: Nmlgc

#include	"compiler.h"
#include	"oemtext.h"
#include	<shlwapi.h>
#include	"cpucore.h"
#include	"pccore.h"
#include	"md_dir.h"
#include	"md_fcb.h"
#include	"md_file.h"
#include	"md_util.h"
#include	"mountdir.h"

void md_set_error(BOOL ret)	{

	if(ret)	{
		CPU_FLAGL &= ~C_FLAG;
	} else {
		CPU_FLAGL |= C_FLAG;
		CPU_AX = GetLastError();
	}
}

void md_create()	{
	
	// The _actual_ number is 20, but Turbo C++ needs more
	md_handle_set_num(30);
}

void md_reset() {

	md_reset_file();
	md_reset_dir();
	md_reset_fcb();
}

void md_destroy()	{

	md_reset();
	md_destroy_file();
}

BOOL md_is_active() {
	
	int i;
	for(i = 0; i < MAX_DIR; i++)	{
		if(np2cfg.mountdir[i][0] != '\0')	{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL md_int21()	{

	UINT8 function = CPU_AH;
	UINT32 ds4 = CPU_STAT_PM ? DS_BASE : (CPU_DS << 4);

	if(!md_is_active())	{
		return FALSE;
	}

	SetCurrentDirectory(np2cfg.mountdir[cur_drive]);
	SetCurrentDirectory(cur_dir);

	switch(function) {
		/// Stuff we would need but will never work
		case 0x4b:	// EXEC
			// Will never work because the $Open function is called directly, without raising an interrupt
			// If only DOS interrupts were reentrant...

		/// Stuff we _might_ need
		case 0x52:	// "Get pointer to DOS INVARS"
		case 0x02:	// Character output... might be interesting
		case 0x09:	// Print string... might be interesting

		/// Stuff we, explicitly, do _not_ need
		case 0x01:	// "Keyboard input with echo"
		case 0x06:	// "Direct Console I/O"
		case 0x0a:	// "Buffered keyboard input"
		case 0x19:	// "Get current default drive" - not necessary since we don't change that ourselves
		case 0x25:	// "Set interrupt vector"
		case 0x29:	// "Parse a filename into a FCB"
		case 0x2a:	// "Get date"
		case 0x2b:	// "Set date"
		case 0x2c:	// "Get time"
		case 0x2d:	// "Set time"
		case 0x2f:	// "Get disk transfer address"
		case 0x30:	// "Get DOS version number"
		case 0x31:	// "Terminate Process and Remain Resident"
		case 0x33:	// "Get/Set System Values (Ctl-Break/Boot Drive)"
		case 0x34:	// "Get Address to DOS Critical Flag"
		case 0x35:	// "Get vector"
		case 0x37:	// "Get/Set Switch Character"
		case 0x38:	// "Get/Set Country Dependent Information"
		case 0x47:	// " Get current directory"
		case 0x48:	// "Allocate memory blocks"
		case 0x49:	// "Free allocated memory blocks"
		case 0x4a:
		case 0x4c:	// "Terminate Process With Return Code"
		case 0x4d:
		case 0x50:
		case 0x51:	// "Get Current Process ID"
		case 0x58:
		case 0x5d:
		case 0x62:	// "Get PSP address"
		case 0x63:	// "Get system lead byte table"
		case 0x6c:
			break;
		default:
			break;
	}

	if(cur_drive < MAX_DIR && np2cfg.mountdir[cur_drive][0] != '\0')	{
		switch(function) {
			/// FCB stuff
			case 0x0f:
				break;
			case 0x10:
				break;
			case 0x11:	return md_fcb_find_first(ds4, CPU_DX);
			case 0x12:	return md_fcb_find_next(ds4, CPU_DX);
			case 0x13:	return md_fcb_delete(ds4, CPU_DX);
			case 0x14:
			case 0x15:
			case 0x16:
				break;
			case 0x17:	return md_fcb_rename(ds4, CPU_DX);
			case 0x21:
			case 0x22:
			case 0x23:
			case 0x24:
			case 0x27:
			case 0x28:
				break;
			case 0x36:	return md_drive_free_space(CPU_DL);
			case 0x39:	return md_dir_create(ds4, CPU_DX);
			case 0x3a:	return md_dir_remove(ds4, CPU_DX);
			case 0x3c:	return md_file_open(ds4, CPU_DX, 2, CREATE_ALWAYS, CPU_CX);
			case 0x3d:	return md_file_open(ds4, CPU_DX, CPU_AL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL);
			case 0x3e:	return md_file_close(CPU_BX);
			case 0x3f:	return md_file_read(CPU_BX, ds4, CPU_DX, CPU_CX);
			case 0x40:	return md_file_write(CPU_BX, ds4, CPU_DX, CPU_CX);
			case 0x41:	return md_file_delete(ds4, CPU_DX);
			case 0x42:	return md_file_seek(CPU_BX, CPU_CX, CPU_DX, CPU_AL);
			case 0x43:	return md_file_attrib(CPU_AL, ds4, CPU_DX, &CPU_CX);
			case 0x44:	return md_ioctl(CPU_AL, CPU_BX, CPU_CX, ds4, CPU_DX);
			case 0x45:	return md_handle_duplicate(CPU_BX);
			case 0x4e:	return md_find_first(ds4, CPU_DX, CPU_CX);
			case 0x4f:	return md_find_next();
			case 0x57:	return md_file_time(CPU_AL, CPU_BX, &CPU_CX, &CPU_DX);
		}
	}

	switch(function)	{
		case 0x0e:	return md_drive_set(CPU_DL);
		case 0x1a:	return md_dta_set(ds4, CPU_DX);
		
		case 0x67:	return md_handle_set_num(CPU_BX);
		case 0x3b:	return md_dir_set(ds4, CPU_DX);
	}
	return FALSE;
}
