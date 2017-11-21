/*hdr
**
**	
**
**	FILE NAME:	
**
**	AUTHOR:		
**
**	DATE:		
**
**	FILE DESCRIPTION:
**			
**			
**
**	FUNCTIONS:	
**			
**			
**			
**	NOTES:		
*/  

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/watchdog.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include "HikConfig.h"
#include "platform.h"
#include "common.h"
#include "specialControl.h"

typedef unsigned short  uint16;
static uint16 g_nLampStatus[8] = {0};
uint16 temp_nLampStatus[8] = {0}; //��������ʽ��������ʱ��
char temp_Sign[16] = {0};//��־λ
char lastRedLight[16] = {0}; //�����һ��״̬
char nowRedLight[16] = {0}; //�������״̬
char stayTimes[16] = {0};//�ӳٷ�������ʱ��
int HardwareWatchdoghd = 0;
static int HardflashDogFlag = 0;
int is_WatchdogEnabled = 0;
//static int g_yellowflash = 0; //������־��1������0����
static int g_errorstat = 0; //���ϱ�־ :1 :�й��ϣ� 0:�޹���
//static int g_closelampflag = 0; //�����ص�
//static uint16 g_yellowflashfreq = 1; //����Ƶ��
static int g_faultstatus[32] = {0}; //ͨ�����ϼ�¼���ڻָ�
static int CurDetectFreq = 0;
//��ʱ�޸ģ�����Խ��
extern CountDownVeh countdown_veh[17];
extern CountDownPed countdown_ped[17];
unsigned char g_RedCurrentValue[32] = {0};   //32·��Ƶ���ֵ
struct FAILURE_INFO s_failure_info;       //������Ϣ�ṹ��

/* add by Jicky */
extern SignalControllerPara *gSignalControlpara;    //ȫ��������Ϣ
extern PHASE_COUNTING_DOWN_PARAMS *gCountDownParams;       //����ʱ�ӿ���Ϣ
extern void *gHandle;	//����ʱʹ�õĶ��о��

extern UInt8 gStepFlag;    //������־��0����ʾδ������1����ʾ����
extern UInt16 gLightArray[8];	//����ʱʹ�õĵ������
extern pthread_rwlock_t gLightArrayLock;    //��������������

extern void CollectCountDownParams();
extern void StepPthreadInit(void);
extern UInt8 gSpecialControlSchemeId;       //������Ʒ�����

/* add over */

extern CHANNEL_LOCK_PARAMS gChannelLockedParams;
extern UInt8 gChannelLockFlag ;



#define TIMEOUT  20
#define _YELLOWFLASH_


#define N 32
#define M 1<<(N-1)
void print_data(unsigned int c)    
{       
	int i;    
	for (i = 0; i < N; i++)	{       
	   if(i % 8 == 0) printf(" ");  
	   putchar(((c & M) == 0) ? '0':'1'); 
	   c <<= 1;    
	}       
	printf("\n");    
}
/*********************************************************************************
*
* 	�趨��Ӧ���ʱ�䡣
*
***********************************************************************************/
extern void SeFreeGreenTime(unsigned char time);
void Count_Down_pulse_All(uint16 *g_nLampStatus);
void Count_Down_pulse_Half(uint16 *g_nLampStatus);

extern int fd;
extern int auto_pressed;    			  //0:�Զ�����û�а���   1:�Զ������Ѱ���
extern int manual_pressed;				  //0:�ֶ�����û�а���   1:�ֶ������Ѱ���
extern int flashing_pressed;			  //0:��������û�а���   1:���������Ѱ���
extern int allred_pressed;				  //0:ȫ�찴��û�а���   1:ȫ�찴���Ѱ���
extern int step_by_step_pressed;		  //0:��������û�а���   1:���������Ѱ���
extern int globalWatchdogFlagDisable;     //0:Enable  1:Disable.//WatchDog ʹ�ܿ���. (����)
extern int RTC_USE_GPS_ENABLE;			  //��0:Enable  0:Disable //GPSʹ�ܿ���.  (����)
extern struct SPECIAL_PARAMS s_special_params;   //�����������
extern struct CURRENT_PARAMS g_struRecCurrent[32];   //��������
extern unsigned int g_failurenumber;          //�����¼����
extern struct Count_Down_Params g_struCountDown;    //����ʱ����
extern unsigned int g_printf_switch;                 //��ӡ��Ϣ�Ƿ�������
/*********************************************************************************
*
* 	ÿ�ο�������������ʱ���¼��ָ���ļ���
*
***********************************************************************************/
int WriteLogInfos(const char *pFile,const char *pchMsg)
{
	time_t now;  
	struct tm *timenow; 
    FILE *pFileDebugInfo = NULL;
	struct stat f_stat;
	
	if(pchMsg == NULL)
	{
		printf("������¼��Ϣ��Ч!\n");
		return -1;
	}
    
    pFileDebugInfo = fopen(pFile, "a");
    if(pFileDebugInfo == NULL)
    {
    	printf("������¼�ļ�δ����ȷ��!\n");
        return -1;
    }
	
	if( stat( pFile, &f_stat ) == -1 )
	{
        fclose(pFileDebugInfo);
		printf("��ȡ������¼�ļ���Ϣʧ��!\n");
		return -1;
	}
	
	if(f_stat.st_size > 100*1024)
	{
		fclose(pFileDebugInfo);
		//���ļ���������0������д
		pFileDebugInfo = fopen(pFile, "w+");
	}

	//��ȡ���ʱ�׼ʱ��
	time(&now);  
	  
	//ת��Ϊ����ʱ��
	timenow = localtime(&now); 

	fprintf(pFileDebugInfo,"%04d.%02d.%02d-%02d:%02d:%02d %s\n",
		timenow->tm_year + 1900,timenow->tm_mon + 1,timenow->tm_mday,
		timenow->tm_hour,timenow->tm_min,timenow->tm_sec,
		pchMsg);
    
    fclose(pFileDebugInfo);
    return 0;
}

