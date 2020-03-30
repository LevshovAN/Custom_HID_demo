/*******************************************************************************
 * uart.cpp	Класс работы с отладочным портом UART.
 *   Поддерживается только вывод.
 *   Объект настраивается на модуль UART0 или UART1 во время выполнения программы.
 *   Не использует прерываний.
 *   Класс поддерживает множественность объектов при условии,
 *    что каждый объект настроен на свой модуль UART.
 *   Поддерживаются режимы прямого вывода и вывода через буферную очередь.
 * 
/-----------------------------------------------------------------------------*/

#include "uart.h"
//#include "main.h"

  DWORD ErrCounter;//счетчик ошибок приёма
//  BYTE Rx_buf[50];//буфер приёма
//  BYTE Rx_Ind;
  struct {
    BYTE buf[FIFO_BUF_MASK+1];
    WORD head;
    WORD InBufsize;
  } txbuf;  
	
  volatile USART_TypeDef * U;

// Инициализация UART
bool uart_Init(BYTE num, WORD baudrate_div, BYTE ier, DWORD UartHandler){


  ErrCounter = 0;
  switch(num){
    case 1:
      U = USART1;
//      PCONP_bit.PCUART0 = 1; //включение питания 
      break;
    case 2:
      U = USART2;
//      PCONP_bit.PCUART1 = 1;
      break;
    case 3:  U = USART3;   break;
    case 4:  U = UART4;   break;
    case 5:  U = UART5;   break;
    case 6:  U = USART6;   break;
   default: return false;
  } 
  
  U->BRR = baudrate_div<<4;
  U->CR2 = 0; //1 стоп бит
  U->CR3 = 0; 
  U->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE; 
  
/*  U->LCR = 0x80;             
  U->DLL = LO(baudrate_div);            //Divisor
  U->DLM = HI(baudrate_div);            //|
  U->LCR = 0x03;                        //8N1

  union{
    DWORD FCR;
    __uartfcriir_bits FCR_bit;
  } r;
  r.FCR_bit.FCRFE = 1;             //RxFifo enable
  r.FCR_bit.RFR = 1;               //RxFifo reset
  r.FCR_bit.TFR = 1;               //TxFifo reset
  r.FCR_bit.RTLS = 1;              //Trigger level = 4
  U->FCR = r.FCR;
  U->IER = ier;                     //Interrupt enable
  
  if(ier){
   return install_irq(VIC_UART0+num,UartHandler,10); 
  }*/
  return true;
}

void uart_Init1(){
  ErrCounter = 0;
   U = USART1;
}

// Передача в UART байта
void uart_SendByte(BYTE byte){
  while(!(U->SR&USART_SR_TXE));             //Wait TxFifo ready
    U->DR = byte;
}

void uart_SendHex4(DWORD dwData){
    uart_SendHex(HI(HIWORD(dwData)));
    uart_SendHex(LO(HIWORD(dwData)));
    uart_SendHex(HI(LOWORD(dwData)));
    uart_SendHex(LO(LOWORD(dwData)));
    uart_SendByte(' ');
}

// Передача в UART строки
void uart_SendStrr(char *str){
  while(*str)
    uart_SendByte(*str++);
}

// Передача в UART байт в шестнадцатиричном представлении
void uart_SendHex(BYTE byte){
  BYTE hex[2];
  btohex(byte, hex);
  uart_SendByte(hex[1]);
  uart_SendByte(hex[0]);
}

// Передача в UART  4-байтного числа в десятичном представлении
void uart_SendDec(DWORD dword){
  BYTE NumDig, dec[12];
  NumDig = dwtodec(dword, dec);
  for(unsigned char i=0;	i<NumDig;	i++){
    uart_SendByte(dec[i]);
	}
}

void uart_SendLongDec(unsigned long long llnumber){
	
	DWORD low_dword = llnumber%1000000000;
	DWORD hi_dword = llnumber/1000000000;
	if(hi_dword>0){
		uart_SendDec(hi_dword);
		//uart_SendByte('G');
		uart_SendByte('_');
		BYTE dec[12]={'0','0','0','0','0','0','0','0','0','0','0','0'};
		dwtodec(low_dword, dec);
		for(unsigned char i=0;	i<3;	i++){
			uart_SendByte(dec[i]);
		}
		//uart_SendByte('M');
		uart_SendByte('_');
		for(unsigned char i=3;	i<6;	i++){
			uart_SendByte(dec[i]);
		}
		//uart_SendByte('k');
		uart_SendByte('_');
		for(unsigned char i=6;	i<9;	i++){
			uart_SendByte(dec[i]);
		}
		
 }else{
	 uart_SendDec(low_dword);
 }
	
	
}


// Запись байта в буфер
void uart_BufByte(BYTE byte){
  uart_TXBufWrite(byte);
}

// Запись строки в буфер
void uart_BufStrr(char *str){
  while(*str)
    uart_TXBufWrite(*str++);
}

// Запись в буфер байт в шестнадцатиричном представлении
void uart_BufHex(BYTE byte){
  BYTE hex[2];
  btohex(byte, hex);
  uart_TXBufWrite(hex[1]);
  uart_TXBufWrite(hex[0]);
}

// Запись в буфер байт в двоичном представлении
void uart_BufBin(DWORD d, BYTE num_bit){
  BYTE cr;
  for(DWORD i = num_bit-1; i<32; i--){
    cr ='0';
    if(d&(1<<i)) cr='1';
    uart_TXBufWrite(cr);
  }
}

// Запись в буфер байт в 10 представлении
void uart_BufBool(BYTE byte){
  BYTE cr ='0';
  if(byte) cr = '1';
  uart_TXBufWrite(cr);
}

