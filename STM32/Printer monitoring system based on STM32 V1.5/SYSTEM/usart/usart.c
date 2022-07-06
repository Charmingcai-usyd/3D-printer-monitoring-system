#include "sys.h"
#include "usart.h"	  
#include "nvic.h"  
#include "stdio.h"
#include "led.h"
USART_TypeDef * DEBUG_USARTx = USART1;   //定义串口指针
u16 Temperature[2];           //温度数据存档
u16 Position[8]   ;           //位置数据存档   

#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
		USART_SendData(DEBUG_USARTx, (uint8_t) ch);	
		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);		
		return (ch);
}
#endif 
 


/****************************************************

    USART  初始化配置函数与中断函数
           函数                           备注                        功能
    uart1_init(u32 bound)              串口1初始化
    USART1_IRQHandler(void)            串口1中断服务函数            
    uart2_init(u32 bound)              串口2初始化
    USART2_IRQHandler(void)            串口2中断服务函数
    uart3_init(u32 bound)              串口3初始化
    USART3_IRQHandler(void)            串口3中断服务函数

******************************************************/
  
void uart1_init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟

	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9

	//USART1_RX	  GPIOA.10初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

	//USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART1, &USART_InitStructure); //初始化串口1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART1, ENABLE);                    //使能串口1 

}
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART1_RX_BUF[USART1_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART1_RX_STA=0;       //接收状态标记	  

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
		Res =USART_ReceiveData(USART1);	//读取接收到的数据

		if((USART1_RX_STA&0x8000)==0)//接收未完成
			{
			if(USART1_RX_STA&0x4000)//接收到了0x0d 0xff
				{
        if(Res!=0x0a)USART1_RX_STA=0;//接收错误,重新开始
				else USART1_RX_STA|=0x8000;	//接收完成了
				}
			else //还没收到0X0d 0xff
				{	
				if(Res==0x0d)USART1_RX_STA|=0x4000;
				else
					{
					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;
					USART1_RX_STA++;
					if(USART1_RX_STA>(USART1_REC_LEN-1))USART1_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}   		 
     } 
} 

#endif

 
 
void uart2_init(u32 bound)
{
	  GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
    /* Enable the USART2 Pins Software Remapping */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB2Periph_AFIO, ENABLE); 
    
    /* Configure USART2 Rx (PA.03) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* Configure USART2 Tx (PA.02) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
	  //Usart2 NVIC Config
    NVIC_Config(3);   
    
    USART_InitStructure.USART_BaudRate = bound;                
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;     
    USART_InitStructure.USART_Parity = USART_Parity_No;        
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;      
    
    USART_Init(USART2, &USART_InitStructure);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    /* Enable USART2 */
    USART_Cmd(USART2, ENABLE);
}

#if EN_USART2_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART2_RX_BUF[USART2_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART2_RX_STA=0;       //接收状态标记	 
void USART2_IRQHandler(void)                	//串口2中断服务程序
{
	u8 Res;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
		Res =USART_ReceiveData(USART2);	//读取接收到的数据
		uart3_send_char(Res);           //实现usart2 ESP8266 与下位机 发送数据透传    
		/* 实现了ESP8266直接控制下位机的技术  */
		if((USART2_RX_STA&0x8000)==0)//接收未完成
			{
			if(USART2_RX_STA&0x4000)//接收到了0x0d
				{
				if(Res!=0x0a)USART2_RX_STA=0;//接收错误,重新开始
				else USART2_RX_STA|=0x8000;	//接收完成了 
				}
			else //还没收到0X0D
				{	
				if(Res==0x0d)USART2_RX_STA|=0x4000;
				else
					{
					USART2_RX_BUF[USART2_RX_STA&0X3FFF]=Res ;
					USART2_RX_STA++;
					if(USART2_RX_STA>(USART2_REC_LEN-1))USART2_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}   		 
     }  
} 
#endif

 
  
 
void uart3_init(u32 bound)
{
	  GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
    /* Enable the USART3 Pins Software Remapping */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); 
    
    /* Configure USART3 Rx (PB.11) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* Configure USART3 Tx (PB.10) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* Enable the USART3 Interrupt */
    NVIC_Config(4);    
    
    USART_InitStructure.USART_BaudRate = bound;                
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;     
    USART_InitStructure.USART_Parity = USART_Parity_No;        
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;      
    
    USART_Init(USART3, &USART_InitStructure);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    /* Enable USART3 */
    USART_Cmd(USART3, ENABLE);
}
 

