#include "include.h"
extern u8 Record_time;//�ǵ�ǰ��¼�ĵ�ĸ��� ��0��ʼ
u32 pulse_rec_num;
extern Motor_State MotorState;

//����һ����¼x��y��������ľ�̬��������֤����ǰһֱ�����ϴε�ֵ
int X_Part_Count=0;  
int Y_Part_Count=0;
int X_Total_Count=0;
int Y_Total_Count=0;


X_SpeedList_TypeDef	X_Speed;
u32 X_TIM3_Step_Position = 0;			// ��ǰλ��
u8	X_TIM3_Motion_Status = 0;			// 0:ֹͣ��1:���٣�2:���٣�3:����

int z_count=0;

float X_Fre_List[X_SpeedList_LEN];			// Ƶ���б�                 //#define	X_SpeedList_LEN		((u8)X_Accel_Step)
u16 X_Toggle_Pulse[X_SpeedList_LEN];		// ��ӦƵ�ʵ�Period         //#define		X_Accel_Step	1.0f
u32 X_TIM3_CosTTNum = 0;					// X ���ٽ׶ε��������

u16 Pluse_High_z=10; //����ߵ�ƽ�жϴ���      1->10us
u16 temp_z=10;//1000/temp_z=���������жϴ��� ,temp_z��ת�ٳ�����
u32 TimeCount_z=0;

u16 Pluse_High_r=10; //����ߵ�ƽ�жϴ���      1->10us
u16 temp_r=10;//1000/temp_r=���������жϴ��� ,temp_r��ת�ٳ�����
u32 TimeCount_r=0;
/******************** X - GPIO *********************/
void bsp_ALL_GPIO_Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM5, ENABLE);
	RCC_AHB1PeriphClockCmd(	RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(	RCC_AHB1Periph_GPIOC, ENABLE);
	// GPIOA A6  pwm\A7 pwm--tim3
	GPIO_PinAFConfig( GPIOA, GPIO_PinSource6, GPIO_AF_TIM3);
	GPIO_PinAFConfig( GPIOA, GPIO_PinSource7, GPIO_AF_TIM3);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;		// TIM3_CH1 - PA6
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			    // ����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		        // ���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			    // ����
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// GPIOA A4 dir\A5 dir  pc4 --tim3  Ҫ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			    // ���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		        // ���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			    // ����
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			    // ���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		        // ���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			    // ����
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	// GPIOA A0 pwm tim5--z
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;		// TIM5_CH1 - PA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			// ����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		// ���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;			// ����
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// GPIOA A1 dir tim5--z
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			    // ���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		        // ���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			    // ����
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// GPIOA A2  tim2 pwm  tim2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;		// TIM2_CH3 - PA2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			// ����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		// ���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;			// ����
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// GPIOA A3 dir  tim2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			    // ���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		        // ���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			    // ����
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/******************** X - TIM3 *********************/


void bsp_XY_TIM3_Config(void)
{
	TIM_TimeBaseInitTypeDef		TIM_TimeBaseStructure;
    TIM_OCInitTypeDef		TIM_OCInitStructure;
    NVIC_InitTypeDef	NVIC_InitStructure;
	// ʱ��Ƶ������
	TIM_TimeBaseStructure.TIM_Prescaler = X_TIM3_Prescaler - 1;     //84
	TIM_TimeBaseStructure.TIM_Period =X_TIM3_Period ;               //X_TIM3_Period  �����ܸ���ô��
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit( TIM3, &TIM_TimeBaseStructure);
	
	
	// ���ù���ģʽ
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;			    // �Ƚ����ģʽ����ת���
	TIM_OCInitStructure.TIM_Pulse = X_TIM3_Period/2;		        //X_TIM3_Period/2 �������ú�������		
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   // ʹ�ܱȽ����					
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;		// �������
	TIM_OC1Init( TIM3, &TIM_OCInitStructure);						// ��ʼ��
	TIM_OC1PreloadConfig( TIM3, TIM_OCPreload_Enable);		        // CH1Ԥװ��ʹ�ܣ��޸�	
	TIM_OC2Init( TIM3, &TIM_OCInitStructure);						// ��ʼ��
	TIM_OC2PreloadConfig( TIM3, TIM_OCPreload_Enable);		        // CH1Ԥװ��ʹ�ܣ��޸�	
    TIM_ARRPreloadConfig(TIM3,ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure);	
}

void Z_TIM5_Config(u32 Prescaler,u32 Period)
{
	TIM_TimeBaseInitTypeDef		TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	// ʱ��Ƶ������
	TIM_TimeBaseStructure.TIM_Prescaler = Prescaler - 1;
	TIM_TimeBaseStructure.TIM_Period = Period-1;//X_TIM5_Period
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit( TIM5, &TIM_TimeBaseStructure);
	
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);
	
	// ���ù���ģʽ
	NVIC_InitStructure.NVIC_IRQChannel=TIM5_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM5, ENABLE);
}
void TIM5_IRQHandler(void)//10us 
{
	if(TIM_GetITStatus(TIM5,TIM_IT_Update)==SET)//����ж�
	{		
			
			TimeCount_z++;//
			if(TimeCount_z<Pluse_High_z)//����ߵ�ƽ PHZ=10
				GPIO_SetBits(GPIOA,GPIO_Pin_0);
			else if(TimeCount_z>Pluse_High_z)
				GPIO_ResetBits(GPIOA,GPIO_Pin_0);
			
			if(TimeCount_z>Pluse_Period_z)//���ڿ��ƣ�ת�ٿ��ƣ� Pluse=1000/temp_z   the more temp_z the less PPZ and the higer freqency
				
				TimeCount_z=0;
			if(MotorState.V_Z>0) 
				z_count++;
			if(MotorState.V_Z<0) 
				z_count--;


	}
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update);  
}
void TIM2_Config(u32 Prescaler,u32 Period)
{
	TIM_TimeBaseInitTypeDef		TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	// ʱ��Ƶ������
	TIM_TimeBaseStructure.TIM_Prescaler = Prescaler - 1;
	TIM_TimeBaseStructure.TIM_Period = Period-1 ;//X_TIM2_Period
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit( TIM2, &TIM_TimeBaseStructure);
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	
	// ���ù���ģʽ
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	TIM_Cmd(TIM2, ENABLE); 
}



