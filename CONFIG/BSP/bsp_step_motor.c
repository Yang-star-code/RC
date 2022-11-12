#include "include.h"

/*********** Group A ***********/
// 定时器1主模式
void TIM1_GPIO_Config(u16 TIM1_Prescaler, u16 TIM1_Period, u16 CCR_A, u16 DIR_A)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef		TIM_TimeBaseStructure;
    TIM_OCInitTypeDef		TIM_OCInitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_TIM1, ENABLE);
	RCC_AHB1PeriphClockCmd(	RCC_AHB1Periph_GPIOE, ENABLE);			// F407: GPIO 串口初始化 与F103不同
	
	GPIO_PinAFConfig( GPIOE, GPIO_PinSource13, GPIO_AF_TIM1);		// F407 端口复用映射
	GPIO_PinAFConfig( GPIOE, GPIO_PinSource14, GPIO_AF_TIM1);		// F407 端口复用映射
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;		// TIM1_CH1 - PE13， CH4 - PE14
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			// 复用
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		// 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			// 上拉
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	
	TIM_TimeBaseStructure.TIM_Period = TIM1_Period - 1;//ARR
	TIM_TimeBaseStructure.TIM_Prescaler = TIM1_Prescaler - 1;//psc
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;			// 重复计数，一定要 = 0  高级定时器TIM1,TIM8，这句必须有。
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	
	
	// 设置工作模式
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;							// PWM1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	// 比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;			// 输出极性
	// PWM通道，TIM1 - 通道1设置函数，50/100
	TIM_OCInitStructure.TIM_Pulse = CCR_A;								// 设置待装入捕获寄存器的脉冲值 pe13
	TIM_OC3Init( TIM1, &TIM_OCInitStructure);
	TIM_SelectMasterSlaveMode( TIM1, TIM_MasterSlaveMode_Enable);
	TIM_SelectOutputTrigger( TIM1, TIM_TRGOSource_Update);
	TIM_OC3PreloadConfig( TIM1, TIM_OCPreload_Enable);
	// PWM通道，TIM1 - 通道4设置函数，100/100 or 0/100
	TIM_OCInitStructure.TIM_Pulse = DIR_A;								// 初始化 TIM1-OC4 pe14
	TIM_OC4Init( TIM1, &TIM_OCInitStructure);						// CH4预装载使能，修改
	TIM_OC4PreloadConfig( TIM1, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM1, ENABLE);
}

//定时器2从模式
void TIM2_GPIO_Config(u32 PulseNum_A)
{
	TIM_TimeBaseInitTypeDef	TIM_TimeBaseStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_TimeBaseStructure.TIM_Period = PulseNum_A;//arr
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit( TIM2, &TIM_TimeBaseStructure);
	
	TIM_SelectInputTrigger( TIM2, TIM_TS_ITR0);			// TIM1-主，TIM2-从
	TIM_SelectSlaveMode( TIM2, TIM_SlaveMode_External1);
	TIM_ITConfig( TIM2, TIM_IT_Update, DISABLE);//为什么要失能
	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure);
}

void PWM_Output_A(u16 Cycle_A, u32 PulseNum_A, u16 DIR_A)			// TIM1-主，TIM2-从
{
	TIM2_GPIO_Config(PulseNum_A);
	TIM_Cmd( TIM2, ENABLE);
	TIM_ClearITPendingBit( TIM2, TIM_IT_Update);
	TIM_ITConfig( TIM2, TIM_IT_Update, ENABLE);
	TIM1_GPIO_Config( 84, Cycle_A, Cycle_A / 2, DIR_A);	//F407: 高级定时器是 168MHz, 故168MHz / 84 = 2MHz
	TIM_Cmd( TIM1, ENABLE);//Enables or disables the specified TIM peripheral
	TIM_CtrlPWMOutputs( TIM1, ENABLE);	// 高级定时器 TIM1 使能 MOE
}

