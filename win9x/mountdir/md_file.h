// md_file.h - mountdir/file handling
// ------
// Author: Nmlgc

#ifdef __cplusplus
extern "C" {
#endif

/// Handle... handling
/// ==================
HANDLE *md_handle_lookup(UINT16 h_dos);
UINT16 md_handle_append(HANDLE h_w32);
BOOL md_handle_set_num(UINT16 num_handles);
BOOL md_handle_duplicate(UINT16 h_dos);
/// ==================

/// File handling
/// =============
BOOL md_file_open(UINT32 fn_seg4, UINT16 fn_off, UINT8 flag_access, DWORD flag_create, DWORD flag_attrib);
BOOL md_file_read(UINT16 h_dos, UINT32 buf_seg4, UINT16 buf_off, UINT16 buf_len);
BOOL md_file_write(UINT16 h_dos, UINT32 buf_seg4, UINT16 buf_off, UINT16 buf_len);
BOOL md_file_seek(UINT16 h_dos, UINT16 offset_hi, UINT16 offset_lo, UINT8 origin);
BOOL md_file_attrib(UINT8 mode, UINT32 fn_seg4, UINT16 fn_off, UINT16 *attrib);
BOOL md_file_time(UINT8 func, UINT16 h_dos, UINT16 *dos_date, UINT16 *dos_time);
BOOL md_file_close(UINT16 h_dos);
BOOL md_file_delete(UINT32 fn_seg4, UINT16 fn_off);
/// =============

/// IOCTL
/// =====
BOOL md_ioctl_info_get(UINT16 h_dos);
BOOL md_ioctl(UINT8 func, UINT16 h_dos, UINT16 len, UINT32 buf_seg4, UINT16 buf_off);
/// =====

void md_reset_file();
void md_destroy_file();

#ifdef __cplusplus
}
#endif