/*********************************************************************************
*
* 	д������־��
*
***********************************************************************************/
int WriteErrorInfos(const char *pFile,const char *pchMsg)
{
	time_t now;  
	struct tm *timenow; 
    FILE *pFileDebugInfo = NULL;
	struct stat f_stat;
	if(pchMsg == NULL)
	{
		printf("���ϼ�¼��Ϣ��Ч!\n");
		return -1;
	}
    pFileDebugInfo = fopen(pFile, "a");
    if(pFileDebugInfo == NULL)
    {
    	printf("���ϼ�¼�ļ�δ����ȷ��!\n");
        return -1;
    }
	if( stat( pFile, &f_stat ) == -1 )
	{
        fclose(pFileDebugInfo);
		printf("��ȡ���ϼ�¼�ļ���Ϣʧ��!\n");
		return -1;
	}
	if(f_stat.st_size > 1024*1024)
	{
		fclose(pFileDebugInfo);
		pFileDebugInfo = fopen(pFile, "w+");
		if(pFileDebugInfo == NULL)
		{	
			printf("���ϼ�¼�ļ�������δ����ȷ��!\n");
			return -1;
		}
	}
	time(&now);  
	timenow = localtime(&now); 
	fprintf(pFileDebugInfo,"%04d.%02d.%02d-%02d:%02d:%02d %s\n",
		timenow->tm_year + 1900,timenow->tm_mon + 1,timenow->tm_mday,
		timenow->tm_hour,timenow->tm_min,timenow->tm_sec,
		pchMsg);
    fclose(pFileDebugInfo);
    return 0;
}


/*********************************************************************************
*
* 	д�����ƹ�����־��
*
***********************************************************************************/
int WriteFailureInfos(const char *pFile,struct FAILURE_INFO s_failure_info)
{
    FILE *pFileDebugInfo = NULL;
	struct stat f_stat;

    pFileDebugInfo = fopen(pFile, "a");
    if(pFileDebugInfo == NULL)
    {
    	printf("���ϼ�¼�ļ�δ����ȷ��!\n");
        return -1;
    }
	if( stat( pFile, &f_stat ) == -1 )
	{
        fclose(pFileDebugInfo);
		printf("��ȡ���ϼ�¼�ļ���Ϣʧ��!\n");
		return -1;
	}
	if(f_stat.st_size > 1024*1024)
	{
		fclose(pFileDebugInfo);
		pFileDebugInfo = fopen(pFile, "w+");
		if(pFileDebugInfo == NULL)
		{	
			printf("���ϼ�¼�ļ�������δ����ȷ��!\n");
			return -1;
		}
	}
	fwrite(&s_failure_info,sizeof(s_failure_info),1,pFileDebugInfo);
    fclose(pFileDebugInfo);
    return 0;
}

/*********************************************************************************
*
* 	д����״̬��־
*
***********************************************************************************/
void WriteFaultStatus()
{
	FILE *pFile = NULL;
	pFile = fopen("/home/FaultStatus.dat", "wb");
	if (pFile == NULL)
	{
		printf("��FaultStatus.datʧ��,write\n");
		return;
	}
	fwrite(g_faultstatus, sizeof(g_faultstatus), 1, pFile);
	fclose(pFile);
}

/*********************************************************************************
*
* 	������״̬��־
*
***********************************************************************************/
void ReadFaultStatus()
{
	FILE *pFile = NULL;
	pFile = fopen("/home/FaultStatus.dat", "rb");
	if (pFile == NULL)
	{
		printf("��FaultStatus.datʧ��,read\n");
		return;
	}
	fread(g_faultstatus, sizeof(g_faultstatus), 1, pFile);
	fclose(pFile);
}



/*********************************************************************************
*
* 	Ӳ�����Ź���ʼ����
*
***********************************************************************************/

void HardwareWatchdogInit()
{
	//����Ӳ�����Ź�ι��
	is_WatchdogEnabled = s_special_params.iWatchdogSwitch;
	if(is_WatchdogEnabled == 1)
	{
		//ʹ��Ӳ�����Ź���ͨ���������ι��
		//�ر��ⲿι������
		system("killall watchdog");
		//��Ӳ�����Ź�
		HardwareWatchdoghd = open("/dev/watchdog", O_WRONLY);
		if(HardwareWatchdoghd == -1) 
		{
			printf("Open watchdog error!!!\n");
			return;
		}
		//����Ӳ�����Ź�
		ioctl(HardwareWatchdoghd, WDIOC_SETOPTIONS, WDIOS_ENABLECARD);	
		printf("Watchdog is enabled!!!\n");
	}
	else
	{
		//�ⲿι�������������(�ⲿι�������Ѿ��ڿ�������ʱ�Զ�����)
		printf("Watchdog is disabled!!!\n");
	}
}

/*********************************************************************************
*
* 	Ӳ�����Ź�ι����
*
***********************************************************************************/

unsigned int HardwareWatchdogKeepAlive(int boardNum)
{
	int dummy = 0;
	//Ӳ�����Ź�ʹ��ʱ����boardNumΪ8ʱι��
	if(boardNum == 8 && is_WatchdogEnabled == 1 )
	{
		//����ι������
		ioctl(HardwareWatchdoghd, WDIOC_KEEPALIVE, &dummy);
	}
	//����־
	if(1 == g_printf_switch)
    {
        freopen("/dev/tty","w",stdout);//����־
    }
    else
    {
        freopen("/dev/tty","w",stderr);//�ر���־
    }
	return 1;
}

/*********************************************************************************
*
* 	GPS�Ƿ����ÿ��ƺ�����
*
***********************************************************************************/

