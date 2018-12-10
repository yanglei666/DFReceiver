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