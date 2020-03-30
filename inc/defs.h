/*------------------------------------------------------------------------------
	defs.h
    STM32F4xx
	Определение основных дефайнов
------------------------------------------------------------------------------*/

#ifndef __DEFS_H
#define __DEFS_H

//#include <intrinsics.h>

#define BYTE        unsigned char                   //Типы данных

#define WORD        unsigned short
#define DWORD       unsigned long
#define LPCSTR      const char *
typedef enum
{
  false = 0, true  = !false
}
bool;

//#define BOOL        bool
#define PTR_REG     DWORD
#ifndef NULL
  #define NULL        0
#endif // NULL


#define ENDL        '\r'

#define BSET(x,y)   ((x) & (1 << (y)))              //Проверка установки бита
#define BCLR(x,y)   (!((x) & (1 << (y))))           //Проверка очистки бита
#define SETB(x,y)   ((x) |= (1 << (y)))             //Установка бита
#define CLRB(x,y)   ((x) &= (~(1 << (y))))            //Очистка бита
#define XORB(x,y)   ((x) ^= (1 << (y)))             //Изменение бита
#define BFLD(x,y,z) ((x) = (x) & ~(y) | (z))        //Установка нескольких бит
#define LO(x)       ((BYTE)(x))
#define HI(x)       ((BYTE)((x)>>8))
//#define LOBYTE      LO
//#define HIBYTE      HI
#define LOWORD(x)   ((WORD)(x))
#define HIWORD(x)   ((WORD)((x)>>16))
#define PTR(x)      (*(volatile BYTE*)(x))
#define PTRW(x)     (*(volatile WORD *)(x))
#define SWAP(Word) ((LOBYTE(Word)<<8)|HIBYTE(Word))
#define SWAPW(Dword) (((WORD)((Dword)<<16))|((WORD)((Dword)>>16)))
//#define Hlt()       __disable_interrupt(); while(1) {WDFEED=0xAA; WDFEED=0x55;}
#define __swap_nibbles(byte) ((byte>>4)|(byte<<4))
#define __flash const
#define countof(array) (sizeof(array)/sizeof(array[0]))
//#define offsetof(s,m) (size_t)&(((s *)0)->m)
//------------------------------------------------------------------------------

#define INT_PROTECT DWORD _prm, _fm; _prm = __get_PRIMASK(); _fm = __get_FAULTMASK()
#define INT_REPROTECT _prm = __get_PRIMASK(); _fm = __get_FAULTMASK()
#define INT_DISABLE 	__disable_irq();
#define INT_RESTORE __set_PRIMASK(_prm); __set_FAULTMASK(_fm)

//------------------------------------------------------------------------------
#define ROUND_DIV(x,y)      (((x)+(y)/2)/(y))
#define C_ASSERT(e) typedef char __C_ASSERT__[(e)?1:-1]


#endif
