#include"EEPROM.h"
#include"PCA.h"
#include"ADC.h"
//IO初始化操作
unsigned char cnt    = 0;                           //存储PCA计时溢出次数
unsigned long count0 = 0;                           //记录上一次的捕获值
unsigned long count1 = 0;                           //记录本次的捕获值
unsigned long length = 0;                           //存储信号的时间长度
bit  bSigSwitch = 0;
sbit LEDR = P2^3;
sbit LEDG = P5^5;
sbit LEDB = P2^5;
sbit Beep = P0^7;
sbit bAnalog   = P1^6;	    //模拟电路开关
sbit bBoost    = P3^4;	    //Boost开关
sbit bAttack   = P3^5;	    //电击开关
sbit BoostCtrl1   = P3^6;	//Boost升压控制开关1
sbit BoostCtrl2   = P3^7;	//Boost升压控制开关2

bit  bEnableBeep   = 1;
bit  bEnableAttack = 1;

u8 nAlarm1Cnt = 0;
u8 nAlarm2Cnt = 0;
u8 nCodeCnt   = 0;
u16 nTrgCnt    = 0;    //触发次数
u8 nAlarmLevel = 1;    //档位
u8 nAlarmLevelOld = 1; //上一次档位

u16 nADCVal = 0;	    //ADC数值

bit  bStartData = 0;
u8   nAlarm1BeepEventCnt = 0;
u8   nAlarm1AttackEventCnt = 0;
u8   nAlarm2BeepEventCnt = 0;
u8   nAlarm2AttackEventCnt = 0;
bit  bAlarm1Event = 0;
bit  bAlarm2Event = 0;
bit  bCodeEvent = 0;

u8 nBeepTrgCnt        = 0;   //Beep触发次数
u8 nLedTrgCnt         = 0;   //Led触发次数
u8 nAttackTrgCnt      = 0;   //电击触发次数
u16  iAttackCnt       = 0;	 //电击定时器计数
u16  iAttackIDLECnt   = 0;	 //电击空闲计数
bit  bAttackIDLE      = 0;	 //电击空闲计数开关
u16  nBeepWaitCnt     = 0;   //Beep延迟时间ms
u16  nAttackOffCntS   = 0;   //Boost空闲时间ms(设置的)
u16  nAttackOffCntC   = 0;   //Boost空闲时间ms(当前使用的)
bit  bStartBeepTrg    = 0;	 //Beep触发开始标志位
bit  bBeepFlag        = 0;	 //Beep立即响应标志位
bit  bStartLedTrg     = 0;	 //Led触发开始标志位
bit  bLowPowerAlarm   = 0;	 //低电量警告标志位

//计时模块
u16  iClockCnt        = 0;	 //时钟计数
u8   nCurTime         = 0;   //当前时间
u8   nStartTime       = 0;   //开始触发电击时间
u8   nPreTime         = 0;   //上一次触发电击时间
bit  bAttackFull      = 0;	 //攻击满时间标志位

//低功耗模块变量
bit  bStartLowPower   = 0;	 //低功耗触发开始标志位
bit  bIDLEFlag        = 0;	 //单片机CPU空闲累标志位
u16  nIDLECnt         = 0;   //单片机CPU空闲累计计数
u16  nSleepCnt        = 0;   //单片机休眠累计计数
bit  bGetADCFlag      = 1;   //工作模式测量ADC开关

//不同档位刺激次数表
u8 AttackLeveTb[] = {2,3,4,5,6};

//训练策略
void TrainingStrategy()
{ 	
	u8 GapTime = 0;
	if(nCurTime < nPreTime)
		GapTime =  nCurTime + CLKS_MAX_CNT - nPreTime;
	else
		GapTime =  nCurTime - nPreTime;

	//printf("C %bu P %bu S %bu \n",nCurTime,nPreTime,nStartTime);
	if(GapTime > 1) //两次触发在1秒以内，代表连续触发
	{
		nStartTime = nCurTime;
	}

	if(bAttackFull)
	{	
		if(nCurTime < nPreTime)
			GapTime =  nCurTime + CLKS_MAX_CNT - nPreTime;
		else
			GapTime =  nCurTime - nPreTime;

		if(GapTime >= 5)
		{
			bAttackFull = 0;
			//printf("start request \n");
		}
	}
	else
	{
		if(nCurTime < nStartTime)
			GapTime =  nCurTime + CLKS_MAX_CNT - nStartTime;
		else
			GapTime =  nCurTime - nStartTime;

		if(GapTime >= 15)
		{
			bAttackFull = 1;
			//printf("stop request \n");
		}	
	}
	
	if(bAttackFull)
	{
		bEnableBeep   = 0;
		bEnableAttack = 0;
	}
	else
	{
		bEnableBeep   = 1;
		bEnableAttack = 1;
	}

	nPreTime = nCurTime;
}

