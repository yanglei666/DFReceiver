#include"EEPROM.h"
#include"PCA.h"
#include"ADC.h"
//IO��ʼ������
unsigned char cnt    = 0;                           //�洢PCA��ʱ�������
unsigned long count0 = 0;                           //��¼��һ�εĲ���ֵ
unsigned long count1 = 0;                           //��¼���εĲ���ֵ
unsigned long length = 0;                           //�洢�źŵ�ʱ�䳤��
bit  bSigSwitch = 0;
sbit LEDR = P2^3;
sbit LEDG = P5^5;
sbit LEDB = P2^5;
sbit Beep = P0^7;
sbit bAnalog   = P1^6;	    //ģ���·����
sbit bBoost    = P3^4;	    //Boost����
sbit bAttack   = P3^5;	    //�������
sbit BoostCtrl1   = P3^6;	//Boost��ѹ���ƿ���1
sbit BoostCtrl2   = P3^7;	//Boost��ѹ���ƿ���2

bit  bEnableBeep   = 1;
bit  bEnableAttack = 1;

u8 nAlarm1Cnt = 0;
u8 nAlarm2Cnt = 0;
u8 nCodeCnt   = 0;
u16 nTrgCnt    = 0;    //��������
u8 nAlarmLevel = 1;    //��λ
u8 nAlarmLevelOld = 1; //��һ�ε�λ

u16 nADCVal = 0;	    //ADC��ֵ

bit  bStartData = 0;
u8   nAlarm1BeepEventCnt = 0;
u8   nAlarm1AttackEventCnt = 0;
u8   nAlarm2BeepEventCnt = 0;
u8   nAlarm2AttackEventCnt = 0;
bit  bAlarm1Event = 0;
bit  bAlarm2Event = 0;
bit  bCodeEvent = 0;

u8 nBeepTrgCnt        = 0;   //Beep��������
u8 nLedTrgCnt         = 0;   //Led��������
u8 nAttackTrgCnt      = 0;   //�����������
u16  iAttackCnt       = 0;	 //�����ʱ������
u16  iAttackIDLECnt   = 0;	 //������м���
bit  bAttackIDLE      = 0;	 //������м�������
u16  nBeepWaitCnt     = 0;   //Beep�ӳ�ʱ��ms
u16  nAttackOffCntS   = 0;   //Boost����ʱ��ms(���õ�)
u16  nAttackOffCntC   = 0;   //Boost����ʱ��ms(��ǰʹ�õ�)
bit  bStartBeepTrg    = 0;	 //Beep������ʼ��־λ
bit  bBeepFlag        = 0;	 //Beep������Ӧ��־λ
bit  bStartLedTrg     = 0;	 //Led������ʼ��־λ
bit  bLowPowerAlarm   = 0;	 //�͵��������־λ

//��ʱģ��
u16  iClockCnt        = 0;	 //ʱ�Ӽ���
u8   nCurTime         = 0;   //��ǰʱ��
u8   nStartTime       = 0;   //��ʼ�������ʱ��
u8   nPreTime         = 0;   //��һ�δ������ʱ��
bit  bAttackFull      = 0;	 //������ʱ���־λ

//�͹���ģ�����
bit  bStartLowPower   = 0;	 //�͹��Ĵ�����ʼ��־λ
bit  bIDLEFlag        = 0;	 //��Ƭ��CPU�����۱�־λ
u16  nIDLECnt         = 0;   //��Ƭ��CPU�����ۼƼ���
u16  nSleepCnt        = 0;   //��Ƭ�������ۼƼ���
bit  bGetADCFlag      = 1;   //����ģʽ����ADC����

//��ͬ��λ�̼�������
u8 AttackLeveTb[] = {2,3,4,5,6};

