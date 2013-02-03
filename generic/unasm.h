enum OPTYPE;

typedef struct _UNASM_t {
const char	*mnemonic;
	char	operand[44];
	UINT8 type_targ;
	UINT8 type_oper;
	UINT16 seg;
	UINT16 off;
} _UNASM, *UNASM;


#ifdef __cplusplus
extern "C" {
#endif

UINT unasm(UNASM r, const UINT8 *ptr, UINT leng, BOOL d, UINT32 addr);

#ifdef __cplusplus
}
#endif