//开始电击操作
void StartAttack()
{
	iAttackIDLECnt = 0;
	iAttackCnt     = 0;	
	TF1 = 0;		    //清除TF1标志
	TR1 = 1;		    //定时器1开始计时
	CR  = 0;			//关闭PCA
}

//保存EEPROM配置
void SaveConfig()
{
  IapEraseSector(ALARM_SET_ADDR);
  IapProgramByte(ALARM_SET_ADDR,nAlarmLevel);
}

//电池管理任务
void PowerManageTask()
{
	if(nADCVal >= POWER_V27)  //高于2.7V时 P3.6 P3.7 拉低
	{
		P3M0 |= 0xc0;
   		P3M1 &= 0x3f;
		BoostCtrl1 = 0;
		BoostCtrl2 = 0;
	}
	else if((nADCVal < POWER_V25)&&(nADCVal > POWER_V19)) //低于2.5V时 P3.7高阻
	{
		P3M0 |= 0x80;
   		P3M1 &= 0x7f;
		P3M0 &= 0xbf;
		P3M1 |= 0x40;
		BoostCtrl1 = 0;
	}
	else if(nADCVal <= POWER_V19)	//低于1.9V时 P3.6 P3.7 全部高阻
	{
		P3M0 &= 0x3f;
		P3M1 |= 0xc0;
	}

	//低于1.7V 低电量报警
	if(nADCVal < POWER_V17)
		bLowPowerAlarm = 1;
	else
		bLowPowerAlarm = 0;
}

//低功耗任务
void LowPowerTask()
{
	while(bStartLowPower)  //进入低功耗睡眠模式
	{
		EA = 0;            //关闭总中断
	    bAnalog = 1;	   //关闭模拟电路
		printf("come sleep1 \n");
	    _nop_();
	    _nop_();
		Delay2ms();
	    WKTCH=0x83;	       //定时0.5秒
	    WKTCL=0xe7;
	    PCON = PD;		   //开始休眠
			       
	    bAnalog = 0;       //打开模拟电路
		nSleepCnt++;
		if(nSleepCnt == CPU_SLEEP_CNT)
		{
			nSleepCnt = 0;
			Delay10ms();
			//采集ADC当前数据
            GetADCVal();
		}
		//退出低功耗模式的时候主程序测量一次ADC
		bGetADCFlag = 1;  
		PowerManageTask();
		//如果低电量报警，则闪烁红灯
		printf("come sleep2 \n");
		if(bLowPowerAlarm)
		{
			LEDR = 0;
			LEDG = 1;
			LEDB = 1;
			Delay10ms();
			LEDR = 1;
		}
		else
		    Delay3ms();       //延时一段时间再开中断检测PCA,避免干扰
		CCF3 = 0;			  //在开中断之前必须先清除中断标志位
	    EA = 1;		          //打开总中断
	    WKTCH=0x83;		      //定时0.5秒
	    WKTCL=0xe7;
	    PCON = PD;	          //开始休眠
	}
}

//系统任务
void SystemTask()
{
   if(bGetADCFlag)
   {
		bGetADCFlag = 0;
		Delay10ms();
		GetADCVal();
   }
   //电池管理任务
   PowerManageTask();
   //如果低电量报警，则闪烁红灯
   //if(bLowPowerAlarm)
   //{
	  //LEDR = 0;
	  //Delay10ms();
	  //LEDR = 1;
   //} 
}

