#include"ADC.h"
extern u16 nADCVal;
void ADCInit()
{
    P1M0 &= 0xfe;                               //����P1.0ΪADC��
    P1M1 |= 0x01;
    ADCCFG = 0x0f;                              //����ADCʱ��Ϊϵͳʱ��/2/16/16
    ADC_CONTR = 0x8c;                           //ʹ��ADCģ��,ͨ��13
    //EADC = 1;                                   //ʹ��ADC�ж�  
}

void StartADC()
{
    ADC_CONTR |= 0x40;                          //����ADת��
}

void  GetADCVal()
{
    u16 DataL = 0,DataH = 0;
	ADC_CONTR |= 0x40;                      //����ADת��
    _nop_();
    _nop_();
    while (!(ADC_CONTR & 0x20));            //��ѯADC��ɱ�־
    ADC_CONTR &= ~0x20;                     //����ɱ�־

    nADCVal = ADC_RES;					        //��ȡADC���
	DataH   = nADCVal;                   
	nADCVal = nADCVal<<4;
	DataL = ADC_RESL>>4;
	nADCVal = nADCVal|DataL;	
}