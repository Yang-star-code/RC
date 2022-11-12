#include "include.h"
#include "picture.h"
Robot_State RobotState;
Motor_State MotorState;
POINTS record_points[10] ;             //定义一个结构体数组，存放点的坐标、以及各种成员变量
POINTS *RECORD_PTR = &record_points[0];    //使一个指针指向结构体数组名（代表数组首元素地址） 用来代替操作  记录指针
POINTS *BACK_PTR = NULL;        //回城指针 先指向空
u8 Record_time=0;               //是当前有效记录的点的个数 从0开始
extern int Y_Part_Count;
extern int X_Part_Count;
extern int X_Total_Count;
extern int Y_Total_Count;
float Initial_Height=3;//初始高度
float total_time;

float i=1.0;
//Current_Height（0-50）   Before_Style_Time   Style_State 		Lifting_Speed 				After_Style_Time 
//针距离孔的高度				下降前停留时间		是否下降				下降速度	  	  		下降后停留时间
POINTS record_points[10] = 
{
	//50,		2000,		0,		100,		0,		{0},
	
	13,		2000,		0,		100,		2000,		{0},  //  c
	
	40,		2000,		1,		100,		2000,		{0},  // b-

	10,		2000,		0,		100,		2000,		{0},  //b-
	
	10,		2000,		1,		100,		2000,		{0},  //b
	
	10,		2000,		1,		100,		2000,		{0},//a
	
	10,		2000,		0,		100,		2000,		{0},//d- 
	
	10,		2000,		0,		100,		2000,		{0},//d- 
	
	20,		2000,		1,		100,		2000,		{0},//d 
	
	10,		2000,		0,		100,		2000,		{0},//c


};
void XY_Action(int n)
{
	//到达位置
	Calculate_SpeedList(RECORD_PTR[n].PosArray.X_Show_Count/2);
  X_PWM_S_Output(RECORD_PTR[n].PosArray.X_Show_Dir);
  vTaskDelay(500);
  Calculate_SpeedList(RECORD_PTR[n].PosArray.Y_Show_Count/2);
  Y_PWM_S_Output(RECORD_PTR[n].PosArray.Y_Show_Dir);
	vTaskDelay(500);
	//下针前等待时间;是否下针;下针速度;下针后等待时间
	Z_Action(RECORD_PTR[n].Current_Height,RECORD_PTR[n].Before_Style_Time, RECORD_PTR[n].Style_State, RECORD_PTR[n].Lifting_Speed, RECORD_PTR[n].After_Style_Time);
}
void RobotStateInit(void)
{
   RobotState.MovingState=STOP;
   RobotState.RecordState.Record_Close_Switch=Ok;
   RobotState.RecordState.Record_Switch=Ok;
   RobotState.RecordState.Show_Switch=Ok;
    //停止移动任务
}

