#include "PCA.h"
void PCAInit()
{
    CCON = 0x00;
    CMOD = 0x09;                                //PCAʱ��Ϊϵͳʱ��,ʹ��PCA��ʱ�ж�
    CL = 0x00;
    CH = 0x00;
    CCAPM3 = 0x21;                              //PCAģ��0Ϊ16λ����ģʽ�������ز���
    CCAP3L = 0x00;
    CCAP3H = 0x00;
    CR = 1;                                     //����PCA��ʱ��
    //EA = 1;
}

