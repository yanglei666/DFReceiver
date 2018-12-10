#include "Timer.h"

void Timer0Init()	//106微秒@11.0592MHz
{
	AUXR |= 0x80;	//定时器时钟1T模式
	TMOD &= 0xF0;	//设置定时器模式
	TMOD |= 0x01;	//设置定时器模式
	TL0 = 0x6C;		//设置定时初值 106
	TH0 = 0xFB;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;
	//INTCLKO = 1;    //开启定时器0分频输出
}

void Timer1Init()	//10毫秒@11.0592MHz
{
	AUXR &= 0xBF;	//定时器时钟12T模式
	TMOD &= 0x0F;	//设置定时器模式
	TL1 = 0x00;		//设置定时初值
	TH1 = 0xDC;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
	ET1 = 1;

    //设置定时器1为高优先级
    IP  |= 0x08;
    IPH |= 0x08;
}

void Timer3Init()		//1毫秒@11.0592MHz
{
    T4T3M &= 0xFD;		//定时器时钟12T模式
	T3L = 0x66;		    //设置定时初值
	T3H = 0xFC;		    //设置定时初值
	IE2 = 0x20;         //打开T3中断
	T4T3M |= 0x08;		//定时器3开始计时
}

void StartT3()
{
   T4T3M |= 0x08;		//定时器3开始计时
}

void StopT3()
{
   T4T3M &= 0xf7;		//定时器3停止计时
}
