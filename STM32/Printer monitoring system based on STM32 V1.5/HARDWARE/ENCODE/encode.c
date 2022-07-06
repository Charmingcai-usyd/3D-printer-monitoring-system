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

#define Readmemory   1          // 0   重新上电采用读取的坐标轴坐标 1   重新上电采用监视坐标  2 擦写新坐标轴
#define SIZE sizeof(TEXT_Buffer)
static float Speed_1=0,Speed_2=0,Speed_3=0;
uint32_t temp_speed;
uint32_t hValue_1,hValue_2,hValue_3;
static signed long Count_roll_1=0,Count_roll_2=0,Count_roll_3=0;//32位有符号整型
static int   before_mm_x,before_mm_y,before_mm_z;//上电基准坐标



//要写入到24c02的字符串数组
u8 TEXT_Buffer[8]={0,0,0,0,0,0,0,0};
void data_init(void);  //掉电前数据存储

void TIM2_Mode_Config(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//使能TIM2时钟	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能GPIOA时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 ;//将GPIO_InitStruct中的参数按缺省值输入
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//PA0 PA1浮空输入
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 0; //
    TIM_TimeBaseStructure.TIM_Period = (ENCODER_PPR*4)-1;  // number of pulses per revolution*4;  //设定计数器重装值   TIMx_ARR = 600*4
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//TIM2时钟预分频值
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM向上计数 
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
    TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//使用编码器模式3，上升下降都计数
    TIM_ICStructInit(&TIM_ICInitStructure);//将结构体中的内容缺省输入
    TIM_ICInitStructure.TIM_ICFilter = 6;  //选择输入比较滤波器 
    TIM_ICInit(TIM2, &TIM_ICInitStructure);//将TIM_ICInitStructure中的指定参数初始化TIM5
 
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);//清除TIM2的更新标志位
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);//运行更新中断

    TIM_SetCounter(TIM2,0);
    TIM_Cmd(TIM2, ENABLE); //启动TIM5定时器

}



void TIM3_Mode_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;   	

	//PA6 ch1 A,PA7 ch2 B
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//使能TIM3时钟	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能GPIOA时钟
	
	GPIO_StructInit(&GPIO_InitStructure);//将GPIO_InitStruct中的参数按缺省值输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//PA6 PA7浮空输入	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);                           


	TIM_DeInit(TIM3);
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Period = (ENCODER_PPR*4)-1;         // number of pulses per revolution*4;  //设定计数器重装值   TIMx_ARR = 600*4
	TIM_TimeBaseStructure.TIM_Prescaler = 0; //TIM3时钟预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision =TIM_CKD_DIV1 ;//设置时钟分割 T_dts = T_ck_int	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数 
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);              
                 
	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising ,TIM_ICPolarity_Rising);//使用编码器模式3，上升下降都计数
	TIM_ICStructInit(&TIM_ICInitStructure);//将结构体中的内容缺省输入
	TIM_ICInitStructure.TIM_ICFilter = 6;  //选择输入比较滤波器 
	TIM_ICInit(TIM3, &TIM_ICInitStructure);//将TIM_ICInitStructure中的指定参数初始化TIM3
	
//	TIM_ARRPreloadConfig(TIM3, ENABLE);//使能预装载
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);//清除TIM3的更新标志位
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);//运行更新中断
	//Reset counter
	TIM3->CNT = 0;//

	TIM_Cmd(TIM3, ENABLE);   //启动TIM3定时器
}



void TIM4_Mode_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;   	

	//PB6 ch1  A,PB7 ch2 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//使能TIM4时钟	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//使能GPIOB时钟
	
	GPIO_StructInit(&GPIO_InitStructure);//将GPIO_InitStruct中的参数按缺省值输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//PB6 PB7浮空输入	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);                           
    
	/* Enable the TIM4 Interrupt */
	NVIC_Config(2);

	TIM_DeInit(TIM4);
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Period = (ENCODER_PPR*4)-1;         // number of pulses per revolution*4;  //设定计数器重装值   TIMx_ARR = 600*4
	TIM_TimeBaseStructure.TIM_Prescaler = 0; //TIM4时钟预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision =TIM_CKD_DIV1 ;//设置时钟分割 T_dts = T_ck_int	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数 
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);              
                 
	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising ,TIM_ICPolarity_Rising);//使用编码器模式3，上升下降都计数
	TIM_ICStructInit(&TIM_ICInitStructure);//将结构体中的内容缺省输入
	TIM_ICInitStructure.TIM_ICFilter = 6;  //选择输入比较滤波器 
	TIM_ICInit(TIM4, &TIM_ICInitStructure);//将TIM_ICInitStructure中的指定参数初始化TIM4
	