void GPSInit()
{
	
	//GPS�����Ƿ��
	if(s_special_params.iGpsSwitch == 0)
	{
		//GPS����û�д�
		printf("GPS is disabled!!!\n");
		system("killall -9 GPS");
		Set_LED2_OFF();
	}
	else 
	{
		//GPS�����Ѿ���
		printf("GPS is enabled!!!\n");
		system("killall -9 GPS");
		//Ϩ��GPSָʾ��
		Set_LED2_OFF();
		system("/root/GPS&");
	}
}


/*********************************************************************************
*
* ����λ���ѹ�źš�
*
***********************************************************************************/

extern void RedgreenCollision(int boardNum, uint16 boardInfo);
uint16 PhaseLampVoltInput(int boardNum)
{
	if (boardNum<1 || boardNum>8)
	{
		printf("boardNum error:%d\n", boardNum);
		return 0;
	}
	char msg[128] = "";
	time_t now;  
	struct tm *timenow;
	uint16 boardInfo = 0;
	if (g_errorstat == 1)
	{
		return ~boardInfo;
	}
	i_can_its_get_Volt(boardNum, &boardInfo);
	//��ȡ���ʱ�׼ʱ��
	time(&now);  
	  
	//ת��Ϊ����ʱ��
	timenow = localtime(&now); 

	sprintf(msg,"%04d.%02d.%02d-%02d:%02d:%02d",
		timenow->tm_year + 1900,timenow->tm_mon + 1,timenow->tm_mday,
		timenow->tm_hour,timenow->tm_min,timenow->tm_sec);
	printf("%s  INFO 100131: boardNum:%d, boardInfodes:%d, boardInfosrc:%d\n",msg, boardNum, boardInfo, g_nLampStatus[boardNum-1]&0xfff); 
	
	//���̳�ͻ���
	RedgreenCollision(boardNum, boardInfo);
	return ~boardInfo;
}

#define Time_To_Minutes(hour,minute,second) ((hour)*3600+(minute)*60+second)

/*****************************************************************************
 �� �� ��  : IsDateInControlArea
 ��������  : �жϵ�ǰʱ���Ƿ���ָ��ʱ�����
 �������  : CHANNEL_LOCK_PARAMS *pChannnelLockedParams  
 �� �� ֵ  : 
 �޸���ʷ  
  1.��    ��   : 2015��3��10��
    ��    ��   : Ф�Ļ�
    �޸�����   : �����ɺ���

*****************************************************************************/
static unsigned char IsDateInControlArea(CHANNEL_LOCK_PARAMS *pChannnelLockedParams)
{
	time_t timep = time(NULL);
    struct tm now;

	localtime_r(&timep, &now);

    unsigned int iBeginTime = 0;
    unsigned int iEndTime = 0;
    unsigned int iNow = 0;

    iBeginTime = Time_To_Minutes(pChannnelLockedParams->ucBeginTimeHour,pChannnelLockedParams->ucBeginTimeMin,pChannnelLockedParams->ucBeginTimeSec);
    iEndTime = Time_To_Minutes(pChannnelLockedParams->ucEndTimeHour,pChannnelLockedParams->ucEndTimeMin,pChannnelLockedParams->ucEndTimeSec);
    iNow = Time_To_Minutes(now.tm_hour,now.tm_min,now.tm_sec);

    //normal 
    if(iBeginTime < iEndTime)
    {
        if((iNow > iBeginTime) && (iNow < iEndTime))
        {
            return 1;
        }
    }
    else//����ʼʱ��Ƚ���ʱ��Сʱ
    {
        if((iNow > iBeginTime) && (iNow < iEndTime))
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }

    return 0;
}


/*****************************************************************************
 �� �� ��  : IsStartLockChannel
 ��������  : �ж��Ƿ�ʼ����ͨ�������û���յ����������򷵻�0�����յ�������
             ���ǰʱ����ڲ��������򷵻ش����������򷵻�����
 �������  : CHANNEL_LOCK_PARAMS *pChannnelLockedParams  
 �� �� ֵ  : 
 �޸���ʷ  
  1.��    ��   : 2015��3��31��
    ��    ��   : Ф�Ļ�
    �޸�����   : �����ɺ���

*****************************************************************************/
unsigned char IsStartLockChannel(CHANNEL_LOCK_PARAMS *pChannnelLockedParams)
{
    if(gChannelLockFlag == 0)//���û��ͨ�����������ֱ�ӷ��ء�
    {
        return 0;//������
    }

    //��������˹������򵫵�ǰʱ�䲢���ڹ��������ڣ���ֱ�ӷ���2��
    if((pChannnelLockedParams->ucWorkingTimeFlag == 1) && (IsDateInControlArea(pChannnelLockedParams) == 0))
    {
        gChannelLockFlag = 2;
        return 2;//������
    }

    gChannelLockFlag = 1;//����
    return 1;
}

