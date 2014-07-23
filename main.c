#include"bsp.h"
#include"delay.h"
#include"printer.h"
#include"test_case.h"
#include"uart.h"
#include"compute.h"
#include"LED.h"

#define PRT_DOTLINE 1
#define PRT_DRYRUN 2

#ifdef TEST_CASE_COMMAND
	const unsigned char case_buff[]={86,57,32,42,1,5,96,221};
	unsigned char c1,c2;
#endif

#ifdef TEST_CASE_COMAGR
	const unsigned char case_buff[]={0x55,0x55,6,0,86,57,32,42,1,5,97,223};
	unsigned char xdata rx_data_buff[20];
#endif


int Data_Gather(unsigned char xdata *buff);


void main()
{
	short int cmd;
	int line_num=4;
	unsigned char xdata buff[40]={};
	int checkflag=1;

	port_init();		//端口的初始化//
	PRT_STM_init();		//状态机的初始化//
	UART_init();


//time0中断时间精度测试

#ifdef TEST_CASE_TIME0		
	timer0_start();
	while(1);
#endif



//发送一行dotline测例（仅32个dot）
#ifdef TEST_CASE_DOTLINE
	buff[0]=0x55;
	buff[1]=0x00;
	buff[2]=0xff;
	buff[3]=0x55;
	while(1)
	{
		PRT_DotLine(buff,48);
		delay(40);
	}

#endif

//MOTOR测试
#ifdef TEST_CASE_MOTOR
	timer0_start();
	
	while(1)
	{
		prt_stm.data_ready=1;
		delay(500);
	}

#endif


//快进测试
#ifdef TEST_CASE_FF
	while(1)
	{
		PRT_dry_run(10);
		delay(500);
	}
#endif

//校验测试
#ifdef TEST_CASE_COMMAND
	checkout ((unsigned char*)case_buff,6,&c1,&c2);
	while(1)
	{
		DI=0;
		delay(10);
	
		if(c1 == 97)
		{
			DI=1;
			delay(2);
			DI=0;
			delay(2);
		}
		if(c2 == 223)
		{
			DI=1;
			delay(4);
			DI=0;
			delay(4);	
		}
	
	}
#endif
  
//#ifdef TEST_CASE_COMAGR
			
//#endif

	timer0_start();			 //启动中断0

	while(1)
	{			
		if(Data_Gather(buff) == 0 )
			continue;

		cmd = buff[5];
		cmd <<= 8;
		cmd += buff[4];
#ifdef TEST_CASE_GATHER
		int test_led_cnt;
		if(cmd == PRT_DOTLINE)
			test_led_cnt=2;
		else
		if(cmd == PRT_DRYRUN)
			test_led_cnt=3;
		else
			test_led_cnt=1;	
		while(1)
		{
			test_led(com_flag);	
		}
#endif			
		if(cmd == PRT_DOTLINE)
		{
			PRT_DotLine(&buff[6],48);
		}
		
		else 
		if(cmd == PRT_DRYRUN && NO_PAPER == 0)
		{
			PRT_dry_run(line_num);
		}
		else
		{
				
		}
	}

}


int Data_Gather(unsigned char xdata *buff)
{
	unsigned char xdata temp[4];
	short int head,length;
	unsigned char c1,c2;
	int rx_count;

	rx_count=UART_RecvCnt();
	if(rx_count < 6)
		return 0;

	UART_receive(temp ,4);
	head=temp[1];								  	
	head <<= 8;
	head += temp[0];
	if(head != 0x5555)
		return 0;	
	length=temp[3];
	length <<= 8;
	length += temp[2];
	if(length > rx_count-2)
		return 0;
			
	UART_receive(buff ,length+2);
	UART_clear();

	checkout (&buff[2], length-2, &c1, &c2);
	if(c1 != buff[length] || c2 != buff[length+1])
	{
#ifdef TEST_CASE_GATHER
			test_led(1);
			while(1);
#endif
		return 0;
	 }
	return 1;
					
}

