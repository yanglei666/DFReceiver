#include"ADC.h"
void ADCInit()
{
    P1M0 &= 0xfe;                               //����P1.0ΪADC��
    P1M1 |= 0x01;
    ADCCFG = 0x0f;                              //����ADCʱ��Ϊϵͳʱ��/2/16/16
    ADC_CONTR = 0x8c;                           //ʹ��ADCģ��,ͨ��13
    EADC = 1;                                   //ʹ��ADC�ж�  
}

void StartADC()
{
    ADC_CONTR |= 0x40;                          //����ADת��
}