#if EN_USART3_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART3_RX_BUF[USART2_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART3_RX_STA=0;       //接收状态标记	 
void USART3_IRQHandler(void)                	//串口1中断服务程序
{
  // "ok" 为字符串结束标志 然后对数据进行上传分析
	u8 Res;
	u16 len,t;//长度
	u8    i,j;
	u8   BUF[5];
	static u8   IND[2]={0,0};
	static u8   k=0;
  unsigned char Count_flag = 0;//绝对坐标标志位
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
		Res =USART_ReceiveData(USART3);	//读取接收到的数据
    if(!k)		
		{	  if(Res == 0x6f)//首先接收的是'o'
				{
				   IND[0] = 0X40;
					 IND[1] = 0X3A;
				}
				else//默认接收的是'X'	
				{
				   IND[0] = 0X6F;
					 IND[1] = 0X6B;				
				}			
		}
		k=1;
		if((USART3_RX_STA&0x8000)==0)//接收未完成
			{
			if(USART3_RX_STA&0x4000)//接收到了
				{
					if(Res!=IND[1])//0X6B
					{
						USART3_RX_STA&=0x3fff;
					}//接收错误
					else 
					{
						USART3_RX_STA|=0x8000;	//接收完成了 		
					}
				}
			else //还没收到
				{	
					if(Res==IND[0])//0X6F
					USART3_RX_STA|=0x4000;
					else
					{
						USART3_RX_BUF[USART3_RX_STA&0X3FFF]=Res ;
						USART3_RX_STA++;
						if(USART3_RX_STA>(USART3_REC_LEN-1))USART3_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			} 		 
     }  
     
   



		 
		if(USART3_RX_STA&0x8000)
		{						
			LED = !LED;
			IND[0]=0;IND[1]=0;
			len=USART3_RX_STA&0x3fff;//得到此次接收到的数据长度
			DEBUG_USARTx = USART3;//输出为串口3
			printf("%d\r\n",len);
			for(t=0;t<len;t++)
			{								
				  switch(USART3_RX_BUF[t])
					{
						case 'C'://Count
						{
										Count_flag = 1;
						};break;
						case 'X'://X坐标
						{
							if(USART3_RX_BUF[t+1] == ':')//"X:"
							{
								  
									if(Count_flag == 0)//Count_flag=0 写入绝对位移距离
									{    
										    i=0;j=0;//计数清零
												while(!(USART3_RX_BUF[t+i]==0x20))
												{
													  if((USART3_RX_BUF[t+i]>=0x30)&&(USART3_RX_BUF[t+i]<=0x39))
														{	
												      BUF[j] = USART3_RX_BUF[t+i] - 0x30;
													    j++;
														}
														i++;
												}
												if(j == 4)
												{
												    Position[0]=BUF[0]*1000+BUF[1]*100+BUF[2]*10+BUF[3];       
												}
												else if(j == 5)
												{
					                  Position[0]=BUF[0]*10000+BUF[1]*1000+BUF[2]*100+BUF[3]*10+BUF[4];    							
												}						   
									}
									else//Count_flag=1 写入绝对步进值
									{
										    i=0;j=0;//计数清零
												while(!(USART3_RX_BUF[t+i]==0x20))
												{
													  if((USART3_RX_BUF[t+i]>=0x30)&&(USART3_RX_BUF[t+i]<=0x39))
														{	
												      BUF[j] = USART3_RX_BUF[t+i] - 0x30;
													    j++;
														}
														i++;
												}
												if(j == 4)
												{
												    Position[5]=BUF[0]*1000+BUF[1]*100+BUF[2]*10+BUF[3];       
												}
												else if(j == 5)
												{
					                  Position[5]=BUF[0]*10000+BUF[1]*1000+BUF[2]*100+BUF[3]*10+BUF[4];    							
												}						   												
									}
							}
						
						};break;
						case 'Y'://Y坐标
						{
							if(USART3_RX_BUF[t+1] == ':')//"Y:"
							{
									if(Count_flag == 0)//Count_flag=0 写入绝对位移距离
									{
										    i=0;j=0;//计数清零
												while(!(USART3_RX_BUF[t+i]==0x20))
												{
													  if((USART3_RX_BUF[t+i]>=0x30)&&(USART3_RX_BUF[t+i]<=0x39))
														{	
												      BUF[j] = USART3_RX_BUF[t+i] - 0x30;
													    j++;
														}
														i++;
												}
												if(j == 4)
												{
												    Position[1]=BUF[0]*1000+BUF[1]*100+BUF[2]*10+BUF[3];       
												}
												else if(j == 5)
												{
					                  Position[1]=BUF[0]*10000+BUF[1]*1000+BUF[2]*100+BUF[3]*10+BUF[4];    							
												}								
									}
									else//Count_flag=1 写入绝对步进值
									{
										    i=0;j=0;//计数清零
												while(!(USART3_RX_BUF[t+i]==0x20))
												{
													  if((USART3_RX_BUF[t+i]>=0x30)&&(USART3_RX_BUF[t+i]<=0x39))
														{	
												      BUF[j] = USART3_RX_BUF[t+i] - 0x30;
													    j++;
														}
														i++;
												}
												if(j == 4)
												{
												    Position[6]=BUF[0]*1000+BUF[1]*100+BUF[2]*10+BUF[3];       
												}
												else if(j == 5)
												{
					                  Position[6]=BUF[0]*10000+BUF[1]*1000+BUF[2]*100+BUF[3]*10+BUF[4];    							
												}											
									}							
							}						
						
						};break;
						case 'Z'://Z坐标
						{
							if(USART3_RX_BUF[t+1] == ':')//"Z:"
							{
									if(Count_flag == 0)//Count_flag=0 写入绝对位移距离
									{
										    i=0;j=0;//计数清零
												while(!(USART3_RX_BUF[t+i]==0x20))
												{
													  if((USART3_RX_BUF[t+i]>=0x30)&&(USART3_RX_BUF[t+i]<=0x39))
														{	
												      BUF[j] = USART3_RX_BUF[t+i] - 0x30;
													    j++;
														}
														i++;
												}
												if(j == 4)
												{
												    Position[2]=BUF[0]*1000+BUF[1]*100+BUF[2]*10+BUF[3];       
												}
												else if(j == 5)
												{
					                  Position[2]=BUF[0]*10000+BUF[1]*1000+BUF[2]*100+BUF[3]*10+BUF[4];    							
												}								
									}
									else//Count_flag=1 写入绝对步进值
									{
										    i=0;j=0;//计数清零
												while(!(USART3_RX_BUF[t+i]==0x0d))
												{
													  if((USART3_RX_BUF[t+i]>=0x30)&&(USART3_RX_BUF[t+i]<=0x39))
														{	
												      BUF[j] = USART3_RX_BUF[t+i] - 0x30;
													    j++;
														}
														i++;
												}
												if(j == 4)
												{
												    Position[7]=BUF[0]*1000+BUF[1]*100+BUF[2]*10+BUF[3];       
												}
												else if(j == 5)
												{
					                  Position[7]=BUF[0]*10000+BUF[1]*1000+BUF[2]*100+BUF[3]*10+BUF[4];    							
												}													
									}									
							}							
						
						};break;
						case 'E'://挤出距离
						{
							if(USART3_RX_BUF[t+1] == ':')//"E:"
							{
										    i=0;j=0;//计数清零
												while(!(USART3_RX_BUF[t+i]==0x20))
												{
													  if((USART3_RX_BUF[t+i]>=0x30)&&(USART3_RX_BUF[t+i]<=0x39))
														{	
												      BUF[j] = USART3_RX_BUF[t+i] - 0x30;
													    j++;
														}
														i++;
												}
												if(j == 4)
												{
												    Position[3]=BUF[0]*1000+BUF[1]*100+BUF[2]*10+BUF[3];       
												}
												else if(j == 5)
												{
					                  Position[3]=BUF[0]*10000+BUF[1]*1000+BUF[2]*100+BUF[3]*10+BUF[4];    							
												}								
							}							
						
						};break;
						case 'T':
						{
							if(USART3_RX_BUF[t+1] == ':')//"T:"
							{
										    i=0;j=0;//计数清零
												while(!(USART3_RX_BUF[t+i]==0x20))
												{
													  if((USART3_RX_BUF[t+i]>=0x30)&&(USART3_RX_BUF[t+i]<=0x39))
														{	
												      BUF[j] = USART3_RX_BUF[t+i] - 0x30;
													    j++;
														}
														i++;
												}
												if(j == 4)
												{
												    Temperature[0]=BUF[0]*1000+BUF[1]*100+BUF[2]*10+BUF[3];       
												}
												else if(j == 5)
												{
					                  Temperature[0]=BUF[0]*10000+BUF[1]*1000+BUF[2]*100+BUF[3]*10+BUF[4];    							
												}															
							}	
						
						};break;
						case 'B':
						{
							if(USART3_RX_BUF[t+1] == ':')//"B:"
							{
										    i=0;j=0;//计数清零
												while(!(USART3_RX_BUF[t+i]==0x20))
												{
													  if((USART3_RX_BUF[t+i]>=0x30)&&(USART3_RX_BUF[t+i]<=0x39))
														{	
												      BUF[j] = USART3_RX_BUF[t+i] - 0x30;
													    j++;
														}
														i++;
												}
												if(j == 4)
												{
												    Temperature[1]=BUF[0]*1000+BUF[1]*100+BUF[2]*10+BUF[3];       
												}
												else if(j == 5)
												{
					                  Temperature[1]=BUF[0]*10000+BUF[1]*1000+BUF[2]*100+BUF[3]*10+BUF[4];    							
												}							
							}	
						};break;
						
						case '@':
						{
							
						};break;
						default:break;
					}				
			}
				
			DEBUG_USARTx = USART3;//输出为串口3
			printf("Position[0]：%d",Position[0]);
			printf("Position[1]：%d",Position[1]);
			printf("Position[2]：%d",Position[2]);
			printf("Position[3]：%d",Position[3]);
			printf("Position[5]：%d",Position[5]);
			printf("Position[6]：%d",Position[6]);
			printf("Position[7]：%d",Position[7]);
			printf("Temperature[0]：%d",Temperature[0]);
			printf("Temperature[1]：%d",Temperature[1]);
			USART3_RX_STA=0;		
			k=0;
		}

} 
#endif

  
 


 /***************** 发送一个字符  **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* 发送一个字节到USART */
	USART_SendData(pUSARTx,ch);
		
	/* 等待发送数据寄存器为空 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/***************** 发送字符串  **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* 等待发送完成 */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}
}

