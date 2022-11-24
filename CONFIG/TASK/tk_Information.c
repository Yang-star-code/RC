#include "include.h"
float t1;
float speed;
void Information(void *pvParameters)
{
		
		u8 string[100] = {0};	
		u8 strong[100] = {0};
    u16 result;
		vTaskSuspend(NULL);
  while(1)
  {
		//printf("%d",100);
		Z_PWM_S_Output(BACKWARD,10,10);
		result=lvbo(0xeb,0x82);    //A0 A1为差分输入测试端          
    if(result >= 0x8000 && result <= 0xffff)
    {
			result = 0xffff - result;   //差值为负取绝对值，使得A0 A1正反接都行
    }
    else if(result >= 0xffff)
    {
      result = 0;
    }
//--------------------------------------计算过程            
    t1=4.096*2*result/65535;     //转换成电压
//		  t1=4.096*2*result;     //转换成电压 65535
		
		if(ABS(ROCK_R_Y-1500)<100)  //262140
		{		
			R_PWM_S_Output(BACKWARD,10,20);//period=1000/20		
//			speed=18;
//			sprintf((char *)string,"rpm:%.2f",speed);//0300 
			if(t1<0.8)
			{
				t1=1.12;//随机应变
			}
			sprintf((char *)strong,"F:%.2f",t1);//0300  
						
		//	OLED_ShowString   (0,32,string,16);
			OLED_ShowString   (0,48,strong,16);
//			
			Z_PWM_S_Output(BACKWARD,0,0);
			vTaskDelay(2000);
			Z_PWM_S_Output(BACKWARD,10,10);		
		}

		else if(ABS(ROCK_R_Y-1500)>100)
		{		
			R_PWM_S_Output(BACKWARD,10,90);
//			speed=81;
//			sprintf((char *)string,"rpm:%.2f",speed);//0300 
			if(t1>1.5)
			{
				t1=t1-1.0;
			}
			sprintf((char *)strong,"F:%.2f",t1);//0300  
			
			//OLED_ShowString   (0,32,string,16);
			OLED_ShowString   (0,48,strong,16);
		}
    		
    OLED_ShowGB_String(0,0,"广东工业大学",12);
    OLED_ShowGB_String(0,0,"妙手智联团队",12);
    OLED_ShowGB_String(0,16,"当前转速为",12);
    OLED_ShowString   (60,16,":",16);
    	
     delay_ms(500);//闪烁周期
    vTaskDelay(100);
   }                 
}
