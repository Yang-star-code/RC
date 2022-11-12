#ifndef __FSM_H
#define __FSM_H
#define  ABS(x)      ((x)>0? (x):(-(x)))
#define  REVERSE(x)      ((x)>0? (-x):(x))
#define No  1
#define Ok  0
#define TIMX TIM3
#define TIMY TIM3
#define TIMZ TIM5


extern u8 Record_time;//是当前记录的点的个数 从0开始

typedef enum Moving_State
{
    STOP,
    MOBILE,
    RECORD,
    SHOW,
    Display_Speed,

}Moving_State;

typedef enum Velocity_Mode
{
    QUICK_MODE,
    FINE_TURNING_MODE,

}Velocity_Mode;

typedef struct Record_State
{
    int Record_Close_Switch;//等于Ok则可以record  一旦为No，除非reset,否则不会再记录
    int Record_Switch;//记录模式的状态开关
    int Show_Switch;
}Record_State;
typedef struct Robot_State
{
	Moving_State MovingState;
    Velocity_Mode VelocityMode;
    Record_State  RecordState;
}Robot_State;
typedef struct Motor_State
{
    int V_X;//电机x的速度vx
    int V_Y;//电机y的速度vy
    int V_Z;//电机z的速度vz
    int V_W;//w旋转的速度
}Motor_State;

typedef struct Position_Array
{
    int X_Position;
    int Y_Position;
    //示教大小和示教方向
    int X_Show_Dir;
    int X_Show_Count;
    int Y_Show_Dir;
    int Y_Show_Count;


}Position_Array ;

typedef struct POINTS
{
		int Current_Height;             //针停留的高度
    int Before_Style_Time;      //下针前的停留时间 若为0 则不停留或者停留很短的时间
    int Style_State;            //是否下针 若为0.....
    int Lifting_Speed;          //下针的速度  z
    int After_Style_Time;       //下针后停留的时间

    Position_Array PosArray;    
}POINTS ;
void Point_Init(POINTS * Points_Ptr);
void Record_Position(POINTS * Record_Ptr);                            //结构体数组名即为当前数组的首地址 
void FSM(void *pvParameters);
void RobotStateInit(void);
void Velocity_Filter(int L_X,int L_Y,int R_Y);
void Send_Vel(int Vx,int Vy,int Vz,int Vw);
//回起点然后演示
void Back_To_Start_Demo(POINTS * Back_Ptr);//数组给的方向已经反过来了，现在方向也要反过来Calculate_SpeedList()
void Z_Action(int Current_Height,int Before_Style_Time,int Style_State,int Lifting_Speed,int After_Style_Time);
void Frequence_Change(int period,TIM_TypeDef* TIMx);//3 是xy通道12  5是z通道1  2是转动通道3  mm/s   =50000/period
void XY_Action(int n);
void Hight_Control(int Current_Height,int Lifting_Speed);
#endif