//PCA事件处理任务
void PCAEventTask()
{
   if(nAlarm1BeepEventCnt > 1)
   {
	  if(nBeepTrgCnt == 0)	//如果结束了上一次触发
	  {
	     //printf("nAlarm1BeepEventCnt %bu \n",nAlarm1BeepEventCnt);
	     nAlarm1BeepEventCnt--;
	  	 nBeepTrgCnt   = 1;
		 bBeepFlag     = 1;	   
	  }
   }
   
   if(nAlarm2BeepEventCnt > 1)
   {
	  if(nBeepTrgCnt == 0)	//如果结束了上一次触发
	  {
	     //printf("nAlarm2BeepEventCnt %bu \n",nAlarm2BeepEventCnt);
	     nAlarm2BeepEventCnt--;
	  	 nBeepTrgCnt   = 1;
		 bBeepFlag     = 1; 
	  }
   }

   if(nAlarm1AttackEventCnt > 0)
   {
	  if(nAttackTrgCnt == 0)
	  {
	     nAlarm1AttackEventCnt--;
	  	 if(nAlarmLevel >= 1)//1档不电击
		 {
		    nAlarmLevelOld = nAlarmLevel;
		 	nAttackTrgCnt  = AttackLeveTb[nAlarmLevel-1];
			TrainingStrategy(); //训练策略
			StartAttack();      //开始电击
		 }
	  }
   }
   
   if(nAlarm2AttackEventCnt > 0)
   {
	  if(nAttackTrgCnt == 0)
	  {
	     nAlarm2AttackEventCnt--;
	  	 if(nAlarmLevel >= 1)//1档不电击
		 {
		    nAlarmLevelOld = nAlarmLevel;
		 	nAttackTrgCnt  = AttackLeveTb[nAlarmLevel-1];
			TrainingStrategy(); //训练策略
			StartAttack();      //开始电击
		 }
	  }
   }
   
   if(bCodeEvent)
   {
      bCodeEvent = 0;
	  if(nAlarmLevel >= 5)
	  {
	     nAlarmLevel = 1;
	  }	
	  else
	  {
	  	 nAlarmLevel++;
	  }
	  SaveConfig();	
	  nLedTrgCnt = nAlarmLevel;
      printf("bCodeEvent happen \n");
   }	
}

//设置蜂鸣器
void SetBeep(bit bval)
{
  	if(bEnableBeep)
	{
		Beep = bval;
	}
}

//获取EEPROM配置
void LoadConfig()
{
   nAlarmLevel = IapReadByte(ALARM_SET_ADDR);
   //对数据的有效性进行判断
   if((nAlarmLevel >5)||(nAlarmLevel < 1))	
   {
       nAlarmLevel = 1;
   }
   nAlarmLevelOld = nAlarmLevel;
}

void IOInit(void)
{
   //输出口全部设置推挽输出模式
   P2M1 = 0x00;
   P2M0 = 0xff;
   P3M1 &= 0xfd;
   P3M0 |= 0x02;
   P3M1 &= 0xdf;
   P3M0 |= 0x20;
   P3M1 &= 0xef;
   P3M0 |= 0x10;
   P1M0 |= 0x40;
   P1M1 &= 0xbf;
   P0M0 |= 0x83;
   P0M1 &= 0x7c;
}

//时钟初始化
void ClockInit()
{
    P_SW2 = 0x80;
	CLKDIV = 0x04;	 //四分频
	P_SW2 = 0x00;
}

//数据初始化
void DataInit()
{
   //打开总中断
   EA = 1;
   //设置PCA为最高优先级
   //IP |= 0x80;
   //IPH |= 0x80;
   //打开模拟电路
   bAnalog  = 0;
   bBoost   = 0;
   bAttack  = 0;
   Beep     = 0;
   BoostCtrl1 = 0;
   BoostCtrl2 = 0;
}

void main()
{  
   //掉电模式时使用内部SCC模块,功耗约1.5uA
   VOCTRL = 0x00;
   //时钟初始化
   ClockInit();                              
   //获取EEPROM配置
   LoadConfig();
   //配置Timer0
   Timer0Init(); 
   //配置Timer3
   Timer1Init();
   //PCA配置
   PCAInit();
   //初始化串口1    
   Uart1Init();
   //IO口配置
   IOInit();
   //ADC配置
   ADCInit();
   //低压检测配置
   LVDFInit();
   //数据初始化
   DataInit();
   delay_ms(10);
   while(1)
   {     
	   //低功耗任务
       LowPowerTask();	   
       //PCA事件处理任务
       PCAEventTask();
	   //系统任务
       SystemTask();
	   //printf("main is running \n");      
   }
}

void Lvd_Isr() interrupt 6 using 1
{
   PCON &= ~LVDF;         //清中断标志
   bAnalog       = 1;	  //关闭模拟电路
   bEnableBeep   = 0;	  //关闭蜂鸣器和boost
   bEnableAttack = 0;
   bBoost   = 0;
   bAttack  = 0;
   Beep     = 0;
   LEDG     = 0;          //绿灯表示进入低压模式
   LEDR     = 1;
   LEDB     = 1;
}

void ADC_Isr() interrupt 5 using 1
{
    u16 DataL = 0,DataH = 0;
    ADC_CONTR &= ~0x20;                         //清中断标志
    nADCVal = ADC_RES;
	DataH   = nADCVal;                          //读取ADC结果
	nADCVal = nADCVal <<4;
	DataL   = ADC_RESL>>4;
	nADCVal = nADCVal|DataL;
    ADC_CONTR |= 0x40;                          //继续AD转换
	//printf("nADCVal :%d H : %d L :%d \n",nADCVal,DataH,DataL);
}

