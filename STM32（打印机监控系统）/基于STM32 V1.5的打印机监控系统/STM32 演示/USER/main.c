#include "delay.h"    //��ʱͷ�ļ�
#include "sys.h"      //�⺯��ͷ�ļ�
#include "usart.h"    //����ͷ�ļ�
#include "led.h"      //LEDͷ�ļ�
#include "24cxx.h"	  //EEPROMͷ�ļ�
#include "stdlib.h"   //�����׼�������ͷ�ļ�
#include "stdio.h"    //��׼C��
#include "encode.h"   //������ͷ�ļ�

/*
                                      ��־
					����													 ��ע                                            �汾��
         2021.3.4                    ���EEPROM ��ֲ ����δ�������                       V1.0
				 2021.3.6                    ���봮������ʾ����                                   V1.1.0
				 2021.3.10                   �Խӵ����߲���                                       V1.1.1
				 2021.3.11                   �ԽӴ�ӡ�����ư� �ܽ�������ɸѡ                      V1.2
				 2021.3.15                   ��ɱ�����������������ģ�������ϴ�                   V1.3
				 2021.4.2                    �������Ƶ�һ��                                       V1.4
				 2021.5.2                    ���յ��Զ���                                         V1.5
*/


 int main(void)
 {		
	
	delay_init();	    	 //��ʱ������ʼ��	
	uart1_init(9600);	   //���ڳ�ʼ��Ϊ115200
	uart2_init(115200);	 //���ڳ�ʼ��Ϊ115200
	uart3_init(115200);	 //���ڳ�ʼ��Ϊ115200
	 
  DEBUG_USARTx = USART1;//���Ϊ����3
	printf("data.va13.val=0");				     HMI_end();   //None
	
	LED_Init();
	//vl53l0x_init(4);      //��ʼ��2�����⴫���� ʣ�µĽ����������ų�ʼ������ɳ�ʼ������
  AT24CXX_Init();			//IIC��ʼ�� 
	 
 	while(AT24CXX_Check())//��ⲻ��24c02
	{
		delay_ms(1000);
		LED=!LED;//DS0��˸
	}	
	
	LED=!LED;	
  data_init();          //���ݳ�ʼ��   
	DEBUG_USARTx = USART1;//���Ϊ����1
	printf("USART1 Init OK!");
	DEBUG_USARTx = USART2;//���Ϊ����2
	printf("USART2 Init OK!");
	DEBUG_USARTx = USART3;//���Ϊ����3
	printf("USART3 Init OK!");  
  delay_ms(1000);	 
	LED=!LED;
	
	TIM_Init();          //��ʱ���������ӿڳ�ʼ��	 
	
	delay_ms(3000);	 
	DEBUG_USARTx = USART1;//���Ϊ����1
	printf("data.va13.val=1");				     HMI_end();   //��ʼ���ɹ�
	delay_ms(3000);	 
 	while(1)
  {
     scan();//ˢ��������
	}
 }



