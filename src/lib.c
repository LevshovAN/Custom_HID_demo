
#include "lib.h"
//#include "main.h"


//Преобразование байта в текстовое шестнадцатеричное представление
//Параметры: byte-исходный байт, hex-указатель на 2-х байтовую область для результата
void btohex(BYTE byte, BYTE* hex)
{
    BYTE res[2];

    res[1]=(byte>>4)+0x30;
    if(res[1]>=0x3A)
        res[1]+=7;
    res[0]=(byte&0x0F)+0x30;
    if(res[0]>=0x3A)
        res[0]+=7;
    *hex=res[0];
    *(hex+1)=res[1];
}

//Преобразование знакового байта в текстовое десятичное представление
//Параметры: byte-исходный байт, dec-указатель на 4-х байтовую область для результата
//Возврат: количество цифр
BYTE Signed_byte_to_dec(signed char byte, BYTE* dec){
    if(byte<0){
      dec[0]='-';
      byte = 256 - byte;
    }
    return wtodec(byte,&dec[1])+1;
}

//Преобразование слова в текстовое десятичное представление
//Параметры: word-исходное число, dec-указатель на 6-х байтовую область для результата
//Возврат: количество цифр
BYTE wtodec(WORD word, BYTE* dec){
  DWORD dig, idx;
  BYTE buf[5];
  idx=0;
  do{
    dig = word/10;
    buf[idx++] = word-(dig*10)+'0';
    word = dig;
  }while(dig);

  for(DWORD i=0; i<idx; i++){
    dec[i] = buf[idx-i-1];
  }
  return idx;
}

//Преобразование слова в текстовое десятичное представление
//Параметры: dword-исходное число, dec-указатель на 11-х байтовую область для результата
//Возврат: количество цифр
BYTE dwtodec(DWORD dword, BYTE* dec){
  DWORD dig;
  BYTE idx;
  BYTE buf[10];
  idx=0;
  do{
    dig = dword/10;
    buf[idx++] = dword-(dig*10)+'0';
    dword = dig;
  }while(dig);

  for(BYTE i=0; i<idx; i++){
    dec[i] = buf[idx-i-1];
  }
  return idx;
}


//Преобразование слова в текстовое десятичное представление, с дробной частью
//Параметры:
//word - исходное число,
//dec - указатель на область для результата (до 15 байт)
//dot - Позиция тесятичной точки
//Возврат: количество цифр

BYTE wtodec_dot(WORD word, BYTE* dec, BYTE  dot){
  BYTE idx;
  
  idx = wtodec(word, dec);
  if(dot<=12){
    if(dot==0){//точка попадает в конец числа: добавить точку и ноль
      dec[idx++] = '.';
      dec[idx++] = '0';
    }else if(idx>dot){//точка попадает между цифр
      for(BYTE i = 0; i<dot; i++){// раздвижка для вставки точки
        dec[idx-i] = dec[idx-i-1];
      }
      dec[idx-dot] = '.';
      idx++;
    }else{ //точка перед цифрами: добавить нули и точку
      for(BYTE i = 0; i<idx; i++){
        dec[dot+1-i] = dec[idx-i-1];
      }
      for(BYTE i = 0; i<(dot-idx); i++){
        dec[i+2] = '0';
      }
      dec[0] = '0';
      dec[1] = '.';
      idx = dot+2;
    }
  }
  return idx;
}
/*
#define CRC_POLY 0x1021
WORD CalcCRC(BYTE *btData, WORD btLen)
{
  WORD wCRC=0x0000;
  for (WORD i=0;i<btLen;i++)
  {
    ((BYTE*)&wCRC)[1]=HIBYTE(wCRC)^btData[i];
    for (BYTE j=0;j<8;j++)
    if (wCRC & 0x8000)
    wCRC=(wCRC<<1)^CRC_POLY;
    else wCRC=wCRC<<1;
  }
  return wCRC;
}*/
/*
void crc(WORD *acc, BYTE sbyte)
{
    BYTE i;
    ((BYTE*)acc)[1]^=sbyte;
    for(i=0; i<8; i++)
        ((*acc)&0x8000) ? (*acc)=((*acc)<<1)^0x1021 : (*acc)=((*acc)<<1);
}*/

void _memset(void *pDest, BYTE btData, int len)
{
  for (int i=0;i<len;i++)
    ((BYTE*)pDest)[i]=btData;
}
void _memcpy(void *pDest, void *pSource, int len)
{
  for (int i=0;i<len;i++)
    ((BYTE*)pDest)[i]=((BYTE*)pSource)[i];
}
void _memwcpy(void *pDest, void *pSource, int len)
{
  for (int i=0;i<len;i++)
    ((WORD*)pDest)[i]=((WORD*)pSource)[i];
}
bool _memcmp(void *pData1, void *pData2, int len)
{
  for (int i=0;i<len;i++)
    if (((BYTE*)pData1)[i]!=((BYTE*)pData2)[i]) return true;
  return false;
}
int _abs(int a)
{
  if (a>0) return a;
  return (~a)+1;
}

WORD Revers_Word(WORD word){
 WORD ret; 
    ret = word<<8;
    ret+= word>>8;
  return ret;
}


