#include"LVDF.h"

#define LVD2V0          0x00                    //LVD@2.0V
#define LVD2V4          0x01                    //LVD@2.4V
#define LVD2V7          0x02                    //LVD@2.7V
#define LVD3V0          0x03                    //LVD@3.0V

void LVDFInit()
{
	PCON &= ~LVDF;                              //���Զ˿�
    RSTCFG = LVD2V7;                            //ʹ��2.7Vʱ��ѹ�ж�
    ELVD = 1;                                   //ʹ��LVD�ж�
}