void FSM(void *pvParameters)
{
    while(1)
    {
			//i=lvbo(0xeb,0x82);
			//i++;
    if(AIR_R_SHORT==1000)
    {
        RobotState.MovingState=STOP;//开机按键上拨默认停止状态
    }
//--------------------------------------FSM
    else if(AIR_R_SHORT==2000&&AIR_R_LONG==1000)
    {
        RobotState.MovingState=MOBILE;
        //钻头和移动任务均开始

    }
//--------------------------------------FSM
    else if(AIR_R_SHORT>=1900&&AIR_R_LONG==1500)
    {
        RobotState.MovingState=RECORD;
        //记录一个位置,共三个位置，结构体数组 x y 
    }
//--------------------------------------FSM
    else if(AIR_L_LONG==1000)
    {
        RobotState.VelocityMode=QUICK_MODE;
			RobotState.RecordState.Show_Switch=Ok;
    }
//--------------------------------------FSM
    else if(AIR_L_LONG==1500)
    {
        RobotState.VelocityMode=FINE_TURNING_MODE;
			RobotState.RecordState.Show_Switch=Ok;
    }
//--------------------------------------FSM
    else if(AIR_L_LONG>=1900)
    {
        RobotState.MovingState=SHOW;//根据记录点，顺序跑完位置
        RobotState.RecordState.Record_Close_Switch=No;
    }
		if(AIR_R_SHORT>=1900&&AIR_R_LONG>=1900)
    {
        RobotState.MovingState=Display_Speed;//只开启显示速度和调节自转速度的任务
    }
		else 
		{
			vTaskSuspend(Information_Handle);
			Z_PWM_S_Output(UPWARD,0,100);//方向 高电平时间（时间为0时电机停止） 速度（10-100）
			R_PWM_S_Output(BACKWARD,0,0);
		}
//--------------------------------------FSM
    
    //RobotState.MovingState=MOBILE;
    switch(RobotState.MovingState)
    {
        case STOP://xyzw电机速度都为0
        {
            MotorState.V_X=0;
            MotorState.V_Y=0;
            MotorState.V_Z=0;
            MotorState.V_W=0;   
        }break;
//--------------------------------------Switch        
        case MOBILE://搞一个获取按键值的函数，根据按键值来得到电机的速度(频率) 
        {
          RobotState.RecordState.Record_Switch=Ok;//清零之后才可以进入RECORD状态
         switch (RobotState.VelocityMode)
         {
             case QUICK_MODE:
             {
             //啥也不做
             }break;
             case FINE_TURNING_MODE:
             {
                vTaskDelay(50);//等待移动摇杆            
             }break;
         }
        Velocity_Filter(ROCK_L_X,ROCK_L_Y,ROCK_R_Y);//速度过滤并赋值
        Send_Vel(MotorState.V_X,MotorState.V_Y,MotorState.V_Z,MotorState.V_W);//把赋值后的电机速度发送出去  中断会发送脉冲
        }break;
//--------------------------------------Switch
                
        case RECORD:
//如果进入此状态，则判断某个长按键L_LONG的状态，如果超过人为计数值，则停止记位置，而且一旦记录完之后就要先把R_LONG拨到最上面才可以再次记录            
        {
            if(RobotState.RecordState.Record_Switch==Ok&&RobotState.RecordState.Record_Close_Switch==Ok)
            {
            RobotState.RecordState.Record_Switch=No;                //防止拨了按键之后又一直记录 手动之后又可以记录
            Record_Position(RECORD_PTR);                            //结构体数组名即为当前数组的首地址
            }
   
        }break;
//--------------------------------------Switch 
        case SHOW:
        {
            if(RobotState.RecordState.Show_Switch==Ok)
        {
            int n;
            Back_To_Start_Demo(&RECORD_PTR[Record_time-1]);//最后记录完还加了一，所以要减一  先回程,adjust for x,y,to the initial
						vTaskDelay(500);//The pause time of each point.
            for(n=0;n<Record_time;n++)
					{
						 XY_Action(n);//
						 vTaskDelay(500);
					}
					
					RobotState.RecordState.Show_Switch=No;
        }
        
        }break;
				case Display_Speed:
				{
					vTaskResume(Information_Handle);
				}break;
//--------------------------------------Switch
        
    }
    vTaskDelay(5);
    }
}

/**
  * @brief (函数功能简介)  改变点的各种成员变量的函数
  * @note  (重点摘要)   
  * @param (写入的参数)
  *            @arg (参数具体值) 
  *            @arg (参数具体值)
  *            @arg (参数具体值)
  * @param            
  * @retval None
  */
    void Point_Change(POINTS * Points_Ptr)
{


}


//======================================================================手动区
/**
  * @brief (函数功能简介)把航模遥控的摇杆转换为电机速度过滤并赋值
  * @note  (重点摘要)   
  * @param (写入的参数)
  *            @arg (参数具体值) 
  *            @arg (参数具体值)
  *            @arg (参数具体值)
  * @param            
  * @retval None
  */

void Velocity_Filter(int L_X,int L_Y,int R_Y)
{   
    if(ABS(L_X-1500)>100)
        {
            MotorState.V_X=L_X-1500;
        }
        else
        {MotorState.V_X=0;}
        if(ABS(L_Y-1500)>100)
        {
            MotorState.V_Y=L_Y-1500;
        }
        else
        {MotorState.V_Y=0;} 
        if(ABS(R_Y-1500)>100)
        {
            MotorState.V_Z=R_Y-1500;
        } 
        else
        {MotorState.V_Z=0;}
        MotorState.V_W=0;//自转速度为0



}

void Send_Vel(int Vx,int Vy,int Vz,int Vw)//只是判断而已
{
  if(Vx>0)//往右运动
  {
    X_PWM_S_Output(UPWARD);  
  }
 if(Vx<0)
  {
    X_PWM_S_Output(BACKWARD);
  }
 if(Vy>0)//向上
  {
    Y_PWM_S_Output(UPWARD);

  } 
 if(Vy<0)
  {
    Y_PWM_S_Output(BACKWARD);       
  }
  
 if(Vz>0)
  {
	//	vTaskDelay(100);
  	Z_PWM_S_Output(UPWARD,10,100);
		vTaskDelay(50); 
		Z_PWM_S_Output(UPWARD,0,0);
	}
  
 if(Vz<0)
  {
//		vTaskDelay(100);
  	Z_PWM_S_Output(BACKWARD,10,100);
		vTaskDelay(50); 
		Z_PWM_S_Output(BACKWARD,0,0);
	}
}

////======================================================================自动区

