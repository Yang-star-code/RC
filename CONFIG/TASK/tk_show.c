#include "include.h"

extern int task;
extern POINTS record_points[10];
void Show(void *pvParameters)
{
	u8 strong2[100] = {0};
	while(1)
	{
		
		
		sprintf((char *)strong2,"%d",task+1);//0300  
		OLED_ShowString   (0,0,strong2,16);
		
		sprintf((char *)strong2,"%d",record_points[task].Current_Height);//0300  
		OLED_ShowString   (0,16,strong2,16);
		
		sprintf((char *)strong2,"%d",record_points[task].Style_State);//0300  
		OLED_ShowString   (0,32,strong2,16);
   
	  delay_ms(5);//…¡À∏÷‹∆⁄
    vTaskDelay(10);
   }                 
}
