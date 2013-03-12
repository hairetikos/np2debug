// md_dir.h - mountdir/directory handling
// ------
// Author: Nmlgc

extern INT8 cur_drive;
extern OEMCHAR cur_dir[MAX_PATH];

#ifdef __cplusplus
extern "C" {
#endif

/// Drive handling
/// ==============
BOOL md_drive_set(UINT8 drive);
OEMCHAR *md_drive_parse(OEMCHAR *new_dir);
BOOL md_drive_free_space(UINT8 drive);
/// ==============

/// Directory handling
/// ==================
BOOL md_dir_set(UINT32 dir_seg4, UINT16 dir_off);
BOOL md_dir_create(UINT32 dir_seg4, UINT16 dir_off);
BOOL md_dir_remove(UINT32 dir_seg4, UINT16 dir_off);
/// ==================

void md_reset_dir();

#ifdef __cplusplus
}
#endif