/*****************************************************************************
 �� �� ��  : LockChannel
 ��������  : �����Ƿ�����ͨ���ı�ͨ��״̬
 �������  : CHANNEL_LOCK_PARAMS *pChannnelLockedParams  
 �� �� ֵ  : 
 �޸���ʷ  
  1.��    ��   : 2015��3��10��
    ��    ��   : Ф�Ļ�
    �޸�����   : �����ɺ���

*****************************************************************************/
void LockChannel(CHANNEL_LOCK_PARAMS *pChannnelLockedParams)
{
    int i = 0;
    unsigned short  value;
    unsigned char cFlag = 0;
    static unsigned char cArrayBlinkStatus[32] = {0};//����������������ʱ��һ�ε�ɫ��״̬������������ʱ�Ƿ�Ӧ�����
    static unsigned char cArrayIsTimeEnd[32] = {0};//����ʱ��Ҫ��֤״̬����ʱ���500ms.
    
    if((pChannnelLockedParams == NULL)||(IsStartLockChannel(pChannnelLockedParams) != 1))
    {
        return;
    }

    for(i = 0 ; i < 32; i++)
    {
        cFlag = 0;
        
        switch(pChannnelLockedParams->ucChannelStatus[i])
        {
            case GREEN:             value = 1;    break;
            case RED:               value = 2;    break;
            case YELLOW:            value = 4;    break;
            case GREEN_BLINK:       
                                    if(++cArrayIsTimeEnd[i] < 2)
                                    {
                                        value = cArrayBlinkStatus[i];
                                        break;
                                    }
                                    cArrayIsTimeEnd[i] = 0;
                                    value = (cArrayBlinkStatus[i] == 0) ? 1 : 0;    
                                    cArrayBlinkStatus[i] = value;
                                    break;
            case YELLOW_BLINK:      
                                    if(++cArrayIsTimeEnd[i] < 2)
                                    {
                                        value = cArrayBlinkStatus[i];
                                        break;
                                    }
                                    cArrayIsTimeEnd[i] = 0;                                    
                                    value = (cArrayBlinkStatus[i] == 0) ? 4 : 0;  
                                    cArrayBlinkStatus[i] = value;
                                    break;
            case TURN_OFF:          value = 0;    break;
            case INVALID:           cFlag = 1;    break;
            default :               cFlag = 1;    break;
        }

        if(cFlag == 0)//����Ǻ��Ի���Ĭ������£��������øýӿڡ�
        {   
            put_lamp_value(&g_nLampStatus[i/4], i % 4, value);
        }
    }
}

/*********************************************************************************
*
*  д�ƿذ��źŵơ�
*
***********************************************************************************/
int PhaseLampOutput(int boardNum, uint16 outLamp)
{

//	printf("INFO 100130:%d:0x%x|",boardNum, outLamp); 
//	print_data(outLamp);
    
	if (boardNum<1 || boardNum>8)
	{
		printf("boardNum error:%d\n", boardNum);
		return -1;
	}
	g_nLampStatus[boardNum-1] = outLamp;
	if (boardNum == 8)
	{
		if(1 == g_struCountDown.iCountDownMode)
		{
			Count_Down_pulse_All(g_nLampStatus);
		}
		else if(2 == g_struCountDown.iCountDownMode)
		{
			Count_Down_pulse_Half(g_nLampStatus);
		}
		LockChannel(&gChannelLockedParams);
		if (gStepFlag == 0)
		    i_can_its_send_led_request(boardNum, g_nLampStatus);
		else
		{
		    pthread_rwlock_rdlock(&gLightArrayLock);
            i_can_its_send_led_request(boardNum, gLightArray);
		    pthread_rwlock_unlock(&gLightArrayLock);
		}		
		//���ذ�����ָʾ��
		Hiktsc_Running_Status();
		//Ӳ�����Ź�ι������
		HardwareWatchdogKeepAlive(boardNum);
	}

	//��ѹ���
	if(s_special_params.iVoltageAlarmSwitch == 1 && s_special_params.iErrorDetectSwitch == 1)
	{
		PhaseLampVoltInput(boardNum);
	}
	
	
	return 0;
}


/*********************************************************************************
*
* 	��������źš�
*
***********************************************************************************/

uint16 VehicleCheckInput(int boardNum)
{
	uint16 boardInfo = 0;
	
	if (boardNum<1 || boardNum>3)
	{
		printf("boardNum error:%d\n", boardNum);
		return -1;
	}
	boardInfo = recv_date_from_vechile(boardNum);
	
	printf("INFO 100140:%d,boardInfo:%d\n",boardNum,boardInfo); 


	return boardInfo;
}


/*********************************************************************************
*
*	дӲ�����źš�
*
***********************************************************************************/
extern void RedExtinguish(); //��Ƹ�����������Ϊ���Ϩ��
void HardflashDogOutput(void)
{
	if (HardflashDogFlag == 1)
	{
		HardflashDogFlag = 0;
	}
	else
	{
		HardflashDogFlag = 1;
	}
	
	HardflashDogCtrl(HardflashDogFlag);

	printf("INFO 100150 Ӳ�����ź�:%d\n", HardflashDogFlag); 

	CurDetectFreq++;
	if( CurDetectFreq > 2 )
	{
		//1����ͬ��һ��nandflash����ֹ�ϵ��������ļ���ʧ
		//system("sync &");
		//�������
		if(s_special_params.iCurrentAlarmSwitch == 1 && s_special_params.iErrorDetectSwitch == 1)
		{
			RedExtinguish();
		}
		CurDetectFreq = 0;
	}
	return;
}



/*********************************************************************************
*
* 	���ſ����źš�
*
***********************************************************************************/

uint16 DoorCheckInput(void)
{
	uint16 input = 0;

	input = DoorCheck();
	printf("INFO 100142 �ſ����ź�: 0x%x\n",input); 


	return input;
}

