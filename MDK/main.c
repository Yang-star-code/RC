#include <include.h>

TaskHandle_t startTaskHandle;
TaskHandle_t Information_Handle;

void startTask(void *pvParameters);
void BSP_INIT(void);//先硬件初始化
   
int main(void)
{

    BSP_INIT();//压力传感器 航模遥控 电机
	  //printf("%d",100);//0300  
    xTaskCreate(startTask, "START_TASK", 300, NULL, 2, &startTaskHandle); //创建起始任务
	  vTaskStartScheduler();												  //开启任务调度
}//main
	
	
//开始任务函数
void startTask(void *pvParameters)
{
	taskENTER_CRITICAL();										  //进入临界区
    xTaskCreate(FSM,         "FSM"        , 500, NULL, 4, NULL);  //状态机任务
	  xTaskCreate(Show,         "Show"       , 500, NULL, 6, NULL);  //状态机任务
		xTaskCreate(Information, "Information", 500, NULL, 5, &Information_Handle);  //ADS1115读取示数OLED打印信息    
 	  vTaskDelete(startTaskHandle);                                 //删除任务
	taskEXIT_CRITICAL();		                                  //退出临界区
}	

	
void BSP_INIT(void)
{
	//基本功能
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //中断分组
	delay_init(168);                                //初始化延时函数
    RobotStateInit();                               //机器人状态初始化
	usart1_init(115200);                            //初始化串口
    bsp_Air_Init();                                 //航模遥控
    bsp_ADS1115_Init();                             //压力传感器
    bsp_ALL_GPIO_Init();                            //电机GPIO配置
    bsp_XY_TIM3_Config();                           //xy电机定时器配置
		z_init();
		r_init(); 
	#if (USE_OLED==Config)
    bsp_I2C_OLED_Init();
	  bsp_OLED_Init();
    #else
    #endif
}

