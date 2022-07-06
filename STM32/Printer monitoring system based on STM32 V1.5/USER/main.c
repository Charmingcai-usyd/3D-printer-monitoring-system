#include "delay.h"    //ÑÓÊ±Í·ÎÄ¼ş
#include "sys.h"      //¿âº¯ÊıÍ·ÎÄ¼ş
#include "usart.h"    //´®¿ÚÍ·ÎÄ¼ş
#include "led.h"      //LEDÍ·ÎÄ¼ş
#include "24cxx.h"	  //EEPROMÍ·ÎÄ¼ş
#include "stdlib.h"   //¶¨Òå±ê×¼ÊäÈëÊä³öÍ·ÎÄ¼ş
#include "stdio.h"    //±ê×¼C¿â
#include "encode.h"   //±àÂëÆ÷Í·ÎÄ¼ş

/*
                                      ÈÕÖ¾
					ÈÕÆÚ													 ±¸×¢                                            °æ±¾ºÅ
         2021.3.4                    Íê³ÉEEPROM ÒÆÖ² µ«²¢Î´½ÓÈë²âÊÔ                       V1.0
				 2021.3.6                    ¼ÓÈë´®¿ÚÆÁÏÔÊ¾³ÌĞò                                   V1.1.0
				 2021.3.10                   ¶Ô½Óµ¥×ÜÏß²âÊÔ                                       V1.1.1
				 2021.3.11                   ¶Ô½Ó´òÓ¡»ú¿ØÖÆ°å ÄÜ½øĞĞÊı¾İÉ¸Ñ¡                      V1.2
				 2021.3.15                   Íê³É±àÂëÆ÷½âËãÓëÎïÁªÍøÄ£¿éÊı¾İÉÏ´«                   V1.3
				 2021.4.2                    ¹¦ÄÜÍêÉÆµÚÒ»°æ                                       V1.4
				 2021.5.2                    ×îÖÕµ÷ÊÔ¶¨ĞÍ                                         V1.5
*/


 int main(void)
 {		
	
	delay_init();	    	 //ÑÓÊ±º¯Êı³õÊ¼»¯	
	uart1_init(9600);	   //´®¿Ú³õÊ¼»¯Îª115200
	uart2_init(115200);	 //´®¿Ú³õÊ¼»¯Îª115200
	uart3_init(115200);	 //´®¿Ú³õÊ¼»¯Îª115200
	 
  DEBUG_USARTx = USART1;//Êä³öÎª´®¿Ú3
	printf("data.va13.val=0");				     HMI_end();   //None
	
	LED_Init();
	//vl53l0x_init(4);      //³õÊ¼»¯2¸ö¼¤¹â´«¸ĞÆ÷ Ê£ÏÂµÄ½ö½ö½øĞĞÒı½Å³õÊ¼»¯£¬Íê³É³õÊ¼»¯µÄÉè±
  AT24CXX_Init();			//IIC³õÊ¼»¯ 
	 
 	while(AT24CXX_Check())//¼ì²â²»µ½24c02
	{
		delay_ms(1000);
		LED=!LED;//DS0ÉÁË¸
	}	
	
	LED=!LED;	
  data_init();          //Êı¾İ³õÊ¼»¯   
	DEBUG_USARTx = USART1;//Êä³öÎª´®¿Ú1
	printf("USART1 Init OK!");
	DEBUG_USARTx = USART2;//Êä³öÎª´®¿Ú2
	printf("USART2 Init OK!");
	DEBUG_USARTx = USART3;//Êä³öÎª´®¿Ú3
	printf("USART3 Init OK!");  
  delay_ms(1000);	 
	LED=!LED;
	
	TIM_Init();          //¶¨Ê±Æ÷±àÂëÆ÷½Ó¿Ú³õÊ¼»¯	 
	
	delay_ms(3000);	 
	DEBUG_USARTx = USART1;//Êä³öÎª´®¿Ú1
	printf("data.va13.val=1");				     HMI_end();   //³õÊ¼»¯³É¹¦
	delay_ms(3000);	 
 	while(1)
  {
     scan();//Ë¢ĞÂÖ÷ÈÎÎñ
	}
 }