// Запись в буфер знакового байт в десятичном представлении
void uart_BufDec_SignByte(BYTE byte){
  BYTE NumDig,dec[5];
  NumDig = Signed_byte_to_dec(byte, dec);
  
  for(BYTE i=0; i<NumDig; i++)
    uart_TXBufWrite(dec[i]);
}

// Запись в буфер слова в десятичном представлении
void Cuart_BufDec2(WORD word){
  BYTE NumDig,dec[6];
  NumDig = wtodec(word, dec);
  
  for(BYTE i=0; i<NumDig; i++)
    uart_TXBufWrite(dec[i]);
}

// Запись в буфер 4-байтного числа в десятичном представлении
void uart_BufDec(DWORD dword){
  BYTE NumDig, dec[12];
  NumDig = dwtodec(dword, dec);
  for(BYTE i=0; i<NumDig; i++)
    uart_TXBufWrite(dec[i]);
}

// Запись в буфер слова в десятичном представлении с точкой
void uart_BufDecDot(WORD word,BYTE dot){
  BYTE NumDig,dec[15];
  NumDig = wtodec_dot(word, dec,dot);
  
  for(BYTE i=0; i<NumDig; i++)
    uart_TXBufWrite(dec[i]);
}
// Передача байт из буфера в UART
void uart_Send(void){
  if (!(U->SR&USART_SR_TXE)) return;
  BYTE byte;
  if(uart_TXBufRead(&byte)) U->DR = byte;
}

//Прием байта по опросу
//Возврат: 1-был прием байта
BYTE uart_ReceiveByte(BYTE *byte){
  if(U->SR&USART_SR_RXNE){//если есть непрочтённый бит - прочитать
    *byte = U->DR;
    return 1; 
  }
  return 0;
}

/*
//--------------------------------------------------------------------------
//! Обработчик прерывания UART 
//--------------------------------------------------------------------------
void CUart::Handler(void){
  union{
    BYTE LSR;
    __uartlsr_bits LSR_bit;
  } StLsr; //для регистра LSR
  union{
    DWORD IIR;
    __uartfcriir_bits IIR_bit;
  }IntSrc; //для регистра IIR
  BYTE tmp_data; 
  
  IntSrc.IIR = U->IIR;//прочитать IIR, убрать признак прерывания
  
  //Обработчик по источнику прерываний
  switch (IntSrc.IIR_bit.IID){
  case RX_LINE_STATUS:
  case CHAR_TIMEOUT:
  case RX_DATA:
	StLsr.LSR = U->LSR;      //чтение статуса
    while(StLsr.LSR_bit.DR){ //Если есть данные 
      tmp_data = U->RBR;     //прочитать данные
      if((StLsr.LSR_bit.PE)||(StLsr.LSR_bit.FE)||(StLsr.LSR_bit.BI)){ //Если есть ошибки PE,FE,BI посчитать
        ErrCounter++;
        //анулировать записанные в буфер данные ??
      }else{                                                          //Если нет ошибок - записать в буфер
        if(Rx_Ind<sizeof(Rx_buf)){
          Rx_buf[Rx_Ind] = tmp_data;
  //        if(Rx_Ind==0) 
  //            t1ms.SetTimer(Command_timer,EV_COMMAND_TIMEOUT,100); 

          Rx_Ind++;
        }else{
          for(BYTE i = 0; i<10; i++) BufByte(Rx_buf[i]);
          Rx_Ind = 0;
        }
        
      }
      StLsr.LSR = U->LSR;      //чтение статуса следующего байта
    }
	break;
    
  default: 
	StLsr.LSR = U->LSR;
	break;
  }
}
*/

//--------------------------------------------------------------------------
//! Добавление символа в буфер передачи
//--------------------------------------------------------------------------
void uart_TXBufWrite(BYTE data){
  if (txbuf.InBufsize<FIFO_BUF_MASK){
    INT_PROTECT;
    INT_DISABLE;
    txbuf.buf[txbuf.head] = data;
    txbuf.head = (txbuf.head+1)&FIFO_BUF_MASK;
    txbuf.InBufsize++;
    INT_RESTORE;
  }
}

//--------------------------------------------------------------------------
//! Чтение символа из буфера передачи
//--------------------------------------------------------------------------
 bool uart_TXBufRead(BYTE * data){
  if(txbuf.InBufsize){
    INT_PROTECT;
    INT_DISABLE;
    WORD tail = (txbuf.head-txbuf.InBufsize)&FIFO_BUF_MASK; 
    *data = txbuf.buf[tail];
    txbuf.InBufsize--;
    INT_RESTORE;
    return true;
  }
  return false;
}

//--------------------------------------------------------------------------
//! Передача пачки символов из буфера передачи
//--------------------------------------------------------------------------
void uart_SendBurst(){
//  if (!U->LSR_bit.THRE) return; //если передатчик пуст продолжить
  WORD num_bytes = txbuf.InBufsize;
  if(num_bytes>12) num_bytes = 12;
  if(num_bytes==0) return;  //если есть данные для записи - продолжить
  INT_PROTECT;
  INT_DISABLE;
  WORD  tail = (txbuf.head-txbuf.InBufsize)&FIFO_BUF_MASK;
  INT_RESTORE;
  for(WORD i = 0; i<num_bytes; i++){  //передавать до 12 байт
    U->DR = txbuf.buf[tail];
    tail = (tail+1)&FIFO_BUF_MASK;
  }
  INT_DISABLE;
  txbuf.InBufsize-= num_bytes;
  INT_RESTORE;
}
