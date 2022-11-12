#ifndef  __ADS1115_H
#define  __ADS1115_H

/***************************************************************************************
*说明：当端输入时候正输入为输入信号，负输入为地但是输入信号不能为负电压(不能比地电位低)
*       双端输入时候正输入为输入信号，负输入为负输入输入信号的差值可以为负电压
****************************************************************************************/

//#define ADS1115_ADDR 0x90  //ADDR PIN ->GND
//#define CMD_Write 0x90//写寄存器
//#define CMD_Read 	0x91//读寄存器
#define ADS1115_SCL    PBout(6) //SCL
#define ADS1115_SDA    PBout(7) //SDA
#define     SDA_AI        PBin(7)   //SDA读入
////地址指针寄存器
//#define CMD_CONF_REG 0x01
//#define CONF_L 0xe3
//#define CMD_POINT_REG 0x00//转换数据寄存器

//I2C地址以及读写设置
#define  WR_REG 0x90       //写寄存器
#define  RE_REG 0x91       //读寄存器
 
/***********************************寄存器控制字**********************************************/
#define  DATA_REG  0x00        //转换数据寄存器
#define  CONF_REG  0x01     //控制字设置寄存器
#define  LOTH_REG  0x02        //最低阀值寄存器
#define  HITH_REG  0x03        //最高阀值寄存器
 
#define  ch0  0xc0       //通道0
#define  ch1  0xd0       //通道1
#define  ch2  0xe0       //通道2
#define  ch3  0xf0       //通道3
 
/***********************控制字申明*************************************************************
*|  OS | MUX2 | MUX1 | MUX0 | PGA2 | PGA1 | PGA0 | MODE  |------HCMD
*|  DR2| DR1  | DR0  | COMP_MODE | COMP_POL | COMP_LAT |  COMP_QUE1 | COMP_QUE0 |-----LCMD
***********************************************************************************************/
#define  HCMD1    0x64   //AIN0单端输入 +-4.096量程  连续模式  01000100b
#define  LCMD1      0xf0     //860sps 窗口比较器模式 输出低有效  不锁存信号至读 每周期检测阀值 11110000b
 
 

static void ADS1115_delay(u16 D);
void delay_nms(u16 ms);
void bsp_ADS1115_Init(void);
void ADS1115Start_A(void);
void ADS1115Config_A(u8 LCMD,u8 HCMD);
u16 ReadAD_A(void);
u16 getad(u8 LCMD,u8 HCMD);
u16 lvbo(u8 LCMD,u8 HCMD);

#endif

