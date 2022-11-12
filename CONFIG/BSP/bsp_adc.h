#ifndef __ADC_H
#define __ADC_H
#include "sys.h"
u16  Get_Adc(u8 ch); 
u16 Get_Adc_Average(u8 ch,u8 times); 
void Adc_Init(void);
void Adc(void *pvParameters);//直接赋值 不解算

#endif 

