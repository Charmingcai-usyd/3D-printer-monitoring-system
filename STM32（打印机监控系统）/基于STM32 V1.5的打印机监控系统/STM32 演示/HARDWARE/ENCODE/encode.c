#include "stm32f10x.h"
#include "encode.h"
#include "misc.h"
#include "nvic.h"
#include "usart.h"
#include "sys.h" 
#include "delay.h"
#include "stdio.h"
#include "led.h"
#include "math.h"
#include "encode.h"
#include "24cxx.h"

#define Readmemory   1          // 0   �����ϵ���ö�ȡ������������ 1   �����ϵ���ü�������  2 ��д��������
#define SIZE sizeof(TEXT_Buffer)
static float Speed_1=0,Speed_2=0,Speed_3=0;
uint32_t temp_speed;
uint32_t hValue_1,hValue_2,hValue_3;
static signed long Count_roll_1=0,Count_roll_2=0,Count_roll_3=0;//32λ�з�������
static int   before_mm_x,before_mm_y,before_mm_z;//�ϵ��׼����



//Ҫд�뵽24c02���ַ�������
u8 TEXT_Buffer[8]={0,0,0,0,0,0,0,0};
void data_init(void);  //����ǰ���ݴ洢

void TIM2_Mode_Config(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//ʹ��TIM2ʱ��	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 ;//��GPIO_InitStruct�еĲ�����ȱʡֵ����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//PA0 PA1��������
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 0; //
    TIM_TimeBaseStructure.TIM_Period = (ENCODER_PPR*4)-1;  // number of pulses per revolution*4;  //�趨��������װֵ   TIMx_ARR = 600*4
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//TIM2ʱ��Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM���ϼ��� 
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
    TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//ʹ�ñ�����ģʽ3�������½�������
    TIM_ICStructInit(&TIM_ICInitStructure);//���ṹ���е�����ȱʡ����
    TIM_ICInitStructure.TIM_ICFilter = 6;  //ѡ������Ƚ��˲��� 
    TIM_ICInit(TIM2, &TIM_ICInitStructure);//��TIM_ICInitStructure�е�ָ��������ʼ��TIM5
 
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);//���TIM2�ĸ��±�־λ
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);//���и����ж�

    TIM_SetCounter(TIM2,0);
    TIM_Cmd(TIM2, ENABLE); //����TIM5��ʱ��

}



void TIM3_Mode_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;   	

	//PA6 ch1 A,PA7 ch2 B
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//ʹ��TIM3ʱ��	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
	
	GPIO_StructInit(&GPIO_InitStructure);//��GPIO_InitStruct�еĲ�����ȱʡֵ����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//PA6 PA7��������	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);                           


	TIM_DeInit(TIM3);
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Period = (ENCODER_PPR*4)-1;         // number of pulses per revolution*4;  //�趨��������װֵ   TIMx_ARR = 600*4
	TIM_TimeBaseStructure.TIM_Prescaler = 0; //TIM3ʱ��Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision =TIM_CKD_DIV1 ;//����ʱ�ӷָ� T_dts = T_ck_int	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM���ϼ��� 
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);              
                 
	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising ,TIM_ICPolarity_Rising);//ʹ�ñ�����ģʽ3�������½�������
	TIM_ICStructInit(&TIM_ICInitStructure);//���ṹ���е�����ȱʡ����
	TIM_ICInitStructure.TIM_ICFilter = 6;  //ѡ������Ƚ��˲��� 
	TIM_ICInit(TIM3, &TIM_ICInitStructure);//��TIM_ICInitStructure�е�ָ��������ʼ��TIM3
	
//	TIM_ARRPreloadConfig(TIM3, ENABLE);//ʹ��Ԥװ��
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);//���TIM3�ĸ��±�־λ
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);//���и����ж�
	//Reset counter
	TIM3->CNT = 0;//

	TIM_Cmd(TIM3, ENABLE);   //����TIM3��ʱ��
}



void TIM4_Mode_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;   	

	//PB6 ch1  A,PB7 ch2 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//ʹ��TIM4ʱ��	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//ʹ��GPIOBʱ��
	
	GPIO_StructInit(&GPIO_InitStructure);//��GPIO_InitStruct�еĲ�����ȱʡֵ����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//PB6 PB7��������	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);                           
    
	/* Enable the TIM4 Interrupt */
	NVIC_Config(2);

	TIM_DeInit(TIM4);
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Period = (ENCODER_PPR*4)-1;         // number of pulses per revolution*4;  //�趨��������װֵ   TIMx_ARR = 600*4
	TIM_TimeBaseStructure.TIM_Prescaler = 0; //TIM4ʱ��Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision =TIM_CKD_DIV1 ;//����ʱ�ӷָ� T_dts = T_ck_int	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM���ϼ��� 
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);              
                 
	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising ,TIM_ICPolarity_Rising);//ʹ�ñ�����ģʽ3�������½�������
	TIM_ICStructInit(&TIM_ICInitStructure);//���ṹ���е�����ȱʡ����
	TIM_ICInitStructure.TIM_ICFilter = 6;  //ѡ������Ƚ��˲��� 
	TIM_ICInit(TIM4, &TIM_ICInitStructure);//��TIM_ICInitStructure�е�ָ��������ʼ��TIM4
	
