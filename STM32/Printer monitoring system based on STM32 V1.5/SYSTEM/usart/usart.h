#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
#include "delay.h"

#define USART1_REC_LEN  			200  	//串口1定义最大接收字节数 200
#define USART2_REC_LEN  			200  	//串口1定义最大接收字节数 200
#define USART3_REC_LEN  			200  	//串口1定义最大接收字节数 200

#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
#define EN_USART2_RX 			1		//使能（1）/禁止（0）串口1接收
#define EN_USART3_RX 			1		//使能（1）/禁止（0）串口1接收

extern u8  USART1_RX_BUF[USART1_REC_LEN]; //接收缓冲,最大USART1_REC_LEN个字节.末字节为换行符 
extern u8  USART2_RX_BUF[USART2_REC_LEN]; //接收缓冲,最大USART2_REC_LEN个字节.末字节为换行符 
extern u8  USART3_RX_BUF[USART3_REC_LEN]; //接收缓冲,最大USART3_REC_LEN个字节.末字节为换行符 

extern u16 USART1_RX_STA;         		//接收状态标记
extern u16 USART2_RX_STA;         		//接收状态标记	
extern u16 USART3_RX_STA;         		//接收状态标记	

/*存档数据格式如下：


*/
extern u16 Temperature[2];           //温度数据存档
extern u16 Position[8]   ;           //位置数据存档     

//如果想串口中断接收，请不要注释以下宏定义
void uart1_init(u32 bound);
void uart2_init(u32 bound);
void uart3_init(u32 bound);



void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
void Usart_SendString( USART_TypeDef * pUSARTx, char *str);
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch);
extern USART_TypeDef * DEBUG_USARTx;
int fputc(int ch,FILE *p);  
void uart1_send_char(u8 temp);   
void uart1_send_buff(u8 buf[],u32 len);  
void uart2_send_char(u8 temp);  
void uart2_send_buff(u8 buf[],u32 len);  
void uart3_send_char(u8 temp);  
void uart3_send_buff(u8 buf[],u32 len);  
void HMI_end(void);//HMI停止信号
#endif

