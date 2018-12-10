#include "PCA.h"
void PCAInit()
{
    CCON = 0x00;
    CMOD = 0x09;                                //PCA时钟为系统时钟,使能PCA计时中断
    CL = 0x00;
    CH = 0x00;
    CCAPM3 = 0x21;                              //PCA模块0为16位捕获模式（上升沿捕获）
    CCAP3L = 0x00;
    CCAP3H = 0x00;
    CR = 1;                                     //启动PCA计时器
    //EA = 1;
}

