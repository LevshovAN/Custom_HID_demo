/*******************************************************************************
 * uart.cpp	����� ������ � ���������� ������ UART.
 *   �������������� ������ �����.
 *   ������ ������������� �� ������ UART0 ��� UART1 �� ����� ���������� ���������.
 *   �� ���������� ����������.
 *   ����� ������������ ��������������� �������� ��� �������,
 *    ��� ������ ������ �������� �� ���� ������ UART.
 *   �������������� ������ ������� ������ � ������ ����� �������� �������.
 * 
/-----------------------------------------------------------------------------*/

#include "uart.h"
//#include "main.h"

  DWORD ErrCounter;//������� ������ �����
//  BYTE Rx_buf[50];//����� �����
//  BYTE Rx_Ind;
  struct {
    BYTE buf[FIFO_BUF_MASK+1];
    WORD head;
    WORD InBufsize;
  } txbuf;  
	
  volatile USART_TypeDef * U;

// ������������� UART
bool uart_Init(BYTE num, WORD baudrate_div, BYTE ier, DWORD UartHandler){


  ErrCounter = 0;
  switch(num){
    case 1:
      U = USART1;
//      PCONP_bit.PCUART0 = 1; //��������� ������� 
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
  U->CR2 = 0; //1 ���� ���
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

// �������� � UART �����
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

// �������� � UART ������
void uart_SendStrr(char *str){
  while(*str)
    uart_SendByte(*str++);
}

// �������� � UART ���� � ����������������� �������������
void uart_SendHex(BYTE byte){
  BYTE hex[2];
  btohex(byte, hex);
  uart_SendByte(hex[1]);
  uart_SendByte(hex[0]);
}

// �������� � UART  4-�������� ����� � ���������� �������������
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


// ������ ����� � �����
void uart_BufByte(BYTE byte){
  uart_TXBufWrite(byte);
}

// ������ ������ � �����
void uart_BufStrr(char *str){
  while(*str)
    uart_TXBufWrite(*str++);
}

// ������ � ����� ���� � ����������������� �������������
void uart_BufHex(BYTE byte){
  BYTE hex[2];
  btohex(byte, hex);
  uart_TXBufWrite(hex[1]);
  uart_TXBufWrite(hex[0]);
}

// ������ � ����� ���� � �������� �������������
void uart_BufBin(DWORD d, BYTE num_bit){
  BYTE cr;
  for(DWORD i = num_bit-1; i<32; i--){
    cr ='0';
    if(d&(1<<i)) cr='1';
    uart_TXBufWrite(cr);
  }
}

// ������ � ����� ���� � 10 �������������
void uart_BufBool(BYTE byte){
  BYTE cr ='0';
  if(byte) cr = '1';
  uart_TXBufWrite(cr);
}

// ������ � ����� ��������� ���� � ���������� �������������
void uart_BufDec_SignByte(BYTE byte){
  BYTE NumDig,dec[5];
  NumDig = Signed_byte_to_dec(byte, dec);
  
  for(BYTE i=0; i<NumDig; i++)
    uart_TXBufWrite(dec[i]);
}

// ������ � ����� ����� � ���������� �������������
void Cuart_BufDec2(WORD word){
  BYTE NumDig,dec[6];
  NumDig = wtodec(word, dec);
  
  for(BYTE i=0; i<NumDig; i++)
    uart_TXBufWrite(dec[i]);
}

// ������ � ����� 4-�������� ����� � ���������� �������������
void uart_BufDec(DWORD dword){
  BYTE NumDig, dec[12];
  NumDig = dwtodec(dword, dec);
  for(BYTE i=0; i<NumDig; i++)
    uart_TXBufWrite(dec[i]);
}

// ������ � ����� ����� � ���������� ������������� � ������
void uart_BufDecDot(WORD word,BYTE dot){
  BYTE NumDig,dec[15];
  NumDig = wtodec_dot(word, dec,dot);
  
  for(BYTE i=0; i<NumDig; i++)
    uart_TXBufWrite(dec[i]);
}
// �������� ���� �� ������ � UART
void uart_Send(void){
  if (!(U->SR&USART_SR_TXE)) return;
  BYTE byte;
  if(uart_TXBufRead(&byte)) U->DR = byte;
}

//����� ����� �� ������
//�������: 1-��� ����� �����
BYTE uart_ReceiveByte(BYTE *byte){
  if(U->SR&USART_SR_RXNE){//���� ���� ����������� ��� - ���������
    *byte = U->DR;
    return 1; 
  }
  return 0;
}

/*
//--------------------------------------------------------------------------
//! ���������� ���������� UART 
//--------------------------------------------------------------------------
void CUart::Handler(void){
  union{
    BYTE LSR;
    __uartlsr_bits LSR_bit;
  } StLsr; //��� �������� LSR
  union{
    DWORD IIR;
    __uartfcriir_bits IIR_bit;
  }IntSrc; //��� �������� IIR
  BYTE tmp_data; 
  
  IntSrc.IIR = U->IIR;//��������� IIR, ������ ������� ����������
  
  //���������� �� ��������� ����������
  switch (IntSrc.IIR_bit.IID){
  case RX_LINE_STATUS:
  case CHAR_TIMEOUT:
  case RX_DATA:
	StLsr.LSR = U->LSR;      //������ �������
    while(StLsr.LSR_bit.DR){ //���� ���� ������ 
      tmp_data = U->RBR;     //��������� ������
      if((StLsr.LSR_bit.PE)||(StLsr.LSR_bit.FE)||(StLsr.LSR_bit.BI)){ //���� ���� ������ PE,FE,BI ���������
        ErrCounter++;
        //����������� ���������� � ����� ������ ??
      }else{                                                          //���� ��� ������ - �������� � �����
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
      StLsr.LSR = U->LSR;      //������ ������� ���������� �����
    }
	break;
    
  default: 
	StLsr.LSR = U->LSR;
	break;
  }
}
*/

//--------------------------------------------------------------------------
//! ���������� ������� � ����� ��������
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
//! ������ ������� �� ������ ��������
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
//! �������� ����� �������� �� ������ ��������
//--------------------------------------------------------------------------
void uart_SendBurst(){
//  if (!U->LSR_bit.THRE) return; //���� ���������� ���� ����������
  WORD num_bytes = txbuf.InBufsize;
  if(num_bytes>12) num_bytes = 12;
  if(num_bytes==0) return;  //���� ���� ������ ��� ������ - ����������
  INT_PROTECT;
  INT_DISABLE;
  WORD  tail = (txbuf.head-txbuf.InBufsize)&FIFO_BUF_MASK;
  INT_RESTORE;
  for(WORD i = 0; i<num_bytes; i++){  //���������� �� 12 ����
    U->DR = txbuf.buf[tail];
    tail = (tail+1)&FIFO_BUF_MASK;
  }
  INT_DISABLE;
  txbuf.InBufsize-= num_bytes;
  INT_RESTORE;
}
