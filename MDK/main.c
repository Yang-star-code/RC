#include <include.h>

TaskHandle_t startTaskHandle;
TaskHandle_t Information_Handle;

void startTask(void *pvParameters);
void BSP_INIT(void);//��Ӳ����ʼ��
   
int main(void)
{

    BSP_INIT();//ѹ�������� ��ģң�� ���
	  //printf("%d",100);//0300  
    xTaskCreate(startTask, "START_TASK", 300, NULL, 2, &startTaskHandle); //������ʼ����
	  vTaskStartScheduler();												  //�����������
}//main
	
	
//��ʼ������
void startTask(void *pvParameters)
{
	taskENTER_CRITICAL();										  //�����ٽ���
    xTaskCreate(FSM,         "FSM"        , 500, NULL, 4, NULL);  //״̬������
	  xTaskCreate(Show,         "Show"       , 500, NULL, 6, NULL);  //״̬������
		xTaskCreate(Information, "Information", 500, NULL, 5, &Information_Handle);  //ADS1115��ȡʾ��OLED��ӡ��Ϣ    
 	  vTaskDelete(startTaskHandle);                                 //ɾ������
	taskEXIT_CRITICAL();		                                  //�˳��ٽ���
}	

	
void BSP_INIT(void)
{
	//��������
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�жϷ���
	delay_init(168);                                //��ʼ����ʱ����
    RobotStateInit();                               //������״̬��ʼ��
	usart1_init(115200);                            //��ʼ������
    bsp_Air_Init();                                 //��ģң��
    bsp_ADS1115_Init();                             //ѹ��������
    bsp_ALL_GPIO_Init();                            //���GPIO����
    bsp_XY_TIM3_Config();                           //xy�����ʱ������
		z_init();
		r_init(); 
	#if (USE_OLED==Config)
    bsp_I2C_OLED_Init();
	  bsp_OLED_Init();
    #else
    #endif
}

