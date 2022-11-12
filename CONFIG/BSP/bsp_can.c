#include <can.h>

void CAN1_init(void)
{
   
	CAN_InitTypeDef        can;
	CAN_FilterInitTypeDef  can_filter;
	GPIO_InitTypeDef       gpio;
	NVIC_InitTypeDef       nvic;

	//开时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

	//配置IO口
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_CAN1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_CAN1);

	gpio.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	gpio.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOA, &gpio);
	
	//配置工作模式
	CAN_DeInit(CAN1);
	CAN_StructInit(&can);
	
	can.CAN_TTCM = DISABLE;  //非时间触发通信模式  
	can.CAN_ABOM = DISABLE;  //软件自动离线管理
	can.CAN_AWUM = DISABLE;  //睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)  
	can.CAN_NART = DISABLE;  //允许报文自动传送  直到成功 0
	can.CAN_RFLM = DISABLE;  //报文不锁定,新的覆盖旧的
	can.CAN_TXFP = ENABLE;   //优先级由请求时间顺序决定 如果diable则由优先级决定
	can.CAN_Mode = CAN_Mode_Normal;
	can.CAN_SJW  = CAN_SJW_1tq;
	can.CAN_BS1 = CAN_BS1_9tq;
	can.CAN_BS2 = CAN_BS2_4tq;
	can.CAN_Prescaler = 3;   //波特率42/(1+9+4)/3=1Mbps
	CAN_Init(CAN1, &can);

	//筛选器
	can_filter.CAN_FilterNumber = 0;  //过滤器0
	can_filter.CAN_FilterMode = CAN_FilterMode_IdMask;   //屏蔽位模式
	can_filter.CAN_FilterScale = CAN_FilterScale_32bit; //32位 
	can_filter.CAN_FilterIdHigh = 0x0000; ////32位ID
	can_filter.CAN_FilterIdLow = 0x0000;
	can_filter.CAN_FilterMaskIdHigh = 0x0000;//32位MASK
	can_filter.CAN_FilterMaskIdLow = 0x0000;
	can_filter.CAN_FilterFIFOAssignment = 0;  //过滤器0关联到FIFO0
	can_filter.CAN_FilterActivation=ENABLE;//激活过滤器0
	CAN_FilterInit(&can_filter);
	
	//中断控制
	nvic.NVIC_IRQChannel = CAN1_RX0_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 1;
	nvic.NVIC_IRQChannelSubPriority = 0;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
	
	nvic.NVIC_IRQChannel = CAN1_TX_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 1;
	nvic.NVIC_IRQChannelSubPriority = 1;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);
	
	//使能中断
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);  //接收中断
	CAN_ITConfig(CAN1,CAN_IT_TME,ENABLE);   //发送中断
   
}


unsigned char can1_tx_success_flag = 0 ;  //判断消息是否发送成功
// CAN1发送中断
void CAN1_TX_IRQHandler(void)
{
    if(CAN_GetITStatus(CAN1,CAN_IT_TME)!= RESET) //已经发生中断
	{	
        
	    CAN_ClearITPendingBit(CAN1,CAN_IT_TME); //Clears the CANx's interrupt pending bits.
		can1_tx_success_flag = 1 ;
    }
}



void CAN1_RX0_IRQHandler(void)
{
  CanRxMsg CAN1_RX0_message;  //临时存放数据结构体
  if(CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET) 
  {
		CAN_Receive(CAN1, CAN_FIFO0, &CAN1_RX0_message);  // 读取数据  把fifo的数据写入CAN1_RX0_message结构体
		
		m3508_update_m3508_info(&CAN1_RX0_message);  // M3508电机数据处理   //更新函数
		
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
  }
}


