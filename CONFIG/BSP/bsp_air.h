#ifndef  __bsp_air_H
#define  __bsp_air_H
typedef struct
{
	struct  //遥控原始数据，8通道
	{
	 uint16_t roll;			//右摇杆
	 uint16_t pitch;		//
	 uint16_t thr;
	 uint16_t yaw;
	 uint16_t AUX1;
	 uint16_t AUX2;
	 uint16_t AUX3;
	 uint16_t AUX4; 
	}Remote; 

}Air_Contorl;

#define AIR_L_SHORT		Device.Remote.AUX4				//AUX4 1000~2000                //坏掉了
#define AIR_L_LONG		Device.Remote.AUX3				//AUX2 1000-1500-2000           //上键是1000    
#define ROCK_R_X		Device.Remote.AUX1				//AUX1 1000~2000                //上键是1000    
#define AIR_R_LONG		Device.Remote.AUX2				//AUX3 1000-1500-2000           //上键是1000
#define ROCK_L_X			Device.Remote.yaw			//YAW  1000-1500-2000           //右大左小     
#define ROCK_L_Y			Device.Remote.thr			//THR  1000-1500-2000           //下大上小

#define AIR_R_SHORT			Device.Remote.roll			//ROLL 1000-1500-2000           //没有配置 无信号 switch once
#define ROCK_R_Y			Device.Remote.pitch			//PITCH 1000-1500-2000P         //上大下小

void bsp_Air_Init(void);
u8 TIM3_TIM5_TIM8_PWM_Out_Init(uint16_t hz,u8 init_duty);//400hz

extern Air_Contorl  Device;

#endif
