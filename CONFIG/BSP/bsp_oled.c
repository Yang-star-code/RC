#include "bsp_oled.h"
#include "font.h"
#include "delay.h"
u8 OLED_GRAM[128][8];
void bsp_I2C_OLED_Init(void)
{
	I2C_InitTypeDef  I2C_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure; 

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);

	/*STM32F103C8T6芯片的硬件I2C: PB6 -- SCL; PB7 -- SDA */ //pb6 7  /8 9都可以
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8| GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//I2C必须开漏输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_I2C1);
	
	I2C_DeInit(I2C1);//使用I2C1
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x30;//主机的I2C地址,随便写的
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 400000;//400K

	I2C_Cmd(I2C1, ENABLE);
	I2C_Init(I2C1, &I2C_InitStructure);
}

void I2C_WriteByte(uint8_t addr,uint8_t data)
{
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
	
	I2C_GenerateSTART(I2C1, ENABLE);//开启I2C1
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));/*EV5,主模式*/

	I2C_Send7bitAddress(I2C1, OLED_ADDRESS, I2C_Direction_Transmitter);//器件地址 -- 默认0x78
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(I2C1, addr);//寄存器地址
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_SendData(I2C1, data);//发送数据
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_GenerateSTOP(I2C1, ENABLE);//关闭I2C1总线
}

void OLED_Send_Cmd(unsigned char I2C_Command)//写命令
{
	I2C_WriteByte(0x00, I2C_Command);
}

void OLED_Send_Data(unsigned char I2C_Data)//写数据
{
	I2C_WriteByte(0x40, I2C_Data);
}

void bsp_OLED_Init(void)
{
	delay_ms(100); //这里的延时很重要
	
	OLED_Send_Cmd(0xAE); //display off
	OLED_Send_Cmd(0x20);	//Set Memory Addressing Mode	
	OLED_Send_Cmd(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	OLED_Send_Cmd(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
	OLED_Send_Cmd(0xc8);	//Set COM Output Scan Direction
	OLED_Send_Cmd(0x00); //---set low column address
	OLED_Send_Cmd(0x10); //---set high column address
	OLED_Send_Cmd(0x40); //--set start line address
	OLED_Send_Cmd(0x81); //--set contrast control register
	OLED_Send_Cmd(0xff); //亮度调节 0x00~0xff
	OLED_Send_Cmd(0xa1); //--set segment re-map 0 to 127
	OLED_Send_Cmd(0xa6); //--set normal display
	OLED_Send_Cmd(0xa8); //--set multiplex ratio(1 to 64)
	OLED_Send_Cmd(0x3F); //
	OLED_Send_Cmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	OLED_Send_Cmd(0xd3); //-set display offset
	OLED_Send_Cmd(0x00); //-not offset
	OLED_Send_Cmd(0xd5); //--set display clock divide ratio/oscillator frequency
	OLED_Send_Cmd(0xf0); //--set divide ratio
	OLED_Send_Cmd(0xd9); //--set pre-charge period
	OLED_Send_Cmd(0x22); //
	OLED_Send_Cmd(0xda); //--set com pins hardware configuration
	OLED_Send_Cmd(0x12);
	OLED_Send_Cmd(0xdb); //--set vcomh
	OLED_Send_Cmd(0x20); //0x20,0.77xVcc
	OLED_Send_Cmd(0x8d); //--set DC-DC enable
	OLED_Send_Cmd(0x14); //
	OLED_Send_Cmd(0xaf); //--turn on oled panel
	OLED_Clear();
	OLED_SetPos(0,0);
}

void OLED_SetPos(unsigned char x, unsigned char y) //设置起始点坐标
{ 
	OLED_Send_Cmd(0xb0+y);
	OLED_Send_Cmd(((x&0xf0)>>4)|0x10);
	OLED_Send_Cmd((x&0x0f)|0x01);
}

void OLED_Clear(void)//全屏填充
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		OLED_Send_Cmd(0xb0+m);		//page0-page1
		OLED_Send_Cmd(0x00);		//low column start address
		OLED_Send_Cmd(0x10);		//high column start address
		for(n=0;n<128;n++)
			{
				OLED_Send_Data(0);
			}
	}
}



//--------------------------------------------------------------
// Prototype      : void OLED_ON(void)
// Calls          : 
// Parameters     : none
// Description    : 将OLED从休眠中唤醒
//--------------------------------------------------------------
void OLED_ON(void)
{
	OLED_Send_Cmd(0X8D);  //设置电荷泵
	OLED_Send_Cmd(0X14);  //开启电荷泵
	OLED_Send_Cmd(0XAF);  //OLED唤醒
}

