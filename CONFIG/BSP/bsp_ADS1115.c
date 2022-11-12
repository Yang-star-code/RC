#include "include.h"
//经常被用来延时
static void ADS1115_delay(u16 D)
{
    while(--D);
}

void delay_nms(u16 ms)
{
    u16 i;
    u32 M = 0;//720W
    for(i = 0;i < ms; i++)
    for(M=12000;M > 0;M--);
}

void bsp_ADS1115_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB ,ENABLE);    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType=GPIO_OType_OD;  
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    ADS1115_SCL=1;    
	ADS1115_SDA=1;
    delay_nms(75);//55555555  
}

//I2C总线启动
void ADS1115Start_A(void)
{
   ADS1115_SDA=1;
   ADS1115_delay(575);
   ADS1115_SCL=1;    
   ADS1115_delay(575);
   ADS1115_SDA=0;
   ADS1115_delay(575);//MIN 160ns
   ADS1115_SCL=0;    
   ADS1115_delay(575);
}

//I2C停止总线
void ADS1115Stop_A(void)
{
   ADS1115_SDA=0;
   ADS1115_delay(575);
   ADS1115_SCL=1;    
   ADS1115_delay(575);
   ADS1115_SDA=1;
   ADS1115_delay(575);//MIN 160ns
}


//I2C 写一字节
void ADS1115WriteByte_A(u8 DATA)
{
      u8 i;
      ADS1115_SCL=0;    ;
      for(i = 0;i < 8; i++)
      {
         if(DATA&0x80)
         {
              ADS1115_SDA=1;
         }
         else
         {
              ADS1115_SDA=0;
         }
         ADS1115_SCL=1;    ;//按照手册不需延时
         ADS1115_delay(30);
         ADS1115_SCL=0;    ;
         ADS1115_delay(30);
         DATA    = DATA << 1;  
      }
      ADS1115_SDA=1;
      ADS1115_SCL=1;    
      ADS1115_delay(30);
      ADS1115_SCL=0;    
}
 
 
//I2C 读一字节
u8 ADS1115_ReadByte_A(void)
{
    u8 TData=0,i;
    for(i=0;i<8;i++)
    {
        ADS1115_SCL=1;    
        ADS1115_delay(30);
        TData=TData<<1;
        if(SDA_AI)
        {
            TData|=0x01;
        }
        ADS1115_SCL=0;    
        ADS1115_delay(30);
    }
    ADS1115_SCL=0;    
    ADS1115_delay(30);
    ADS1115_SDA=0;
    ADS1115_delay(30);
    ADS1115_SCL=1;    
    ADS1115_delay(30);
    ADS1115_SCL=0;    
    ADS1115_delay(30);
    ADS1115_SDA=1;
    return TData;
}


/*********************************************************************
*函数名称:  ADSConfig
*描       述： 设置ADS包括通道配置，采样时间等等
*参       数： HCMD ：命令字高8位(通道，量程，转换模式)
            LCMD : 命令字低8位(采样率设置 比较模式 有效电平 信号输出锁存)
*返       回； 无
********************************************************************/
void ADS1115Config_A(u8 LCMD,u8 HCMD)
{
    u8 i=0;
    u8 Initdata[4];
 
    Initdata[0] = 0x90;  // 地址0x90  器件ADR接地 写寄存器
    Initdata[1] = 0x01;// 配置寄存器
    Initdata[2] = HCMD;    // 配置字高字节
    Initdata[3] = LCMD;    // 配置字低字节
    ADS1115_SCL=1;    ;
    ADS1115Start_A();        //开启
    for(i=0;i<4;i++)
    {
        ADS1115WriteByte_A(Initdata[i]);
        ADS1115_delay(30);
    }
    ADS1115Stop_A();         //关闭
}

/*******************************************************************
*函数名称:  ReadAD_A
*描       述： 获取AD转换的值
*参       数： 获取的值为在前面设置的那个通道
*返       回； 无
********************************************************************/
u16 ReadAD_A(void)
{
   u16 Data[2]={0,0};
//转换指向寄存器
   ADS1115_SCL=1;    ;
   ADS1115Start_A();
   ADS1115WriteByte_A(0x90);
   ADS1115WriteByte_A(0x00);
   ADS1115Stop_A();                        
   
   ADS1115Start_A();
   ADS1115WriteByte_A(0x91);
   Data[0] = ADS1115_ReadByte_A();
   Data[1] = ADS1115_ReadByte_A();
   ADS1115Stop_A();
   
   Data[0] = Data[0]<<8 | Data[1];
   return  (Data[0]);//&0x7fff
}
 
u16 getad(u8 LCMD,u8 HCMD)
{
   u16 value=0;
    ADS1115Config_A(LCMD,HCMD);          //配置通道
        
    delay_nms(15); // 延时一定时间，防止通道切换互相影响    55555555555    
    value=ReadAD_A();
    return value;
}
 
u16 lvbo(u8 LCMD,u8 HCMD)        //求30个值的平均值
{
    u8 k;
    u32 U=0, temp;   //u32 给够叠加空间 或者float、double亦可 
    for(k=0;k<30;k++)
    {
        U+=getad(LCMD,HCMD);    
    }
    temp=U;
        U=0;
    return ((float)temp/30);    //带上小数点
}


    