/*********** Group B ***********/
// 定时器4主模式
void TIM4_GPIO_Config(u16 TIM4_Prescaler, u16 TIM4_Period, u16 CCR_B, u16 DIR_B)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef		TIM_TimeBaseStructure;
    TIM_OCInitTypeDef		TIM_OCInitStructure;
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM4, ENABLE);
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE);			// F407: GPIO 串口初始化 与F103不同
	
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource6, GPIO_AF_TIM5);		// F407 端口复用映射
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource7, GPIO_AF_TIM5);		// F407 端口复用映射
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;			// TIM4_CH3 PB0, CH4 - PB1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			// 复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	
	// 时钟频率设置
	TIM_TimeBaseStructure.TIM_Period = TIM4_Period - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = TIM4_Prescaler - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit( TIM4, &TIM_TimeBaseStructure);
	
	
	// 设置工作模式
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;			// 设置工作模式是PWM，且为PWM1工作模式，TIMx_CNT<TIMx_CCR1时为高电平
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;		// 也就是使能PWM输出到端口					
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;			// 输出极性
	// PWM通道，TIM4 - 通道1设置函数，50/100
	TIM_OCInitStructure.TIM_Pulse = CCR_B;					// 设置待装入捕获寄存器的脉冲值
	TIM_OC1Init( TIM4, &TIM_OCInitStructure);						// 初始化 TIM4-OC3
	TIM_SelectMasterSlaveMode( TIM4, TIM_MasterSlaveMode_Enable);		// 定时器主从模式使能
	TIM_SelectOutputTrigger( TIM4, TIM_TRGOSource_Update);						// 选择触发方式：使用更新事件作为触发输出
	TIM_OC1PreloadConfig( TIM4, TIM_OCPreload_Enable);		// CH1预装载使能，修改				
	// PWM通道，TIM4 - 通道2设置函数，100/100 or 0/100
	TIM_OCInitStructure.TIM_Pulse = DIR_B;									// 初始化 TIM4-OC4
	TIM_OC2Init( TIM4, &TIM_OCInitStructure);						// CH2预装载使能，修改
	TIM_OC2PreloadConfig( TIM4, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig( TIM4, ENABLE);					// 使能ARR预装载寄存器
}

//定时器8从模式
void TIM8_GPIO_Config(u32 PulseNum_B)
{
	TIM_TimeBaseInitTypeDef	TIM_TimeBaseStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_TIM8, ENABLE);
	
	TIM_TimeBaseStructure.TIM_Period = PulseNum_B;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit( TIM8, &TIM_TimeBaseStructure);
	
	TIM_SelectInputTrigger( TIM8, TIM_TS_ITR2);			// TIM2-主，TIM4-从
	TIM_SelectSlaveMode( TIM8, TIM_SlaveMode_External1);
	TIM_ITConfig( TIM8, TIM_IT_Update, DISABLE);
	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_TIM13_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure);
}

void PWM_Output_B(u16 Cycle_B, u32 PulseNum_B, u16 DIR_B)			// TIM2-主，TIM4-从
{
	TIM8_GPIO_Config(PulseNum_B);
	TIM_Cmd( TIM8, ENABLE);
	TIM_ClearITPendingBit( TIM8, TIM_IT_Update);
	TIM_ITConfig( TIM8, TIM_IT_Update, ENABLE);
	TIM4_GPIO_Config( 84, Cycle_B, Cycle_B / 2, DIR_B);		//F407:通用定时器是 84MHz, 故84MHz / 84 = 1MHz
	TIM_Cmd( TIM4, ENABLE);
}

