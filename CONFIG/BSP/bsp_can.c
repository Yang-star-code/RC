#include <can.h>

void CAN1_init(void)
{
   
	CAN_InitTypeDef        can;
	CAN_FilterInitTypeDef  can_filter;
	GPIO_InitTypeDef       gpio;
	NVIC_InitTypeDef       nvic;

	//��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

	//����IO��
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_CAN1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_CAN1);

	gpio.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	gpio.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOA, &gpio);
	
	//���ù���ģʽ
	CAN_DeInit(CAN1);
	CAN_StructInit(&can);
	
	can.CAN_TTCM = DISABLE;  //��ʱ�䴥��ͨ��ģʽ  
	can.CAN_ABOM = DISABLE;  //����Զ����߹���
	can.CAN_AWUM = DISABLE;  //˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)  
	can.CAN_NART = DISABLE;  //�������Զ�����  ֱ���ɹ� 0
	can.CAN_RFLM = DISABLE;  //���Ĳ�����,�µĸ��Ǿɵ�
	can.CAN_TXFP = ENABLE;   //���ȼ�������ʱ��˳����� ���diable�������ȼ�����
	can.CAN_Mode = CAN_Mode_Normal;
	can.CAN_SJW  = CAN_SJW_1tq;
	can.CAN_BS1 = CAN_BS1_9tq;
	can.CAN_BS2 = CAN_BS2_4tq;
	can.CAN_Prescaler = 3;   //������42/(1+9+4)/3=1Mbps
	CAN_Init(CAN1, &can);

	//ɸѡ��
	can_filter.CAN_FilterNumber = 0;  //������0
	can_filter.CAN_FilterMode = CAN_FilterMode_IdMask;   //����λģʽ
	can_filter.CAN_FilterScale = CAN_FilterScale_32bit; //32λ 
	can_filter.CAN_FilterIdHigh = 0x0000; ////32λID
	can_filter.CAN_FilterIdLow = 0x0000;
	can_filter.CAN_FilterMaskIdHigh = 0x0000;//32λMASK
	can_filter.CAN_FilterMaskIdLow = 0x0000;
	can_filter.CAN_FilterFIFOAssignment = 0;  //������0������FIFO0
	can_filter.CAN_FilterActivation=ENABLE;//���������0
	CAN_FilterInit(&can_filter);
	
	//�жϿ���
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
	
	//ʹ���ж�
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);  //�����ж�
	CAN_ITConfig(CAN1,CAN_IT_TME,ENABLE);   //�����ж�
   
}


unsigned char can1_tx_success_flag = 0 ;  //�ж���Ϣ�Ƿ��ͳɹ�
// CAN1�����ж�
void CAN1_TX_IRQHandler(void)
{
    if(CAN_GetITStatus(CAN1,CAN_IT_TME)!= RESET) //�Ѿ������ж�
	{	
        
	    CAN_ClearITPendingBit(CAN1,CAN_IT_TME); //Clears the CANx's interrupt pending bits.
		can1_tx_success_flag = 1 ;
    }
}



void CAN1_RX0_IRQHandler(void)
{
  CanRxMsg CAN1_RX0_message;  //��ʱ������ݽṹ��
  if(CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET) 
  {
		CAN_Receive(CAN1, CAN_FIFO0, &CAN1_RX0_message);  // ��ȡ����  ��fifo������д��CAN1_RX0_message�ṹ��
		
		m3508_update_m3508_info(&CAN1_RX0_message);  // M3508������ݴ���   //���º���
		
		CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
  }
}


//����3508״̬�ṹ��
M3508_REAL_INFO  M3508_CHASSIS_MOTOR_REAL_INFO[4]; //��ʵ��Ϣ   �洢���ܵ�������
//���õ��ͨ��can���������ݸ���m3508״̬��Ϣ
//����ƽ�ʣ�1KHZ  ��
void m3508_update_m3508_info(CanRxMsg *msg)
{
	switch(msg -> StdId)  // ����׼ID
	{
		case M3508_CHASSIS_MOTOR_ID_1:  //�ĸ����
		{ 
			M3508_CHASSIS_MOTOR_REAL_INFO[0].ANGLE   = (msg -> Data[0] << 8) | msg -> Data[1];  // ת�ӻ�е�Ƕ�
			M3508_CHASSIS_MOTOR_REAL_INFO[0].RPM     = (msg -> Data[2] << 8) | msg -> Data[3];  // ʵ��ת��ת��
			M3508_CHASSIS_MOTOR_REAL_INFO[0].CURRENT = (msg -> Data[4] << 8) | msg -> Data[5];  // ʵ��ת�ص�·
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

//can1���͵���m3508�ĵ���
//ת�ӽǶȷ�Χ��0~8191
//ת��ת��RPM
//����¶ȵ�λ
void M3508_Send_Motor_Currents(void)
{
	CanTxMsg tx_message;

	//���ÿ��ƶ�
	tx_message.IDE = CAN_Id_Standard;
	tx_message.RTR = CAN_RTR_Data;
	tx_message.DLC = 0x08;
	
	//�����ٲöκ����ݶ�
	tx_message.StdId = 0x200;  // ����IDΪ 1 2 3 4�ĵ��
    ////	int16_t  TARGET_CURRENT;
	tx_message.Data[0] = (uint8_t)(M3508_CHASSIS_MOTOR_REAL_INFO[0].TARGET_CURRENT >> 8);
	tx_message.Data[1] = (uint8_t) M3508_CHASSIS_MOTOR_REAL_INFO[0].TARGET_CURRENT;
	tx_message.Data[2] = (uint8_t)(M3508_CHASSIS_MOTOR_REAL_INFO[1].TARGET_CURRENT >> 8);
	tx_message.Data[3] = (uint8_t) M3508_CHASSIS_MOTOR_REAL_INFO[1].TARGET_CURRENT;
	tx_message.Data[4] = (uint8_t)(M3508_CHASSIS_MOTOR_REAL_INFO[2].TARGET_CURRENT >> 8);
	tx_message.Data[5] = (uint8_t) M3508_CHASSIS_MOTOR_REAL_INFO[2].TARGET_CURRENT;
    tx_message.Data[6] = (uint8_t)(M3508_CHASSIS_MOTOR_REAL_INFO[3].TARGET_CURRENT >> 8);
    tx_message.Data[7] = (uint8_t) M3508_CHASSIS_MOTOR_REAL_INFO[3].TARGET_CURRENT;
	CAN_Transmit(CAN1, &tx_message);  // ����ָ��
}
//16λ����ȡ��8λʱ��λ��ʧ