//Преобразования байт <-> BCD
BYTE bcdtob(BYTE v)
{
    return ((v>>4)*10)+(v&0x0F);
}
BYTE btobcd(BYTE v)
{
    return ((v/10)<<4)|(v%10);
}

//-----------------------------------------------------------------------
//  Table of CRC-16's of all single-byte values (made by make_crc_table)
//  CRC-CCITT lookup table	 Polynominal = 0x1021
//-----------------------------------------------------------------------
const WORD crc_ccitt_tab[256] = {
	0x0000,	0x1021,	0x2042,	0x3063,	0x4084,	0x50a5,	0x60c6,	0x70e7,
	0x8108,	0x9129,	0xa14a,	0xb16b,	0xc18c,	0xd1ad,	0xe1ce,	0xf1ef,
	0x1231,	0x0210,	0x3273,	0x2252,	0x52b5,	0x4294,	0x72f7,	0x62d6,
	0x9339,	0x8318,	0xb37b,	0xa35a,	0xd3bd,	0xc39c,	0xf3ff,	0xe3de,
	0x2462,	0x3443,	0x0420,	0x1401,	0x64e6,	0x74c7,	0x44a4,	0x5485,
	0xa56a,	0xb54b,	0x8528,	0x9509,	0xe5ee,	0xf5cf,	0xc5ac,	0xd58d,
	0x3653,	0x2672,	0x1611,	0x0630,	0x76d7,	0x66f6,	0x5695,	0x46b4,
	0xb75b,	0xa77a,	0x9719,	0x8738,	0xf7df,	0xe7fe,	0xd79d,	0xc7bc,
	0x48c4,	0x58e5,	0x6886,	0x78a7,	0x0840,	0x1861,	0x2802,	0x3823,
	0xc9cc,	0xd9ed,	0xe98e,	0xf9af,	0x8948,	0x9969,	0xa90a,	0xb92b,
	0x5af5,	0x4ad4,	0x7ab7,	0x6a96,	0x1a71,	0x0a50,	0x3a33,	0x2a12,
	0xdbfd,	0xcbdc,	0xfbbf,	0xeb9e,	0x9b79,	0x8b58,	0xbb3b,	0xab1a,
	0x6ca6,	0x7c87,	0x4ce4,	0x5cc5,	0x2c22,	0x3c03,	0x0c60,	0x1c41,
	0xedae,	0xfd8f,	0xcdec,	0xddcd,	0xad2a,	0xbd0b,	0x8d68,	0x9d49,
	0x7e97,	0x6eb6,	0x5ed5,	0x4ef4,	0x3e13,	0x2e32,	0x1e51,	0x0e70,
	0xff9f,	0xefbe,	0xdfdd,	0xcffc,	0xbf1b,	0xaf3a,	0x9f59,	0x8f78,
	0x9188,	0x81a9,	0xb1ca,	0xa1eb,	0xd10c,	0xc12d,	0xf14e,	0xe16f,
	0x1080,	0x00a1,	0x30c2,	0x20e3,	0x5004,	0x4025,	0x7046,	0x6067,
	0x83b9,	0x9398,	0xa3fb,	0xb3da,	0xc33d,	0xd31c,	0xe37f,	0xf35e,
	0x02b1,	0x1290,	0x22f3,	0x32d2,	0x4235,	0x5214,	0x6277,	0x7256,
	0xb5ea,	0xa5cb,	0x95a8,	0x8589,	0xf56e,	0xe54f,	0xd52c,	0xc50d,
	0x34e2,	0x24c3,	0x14a0,	0x0481,	0x7466,	0x6447,	0x5424,	0x4405,
	0xa7db,	0xb7fa,	0x8799,	0x97b8,	0xe75f,	0xf77e,	0xc71d,	0xd73c,
	0x26d3,	0x36f2,	0x0691,	0x16b0,	0x6657,	0x7676,	0x4615,	0x5634,
	0xd94c,	0xc96d,	0xf90e,	0xe92f,	0x99c8,	0x89e9,	0xb98a,	0xa9ab,
	0x5844,	0x4865,	0x7806,	0x6827,	0x18c0,	0x08e1,	0x3882,	0x28a3,
	0xcb7d,	0xdb5c,	0xeb3f,	0xfb1e,	0x8bf9,	0x9bd8,	0xabbb,	0xbb9a,
	0x4a75,	0x5a54,	0x6a37,	0x7a16,	0x0af1,	0x1ad0,	0x2ab3,	0x3a92,
	0xfd2e,	0xed0f,	0xdd6c,	0xcd4d,	0xbdaa,	0xad8b,	0x9de8,	0x8dc9,
	0x7c26,	0x6c07,	0x5c64,	0x4c45,	0x3ca2,	0x2c83,	0x1ce0,	0x0cc1,
	0xef1f,	0xff3e,	0xcf5d,	0xdf7c,	0xaf9b,	0xbfba,	0x8fd9,	0x9ff8,
	0x6e17,	0x7e36,	0x4e55,	0x5e74,	0x2e93,	0x3eb2,	0x0ed1,	0x1ef0
};

WORD CalcTableCRC(BYTE *btData, WORD wLen)
{
  WORD wCRC=0x0000;
  for (WORD i=0;i<wLen;i++)
  {
	wCRC=(wCRC<<8)^crc_ccitt_tab[(wCRC>>8)^ btData[i]];
  }
  return wCRC;
}

