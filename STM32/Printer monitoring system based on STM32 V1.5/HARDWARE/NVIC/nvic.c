#include "stm32f10x.h"
#include "misc.h"
#include "nvic.h"
#include "sys.h" 
#include "delay.h"
/*
�жϷ���˵�������ȼ��ɸ������
˳��       ����            ����                   ��ռ���ȼ�      ��Ӧ���ȼ�
 1         TIM2         ��ʱ����ʱ                    0                 0
 2         TIM134       ��ʱ���ı�����ģʽ            1                 1
 3         USART2       ESP8266����ͨ���ж�           2                 1
 4         USART3       ��λ������ͨ���ж�            3                 1
*/

//1----��ʱ����ʱ
void TIM1_NVIC_Config(NVIC_InitTypeDef NVIC_InitStructure1)
{
	/* Enable the TIM2 Interrupt  */
	NVIC_InitStructure1.NVIC_IRQChannel = TIM1_UP_IRQn;
	NVIC_InitStructure1.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure1.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure1.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure1);		

} 
//2----��ʱ���ı������ӿ�ģʽ,�Ա��������м���
void TIM4_NVIC_Config(NVIC_InitTypeDef NVIC_InitStructure2)
{
	NVIC_InitStructure2.NVIC_IRQChannel = TIM4_IRQn; 
	NVIC_InitStructure2.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure2.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure2.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure2);

}

//3----����2ͨ�ţ�����ESP8266��������͸��������3
void USART2_NVIC_Config(NVIC_InitTypeDef NVIC_InitStructure3)
{
	NVIC_InitStructure3.NVIC_IRQChannel = USART2_IRQn; 
	NVIC_InitStructure3.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure3.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure3.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure3);
}

//4----����3ͨ�ţ�������λ����������
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
	/* �����ж�ʹ�����  ��ռʽ3λ(0-7)����Ӧʽ1λ(0-1) */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	if(Interrupt_flag == 1)
	{
		/*TIM2�жϣ�����       ----���ȼ� 0  0*/
		TIM1_NVIC_Config(NVIC_InitStructure);
	}
	if(Interrupt_flag == 2)
	{	
		/*TIM4�жϣ����������� ----���ȼ� 1  1*/
		TIM4_NVIC_Config(NVIC_InitStructure);
	}
	if(Interrupt_flag == 3)
	{
		/* USART2�жϣ����ݽ���----���ȼ� 2  1*/
		USART2_NVIC_Config(NVIC_InitStructure);
	}
	if(Interrupt_flag == 4)
	{
		/* USART3�жϣ����ݽ���----���ȼ� 3  1*/
		USART3_NVIC_Config(NVIC_InitStructure);
	}
}


