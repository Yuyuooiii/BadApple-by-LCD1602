#ifndef __LCD1602_H__
#define __LCD1602_H__
void LCD_ShowChar(unsigned char Line,unsigned char Column,unsigned char Char);
void LCD_Init(void);
void LCD_WriteData(unsigned char Data);
void LCD_WriteCommand(unsigned char Command);
void LCD_ShowString(unsigned char Line,unsigned char Column,unsigned char *String);
void LCD_ShowNum(unsigned char Line,unsigned char Column,unsigned short Num,unsigned char Length);
void LCD_ShowSignedNum(unsigned char Line,unsigned char Column,int Num,unsigned char Length);
void LCD_ShowHexNum(unsigned char Line,unsigned char Column,unsigned char Num);
void LCD_ShowBinNum(unsigned char Line,unsigned char Column,unsigned char Num,unsigned char Length);
void LCD_ShowFrame(void);
void LCD_WriteCGRAM(unsigned char (*current_buffer)[8]);
#endif