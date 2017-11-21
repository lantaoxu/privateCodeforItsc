#include "countDown_Hisense.h"
#include "configureManagement.h"


extern PHASE_COUNTING_DOWN_FEEDBACK_PARAMS *gCountDownParams;       //����ʱ�ӿ���Ϣ�������ڵ���ʱ�ӿ��м���
extern CountDownCfg        g_CountDownCfg;                         //ȫ�ֲ�������ŵ���ͨѶЭ����Ҫ�����ݣ������ݱ������ļ��У�ͨ�������ļ������޸ġ�����
extern STRUCT_BINFILE_CUSTOM gStructBinfileCustom;      //�����/home/custom.dat�����е����нṹ��,������Ե���ʱ��Э�������/��Դ��ڲ���������/���ͨ����������������/ͨ��������ʶ

static void SetCountValueHisenseStandard(unsigned char cDeviceId,char *buf)
{
    unsigned char cPhaseCountDownTime = 0;
    unsigned char cPhaseColor = 0;
    unsigned char cDataLen = 0;
    
    GetRuningPhaseId(cDeviceId,&cPhaseCountDownTime,&cPhaseColor);    

	if((gStructBinfileCustom.cIsCountdownValueLimit == 1) && (gStructBinfileCustom.sCountdownParams.iFreeGreenTime < cPhaseCountDownTime))//���ĳ��λ�ĵ���ʱʱ��ȸ�Ӧ���ʱ�������ʾʱ�����㣬������ʾ�������Ҫʵʱ��ʾ����ʱ״̬����ô����ͨ�����ù��ߣ�����Ԫ�����еĸ�Ӧ���ʱ�����Щ��
	{
		return;
	}
	
    strcpy(buf,"$SIGN,");//start 
    sprintf(buf+strlen(buf),"%d,",cDeviceId+1);

    if(g_CountDownCfg.cControllerType[cDeviceId] == PEDESTRIAN)//pedestrian type
    {
        strcat(buf,"P,");
    }
    else
    {
        strcat(buf,"V,");
    }
    
    if((GREEN == cPhaseColor) || (GREEN_BLINK == cPhaseColor))//1 0
    {
        strcat(buf,"G,");
    }
    else if(RED == cPhaseColor)//0 1
    {
        strcat(buf,"R,");
    }
    else if((YELLOW == cPhaseColor) || (YELLOW_BLINK == cPhaseColor))//0 0 
    {
        strcat(buf,"Y,");
    }
    else
    {
        strcat(buf,"W,");
    }
    
    sprintf(buf+strlen(buf),"%d:",cPhaseCountDownTime);

    cDataLen = strlen(buf) - strlen("$SIGN");

    sprintf(buf+strlen(buf),"%x\r\n",cDataLen);

}

static void countDownByHisenseStandard()
{
    int i = 0;
    char buf[48] = {0};
    
    for(i = 0; i < MAX_NUM_COUNTDOWN; i++)
    {
        if(g_CountDownCfg.cControllerType[i] != 0)//�õ���ʱ������
        {
            memset(buf,0,sizeof(buf));
            SetCountValueHisenseStandard(g_CountDownCfg.cDeviceId[i],buf);
            
            //INFO("countDownByHisenseStandard  %s\n",buf);

            Send485Data((unsigned char *)buf,strlen(buf));
        }
    }
}


void HisenseStandardCountDownRun()
{
    SetCountDownValue(gCountDownParams);//ÿ�����������λ�ĵ���ʱ��Ϣ

    countDownByHisenseStandard();

    //PrintDataNationStandard();
}


