enum OPTYPE;

enum {
	MI_WRITE = 0,
	MI_READ = 1
};

typedef struct {
	UINT8 type;
	UINT16 seg;
	UINT16 off;
} UNASM_MEMINFO;

typedef struct _UNASM_t {
const char	*mnemonic;
	char	operand[44];
	UNASM_MEMINFO meminf[2];
} _UNASM, *UNASM;


#ifdef __cplusplus
extern "C" {
#endif

UINT unasm(UNASM r, const UINT8 *ptr, UINT leng, BOOL d, UINT32 addr);

#ifdef __cplusplus
}
#endif

