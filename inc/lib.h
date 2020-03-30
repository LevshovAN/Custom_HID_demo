//************ lib.cpp **********************
#include "defs.h"

//BYTE btodec(BYTE byte, BYTE* dec);
void btohex(BYTE byte, BYTE* hex);
BYTE wtodec(WORD word, BYTE* dec);
WORD CalcCRC(BYTE *btData, WORD btLen);
void _memcpy(void *pDest, void *pSource, int len);
void _memwcpy(void *pDest, void *pSource, int len);
int _abs(int a);
bool _memcmp(void *pData1, void *pData2, int len);
void _memset(void *pDest, BYTE btData, int len);
void crc(WORD *acc, BYTE sbyte);
BYTE bcdtob(BYTE v);
BYTE btobcd(BYTE v);
BYTE Signed_byte_to_dec(signed char byte, BYTE* dec);
BYTE wtodec_dot(WORD word, BYTE* dec, BYTE  dot);
BYTE dwtodec(DWORD dword, BYTE* dec);
WORD Revers_Word(WORD word);


WORD CalcTableCRC(BYTE *btData, WORD wLen);
extern const WORD crc_ccitt_tab[256];
//extern DWORD dwCPULoad;
