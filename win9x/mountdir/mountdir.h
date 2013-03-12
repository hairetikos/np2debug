// mountdir.h - mountdir/root functions
// ------
// Author: Nmlgc

#ifdef __cplusplus
extern "C" {
#endif

void md_set_error(BOOL ret);

void md_create(void);
void md_reset(void);
void md_destroy(void);

BOOL md_int21();

#ifdef __cplusplus
}
#endif