/*********** Group C ***********/
// 定时器3主模式
void TIM3_GPIO_Config(u16 TIM3_Prescaler, u16 TIM3_Period, u16 CCR_C, u16 DIR_C)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef		TIM_TimeBaseStructure;
    TIM_OCInitTypeDef		TIM_OCInitStructure;
	// TIM3通道1\2 - PWM Z轴步进电机脉冲信号
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3, ENABLE);
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE);			// F407: GPIO 串口初始化 与F103不同
	
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource0, GPIO_AF_TIM3);		// F407 端口复用映射
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource1, GPIO_AF_TIM3);		// F407 端口复用映射
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;			// TIM3_CH1 PA6, CH2 - PA7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			// 复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);		
	
	
	// 时钟频率设置
	TIM_TimeBaseStructure.TIM_Period = TIM3_Period - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = TIM3_Prescaler - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit( TIM3, &TIM_TimeBaseStructure);
	
	
	// 设置工作模式
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;			// 设置工作模式是PWM，且为PWM1工作模式，TIMx_CNT<TIMx_CCR1时为高电平
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;		// 也就是使能PWM输出到端口					
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;			// 输出极性
	// PWM通道，TIM3 - 通道1设置函数，50/100
	TIM_OCInitStructure.TIM_Pulse = CCR_C;					// 设置待装入捕获寄存器的脉冲值
	TIM_OC3Init( TIM3, &TIM_OCInitStructure);						// 初始化 TIM3-OC1
	TIM_SelectMasterSlaveMode( TIM3, TIM_MasterSlaveMode_Enable);		// 定时器主从模式使能
	TIM_SelectOutputTrigger( TIM3, TIM_TRGOSource_Update);						// 选择触发方式：使用更新事件作为触发输出
	TIM_OC3PreloadConfig( TIM3, TIM_OCPreload_Enable);		// CH1预装载使能，修改				
	// PWM通道，TIM3 - 通道2设置函数，100/100 or 0/100
	TIM_OCInitStructure.TIM_Pulse = DIR_C;									// 初始化 TIM3-OC2
	TIM_OC4Init( TIM3, &TIM_OCInitStructure);						// CH2预装载使能，修改
	TIM_OC4PreloadConfig( TIM3, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig( TIM3, ENABLE);					// 使能ARR预装载寄存器
}

// 定时器5从模式
void TIM5_GPIO_Config(u32 PulseNum_Z)
{
	TIM_TimeBaseInitTypeDef	TIM_TimeBaseStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	
	TIM_TimeBaseStructure.TIM_Period = PulseNum_Z;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit( TIM5, &TIM_TimeBaseStructure);
	
	TIM_SelectInputTrigger( TIM5, TIM_TS_ITR1);
	TIM_SelectSlaveMode( TIM5,TIM_SlaveMode_External1 );		// 等同下一句 TIM5->SMCR |= 0x07
	TIM_ITConfig( TIM5, TIM_IT_Update, DISABLE);
	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

void PWM_Output_C(u16 Cycle_C, u32 PulseNum_C, u16 DIR_C)			// TIM3-主，TIM5-从
{
	TIM5_GPIO_Config(PulseNum_C);
	TIM_Cmd( TIM5, ENABLE);
	TIM_ClearITPendingBit( TIM5, TIM_IT_Update);
	TIM_ITConfig( TIM5, TIM_IT_Update, ENABLE);
	TIM3_GPIO_Config( 56, Cycle_C, Cycle_C / 2, DIR_C);		//84MHz / 56 = 1.5MHz
	TIM_Cmd( TIM3, ENABLE);
}

// Group A
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus( TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit( TIM2, TIM_IT_Update);			// 清除中断标志位
		TIM_CtrlPWMOutputs( TIM1, DISABLE);		// 高级定时器 TIM1 关闭MOE
		TIM_Cmd( TIM1, DISABLE);			// 关闭定时器1
		TIM_Cmd( TIM2, DISABLE);			// 关闭定时器2	
		TIM_ITConfig( TIM2, TIM_IT_Update, DISABLE);
	}
}

// Group B
void TIM8_UP_TIM13_IRQHandler(void)
{
	if (TIM_GetITStatus( TIM8, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit( TIM8, TIM_IT_Update);			// 清除中断标志位
		TIM_Cmd( TIM4, DISABLE);			// 关闭定时器4
		TIM_Cmd( TIM8, DISABLE);			// 关闭定时器8
		TIM_ITConfig( TIM8, TIM_IT_Update, DISABLE);
	}
}

// Group C
void TIM5_IRQHandler(void)
{
	if (TIM_GetITStatus( TIM5, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit( TIM5, TIM_IT_Update);			// 清除中断标志位
		TIM_Cmd( TIM3, DISABLE);			// 关闭定时器3
		TIM_Cmd( TIM5, DISABLE);			// 关闭定时器5
		TIM_ITConfig( TIM5, TIM_IT_Update, DISABLE);
	}
}

/****************************END OF FILE****************************/

