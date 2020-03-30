/******************************************************************************/
/* UART.H                                                                     */
/* Работа с UART                                                              */
/******************************************************************************/

#ifndef _UART_H
#define _UART_H


#include "stm32f4xx.h"
#include "defs.h"
#include "lib.h"

//вычисление значения делителя (регистры DLL и DLM)
#define UART_DIV_CALC(f,baud)  (ROUND_DIV(f,baud*16L))

//проверка отклонения скорости UARTа от заданной болеечем на заданный (deflection) процент 
#define UART_CHECK_BAUD(f,baud,deflection) C_ASSERT((f*100LL <= UART_DIV_CALC(f,baud)*baud*16*(100LL+deflection))&&(f*100LL >= UART_DIV_CALC(f,baud)*baud*16*(100LL-deflection)) );


/*********************** Конфигурация класса CUart ****************************/

#define TX_FIFO_degree     10                //порядок глубины очереди

/******************************************************************************/

//--- контроль порядка глубины очереди ---
#if TX_FIFO_degree<2
  #error "Порядок глубины очереди не может быть меньше 2"
#endif

#if TX_FIFO_degree>16
  #error "Порядок глубины очереди не может быть больше разрядности WORD"
#endif

#define FIFO_BUF_MASK  ((1<<TX_FIFO_degree)-1) //маска адреса в очереди (размер-1)	
/*
//Идентификаторы источника прерываний
#define RX_LINE_STATUS	0x03
#define RX_DATA			0x02
#define CHAR_TIMEOUT	0x06
#define TX_DATA			0x01

//Маски прерываний прерываний
#define RDAIE_MASK       1
#define THREIE_MASK     (1<<1)
#define RXLSIE          (1<<2)
*/



  // Функции
  void uart_TXBufWrite(BYTE data);
  bool uart_TXBufRead(BYTE * data);
	
  bool uart_Init(BYTE num, WORD baudrate_div, BYTE ier, DWORD UartHandler);
	void uart_Init1(void);

//  bool uart_Init(BYTE num, WORD baudrate_div, BYTE ier);
  void uart_SendByte(BYTE byte);

  void uart_SendStrr(char * str);
  void uart_SendHex(BYTE byte);
	void uart_SendHex4(DWORD dwData);
  void uart_SendDec(DWORD dword);
	void uart_SendLongDec(unsigned long long llnumber);

		
  void uart_BufByte(BYTE byte);
  void uart_BufStrr(char * str);
  void uart_BufHex(BYTE byte);
  void uart_BufBin(DWORD d, BYTE num_bit);
  void uart_BufBool(BYTE byte);
  void uart_BufDec(DWORD byte);
  void uart_BufDec_SignByte(BYTE byte);
  void uart_BufDecDot(WORD word,BYTE dot);

  void uart_Send(void);
  void uart_SendBurst(void);
  BYTE uart_ReceiveByte(BYTE *byte);
	
/*
//Класс UART
class CUart{
  // Данные
public:
                      //буфер передачи
  
private:
public:
//  void Handler(void);

  inline CUart &operator <<(char *szData)
  {
    BufStrr(szData);
    return *this;
  };
  
  inline CUart &operator <<(BYTE btData)
  {
    BufHex(btData);
    BufByte(32);
    return *this;
  };
  
  inline CUart &operator <<(bool bData)
  {
    BufBool(bData);
    BufByte(32);
    return *this;
  };
  
  
  inline CUart &operator <<(char chData)
  {
    BufByte(chData);
    return *this;
  };
  
  inline CUart &operator <<(WORD wData)
  {
    BufHex(HI(wData));
    BufHex(LO(wData));
    BufByte(32);
    return *this;
  };
  
  inline CUart &operator <<(DWORD dwData)
  {
    BufHex(HI(HIWORD(dwData)));
    BufHex(LO(HIWORD(dwData)));
    BufHex(HI(LOWORD(dwData)));
    BufHex(LO(LOWORD(dwData)));
    BufByte(32);
    return *this;
  };
  
  inline CUart &operator <<(unsigned long long ullData)
  {
    BufHex(HI(HIWORD(ullData>>32)));
    BufHex(LO(HIWORD(ullData>>32)));
    BufHex(HI(LOWORD(ullData>>32)));
    BufHex(LO(LOWORD(ullData>>32)));
    
    BufHex(HI(HIWORD(ullData&0xFFFFFFFF)));
    BufHex(LO(HIWORD(ullData&0xFFFFFFFF)));
    BufHex(HI(LOWORD(ullData&0xFFFFFFFF)));
    BufHex(LO(LOWORD(ullData&0xFFFFFFFF)));
    BufByte(32);
    return *this;
  };
	
  inline CUart &operator <=(char chData)
  {
    BufByte(chData);
    return *this;
  };

  inline CUart &operator <=(WORD bData)
  {
    BufDec2((WORD)bData);
    BufByte(32);
    return *this;
  };  

  inline CUart &operator <=(DWORD bData)
  {
     return * BufDec((DWORD)bData)->BufByte(32);
  };  
  
  inline CUart &operator <(char *szData)
  {
    SendStrr(szData);
    return *this;
  };
  
  inline CUart &operator <(BYTE btData)
  {
    SendHex(btData);
    SendByte(32);
    return *this;
  };
  
  inline CUart &operator <(bool bData)
  {
    SendHex((BYTE)bData);
    SendByte(32);
    return *this;
  };
  
  inline CUart &operator <(char chData)
  {
    SendByte(chData);
    return *this;
  };
  
  inline CUart &operator <(WORD wData)
  {
    SendHex(HI(wData));
    SendHex(LO(wData));
    SendByte(32);
    return *this;
  };
  
  inline CUart &operator <(DWORD dwData)
  {
    SendHex(HI(HIWORD(dwData)));
    SendHex(LO(HIWORD(dwData)));
    SendHex(HI(LOWORD(dwData)));
    SendHex(LO(LOWORD(dwData)));
    SendByte(32);
    return *this;
  };
};
*/
#endif

