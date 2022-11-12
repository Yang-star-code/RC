#include "include.h"
//��С���� ���ݾ����ͺŶ�Ӧ�ֲ��ȡ,��λ��g��������RP-18.3-ST�ͺ�Ϊ������С������20g
#define PRESS_MIN	20
//������� ���ݾ����ͺŶ�Ӧ�ֲ��ȡ,��λ��g��������RP-18.3-ST�ͺ�Ϊ�������������6kg
#define PRESS_MAX	6000
//����2���������ݻ�ȡ������
//�����ϣ�
// 1.��Ĥѹ�����������Ǿ�׼��ѹ�����Դ�������ֻ�ʺϴ��Բ���ѹ���ã����ܵ�ѹ���ƾ�ȷ������
// 2. AO��������ĵ�ѹ��Ч��Χ��0.1v��3.3v����ʵ�ʸ��ݲ�ͬ��������Χ���������Χ�ڣ�����һ�������ֵ3.3v��Ҳ���ܵ���3.3v��Ҫʵ�����ñ������
// 	����ֻ�Ǹ�������ֵ����Ҫ��ȷ���������ñ����Ȼ����������2��AO���ŵ�ѹ�����������Сֵ
//���ڷ�����
//��Ĥѹ����������AO������������淶Χ��ͨ������AO_RES��λ������ʵ�ֵģ�
//��Ҫ��΢��׼�㣬��Ҫ�Լ�����������֪����Ȼ�����AO_RES��λ����������������������Լ��������Ϳ�����
#define VOLTAGE_MIN 100
#define VOLTAGE_MAX 3300


u16 value_AD = 0;
long PRESS_AO = 0;
int VOLTAGE_AO = 0;
long map(long x, long in_min, long in_max, long out_min, long out_max);

void Adc(void *pvParameters)//ֱ�Ӹ�ֵ ������
{
     while(1)
    {
        value_AD = Get_Adc_Average(1,10);	//10��ƽ��ֵ
		VOLTAGE_AO = map(value_AD, 0, 4095, 0, 3300);
		if(VOLTAGE_AO < VOLTAGE_MIN)//100
		{
			PRESS_AO = 0;
		}
		else if(VOLTAGE_AO > VOLTAGE_MAX)
		{
			PRESS_AO = PRESS_MAX;
		}
		else
		{
			PRESS_AO = map(VOLTAGE_AO, VOLTAGE_MIN, VOLTAGE_MAX, PRESS_MIN, PRESS_MAX);
		}
		printf("ADֵ = %d,��ѹ = %d mv,ѹ�� = %ld g\r\n",value_AD,VOLTAGE_AO,PRESS_AO);	
						

       vTaskDelay(200);
    
    }



}//ad task

long map(long x, long in_min, long in_max, long out_min, long out_max) {
 return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//��ʼ��ADC
//�������ǽ��Թ���ͨ��Ϊ��
//����Ĭ�Ͻ�����ͨ��0~3																	   
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE );	  //ʹ��ADC1ͨ��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
 
	//PA1 ��Ϊģ��ͨ����������                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;		//ģ����������
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//��λ����
	ADC_DeInit();  //��λADC,������ ADC ��ȫ���Ĵ�������Ϊȱʡֵ

	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//Ԥ��Ƶ4��Ƶ��ADCCLK=PCLK2/4=84/4=21Mhz,ADCʱ����ò�Ҫ����36Mhz 
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMAʧ��
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//���������׶�֮����ӳ�5��ʱ��
    ADC_CommonInit(&ADC_CommonInitStructure);//��ʼ��

    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12λģʽ 6-12
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfConversion = 1;	//˳����й���ת����ADCͨ������Ŀ
    
	ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   
	ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1
    
    
    
	
//	ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼  
//	 
//	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����
//	
//	ADC_StartCalibration(ADC1);	 //����ADУ׼
// 
//	while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����
// 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������

}				  
//���ADCֵ
//ch:ͨ��ֵ 0~3
u16 Get_Adc(u8 ch)   
{
  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_480Cycles);	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����	  			    
  
    ADC_SoftwareStartConv(ADC1);		//ʹ��ָ����ADC1�����ת����������		 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}

u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(5);
	}
	return temp_val/times;
} 	 