/*****************  发送一个16位数 **********************/
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	/* 取出高8位 */
	temp_h = (ch&0XFF00)>>8;
	/*  */
	temp_l = ch&0XFF;
	
	/* 取出低8位 */
	USART_SendData(pUSARTx,temp_h);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
	/* 发送低8位 */
	USART_SendData(pUSARTx,temp_l);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}


 /*******************************************************************************  
* 函数名称     : uart1_send_char  
* 函数功能     : 串口1发送一字节      
* 输入         : 无 
* 输出         : 无  
*******************************************************************************/    
void uart1_send_char(u8 temp)      
{     
    USART_SendData(USART1,(u8)temp);        
    while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);          
}    

/*******************************************************************************  
* 函数名称     : uart1_send_buff  
* 函数功能     : 串口1发送一字符串      
* 输入         : 无 
* 输出         : 无  
*******************************************************************************/    
void uart1_send_buff(u8 buf[],u32 len)         
{    
    u32 i;    
    for(i=0;i<len;i++)    
    uart1_send_char(buf[i]);      
}    

/*******************************************************************************  
* 函数名称     : uart2_send_char  
* 函数功能     : 串口2发送一字节      
* 输入         : 无 
* 输出         : 无  
*******************************************************************************/    
void uart2_send_char(u8 temp)      
{        
    USART_SendData(USART2,(u8)temp);        
    while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);         
}    