/*******************************************************************************
* 函 数 名         : void Timer1() interrupt 3
* 函数功能		   : 定时器0中断函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void Timer1() interrupt 3  using 1
{
	//开始空闲计数
	if(bAttackIDLE)
	  iAttackIDLECnt++;
	else
	  iAttackCnt++;

	//触发电击的第一个周期
	if(iAttackCnt == 1)
	{
		if(nAttackTrgCnt > 1)
		{
			if(bEnableAttack)
			{
				bBoost     = 1;
		        bAttack    = 0;
			}
		}
	}
	else if(iAttackCnt == ATTACK_CHARGE_CNT)
	{
		if(nAttackTrgCnt > 1)
		{
			if(bEnableAttack)
			{
				bBoost  = 0;
				bAttack = 1;
			}
		}
	}
	else if(iAttackCnt == ATTACK_DISCHG_CNT)
	{
		if(nAttackTrgCnt > 1)
		{
		    nAttackTrgCnt--;
			if(nAttackTrgCnt == 1)
			{
			   bAttackIDLE   = 1;
			   if(bEnableAttack)
			   {
				   bBoost  = 0;
				   bAttack = 0;
			   }
			}
			else
			{
			   if(bEnableAttack)
			   {
			   	   bBoost  = 0;
			       bAttack = 0;
			   }
			}   
		}
	    iAttackCnt = 0;
	}
	
	if(iAttackIDLECnt == ATTACK_IDLE_CNT)
	{
		iAttackIDLECnt = 0;
		iAttackCnt     = 0;
		nAttackTrgCnt  = 0;
		bAttackIDLE    = 0;		
		TF1 = 0;		    //清除TF1标志
	    TR1 = 0;		    //定时器1开始计时
		CR  = 1;			//打开PCA
	} 
}

void PCA_Isr() interrupt 7
{
    if (CF)
    {
        CF = 0;
		if(bStartData)  //如果正在检测中
		{
		 	cnt++;
			if((cnt - ((unsigned char *)&count1)[1])>= ENDTIME) //检测到停止信号
			{
			   //212us周期个数检测
			   if((nAlarm1Cnt <= BB_CNT_HIGH)&&(nAlarm1Cnt >= BB_CNT_LOW)&&(nTrgCnt < BB_LEN_HIGH))
			   {
				   if(nAlarm1BeepEventCnt < 2)
				      nAlarm1BeepEventCnt++;
				   if(nAlarm1AttackEventCnt < 2)
				      nAlarm1AttackEventCnt++;
				   //printf("nAlarm1Cnt: %bu nTrgCnt %u\n",nAlarm1Cnt,nTrgCnt);
			   }
			   else if(nAlarm1Cnt > 0)
			   {
			   	   //printf("invalid  nAlarm1Cnt: %bu nTrgCnt %u\n",nAlarm1Cnt,nTrgCnt);
			   }
			   //138us周期个数检测
			   if((nAlarm2Cnt <= CC_CNT_HIGH)&&(nAlarm2Cnt >= CC_CNT_LOW)&&(nTrgCnt < CC_LEN_HIGH))
			   {
			       if(nAlarm2BeepEventCnt < 2)
				      nAlarm2BeepEventCnt++;
				   if(nAlarm2AttackEventCnt < 2)
				      nAlarm2AttackEventCnt++;
				   //printf("nAlarm2Cnt: %bu nTrgCnt %u\n",nAlarm2Cnt,nTrgCnt);
			   }
			   else if(nAlarm2Cnt > 0)
			   {
			   	   //printf("invalid  nAlarm2Cnt: %bu nTrgCnt %u\n",nAlarm2Cnt,nTrgCnt);
			   }
			   //182us周期个数检测
			   if((nCodeCnt <= CODE_CNT_HIGH)&&(nCodeCnt >= CODE_CNT_LOW)&&(nTrgCnt < CODE_LEN_HIGH))
			   {
			       bCodeEvent = 1;
			   }
			   else if(nCodeCnt > 0)
			   {
			   	   printf("invalid  nCodeCnt: %bu\n",nCodeCnt);
			   }
			   //复位操作
			   bStartData = 0;
			   cnt = 0; 
			   nTrgCnt    = 0;
			   nAlarm1Cnt = 0;
			   nAlarm2Cnt = 0; 
			   nCodeCnt   = 0;
			   count0     = 0;
			   count1     = 0;
			   length     = 0;
			}
		}
           
    }
    if (CCF3)
    {
        CCF3 = 0;
		//低功耗模式所有开关清0
		bStartLowPower = 0;	
		nIDLECnt = 0;	  
		//printf("PCA_Isr interrupt come \n");
		nTrgCnt++;
        count0 = count1;                        //备份上一次的捕获值
        ((unsigned char *)&count1)[3] = CCAP3L;
        ((unsigned char *)&count1)[2] = CCAP3H;
        ((unsigned char *)&count1)[1] = cnt;
        ((unsigned char *)&count1)[0] = 0;
        length = count1 - count0;              //length保存的即为捕获的脉冲宽度
		
		if((bStartData)&&(nTrgCnt > (IGNORE_CNT +1)))  //忽略前面起振的脉冲数
		{
		   //212us周期检测
		   if((length <= BB_PWM_HIGH)&&(length >= BB_PWM_LOW))
		   {
		       nAlarm1Cnt++;
		   }
		   //138us周期检测
		   if((length <= CC_PWM_HIGH)&&(length >= CC_PWM_LOW))
		   {
		       nAlarm2Cnt++;
		   }
		   //182us周期检测
		   if((length <= CODE_PWM_HIGH)&&(length >= CODE_PWM_LOW))
		   {
		       nCodeCnt++;
		   }    		
		}

		//第一次接收到上升沿，开启数据检测过程
		if(!bStartData)
		{
	       bStartData = 1;
		   cnt = 0; 
		   nAlarm1Cnt = 0;
		   nAlarm2Cnt = 0; 
		   nCodeCnt   = 0;
		}   	 
    }
}

void Timer0() interrupt 1  using 1
{
	static u16  iBeepCnt    = 0;
	static u16  iLedCnt     = 0;

	iBeepCnt++;
	iLedCnt++;
	nIDLECnt++;
	iClockCnt++;

	//触发Beep的第一个周期
	if((bBeepFlag == 1)&&(nBeepTrgCnt == 1))
	{
	    //printf("iBeepCnt: %u \n",iBeepCnt);
	    bBeepFlag = 0;
	    iBeepCnt  = 1;
	    bStartBeepTrg = 1;
		SetBeep(1);
	}
	 
	  
	//触发Led的第一个周期
	if((iLedCnt == 1)&&(nLedTrgCnt == nAlarmLevel))
	{
	    bStartLedTrg = 1;
		LEDB = 0;
		LEDG = 1;
		LEDR = 1;
	}
	 
	//峰鸣器工作逻辑
	if(iBeepCnt == BEEP_ON_CNT)
	{
	    if((nBeepTrgCnt > 0)&&(bStartBeepTrg == 1))
		{
			SetBeep(0);
		}    
	}
	else if(iBeepCnt == BEEP_OFF_CNT)
	{
	    if((nBeepTrgCnt > 0)&&(bStartBeepTrg == 1))
		{
			nBeepTrgCnt--;
			if(nBeepTrgCnt == 0)
			{
			   bAlarm1Event = 0;
			   bAlarm2Event = 0;
			   bStartBeepTrg = 0;
			   SetBeep(0);
			}
			else
			{
			   SetBeep(1);
			}	   
		}
		iBeepCnt = 0;
	}

	//LED工作逻辑
	if(iLedCnt == LED_ON_CNT)
	{
	    if((nLedTrgCnt > 0)&&(bStartLedTrg == 1))
		{
			LEDB = 1;
			LEDG = 1;
			LEDR = 1;
		}	
	}
	else if(iLedCnt == LED_OFF_CNT)
	{
		if((nLedTrgCnt > 0)&&(bStartLedTrg == 1))
		{
			nLedTrgCnt--;
			if(nLedTrgCnt == 0)
			{
			   bCodeEvent = 0;
			   bStartLedTrg = 0;
			   LEDB = 1;
			   LEDG = 1;
			   LEDR = 1;
			}
			else
			{
			   LEDB = 0;
			   LEDG = 1;
			   LEDR = 1;
			}
		}
		iLedCnt = 0;
	}

	//printf("nIDLECnt is %u \n",nIDLECnt);
	//进入低功耗模式逻辑
	if(nIDLECnt == CPU_IDLE_CNT)
	{
		//printf("nIDLECnt is %u \n",nIDLECnt);
	    bStartLowPower = 1; //打开低功耗模式开关
		nIDLECnt = 0;
	}

    //时钟计时,最大120秒
	if(iClockCnt == CLKMS_MAX_CNT)
	{
		if(nCurTime == CLKS_MAX_CNT)
		{
			nCurTime = 0;
		}
		else
			nCurTime++;

		iClockCnt = 0;
	}
}