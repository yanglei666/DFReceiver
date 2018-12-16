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
sbit LEDG = P2^4;
sbit LEDB = P2^5;
sbit Beep = P0^7;
sbit bAnalog   = P1^6;	    //ģ���·����
sbit bBoost    = P3^4;	    //Boost����
sbit bAttack   = P3^5;	    //�������
sbit BoostCtrl1   = P0^0;	//Boost��ѹ���ƿ���1
sbit BoostCtrl2   = P0^1;	//Boost��ѹ���ƿ���2

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
u16  nBeepWaitCnt     = 0;   //Beep�ӳ�ʱ��ms
u16  nAttackOffCntS   = 0;   //Boost����ʱ��ms(���õ�)
u16  nAttackOffCntC   = 0;   //Boost����ʱ��ms(��ǰʹ�õ�)
bit  bStartBeepTrg    = 0;	 //Beep������ʼ��־λ
bit  bBeepFlag        = 0;	 //Beep������Ӧ��־λ
bit  bStartLedTrg     = 0;	 //Led������ʼ��־λ
bit  bStartAttackTrg  = 0;	 //���������ʼ��־λ
bit  bLowPowerAlarm   = 0;	 //�͵��������־λ

//�͹���ģ�����
bit  bStartLowPower   = 0;	 //�͹��Ĵ�����ʼ��־λ
bit  bIDLEFlag        = 0;	 //��Ƭ��CPU�����۱�־λ
u16  nIDLECnt         = 0;   //��Ƭ��CPU�����ۼƼ���
u16  nSleepCnt        = 0;   //��Ƭ�������ۼƼ���
u16  nGetADCCnt       = 1000;   //��Ƭ������ADC�ۼƼ�������λ��һ�β���ADC

//��ͬ��λ�̼�������
u8 AttackLeveTb[] = {1,2,2,7,13};
//��ͬ��λ�������ʱ���
u16 AttackOffTb[] = {969,1969,969,135,52};

//����EEPROM����
void SaveConfig()
{
  IapEraseSector(ALARM_SET_ADDR);
  IapProgramByte(ALARM_SET_ADDR,nAlarmLevel);
}

//��ع�������
void PowerManageTask()
{
	if(nADCVal >= POWER_V27)  //����2.7Vʱ P0.0 P0.1 ����
	{
		P0M0 |= 0x03;
   		P0M1 &= 0xfc;
		BoostCtrl1 = 0;
		BoostCtrl2 = 0;
	}
	else if((nADCVal < POWER_V25)&&(nADCVal > POWER_V19)) //����2.5Vʱ P0.1����
	{
		P0M0 |= 0x01;
   		P0M1 &= 0xfe;
		P0M0 &= 0xfd;
		P0M1 |= 0x02;
		BoostCtrl1 = 0;
	}
	else if(nADCVal <= POWER_V19)	//����1.9Vʱ P0.0 P0.1 ȫ������
	{
		P0M0 &= 0xfc;
		P0M1 |= 0x03;
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
		delay_ms(2);
	    WKTCH=0x88;	       //��ʱ0.5��
	    WKTCL=0x00;
	    PCON = PD;		   //��ʼ����
			       
	    bAnalog = 0;       //��ģ���·
		nSleepCnt++;
		if(nSleepCnt == CPU_SLEEP_CNT)
		{
			nSleepCnt = 0;
			//�ɼ�ADC��ǰ����
            GetADCVal();
			printf("ADC is %d \n",nADCVal);
		}
		PowerManageTask();
		//����͵�������������˸���
		printf("come sleep2 \n");
		if(bLowPowerAlarm)
		{
			LEDR = 0;
			delay_ms(10);
			LEDR = 1;
		}
		else
		    delay_ms(5);      //��ʱһ��ʱ���ٿ��жϼ��PCA,�������
		CCF3 = 0;			  //�ڿ��ж�֮ǰ����������жϱ�־λ
	    EA = 1;		          //�����ж�
	    WKTCH=0x88;		      //��ʱ0.5��
	    WKTCL=0x00;
	    PCON = PD;	          //��ʼ����
	}
}