//--------------------------------------------------------------
// Prototype      : void OLED_OFF(void)
// Calls          : 
// Parameters     : none
// Description    : 让OLED休眠 -- 休眠模式下,OLED功耗不到10uA
//--------------------------------------------------------------
void OLED_OFF(void)
{
	OLED_Send_Cmd(0X8D);  //设置电荷泵
	OLED_Send_Cmd(0X10);  //关闭电荷泵
	OLED_Send_Cmd(0XAE);  //OLED休眠
}

void OLED_ShowChar(u8 x,u8 y,u8 ch,u8 size)
{
	u8 i,j;
	u16 cnt=0;
	if(x>127){x=0;y+=2;}
	for(j=0;j<64/8;j++)								//y最大坐标64  共有8页
	{
		if(size/8.0<=j*1.0)return;
		OLED_SetPos(x,y/8+j);
		for(i=0;i<size/2;i++)
		{
			if(size==12)OLED_Send_Data(ASCII_6_12[ch-' '][cnt++]);
			else if(size==16)OLED_Send_Data(ASCII_8_16[ch-' '][cnt++]);
			else if(size==24)OLED_Send_Data(ASCII_12_24[ch-' '][cnt++]);
			else return;
		}
	}
	
}

void OLED_ShowString(u8 x,u8 y,u8 ch[],u8 size)
{
	u8 i=0;
	while(ch[i]!='\0')
	{
		OLED_ShowChar(x,y,ch[i],size);
		i++;
		x+=size/2;
	}
}

void OLED_ShowGB_Char(u8 x0,u8 y0,const char p[],u8 size)
{
	u8 i,j,k;
	u16 cnt=0;
	for(i=0;i<GB_Num;i++)
	{
		if((p[0]==GB_1616[i].index[0])&&(p[1]==GB_1616[i].index[1]))
		{
			for(k=0;k<2;k++)
			{
				if(size/8.0<=k*1.0)return;
				OLED_SetPos(x0,y0/8+k);
				for(j=0;j<size;j++)
				{
					if(size==12)
						OLED_Send_Data(GB_1212[i].matrix[cnt++]);
					else if(size==16)
						OLED_Send_Data(GB_1616[i].matrix[cnt++]);
					else return;
				}
			}
			return;			//找到对应的汉字不用在找了
		}
	}
}


void OLED_ShowGB_String(u8 x0,u8 y0,const char *s,u8 size)
{
	u16 i=0;
	while(*s!='\0')
	{
		OLED_ShowGB_Char(x0+i,y0,s,size);
		i+=size;
		s+=2;
	}
}

void OLED_ShowStr(u8 x,u8 y,const char *str,u8 size)
{												  	  
	u8 bHz=0;     //字符或者中文  	    				    				  	  
	while(*str!=0)//数据未结束
	{ 
			if(!bHz)
			{
				if(*str>0x80)bHz=1;//中文 
				else              //字符
				{            
				  OLED_ShowChar(x,y,*str,size);//有效部分写入 
					str++; 
					x+=size/2; //字符,为全字的一半 
				}
			}
			else//中文 
			{     
					bHz=0;//有汉字库         
					OLED_ShowGB_Char(x,y,str,size); //显示这个汉字,空心显示 
					str+=2; 
					x+=size;//下一个汉字偏移	    
			}						 
	}   
} 


void OLED_ShowBMP(u8 x0,u8 y0,u8 x1,u8 y1,const u8 pic[])
{
	u16 i,j;
	u32 count=0;
	for(i=0;i<y1/8;i++)                           //有h行
	{
		OLED_SetPos(x0,y0+i);				   //设置光标,显示低i行
		for(j=0;j<x1;j++)					   //有w列
		{
			OLED_Send_Data(pic[count++]);	
//			count++;
			
		}		
	}
  }  
    
    
//更新显存到OLED	
void OLED_Refresh(void)
{
	u8 i,n;
	for(i=0;i<8;i++)
	{
	   OLED_WR_Byte(0xb0+i,OLED_CMD); //设置行起始地址
	   OLED_WR_Byte(0x00,OLED_CMD);   //设置低列起始地址
	   OLED_WR_Byte(0x10,OLED_CMD);   //设置高列起始地址
	   for(n=0;n<128;n++)
		 OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA);
  }
}


void Write_IIC_Byte(unsigned char IIC_Byte)
{
	unsigned char i;
	unsigned char m,da;
	da=IIC_Byte;
	OLED_SCLK_Clr();
	for(i=0;i<8;i++)		
	{
			m=da;
		//	OLED_SCLK_Clr();
		m=m&0x80;
		if(m==0x80)
		{OLED_SDIN_Set();}
		else OLED_SDIN_Clr();
			da=da<<1;
		OLED_SCLK_Set();
		OLED_SCLK_Clr();
		}


}
//等待信号响应
void IIC_Wait_Ack(void)
{


	OLED_SCLK_Set() ;
	OLED_SCLK_Clr();
}