/*********************************************************************************
*
* 	���̳�ͻ���̳�ͻ��⡣
*
***********************************************************************************/
void RedgreenCollision(int boardNum, uint16 boardInfo) 
{
	uint16 j = 0;
	static uint16 errorcountred[32] = {0};
	static uint16 normalcountred[32] = {0};
	static uint16 errorcountgreen[32] = {0};
	static uint16 normalcountgreen[32] = {0};
	time_t now;  
	struct tm *timenow;
	
	char msg[128] = "";

	printf("**************************boardno:%d,g_nLampStatus:%d,boardinfo:%d\n", boardNum, g_nLampStatus[boardNum-1], boardInfo);
	for (j=0; j<4; j++)
	{
		//���̳�ͻ���
		if ((((g_nLampStatus[boardNum-1]>>(j*3+1))&0x1) == 0)  && (((boardInfo>>(j*3+1))&0x1) == 1)) 
		{
			//��Ʋ�����������Ϊ���̳�ͻ
			//��ȡ���ʱ�׼ʱ��
			time(&now);   
			//ת��Ϊ����ʱ��
			timenow = localtime(&now); 

			sprintf(msg,"%04d.%02d.%02d-%02d:%02d:%02d",
				timenow->tm_year + 1900,timenow->tm_mon + 1,timenow->tm_mday,
				timenow->tm_hour,timenow->tm_min,timenow->tm_sec);
			printf("%s ��%dͨ����Ƶ�ѹ�쳣(���̳�ͻ����)��errorcountredgreen:%d\n", msg, (boardNum-1)*4+j+1, errorcountred[(boardNum-1)*4+j]);
			
			if (++errorcountred[(boardNum-1)*4+j] > TIMEOUT)
			{
				errorcountred[(boardNum-1)*4+j] = 0;
				normalcountred[(boardNum-1)*4+j] = 0;
				if(s_special_params.iVoltageAlarmAndProcessSwitch == 1)
				{
					g_errorstat = 1;
					SendSpecialCtrolUdpMsg(SPECIAL_CONTROL_YELLOW_BLINK);
				}
				if(g_faultstatus[(boardNum-1)*4+j] == 0)
				{
					g_failurenumber ++;
					printf("%s ��%dͨ�����̳�ͻ,errorcountredgreen:%d\n", msg, (boardNum-1)*4+j+1, errorcountred[(boardNum-1)*4+j]);
					sprintf(msg, "��%dͨ�����̳�ͻ", (boardNum-1)*4+j+1);
					WriteErrorInfos("/home/FaultLog.dat", msg);
					s_failure_info.nID = 0x0b;          //��Ϣ����ID
					s_failure_info.nNumber = g_failurenumber;       //���к�
					s_failure_info.nTime = now;
	  				s_failure_info.nValue = (boardNum-1)*4+j+1;     //ͨ����   
					WriteFailureInfos("/home/FailureLog.dat",s_failure_info);
					sprintf(msg, "���:%d,����:0x%x,ʱ��:%ld,ͨ��:%d",
							s_failure_info.nNumber,s_failure_info.nID,s_failure_info.nTime,s_failure_info.nValue);
					WriteErrorInfos("/home/FaultLog.dat", msg);
					set_failure_number(g_failurenumber);
					g_faultstatus[(boardNum-1)*4+j] = 2;
					//��g_faultstatus����д���ļ�
					WriteFaultStatus();
				}
			}		
		}
		else if ((((g_nLampStatus[boardNum-1]>>(j*3+1))&0x1) == 0)  && (((boardInfo>>(j*3+1))&0x1) == 0)) 
		{
			//����ĺ�ƶ�Ϩ���ˣ�����
			if(++normalcountred[(boardNum-1)*4+j] > TIMEOUT)
			{	
				normalcountred[(boardNum-1)*4+j] = 0;
				errorcountred[(boardNum-1)*4+j] = 0;
				if(g_faultstatus[(boardNum-1)*4+j] == 2)
				{
					//��ȡ���ʱ�׼ʱ��
					time(&now); 
					g_failurenumber++;
					sprintf(msg, "��%dͨ�����̳�ͻ��������", (boardNum-1)*4+j+1);
					WriteErrorInfos("/home/FaultLog.dat", msg);
					s_failure_info.nID = 0x0c;                      //���̳�ͻ�����Ϣ����ID
					s_failure_info.nNumber = g_failurenumber;       //���к�
					s_failure_info.nTime = now;
		  			s_failure_info.nValue = (boardNum-1)*4+j+1;     //ͨ����   
					WriteFailureInfos("/home/FailureLog.dat",s_failure_info);
					sprintf(msg, "���:%d,����:0x%x,ʱ��:%ld,ͨ��:%d",
							s_failure_info.nNumber,s_failure_info.nID,s_failure_info.nTime,s_failure_info.nValue);
					WriteErrorInfos("/home/FaultLog.dat", msg);
					set_failure_number(g_failurenumber);
					g_faultstatus[(boardNum-1)*4+j] = 0;
					WriteFaultStatus();
				}
				
			}
			
		}

		//�̳�ͻ���
		if ((((g_nLampStatus[boardNum-1]>>(j*3))&0x1) == 0) && (((boardInfo>>(j*3))&0x1) == 1)) //�̵Ʋ�����������Ϊ�̳�ͻ
		{
			//��ȡ���ʱ�׼ʱ��
			time(&now);  
			  
			//ת��Ϊ����ʱ��
			timenow = localtime(&now); 

			sprintf(msg,"%04d.%02d.%02d-%02d:%02d:%02d",
				timenow->tm_year + 1900,timenow->tm_mon + 1,timenow->tm_mday,
				timenow->tm_hour,timenow->tm_min,timenow->tm_sec);
			printf("%s ��%dͨ���̵Ƶ�ѹ�쳣��(�̳�ͻ����),errorcountgreen:%d\n", msg, (boardNum-1)*4+j+1, errorcountgreen[(boardNum-1)*4+j]);
			if (++errorcountgreen[(boardNum-1)*4+j] > TIMEOUT)
			{		
				errorcountgreen[(boardNum-1)*4+j] = 0;
				normalcountgreen[(boardNum-1)*4+j] = 0;
				
				if(s_special_params.iVoltageAlarmAndProcessSwitch == 1)
				{
					g_errorstat = 1;
					SendSpecialCtrolUdpMsg(SPECIAL_CONTROL_YELLOW_BLINK);
				}
				g_failurenumber++;
				printf("%s ��%dͨ���̳�ͻ,errorcountgreen:%d\n", msg, (boardNum-1)*4+j+1, errorcountgreen[(boardNum-1)*4+j]);
				sprintf(msg, "��%dͨ���̳�ͻ", (boardNum-1)*4+j+1);
				WriteErrorInfos("/home/FaultLog.dat", msg);
				s_failure_info.nID = 0x0f;                      //�̳�ͻ��Ϣ����ID
				s_failure_info.nNumber = g_failurenumber;       //���к�
				s_failure_info.nTime = now;
  				s_failure_info.nValue = (boardNum-1)*4+j+1;     //ͨ����   
				WriteFailureInfos("/home/FailureLog.dat",s_failure_info);
					sprintf(msg, "���:%d,����:0x%x,ʱ��:%ld,ͨ��:%d",
							s_failure_info.nNumber,s_failure_info.nID,s_failure_info.nTime,s_failure_info.nValue);
				WriteErrorInfos("/home/FaultLog.dat", msg);
				set_failure_number(g_failurenumber);
				g_faultstatus[(boardNum-1)*4+j] = 1;
				//��g_faultstatus����д���ļ�
				WriteFaultStatus();				
			}		
		}
		else if((((g_nLampStatus[boardNum-1]>>(j*3))&0x1) == 0) && (((boardInfo>>(j*3))&0x1) == 0))
		{
			//������̵ƶ����ˣ�����
			if(++normalcountgreen[(boardNum-1)*4+j] > TIMEOUT)
			{	
				normalcountgreen[(boardNum-1)*4+j] = 0;
				errorcountgreen[(boardNum-1)*4+j] = 0;
				if(g_faultstatus[(boardNum-1)*4+j] == 1)
				{
					//��ȡ���ʱ�׼ʱ��
					time(&now); 
					g_failurenumber++;
					sprintf(msg, "��%dͨ���̳�ͻ��������", (boardNum-1)*4+j+1);
					WriteErrorInfos("/home/FaultLog.dat", msg);
					s_failure_info.nID = 0x10;                      //���̳�ͻ�����Ϣ����ID
					s_failure_info.nNumber = g_failurenumber;       //���к�
					s_failure_info.nTime = now;
		  			s_failure_info.nValue = (boardNum-1)*4+j+1;     //ͨ����   
					WriteFailureInfos("/home/FailureLog.dat",s_failure_info);
					sprintf(msg, "���:%d,����:0x%x,ʱ��:%ld,ͨ��:%d",
							s_failure_info.nNumber,s_failure_info.nID,s_failure_info.nTime,s_failure_info.nValue);
					WriteErrorInfos("/home/FaultLog.dat", msg);
					set_failure_number(g_failurenumber);
					g_faultstatus[(boardNum-1)*4+j] = 0;
					WriteFaultStatus();
				}
				
			}
			
		}
	}

}


