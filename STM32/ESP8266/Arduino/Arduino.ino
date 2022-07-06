#define BLINKER_WIFI
#include <Blinker.h>

char auth[] = "10456b3f9afc";
char ssid[] = "CZM";
char pswd[] = "12345678";

// 组件对象2
BlinkerButton Button_Stop("btn-stop");
BlinkerButton Button_3D("3D-Printer");
BlinkerNumber Num_speedx("num-speedx");
BlinkerNumber Num_speedy("num-speedy");
BlinkerNumber Num_speedz("num-speedz");
BlinkerNumber Num_xc("num-xc");
BlinkerNumber Num_yc("num-yc");
BlinkerNumber Num_zc("num-zc");
BlinkerNumber Num_x("num-x");
BlinkerNumber Num_y("num-y");
BlinkerNumber Num_z("num-z");
BlinkerNumber Num_e("num-e");
BlinkerNumber Num_power("num-power");
BlinkerNumber Num_pt1("num-pt1");
BlinkerNumber Num_pt2("num-pt2");
BlinkerText   Text_info("tex-info");
BlinkerText   Text_check("tex-check");

int sensorPin = A0;    // select the input pin for the potentiometer
int sensorValue = 0;   // variable to store the value coming from the sensor
int PowerValue = 0;   // variable to store the value computing from the value of sensorValue
int i=0;                 //刷新次数
String message_c;
const char* message;
int Position[5],Temperature0,Temperature1,Distance1,Distance2,Distance3,Position_x,Position_y,Position_z;
float Speed1,Speed2,Speed3;


//云存储温湿度数据函数
void dataStorage()
{
    Blinker.dataStorage("Power",sensorValue);//存储功率
}

void Button_3DPrinter_callback(const String & state) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    Serial.printf("%s\r\n",&state);   
}
void Button_Stop_callback(const String & state) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    BLINKER_LOG("get button state: ", state);
    if (state=="on") {
        Button_Stop.print("on");
        Button_Stop.icon("far fa-computer-classic");//急停按钮
        Button_Stop.color("#00FF00");//绿色
        Button_Stop.text("ON");//打开
       
        Serial.printf("M18\r\n");//释放所有步进电机
        
    } else if(state=="off"){
        Button_Stop.print("off");
        Button_Stop.icon("far fa-computer-classic");//急停按钮
        Button_Stop.color("#FF0000");//红色
        Button_Stop.text("OFF");//关闭
        
        Serial.printf("M17\r\n");//锁死所有步进电机
    }
}


