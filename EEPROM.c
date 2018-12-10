#include"EEPROM.h"
//关闭IAP
void IapIdle()
{
	  IAP_CONTR=0;		 //关闭IAP功能
	  IAP_CMD  =0;		 //清除命令寄存器
	  IAP_TRIG =0;		 //清楚触发寄存器
	  IAP_ADDRH=0X80;	 //将地址设置到非IAP区域
	  IAP_ADDRL=0;
}

//从ISP/IAP/EEPROM区域读取一个字节
u8 IapReadByte(u16 addr)
{
	 u8 dat;				 //数据缓冲区
	 IAP_CONTR=ENABLE_IAP;	 //使能IAP	   同时设置等待时间
	 IAP_CMD=CMD_READ;		 //设置IAP命令
	 IAP_ADDRL=addr;		 //设置IAP低地址
	 IAP_ADDRH=addr>>8;		 //设置IAP高地址
	 IAP_TRIG=0X5a;			 //写触发命令（0x5a）	 写触摸命令以后  命令才会生效 手册713页
	 IAP_TRIG=0Xa5;			 //写触发命令（0xa5）
	 delay_us(10);				 //等待ISP/IAP/EEPROM操作完成
	 dat=IAP_DATA;			 //读ISP/IAP/EEPROM数据
	 IapIdle();				 //关闭IAP功能
	
	 return dat;			 //返回
}

//写一个字节数据到ISP/IAP/EEPROM区域
void IapProgramByte(u16 addr,u8 dat)
{
	 IAP_CONTR=ENABLE_IAP;	 //使能IAP	   同时设置等待时间
	 IAP_CMD=CMD_PROGRAM;	 //设置IAP命令
	 IAP_ADDRL=addr;		 //设置IAP低地址
	 IAP_ADDRH=addr>>8;		 //设置IAP高地址
	 IAP_DATA=dat;			 //写ISP/IAP/EEPROM数据
	 IAP_TRIG=0X5a;			 //写触发命令（0x5a）    写触摸命令以后  命令才会生效 手册713页
	 IAP_TRIG=0Xa5;			 //写触发命令（0xa5）
	 delay_us(10);				 //等待ISP/IAP/EEPROM操作完成
	 IapIdle();				 //关闭IAP功能  
}

//扇区擦除
void IapEraseSector(u16 addr)
{
	 IAP_CONTR=ENABLE_IAP;	 //使能IAP	 同时设置等待时间
	 IAP_CMD=CMD_ERASE;	     //设置IAP命令
	 IAP_ADDRL=addr;		 //设置IAP低地址
	 IAP_ADDRH=addr>>8;		 //设置IAP高地址
	 IAP_TRIG=0X5a;			 //写触发命令（0x5a）	 写触摸命令以后  命令才会生效 手册713页
	 IAP_TRIG=0Xa5;			 //写触发命令（0xa5）
	 delay_us(10);				 //等待ISP/IAP/EEPROM操作完成
	 IapIdle();				 //关闭IAP功能 
}