/*********************************************************************************
*
* 	���Ϩ���⡣
*
***********************************************************************************/
void RedExtinguish() //��Ƹ�����������Ϊ���Ϩ��
{
	uint16 i = 0;
	uint16 j = 0;
	uint16 pcurInfo = 0;
	static uint16 errorcountred[32] = {0};
	static uint16 normalcountred[32] = {0};
	char msg[128] = "";
	time_t now; 
	for (i=0; i<8; i++)
	{
		for (j=0; j<4; j++)
		{
			pcurInfo = i_can_its_get_cur(i+1, j+1, 1);   //�ڣ�������д����,ֻ��ȡ��Ƶ���
			//pcurInfo = 100;
			g_RedCurrentValue[i*4+j] = pcurInfo;
			printf("ͨ��%02d��Ƶ���ֵ:%03d\n",i*4+j+1,pcurInfo);
			
			if ((g_nLampStatus[i] & (1<<(1+j*3))) > 0 && pcurInfo < (g_struRecCurrent[i*4+j].RedCurrentBase - g_struRecCurrent[i*4+j].RedCurrentDiff) )
			{	
				printf("ͨ��%02d��������Ҽ������쳣,errorcountred[%d]=%d\n",i*4+j+1,i*4+j+1,errorcountred[i*4+j]+1);
				//��������ҵ�������쳣
				if (++errorcountred[i*4+j] > 10)
				{
					errorcountred[i*4+j] = 0;
					normalcountred[i*4+j] = 0;
					if(s_special_params.iCurrentAlarmAndProcessSwitch == 1)
					{
						//g_yellowflash = 1;
						//���Ϊ�й���
						g_errorstat = 1;
						SendSpecialCtrolUdpMsg(SPECIAL_CONTROL_YELLOW_BLINK);
					}
					if(g_faultstatus[i*4+j] == 0)
					{
						//��ȡ���ʱ�׼ʱ��
						time(&now); 
						
						
						g_failurenumber ++;
						printf("��%dͨ�����Ϩ��\n", i*4+j+1);
						sprintf(msg, "��%dͨ�����Ϩ��", i*4+j+1);
						WriteErrorInfos("/home/FaultLog.dat", msg);
						s_failure_info.nID = 0x0d;                      //���Ϩ����Ϣ����ID
						s_failure_info.nNumber = g_failurenumber;       //���к�
						s_failure_info.nTime = now;
	  					s_failure_info.nValue = i*4+j+1;     //ͨ����   
						WriteFailureInfos("/home/FailureLog.dat",s_failure_info);
						sprintf(msg, "���:%d,����:0x%x,ʱ��:%ld,ͨ��:%d",
							s_failure_info.nNumber,s_failure_info.nID,s_failure_info.nTime,s_failure_info.nValue);
						WriteErrorInfos("/home/FaultLog.dat", msg);
						set_failure_number(g_failurenumber);
						g_faultstatus[i*4+j] = 3;
						//��g_faultstatus����д���ļ�
						WriteFaultStatus();
						//return;
					}
				}
				//return;
			}
			else if((g_nLampStatus[i] & (1<<(1+j*3))) > 0 && pcurInfo > (g_struRecCurrent[i*4+j].RedCurrentBase - g_struRecCurrent[i*4+j].RedCurrentDiff))
			{
				
				//��ͨ��������������ҵ���ֵ����
				if (++normalcountred[i*4+j] > 10)
				{
					normalcountred[i*4+j] = 0;
					errorcountred[i*4+j] = 0;
					//��ͨ�����д�����������
					if (g_faultstatus[i*4+j] == 3)
					{
						time(&now); 
						sprintf(msg, "��%dͨ�����Ϩ���������", i*4+j+1);
						WriteErrorInfos("/home/FaultLog.dat", msg);
						g_failurenumber ++;
						s_failure_info.nID = 0x0e;                      //���Ϩ��������Ϣ����ID
						s_failure_info.nNumber = g_failurenumber;       //���к�
						s_failure_info.nTime = now;
			  			s_failure_info.nValue = i*4+j+1;     //ͨ����   
						WriteFailureInfos("/home/FailureLog.dat",s_failure_info);
						sprintf(msg, "���:%d,����:0x%x,ʱ��:%ld,ͨ��:%d",
									s_failure_info.nNumber,s_failure_info.nID,s_failure_info.nTime,s_failure_info.nValue);
						WriteErrorInfos("/home/FaultLog.dat", msg);
						set_failure_number(g_failurenumber);
						g_faultstatus[i*4+j] = 0;
						WriteFaultStatus();
					}
				}
			}
		}
	}
}
unsigned char VehicleCheckFaultInput(unsigned char phase)
{
	return  0; 
}


