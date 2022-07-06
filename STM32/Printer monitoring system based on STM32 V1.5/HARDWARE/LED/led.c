#include "led.h"
#include "delay.h"  

/********************************
   
	 ������LED_Init(void)
	 ���ܣ�LED��ʼ��
   ���룺
	 �����dat
	 ��ע�LLED��ʼ��             

********************************/
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��PA�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 //LED-->PA.12 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.12
 GPIO_SetBits(GPIOA,GPIO_Pin_12);						 //PA.12 �����

 LED = 0;
 delay_ms(1000); 	 
 LED = 1;

}
 
