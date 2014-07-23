#include"bsp.h"
#include"delay.h"
#include"printer.h"
#include"test_case.h"

#define STEP_MAX 4
PRT_STM prt_stm;
char code step[8]={0,0,0,1,1,1,1,0};	//步进电机状态选择数组//								 


/***************************初始化函数，状态机、步进电机****************************/


void PRT_STM_init (void)
{
	prt_stm.no_paper=1;		  //无纸//
	prt_stm.over_hot=0;		  //过热//
	prt_stm.step_count=0;
	prt_stm.thermal_head_count=0;
	prt_stm.data_ready=0;
	prt_stm.time0_count=0;
}



/*******************************************************************************************/
/******************************有关发送*****************************************************/
void PRT_DotLine(char *str, int length)
{
		int i,j;
		char temp;
	
	
		for(i=0;i<length;i++)
		{
			temp = *str;
			for(j=0;j<8;j++)
			{
				if(temp & 0x80)
					DI=1;
				else
					DI=0;
				temp <<= 1;
	
				CLK=1;
				delay(1);
				CLK=0;
				delay(1);				
			}
			str++;
		}
	
	

}

void PRT_dry_run(int line_num)
{
	int i;
	for(i=0;i<line_num*2;i++)
	{
		STEP_A=step[prt_stm.step_count*2+1];	 //步进电机控制//
		STEP_B=step[prt_stm.step_count*2];
		prt_stm.step_count++;
		if(prt_stm.step_count >= STEP_MAX)
			prt_stm.step_count=0;
		delay(1);
	}
}




/*******************************************************************************************************/


/**********************中断函数，开启、处理、关闭*************************************************/



void timer0_start(void)		//中断开启函数//
{
	TMOD=0x01;   //设置为定时器0,工作方式1
	TH0=(65536-1200)/256;   //装入初此值(整数高8位的)
	TL0=(65536-1200)%256;   //装入初此值(余数低8位的）

	EA=1;                   //开总中断
	ET0=1;                  //定时器0中断
	TR0=1;                  //启动定时器


	STEP_A=step[prt_stm.step_count*2+1];   //步进电机A相//
	STEP_B=step[prt_stm.step_count*2];	  //步进电机B相//
	STEP_E=1;							  //步进电机使能//

	prt_stm.step_count++;

}


void time0() interrupt 1
{
#ifdef TEST_CASE_TIME0
	TH0=(65536-1500)/256;  //装入初此值(整数高8位的)
	TL0=(65536-1500)%256;  //装入初此值(余数低8位的）
	DI=~DI;
	ET0=1;
#else


	ET0=0;	//关中断//
	TH0=(65536-1500)/256;  //装入初始值(整数高8位的)
	TL0=(65536-1500)%256;  //装入初始值(余数低8位的）

	prt_stm.time0_count++;
	if(prt_stm.time0_count == 1)
	{
		//判断是否有dotline数据
		if(prt_stm.data_ready == 0 || NO_PAPER == 1)
		{
			prt_stm.time0_count=0;
			ET0=1;
			STB2=0;	   
	   		STB1=0;
			return;	
		}
		prt_stm.data_ready = 0;
		//加热
		STB2=0;	   
	   	STB1=1;
		//步进
		STEP_A=step[prt_stm.step_count*2+1];	 //步进电机控制//
		STEP_B=step[prt_stm.step_count*2];
		prt_stm.step_count++;
	}
	else
	if(prt_stm.time0_count == 2) 
	{
		STB1=0;
		STB2=1;
	}
	else
	if(prt_stm.time0_count == 3) 
	{
		STB2=0;
		STB1=1;
		STEP_A=step[prt_stm.step_count*2+1];	 //步进电机控制//
		STEP_B=step[prt_stm.step_count*2];
		prt_stm.step_count++;

		if(prt_stm.step_count >= STEP_MAX)
			prt_stm.step_count=0;			
	}
	else
	if(prt_stm.time0_count == 4) 
	{
		STB1=0;
		STB2=1;
		prt_stm.time0_count=0;
	}
	ET0=1;
#endif

}


