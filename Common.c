#include"Common.h"
//—” ±n∫¡√Î
void delay_ms(unsigned int n)
{
    int  a=0,b=0;
  	for(a=0;a<500;a++)
  	for(b=0;b<n;b++);
}

void delay_us(unsigned int n)
{
	while(n--);
}

void Delay1ms()		//@5.5296MHz
{
	unsigned char i, j;

	_nop_();
	_nop_();
	i = 6;
	j = 93;
	do
	{
		while (--j);
	} while (--i);
}

void Delay2ms()		//@5.5296MHz
{
	unsigned char i, j;

	_nop_();
	_nop_();
	_nop_();
	i = 11;
	j = 190;
	do
	{
		while (--j);
	} while (--i);
}

void Delay3ms()		//@5.5296MHz
{
	unsigned char i, j;

	_nop_();
	i = 17;
	j = 31;
	do
	{
		while (--j);
	} while (--i);
}

void Delay5ms()		//@5.5296MHz
{
	unsigned char i, j;

	i = 27;
	j = 226;
	do
	{
		while (--j);
	} while (--i);
}

void Delay10ms()		//@5.5296MHz
{
	unsigned char i, j;

	i = 54;
	j = 199;
	do
	{
		while (--j);
	} while (--i);
}