/*********************************************************************************
*
* 	����λ�����ֵ��(����)
*
***********************************************************************************/

unsigned int PhaseLampCurInput(int boardNum, int pahseNum, int redGreen)
{
	if (boardNum<1 || boardNum>8 || pahseNum<1 || pahseNum>4)
	{
		printf("boardNum or pahseNum error:%d, %d\n", boardNum, pahseNum);
		return 0;
	}
	uint16 curInfo = 0;
	if (g_errorstat == 1)
	{
		return curInfo;
	}
	printf("INFO 100132:%d  %d   %d\n",boardNum, pahseNum, redGreen); 
	curInfo = i_can_its_get_cur(boardNum, pahseNum, redGreen);
	printf("��λ�����ֵ:%d\n", curInfo);

	//RedExtinguish();

	curInfo &= 0x00ff;
	if (curInfo == 0xff)
	{
		curInfo = 0;
	}

	return curInfo;
}

/*********************************************************************************
*
* 	�����˰�ť�źš�
*
***********************************************************************************/

uint16 PedestrianCheckInput(void)
{
	uint16 input = PedestrianCheck();

	printf("INFO 100141 ���˰�ť: 0x%x\n",input); 


	return input;
}


/*********************************************************************************
*
*  ���̴���
	����ֵ:
	0: �޼�����.
	1:�Զ�������,���ѷſ�.
	2:�ֶ�������,���ѷſ�.
	3:����������,���ѷſ�.
	4:ȫ�������,���ѷſ�.
	5:����������,���ѷſ�.
*
***********************************************************************************/
int ProcessKey(void)
{
	int result = 0;
	char tmpstr[256] = "";
	result = ProcessKeyBoard();
	
	printf("INFO 100600: ���̴��� return %d,manual_pressed=%d,auto_pressed=%d,flashing_pressed=%d,allred_pressed=%d,step_by_step_pressed=%d \n",
		result,manual_pressed,auto_pressed,
		flashing_pressed,allred_pressed,step_by_step_pressed);

	if(result != 0)
	{
		sprintf(tmpstr,"���̴��� return %d,�ֶ�=%d,�Զ�=%d,����=%d,ȫ��=%d,����=%d \n",
			result,manual_pressed,auto_pressed,
			flashing_pressed,allred_pressed,step_by_step_pressed);
	
		WriteLogInfos("/home/Keyboard.log",tmpstr);
	}
	
	return result;
	//return (0);
}

/*********************************************************************************
*
* 	д����������־��
*
***********************************************************************************/
void write_running_log()
{
	//д������־(ע:�ļ����ϼ�Ŀ¼�����Ѿ����ڣ������޷�����д������־)
	char msg[128] = "";
	sprintf(msg,"�źŻ�����ʼ����,��ǰ����汾��:%s",SOFTWARE_VERSION_INFO);
	WriteLogInfos("/home/StartUp.log",msg);
	ReadFaultStatus();
}

int itstaskmain(int argc, char *argv[]);

void SigHandler(int signum) //kill or killall will be called to free memory!
{
    INFO("signum = %d", signum);
    exit(0);
}
//It can be called after main() or exit() automatically!
__attribute__((destructor)) void ProgramExit(void)
{
    if (gSignalControlpara == NULL)
        free(gSignalControlpara);
    if (gCountDownParams == NULL)
        free(gCountDownParams);
    if (gHandle == NULL)
        free(gHandle);
    INFO("The program exit unnormal!");
}

/*********************************************************************************
*
* 	���������
*
***********************************************************************************/
int main(int argc, char *argv[])
{	
	INFO("********************************main()****************************\n");
	INFO("compile time: %s, %s", __DATE__, __TIME__);
	signal(SIGTERM, SigHandler);    //for command 'kill' or 'killall'
	//д����������־����¼�����汾��
	write_running_log();
	//�������ļ��л�ȡ���в�������
	InitCountDownParams();//����Ҫ�ȸ�����ʱ����ռ䣬�ٶ��ļ����������ɶδ���
	get_all_params_from_config_file();
	InitSignalMachineConfig();
	StepPthreadInit();

	//canͨ�ų�ʼ��
	i_can_its_init();
	
	//CPUͨ��CPLD�����IO�ڳ�ʼ��
	CPLD_IO_Init();

	//�������udp��������
	udp_server_init();
	
	//����ʱ�ƽ���
	//count_down_thread_create();

	//Ĭ�ϵ����Զ���
	ProcessKeyBoardLight();

	//�������Ź���ʼ��
	HardwareWatchdogInit();

	//GPS��ʼ��
	GPSInit();

	//��sadp����������
	system("killall -9 RTSC_sadp");
	system("sleep 5 && /root/RTSC_sadp &");


	return itstaskmain(argc, argv);
}


