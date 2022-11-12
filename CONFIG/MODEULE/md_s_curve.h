#ifndef __X_STEP_MOTOR_H
#define __X_STEP_MOTOR_H
#include "sys.h"
#include "math.h"				// exp()
#include <stdio.h>
#include "delay.h"
extern int X_Part_Count;
extern int Y_Part_Count;
extern int z_count;
extern u16 Pluse_High_z;
extern u16 temp_z;//temp_r=10时，脉冲频率f=1000,电机一圈1600pwm，转速约等于36rpm
#define  Pluse_Period_z  1000/temp_z

extern u16 Pluse_High_r;
extern u16 temp_r;//temp_r=10时，脉冲频率f=1000,电机一圈6400pwm，转速约等于9rpm
#define  Pluse_Period_r  1000/temp_r

// Motor Parameter
typedef struct{
	float X_Step;
	float X_Fre_Min;
	float X_Fre_Max;
	float X_Jerk;
}X_SpeedList_TypeDef;
#define		X_TIM3_FREQ							2000000		// 2MHz
#define		X_Accel_Step						1.0f
#define		X_SpeedList_LEN					((u8)X_Accel_Step)
#define		X_FREQ_MIN							500.0f
#define		X_FREQ_MAX							3500.0f
#define		X_JERK								4.0f
// Motor State
#define		X_ACCEL								1						// acceleration
#define		X_COSTT								2						// constant
#define     X_DECEL                             3						// deceleration
#define		X_UNIFM								4						// uniform
#define		X_STOP								0						// stop
#define		TRUE								1
#define		FALSE								0
// TIM3: CH1 - PA6\PA7脉冲, DIR - PA5\pa4方向  xy移动
#define		X_TIM3_Prescaler				1
#define		X_TIM3_Period					10000000
// TIM5: CH1 - PA0, DIR - PA1  上下             tim5 ch1 pwm pa0 上下
#define		X_TIM5_Prescaler				1
#define		X_TIM5_Period					4750
// TIM2: CH3 - PA2, DIR - PA3    电机转速       tim2 ch3 pwm pa2 转动
#define		X_TIM2_Prescaler				84
#define		X_TIM2_Period					2000
#define  UPWARD    1
#define  BACKWARD  0
// Calculate
void Calculate_SpeedList(u32 X_PulseNum);
// X
void bsp_ALL_GPIO_Init(void);			//GPIO
void bsp_XY_TIM3_Config(void);		// TIM
void Z_TIM5_Config(u32 Prescaler,u32 Period);
void z_init(void);
void TIM2_Config(u32 Prescaler,u32 Period);
void r_init(void);
void X_PWM_S_Output(int DIR);//控制x/y/z方向
void Y_PWM_S_Output(int DIR);
void Z_PWM_S_Output(int DIR,int pluse_high_z,int v_z);
void R_PWM_S_Output(int DIR,int pluse_high_r,int v_r);
#endif	/* __X_STEP_MOTOR_H */
/****************************END OF FILE****************************/

