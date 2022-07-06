#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
#include "delay.h"

#define USART1_REC_LEN  			200  	//����1�����������ֽ��� 200
#define USART2_REC_LEN  			200  	//����1�����������ֽ��� 200
#define USART3_REC_LEN  			200  	//����1�����������ֽ��� 200

#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
#define EN_USART2_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
#define EN_USART3_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����

extern u8  USART1_RX_BUF[USART1_REC_LEN]; //���ջ���,���USART1_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u8  USART2_RX_BUF[USART2_REC_LEN]; //���ջ���,���USART2_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u8  USART3_RX_BUF[USART3_REC_LEN]; //���ջ���,���USART3_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 

extern u16 USART1_RX_STA;         		//����״̬���
extern u16 USART2_RX_STA;         		//����״̬���	
extern u16 USART3_RX_STA;         		//����״̬���	

/*�浵���ݸ�ʽ���£�


*/
extern u16 Temperature[2];           //�¶����ݴ浵
extern u16 Position[8]   ;           //λ�����ݴ浵     

//����봮���жϽ��գ��벻Ҫע�����º궨��
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
void HMI_end(void);//HMIֹͣ�ź�
#endif

