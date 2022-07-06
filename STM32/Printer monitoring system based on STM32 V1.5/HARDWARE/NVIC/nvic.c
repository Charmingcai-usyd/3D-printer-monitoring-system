#include "stm32f10x.h"
#include "misc.h"
#include "nvic.h"
#include "sys.h" 
#include "delay.h"
/*
中断分配说明，优先级由高向低排
顺序       类型            作用                   抢占优先级      响应优先级
 1         TIM2         定时器计时                    0                 0
 2         TIM134       定时器的编码器模式            1                 1
 3         USART2       ESP8266串口通信中断           2                 1
 4         USART3       下位机串口通信中断            3                 1
*/

//1----定时器计时
void TIM1_NVIC_Config(NVIC_InitTypeDef NVIC_InitStructure1)
{
	/* Enable the TIM2 Interrupt  */
	NVIC_InitStructure1.NVIC_IRQChannel = TIM1_UP_IRQn;
	NVIC_InitStructure1.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure1.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure1.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure1);		

} 
//2----定时器的编码器接口模式,对编码器进行计数
void TIM4_NVIC_Config(NVIC_InitTypeDef NVIC_InitStructure2)
{
	NVIC_InitStructure2.NVIC_IRQChannel = TIM4_IRQn; 
	NVIC_InitStructure2.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure2.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure2.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure2);

}

//3----串口2通信，用于ESP8266串口数据透传至串口3
void USART2_NVIC_Config(NVIC_InitTypeDef NVIC_InitStructure3)
{
	NVIC_InitStructure3.NVIC_IRQChannel = USART2_IRQn; 
	NVIC_InitStructure3.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure3.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure3.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure3);
}

//4----串口3通信，用于下位机接收数据
void USART3_NVIC_Config(NVIC_InitTypeDef NVIC_InitStructure4)
{
	NVIC_InitStructure4.NVIC_IRQChannel = USART3_IRQn; 
	NVIC_InitStructure4.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure4.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure4.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure4);
}

void NVIC_Config(u8 Interrupt_flag)
{

  NVIC_InitTypeDef   NVIC_InitStructure;
	/* 配置中断使用组合  抢占式3位(0-7)，响应式1位(0-1) */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	if(Interrupt_flag == 1)
	{
		/*TIM2中断，计算       ----优先级 0  0*/
		TIM1_NVIC_Config(NVIC_InitStructure);
	}
	if(Interrupt_flag == 2)
	{	
		/*TIM4中断，编码器解码 ----优先级 1  1*/
		TIM4_NVIC_Config(NVIC_InitStructure);
	}
	if(Interrupt_flag == 3)
	{
		/* USART2中断，数据接收----优先级 2  1*/
		USART2_NVIC_Config(NVIC_InitStructure);
	}
	if(Interrupt_flag == 4)
	{
		/* USART3中断，数据接收----优先级 3  1*/
		USART3_NVIC_Config(NVIC_InitStructure);
	}
}


