#ifndef __STEP_MOTOR_H
#define __STEP_MOTOR_H
#include "sys.h"

/* Group A: TIM1 - 主定时器，TIM2 - 从定时器
	 TIM1: CH3 - PE13, CH4 - PE14   RCC_APB2Periph_TIM1, RCC_AHB1Periph_GPIOE
   TIM2: TIM2: RCC_APB1Periph_TIM2
	 
	 Group B: TIM4 - 主定时器，TIM8 - 从定时器
	 TIM4: CH1 - PB6,  CH2 - PB7		RCC_APB1Periph_TIM4
   TIM8: RCC_APB2Periph_TIM8
	 
	 Group C: TIM3 - 主定时器，TIM4 - 从定时器
	 TIM3: TIM3: CH3 - PB0,  CH4 - PB1		RCC_APB1Periph_TIM3
   TIM5: RCC_APB1Periph_TIM5	
*/

/*****定时器初始化函数*****/
// Group A
void TIM1_GPIO_Config(u16 TIM1_Prescaler, u16 TIM1_Period, u16 CCR_A, u16 DIR_A);
void TIM2_GPIO_Config(u32 PulseNum_A);
void PWM_Output_A(u16 Cycle_A, u32 PulseNum_A, u16 DIR_A);
void TIM2_IRQHandler(void);

// Group B
void TIM4_GPIO_Config(u16 TIM4_Prescaler, u16 TIM4_Period, u16 CCR_B, u16 DIR_B);
void TIM8_GPIO_Config(u32 PulseNum_B);
void PWM_Output_B(u16 Cycle_B, u32 PulseNum_B, u16 DIR_B);
void TIM8_UP_TIM13_IRQHandler(void);

// Group C
void TIM3_GPIO_Config(u16 TIM3_Prescaler, u16 TIM3_Period, u16 CCR_C, u16 DIR_C);
void TIM5_GPIO_Config(u32 PulseNum_C);
void PWM_Output_C(u16 Cycle_C, u32 PulseNum_C, u16 DIR_C);
void TIM5_IRQHandler(void);

#endif	

/****************************END OF FILE****************************/

