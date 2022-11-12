#ifndef  __can_H
#define  __can_H

#include "stm32f4xx.h"

// M3508电机编号
#define M3508_CHASSIS_MOTOR_ID_1 0x201
#define M3508_CHASSIS_MOTOR_ID_2 0x202
#define M3508_CHASSIS_MOTOR_ID_3 0x203
#define M3508_CHASSIS_MOTOR_ID_4 0x204


// M3508返回电机的真实信息
typedef struct M3508_REAL_INFO
{
	uint16_t ANGLE;   								
	int16_t  RPM;								
	int16_t  CURRENT;//前三项是根据手册里的数据
	int16_t  TARGET_CURRENT;
	float  REAL_ANGLE;
}M3508_REAL_INFO;

extern M3508_REAL_INFO M3508_CHASSIS_MOTOR_REAL_INFO[4];
void m3508_update_m3508_info(CanRxMsg *msg);
void M3508_Send_Motor_Currents(void);


void CAN1_init(void);
void CAN_motor_init(void);

#endif
