#ifndef __ENCODE_H__
#define __ENCODE_H__
#include "sys.h" 

#define ENCODER_TIMER_1   TIM2  // Encoder unit connected to TIM1
#define ENCODER_TIMER_2   TIM3  // Encoder unit connected to TIM3
#define ENCODER_TIMER_3   TIM4  // Encoder unit connected to TIM4
#define ENCODER_PPR          (uint16_t)(600)   // number of pulses per revolution
#define transfer_degree_mm  1
#define Threshold_Pluse     1000

void TIM2_Mode_Config(void);//������ģʽ����
void TIM3_Mode_Config(void);//������ģʽ����
void TIM4_Mode_Config(void);//������ģʽ����
void TIM_Init(void);//��ʼ��
void data_init(void);//�ϵ������ʼ��
void scan(void);

#endif
