#include <STC89C5xRC.H>
#include "Delay.h"

sbit LCD_EN = P2^7;
sbit LCD_RW = P2^5;
sbit LCD_RS = P2^6;
#define LCD_IO P0

/**
  * @brief	写入指令时序  
  * @param 	Command指令（参见LCD1602指令集）
  * @retval	无
  */
void LCD_WriteCommand(unsigned char Command){
	LCD_RS = 0;
	LCD_RW = 0;
	LCD_IO = Command;
	LCD_EN = 1;
	Delay(1);			//必须加这个延时，原因未知
	LCD_EN = 0;
	Delay(1);
}
/**
  * @brief	写入数据时序	       
  * @param 	Data一个数据字节
  * @retval	无
  */
void LCD_WriteData(unsigned char Data){
	LCD_RS = 1;
	LCD_RW = 0;
	LCD_IO = Data;
	LCD_EN = 1;
	Delay(1);			//必须加这个延时，原因未知
	LCD_EN = 0;
	Delay(1);
}

/**
  * @brief	初始化，即调节一些基本设置	       
  * @param 	无
  * @retval	无
  */
void LCD_Init(void){
	LCD_WriteCommand(0x38);
	LCD_WriteCommand(0x0c);
	LCD_WriteCommand(0x06);
	LCD_WriteCommand(0x01);
}
/**
  * @brief	在指定行列显示一个字符	       
  * @param 	Line行	Column列	  Char待显示字符
  * @retval	无
  */
void LCD_ShowChar(unsigned char Line,unsigned char Column,unsigned char Char){
	if (Line == 1){
		LCD_WriteCommand(0x80|(Column-1));
	}else if (Line == 2){
		LCD_WriteCommand(0x80|(Column-1)+0x40);
	}
	LCD_WriteData(Char);
}
/**
  * @brief	在指定起始位置显示一段字符串	       
  * @param 	Line行	Column列	  String待显示字符串如"Hello"
  * @retval	无
  */
void LCD_ShowString(unsigned char Line,unsigned char Column,unsigned char *String){
	unsigned char i = 0;
	if (Line==1){
		LCD_WriteCommand(0x80|(Column-1));
	}else if (Line==2){
		LCD_WriteCommand(0x80|(Column-1+0x40));
	}
	while (*(String+i)!='\0'){
		LCD_WriteData(*(String+i));
		i++;
	}
}
/**
  * @brief	在指定起始位置显示指定位数的数字，范围0~65535，默认显示最后length长度位	       
  * @param 	Line行	Column列	  Num待显示的数字    Length显示位数
  * @retval	无
  */
void LCD_ShowNum(unsigned char Line,unsigned char Column,unsigned short Num,unsigned char Length){
	while (Length--){
		if (Line==1){
			LCD_WriteCommand(0x80|(Column-1+Length));
		}else if (Line==2){
			LCD_WriteCommand(0x80|(Column-1+0x40+Length));
		}
		LCD_WriteData(Num%10+0x30);
		Num /= 10;
	}	
}
/**
  * @brief	在指定起始位置显示有符号的指定位数的数字,范围-32768~32767	       
  * @param 	Line行	Column列	  Num待显示的有符号数字    Length显示位数（含符号）
  * @retval	无
  */
void LCD_ShowSignedNum(unsigned char Line,unsigned char Column,short Num,unsigned char Length){
	Length--;
	if (Line==1){
		LCD_WriteCommand(0x80|(Column-1));
	}else if (Line==2){
		LCD_WriteCommand(0x80|(Column-1+0x40));
	}
	if (Num<0){
		LCD_WriteData(0x2D);
		Num = -Num;
	}else {

		LCD_WriteData(0x2B);
	}
	
	while (Length--){
		if (Line==1){
			LCD_WriteCommand(0x80|(Column-1+Length+1));
		}else if (Line==2){
			LCD_WriteCommand(0x80|(Column-1+0x40+Length+1));
		}
		LCD_WriteData(Num%10+0x30);
		Num /= 10;
	}	
}
/**
  * @brief	在指定起始位置显示无符号的16进制数,范围0~255	，默认带0X前缀	       
  * @param 	Line行	Column列	  Num待显示的无符号数字
  * @retval	无
  */
void LCD_ShowHexNum(unsigned char Line,unsigned char Column,unsigned char Num){
	unsigned char NH,NL;
	if (Line==1){
		LCD_WriteCommand(0x80|(Column-1));
	}else if (Line==2){
		LCD_WriteCommand(0x80|(Column-1)+0x40);
	}
	LCD_WriteData(0+0x30);
	LCD_WriteData('X');
	
	NH = Num>>4;
	NL = Num<<4;
	NL = NL>>4;
	if (NH<10){
		LCD_WriteData(NH+0x30);
	}else {
		LCD_WriteData(NH-10+0x41);
	}
	if (NL<10){
		LCD_WriteData(NL+0x30);
	}else {
		LCD_WriteData(NL-10+0x41);
	}
}
/**
  * @brief	在指定起始位置显示无符号的2进制数,范围0~255，指定长度Length，默认显示靠后位	       
  * @param 	Line行	Column列	  Num待显示的无符号数字    Length显示位数
  * @retval	无
  */
void LCD_ShowBinNum(unsigned char Line,unsigned char Column,unsigned char Num,unsigned char Length){

	while (Length--){
		if (Line==1){
			LCD_WriteCommand(0x80|(Column-1+Length));
		}else if (Line==2){
			LCD_WriteCommand(0x80|(Column-1+0x40+Length));
		}
		LCD_WriteData(Num%2+0x30);
		Num /= 2;
	}	
}


void LCD_ShowFrame(void){
	
	LCD_ShowChar(1,3,0x00);
	LCD_ShowChar(1,4,0x01);
	LCD_ShowChar(1,5,0x02);
	LCD_ShowChar(1,6,0x03);
	LCD_ShowChar(2,3,0x04);
	LCD_ShowChar(2,4,0x05);
	LCD_ShowChar(2,5,0x06);
	LCD_ShowChar(2,6,0x07);
}


void LCD_WriteCGRAM(unsigned char (*current_buffer)[8]){
	unsigned char i,j;
	LCD_WriteCommand(0x40);
	for(i=0;i<8;i++){
		for(j=0;j<8;j++){
			LCD_WriteData(current_buffer[i][j]);
		}
	}
}