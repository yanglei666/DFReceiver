#ifndef COMMON_H
#define COMMON_H
#include "STC\stc8.h"
#include "stdio.h"
#include <intrins.h>
//״̬(ģʽ)����
#define POWER_SET_STATE	   0x01 //����ǿ������ģʽ
#define FREQ_SET_STATE	   0x02 //����Ƶ������ģʽ
#define ALARM_SET_STATE	   0x04 //���ǿ������ģʽ
#define ERROR_STATE	       0x08 //�쳣ģʽ

#define ALARM_SET_ADDR	   0x0000 //����ǿ������EEPROM��ַ

//����Ƶ�ʵȼ�
#define BB_FREQ 106
#define CC_FREQ 70

#define BB_PWM_LOW      2289   //(207*11059200/1000000)	//212usʱ������
#define BB_PWM_HIGH     2399   //(217*11059200/1000000)	//212usʱ������
#define CC_PWM_LOW      1470   //(133*11059200/1000000)	//138usʱ������
#define CC_PWM_HIGH     1581   //(143*11059200/1000000)	//138usʱ������
#define CODE_PWM_LOW    1968   //(178*11059200/1000000)	//182usʱ������
#define CODE_PWM_HIGH   2068   //(187*11059200/1000000)	//182usʱ������
#define ENDTIME         2

#define IGNORE_CNT      3      //���Ե�������
#define BB_CNT_LOW      (10 - IGNORE_CNT)	   //212us��������
#define BB_CNT_HIGH     (20	- IGNORE_CNT)      //212us��������
#define BB_LEN_HIGH     30
#define CC_CNT_LOW      (18 - IGNORE_CNT)	   //138us��������
#define CC_CNT_HIGH     (30 - IGNORE_CNT)	   //138us��������
#define CC_LEN_HIGH     40
#define CODE_CNT_LOW    (25 - IGNORE_CNT)	   //182us��������
#define CODE_CNT_HIGH   (40 - IGNORE_CNT)	   //182us��������
#define CODE_LEN_HIGH   50


#define BEEP_ON_CNT         80      //Beep Onʱ��
#define BEEP_OFF_CNT        200     //Beep Offʱ��
#define LED_ON_CNT          200     //Led Onʱ��
#define LED_OFF_CNT         400     //Led Offʱ��
#define ATTACK_CHARGE_CNT   20      //���ʱ��
#define ATTACK_DISCHG_CNT   21      //�ŵ�ʱ��

#define POWER_BEEP_ON         0x0936	//ʹ�ܷ�������ֵ1.9V
#define POWER_BEEP_OFF        0x083e	//��ʹ�ܷ�������ֵ1.7V
#define POWER_ATTACK_ON       0x0aaa	//ʹ�ܵ����ֵ2.2V
#define POWER_ATTACK_OFF      0x09b2	//��ʹ�ܵ����ֵ2.0V

typedef unsigned int  u16;	  //���������ͽ�����������
typedef unsigned char u8;

void delay_ms(unsigned int n);
void delay_us(unsigned int n);
#endif