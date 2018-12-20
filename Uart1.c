#include "uart1.h"

#define FOSC   5529600
//#define BRT    (65535 - FOSC/115200/4)
#define BRT    (65535 - FOSC/9600/4)

void Uart1Init()
{
  SCON = 0x50;     //REN=1允许串行接受状态，串口工作模式2     	                  
  AUXR |= 0X15;	   //开启1T模式
  TH2  = BRT>>8;
  TL2  = BRT;      // 设置波特率为115200                                                                     
  //EA   = 1;      // 开总中断 
  TI   = 1;
}