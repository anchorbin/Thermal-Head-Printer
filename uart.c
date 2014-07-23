#include<reg52.h>
#include"uart.h"


unsigned  char xdata rx_buf[64];							  
unsigned  int rx_len;
unsigned  char xdata tx_buf[64];
unsigned  char tx_count,tx_size;							  //���ͱ���


void UART_init(void)
{
	TMOD=(TMOD&0x0f)+0x20;    
	TH1=0xfa;
	TL1=0xfa;       //���ò�����Ϊ9600
	PCON=0x00;      //�������޼ӱ�
	SCON=0x50;		//���ô���ͨѶ��ʽ2
	TR1=1; 
	ES=1;
	EA=1;
	rx_len=0;
	tx_count=0;
	tx_size=0;

}


//���ȷ��͵�һ���ֽڣ�Ȼ�󽻸��жϷ��������ֽ�
void UART_send(unsigned char xdata *buf ,int len)				 
{	 
     int i;

	 for(i=0;i<len;i++)
		tx_buf[i]=buf[i];
	 tx_size=len;
	 tx_count=1;
     SBUF=buf[0];

}

int UART_RecvCnt(void)
{
	return rx_len;

}

void UART_receive(unsigned char xdata *buf ,int len)				 
{
	int i;
	for(i=0;i<len;i++)
	{
		*buf=rx_buf[i];
		buf++;
	}	
}
void URAT_break(void) interrupt 4    									   //�����ж�
{
	if(RI)
	{
	    RI=0;
		if(rx_len<256)
		{
		  	rx_buf[rx_len]=SBUF;
			rx_len++;	   	
		}
	}
	if(TI)
	{	 
	     TI=0;
	     if( tx_count<tx_size)
		 {
		 	  SBUF=tx_buf[tx_count++];
		 }
		 else
		 {
		 	  tx_size=tx_count=0;
		 }
	}	    
}

void UART_clear(void)
{
	rx_len=0;	
}