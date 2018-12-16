#include"ADC.h"
extern u16 nADCVal;
void ADCInit()
{
    P1M0 &= 0xfe;                               //设置P1.0为ADC口
    P1M1 |= 0x01;
    ADCCFG = 0x0f;                              //设置ADC时钟为系统时钟/2/16/16
    ADC_CONTR = 0x8c;                           //使能ADC模块,通道13
    //EADC = 1;                                   //使能ADC中断  
}

void StartADC()
{
    ADC_CONTR |= 0x40;                          //启动AD转换
}

void  GetADCVal()
{
    u16 DataL = 0,DataH = 0;
	ADC_CONTR |= 0x40;                      //启动AD转换
    _nop_();
    _nop_();
    while (!(ADC_CONTR & 0x20));            //查询ADC完成标志
    ADC_CONTR &= ~0x20;                     //清完成标志

    nADCVal = ADC_RES;					        //读取ADC结果
	DataH   = nADCVal;                   
	nADCVal = nADCVal<<4;
	DataL = ADC_RESL>>4;
	nADCVal = nADCVal|DataL;	
}