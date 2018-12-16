#include"LVDF.h"

#define LVD2V0          0x00                    //LVD@2.0V
#define LVD2V4          0x01                    //LVD@2.4V
#define LVD2V7          0x02                    //LVD@2.7V
#define LVD3V0          0x03                    //LVD@3.0V

void LVDFInit()
{
	PCON &= ~LVDF;                              //测试端口
    RSTCFG = LVD2V7;                            //使能2.7V时低压中断
    ELVD = 1;                                   //使能LVD中断
}