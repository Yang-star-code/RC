#ifndef __FSM_H
#define __FSM_H
#define  ABS(x)      ((x)>0? (x):(-(x)))
#define  REVERSE(x)      ((x)>0? (-x):(x))
#define No  1
#define Ok  0
#define TIMX TIM3
#define TIMY TIM3
#define TIMZ TIM5


extern u8 Record_time;//�ǵ�ǰ��¼�ĵ�ĸ��� ��0��ʼ

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
    int Record_Close_Switch;//����Ok�����record  һ��ΪNo������reset,���򲻻��ټ�¼
    int Record_Switch;//��¼ģʽ��״̬����
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
    int V_X;//���x���ٶ�vx
    int V_Y;//���y���ٶ�vy
    int V_Z;//���z���ٶ�vz
    int V_W;//w��ת���ٶ�
}Motor_State;

typedef struct Position_Array
{
    int X_Position;
    int Y_Position;
    //ʾ�̴�С��ʾ�̷���
    int X_Show_Dir;
    int X_Show_Count;
    int Y_Show_Dir;
    int Y_Show_Count;


}Position_Array ;

typedef struct POINTS
{
		int Current_Height;             //��ͣ���ĸ߶�
    int Before_Style_Time;      //����ǰ��ͣ��ʱ�� ��Ϊ0 ��ͣ������ͣ���̵ܶ�ʱ��
    int Style_State;            //�Ƿ����� ��Ϊ0.....
    int Lifting_Speed;          //������ٶ�  z
    int After_Style_Time;       //�����ͣ����ʱ��

    Position_Array PosArray;    
}POINTS ;
void Point_Init(POINTS * Points_Ptr);
void Record_Position(POINTS * Record_Ptr);                            //�ṹ����������Ϊ��ǰ������׵�ַ 
void FSM(void *pvParameters);
void RobotStateInit(void);
void Velocity_Filter(int L_X,int L_Y,int R_Y);
void Send_Vel(int Vx,int Vy,int Vz,int Vw);
//�����Ȼ����ʾ
void Back_To_Start_Demo(POINTS * Back_Ptr);//������ķ����Ѿ��������ˣ����ڷ���ҲҪ������Calculate_SpeedList()
void Z_Action(int Current_Height,int Before_Style_Time,int Style_State,int Lifting_Speed,int After_Style_Time);
void Frequence_Change(int period,TIM_TypeDef* TIMx);//3 ��xyͨ��12  5��zͨ��1  2��ת��ͨ��3  mm/s   =50000/period
void XY_Action(int n);
void Hight_Control(int Current_Height,int Lifting_Speed);
#endif