void setup() {
    // 初始化串口，并开启调试信息
    Serial.begin(115200);    
    // 初始化有LED的IO
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    // 初始化blinker
    Blinker.begin(auth, ssid, pswd);
    Button_Stop.attach(Button_Stop_callback);
    Button_3D.attach(Button_3DPrinter_callback);
    Blinker.attachDataStorage(dataStorage);
    UI_init();//UI初始化
    Serial.printf("ESP8266 Init OK!/r");
}
void UI_init()
{
     Blinker.delay(2000);
        //界面初始化
        Text_info.print("Connected","Init OK");
        Num_x.print(0);
        Num_y.print(0);       
        Num_z.print(0);
        Num_e.print(0);  
        Num_power.print(0);      
        Num_xc.print(0);
        Num_yc.print(0);
        Num_zc.print(0);
        Num_speedx.print(0);
        Num_speedy.print(0);
        Num_speedz.print(0);   
        Num_pt1.print(0);
        Num_pt2.print(0);       
        Text_check.print("监视离线","None"); 

                        
        Button_Stop.print("on");
        Button_Stop.icon("far fa-computer-classic");//急停按钮
        Button_Stop.color("#00FF00");//绿色
        Button_Stop.text("ON");//打开 
        Text_info.color("#FF4500");//橘色
        Text_info.print("Connected","unwork");//状态：连接并汇报打印机状态 
}
void loop() {
    Blinker.run();
    i++;
    if( i>= 10 )
    {
        Text_info.color("#3CB371");//深绿色
        Text_info.print("Connected","work");//状态：连接并汇报打印机状态
        sensorValue = analogRead(sensorPin);//检测模拟信号
        PowerValue = (float)11.8*(sensorValue*5000/1024/92.5);//计算功率
        Position_x = Position[0]/100;
        Position_y = Position[1]/100;
        Position_z = Position[2]/100; 
           
        Num_x.print(Position_x);
        Num_y.print(Position_y);       
        Num_z.print(Position_z);
        Num_e.print(Position[3]/10);    
        Num_x.color("#48D1CC");//蓝色
        Num_y.color("#48D1CC");//蓝色
        Num_z.color("#48D1CC");//蓝色
        Num_e.color("#48D1CC");//蓝色   
             
        Num_xc.print(Distance1);
        Num_yc.print(Distance2);
        Num_zc.print(Distance3); 
        
        Num_pt1.print(Temperature0);
        Num_pt2.print(Temperature1); 
          
        if(Speed1>0)
        {
          Num_speedx.print(Speed1);  
          Num_speedx.color("#48D1CC");//蓝色         
        }
        else
        {
          Num_speedx.print(0);  
          Num_speedx.color("#808080");//灰色        
        }
        if(Speed2>0)
        {
          Num_speedy.print(Speed2);
          Num_speedy.color("#48D1CC");//蓝色  
        }       
        else
        {
          Num_speedy.print(0);  
          Num_speedy.color("#808080");//灰色        
        }
        if(Speed3>0)
        {
          Num_speedz.print(Speed3);
          Num_speedz.color("#48D1CC");//蓝色    
        } 
        else
        {
          Num_speedz.print(0);  
          Num_speedz.color("#808080");//灰色        
        }
        
        if(Temperature0>=200)//如果喷头温度 full
        {
           Num_pt1.color("#800000");//红黑色
           Num_pt1.icon("fa fa-thermometer-full");//  
        }
        if((Temperature0>=150)&&(Temperature0<=200))//如果喷头温度 three-quarts
        {
           Num_pt1.color("#FF0000");//红色
           Num_pt1.icon("fa fa-thermometer-three-quarters");//   
        }        
        if((Temperature0>=100)&&(Temperature0<=150))//如果喷头温度 half
        {
           Num_pt1.color("#FF8C00");//橙色
           Num_pt1.icon("fa fa-thermometer-half");//     
        }      
        if((Temperature0>=70)&&(Temperature0<=100))//如果喷头温度 quarter
        {
           Num_pt1.color("#87CEFA");//淡蓝色
           Num_pt1.icon("fa fa-thermometer-quarter");//      
        }      
        if(Temperature0<70)//如果喷头温度 empty
        {
           Num_pt1.color("#1E90FF");//蓝色
           Num_pt1.icon("fa fa-thermometer-empty");//  
        }
        if(Temperature1>=200)//如果喷头温度 full
        {
           Num_pt2.color("#800000");//红黑色
           Num_pt2.icon("fa fa-thermometer-full");//  
        }
        if((Temperature1>=150)&&(Temperature1<=200))//如果喷头温度 three-quarts
        {
           Num_pt2.color("#FF0000");//红色
           Num_pt2.icon("fa fa-thermometer-three-quarters");//   
        }        
        if((Temperature1>=100)&&(Temperature1<=150))//如果喷头温度 half
        {
           Num_pt2.color("#FF8C00");//橙色
           Num_pt2.icon("fa fa-thermometer-half");//     
        }      
        if((Temperature1>=70)&&(Temperature1<=100))//如果喷头温度 quarter
        {
           Num_pt2.color("#87CEFA");//淡蓝色
           Num_pt2.icon("fa fa-thermometer-quarter");//      
        }      
        if(Temperature1<70)//如果喷头温度 empty
        {
           Num_pt2.color("#1E90FF");//蓝色
           Num_pt2.icon("fa fa-thermometer-empty");//  
        }
            
        Text_check.print("监视在线","Enable");   
        Text_check.color("#00FFFF");//青色
        Text_check.icon("fad fa-lock");//锁上      
        i = 0;   
        //颜色UI变化

/************************    UI设计    ***************************************/
        if(sensorValue>=0)//如果检测到功率
        {    
           Num_power.print(sensorValue);   
           Num_power.color("#FFA500");//金黄色
           Num_power.icon("fas fa-lightbulb-on");//灯亮              
          }
        else//如果没有检测到功率 功率为0
        {
           Num_power.print(0);      
           Num_power.color("#808080");//灰色 
           Num_power.icon("fas fa-lightbulb-slash");//灯灭              
          }
        if(((Distance1+10)>=Position_x)&&((Distance1-10)<=Position_x))
        {
           Num_xc.color("#00FA9A");//绿色
          }
        else
        {
           Num_xc.color("#FF0000");//红色
          }
          
        if(((Distance2+10)>=Position_y)&&((Distance2-10)<=Position_y))
        {
           Num_yc.color("#00FA9A");//绿色
          }
        else
        {
           Num_yc.color("#FF0000");//红色
          }
          
        if(((Distance3+10)>=Position_z)&&((Distance3-10)<=Position_z))
        {
           Num_zc.color("#00FA9A");//绿色
          }
        else
        {
           Num_zc.color("#FF0000");//红色
          }
         
     }
    if (Serial.available())
      { 
        String message_c = Serial.readString(); //串口1接收字符串
        //角度方向%d；角度%f；移动距离%d；速度方向%d；速度%f 
        message = message_c.c_str();                //由于sscanf只能识别const char*类型字符串，将String类型字符串转成const char*类型
        sscanf(message,"%d;%f;%d;%f;%d;%f;%d;%d;%d;%d;%d;%d,"
                        ,&Distance1,&Speed1,&Distance2,&Speed2,&Distance3,&Speed3
                        ,&Position[0],&Position[1],&Position[2],&Position[3]
                        ,&Temperature0,&Temperature1 );       
                        Temperature0 = Temperature0/100;
                        Temperature1 = Temperature1/100;
        while(Serial.read()>=0){};      //清空串口1缓存，保证字符串的长度稳定
       } 
}
