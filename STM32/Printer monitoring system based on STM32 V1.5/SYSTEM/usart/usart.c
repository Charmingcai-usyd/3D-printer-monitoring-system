#include "sys.h"
#include "usart.h"	  
#include "nvic.h"  
#include "stdio.h"
#include "led.h"
USART_TypeDef * DEBUG_USARTx = USART1;   //���崮��ָ��
u16 Temperature[2];           //�¶����ݴ浵
u16 Position[8]   ;           //λ�����ݴ浵   

#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
		USART_SendData(DEBUG_USARTx, (uint8_t) ch);	
		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);		
		return (ch);
}
#endif 
 


/****************************************************

    USART  ��ʼ�����ú������жϺ���
           ����                           ��ע                        ����
    uart1_init(u32 bound)              ����1��ʼ��
    USART1_IRQHandler(void)            ����1�жϷ�����            
    uart2_init(u32 bound)              ����2��ʼ��
    USART2_IRQHandler(void)            ����2�жϷ�����
    uart3_init(u32 bound)              ����3��ʼ��
    USART3_IRQHandler(void)            ����3�жϷ�����

******************************************************/
  
void uart1_init(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��

	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9

	//USART1_RX	  GPIOA.10��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

	//USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 

}
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART1_RX_BUF[USART1_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART1_RX_STA=0;       //����״̬���	  

void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������

		if((USART1_RX_STA&0x8000)==0)//����δ���
			{
			if(USART1_RX_STA&0x4000)//���յ���0x0d 0xff
				{
        if(Res!=0x0a)USART1_RX_STA=0;//���մ���,���¿�ʼ
				else USART1_RX_STA|=0x8000;	//���������
				}
			else //��û�յ�0X0d 0xff
				{	
				if(Res==0x0d)USART1_RX_STA|=0x4000;
				else
					{
					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;
					USART1_RX_STA++;
					if(USART1_RX_STA>(USART1_REC_LEN-1))USART1_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
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

#if EN_USART2_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART2_RX_BUF[USART2_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART2_RX_STA=0;       //����״̬���	 
void USART2_IRQHandler(void)                	//����2�жϷ������
{
	u8 Res;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
		Res =USART_ReceiveData(USART2);	//��ȡ���յ�������
		uart3_send_char(Res);           //ʵ��usart2 ESP8266 ����λ�� ��������͸��    
		/* ʵ����ESP8266ֱ�ӿ�����λ���ļ���  */
		if((USART2_RX_STA&0x8000)==0)//����δ���
			{
			if(USART2_RX_STA&0x4000)//���յ���0x0d
				{
				if(Res!=0x0a)USART2_RX_STA=0;//���մ���,���¿�ʼ
				else USART2_RX_STA|=0x8000;	//��������� 
				}
			else //��û�յ�0X0D
				{	
				if(Res==0x0d)USART2_RX_STA|=0x4000;
				else
					{
					USART2_RX_BUF[USART2_RX_STA&0X3FFF]=Res ;
					USART2_RX_STA++;
					if(USART2_RX_STA>(USART2_REC_LEN-1))USART2_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
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
 

#if EN_USART3_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART3_RX_BUF[USART2_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART3_RX_STA=0;       //����״̬���	 
void USART3_IRQHandler(void)                	//����1�жϷ������
{
  // "ok" Ϊ�ַ���������־ Ȼ������ݽ����ϴ�����
	u8 Res;
	u16 len,t;//����
	u8    i,j;
	u8   BUF[5];
	static u8   IND[2]={0,0};
	static u8   k=0;
  unsigned char Count_flag = 0;//���������־λ
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
		Res =USART_ReceiveData(USART3);	//��ȡ���յ�������
    if(!k)		
		{	  if(Res == 0x6f)//���Ƚ��յ���'o'
				{
				   IND[0] = 0X40;
					 IND[1] = 0X3A;
				}
				else//Ĭ�Ͻ��յ���'X'	
				{
				   IND[0] = 0X6F;
					 IND[1] = 0X6B;				
				}			
		}
		k=1;
		if((USART3_RX_STA&0x8000)==0)//����δ���
			{
			if(USART3_RX_STA&0x4000)//���յ���
				{
					if(Res!=IND[1])//0X6B
					{
						USART3_RX_STA&=0x3fff;
					}//���մ���
					else 
					{
						USART3_RX_STA|=0x8000;	//��������� 		
					}
				}
			else //��û�յ�
				{	
					if(Res==IND[0])//0X6F
					USART3_RX_STA|=0x4000;
					else
					{
						USART3_RX_BUF[USART3_RX_STA&0X3FFF]=Res ;
						USART3_RX_STA++;
						if(USART3_RX_STA>(USART3_REC_LEN-1))USART3_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
					}		 
				}
			} 		 
     }  
     
   



		 
		if(USART3_RX_STA&0x8000)
		{						
			LED = !LED;
			IND[0]=0;IND[1]=0;
			len=USART3_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
			DEBUG_USARTx = USART3;//���Ϊ����3
			printf("%d\r\n",len);
			for(t=0;t<len;t++)
			{								
				  switch(USART3_RX_BUF[t])
					{
						case 'C'://Count
						{
										Count_flag = 1;
						};break;
						case 'X'://X����
						{
							if(USART3_RX_BUF[t+1] == ':')//"X:"
							{
								  
									if(Count_flag == 0)//Count_flag=0 д�����λ�ƾ���
									{    
										    i=0;j=0;//��������
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
									else//Count_flag=1 д����Բ���ֵ
									{
										    i=0;j=0;//��������
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
						case 'Y'://Y����
						{
							if(USART3_RX_BUF[t+1] == ':')//"Y:"
							{
									if(Count_flag == 0)//Count_flag=0 д�����λ�ƾ���
									{
										    i=0;j=0;//��������
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
									else//Count_flag=1 д����Բ���ֵ
									{
										    i=0;j=0;//��������
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
						case 'Z'://Z����
						{
							if(USART3_RX_BUF[t+1] == ':')//"Z:"
							{
									if(Count_flag == 0)//Count_flag=0 д�����λ�ƾ���
									{
										    i=0;j=0;//��������
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
									else//Count_flag=1 д����Բ���ֵ
									{
										    i=0;j=0;//��������
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
						case 'E'://��������
						{
							if(USART3_RX_BUF[t+1] == ':')//"E:"
							{
										    i=0;j=0;//��������
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
										    i=0;j=0;//��������
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
										    i=0;j=0;//��������
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
				
			DEBUG_USARTx = USART3;//���Ϊ����3
			printf("Position[0]��%d",Position[0]);
			printf("Position[1]��%d",Position[1]);
			printf("Position[2]��%d",Position[2]);
			printf("Position[3]��%d",Position[3]);
			printf("Position[5]��%d",Position[5]);
			printf("Position[6]��%d",Position[6]);
			printf("Position[7]��%d",Position[7]);
			printf("Temperature[0]��%d",Temperature[0]);
			printf("Temperature[1]��%d",Temperature[1]);
			USART3_RX_STA=0;		
			k=0;
		}

} 
#endif

  
 


 /***************** ����һ���ַ�  **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* ����һ���ֽڵ�USART */
	USART_SendData(pUSARTx,ch);
		
	/* �ȴ��������ݼĴ���Ϊ�� */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/***************** �����ַ���  **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* �ȴ�������� */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}
}

/*****************  ����һ��16λ�� **********************/
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	/* ȡ����8λ */
	temp_h = (ch&0XFF00)>>8;
	/*  */
	temp_l = ch&0XFF;
	
	/* ȡ����8λ */
	USART_SendData(pUSARTx,temp_h);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
	/* ���͵�8λ */
	USART_SendData(pUSARTx,temp_l);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}


 /*******************************************************************************  
* ��������     : uart1_send_char  
* ��������     : ����1����һ�ֽ�      
* ����         : �� 
* ���         : ��  
*******************************************************************************/    
void uart1_send_char(u8 temp)      
{     
    USART_SendData(USART1,(u8)temp);        
    while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);          
}    

/*******************************************************************************  
* ��������     : uart1_send_buff  
* ��������     : ����1����һ�ַ���      
* ����         : �� 
* ���         : ��  
*******************************************************************************/    
void uart1_send_buff(u8 buf[],u32 len)         
{    
    u32 i;    
    for(i=0;i<len;i++)    
    uart1_send_char(buf[i]);      
}    

/*******************************************************************************  
* ��������     : uart2_send_char  
* ��������     : ����2����һ�ֽ�      
* ����         : �� 
* ���         : ��  
*******************************************************************************/    
void uart2_send_char(u8 temp)      
{        
    USART_SendData(USART2,(u8)temp);        
    while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);         
}    

/*******************************************************************************  
* ��������     : uart2_send_buff  
* ��������     : ����2����һ�ַ���
* ����         : �� 
* ���         : ��  
*******************************************************************************/    
void uart2_send_buff(u8 buf[],u32 len)     
{    
    u32 i;    
    for(i=0;i<len;i++)    
    uart2_send_char(buf[i]);         
}    


/*******************************************************************************  
* ��������     : uart3_send_buff  
* ��������     : ����3����һ�ֽ�
* ����         : �� 
* ���         : ��  
*******************************************************************************/    
void uart3_send_char(u8 temp)         
{    
    USART_SendData(USART3,(u8)temp);        
    while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);         
}    



/*******************************************************************************  
* ��������     : uart3_send_buff  
* ��������     : ����3����һ�ַ���
* ����         : �� 
* ���         : ��  
*******************************************************************************/    
void uart3_send_buff(u8 buf[],u32 len)    
{    
    u32 i;    
    for(i=0;i<len;i++)    
    uart3_send_char(buf[i]);            
}

/*******************************************************************************  
* ��������     : HMI_end 
* ��������     : HMIֹͣ�ź�
* ����         : �� 
* ���         : ��  
*******************************************************************************/ 
void HMI_end(void)
{
    uart1_send_char(0xff);
    uart1_send_char(0xff);
    uart1_send_char(0xff);//����ָ��
}