//	TIM_ARRPreloadConfig(TIM4, ENABLE);//ʹ��Ԥװ��
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);//���TIM4�ĸ��±�־λ
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);//���и����ж�
	//Reset counter
	TIM4->CNT = 0;//
	TIM_Cmd(TIM4, ENABLE);   //����TIM4��ʱ��
}

void TIM_Init(void)
{
	TIM2_Mode_Config();//PA0 PA1
	TIM3_Mode_Config();//PA6 PA7
  TIM4_Mode_Config();//PB6 PB7
}



void scan(void)
{
	//unsigned int temp;
	static unsigned short i=0;
	uint32_t Spped_temp;
	signed  mm_temp_1,mm_temp_2,mm_temp_3;
	unsigned short temp_timer1,temp_timer2,temp_timer3;
	static unsigned short before_timer1,before_timer2,before_timer3;
	static int flag=0;
	signed int count;
  delay_ms(4);
	delay_us(500);//1S��ʱ
	i++;
	if(i<150)
	{
			
		 temp_timer1 = TIM_GetCounter(ENCODER_TIMER_1);
		 count = fabs(temp_timer1 - before_timer1);
		 if(count<=Threshold_Pluse)
		 {
		     if(temp_timer1>before_timer1) 
				 Count_roll_1=Count_roll_1+count;
				 else  
				 Count_roll_1=Count_roll_1-count;
		 }
		 else
		 {
		     if(temp_timer1>before_timer1) Count_roll_1 = Count_roll_1+(4*ENCODER_PPR-(temp_timer1+before_timer1));
				 else  Count_roll_1 = Count_roll_1-(4*ENCODER_PPR-(temp_timer1+before_timer1));		 
		 }
		 before_timer1 = temp_timer1;//��ֵ
		 Speed_1 = count/120+(count%120*0.1);		
		 
		 
		 
		 
		 

		 temp_timer2 = TIM_GetCounter(ENCODER_TIMER_2);
		 count = fabs(temp_timer2 - before_timer2);
		 if(count<=Threshold_Pluse)
		 {
		     if(temp_timer2>before_timer2) 
				 Count_roll_2=Count_roll_2+count;
				 else  
				 Count_roll_2=Count_roll_2-count;
		 }
		 else
		 {
		     if(temp_timer2>before_timer2) Count_roll_2 = Count_roll_2+(4*ENCODER_PPR-(temp_timer2+before_timer2));
				 else  Count_roll_2 = Count_roll_2-(4*ENCODER_PPR-(temp_timer2+before_timer2));		 
		 }
		 before_timer2 = temp_timer2;//��ֵ
		 Speed_2 = count/120+(count%120*0.1);				 
		 
		 
		 
		 temp_timer3 = TIM_GetCounter(ENCODER_TIMER_3);
		 count = fabs(temp_timer3 - before_timer3);
		 if(count<=Threshold_Pluse)
		 {
		     if(temp_timer3>before_timer3) 
				 Count_roll_3=Count_roll_2+count;
				 else  
				 Count_roll_3=Count_roll_3-count;
		 }
		 else
		 {
		     if(temp_timer3>before_timer3) Count_roll_3 = Count_roll_3+(4*ENCODER_PPR-(temp_timer3+before_timer3));
				 else  Count_roll_3 = Count_roll_3-(4*ENCODER_PPR-(temp_timer3+before_timer3));		 
		 }
		 before_timer3 = temp_timer3;//��ֵ
		 Speed_3 = count/120+(count%120*0.1);			 
		 
 	}
	
	if(i>=150)
	{		 
		  LED =~LED;
			i=0;
			/*******************************   ˢ �� �� Ϣ --ESP8266  **********************************/	
/**֡��ʽ��sscanf(message,"%f;%f;%f;%f;%f;%f;%d;%d;%d;%d;%d;%d,"
														&Distance1,&Speed1
														&Distance2,&Speed2
														&Distance3,&Speed3
		                        ,&Position[0],&Position[1]
		                        ,&Position[2],&Position[3]  
                            ,&Temperature[0],&Temperature[1]);
                            1Ϊ���� 0Ϊ����
*/	
	    DEBUG_USARTx = USART2;//���Ϊ����2
		
		  mm_temp_1 = before_mm_x + (Count_roll_1*360/2048)*transfer_degree_mm;
		  //ͨ����ǰת��Ȧ������Ƥ���ƶ�����
		  printf("%d;",mm_temp_1);//����ƶ����룬��ʽΪ����
			Spped_temp = Speed_1;
      Spped_temp=Spped_temp*60/1000;
      temp_speed=Spped_temp;
      hValue_1=temp_speed/10;
      printf("%d.%d;",hValue_1,temp_speed%10);//���㵱ǰ�ٶ� ��λΪr/s			

		  
		  mm_temp_2 = before_mm_y + (Count_roll_2*360/2048)*transfer_degree_mm;
			//ͨ����ǰ�Ƕ���ת��Ȧ������Ƥ���ƶ�����
		  printf("%d;",mm_temp_2);//����ƶ����룬��ʽΪ����	
			Spped_temp = Speed_2;
      Spped_temp=Spped_temp*60/1000;
      temp_speed=Spped_temp;
      hValue_2=temp_speed/10; 
      printf("%d.%d;",hValue_2,temp_speed%10);//���㵱ǰ�ٶ� ��λΪr/s			
		  
		
      mm_temp_3 = before_mm_z + (Count_roll_3*360/2048)*transfer_degree_mm;
			//ͨ����ǰ�Ƕ���ת��Ȧ������Ƥ���ƶ�����
		  printf("%d;",mm_temp_3);//����ƶ����룬��ʽΪ����	
			Spped_temp = Speed_3;
      Spped_temp=Spped_temp*60/1000;
      temp_speed=Spped_temp;		
      hValue_3=temp_speed/10;
      printf("%d.%d;",hValue_3,temp_speed%10);		

/******************   ˢ�µ�ǰ�������¶�    ******************/
			
			printf("%d;%d;%d;%d;%d;%d,",Position[0],Position[1],Position[2],Position[3]
			                           ,Temperature[0],Temperature[1]);
					
/*****************************************   ˢ �� �� Ϣ --������ ***********************************/
	    DEBUG_USARTx = USART1;//���Ϊ����1

			printf("data.va3.val=%d",mm_temp_1);			  HMI_end();
			printf("data.va4.val=%d",mm_temp_2);				HMI_end();
			printf("data.va5.val=%d",mm_temp_3);			  HMI_end();    //����н�·��		
			
			printf("data.va6.val=%d",hValue_1);			    HMI_end();
			printf("data.va7.val=%d",hValue_2);				  HMI_end();
			printf("data.va8.val=%d",hValue_3);    			HMI_end();    //����ٶ�			

      printf("data.va10.val=1");				          HMI_end();  
      printf("data.va11.val=1");				          HMI_end();  
      printf("data.va12.val=1");				          HMI_end();      	
      printf("data.va13.val=2");				          HMI_end();    //���ڹ���
  
/*****************************************   ˢ �� �� Ϣ -- EEPROM ***********************************/	
		  
			TEXT_Buffer[0]=mm_temp_1;
			TEXT_Buffer[1]=mm_temp_2;
			TEXT_Buffer[2]=mm_temp_3;
			TEXT_Buffer[3]=Position[0];
			TEXT_Buffer[4]=Position[1];
			TEXT_Buffer[5]=Position[2];
			TEXT_Buffer[6]=Position[3];
			AT24CXX_Write(0,(u8*)TEXT_Buffer,SIZE);


/*****************************************   ˢ �� ��λ��ͨ�� --  G Mָ�� ****************************/	

	    DEBUG_USARTx = USART3;//���Ϊ����3
      if(flag==0)
			{
					printf("G92\r\n");//��ȡ����
					delay_ms(50);
				  flag=1;
			}
			else 
			{
					printf("M105\r\n");//��ȡ�¶�
					delay_ms(50);
				  flag=0;
			}
	}

}
//****************************************************************************************************
//TIm2_IRQ
//****************************************************************************************************
void TIM2_IRQHandler(void)
{
	  /* Clear the interrupt pending flag */
  TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //���TIMx�����жϱ�־ 
}
//****************************************************************************************************
//TIm3_IRQ
//****************************************************************************************************
void TIM3_IRQHandler(void)
{
	  /* Clear the interrupt pending flag */
  TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //���TIMx�����жϱ�־ 
	
}
//****************************************************************************************************
//TIm4_IRQ
//****************************************************************************************************
void TIM4_IRQHandler(void)
{
	  /* Clear the interrupt pending flag */
  TIM_ClearITPendingBit(TIM4, TIM_IT_Update);  //���TIMx�����жϱ�־ 
}

void data_init(void)
{	
	u8 datatemp[8];
  if(Readmemory==1)
	{
	  //����������
	  AT24CXX_Read(0,datatemp,8); 
	  before_mm_x  = datatemp[0];	
	  before_mm_y  = datatemp[1];	
	  before_mm_z  = datatemp[2];	
	}
  else if(Readmemory==0)
	{
		//��׼������
		AT24CXX_Read(0,datatemp,8); 
		before_mm_x = datatemp[3];
		before_mm_y = datatemp[4];
		before_mm_z = datatemp[5];		
	}
	else if(Readmemory==2)
	{
		TEXT_Buffer[0]=100;
		TEXT_Buffer[1]=50;
		TEXT_Buffer[2]=25;		
	  AT24CXX_Write(0,(u8*)TEXT_Buffer,SIZE);//д����
		
	  AT24CXX_Read(0,datatemp,8); 
	  before_mm_x  = datatemp[0];	
	  before_mm_y  = datatemp[1];	
	  before_mm_z  = datatemp[2];//������
	}
  DEBUG_USARTx = USART1;//���Ϊ����1
	printf("x=%d y=%d z=%d",before_mm_x,before_mm_y,before_mm_z);  
}


