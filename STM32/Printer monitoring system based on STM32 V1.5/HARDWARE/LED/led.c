#include "led.h"
#include "delay.h"  

/********************************
   
	 函数：LED_Init(void)
	 功能：LED初始化
   输入：
	 输出：dat
	 备注LED初始化             

********************************/
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PA端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 //LED-->PA.12 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.12
 GPIO_SetBits(GPIOA,GPIO_Pin_12);						 //PA.12 输出高

 LED = 0;
 delay_ms(1000); 	 
 LED = 1;

}
 