/*********************************************************************************
*
* 	����ʱ�䵽Ӳ��RTC
*
***********************************************************************************/

void RTC_HW_fc(void)
{
	system("hwclock -w&");
	printf("rtc hw fc.\n");
}


/*����color: 1:�̵ƣ�2:��ƣ�3:�Ƶ�*/
void CountDownOutVeh(unsigned char phase, unsigned char color, unsigned char phaseTime)
{
	
	printf("veh:phase %d color %d phaseTime %d \n", phase, color, phaseTime);

    static unsigned char cIsFirstTime = 0;
    if((cIsFirstTime == 0) && (gSpecialControlSchemeId >= 251))
    {
        cIsFirstTime = 1;
        SendSpecialCtrolUdpMsg(gSpecialControlSchemeId);
    }
	countdown_veh[phase].veh_color = color;
	countdown_veh[phase].veh_phaseTime = phaseTime;
	gCountDownParams->stVehPhaseCountingDown[phase - 1][0] = color;
	gCountDownParams->stVehPhaseCountingDown[phase - 1][1] = phaseTime;
	CollectCountDownParams();
	
	/*
	if(SetFreeGreenTimeSign == 0)
	{
		SeFreeGreenTime(3);//��Ӧ���ʱ��
		SetFreeGreenTimeSign = 1;
	}*/
}

void CountDownOutPed(unsigned char phase, unsigned char color, unsigned char phaseTime)
{
	printf("ped:phase %d color %d phaseTime %d \n", phase, color, phaseTime);
	countdown_ped[phase].ped_color = color;
	countdown_ped[phase].ped_phaseTime = phaseTime;
	gCountDownParams->stPedPhaseCountingDown[phase - 1][0] = color;
	gCountDownParams->stPedPhaseCountingDown[phase - 1][1] = phaseTime;
	CollectCountDownParams();
}
//����ȫ�̵���ʱ
void Count_Down_pulse_All(uint16 *g_nLampStatus)
{
	int iPhase = 0;
	int iChannel = 0;
	int dataNum1 = 0;
	int dataNum2 = 0;

	memcpy(temp_nLampStatus,g_nLampStatus,16);
	for(iPhase = 1; iPhase<16; iPhase++)
	{
		if(countdown_veh[iPhase].veh_color > 0)
		{
			if(countdown_veh[iPhase].veh_color == 2)
			{
				nowRedLight[iPhase] = 1;//���
			}
			else
			{
				nowRedLight[iPhase] = 0;//���Ǻ��
			}
			if(nowRedLight[iPhase] != lastRedLight[iPhase])
			{
				stayTimes[iPhase]++;
				if(stayTimes[iPhase] == 2)
				{
					lastRedLight[iPhase] = nowRedLight[iPhase];
					for(iChannel=0; iChannel<32; iChannel++)
					{
						if(g_struCountDown.iPhaseOfChannel[iChannel].iphase == iPhase)
						{
							dataNum1 = iChannel/4;
							dataNum2 = iChannel%4;
							temp_nLampStatus[dataNum1] = temp_nLampStatus[dataNum1] &( ~(1<<(dataNum2*3)));
							temp_nLampStatus[dataNum1] = temp_nLampStatus[dataNum1] &( ~(1<<(dataNum2*3+1)));
							temp_nLampStatus[dataNum1] = temp_nLampStatus[dataNum1] &( ~(1<<(dataNum2*3+2)));
						}
					}
				}
			}
			else
			{
				stayTimes[iPhase] = 0;//������λ
			}
		}
	}
	memcpy(g_nLampStatus,temp_nLampStatus,16);
}
//�����̵���ʱ
void Count_Down_pulse_Half(uint16 *g_nLampStatus)
{
	int iPhase = 0;
	int iChannel = 0;
	int dataNum1 = 0;
	int dataNum2 = 0;

	memcpy(temp_nLampStatus,g_nLampStatus,16);
	for(iPhase = 1; iPhase<16; iPhase++)
	{
		if(countdown_veh[iPhase].veh_color > 0)
		{
			if(((countdown_veh[iPhase].veh_phaseTime == g_struCountDown.iPulseGreenTime) && (countdown_veh[iPhase].veh_color == 1))
			||((countdown_veh[iPhase].veh_phaseTime == g_struCountDown.iPulseRedTime) && (countdown_veh[iPhase].veh_color == 2)))//��̵���ʱ
			{
				if(temp_Sign[iPhase] == 0)
				{
					for(iChannel=0; iChannel<32; iChannel++)
					{
						if(g_struCountDown.iPhaseOfChannel[iChannel].iphase== iPhase)
						{
							dataNum1 = iChannel/4;
							dataNum2 = iChannel%4;
							temp_nLampStatus[dataNum1] = temp_nLampStatus[dataNum1] &( ~(1<<(dataNum2*3)));
							temp_nLampStatus[dataNum1] = temp_nLampStatus[dataNum1] &( ~(1<<(dataNum2*3+1)));
							temp_nLampStatus[dataNum1] = temp_nLampStatus[dataNum1] &( ~(1<<(dataNum2*3+2)));
						}
					}
					temp_Sign[iPhase] = 1;
				}
			}
			else
			{
				temp_Sign[iPhase] = 0;//��־λ��λ
			}
		}
	}
	memcpy(g_nLampStatus,temp_nLampStatus,16);
}