void Write_IIC_Data(unsigned char IIC_Data)
{
   IIC_Start();
   Write_IIC_Byte(0x78);			//D/C#=0; R/W#=0
	IIC_Wait_Ack();	
   Write_IIC_Byte(0x40);			//write data
	IIC_Wait_Ack();	
   Write_IIC_Byte(IIC_Data);
	IIC_Wait_Ack();	
   IIC_Stop();
}


void Write_IIC_Command(unsigned char IIC_Command)
{
   IIC_Start();
   Write_IIC_Byte(0x78);            //Slave address,SA0=0
	IIC_Wait_Ack();	
   Write_IIC_Byte(0x00);			//write command
	IIC_Wait_Ack();	
   Write_IIC_Byte(IIC_Command); 
	IIC_Wait_Ack();	
   IIC_Stop();
}


void OLED_WR_Byte(unsigned dat,unsigned cmd)
{
	if(cmd)
			{

   Write_IIC_Data(dat);
   
		}
	else {
   Write_IIC_Command(dat);
		
	}
}

void IIC_Start()
{

	OLED_SCLK_Set() ;
	OLED_SDIN_Set();
	OLED_SDIN_Clr();
	OLED_SCLK_Clr();
}


//结束信号
void IIC_Stop()
{
OLED_SCLK_Set() ;
//	OLED_SCLK_Clr();
	OLED_SDIN_Clr();
	OLED_SDIN_Set();
	
}



//写入一个字节
void Send_Byte(u8 dat)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		OLED_SCLK_Clr();//将时钟信号设置为低电平
		if(dat&0x80)//将dat的8位从最高位依次写入
		{
			OLED_SDIN_Set();
    }
		else
		{
			OLED_SDIN_Clr();
    }
		OLED_SCLK_Set();//将时钟信号设置为高电平
		OLED_SCLK_Clr();//将时钟信号设置为低电平
		dat<<=1;
  }
}

//反显函数
void OLED_ColorTurn(u8 i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xA6,OLED_CMD);//正常显示
		}
	if(i==1)
		{
			OLED_WR_Byte(0xA7,OLED_CMD);//反色显示
		}
}

//屏幕旋转180度
void OLED_DisplayTurn(u8 i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xC8,OLED_CMD);//正常显示
			OLED_WR_Byte(0xA1,OLED_CMD);
		}
	if(i==1)
		{
			OLED_WR_Byte(0xC0,OLED_CMD);//反转显示
			OLED_WR_Byte(0xA0,OLED_CMD);
		}
}

//开启OLED显示 
void OLED_DisPlay_On(void)
{
	OLED_WR_Byte(0x8D,OLED_CMD);//电荷泵使能
	OLED_WR_Byte(0x14,OLED_CMD);//开启电荷泵
	OLED_WR_Byte(0xAF,OLED_CMD);//点亮屏幕
}

//关闭OLED显示 
void OLED_DisPlay_Off(void)
{
	OLED_WR_Byte(0x8D,OLED_CMD);//电荷泵使能
	OLED_WR_Byte(0x10,OLED_CMD);//关闭电荷泵
	OLED_WR_Byte(0xAF,OLED_CMD);//关闭屏幕
}


//t:1 填充 0,清空				   
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//超出范围了.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}

////显示2个数字
////x,y :起点坐标	 
////len :数字的位数
////size:字体大小
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1)
{
	u8 t,temp;
	for(t=0;t<len;t++)
	{
		temp=(num/OLED_Pow(10,len-t-1))%10;
			if(temp==0)
			{
				OLED_ShowChar(x+(size1/2)*t,y,'0',size1);
      }
			else 
			{
			  OLED_ShowChar(x+(size1/2)*t,y,temp+'0',size1);
			}
  }
}


//m^n
u32 OLED_Pow(u8 m,u8 n)
{
	u32 result=1;
	while(n--)
	{
	  result*=m;
	}
	return result;
}

//显示汉字
//void OLED_ShowCHinese(u8 x,u8 y,u8 no)
//{      			    
//	u8 t,adder=0;
//	OLED_Set_Pos(x,y);	
//    for(t=0;t<16;t++)
//		{
//				OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);
//				adder+=1;
//     }	
//		OLED_Set_Pos(x,y+1);	
//    for(t=0;t<16;t++)
//			{	
//				OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);
//				adder+=1;
//      }					
//}


//坐标设置

	void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f),OLED_CMD); 
}   
