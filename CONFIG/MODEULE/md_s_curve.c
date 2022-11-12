#include "include.h"
extern u8 Record_time;//是当前记录的点的个数 从0开始
u32 pulse_rec_num;
extern Motor_State MotorState;

//定义一个记录x和y脉冲个数的静态变量，保证清零前一直保持上次的值
int X_Part_Count=0;  
int Y_Part_Count=0;
int X_Total_Count=0;
int Y_Total_Count=0;


X_SpeedList_TypeDef	X_Speed;
u32 X_TIM3_Step_Position = 0;			// 当前位置
u8	X_TIM3_Motion_Status = 0;			// 0:停止，1:加速，2:匀速，3:减速

int z_count=0;

float X_Fre_List[X_SpeedList_LEN];			// 频率列表                 //#define	X_SpeedList_LEN		((u8)X_Accel_Step)
u16 X_Toggle_Pulse[X_SpeedList_LEN];		// 对应频率的Period         //#define		X_Accel_Step	1.0f
u32 X_TIM3_CosTTNum = 0;					// X 匀速阶段的脉冲个数

u16 Pluse_High_z=10; //脉冲高电平中断次数      1->10us
u16 temp_z=10;//1000/temp_z=脉冲周期中断次数 ,temp_z与转速成正比
u32 TimeCount_z=0;

u16 Pluse_High_r=10; //脉冲高电平中断次数      1->10us
u16 temp_r=10;//1000/temp_r=脉冲周期中断次数 ,temp_r与转速成正比
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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			    // 复用
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		        // 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			    // 上拉
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// GPIOA A4 dir\A5 dir  pc4 --tim3  要改
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			    // 输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		        // 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			    // 上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			    // 输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		        // 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			    // 上拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	// GPIOA A0 pwm tim5--z
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;		// TIM5_CH1 - PA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			// 复用
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		// 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;			// 上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// GPIOA A1 dir tim5--z
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			    // 输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		        // 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			    // 上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// GPIOA A2  tim2 pwm  tim2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;		// TIM2_CH3 - PA2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			// 复用
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		// 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;			// 上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// GPIOA A3 dir  tim2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			    // 输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		        // 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			    // 上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/******************** X - TIM3 *********************/


