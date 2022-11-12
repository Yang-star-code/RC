#ifndef __OLED_I2C_H
#define	__OLED_I2C_H
#include "sys.h"



//-----------------OLED端口定义---------------- 

#define OLED_SCLK_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_6)
#define OLED_SCLK_Set() GPIO_SetBits(GPIOB,GPIO_Pin_6)//SCL

#define OLED_SDIN_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_7)//DIN
#define OLED_SDIN_Set() GPIO_SetBits(GPIOB,GPIO_Pin_7)
#define OLED_ADDRESS	0x78 //通过调整0R电阻,屏可以0x78和0x7A两个地址 -- 默认0x78
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

void I2C_Configuration(void);
void I2C_WriteByte(uint8_t addr,uint8_t data);
void OLED_Send_Cmd(unsigned char I2C_Command);
void OLED_Send_Data(unsigned char I2C_Data);
void OLED_Init(void);
void OLED_SetPos(unsigned char x, unsigned char y);
void OLED_Clear(void);
void OLED_ShowChar(u8 x,u8 y,u8 ch,u8 size);
void OLED_ShowString(u8 x,u8 y,u8 ch[],u8 size);
void OLED_ShowGB1616(u8 x0,u8 y0,const char p[],u8 size);
void OLED_ShowGB_String(u8 x0,u8 y0,const char *s,u8 size);
void OLED_ShowStr(u8 x,u8 y,const char *str,u8 size);
void OLED_ShowBMP(u8 x0,u8 y0,u8 x1,u8 y1,const u8 pic[]);
void OLED_Refresh(void);
void OLED_WR_Byte(unsigned dat,unsigned cmd);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_WaitAck(void); //测数据信号的电平
void Send_Byte(u8 dat);
void OLED_DisPlay_On(void);
void OLED_DisplayTurn(u8 i);
void OLED_DisPlay_Off(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1);
u32 OLED_Pow(u8 m,u8 n);
void OLED_ShowCHinese(u8 x,u8 y,u8 no);
void OLED_Set_Pos(unsigned char x, unsigned char y); 
void Write_IIC_Data(unsigned char IIC_Data);
void Write_IIC_Byte(unsigned char IIC_Byte);




#endif
