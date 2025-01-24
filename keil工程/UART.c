#include <STC89C5xRC.H>

void UART_Init()		//57600bps@11.0592MHz
{
	PCON |= 0x80;		//使能波特率倍速位SMOD
	SCON = 0x50;		//8位数据,可变波特率
	AUXR &= 0xBF;		//定时器时钟12T模式
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设置定时器模式
	TMOD |= 0x20;		//设置定时器模式
	TL1 = 0xFF;			//设置定时初始值
	TH1 = 0xFF;			//设置定时重载值
	ET1 = 0;			//禁止定时器中断
	TR1 = 1;			//定时器1开始计时
	EA = 1;
	ES = 1;
}
void UART_Send_Byte(unsigned char Byte){
	SBUF = Byte;
	while(TI == 0);
	TI = 0;
}
/*
void UART_Routine()	interrupt 4
{



}
*/