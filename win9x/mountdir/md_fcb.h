// md_fcb.h - mountdir/handling of anything that uses DOS' three-letter-acronym structures
// ------
// Author: Nmlgc

#define DOS_FN_LEN 13 /* 8 + '.' + 3 + '\0' */

#if !defined(__GNUC__)
#pragma pack(push, 1)
#endif
typedef struct {
	UINT8	drive;
	CHAR 	s_fn[8];
	CHAR		s_ext[3];
	UINT8	s_attrib;
	UINT16	dir_id;
	UINT16	dir_cluster;
	UINT16	reserved[2];
	UINT8	f_attrib;
	UINT16	f_time;
	UINT16	f_date;
	UINT32	f_size;
	CHAR		f_name[DOS_FN_LEN];
#if !defined(__GNUC__)
} DOS_DTA;
#else
} __attribute__ ((packed)) DOS_DTA;
#endif
typedef struct {
	UINT8	drive;
	CHAR 	f_name[8];
	CHAR		f_ext[3];
	CHAR		attrib;
	UINT8	stuff[10];
	UINT16	f_time;
	UINT16	f_date;
	UINT16	f_sector; // ???
	UINT32	f_size;
#if !defined(__GNUC__)
} DOS_FCB;
#else
} __attribute__ ((packed)) DOS_FCB;
#endif
typedef struct {
	UINT8	extended;
	UINT8 	reserved[5];
	UINT8	attrib;
	DOS_FCB	fcb;
#if !defined(__GNUC__)
} DOS_FCBEXT;
#else
} __attribute__ ((packed)) DOS_FCBEXT;
#endif
// Modified FCB for renaming
typedef struct {
	UINT8	drive;
	CHAR 	old_name[8];
	CHAR		old_ext[3];
	CHAR		padding[5];
	CHAR 	new_name[8];
	CHAR		new_ext[3];
#if !defined(__GNUC__)
} DOS_FCBREN;
#else
} __attribute__ ((packed)) DOS_FCBREN;
#endif
#if !defined(__GNUC__)
#pragma pack(pop)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/// Basic find functions
/// ====================
BOOL md_find_first_base(CHAR *mem_str_raw, UINT16 attrib);
BOOL md_find_first(UINT32 fn_seg4, UINT16 fn_off, UINT16 attrib);
BOOL md_find_next();
/// ====================

/// Disk Transfer Area
/// ==================
BOOL md_dta_set(UINT32 addr_seg, UINT16 addr_off);
void md_dta_write(DOS_DTA *dta, WIN32_FIND_DATA *w32fd);
/// ==================

/// File Control Block
/// ==================
BOOL md_fcb_rename(UINT32 fcb_seg4, UINT16 fcb_off);
BOOL md_fcb_delete(UINT32 fcb_seg4, UINT16 fcb_off);
BOOL md_fcb_find_first(UINT32 fcb_seg4, UINT16 fcb_off);
BOOL md_fcb_find_next(UINT32 fcb_seg4, UINT16 fcb_off);
void md_fcb_write_find(WIN32_FIND_DATA *w32fd);
/// ==================

void md_reset_fcb();

#ifdef __cplusplus
}
#endif