void Record_Position(POINTS * Record_Ptr)
{
              //记录每次点的位置和方向
              Record_Ptr[Record_time].PosArray.X_Position   = X_Total_Count;        //每次记录要记录一个全局的距离 回程时使用
              Record_Ptr[Record_time].PosArray.X_Show_Count = ABS(X_Part_Count);    //示教数组计数值要保证为正 
              Record_Ptr[Record_time].PosArray.Y_Position   = Y_Total_Count;
              Record_Ptr[Record_time].PosArray.Y_Show_Count = ABS(Y_Part_Count);
                if(X_Part_Count>0)//局部方向，可以判断每次该往哪里走
                    {
                        Record_Ptr[Record_time].PosArray.X_Show_Dir = UPWARD;
                        X_Part_Count=0;                                 //当前局部计数值清零
                    }
                else if(X_Part_Count<0)
                    {
                        Record_Ptr[Record_time].PosArray.X_Show_Dir = BACKWARD;
                        X_Part_Count=0;
                    }
            
                if(Y_Part_Count>0)
                    {
                        Record_Ptr[Record_time].PosArray.Y_Show_Dir = UPWARD;
                        Y_Part_Count=0;
                    }
                else if(Y_Part_Count<0)
                    {
                        Record_Ptr[Record_time].PosArray.Y_Show_Dir = BACKWARD;
                        Y_Part_Count=0;
                    }
//                  ++Record_Ptr;     //指针自己移动好像不行
                    Record_time++;      //用来当数组有效长度计数值  
}


void Back_To_Start_Demo(POINTS * Back_Ptr)//传入最后一个点的地址 因为最后一个点的坐标是最新的X_Total_Count Y_Total_Count
{
    if(Back_Ptr->PosArray.X_Position>0)
       {
                Calculate_SpeedList(ABS(Back_Ptr->PosArray.X_Position)/2);
                X_PWM_S_Output(BACKWARD);
       }
       if (Back_Ptr->PosArray.X_Position<0)
       {        
                Calculate_SpeedList(ABS(Back_Ptr->PosArray.X_Position)/2);
                X_PWM_S_Output(UPWARD);
       }
       vTaskDelay(50);
       if(Back_Ptr->PosArray.Y_Position>0)
       {         
                Calculate_SpeedList(ABS(Back_Ptr->PosArray.Y_Position)/2);//这个可以为负  回城
                Y_PWM_S_Output(BACKWARD);
       }
       if (Back_Ptr->PosArray.Y_Position<0)
       {

                Calculate_SpeedList(ABS(Back_Ptr->PosArray.Y_Position)/2);
                Y_PWM_S_Output(UPWARD);             
       }
		
       
}
//钻孔，Current_Height=18，刚到孔，Current_Height=30
//高度8cm，下降速度100（即360rpm）时，1.6秒走完，下降完需要9.6圈
//total_time=9.6/3.6/Lifting_Speed*60*1000
//高度控制函数（高度范围：18-80mm）
void Hight_Control(int Current_Height,int Lifting_Speed)
{
	total_time=9.6/3.6/Lifting_Speed*60*1000;
	int adjust_time;
//	Initial_Height=0;
	if(Current_Height>Initial_Height)
	{
		adjust_time=total_time*(Current_Height-Initial_Height)/80;
		Z_PWM_S_Output(UPWARD,10,Lifting_Speed);
		R_PWM_S_Output(BACKWARD,10,50);
		vTaskDelay(adjust_time);
		Z_PWM_S_Output(UPWARD,0,0);
		R_PWM_S_Output(BACKWARD,0,0);
	}
	else if(Current_Height<Initial_Height)
	{
		adjust_time=total_time*(Initial_Height-Current_Height)/80;
		Z_PWM_S_Output(BACKWARD,10,Lifting_Speed);
		R_PWM_S_Output(BACKWARD,10,50);
		vTaskDelay(adjust_time);
		Z_PWM_S_Output(BACKWARD,0,0);
		R_PWM_S_Output(BACKWARD,0,0);
	}
	  Initial_Height=Current_Height;
}

void Z_Action(int Current_Height,int Before_Style_Time,int Style_State,int Lifting_Speed,int After_Style_Time)//Lifting_Speed=100
{
	float Initial_Time;
	int pre_time,current_height;
	current_height=Current_Height+30;
	Hight_Control(current_height,Lifting_Speed);
	pre_time=total_time/80*(current_height-25);
	Initial_Time=total_time/80*25;
	if(Style_State==1)
	{
		vTaskDelay(Before_Style_Time);
		vTaskDelay(100);
		Z_PWM_S_Output(BACKWARD,10,Lifting_Speed);
		R_PWM_S_Output(BACKWARD,10,50);
		vTaskDelay(pre_time);
		Z_PWM_S_Output(BACKWARD,0,0);
		vTaskDelay(After_Style_Time);//wait 2s and lift
		Z_PWM_S_Output(UPWARD,10,Lifting_Speed);
		vTaskDelay(Initial_Time);
		Z_PWM_S_Output(UPWARD,0,0);
		R_PWM_S_Output(UPWARD,0,0);
		Initial_Height=10;
	}
	else
	{
		R_PWM_S_Output(BACKWARD,10,50);
		vTaskDelay(Before_Style_Time);
		R_PWM_S_Output(UPWARD,0,0);
		vTaskDelay(100);
	}
	
}