//ϵͳ����
void SystemTask()
{
   //�鿴�Ƿ����¼�����������ͳ�ƣ������Ƿ����͹���ģʽ
   /*if(bIDLEFlag)
   {	
   	  nIDLECnt++;
	  printf("nIDLECnt: %u  \n",nIDLECnt);
	  delay_ms(5);
	  if(nIDLECnt >= CPU_IDLE_CNT)
	  {	
	  	 bStartLowPower = 1; //�򿪵͹���ģʽ����
		 nIDLECnt = 0;
		 printf("come bStartLowPower \n");
	  }	
   }
   else
   {
   	  nIDLECnt = 0;
	  printf("nIDLECnt: 0000000\n");
   }*/

   //1000�������ֻز���һ��ADC
   nGetADCCnt++;
   if(nGetADCCnt >= 1000)
   {
   		nGetADCCnt = 0;
   		GetADCVal();
   }
   //��ع�������
   PowerManageTask();
   //����͵�������������˸���
   if(bLowPowerAlarm)
   {
	  LEDR = 0;
	  delay_ms(10);
	  LEDR = 1;
   } 
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
	  	 if(nAlarmLevel > 1)//1�������
		 {
		    nAlarmLevelOld = nAlarmLevel;
		 	nAttackTrgCnt  = AttackLeveTb[nAlarmLevel-1];
		    nAttackOffCntS = AttackOffTb[nAlarmLevel-1];
		 }
	  }
   }
   
   if(nAlarm2AttackEventCnt > 0)
   {
	  if(nAttackTrgCnt == 0)
	  {
	     nAlarm2AttackEventCnt--;
	  	 if(nAlarmLevel > 1)//1�������
		 {
		    nAlarmLevelOld = nAlarmLevel;
		 	nAttackTrgCnt  = AttackLeveTb[nAlarmLevel-1];
		    nAttackOffCntS = AttackOffTb[nAlarmLevel-1];
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
   nAttackOffCntS = AttackOffTb[nAlarmLevel-1];
   nAttackOffCntC = nAttackOffCntS;
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

void main()
{  
   //����ģʽʱʹ���ڲ�SCCģ��,����Լ1.5uA
   VOCTRL = 0x00;                              
   //��ȡEEPROM����
   LoadConfig(); 
   //����Timer3
   Timer3Init();
   //PCA����
   PCAInit();
   //��ʼ������1    
   Uart1Init();
   //IO������
   IOInit();
   //ADC����
   ADCInit();
   //��ѹ�������
   //LVDFInit();
   //�����ж�
   EA = 1;

   //��ģ���·
   bAnalog  = 0;
   bBoost   = 0;
   bAttack  = 0;
   Beep     = 0;
   BoostCtrl1 = 0;
   BoostCtrl2 = 0;

   while(1)
   {     
	   //�͹�������
       LowPowerTask();	   
       //PCA�¼���������
       PCAEventTask();
	   //ϵͳ����
       SystemTask();
	   printf("main is running \n");      
   }
}

void Lvd_Isr() interrupt 6 using 1
{
   PCON &= ~LVDF;         //���жϱ�־
   bAnalog  = 1;		  //�ر�ģ���·
   bEnableBeep   = 0;	  //�رշ�������boost
   bEnableAttack = 0;
   bBoost   = 0;
   bAttack  = 0;
   Beep     = 0;
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
* �� �� ��         : void Timer0() interrupt 1
* ��������		   : ��ʱ��0�жϺ���
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void Timer0() interrupt 1  using 1
{
	static u16 iCnt =0;	    //��������ָ��
	u16 PWMOnCnt = 0,PWMOffCnt = 0;
	PWMOnCnt  = 30;
	PWMOffCnt = 314;
	TL0 = 0x6C;		//���ö�ʱ��ֵ 106
	TH0 = 0xFB;		//���ö�ʱ��ֵ
    iCnt++;
	if(iCnt == PWMOnCnt)
	{
		bSigSwitch = 0;
	}
	if(iCnt == PWMOffCnt)
	{
		iCnt = 0;
		bSigSwitch = 1;
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
				   printf("nAlarm1Cnt: %bu nTrgCnt %u\n",nAlarm1Cnt,nTrgCnt);
			   }
			   else if(nAlarm1Cnt > 0)
			   {
			   	   printf("invalid  nAlarm1Cnt: %bu nTrgCnt %u\n",nAlarm1Cnt,nTrgCnt);
			   }
			   //138us���ڸ������
			   if((nAlarm2Cnt <= CC_CNT_HIGH)&&(nAlarm2Cnt >= CC_CNT_LOW)&&(nTrgCnt < CC_LEN_HIGH))
			   {
			       if(nAlarm2BeepEventCnt < 2)
				      nAlarm2BeepEventCnt++;
				   if(nAlarm2AttackEventCnt < 2)
				      nAlarm2AttackEventCnt++;
				   printf("nAlarm2Cnt: %bu nTrgCnt %u\n",nAlarm2Cnt,nTrgCnt);
			   }
			   else if(nAlarm2Cnt > 0)
			   {
			   	   printf("invalid  nAlarm2Cnt: %bu nTrgCnt %u\n",nAlarm2Cnt,nTrgCnt);
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
		bStartLowPower = 0;	//�͹���ģʽ���п�����0
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

void Timer3() interrupt 19  using 1
{
	static u16  iBeepCnt    = 0;
	static u16  iLedCnt     = 0;
	static u16  iAttackCnt  = 0;

	iBeepCnt++;
	iLedCnt++;
	iAttackCnt++;
	nIDLECnt++;

	//����Beep�ĵ�һ������
	if((bBeepFlag == 1)&&(nBeepTrgCnt == 1))
	{
	    printf("iBeepCnt: %u \n",iBeepCnt);
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
	}
	 
	//��������ĵ�һ������
	if((iAttackCnt == 1)&&(nAttackTrgCnt == AttackLeveTb[nAlarmLevelOld-1]))
	{
	    bStartAttackTrg = 1;
		nAttackOffCntC = nAttackOffCntS;   //���õ����ڴ��ݸ���ǰʹ�õ�
		if(bEnableAttack)
		{
			bBoost     = 0;
	        bAttack    = 0;
		}
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
	    	LEDB = 1;
		
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
			}
			else
			   LEDB = 0;
		}
		iLedCnt = 0;
	}

	//��������߼�
	if(iAttackCnt == nAttackOffCntC)
	{
	    if((nAttackTrgCnt > 1)&&(bStartAttackTrg == 1))
		{
		    if(bEnableAttack)
			{
				bBoost  = 1;
				bAttack = 0;
			}
		}	
	}
	else if(iAttackCnt == (nAttackOffCntC + ATTACK_CHARGE_CNT))
	{
		if((nAttackTrgCnt > 1)&&(bStartAttackTrg == 1))
		{
		    if(bEnableAttack)
			{
				bBoost  = 0;
				bAttack = 1;
			}
		}
	}
	else if(iAttackCnt == (nAttackOffCntC + ATTACK_DISCHG_CNT))
	{
		if((nAttackTrgCnt > 1)&&(bStartAttackTrg == 1))
		{
		    nAttackTrgCnt--;
			if(nAttackTrgCnt == 1)
			{
			   nAttackTrgCnt = 0;
			   bStartAttackTrg = 0;
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

	//����͹���ģʽ�߼�
	if(nIDLECnt >= CPU_IDLE_CNT)
	{	
	    bStartLowPower = 1; //�򿪵͹���ģʽ����
		nIDLECnt = 0;
	}
}