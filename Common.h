#ifndef COMMON_H
#define COMMON_H
#include "STC\stc8.h"
#include "stdio.h"
#include <intrins.h>
//状态(模式)定义
#define POWER_SET_STATE	   0x01 //发射强度设置模式
#define FREQ_SET_STATE	   0x02 //发射频率设置模式
#define ALARM_SET_STATE	   0x04 //电击强度设置模式
#define ERROR_STATE	       0x08 //异常模式

#define ALARM_SET_ADDR	   0x0000 //发射强度设置EEPROM地址

//发射频率等级
#define BB_FREQ 106
#define CC_FREQ 70

/*
#define BB_PWM_LOW      2289   //(207*11059200/1000000)	//212us时间下限
#define BB_PWM_HIGH     2399   //(217*11059200/1000000)	//212us时间上限
#define CC_PWM_LOW      1470   //(133*11059200/1000000)	//138us时间下限
#define CC_PWM_HIGH     1581   //(143*11059200/1000000)	//138us时间上限
#define CODE_PWM_LOW    1968   //(178*11059200/1000000)	//182us时间下限
#define CODE_PWM_HIGH   2068   //(187*11059200/1000000)	//182us时间上限
#define ENDTIME         2
*/

#define BB_PWM_LOW      1144   //(207*5529600/1000000)	//212us时间下限
#define BB_PWM_HIGH     1200   //(217*5529600/1000000)	//212us时间上限
#define CC_PWM_LOW      735    //(133*5529600/1000000)	//138us时间下限
#define CC_PWM_HIGH     791    //(143*5529600/1000000)	//138us时间上限
#define CODE_PWM_LOW    984    //(178*5529600/1000000)	//182us时间下限
#define CODE_PWM_HIGH   1034   //(187*5529600/1000000)	//182us时间上限
#define ENDTIME         2

#define IGNORE_CNT      3      //忽略的脉冲数
#define BB_CNT_LOW      (10 - IGNORE_CNT)	   //212us个数下限
#define BB_CNT_HIGH     (20	- IGNORE_CNT)      //212us个数上限
#define BB_LEN_HIGH     30
#define CC_CNT_LOW      (18 - IGNORE_CNT)	   //138us个数下限
#define CC_CNT_HIGH     (30 - IGNORE_CNT)	   //138us个数上限
#define CC_LEN_HIGH     40
#define CODE_CNT_LOW    (25 - IGNORE_CNT)	   //182us个数下限
#define CODE_CNT_HIGH   (40 - IGNORE_CNT)	   //182us个数上限
#define CODE_LEN_HIGH   50

/*
#define BEEP_ON_CNT         8000      //Beep On时间(单位10us)
#define BEEP_OFF_CNT        20000     //Beep Off时间(单位10us)
#define LED_ON_CNT          20000     //Led On时间(单位10us)
#define LED_OFF_CNT         40000     //Led Off时间(单位10us)
#define ATTACK_CHARGE_CNT   2000      //充电时间(单位10us)
#define ATTACK_DISCHG_CNT   2030      //放电时间(单位10us)
*/

#define BEEP_ON_CNT         80      //Beep On时间(单位10us)
#define BEEP_OFF_CNT        200     //Beep Off时间(单位10us)
#define LED_ON_CNT          200     //Led On时间(单位10us)
#define LED_OFF_CNT         400     //Led Off时间(单位10us)
#define ATTACK_CHARGE_CNT   201    //充电时间(单位10us)
#define ATTACK_DISCHG_CNT   204    //放电时间(单位10us)
#define ATTACK_IDLE_CNT     700    //电击空闲时间(单位10us)

#define POWER_BEEP_ON         0x0936	//使能蜂鸣器阈值1.9V
#define POWER_BEEP_OFF        0x083e	//关使能蜂鸣器阈值1.7V
#define POWER_ATTACK_ON       0x0aaa	//使能电击阈值2.2V
#define POWER_ATTACK_OFF      0x09b2	//关使能电击阈值2.0V
#define POWER_LOW             0x083e	//关使能电击阈值2.0V

#define POWER_V27             0x0d17	//关使能电击阈值2.7V  3351
#define POWER_V25             0x0c1f	//关使能电击阈值2.5V  3103
#define POWER_V21             0x083e	//关使能电击阈值2.1V  2606
#define POWER_V19             0x083e	//关使能电击阈值1.9V  2358
#define POWER_V17             0x083e	//关使能电击阈值1.7V

#define CPU_IDLE_CNT        5000	//空闲最大循环次数 5秒
#define CPU_SLEEP_CNT       10	    //CPU休眠最大循环次数 (1小时测试一次ADC)

#define CLKMS_MAX_CNT       1000	//微秒指针最大循环次数 1000ms
#define CLKS_MAX_CNT        60	    //秒指针最大循环次数 120s

typedef unsigned int  u16;	  //对数据类型进行声明定义
typedef unsigned char u8;

void delay_ms(unsigned int n);
void delay_us(unsigned int n);
void Delay1ms();		//@5.5296MHz
void Delay2ms();		//@5.5296MHz
void Delay3ms();		//@5.5296MHz
void Delay5ms();		//@5.5296MHz
void Delay10ms();		//@5.5296MHz
#endif