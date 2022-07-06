#include "exti.h"
#include "nvic.h"
#include "led.h"
#include "sys.h" 
#include "delay.h"

//Z相归零
void EXTI_PA1_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;

	/* config the extiline(PA1) clock and AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA| RCC_APB2Periph_AFIO,ENABLE);
	/* config the NVIC(PA1) */
	NVIC_Config(1);

	/* EXTI line gpio config(PA1) */	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;       
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 // 上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* EXTI line(PA1) mode config */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource1); 
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿中断
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 
}