void TIM2_IRQHandler(void)//10us
	
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)//����ж�
	{		
			TimeCount_r++;
			if(TimeCount_r<Pluse_High_r)//����ߵ�ƽ
				GPIO_SetBits(GPIOA,GPIO_Pin_2);
			else if(TimeCount_r>Pluse_High_r)
				GPIO_ResetBits(GPIOA,GPIO_Pin_2);
			
			if(TimeCount_r>Pluse_Period_r)//���ڿ��ƣ�ת�ٿ��ƣ�
				TimeCount_r=0;

	}
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  
}
/**
	* �������ܣ��ٶȱ���㺯��
	* ���������fre_List[], ��������Ƶ������
	*			toggle_pulse[], ��ӦƵ�ʵ�Period
	*			step[],	���ٹ����в���Ҫ�� ʵ�ʼӼ��ٽ׶��ܲ��� = 1 + 2 * step = 101
	*   		fre_min - ��ʼƵ�ʣ�Hz / step/s
    *           fre_max - ���Ƶ�ʣ�Hz / step/s
	*			jerk, S�͵�ƽ���̶ȣ�ԽСԽƽ��
	*	˵    ���������ٶ����ߺͼ���ʱ�䣬�������ܼ�����������ÿһ�����ٶ�ֵ����������ڴ��С�
    *   ������õ���ѧģ�����ȱ���ֱ���˶����Ӽ���-������-����-�Ӽ���-�����١�
**/
void Calculate_SpeedList(u32 X_PulseNum)
{
	u8 i_x = 0;                         //ѭ������
  float num_x;					    // ��ʵ������
	float molecule_x;				    // ��ʽ����
	float denominator_x;		        // ��ʽ��ĸ
	X_Speed.X_Step = X_Accel_Step;      //#define		X_Accel_Step		1.0f
	X_Speed.X_Fre_Min = X_FREQ_MIN;     //#define		X_FREQ_MIN			500.0f
	X_Speed.X_Fre_Max = X_FREQ_MAX;     //#define		X_FREQ_MAX			3500.0f
	X_Speed.X_Jerk = X_JERK;            //#define		X_JERK				4.0f


	
	num_x = X_Speed.X_Step / 2;
//	step = step + 1;			                                                        // ��һ����ʼƵ�ʣ������õľ���50����
	molecule_x = X_Speed.X_Fre_Max - X_Speed.X_Fre_Min;                                 // molecule_x = 3500.0f - 500.0f = 3000.0f
	X_TIM3_CosTTNum = X_PulseNum - (2 * X_Speed.X_Step + 1);                            //X_TIM3_CosTTNum = X_PulseNum - 2*1/2+1(2)
	
	for (i_x = 0; i_x < (u8)X_Speed.X_Step; i_x++)
	{
		denominator_x = 1.0f + (float)exp(-X_Speed.X_Jerk * (i_x - num_x) / num_x);     //denominator_x = 1+e^4
		X_Fre_List[i_x] = X_Speed.X_Fre_Min + molecule_x / denominator_x;               //��������Ƶ������X_Fre_List[0]=500 + 3000/(e^4)  = 105
		X_Toggle_Pulse[i_x] = (u16)(X_TIM3_FREQ / X_Fre_List[i_x]);                     //2000000/105 = 19047 X_Toggle_Pulse[0] = 19047
	}
}