//	TIM_ARRPreloadConfig(TIM4, ENABLE);//使能预装载
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);//清除TIM4的更新标志位
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);//运行更新中断
	//Reset counter
	TIM4->CNT = 0;//
	TIM_Cmd(TIM4, ENABLE);   //启动TIM4定时器
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
	delay_us(500);//1S定时
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
		 before_timer1 = temp_timer1;//赋值
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
		 before_timer2 = temp_timer2;//赋值
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
		 before_timer3 = temp_timer3;//赋值
		 Speed_3 = count/120+(count%120*0.1);			 
		 
 	}
	
	if(i>=150)
	{		 
		  LED =~LED;
			i=0;
			/*******************************   刷 新 信 息 --ESP8266  **********************************/	
/**帧格式：sscanf(message,"%f;%f;%f;%f;%f;%f;%d;%d;%d;%d;%d;%d,"
														&Distance1,&Speed1
														&Distance2,&Speed2
														&Distance3,&Speed3
		                        ,&Position[0],&Position[1]
		                        ,&Position[2],&Position[3]  
                            ,&Temperature[0],&Temperature[1]);
                            1为负数 0为正数
*/	
	    DEBUG_USARTx = USART2;//输出为串口2
		
		  mm_temp_1 = before_mm_x + (Count_roll_1*360/2048)*transfer_degree_mm;
		  //通过当前转动圈数计算皮带移动距离
		  printf("%d;",mm_temp_1);//输出移动距离，格式为整数
			Spped_temp = Speed_1;
      Spped_temp=Spped_temp*60/1000;
      temp_speed=Spped_temp;
      hValue_1=temp_speed/10;
      printf("%d.%d;",hValue_1,temp_speed%10);//计算当前速度 单位为r/s			

		  
		  mm_temp_2 = before_mm_y + (Count_roll_2*360/2048)*transfer_degree_mm;
			//通过当前角度与转动圈数计算皮带移动距离
		  printf("%d;",mm_temp_2);//输出移动距离，格式为整数	
			Spped_temp = Speed_2;
      Spped_temp=Spped_temp*60/1000;
      temp_speed=Spped_temp;
      hValue_2=temp_speed/10; 
      printf("%d.%d;",hValue_2,temp_speed%10);//计算当前速度 单位为r/s			
		  
		
      mm_temp_3 = before_mm_z + (Count_roll_3*360/2048)*transfer_degree_mm;
			//通过当前角度与转动圈数计算皮带移动距离
		  printf("%d;",mm_temp_3);//输出移动距离，格式为整数	
			Spped_temp = Speed_3;
      Spped_temp=Spped_temp*60/1000;
      temp_speed=Spped_temp;		
      hValue_3=temp_speed/10;
      printf("%d.%d;",hValue_3,temp_speed%10);		

/******************   刷新当前坐标与温度    ******************/
			
			printf("%d;%d;%d;%d;%d;%d,",Position[0],Position[1],Position[2],Position[3]
			                           ,Temperature[0],Temperature[1]);
					
/*****************************************   刷 新 信 息 --串口屏 ***********************************/
	    DEBUG_USARTx = USART1;//输出为串口1

			printf("data.va3.val=%d",mm_temp_1);			  HMI_end();
			printf("data.va4.val=%d",mm_temp_2);				HMI_end();
			printf("data.va5.val=%d",mm_temp_3);			  HMI_end();    //输出行进路程		
			
			printf("data.va6.val=%d",hValue_1);			    HMI_end();
			printf("data.va7.val=%d",hValue_2);				  HMI_end();
			printf("data.va8.val=%d",hValue_3);    			HMI_end();    //输出速度			

      printf("data.va10.val=1");				          HMI_end();  
      printf("data.va11.val=1");				          HMI_end();  
      printf("data.va12.val=1");				          HMI_end();      	
      printf("data.va13.val=2");				          HMI_end();    //正在工作
  
/*****************************************   刷 新 信 息 -- EEPROM ***********************************/	
		  
			TEXT_Buffer[0]=mm_temp_1;
			TEXT_Buffer[1]=mm_temp_2;
			TEXT_Buffer[2]=mm_temp_3;
			TEXT_Buffer[3]=Position[0];
			TEXT_Buffer[4]=Position[1];
			TEXT_Buffer[5]=Position[2];
			TEXT_Buffer[6]=Position[3];
			AT24CXX_Write(0,(u8*)TEXT_Buffer,SIZE);


/*****************************************   刷 新 上位机通信 --  G M指令 ****************************/	

	    DEBUG_USARTx = USART3;//输出为串口3
      if(flag==0)
			{
					printf("G92\r\n");//获取坐标
					delay_ms(50);
				  flag=1;
			}
			else 
			{
					printf("M105\r\n");//获取温度
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
  TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //清除TIMx更新中断标志 
}
//****************************************************************************************************
//TIm3_IRQ
//****************************************************************************************************
void TIM3_IRQHandler(void)
{
	  /* Clear the interrupt pending flag */
  TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx更新中断标志 
	
}
//****************************************************************************************************
//TIm4_IRQ
//****************************************************************************************************
void TIM4_IRQHandler(void)
{
	  /* Clear the interrupt pending flag */
  TIM_ClearITPendingBit(TIM4, TIM_IT_Update);  //清除TIMx更新中断标志 
}

void data_init(void)
{	
	u8 datatemp[8];
  if(Readmemory==1)
	{
	  //监视坐标轴
	  AT24CXX_Read(0,datatemp,8); 
	  before_mm_x  = datatemp[0];	
	  before_mm_y  = datatemp[1];	
	  before_mm_z  = datatemp[2];	
	}
  else if(Readmemory==0)
	{
		//标准坐标轴
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
	  AT24CXX_Write(0,(u8*)TEXT_Buffer,SIZE);//写数据
		
	  AT24CXX_Read(0,datatemp,8); 
	  before_mm_x  = datatemp[0];	
	  before_mm_y  = datatemp[1];	
	  before_mm_z  = datatemp[2];//读数据
	}
  DEBUG_USARTx = USART1;//输出为串口1
	printf("x=%d y=%d z=%d",before_mm_x,before_mm_y,before_mm_z);  
}


