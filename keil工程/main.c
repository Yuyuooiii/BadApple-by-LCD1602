#include <STC89C5xRC.H>
#include "LCD1602.h"
#include "UART.h"

// 帧缓冲区
unsigned char frame_buffer_1[8][8];
unsigned char frame_buffer_2[8][8];

// 帧状态变量
unsigned char frame_ready_flag = 0;    
unsigned char (*active_buffer)[8] = frame_buffer_1;  // 当前接收缓冲区

/**
 * 切换缓冲区并更新显示
 */
void handle_frame_complete(void) {	
    if (active_buffer == frame_buffer_1) {
        LCD_WriteCGRAM(frame_buffer_1); // 写入CGRAM
        active_buffer = frame_buffer_2; // 切换至另一个缓冲区
    } else {
        LCD_WriteCGRAM(frame_buffer_2);
        active_buffer = frame_buffer_1;
    }
    LCD_ShowFrame(); // 更新LCD显示
}

void main(void) {
    LCD_Init();    
    UART_Init();    
	LCD_ShowString(1,7,"BadApple!!");
	LCD_ShowString(2,7,"By suyu");
    while (1) {
        // 主循环可扩展其他逻辑
     
    }
}

/**
 * UART接收中断
 */
void UART_Receive_ISR(void) interrupt 4 { 
    static unsigned char received_byte_count = 0; // 接收字节计数
    unsigned char received_byte;

    if (RI) {  
        RI = 0;
        received_byte = SBUF; 

        if (received_byte == 0xFF) { // 起始位
            received_byte_count = 0; 
            frame_ready_flag = 0;    
        } else if (received_byte == 0xFE) { // 结束位
            if (received_byte_count == 64) { // 判断是否接收满一帧
                frame_ready_flag = 1; 
			  handle_frame_complete(); // 切换缓冲区并更新显示
            }
        } else { // 接收普通数据字节
            active_buffer[received_byte_count / 8][received_byte_count % 8] = received_byte;
            received_byte_count++; 
        }
    }  
}