void X_PWM_S_Output(int DIR)
	
{
	X_TIM3_Step_Position = 0;
	X_TIM3_Motion_Status = X_ACCEL;
    
	if(DIR==UPWARD)
	{

		GPIO_SetBits(GPIOA, GPIO_Pin_4);
		GPIO_SetBits(GPIOC, GPIO_Pin_4);
	}
	else
	{

		GPIO_ResetBits(GPIOA, GPIO_Pin_4);
		GPIO_ResetBits(GPIOC, GPIO_Pin_4);
	}
	TIM_ClearITPendingBit( TIM3, TIM_IT_CC1);//ȥ��֮��Ͳ��ܿ�����  ��������  ��� TIMx ���жϴ�����λ
	TIM_ClearITPendingBit( TIM3, TIM_IT_CC2);
	TIM_ITConfig( TIM3, TIM_IT_CC1, ENABLE);//Enables or disables the specified TIM interrupts.
	TIM_ITConfig( TIM3, TIM_IT_CC2, ENABLE);
	TIM_Cmd(TIM3, ENABLE);//Enables or disables the specified TIM peripheral  ��֮�⼸����ǽ��ж�
}

void Y_PWM_S_Output(int DIR)
{
	X_TIM3_Step_Position = 0;
	X_TIM3_Motion_Status = X_ACCEL;
	
	if(DIR==UPWARD)
	{
       
		GPIO_SetBits(GPIOC, GPIO_Pin_4);
		GPIO_ResetBits(GPIOA, GPIO_Pin_4);
	}
	else
	{

		GPIO_ResetBits(GPIOC, GPIO_Pin_4);
		GPIO_SetBits(GPIOA, GPIO_Pin_4);
	}

//    TIM_IT: specifies the pending bit to clear.
//  *          This parameter can be any combination of the following values:
//  *            @arg TIM_IT_Update: TIM1 update Interrupt source
//  *            @arg TIM_IT_CC1: TIM Capture Compare 1 Interrupt source
//  *            @arg TIM_IT_CC2: TIM Capture Compare 2 Interrupt source
//  *            @arg TIM_IT_CC3: TIM Capture Compare 3 Interrupt source
//  *            @arg TIM_IT_CC4: TIM Capture Compare 4 Interrupt source
//  *            @arg TIM_IT_COM: TIM Commutation Interrupt source
//  *            @arg TIM_IT_Trigger: TIM Trigger Interrupt source
//  *            @arg TIM_IT_Break: TIM Break Interrupt source
	TIM_ClearITPendingBit( TIM3, TIM_IT_CC1);
	TIM_ClearITPendingBit( TIM3, TIM_IT_CC2);
	TIM_ITConfig( TIM3, TIM_IT_CC1, ENABLE);
	TIM_ITConfig( TIM3, TIM_IT_CC2, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
}



void Z_PWM_S_Output(int DIR,int pluse_high_z,int v_z)//���� �ߵ�ƽʱ�䣨ʱ��Ϊ0ʱ���ֹͣ�� �ٶȣ�10-100��
{
	if(DIR==UPWARD)
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_1);
	}
	else
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_1);
	}
	Pluse_High_z=pluse_high_z;
	temp_z=v_z;
	
}
void R_PWM_S_Output(int DIR,int pluse_high_r,int v_r)//���� �ߵ�ƽʱ�䣨ʱ��Ϊ0ʱ���ֹͣ�� �ٶȣ�10-100��
{
	if(DIR==UPWARD)
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_3);
	}
	else
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_3);
	}
	Pluse_High_r=pluse_high_r;
	temp_r=v_r;
	
}
/******************** X - IRQ *********************/
void TIM3_IRQHandler(void)//10000000/84m
{
	u16 X_TIM_Count = 0;
	volatile static float X_TIM_Pulse = 2000;			                //       ����һ������ 500Hz
	if (TIM_GetITStatus( TIM3, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit( TIM3, TIM_IT_CC1);		                // ����жϱ�־λ
		X_TIM_Count = TIM_GetCapture1(TIM3);                            //�õ�ͨ��һ�ļ���ֵ
        pulse_rec_num++;//%add when IT comes
        if(MotorState.V_X>0) 		{X_Part_Count++;X_Total_Count++;}   //�ֲ�����ֵ���ܼ���ֵ��һ,it means recording the distance
		if(MotorState.V_X<0) 		{X_Part_Count--;X_Total_Count--;}
		if(MotorState.V_Y>0)   	    {Y_Part_Count++;Y_Total_Count++;}
		if(MotorState.V_Y<0)        {Y_Part_Count--;Y_Total_Count--;}
        
		if (pulse_rec_num%2==0)       //                                  //���������ν����жϲŻ������һ��  �������ִ����֮��Ÿ�xy��λ�ü�һ
		{
			
			if (X_TIM3_Motion_Status == X_ACCEL)                        //���ٽ׶�
			{
				X_TIM3_Step_Position++;                                 // u32 X_TIM3_Step_Position = 0;  ��ǰλ�������һ

				if (X_TIM3_Step_Position < X_SpeedList_LEN)             //���ڼӼ��ٽ׶� X_Accel_Step	1.0f�ǲ���С��1�� ��ִ�д˴�����

				{
					X_TIM_Pulse = X_Toggle_Pulse[X_TIM3_Step_Position - 1] / 2; //X_TIM_Pulse = X_Toggle_Pulse[0] = 19047
				}
				else//(X_TIM3_Step_Position >= X_SpeedList_LEN)         //���˼Ӽ��ٽ׶�
				{
					X_TIM_Pulse = X_Toggle_Pulse[X_TIM3_Step_Position - 1] / 2;	//X_TIM_Pulse = X_Toggle_Pulse[0] = 19047	
					if (X_TIM3_CosTTNum > 0)                                    //��������е����ٽ׶�
					{
						X_TIM3_Motion_Status = X_COSTT;                         
						//TIM_Pulse = Toggle_Pulse[50] / 2; 		    // �൱�ڱ�����һ�� TIM_Pulse = Toggle_Pulse[50] / 2; 
					}
					else//�����е����ٽ׶ι�ȥ�ˣ���ʼ�����еļ���
					{
						X_TIM3_Motion_Status = X_DECEL;
					}
					X_TIM3_Step_Position = 0;
				}
			}
//--------------------------------------IF            
            
            //����
			else if (X_TIM3_Motion_Status == X_COSTT)
			{
				X_TIM3_Step_Position++;			                        // ��ǰ��Ҫִ�еĲ��� Step_Position - 1 ���Ѿ�ִ�еĲ���
				X_TIM_Pulse = X_Toggle_Pulse[X_SpeedList_LEN - 1] / 2;  ////X_TIM_Pulse = X_Toggle_Pulse[0] = 19047
				if (X_TIM3_Step_Position == X_TIM3_CosTTNum)            //�Ѿ��ﵽ������Ҫ��
				{
					X_TIM3_Motion_Status = X_DECEL;
					X_TIM3_Step_Position = 0;                           //���˼��ٽ׶� ��0
				}
			}
//--------------------------------------IF
			else if (X_TIM3_Motion_Status == X_DECEL)//����
			{
				X_TIM3_Step_Position++;
				if (X_TIM3_Step_Position < (X_SpeedList_LEN + 1))//����������ڼ��ٶ�һ��
				{
					X_TIM_Pulse = X_Toggle_Pulse[X_SpeedList_LEN - X_TIM3_Step_Position] / 2;
				}
				else//�߹�ͷ��
				{
					TIM_ITConfig( TIM3, TIM_IT_CC1, DISABLE);
					TIM_ITConfig( TIM3, TIM_IT_CC2, DISABLE);
					TIM_Cmd(TIM3, DISABLE);		// �رն�ʱ��
					X_TIM3_Step_Position = 0;
					X_TIM3_Motion_Status = X_STOP;
				}
			}
        
         
		}//if ���μ�¼���ж�
	}//if�ж�
	TIM_SetCompare1(TIM3, (u16)(X_TIM_Count + X_TIM_Pulse));
	TIM_SetCompare2(TIM3, (u16)(X_TIM_Count + X_TIM_Pulse));
}//hs
void z_init(void)
{
	Z_TIM5_Config(84,10);//
	Pluse_High_z=0;
}
void r_init(void)
{
	TIM2_Config(84,10);
	Pluse_High_r=0;
}
///****************************END OF FILE****************************/