//ѵ������
void TrainingStrategy()
{ 	
	u8 GapTime = 0;
	if(nCurTime < nPreTime)
		GapTime =  nCurTime + CLKS_MAX_CNT - nPreTime;
	else
		GapTime =  nCurTime - nPreTime;

	//printf("C %bu P %bu S %bu \n",nCurTime,nPreTime,nStartTime);
	if(GapTime > 1) //���δ�����1�����ڣ�������������
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

//��ʼ�������
void StartAttack()
{
	iAttackIDLECnt = 0;
	iAttackCnt     = 0;	
	TF1 = 0;		    //���TF1��־
	TR1 = 1;		    //��ʱ��1��ʼ��ʱ
	CR  = 0;			//�ر�PCA
}

//����EEPROM����
void SaveConfig()
{
  IapEraseSector(ALARM_SET_ADDR);
  IapProgramByte(ALARM_SET_ADDR,nAlarmLevel);
}

//��ع�������
void PowerManageTask()
{
	if(nADCVal >= POWER_V27)  //����2.7Vʱ P3.6 P3.7 ����
	{
		P3M0 |= 0xc0;
   		P3M1 &= 0x3f;
		BoostCtrl1 = 0;
		BoostCtrl2 = 0;
	}
	else if((nADCVal < POWER_V25)&&(nADCVal > POWER_V19)) //����2.5Vʱ P3.7����
	{
		P3M0 |= 0x80;
   		P3M1 &= 0x7f;
		P3M0 &= 0xbf;
		P3M1 |= 0x40;
		BoostCtrl1 = 0;
	}
	else if(nADCVal <= POWER_V19)	//����1.9Vʱ P3.6 P3.7 ȫ������
	{
		P3M0 &= 0x3f;
		P3M1 |= 0xc0;
	}

	//����1.7V �͵�������
	if(nADCVal < POWER_V17)
		bLowPowerAlarm = 1;
	else
		bLowPowerAlarm = 0;
}

//�͹�������
void LowPowerTask()
{
	while(bStartLowPower)  //����͹���˯��ģʽ
	{
		EA = 0;            //�ر����ж�
	    bAnalog = 1;	   //�ر�ģ���·
		printf("come sleep1 \n");
	    _nop_();
	    _nop_();
		Delay2ms();
	    WKTCH=0x83;	       //��ʱ0.5��
	    WKTCL=0xe7;
	    PCON = PD;		   //��ʼ����
			       
	    bAnalog = 0;       //��ģ���·
		nSleepCnt++;
		if(nSleepCnt == CPU_SLEEP_CNT)
		{
			nSleepCnt = 0;
			Delay10ms();
			//�ɼ�ADC��ǰ����
            GetADCVal();
		}
		//�˳��͹���ģʽ��ʱ�����������һ��ADC
		bGetADCFlag = 1;  
		PowerManageTask();
		//����͵�������������˸���
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
		    Delay3ms();       //��ʱһ��ʱ���ٿ��жϼ��PCA,�������
		CCF3 = 0;			  //�ڿ��ж�֮ǰ����������жϱ�־λ
	    EA = 1;		          //�����ж�
	    WKTCH=0x83;		      //��ʱ0.5��
	    WKTCL=0xe7;
	    PCON = PD;	          //��ʼ����
	}
}

//ϵͳ����
void SystemTask()
{
   if(bGetADCFlag)
   {
		bGetADCFlag = 0;
		Delay10ms();
		GetADCVal();
   }
   //��ع�������
   PowerManageTask();
   //����͵�������������˸���
   //if(bLowPowerAlarm)
   //{
	  //LEDR = 0;
	  //Delay10ms();
	  //LEDR = 1;
   //} 
}

//PCA�¼���������
void PCAEventTask()
{
   if(nAlarm1BeepEventCnt > 1)
   {
	  if(nBeepTrgCnt == 0)	//�����������һ�δ���
	  {
	     //printf("nAlarm1BeepEventCnt %bu \n",nAlarm1BeepEventCnt);
	     nAlarm1BeepEventCnt--;
	  	 nBeepTrgCnt   = 1;
		 bBeepFlag     = 1;	   
	  }
   }
   
   if(nAlarm2BeepEventCnt > 1)
   {
	  if(nBeepTrgCnt == 0)	//�����������һ�δ���
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
	  	 if(nAlarmLevel >= 1)//1�������
		 {
		    nAlarmLevelOld = nAlarmLevel;
		 	nAttackTrgCnt  = AttackLeveTb[nAlarmLevel-1];
			TrainingStrategy(); //ѵ������
			StartAttack();      //��ʼ���
		 }
	  }
   }
   
   if(nAlarm2AttackEventCnt > 0)
   {
	  if(nAttackTrgCnt == 0)
	  {
	     nAlarm2AttackEventCnt--;
	  	 if(nAlarmLevel >= 1)//1�������
		 {
		    nAlarmLevelOld = nAlarmLevel;
		 	nAttackTrgCnt  = AttackLeveTb[nAlarmLevel-1];
			TrainingStrategy(); //ѵ������
			StartAttack();      //��ʼ���
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

//���÷�����
void SetBeep(bit bval)
{
  	if(bEnableBeep)
	{
		Beep = bval;
	}
}

//��ȡEEPROM����
void LoadConfig()
{
   nAlarmLevel = IapReadByte(ALARM_SET_ADDR);
   //�����ݵ���Ч�Խ����ж�
   if((nAlarmLevel >5)||(nAlarmLevel < 1))	
   {
       nAlarmLevel = 1;
   }
   nAlarmLevelOld = nAlarmLevel;
}

void IOInit(void)
{
   //�����ȫ�������������ģʽ
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

//ʱ�ӳ�ʼ��
void ClockInit()
{
    P_SW2 = 0x80;
	CLKDIV = 0x04;	 //�ķ�Ƶ
	P_SW2 = 0x00;
}

//���ݳ�ʼ��
void DataInit()
{
   //�����ж�
   EA = 1;
   //����PCAΪ������ȼ�
   //IP |= 0x80;
   //IPH |= 0x80;
   //��ģ���·
   bAnalog  = 0;
   bBoost   = 0;
   bAttack  = 0;
   Beep     = 0;
   BoostCtrl1 = 0;
   BoostCtrl2 = 0;
}

void main()
{  
   //����ģʽʱʹ���ڲ�SCCģ��,����Լ1.5uA
   VOCTRL = 0x00;
   //ʱ�ӳ�ʼ��
   ClockInit();                              
   //��ȡEEPROM����
   LoadConfig();
   //����Timer0
   Timer0Init(); 
   //����Timer3
   Timer1Init();
   //PCA����
   PCAInit();
   //��ʼ������1    
   Uart1Init();
   //IO������
   IOInit();
   //ADC����
   ADCInit();
   //��ѹ�������
   LVDFInit();
   //���ݳ�ʼ��
   DataInit();
   delay_ms(10);
   while(1)
   {     
	   //�͹�������
       LowPowerTask();	   
       //PCA�¼���������
       PCAEventTask();
	   //ϵͳ����
       SystemTask();
	   //printf("main is running \n");      
   }
}

void Lvd_Isr() interrupt 6 using 1
{
   PCON &= ~LVDF;         //���жϱ�־
   bAnalog       = 1;	  //�ر�ģ���·
   bEnableBeep   = 0;	  //�رշ�������boost
   bEnableAttack = 0;
   bBoost   = 0;
   bAttack  = 0;
   Beep     = 0;
   LEDG     = 0;          //�̵Ʊ�ʾ�����ѹģʽ
   LEDR     = 1;
   LEDB     = 1;
}

void ADC_Isr() interrupt 5 using 1
{
    u16 DataL = 0,DataH = 0;
    ADC_CONTR &= ~0x20;                         //���жϱ�־
    nADCVal = ADC_RES;
	DataH   = nADCVal;                          //��ȡADC���
	nADCVal = nADCVal <<4;
	DataL   = ADC_RESL>>4;
	nADCVal = nADCVal|DataL;
    ADC_CONTR |= 0x40;                          //����ADת��
	//printf("nADCVal :%d H : %d L :%d \n",nADCVal,DataH,DataL);
}

/*******************************************************************************
* �� �� ��         : void Timer1() interrupt 3
* ��������		   : ��ʱ��0�жϺ���
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void Timer1() interrupt 3  using 1
{
	//��ʼ���м���
	if(bAttackIDLE)
	  iAttackIDLECnt++;
	else
	  iAttackCnt++;

	//��������ĵ�һ������
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
		TF1 = 0;		    //���TF1��־
	    TR1 = 0;		    //��ʱ��1��ʼ��ʱ
		CR  = 1;			//��PCA
	} 
}

void PCA_Isr() interrupt 7
{
    if (CF)
    {
        CF = 0;
		if(bStartData)  //������ڼ����
		{
		 	cnt++;
			if((cnt - ((unsigned char *)&count1)[1])>= ENDTIME) //��⵽ֹͣ�ź�
			{
			   //212us���ڸ������
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
			   //138us���ڸ������
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
			   //182us���ڸ������
			   if((nCodeCnt <= CODE_CNT_HIGH)&&(nCodeCnt >= CODE_CNT_LOW)&&(nTrgCnt < CODE_LEN_HIGH))
			   {
			       bCodeEvent = 1;
			   }
			   else if(nCodeCnt > 0)
			   {
			   	   printf("invalid  nCodeCnt: %bu\n",nCodeCnt);
			   }
			   //��λ����
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
		//�͹���ģʽ���п�����0
		bStartLowPower = 0;	
		nIDLECnt = 0;	  
		//printf("PCA_Isr interrupt come \n");
		nTrgCnt++;
        count0 = count1;                        //������һ�εĲ���ֵ
        ((unsigned char *)&count1)[3] = CCAP3L;
        ((unsigned char *)&count1)[2] = CCAP3H;
        ((unsigned char *)&count1)[1] = cnt;
        ((unsigned char *)&count1)[0] = 0;
        length = count1 - count0;              //length����ļ�Ϊ�����������
		
		if((bStartData)&&(nTrgCnt > (IGNORE_CNT +1)))  //����ǰ�������������
		{
		   //212us���ڼ��
		   if((length <= BB_PWM_HIGH)&&(length >= BB_PWM_LOW))
		   {
		       nAlarm1Cnt++;
		   }
		   //138us���ڼ��
		   if((length <= CC_PWM_HIGH)&&(length >= CC_PWM_LOW))
		   {
		       nAlarm2Cnt++;
		   }
		   //182us���ڼ��
		   if((length <= CODE_PWM_HIGH)&&(length >= CODE_PWM_LOW))
		   {
		       nCodeCnt++;
		   }    		
		}

		//��һ�ν��յ������أ��������ݼ�����
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

	//����Beep�ĵ�һ������
	if((bBeepFlag == 1)&&(nBeepTrgCnt == 1))
	{
	    //printf("iBeepCnt: %u \n",iBeepCnt);
	    bBeepFlag = 0;
	    iBeepCnt  = 1;
	    bStartBeepTrg = 1;
		SetBeep(1);
	}
	 
	  
	//����Led�ĵ�һ������
	if((iLedCnt == 1)&&(nLedTrgCnt == nAlarmLevel))
	{
	    bStartLedTrg = 1;
		LEDB = 0;
		LEDG = 1;
		LEDR = 1;
	}
	 
	//�����������߼�
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

	//LED�����߼�
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
	//����͹���ģʽ�߼�
	if(nIDLECnt == CPU_IDLE_CNT)
	{
		//printf("nIDLECnt is %u \n",nIDLECnt);
	    bStartLowPower = 1; //�򿪵͹���ģʽ����
		nIDLECnt = 0;
	}

    //ʱ�Ӽ�ʱ,���120��
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