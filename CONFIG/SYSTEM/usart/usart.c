 #include "sys.h"
#include "usart.h"	
Computer_Information ComputerInformation[5];

short Readout=0;
unsigned char  receiveBuff[4] = {0}; 
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"					//ucos 使用	  
#endif

//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif
 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	
u8 VFlag;
u8 Res;
//初始化IO 串口1 //其实是usart1
//bound:波特率
void usart1_init(u32 bound){
   //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOB时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOB6复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOB7复用为USART1
	
	//USART1端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOB6与GPIOB7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PB6，PB7

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
  USART_Cmd(USART1, ENABLE);  //使能串口1 
	
	//USART_ClearFlag(USART1, USART_FLAG_TC);
	
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、


	
}

int data;
void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)   
	{	
     data=USART_ReceiveData(USART1);
		//ReceiveData();
//		if(Readout==1){
//		switch(receiveBuff[3]){
//			case 1:{
//				ComputerInformation[1].Target_Obeject_X=receiveBuff[1];
//				ComputerInformation[1].Target_Obeject_Y=receiveBuff[2];		
//			}break;
//			
//			case 2:{
//				ComputerInformation[2].Target_Obeject_X=receiveBuff[1];
//				ComputerInformation[2].Target_Obeject_Y=receiveBuff[2];	
//			}break;
//			
//			case 3:{
//				ComputerInformation[3].Target_Obeject_X=receiveBuff[1];
//				ComputerInformation[3].Target_Obeject_Y=receiveBuff[2];	
//			}break;
//			case 4:{
//				ComputerInformation[4].Target_Obeject_X=receiveBuff[1];
//				ComputerInformation[4].Target_Obeject_Y=receiveBuff[2];	
//			}break;
//			default:
//			break;
//		}
	//}
		
	
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);  // 清除标志位
	}

} 

#endif






//tx PC6  rx pc7  USART6
#if EN_USART6_RX  //如果使能了接收

u8 VFlag;
void usart6_init(u32 bound)
	{
   //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //使能GPIOB时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);//使能USART1时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6); //GPIOB6复用为USART1
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6); //GPIOB7复用为USART1
	
	//USART1端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //GPIOB6与GPIOB7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOC,&GPIO_InitStructure); //初始化PB6，PB7

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	//USART_InitStructure.USART_WordLength =USART_WordLength_8b;//字长为8位数据格式
		USART_InitStructure.USART_WordLength =USART_WordLength_8b;//字长为8位数据格式

	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART6, &USART_InitStructure); //初始化串口1
	
  USART_Cmd(USART6, ENABLE);  //使能串口1 
	
	//USART_ClearFlag(USART1, USART_FLAG_TC);
//	
//#if EN_USART6_RX	
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、

#endif
	
}


void USART6_IRQHandler(void)                	//串口1中断服务程序
{
	if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		VFlag =USART_ReceiveData(USART6);//(USART1->DR);	//读取接收到的数据
		
		
	

		}   		 

  

}

//#endif

	
void ReceiveData(void)
{
  unsigned char USART_Receiver             = 0;          //接收数据
	static int Counter1 =0;//每次只接收四次

	USART_Receiver = USART_ReceiveData(USART1);   //@@@@@#####如果你使用不是USART1更改成相应的，比如USART3
	if(Counter1>4){
		Counter1=0;
		Readout=0;
		if(USART_Receiver==0x55){
			receiveBuff[Counter1]=USART_Receiver;
		}
		else{
			receiveBuff[0]=0;
			receiveBuff[1]=0;
			receiveBuff[2]=0;
			receiveBuff[3]=0;
			receiveBuff[4]=0;
		}
	}
  else{
		if((USART_Receiver!=0x55)&&(Counter1==0)){
		}
		else {
			receiveBuff[Counter1]=USART_Receiver;
			if(Counter1==4){
				Readout=1;
			}
			else{
			}
			Counter1++;
			
		}
	}
}
