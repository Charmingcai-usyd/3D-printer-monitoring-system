#include "delay.h"    //延时头文件
#include "sys.h"      //库函数头文件
#include "usart.h"    //串口头文件
#include "led.h"      //LED头文件
#include "24cxx.h"	  //EEPROM头文件
#include "stdlib.h"   //定义标准输入输出头文件
#include "stdio.h"    //标准C库
#include "encode.h"   //编码器头文件

/*
                                      日志
					日期													 备注                                            版本号
         2021.3.4                    完成EEPROM 移植 但并未接入测试                       V1.0
				 2021.3.6                    加入串口屏显示程序                                   V1.1.0
				 2021.3.10                   对接单总线测试                                       V1.1.1
				 2021.3.11                   对接打印机控制板 能进行数据筛选                      V1.2
				 2021.3.15                   完成编码器解算与物联网模块数据上传                   V1.3
				 2021.4.2                    功能完善第一版                                       V1.4
				 2021.5.2                    最终调试定型                                         V1.5
*/


 int main(void)
 {		
	
	delay_init();	    	 //延时函数初始化	
	uart1_init(9600);	   //串口初始化为115200
	uart2_init(115200);	 //串口初始化为115200
	uart3_init(115200);	 //串口初始化为115200
	 
  DEBUG_USARTx = USART1;//输出为串口3
	printf("data.va13.val=0");				     HMI_end();   //None
	
	LED_Init();
	//vl53l0x_init(4);      //初始化2个激光传感器 剩下的仅仅进行引脚初始化，完成初始化的设?
  AT24CXX_Init();			//IIC初始化 
	 
 	while(AT24CXX_Check())//检测不到24c02
	{
		delay_ms(1000);
		LED=!LED;//DS0闪烁
	}	
	
	LED=!LED;	
  data_init();          //数据初始化   
	DEBUG_USARTx = USART1;//输出为串口1
	printf("USART1 Init OK!");
	DEBUG_USARTx = USART2;//输出为串口2
	printf("USART2 Init OK!");
	DEBUG_USARTx = USART3;//输出为串口3
	printf("USART3 Init OK!");  
  delay_ms(1000);	 
	LED=!LED;
	
	TIM_Init();          //定时器编码器接口初始化	 
	
	delay_ms(3000);	 
	DEBUG_USARTx = USART1;//输出为串口1
	printf("data.va13.val=1");				     HMI_end();   //初始化成功
	delay_ms(3000);	 
 	while(1)
  {
     scan();//刷新主任务
	}
 }



