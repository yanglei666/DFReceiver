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

/*
#define BB_PWM_LOW      2289   //(207*11059200/1000000)	//212usʱ������
#define BB_PWM_HIGH     2399   //(217*11059200/1000000)	//212usʱ������
#define CC_PWM_LOW      1470   //(133*11059200/1000000)	//138usʱ������
#define CC_PWM_HIGH     1581   //(143*11059200/1000000)	//138usʱ������
#define CODE_PWM_LOW    1968   //(178*11059200/1000000)	//182usʱ������
#define CODE_PWM_HIGH   2068   //(187*11059200/1000000)	//182usʱ������
#define ENDTIME         2
*/

#define BB_PWM_LOW      1144   //(207*5529600/1000000)	//212usʱ������
#define BB_PWM_HIGH     1200   //(217*5529600/1000000)	//212usʱ������
#define CC_PWM_LOW      735    //(133*5529600/1000000)	//138usʱ������
#define CC_PWM_HIGH     791    //(143*5529600/1000000)	//138usʱ������
#define CODE_PWM_LOW    984    //(178*5529600/1000000)	//182usʱ������
#define CODE_PWM_HIGH   1034   //(187*5529600/1000000)	//182usʱ������
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

/*
#define BEEP_ON_CNT         8000      //Beep Onʱ��(��λ10us)
#define BEEP_OFF_CNT        20000     //Beep Offʱ��(��λ10us)
#define LED_ON_CNT          20000     //Led Onʱ��(��λ10us)
#define LED_OFF_CNT         40000     //Led Offʱ��(��λ10us)
#define ATTACK_CHARGE_CNT   2000      //���ʱ��(��λ10us)
#define ATTACK_DISCHG_CNT   2030      //�ŵ�ʱ��(��λ10us)
*/

#define BEEP_ON_CNT         80      //Beep Onʱ��(��λ10us)
#define BEEP_OFF_CNT        200     //Beep Offʱ��(��λ10us)
#define LED_ON_CNT          200     //Led Onʱ��(��λ10us)
#define LED_OFF_CNT         400     //Led Offʱ��(��λ10us)
#define ATTACK_CHARGE_CNT   201    //���ʱ��(��λ10us)
#define ATTACK_DISCHG_CNT   204    //�ŵ�ʱ��(��λ10us)
#define ATTACK_IDLE_CNT     700    //�������ʱ��(��λ10us)

#define POWER_BEEP_ON         0x0936	//ʹ�ܷ�������ֵ1.9V
#define POWER_BEEP_OFF        0x083e	//��ʹ�ܷ�������ֵ1.7V
#define POWER_ATTACK_ON       0x0aaa	//ʹ�ܵ����ֵ2.2V
#define POWER_ATTACK_OFF      0x09b2	//��ʹ�ܵ����ֵ2.0V
#define POWER_LOW             0x083e	//��ʹ�ܵ����ֵ2.0V

#define POWER_V27             0x0d17	//��ʹ�ܵ����ֵ2.7V  3351
#define POWER_V25             0x0c1f	//��ʹ�ܵ����ֵ2.5V  3103
#define POWER_V21             0x083e	//��ʹ�ܵ����ֵ2.1V  2606
#define POWER_V19             0x083e	//��ʹ�ܵ����ֵ1.9V  2358
#define POWER_V17             0x083e	//��ʹ�ܵ����ֵ1.7V

#define CPU_IDLE_CNT        5000	//�������ѭ������ 5��
#define CPU_SLEEP_CNT       10	    //CPU�������ѭ������ (1Сʱ����һ��ADC)

#define CLKMS_MAX_CNT       1000	//΢��ָ�����ѭ������ 1000ms
#define CLKS_MAX_CNT        60	    //��ָ�����ѭ������ 120s

typedef unsigned int  u16;	  //���������ͽ�����������
typedef unsigned char u8;

void delay_ms(unsigned int n);
void delay_us(unsigned int n);
void Delay1ms();		//@5.5296MHz
void Delay2ms();		//@5.5296MHz
void Delay3ms();		//@5.5296MHz
void Delay5ms();		//@5.5296MHz
void Delay10ms();		//@5.5296MHz
#endif