void bsp_XY_TIM3_Config(void)
{
	TIM_TimeBaseInitTypeDef		TIM_TimeBaseStructure;
    TIM_OCInitTypeDef		TIM_OCInitStructure;
    NVIC_InitTypeDef	NVIC_InitStructure;
	// 时钟频率设置
	TIM_TimeBaseStructure.TIM_Prescaler = X_TIM3_Prescaler - 1;     //84
	TIM_TimeBaseStructure.TIM_Period =X_TIM3_Period ;               //X_TIM3_Period  好像不能给那么大
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit( TIM3, &TIM_TimeBaseStructure);
	
	
	// 设置工作模式
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;			    // 比较输出模式，反转输出
	TIM_OCInitStructure.TIM_Pulse = X_TIM3_Period/2;		        //X_TIM3_Period/2 这里我用函数处理		
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   // 使能比较输出					
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;		// 输出极性
	TIM_OC1Init( TIM3, &TIM_OCInitStructure);						// 初始化
	TIM_OC1PreloadConfig( TIM3, TIM_OCPreload_Enable);		        // CH1预装载使能，修改	
	TIM_OC2Init( TIM3, &TIM_OCInitStructure);						// 初始化
	TIM_OC2PreloadConfig( TIM3, TIM_OCPreload_Enable);		        // CH1预装载使能，修改	
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
	// 时钟频率设置
	TIM_TimeBaseStructure.TIM_Prescaler = Prescaler - 1;
	TIM_TimeBaseStructure.TIM_Period = Period-1;//X_TIM5_Period
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit( TIM5, &TIM_TimeBaseStructure);
	
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);
	
	// 设置工作模式
	NVIC_InitStructure.NVIC_IRQChannel=TIM5_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM5, ENABLE);
}
void TIM5_IRQHandler(void)//10us 
{
	if(TIM_GetITStatus(TIM5,TIM_IT_Update)==SET)//溢出中断
	{		
			
			TimeCount_z++;//
			if(TimeCount_z<Pluse_High_z)//脉冲高电平 PHZ=10
				GPIO_SetBits(GPIOA,GPIO_Pin_0);
			else if(TimeCount_z>Pluse_High_z)
				GPIO_ResetBits(GPIOA,GPIO_Pin_0);
			
			if(TimeCount_z>Pluse_Period_z)//周期控制（转速控制） Pluse=1000/temp_z   the more temp_z the less PPZ and the higer freqency
				
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
	// 时钟频率设置
	TIM_TimeBaseStructure.TIM_Prescaler = Prescaler - 1;
	TIM_TimeBaseStructure.TIM_Period = Period-1 ;//X_TIM2_Period
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit( TIM2, &TIM_TimeBaseStructure);
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	
	// 设置工作模式
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	TIM_Cmd(TIM2, ENABLE); 
}



void TIM2_IRQHandler(void)//10us
	
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)//溢出中断
	{		
			TimeCount_r++;
			if(TimeCount_r<Pluse_High_r)//脉冲高电平
				GPIO_SetBits(GPIOA,GPIO_Pin_2);
			else if(TimeCount_r>Pluse_High_r)
				GPIO_ResetBits(GPIOA,GPIO_Pin_2);
			
			if(TimeCount_r>Pluse_Period_r)//周期控制（转速控制）
				TimeCount_r=0;

	}
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  
}
/**
	* 函数功能：速度表计算函数
	* 输入参数：fre_List[], 各个步的频率数组
	*			toggle_pulse[], 对应频率的Period
	*			step[],	变速过程中步数要求， 实际加减速阶段总步数 = 1 + 2 * step = 101
	*   		fre_min - 初始频率，Hz / step/s
    *           fre_max - 最高频率，Hz / step/s
	*			jerk, S型的平滑程度，越小越平滑
	*	说    明：根据速度曲线和加速时间，将数据密集化，即计算每一步的速度值，并存放在内存中。
    *   这里采用的数学模型是匀变速直线运动，加加速-减加速-匀速-加减速-减减速。
**/
void Calculate_SpeedList(u32 X_PulseNum)
{
	u8 i_x = 0;                         //循环变量
  float num_x;					    // 其实是整数
	float molecule_x;				    // 公式分子
	float denominator_x;		        // 公式分母
	X_Speed.X_Step = X_Accel_Step;      //#define		X_Accel_Step		1.0f
	X_Speed.X_Fre_Min = X_FREQ_MIN;     //#define		X_FREQ_MIN			500.0f
	X_Speed.X_Fre_Max = X_FREQ_MAX;     //#define		X_FREQ_MAX			3500.0f
	X_Speed.X_Jerk = X_JERK;            //#define		X_JERK				4.0f


	
	num_x = X_Speed.X_Step / 2;
//	step = step + 1;			                                                        // 有一个起始频率，现在用的就是50步。
	molecule_x = X_Speed.X_Fre_Max - X_Speed.X_Fre_Min;                                 // molecule_x = 3500.0f - 500.0f = 3000.0f
	X_TIM3_CosTTNum = X_PulseNum - (2 * X_Speed.X_Step + 1);                            //X_TIM3_CosTTNum = X_PulseNum - 2*1/2+1(2)
	
	for (i_x = 0; i_x < (u8)X_Speed.X_Step; i_x++)
	{
		denominator_x = 1.0f + (float)exp(-X_Speed.X_Jerk * (i_x - num_x) / num_x);     //denominator_x = 1+e^4
		X_Fre_List[i_x] = X_Speed.X_Fre_Min + molecule_x / denominator_x;               //各个步的频率数组X_Fre_List[0]=500 + 3000/(e^4)  = 105
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
	TIM_ClearITPendingBit( TIM3, TIM_IT_CC1);//去掉之后就不受控制了  看到这里  清除 TIMx 的中断待处理位
	TIM_ClearITPendingBit( TIM3, TIM_IT_CC2);
	TIM_ITConfig( TIM3, TIM_IT_CC1, ENABLE);//Enables or disables the specified TIM interrupts.
	TIM_ITConfig( TIM3, TIM_IT_CC2, ENABLE);
	TIM_Cmd(TIM3, ENABLE);//Enables or disables the specified TIM peripheral  总之这几句就是进中断
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



void Z_PWM_S_Output(int DIR,int pluse_high_z,int v_z)//方向 高电平时间（时间为0时电机停止） 速度（10-100）
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
void R_PWM_S_Output(int DIR,int pluse_high_r,int v_r)//方向 高电平时间（时间为0时电机停止） 速度（10-100）
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
	volatile static float X_TIM_Pulse = 2000;			                //       ？第一个脉冲 500Hz
	if (TIM_GetITStatus( TIM3, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit( TIM3, TIM_IT_CC1);		                // 清楚中断标志位
		X_TIM_Count = TIM_GetCapture1(TIM3);                            //得到通道一的计数值
        pulse_rec_num++;//%add when IT comes
        if(MotorState.V_X>0) 		{X_Part_Count++;X_Total_Count++;}   //局部计数值和总计数值加一,it means recording the distance
		if(MotorState.V_X<0) 		{X_Part_Count--;X_Total_Count--;}
		if(MotorState.V_Y>0)   	    {Y_Part_Count++;Y_Total_Count++;}
		if(MotorState.V_Y<0)        {Y_Part_Count--;Y_Total_Count--;}
        
		if (pulse_rec_num%2==0)       //                                  //这样是两次进入中断才会进这里一次  这个函数执行完之后才给xy的位置加一
		{
			
			if (X_TIM3_Motion_Status == X_ACCEL)                        //加速阶段
			{
				X_TIM3_Step_Position++;                                 // u32 X_TIM3_Step_Position = 0;  当前位置数组加一

				if (X_TIM3_Step_Position < X_SpeedList_LEN)             //还在加加速阶段 X_Accel_Step	1.0f是不会小于1的 不执行此处代码

				{
					X_TIM_Pulse = X_Toggle_Pulse[X_TIM3_Step_Position - 1] / 2; //X_TIM_Pulse = X_Toggle_Pulse[0] = 19047
				}
				else//(X_TIM3_Step_Position >= X_SpeedList_LEN)         //过了加加速阶段
				{
					X_TIM_Pulse = X_Toggle_Pulse[X_TIM3_Step_Position - 1] / 2;	//X_TIM_Pulse = X_Toggle_Pulse[0] = 19047	
					if (X_TIM3_CosTTNum > 0)                                    //进入加速中的匀速阶段
					{
						X_TIM3_Motion_Status = X_COSTT;                         
						//TIM_Pulse = Toggle_Pulse[50] / 2; 		    // 相当于保持上一个 TIM_Pulse = Toggle_Pulse[50] / 2; 
					}
					else//加速中的匀速阶段过去了，开始加速中的减速
					{
						X_TIM3_Motion_Status = X_DECEL;
					}
					X_TIM3_Step_Position = 0;
				}
			}
//--------------------------------------IF            
            
            //匀速
			else if (X_TIM3_Motion_Status == X_COSTT)
			{
				X_TIM3_Step_Position++;			                        // 当前将要执行的步数 Step_Position - 1 是已经执行的步数
				X_TIM_Pulse = X_Toggle_Pulse[X_SpeedList_LEN - 1] / 2;  ////X_TIM_Pulse = X_Toggle_Pulse[0] = 19047
				if (X_TIM3_Step_Position == X_TIM3_CosTTNum)            //已经达到了匀速要求
				{
					X_TIM3_Motion_Status = X_DECEL;
					X_TIM3_Step_Position = 0;                           //到了减速阶段 清0
				}
			}
//--------------------------------------IF
			else if (X_TIM3_Motion_Status == X_DECEL)//减速
			{
				X_TIM3_Step_Position++;
				if (X_TIM3_Step_Position < (X_SpeedList_LEN + 1))//减速是相比于加速多一步
				{
					X_TIM_Pulse = X_Toggle_Pulse[X_SpeedList_LEN - X_TIM3_Step_Position] / 2;
				}
				else//走过头了
				{
					TIM_ITConfig( TIM3, TIM_IT_CC1, DISABLE);
					TIM_ITConfig( TIM3, TIM_IT_CC2, DISABLE);
					TIM_Cmd(TIM3, DISABLE);		// 关闭定时器
					X_TIM3_Step_Position = 0;
					X_TIM3_Motion_Status = X_STOP;
				}
			}
        
         
		}//if 两次记录的判断
	}//if中断
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