//定义3508状态结构体
M3508_REAL_INFO  M3508_CHASSIS_MOTOR_REAL_INFO[4]; //真实信息   存储接受到的数据
//利用电机通过can反馈的数据更新m3508状态信息
//接受平率：1KHZ  ？
void m3508_update_m3508_info(CanRxMsg *msg)
{
	switch(msg -> StdId)  // 检测标准ID
	{
		case M3508_CHASSIS_MOTOR_ID_1:  //四个电机
		{ 
			M3508_CHASSIS_MOTOR_REAL_INFO[0].ANGLE   = (msg -> Data[0] << 8) | msg -> Data[1];  // 转子机械角度
			M3508_CHASSIS_MOTOR_REAL_INFO[0].RPM     = (msg -> Data[2] << 8) | msg -> Data[3];  // 实际转子转速
			M3508_CHASSIS_MOTOR_REAL_INFO[0].CURRENT = (msg -> Data[4] << 8) | msg -> Data[5];  // 实际转矩电路
		}; break;
		
		case M3508_CHASSIS_MOTOR_ID_2:
		{ 
			M3508_CHASSIS_MOTOR_REAL_INFO[1].ANGLE   = (msg -> Data[0] << 8) | msg -> Data[1];  
			M3508_CHASSIS_MOTOR_REAL_INFO[1].RPM     = (msg -> Data[2] << 8) | msg -> Data[3];  
			M3508_CHASSIS_MOTOR_REAL_INFO[1].CURRENT = (msg -> Data[4] << 8) | msg -> Data[5];  
		}; break;
		
		case M3508_CHASSIS_MOTOR_ID_3:
		{ 
			M3508_CHASSIS_MOTOR_REAL_INFO[2].ANGLE   = (msg -> Data[0] << 8) | msg -> Data[1]; 
			M3508_CHASSIS_MOTOR_REAL_INFO[2].RPM     = (msg -> Data[2] << 8) | msg -> Data[3];  
			M3508_CHASSIS_MOTOR_REAL_INFO[2].CURRENT = (msg -> Data[4] << 8) | msg -> Data[5];  
		}; break;	
		
  	case M3508_CHASSIS_MOTOR_ID_4:
		{ 
			M3508_CHASSIS_MOTOR_REAL_INFO[3].ANGLE   = (msg -> Data[0] << 8) | msg -> Data[1];  
			M3508_CHASSIS_MOTOR_REAL_INFO[3].RPM     = (msg -> Data[2] << 8) | msg -> Data[3];  
			M3508_CHASSIS_MOTOR_REAL_INFO[3].CURRENT = (msg -> Data[4] << 8) | msg -> Data[5]; 
		}; break;	
		default: break;
	}
}

//can1发送底盘m3508的电流
//转子角度范围：0~8191
//转子转速RPM
//电机温度单位
void M3508_Send_Motor_Currents(void)
{
	CanTxMsg tx_message;

	//配置控制端
	tx_message.IDE = CAN_Id_Standard;
	tx_message.RTR = CAN_RTR_Data;
	tx_message.DLC = 0x08;
	
	//配置仲裁段和数据段
	tx_message.StdId = 0x200;  // 用于ID为 1 2 3 4的电机
    ////	int16_t  TARGET_CURRENT;
	tx_message.Data[0] = (uint8_t)(M3508_CHASSIS_MOTOR_REAL_INFO[0].TARGET_CURRENT >> 8);
	tx_message.Data[1] = (uint8_t) M3508_CHASSIS_MOTOR_REAL_INFO[0].TARGET_CURRENT;
	tx_message.Data[2] = (uint8_t)(M3508_CHASSIS_MOTOR_REAL_INFO[1].TARGET_CURRENT >> 8);
	tx_message.Data[3] = (uint8_t) M3508_CHASSIS_MOTOR_REAL_INFO[1].TARGET_CURRENT;
	tx_message.Data[4] = (uint8_t)(M3508_CHASSIS_MOTOR_REAL_INFO[2].TARGET_CURRENT >> 8);
	tx_message.Data[5] = (uint8_t) M3508_CHASSIS_MOTOR_REAL_INFO[2].TARGET_CURRENT;
    tx_message.Data[6] = (uint8_t)(M3508_CHASSIS_MOTOR_REAL_INFO[3].TARGET_CURRENT >> 8);
    tx_message.Data[7] = (uint8_t) M3508_CHASSIS_MOTOR_REAL_INFO[3].TARGET_CURRENT;
	CAN_Transmit(CAN1, &tx_message);  // 发送指令
}
//16位数据取成8位时高位损失