/*******************************************************************************  
* 函数名称     : uart2_send_buff  
* 函数功能     : 串口2发送一字符串
* 输入         : 无 
* 输出         : 无  
*******************************************************************************/    
void uart2_send_buff(u8 buf[],u32 len)     
{    
    u32 i;    
    for(i=0;i<len;i++)    
    uart2_send_char(buf[i]);         
}    


/*******************************************************************************  
* 函数名称     : uart3_send_buff  
* 函数功能     : 串口3发送一字节
* 输入         : 无 
* 输出         : 无  
*******************************************************************************/    
void uart3_send_char(u8 temp)         
{    
    USART_SendData(USART3,(u8)temp);        
    while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);         
}    



/*******************************************************************************  
* 函数名称     : uart3_send_buff  
* 函数功能     : 串口3发送一字符串
* 输入         : 无 
* 输出         : 无  
*******************************************************************************/    
void uart3_send_buff(u8 buf[],u32 len)    
{    
    u32 i;    
    for(i=0;i<len;i++)    
    uart3_send_char(buf[i]);            
}

/*******************************************************************************  
* 函数名称     : HMI_end 
* 函数功能     : HMI停止信号
* 输入         : 无 
* 输出         : 无  
*******************************************************************************/ 
void HMI_end(void)
{
    uart1_send_char(0xff);
    uart1_send_char(0xff);
    uart1_send_char(0xff);//结束指令
}

