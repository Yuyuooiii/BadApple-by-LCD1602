# BadApple-by-LCD1602
利用py程序将视频取模通过串口发送至单片机，写入LCD1602的CGRAM，刷新显示8个自定义字符，以此达到播放视频的目的。  
单片机型号：STC89C52RC    波特率：57600      
其他型号可以在keil工程中改改寄存器和引脚定义，我用的是普中的51开发板，如果你也一样就不用改了。  
py程序里把串口号改一改，MP4文件路径指定好，单片机上电，然后运行程序即可~  
