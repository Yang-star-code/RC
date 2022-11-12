#ifndef  __ADS1115_H
#define  __ADS1115_H

/***************************************************************************************
*˵������������ʱ��������Ϊ�����źţ�������Ϊ�ص��������źŲ���Ϊ����ѹ(���ܱȵص�λ��)
*       ˫������ʱ��������Ϊ�����źţ�������Ϊ�����������źŵĲ�ֵ����Ϊ����ѹ
****************************************************************************************/

//#define ADS1115_ADDR 0x90  //ADDR PIN ->GND
//#define CMD_Write 0x90//д�Ĵ���
//#define CMD_Read 	0x91//���Ĵ���
#define ADS1115_SCL    PBout(6) //SCL
#define ADS1115_SDA    PBout(7) //SDA
#define     SDA_AI        PBin(7)   //SDA����
////��ַָ��Ĵ���
//#define CMD_CONF_REG 0x01
//#define CONF_L 0xe3
//#define CMD_POINT_REG 0x00//ת�����ݼĴ���

//I2C��ַ�Լ���д����
#define  WR_REG 0x90       //д�Ĵ���
#define  RE_REG 0x91       //���Ĵ���
 
/***********************************�Ĵ���������**********************************************/
#define  DATA_REG  0x00        //ת�����ݼĴ���
#define  CONF_REG  0x01     //���������üĴ���
#define  LOTH_REG  0x02        //��ͷ�ֵ�Ĵ���
#define  HITH_REG  0x03        //��߷�ֵ�Ĵ���
 
#define  ch0  0xc0       //ͨ��0
#define  ch1  0xd0       //ͨ��1
#define  ch2  0xe0       //ͨ��2
#define  ch3  0xf0       //ͨ��3
 
/***********************����������*************************************************************
*|  OS | MUX2 | MUX1 | MUX0 | PGA2 | PGA1 | PGA0 | MODE  |------HCMD
*|  DR2| DR1  | DR0  | COMP_MODE | COMP_POL | COMP_LAT |  COMP_QUE1 | COMP_QUE0 |-----LCMD
***********************************************************************************************/
#define  HCMD1    0x64   //AIN0�������� +-4.096����  ����ģʽ  01000100b
#define  LCMD1      0xf0     //860sps ���ڱȽ���ģʽ �������Ч  �������ź����� ÿ���ڼ�ֵⷧ 11110000b
 
 

static void ADS1115_delay(u16 D);
void delay_nms(u16 ms);
void bsp_ADS1115_Init(void);
void ADS1115Start_A(void);
void ADS1115Config_A(u8 LCMD,u8 HCMD);
u16 ReadAD_A(void);
u16 getad(u8 LCMD,u8 HCMD);
u16 lvbo(u8 LCMD,u8 HCMD);

#endif

