#ifndef __uart_h__
#define __uart_h__

void UART_init(void);
void UART_send(unsigned char xdata *buf ,int len);				 
int UART_RecvCnt(void);
void UART_receive(unsigned char xdata *buf ,int len);				 
void UART_clear(void);

#endif
