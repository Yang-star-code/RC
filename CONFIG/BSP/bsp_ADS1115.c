#include "include.h"
//������������ʱ
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

//I2C��������
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

//I2Cֹͣ����
void ADS1115Stop_A(void)
{
   ADS1115_SDA=0;
   ADS1115_delay(575);
   ADS1115_SCL=1;    
   ADS1115_delay(575);
   ADS1115_SDA=1;
   ADS1115_delay(575);//MIN 160ns
}


//I2C дһ�ֽ�
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
         ADS1115_SCL=1;    ;//�����ֲ᲻����ʱ
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
 
 
//I2C ��һ�ֽ�
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
*��������:  ADSConfig
*��       ���� ����ADS����ͨ�����ã�����ʱ��ȵ�
*��       ���� HCMD �������ָ�8λ(ͨ�������̣�ת��ģʽ)
            LCMD : �����ֵ�8λ(���������� �Ƚ�ģʽ ��Ч��ƽ �ź��������)
*��       �أ� ��
********************************************************************/
void ADS1115Config_A(u8 LCMD,u8 HCMD)
{
    u8 i=0;
    u8 Initdata[4];
 
    Initdata[0] = 0x90;  // ��ַ0x90  ����ADR�ӵ� д�Ĵ���
    Initdata[1] = 0x01;// ���üĴ���
    Initdata[2] = HCMD;    // �����ָ��ֽ�
    Initdata[3] = LCMD;    // �����ֵ��ֽ�
    ADS1115_SCL=1;    ;
    ADS1115Start_A();        //����
    for(i=0;i<4;i++)
    {
        ADS1115WriteByte_A(Initdata[i]);
        ADS1115_delay(30);
    }
    ADS1115Stop_A();         //�ر�
}

/*******************************************************************
*��������:  ReadAD_A
*��       ���� ��ȡADת����ֵ
*��       ���� ��ȡ��ֵΪ��ǰ�����õ��Ǹ�ͨ��
*��       �أ� ��
********************************************************************/
u16 ReadAD_A(void)
{
   u16 Data[2]={0,0};
//ת��ָ��Ĵ���
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
    ADS1115Config_A(LCMD,HCMD);          //����ͨ��
        
    delay_nms(15); // ��ʱһ��ʱ�䣬��ֹͨ���л�����Ӱ��    55555555555    
    value=ReadAD_A();
    return value;
}
 
u16 lvbo(u8 LCMD,u8 HCMD)        //��30��ֵ��ƽ��ֵ
{
    u8 k;
    u32 U=0, temp;   //u32 �������ӿռ� ����float��double��� 
    for(k=0;k<30;k++)
    {
        U+=getad(LCMD,HCMD);    
    }
    temp=U;
        U=0;
    return ((float)temp/30);    //����С����
}


    




