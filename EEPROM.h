#ifndef EEPROM_H
#define EEPROM_H
#include "Common.h"

#define CMD_IDLE    0		 //空闲模式
#define CMD_READ    1		 //IAP字节读命令
#define CMD_PROGRAM 2		 //IAP字节编程命令
#define CMD_ERASE   3		 //IAP字节擦除命令

#define ENABLE_IAP  0X82     //if SYSCLK<20MHz

//测试地址
#define IAP_ADDRESS 0X0000	 //从手册上可以看出 STC15F2K60S2的EEPROM地址是0x0000- 0x03ff	正好是1k的地址空间
 
void IapIdle();
u8 IapReadByte(u16 addr);
void IapProgramByte(u16 addr,u8 dat);
void IapEraseSector(u16 addr);

#endif