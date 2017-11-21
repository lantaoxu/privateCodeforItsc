/******************************************************************************

                  ��Ȩ���� (C), 2001-2015, ���ݺ����������ּ����ɷ����޹�˾

 ******************************************************************************
  �� �� ��   : HikConfig.c
  �� �� ��   : ����
  ��    ��   : Ф�Ļ�
  ��������   : 2014��12��2��
  ����޸�   :
  ��������   : ���ļ���ɶ������ļ����ж�ȡ������У��Ľӿ�
  �����б�   :
              ArrayToInt
              checkPhaseTrunInRing
              DateTimeCmp
              find_next_index
              GetCircleTime
              IsArrayRepeat
              IsBarrierGreenSignalRationEqual
              IsItemInCharArray
              IsItemInShortArray
              IsPhaseContinuousInPhaseTurn
              isPhaseInArray
              IsSignalControllerParaIdLegal
              IsSignalControlparaLegal
              LoadDataFromCfg
              log_error
  �޸���ʷ   :
  1.��    ��   : 2014��12��2��
    ��    ��   : Ф�Ļ�
    �޸�����   : �����ļ�

  2.��    ��   : 2014��12��3��
    ��    ��   : Ф�Ļ�
    �޸�����   : �����µĽṹ�壬�޸���ؽӿ�
******************************************************************************/
/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/
#include "parse_ini.h"
#include "HikConfig.h"
#include <stdarg.h>
/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/
#define COL_DEF "\033[m"
#define COL_RED "\033[0;32;31m"
#define COL_GRE "\033[0;32;32m"
#define COL_BLU "\033[0;32;34m"
#define COL_YEL "\033[1;33m"

//�Ƿ������Ĵ����ӡ
#define LOG_ERR_CHN


#define DEG(fmt,...) fprintf(stdout,"HikConfig library debug : "fmt "\n",##__VA_ARGS__)



/*----------------------------------------------*
 * �ⲿ����˵��                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �ⲿ����ԭ��˵��                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ȫ�ֱ���                                     *
 *----------------------------------------------*/
char *gErrorContent = NULL;//��ȫ�ֱ����洢������Ϣ���������߽��з���

static unsigned char cArrayGreenSignalRation[NUM_GREEN_SIGNAL_RATION] = {0};//�洢����ʵ�ʱ�������ʹ�õ����űȱ�ID

/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/

/*****************************************************************************
 �� �� ��  : log_error
 ��������  : ��ӡ������Ϣ�ӿڣ����д������ݻ� ���浽ȫ�ֻ��������ⲿ�ӿڿ�ֱ
             �Ӳ鿴�����ݡ�
 �������  : const char* format  
             ...                 
 �� �� ֵ  : 
 �޸���ʷ  
  1.��    ��   : 2014��12��2��
    ��    ��   : Ф�Ļ�
    �޸�����   : �����ɺ���
  2.��    ��   : 2014��12��10��
    ��    ��   : Ф�Ļ�
    �޸�����   : �޸Ļ�������СΪ1024�ֽ�
  3.��    ��   : 2014��12��10��
    ��    ��   : Ф�Ļ�
    �޸�����   : �����strcat�����ַ�����Ϊһ��sprintf���������ַ���	
*****************************************************************************/
static void log_error(const char* format, ...)
{
#ifndef LOG_ERR_CHN    
	char buff[1024] = {0};
    if(NULL == gErrorContent)
    {
        gErrorContent = calloc(1024,1);

    }
	sprintf(buff,"%s HikConfig library error: %s <ERROR>  ",COL_BLU,COL_YEL);
	va_list argptr;
	va_start(argptr, format);

    if(NULL != gErrorContent)
    {
    	memset(gErrorContent,0,1024);
    	vsnprintf(gErrorContent,1024,format,argptr);
    }

	vsnprintf(buff+strlen(buff),sizeof(buff)-strlen(buff)-1, format, argptr);//��buff��λ�������ƶ�strlen(buff)�ֽڣ���ǰ����24�ֽ�������ŵ�ǰʱ�䣬��������2�ֽڵĿո������ָ����ݺ�ʱ��
	va_end(argptr);
	strcat(buff,COL_DEF);
	strcat(buff,"\n");
	fprintf(stderr,"%s",buff);
#endif
}

/*****************************************************************************
 �� �� ��  : log_error_cn
 ��������  : ��ӡ������Ϣ�ӿڣ����д������ݻ� ���浽ȫ�ֻ��������ⲿ�ӿڿ�ֱ
             �Ӳ鿴�����ݡ�
 �������  : const char* format  
             ...                 
 �� �� ֵ  : 
 �޸���ʷ  
  1.��    ��   : 2014��12��2��
    ��    ��   : Ф�Ļ�
    �޸�����   : �����ɺ���
  2.��    ��   : 2014��12��10��
    ��    ��   : Ф�Ļ�
    �޸�����   : �޸Ļ�������СΪ1024�ֽ�
  3.��    ��   : 2014��12��10��
    ��    ��   : Ф�Ļ�
    �޸�����   : �����strcat�����ַ�����Ϊһ��sprintf���������ַ���	
*****************************************************************************/
static void log_error_cn(const char* format, ...)
{
#ifdef LOG_ERR_CHN
	char buff[1024] = {0};
    if(NULL == gErrorContent)
    {
        gErrorContent = calloc(1024,1);

    }
	sprintf(buff,"%s HikConfig library error: %s <ERROR>  ",COL_BLU,COL_YEL);
	va_list argptr;
	va_start(argptr, format);

    if(NULL != gErrorContent)
    {
    	memset(gErrorContent,0,1024);
    	vsnprintf(gErrorContent,1024,format,argptr);
    }

	vsnprintf(buff+strlen(buff),sizeof(buff)-strlen(buff)-1, format, argptr);//��buff��λ�������ƶ�strlen(buff)�ֽڣ���ǰ����24�ֽ�������ŵ�ǰʱ�䣬��������2�ֽڵĿո������ָ����ݺ�ʱ��
	va_end(argptr);
	strcat(buff,COL_DEF);
	strcat(buff,"\n");
	fprintf(stderr,"%s",buff);
#endif
}




/*****************************************************************************
 �� �� ��  : GetCircleTime
 ��������  : �������űȺź�����ŵõ�������У����Ų�Ϊ0�����ű�ʱ��֮��
 �������  : SignalControllerPara *pSignalControlpara  
             unsigned char nGreenSignalRationId        
             unsigned char nPhaseTurnId                
 �� �� ֵ  : 
 �޸���ʷ  
  1.��    ��   : 2014��12��2��
    ��    ��   : Ф�Ļ�
    �޸�����   : �����ɺ���

*****************************************************************************/
static int GetCircleTime(SignalControllerPara *pSignalControlpara,unsigned char nGreenSignalRationId,unsigned char nPhaseTurnId)
{

    int i = 0; 
    int j = 0;

    int temp = 0;

    for(i = 0 ; i < 4; i++)//��ѯ��
    {
        if(pSignalControlpara->stPhaseTurn[nPhaseTurnId - 1][i].nPhaseTurnID == 0)//�ҵ���ʵ�����ݵĻ�
        {
			//log_error_cn("turn %d, i %d\n",nPhaseTurnId,i);
            continue;

        }    
        
        for(j = 0 ; j < NUM_PHASE; j++)
        {
			
            if(pSignalControlpara->stPhaseTurn[nPhaseTurnId - 1][i].nTurnArray[j] == 0)//��ѯһ������ֱ�ӽ���
            {
				//log_error_cn("===> turn %d,i %d, j %d\n",nPhaseTurnId,i,j);
                return temp;
            }
			//log_error_cn("nGreenSignalRationId %d,nPhaseTurnId %d, i %d, j %d, %d\n",nGreenSignalRationId,nPhaseTurnId,i,j,pSignalControlpara->stGreenSignalRation[nGreenSignalRationId - 1][pSignalControlpara->stPhaseTurn[nPhaseTurnId - 1][i].nTurnArray[j] - 1].nGreenSignalRationTime);
            
            temp += pSignalControlpara->stGreenSignalRation[nGreenSignalRationId - 1][pSignalControlpara->stPhaseTurn[nPhaseTurnId - 1][i].nTurnArray[j] - 1].nGreenSignalRationTime;

        }

    }
	
    return  temp;
}



/*****************************************************************************
 �� �� ��  : IsBarrierGreenSignalRationEqual
 ��������  : �ж�һ��������������ű�ʱ���Ƿ���ͬ
 �������  : SignalControllerPara *pSignalControlpara  
             unsigned char *cArray                     
             int len                                   
             unsigned char nCircleId                   
 �������  : ��
 
 �޸���ʷ      :
  1.��    ��   : 2014��8��8��
    ��    ��   : xiaowh
    �޸�����   : �����ɺ���

*****************************************************************************/
static int IsBarrierGreenSignalRationEqual(SignalControllerPara *pSignalControlpara,unsigned char *cArray,int len,unsigned char nCircleId)
{
    int i = 0;
    int j = 0;
    int k = 0;
    int m = 0;
    
    int temp = 0;
    int temp2 = 0;
    int temp3 = 0;

    unsigned char cTempArray[4][NUM_PHASE];//�ĸ�����ÿ�������num_phase����λ

    memset(cTempArray,0,sizeof(cTempArray));

    //��������λ���飬���ջ��Ĵ�С˳��
    for(i = 0 ; i < len; i++)
    {
        if(cArray[i] == 0)
        {
            break;
        }

        temp = pSignalControlpara->stPhase[cArray[i] - 1].nCircleID;//�ҵ�����

        cTempArray[temp - 1][cArray[i] - 1] = cArray[i];

       // DEG("===>  cTeampArray   %d\n",cTempArray[temp - 1][cArray[i] - 1]);
    }
#if 1
    //һ�����ڵ���λ�Ĳ�����λ���ű�֮�ͣ�Ӧ�ú͸û�����λ�����ű�֮�����
    for(m = 0 ; m < NUM_GREEN_SIGNAL_RATION; m++)//��ѯ���űȱ����ҵ�ĳ����λ�����ű�ʱ��
    {
        temp = 0;
        temp2 = 0;
        temp3 = 0;
        
		if(cArrayGreenSignalRation[m] == 0)//������ű�û�б����ã���������
		{
			continue;
		}
		
        for(i = 0 ; i < 4; i++)//��ѯ��
        {
            temp = 0;
            temp2 = 0;
            
            for(j = 0 ;  j < NUM_PHASE; j++)//��ѯ���������λ
            {
                if(cTempArray[i][j] == 0)
                {
                    continue;
                }

    			if(pSignalControlpara->stGreenSignalRation[m][cTempArray[i][j] - 1].nGreenSignalRationID == 0)
    			{
    				continue;
    			}

    			temp += pSignalControlpara->stGreenSignalRation[m][cTempArray[i][j] - 1].nGreenSignalRationTime;//������λ�ţ��ҵ�����λ�����ű�ʱ��

    			if(temp2 == 0)
    			{
    				for(k = 0; k < NUM_PHASE; k++)//��ѯ����λ�Ĳ�����λ
    				{
    					temp3 = pSignalControlpara->stPhase[cTempArray[i][j] - 1].byPhaseConcurrency[k];//�ҵ�����λ��Ӧ�Ĳ�����λ
    					if(temp3 == 0)
    					{
    						break;
    					}

    					if(pSignalControlpara->stPhase[temp3 - 1].nCircleID == nCircleId)//����ò�����λ�Ļ��ź��β���ͬ�����������ű�֮��
    					{
    						temp2 += pSignalControlpara->stGreenSignalRation[m][temp3 - 1].nGreenSignalRationTime;//�󲢷���λ�����ű�֮��
    					}
    				    //DEG("====>temp %d temp2   %d   phase  %d\n",temp,temp2,temp3);
    				}
    			}
                //DEG("=========  temp  %d\n",temp);

    		}
    		
            if(temp != temp2)
            {
				//log_error_cn("GreenSignalRationId  %d\n",m+1);
                return  1;
            }
        }
    }
#endif    
    return 0;
}

/*****************************************************************************
 �� �� ��  : IsItemInShortArray
 ��������  : �ж�ĳ�����Ƿ���һ��short�������С�
 �������  : unsigned short *array  
             int length             
             int val                
 �� �� ֵ  : 
 �޸���ʷ  
  1.��    ��   : 2014��12��2��
    ��    ��   : Ф�Ļ�
    �޸�����   : �����ɺ���

*****************************************************************************/
static int IsItemInShortArray(unsigned short *array,int length,int val)
{
	if(!array)
	{
		return False;
	}

	int i = 0;
	for(i = 0 ; i < length ; i++)
	{
		if(array[i] == val)
		{
			return TRUE;
		}

	}

	return FALSE;
}

/*****************************************************************************
 �� �� ��  : IsItemInCharArray
 ��������  : �ж�һ��char�������Ƿ���һ��char�������С�
 �������  : unsigned char *array   
             unsigned short length  
             unsigned short val     
 �� �� ֵ  : 
 �޸���ʷ  
  1.��    ��   : 2014��12��2��
    ��    ��   : Ф�Ļ�
    �޸�����   : �����ɺ���

*****************************************************************************/
static int IsItemInCharArray(unsigned char *array,unsigned short length,unsigned short val)
{
	if(!array)
	{
		return FALSE;
	}

	unsigned short i = 0;
	for(i = 0 ; i < length ; i++)
	{
		if(array[i] == val)
		{
			return i+1;
		}

	}
	return FALSE;

}

/*****************************************************************************
 �� �� ��  : IsPhaseContinuousInPhaseTurn
 ��������  : �ж�������е������Ƿ��ղ�����λ��ָ���Ĳ���˳��
 �������  : SignalControllerPara *pSignalControlpara  
             unsigned char nCircleId                   
             unsigned char *cArray                     
             int len                                   
 �������  : ��
 
 �޸���ʷ      :
  1.��    ��   : 2014��8��8��
    ��    ��   : xiaowh
    �޸�����   : �����ɺ���

*****************************************************************************/
static int IsPhaseContinuousInPhaseTurn(SignalControllerPara *pSignalControlpara,unsigned char nCircleId,unsigned char *cArray,int len)
{
    int i = 0;
    int j = 0;

    unsigned short sTemp = 0;
    unsigned short sTemp2 = 0;
    unsigned char num = 0;
    //�������ݵĸ���
    for(i = 0 ; i < len; i++)
    {
       // DEG("===>   %d  :   %d  len %d\n",cArray[i],pSignalControlpara->stPhase[cArray[i] - 1].nCircleID,len);
        if((cArray[i] != 0) && (pSignalControlpara->stPhase[cArray[i] - 1].nCircleID == nCircleId))
        {
            sTemp2++;
        }
    }

    for(i = 0 ; i < NUM_PHASE_TURN; i++)
    {
        sTemp = pSignalControlpara->stPhaseTurn[i][nCircleId -1].nPhaseTurnID ;
        if(sTemp == 0)
        {
            continue;
        }

        for(j = 0 ; j < NUM_PHASE; j++)
        {
            sTemp = pSignalControlpara->stPhaseTurn[i][nCircleId -1].nTurnArray[j];
            if(sTemp == 0)
            {
                break;
            }
           // DEG("===>  stemp   %d\n",sTemp);
            if(IsItemInCharArray(cArray,len, sTemp) > 0)
            {
                num++;

                if(num == sTemp2)
                {
                    return 1;
                }
            }
            else
            {
                num = 0;
            }

        }
    }
   // DEG("===> circleId   %d  cArray[0]   %d  sTemp2  %d  num  %d\n",nCircleId,cArray[0],sTemp2,num);
    return 0;
}

/*****************************************************************************
 �� �� ��  : IsArrayRepeat
 ��������  : �ж������Ƿ����ظ�Ԫ��
 �������  : unsigned char *cArray  
             int len                
 �������  : ��
 
 �޸���ʷ      :
  1.��    ��   : 2014��8��8��
    ��    ��   : xiaowh
    �޸�����   : �����ɺ���

*****************************************************************************/
static int IsArrayRepeat(unsigned char *cArray,int len)
{
    int i = 0;
    int j = 0;

    unsigned short cTemp = 0;
        
    for(i = 0 ; i < len ; i++)
    {
        cTemp = cArray[i];

        if(cTemp == 0)
        {
            continue;
        }

        for(j = i+1 ; j < len ; j++)
        {
            if(cTemp == cArray[j])
            {
                return 1;
            }
        }
    }

    return 0;
}

/*****************************************************************************
 �� �� ��  : IsSignalControllerParaIdLegal
 ��������  : ����������������������жϲ������±��Ƿ���������
 �������  : SignalControllerPara *pSignalControlpara  
             unsigned short *nPhaseArray               
             unsigned short *nFollowPhaseArray         
 �������  : ��
 
 �޸���ʷ      :
  1.��    ��   : 2014��8��8��
    ��    ��   : xiaowh
    �޸�����   : �����ɺ���

  2.��    ��   : 2014��12��3��
    ��    ��   : Ф�Ļ�
    �޸�����   : �����µĽṹ�壬����У��


  3.��    ��   : 2015��2��6��
    ��    ��   : Ф�Ļ�
    �޸�����   : �޸��жϸ����Ƿ�Ϊ�յķ���
*****************************************************************************/
static int IsSignalControllerParaIdLegal(SignalControllerPara *pSignalControlpara,unsigned short *nPhaseArray,unsigned short *nFollowPhaseArray)
{
    int i = 0;
    int j = 0;
    
    unsigned short sTemp = 0;
    unsigned short sTemp2 = 0;

    //��λ�����±��Ƿ����λ����ͬ
    for(i = 0 ; i < NUM_PHASE; i++)
    {
        nPhaseArray[i] = pSignalControlpara->stPhase[i].nPhaseID;
        if((IS_PHASE_INABLE(pSignalControlpara->stPhase[i].wPhaseOptions) == 1) &&(nPhaseArray[i] != (i+1)))
        {
            log_error("%s:%d stPhase[%d]  subscript is not equal to ID \n",__func__,__LINE__,i+1);
            log_error_cn("ErrorCode: ERROR_SUBSCRIPT_PHASE , Error Content: ��λ%d�±겻���Ϲ淶. \n",i+1);
            return ERROR_SUBSCRIPT_PHASE;
        }
    }

    for(i = 0 ; i < NUM_PHASE; i++)
    {
        //������λ�ظ�
        if(IsArrayRepeat(pSignalControlpara->stPhase[i].byPhaseConcurrency,NUM_PHASE) == 1)
        {
            log_error("%s:%d stConcurrentPhase[%d]  has repeat items \n",__func__,__LINE__,i+1);
            log_error_cn("ErrorCode: ERROR_REPEATE_CONCURRENT_PHASE , Error Content: ��λ%d�Ĳ�����λ���ظ�ֵ. \n",pSignalControlpara->stPhase[i].nPhaseID);
            return ERROR_REPEATE_CONCURRENT_PHASE;
        }

    }

    //�����    ������������ظ�
    for(i = 0 ; i < NUM_PHASE_TURN; i++)
    {
        for(j = 0 ; j < NUM_RING_COUNT; j++)
        {
            sTemp = pSignalControlpara->stPhaseTurn[i][j].nPhaseTurnID;
            sTemp2 = pSignalControlpara->stPhaseTurn[i][j].nCircleID;

            if(sTemp2 == 0)
            {
                continue;
            }

            if(sTemp != (i+1))
            {
                log_error("%s:%d   stPhaseTurn[%d] Circle[%d]   subscript is not equal to nPhaseTurnID  \n ",__func__,__LINE__,i+1,j+1);
                log_error_cn("ErrorCode: ERROR_SUBSCRIPT_PHASE_TURN , Error Content: �����%d�±겻���Ϲ淶. \n",i+1);
                return ERROR_SUBSCRIPT_PHASE_TURN;
            }

            if(sTemp2 != (j+1))
            {
                log_error("%s:%d   stPhaseTurn[%d] Circle[%d] subscript is not equal to nCircleID\n ",__func__,__LINE__,i+1,j+1);
                log_error_cn("ErrorCode: ERROR_SUSCRIPT_PHASE_TURN_CIRCLE , Error Content: �����%d��%d�±겻���Ϲ淶. \n",i+1,j+1);
                return ERROR_SUSCRIPT_PHASE_TURN_CIRCLE;
            }
            
            if(IsArrayRepeat(pSignalControlpara->stPhaseTurn[i][j].nTurnArray,16) == 1)
            {
                log_error("%s:%d   stPhaseTurn[%d] Circle[%d]  has repeat item \n ",__func__,__LINE__,i+1,j+1);
                log_error_cn("ErrorCode: ERROR_REPEATE_PHASE_TURN , Error Content: �����%d��%d���ظ�. \n",sTemp,sTemp2);
                return ERROR_REPEATE_PHASE_TURN;
            }
        }
    }

//���ǵ�У��Ҫ�ŵ�ǰ�����������ʴ˴����¿�ʼУ��    
#if 1
    //���űȱ�
    for(i = 0 ; i < NUM_GREEN_SIGNAL_RATION; i++)
    {
        for(j = 0 ; j < NUM_PHASE; j++)
        {
            sTemp = pSignalControlpara->stGreenSignalRation[i][j].nGreenSignalRationID;
            sTemp2 = pSignalControlpara->stGreenSignalRation[i][j].nPhaseID;
            
            if((sTemp2 != 0) && (sTemp != (i+1)))
            {
                log_error("%s:%d stGreenSignalRation[%d] : nPhaseId[%d]   nGreenSignalRationID  subscript is not equal to ID \n",__func__,__LINE__,i+1,j+1);
                log_error_cn("ErrorCode: ERROR_SUBSCRIPT_GREEN_SIGNAL_RATION , Error Content: ���űȱ�%d�±겻���Ϲ淶. \n",i+1);
                return ERROR_SUBSCRIPT_GREEN_SIGNAL_RATION;
            }
            
            if((sTemp2 != 0) && (sTemp2 != (j+1)))
            {
                log_error("%s:%d stGreenSignalRation[%d] : nPhaseId[%d]    nPhaseID  subscript is not equal to ID \n",__func__,__LINE__,i+1,j+1);
                log_error_cn("ErrorCode: ERROR_SUBSCRIPT_GREEN_SIGNAL_RATION_PHASE , Error Content: ���űȱ�����λ%d�±겻���Ϲ淶. \n",j+1);
                return ERROR_SUBSCRIPT_GREEN_SIGNAL_RATION_PHASE;
            }        

         }
    }
#endif
    //Channel 
    for(i = 0 ; i < NUM_CHANNEL; i++)
    {
        sTemp = pSignalControlpara->stChannel[i].nChannelID;

        if((pSignalControlpara->stChannel[i].nControllerID != 0) && (sTemp != (i+1)))
        {
            log_error("%s:%d stChannel[%d]  subscript is not equal to ID \n",__func__,__LINE__,i+1);
            log_error_cn("ErrorCode: ERROR_SUBSCRIPT_CHANNEL , Error Content: ͨ����%d�±겻���Ϲ淶. \n",i+1);
            return ERROR_SUBSCRIPT_CHANNEL;
        }
    }

    //stScheme
    for(i = 0 ; i < NUM_SCHEME; i++)
    {
        sTemp = pSignalControlpara->stScheme[i].nSchemeID;

        if((pSignalControlpara->stScheme[i].nGreenSignalRatioID != 0) && (sTemp != (i+1)))
        {
            log_error("%s:%d stScheme[%d]  subscript is not equal to ID \n",__func__,__LINE__,i+1);
            log_error_cn("ErrorCode: ERROR_SUBSCRIPT_SCHEME , Error Content: ������%d�±겻���Ϲ淶. \n",i+1);
            return ERROR_SUBSCRIPT_SCHEME;
        }
    }

    //stAction
    for(i = 0 ; i < NUM_ACTION; i++)
    {
        sTemp = pSignalControlpara->stAction[i].nActionID;

        if((pSignalControlpara->stAction[i].nSchemeID != 0) && (sTemp != (i+1)) && (pSignalControlpara->stAction[i].nSchemeID < 249))
        {
            log_error("%s:%d stScheme[%d]  subscript is not equal to ID \n",__func__,__LINE__,i+1);
            log_error_cn("ErrorCode: ERROR_SUBSCRIPT_ACTION , Error Content: ������%d�±겻���Ϲ淶. \n",i+1);
            return ERROR_SUBSCRIPT_ACTION;
        }
    }

    //stTimeInterval
    for(i = 0 ; i < NUM_TIME_INTERVAL; i++)
    {
        for(j = 0 ; j < NUM_TIME_INTERVAL_ID; j++)
        {
            sTemp = pSignalControlpara->stTimeInterval[i][j].nTimeIntervalID ;
            sTemp2 = pSignalControlpara->stTimeInterval[i][j].nTimeID;
            
            if((pSignalControlpara->stTimeInterval[i][j].nActionID != 0) && (sTemp != (i+1)))
            {
                log_error("%s:%d stTimeInterval[%d]:nTimeID[%d] nTimeIntervalID subscript is not equal to ID \n",__func__,__LINE__,i+1,j+1);
                log_error_cn("ErrorCode: ERROR_SUBSCRIPT_TIMEINTERVAL , Error Content: ʱ�α�%d�±겻���Ϲ淶. \n",i+1);
                return ERROR_SUBSCRIPT_TIMEINTERVAL;
            }
            if((pSignalControlpara->stTimeInterval[i][j].nActionID != 0) && (sTemp2 != (j+1)))
            {
                log_error("%s:%d stTimeInterval[%d]:nTimeID[%d] nTimeID subscript is not equal to ID \n",__func__,__LINE__,i+1,j+1);
                log_error_cn("ErrorCode: ERROR_SUBSCRIPT_TIMEINTERVAL_TIME , Error Content: ʱ�α�%dʱ�κ�%d�±겻���Ϲ淶. \n",i+1,j+1);
                return ERROR_SUBSCRIPT_TIMEINTERVAL_TIME;
            }
        }
    }

    //stPlanSchedule
    for(i = 0 ; i < NUM_SCHEDULE; i++)
    {
        sTemp = pSignalControlpara->stPlanSchedule[i].nScheduleID;

        if((pSignalControlpara->stPlanSchedule[i].nTimeIntervalID != 0) && (sTemp != (i+1)))
        {
            log_error("%s:%d stPlanSchedule[%d]  subscript is not equal to ID \n",__func__,__LINE__,i+1);
            log_error_cn("ErrorCode: ERROR_SUBSCRIPT_SCHEDULE , Error Content: ���ȱ�%d�±겻���Ϲ淶. \n",i+1);
            return ERROR_SUBSCRIPT_SCHEDULE;
        }
    }
    
    //FollowPhase
    for(i = 0 ; i < NUM_FOLLOW_PHASE ; i++)
    {
        nFollowPhaseArray[i] = pSignalControlpara->stFollowPhase[i].nFollowPhaseID;

        if((pSignalControlpara->stFollowPhase[i].nArrayMotherPhase[0] != 0 ) && (nFollowPhaseArray[i] != (i+1)))
        {
            log_error("%s:%d stFollowPhase[%d]  subscript is not equal to ID \n ",__func__,__LINE__,i+1);
            log_error_cn("ErrorCode: ERROR_SUBSCRIPT_FOLLOW_PHASE , Error Content: ������λ��%d�±겻���Ϲ淶. \n",i+1);
            return ERROR_SUBSCRIPT_FOLLOW_PHASE;
        }

        if(IsArrayRepeat(pSignalControlpara->stFollowPhase[i].nArrayMotherPhase,NUM_PHASE) == 1)
        {
            log_error("%s:%d stFollowPhase[%d]  has repeat items \n ",__func__,__LINE__,i+1);
            log_error_cn("ErrorCode: ERROR_REPEATE_FOLLOW_PHASE , Error Content: ������λ��%d���ظ���λ. \n",i+1);
            return ERROR_REPEATE_FOLLOW_PHASE;
        }

    }


    return 0;
}

/*****************************************************************************
 �� �� ��  : find_next_index
 ��������  : �ӵ�ǰ�����������ҳ���һ��������ʼ��λ�����������е�����
 �������  : SignalControllerPara *pSignalControlpara  �����ò����Ľṹ��ָ��
             unsigned char *phaseArray                 ������������
             unsigned short index                      ��ʼ������ֵ
 �� �� ֵ  : �ҵ��򷵻�����ֵ���Ҳ����򷵻�0
 �޸���ʷ  
  1.��    ��   : 2014��12��5��
    ��    ��   : Jicky
    �޸�����   : �����ɺ���

*****************************************************************************/
static int find_next_index(SignalControllerPara *pSignalControlpara,unsigned char *phaseArray, unsigned short index)
{
	int i,  ret = 0;
	unsigned short phaseId = phaseArray[index];
	PPhaseItem item = &pSignalControlpara->stPhase[phaseId - 1];

    if(phaseId == 0)//ʹ�ÿؼ����صĻ����������λΪ0�����
    {
        return 0;
    }
	
	if (item->nPhaseID != phaseId) 
	{
		return index + 1;
	}
	phaseId = item->byPhaseConcurrency[0];
	item = &pSignalControlpara->stPhase[phaseId - 1];
	
	for (i = index + 1; i < NUM_PHASE; i++) 
	{
		if (phaseArray[i] == 0) 
		{
			ret = 0;
			break;
		}
		if (IsItemInCharArray(item->byPhaseConcurrency,NUM_PHASE, phaseArray[i]) == FALSE) 
		{
			ret = i;
			break;
		}
	}
	return ret;
}

/*****************************************************************************
 �� �� ��  : checkPhaseTrunInRing
 ��������  : ����������ÿ�����е������Ƿ񲢷���ȷ
 �������  : SignalControllerPara *pSignalControlpara  �����ò����Ľṹ��ָ��
             unsigned short phaseTurn                  �������
 �� �� ֵ  : �����������ÿ�����е�����ȫ��������ȷ�򷵻�True����֮����False
 �޸���ʷ  
  1.��    ��   : 2014��12��5��
    ��    ��   : Jicky
    �޸�����   : �����ɺ���

*****************************************************************************/
static int checkPhaseTrunInRing(SignalControllerPara *pSignalControlpara,unsigned short phaseTurn)
{
	PPhaseTurnItem item = NULL;
	int i = 0;
	unsigned short fatherPhase = 0;
	int indexs[4] = {0};	//���ÿ�����ж�Ӧ���ϵ���ʼ��λ������
	unsigned short ringPhase[4] = {0};	//���ÿ�����ж�Ӧ���ϵ���ʼ��λ�������ж���Щ���Ƿ��ǲ�����ϵ
	
	while (1) 
	{	//��ȡÿ�����ж�Ӧ���ϵ���ʼ��λ���������ringPhase�У�Ȼ����Ѱ��һ������ÿ�����е���ʼ��λ������
		for (i = 0; i < 4; i++) 
		{
			item = &pSignalControlpara->stPhaseTurn[phaseTurn - 1][i];
			if (item->nPhaseTurnID != phaseTurn || item->nCircleID != (i + 1))
				continue;
			ringPhase[i] = item->nTurnArray[indexs[i]];
			indexs[i] = find_next_index(pSignalControlpara,item->nTurnArray, indexs[i]);
		}
		//�ж�ringPhase��������ȡ����λ�Ƿ񲢷�
		//printf("DEBUG: ringPhase = {%d, %d, %d, %d}\n", ringPhase[0], ringPhase[1], ringPhase[2], ringPhase[3]);
		//printf("DEBUG: indexs = {%d, %d, %d, %d}\n", indexs[0], indexs[1], indexs[2], indexs[3]);
		for (i = 0; i < 4; i++) 
		{
			if (ringPhase[i] == 0)
				continue;
			if (fatherPhase == 0) 
			{
				fatherPhase = ringPhase[i];
			} 
			else 
			{
				if (IsItemInCharArray(pSignalControlpara->stPhase[fatherPhase - 1].byPhaseConcurrency, NUM_PHASE, ringPhase[i]) == FALSE)
				{
#if 0
					printf("DEBUG: fatherPhase = %d, correspond = {%d, %d}, ringPhase[%d] = %d\n",
							fatherPhase,
							pSignalControlpara->stPhase[fatherPhase - 1].byPhaseConcurrency[0],
							pSignalControlpara->stPhase[fatherPhase - 1].byPhaseConcurrency[1],
							i, ringPhase[i]);
#endif
					return FALSE;
				}
			}
			
		}
		fatherPhase = 0;
		//��ÿ�����ж�Ӧ���ϵ���ʼ��λ�������ĸ���Ϊ0ʱ��˵���жϽ���
		if (indexs[0] == 0 && indexs[1] == 0 && indexs[2] == 0 && indexs[3] == 0)
		    break;
	}
	return TRUE;
}
//�жϲ�����λ�ĺϷ���
static int IsConcurrencyPhaseLegal(SignalControllerPara *pSignalControlpara,int nPhaseId)
{
	int i = 0;
	int nTemp = 0;	
	for(i = 0 ; i < NUM_PHASE; i++)
    {
		nTemp = pSignalControlpara->stPhase[nPhaseId - 1].byPhaseConcurrency[i];
		
		if(nTemp < 0 || nTemp > NUM_PHASE)
		{
			log_error_cn("��λ%d�Ĳ�����λ�����в��Ϸ���λID\n",nPhaseId);
			return 1;
		}
		
		if(nTemp == 0)
		{
			continue;
		}

		//������λ������
		if(IS_PHASE_INABLE(pSignalControlpara->stPhase[nTemp - 1].wPhaseOptions) == 0)
		{
			log_error_cn("ErrorCode: ERROR_NOT_EXIST_CONCURRENT_PHASE_PHASE , Error Content: ��λ%d�Ĳ�����λ%d������. \n",nPhaseId,nTemp);
			return ERROR_NOT_EXIST_CONCURRENT_PHASE_PHASE;
		}

		//phase is in the same circle
		if(pSignalControlpara->stPhase[nTemp - 1].nCircleID == pSignalControlpara->stPhase[nPhaseId - 1].nCircleID)
		{
			log_error_cn("ErrorCode: ERROR_SAME_CIRCLE_CONCURRENT_PHASE , Error Content: ��λ%d���䲢����λ%d������ͬ. \n",nTemp,nPhaseId);
			return ERROR_SAME_CIRCLE_CONCURRENT_PHASE;
		}

		//����λ�Ĳ�����λ��û�и���λ
		if(IsItemInCharArray(pSignalControlpara->stPhase[nTemp - 1].byPhaseConcurrency,NUM_PHASE, nPhaseId) == 0)
		{
			log_error_cn("ErrorCode: ERROR_FATHER_NOT_EXIST_CONCURRENT_PHASE , Error Content: ��λ%d�����䲢����λ%d�Ĳ�����λ��. \n",nPhaseId,nTemp);
			return ERROR_FATHER_NOT_EXIST_CONCURRENT_PHASE;
		}

		//����λ����������Ƿ�����      
		if(IsPhaseContinuousInPhaseTurn(pSignalControlpara, pSignalControlpara->stPhase[nTemp - 1].nCircleID, pSignalControlpara->stPhase[nPhaseId - 1].byPhaseConcurrency, NUM_PHASE) == 0)
		{
			log_error_cn("ErrorCode: ERROR_CHILD_NOT_CONTINUOUS_CONCURRENT_PHASE , Error Content: ��λ%d�Ĳ�����λ�ڻ�%d�в�����. \n",nPhaseId,pSignalControlpara->stPhase[nTemp - 1].nCircleID);
			return ERROR_CHILD_NOT_CONTINUOUS_CONCURRENT_PHASE;
		}

		//������У�����һ������ű�֮�ͱ������   �ŵ�����ʱ���Ӳ���
		if(IsBarrierGreenSignalRationEqual(pSignalControlpara,pSignalControlpara->stPhase[nPhaseId - 1].byPhaseConcurrency, NUM_PHASE,pSignalControlpara->stPhase[nPhaseId - 1].nCircleID) == 1)
		{
			log_error_cn("ErrorCode: ERROR_BARRIER_CONCURRENT_PHASE , Error Content: ��λ%d����һ�����λ���ű�֮�Ͳ����. \n",nPhaseId);
			return ERROR_BARRIER_CONCURRENT_PHASE;
		 }
    }

	return 0;	
}

//�жϸ�����λ���Ƿ�Ϸ�
static int IsFollowPhaseLegal(SignalControllerPara *pSignalControlpara)
{
    int i = 0;
    int sTemp = 0;
    int j = 0;
    
    /*13. ������λ�е�ĸ��λ�Ƿ����;*/
    for(i = 0 ; i < NUM_FOLLOW_PHASE; i++)
    {
        sTemp = pSignalControlpara->stFollowPhase[i].nFollowPhaseID;
        if(sTemp == 0)
        {
            continue;
        }
    
        for(j = 0 ; j < NUM_PHASE ; j++)
        {
            sTemp = pSignalControlpara->stFollowPhase[i].nArrayMotherPhase[j];

            if(sTemp == 0)
            {
                break;
            }
            if(IS_PHASE_INABLE(pSignalControlpara->stPhase[sTemp - 1].wPhaseOptions)== 0)
            {
                log_error_cn("ErrorCode: ERROR_NOT_EXIST_MOTHER_PHASE_FOLLOW_PHASE , Error Content: ������λ%d��ĸ��λ%d������ \n",i+1,sTemp);
                return ERROR_NOT_EXIST_MOTHER_PHASE_FOLLOW_PHASE;
            }

        }
    }

    return 0;
}

//�ж���λ�Ƿ�Ϸ�
static int IsPhaseLegal(SignalControllerPara *pSignalControlpara,int nPhaseId)
{
	if(nPhaseId <= 0 || nPhaseId > NUM_PHASE)
	{
		log_error_cn("��λ%d���Ϸ�����Χ����Ϊ[1,%d]\n",nPhaseId,NUM_PHASE);
		return 2;
	}

	if(IS_PHASE_INABLE(pSignalControlpara->stPhase[nPhaseId - 1].wPhaseOptions) == 0)
	{
		log_error_cn("��λ%d���Ϸ�����λδʹ��.\n",nPhaseId);
		return 3;
	}

    return 0;
}

//�ж�ͨ�����Ƿ�Ϸ�
static int IsChannelLegal(SignalControllerPara *pSignalControlpara,unsigned short *nPhaseArray,unsigned short *nFollowPhaseArray)
{
    int i = 0;
    int sTemp = 0;
    
    for(i = 0 ; i < NUM_CHANNEL ; i++)
    {
        sTemp = pSignalControlpara->stChannel[i].nControllerID ;
        if(sTemp == 0)
        {
            continue;
        }

        if((pSignalControlpara->stChannel[i].nControllerType == MOTOR ) || (pSignalControlpara->stChannel[i].nControllerType == PEDESTRIAN))
        {
            if(IsItemInShortArray(nPhaseArray,NUM_PHASE,sTemp) == 0)
            {
                log_error_cn("ErrorCode: ERROR_NOT_EXIST_SOURCE_CHANNEL , Error Content: ͨ��%d�Ŀ���Դ%d������. \n",i+1,sTemp);
                return ERROR_NOT_EXIST_SOURCE_CHANNEL;
            }
        }
        else if ((pSignalControlpara->stChannel[i].nControllerType == FOLLOW) || (pSignalControlpara->stChannel[i].nControllerType == OTHER))//OTHER ��ʾ���˸���
        {
            if(IsItemInShortArray(nFollowPhaseArray,NUM_FOLLOW_PHASE,sTemp) == 0)
            {
                log_error_cn("ErrorCode: ERROR_NOT_EXIST_SOURCE_FOLLOW_CHANNEL , Error Content: ͨ��%d�ĸ������Դ%d������. \n",i+1,sTemp);
                return ERROR_NOT_EXIST_SOURCE_FOLLOW_CHANNEL;
            }
        }
    }

    return 0;
}


//�ж����űȱ��Ƿ�Ϸ�
static int IsGreenSignalRationLegal(SignalControllerPara *pSignalControlpara,int nGreenSignalRationId)
{
	int i = 0;
	int nPhaseId = 0;
	int nGreenLightTime = 0;
	int sTemp = 0;
    int ret = 0;
    int count = 0;
    
	if(nGreenSignalRationId <= 0 || nGreenSignalRationId > NUM_GREEN_SIGNAL_RATION )
	{
		log_error_cn("���űȱ�%d ID���Ϸ�����Χ����Ϊ[1,%d]\n",nGreenSignalRationId,NUM_GREEN_SIGNAL_RATION);
		return 1;	
	}
	
	for(i = 0; i < NUM_PHASE; i++)
	{
        nPhaseId = pSignalControlpara->stGreenSignalRation[nGreenSignalRationId - 1][i].nPhaseID;
        nGreenLightTime = pSignalControlpara->stGreenSignalRation[nGreenSignalRationId - 1][i].nGreenSignalRationTime;
        
        if((nPhaseId == 0) || (nGreenLightTime == 0))//ÿ�����űȱ������űȺż���λ�Ŷ�����0������û���жϵ����ĸ����ű���û��ʹ�õģ���ֱ�Ӳ�У�����ű�ʱ��Ϊ0�������
        {
            count++;
            continue;
        }
        
        if((ret = IsPhaseLegal(pSignalControlpara,nPhaseId)) != 0)
        {
            return ret;
        }
		
		sTemp = pSignalControlpara->AscSignalTransTable[nPhaseId - 1].nGreenLightTime + pSignalControlpara->stPhase[nPhaseId - 1].nYellowTime +  pSignalControlpara->stPhase[nPhaseId - 1].nAllRedTime;
	
		if(nGreenLightTime < sTemp)
		{
			log_error_cn("���űȱ�%d����λ%d�����ű�ʱ�䲻�Ϸ������ű�ʱ�����Ȼ���ȫ��Ƶ�ʱ��֮��Ҫ��.\n",
													nGreenSignalRationId,
													nPhaseId);
			return 3;	
		}
	}

    if(count == NUM_PHASE)
    {
        log_error_cn("���űȱ�%d������.\n",nGreenSignalRationId);
        return 2;
    }
	
    return 0;
}


//�ж�������Ƿ�Ϸ�
static int IsPhaseTurnLegal(SignalControllerPara *pSignalControlpara,int nPhaseTurnId)
{
	int i = 0;
	int ring = 0;
	int nPhaseId = 0;
	int nCircleId = 0;
	int ret = 0;

	if(nPhaseTurnId <= 0 || nPhaseTurnId > NUM_PHASE_TURN)
	{
		log_error_cn("�����%d������Ų��Ϸ�����Χ����Ϊ[0,%d]\n",nPhaseTurnId,NUM_PHASE_TURN);
		return 1;	
	}
	for(ring = 0; ring < 4; ring++)
	{
		nCircleId = pSignalControlpara->stPhaseTurn[nPhaseTurnId - 1][ring].nCircleID;
		if(nCircleId < 0 || nCircleId > 4)
		{
			log_error_cn("�����%d�Ļ�%d�Ļ��Ų��Ϸ�����Χ����Ϊ[0,4]\n",nPhaseTurnId,nCircleId);
			return 2;	
		}

		if(nCircleId == 0)
			continue;
		
		for(i = 0; i < NUM_PHASE; i++)
		{
			nPhaseId = pSignalControlpara->stPhaseTurn[nPhaseTurnId - 1][ring].nTurnArray[i];
			if(nPhaseId == 0)
				continue;

             if((ret = IsPhaseLegal(pSignalControlpara,nPhaseId)) != 0)
             {
                return ret;
             }

			if(pSignalControlpara->stPhase[nPhaseId - 1].nCircleID != nCircleId)
			{
				log_error_cn("�����%d����%d����λ%d�Ļ�������λ���в�һ��\n",nPhaseTurnId,nPhaseId);	
				return 4;
			}

			if((ret = IsConcurrencyPhaseLegal(pSignalControlpara,nPhaseId)) != 0)
			{
                return ret;
			}
		} 
	}

    if(checkPhaseTrunInRing(pSignalControlpara,nPhaseTurnId) == FALSE)
    {
        log_error_cn("ErrorCode: ERROR_NOT_CORRECT_PHASE_TURN , Error Content: �����%d������λ����. \n",nPhaseTurnId);
        return ERROR_NOT_CORRECT_PHASE_TURN;
    }

    return 0;
}


//�жϷ������Ƿ�Ϸ�
static int IsSchemeLegal(SignalControllerPara *pSignalControlpara,int nSchemeId) 
{
	int nGreenSignalRatioId = 0;
	int nPhaseTurnId = 0;
	int ret = 0;

    if((nSchemeId >= 251) && (nSchemeId <= 255))//���ⷽ����
    {
        return 0;
    }
	
	if(nSchemeId <= 0 || nSchemeId > NUM_SCHEME)
	{
		log_error_cn("������%d ID���Ϸ�����Χ����Ϊ[0,%d]\n",nSchemeId,NUM_SCHEME);
		return 1;	
	}

	nGreenSignalRatioId = pSignalControlpara->stScheme[nSchemeId - 1].nGreenSignalRatioID;
	nPhaseTurnId = pSignalControlpara->stScheme[nSchemeId - 1].nPhaseTurnID;

    if((nGreenSignalRatioId == 0) || (nPhaseTurnId == 0))
    {
        return 0;
    }

	//�ж����űȱ��Ƿ�Ϸ�
    if((ret = IsGreenSignalRationLegal(pSignalControlpara,nGreenSignalRatioId)) != 0)
	{
        return ret;
	}
	
	cArrayGreenSignalRation[nGreenSignalRatioId - 1] = nGreenSignalRatioId; 
	
	//�ж�������Ƿ�Ϸ�
    if((ret = IsPhaseTurnLegal(pSignalControlpara,nPhaseTurnId)) != 0)
	{
        return ret;
	}
	
    //�������е����ڳ��Ƿ����������е������ű�ʱ��֮��
    if(GetCircleTime(pSignalControlpara,nGreenSignalRatioId,nPhaseTurnId) != pSignalControlpara->stScheme[nSchemeId - 1].nCycleTime)
    {
        log_error_cn("ErrorCode: ERROR_CIRCLE_TIME_SCHEME , Error Content: ������%d�����ڳ����Ϸ�.\n",nSchemeId);
		//log_error_cn("nPhaseTurnId %d  , nGreenSignalRatioId %d, %d  --- %d \n",nPhaseTurnId,nGreenSignalRatioId
		//					,GetCircleTime(pSignalControlpara,nPhaseTurnId,nGreenSignalRatioId),
		//				pSignalControlpara->stScheme[nSchemeId - 1].nCycleTime);
        return ERROR_CIRCLE_TIME_SCHEME;
    }

    return 0;
}

//�ж϶������Ƿ�Ϸ�
static int IsActionLegal(SignalControllerPara *pSignalControlpara,int nActionId)
{
	int  nSchemeId = 0;

	if(nActionId <=0 || nActionId > NUM_ACTION)
	{
		log_error_cn("������%d ID���Ϸ�����Χ����Ϊ[0,%d]\n",nActionId,NUM_ACTION);
		return 1;	
	}

	nSchemeId =  pSignalControlpara->stAction[nActionId - 1].nSchemeID;

	//��������������жϷ������Ƿ�Ϸ�
    return (nSchemeId != 0) && IsSchemeLegal(pSignalControlpara,nSchemeId);
}


//�ж�ʱ�α��Ƿ�Ϸ�
static int IsTimeIntervalLegal(SignalControllerPara *pSignalControlpara,int nIntervalId)
{
	//Ĭ�ϲ�У��ָ��Ϸ����ˡ�
	int i = 0;
	int nTimeId = 0;
	int nActionId = 0;
    int ret = 0;
    int count = 0;

	if(nIntervalId <=0 || nIntervalId > NUM_TIME_INTERVAL)
	{
		log_error_cn("ʱ�α�%d��ID���Ϸ�����Χ����Ϊ[0,%d]\n",nIntervalId,NUM_TIME_INTERVAL);
		return 1;
	}

	for(i = 0; i < NUM_TIME_INTERVAL_ID; i++)
	{
		nTimeId = pSignalControlpara->stTimeInterval[nIntervalId - 1][i].nTimeID;

        if(nTimeId == 0)
		{
		    count++;
            continue;
		}
		
		if(nTimeId < 0 || nTimeId > NUM_TIME_INTERVAL_ID)//���ʱ�α��е�ʱ��ID���ںϷ����䣬������ΪERROR��
		{
			log_error_cn("ʱ�α�%d��ʱ��%d���Ϸ�����Χ����Ϊ[0,%d]\n",nIntervalId,nTimeId,NUM_TIME_INTERVAL_ID);
			return 1;	
		}

		//ʱ�α���������ж����Ӧ�Ķ������Ƿ�Ϸ���
		nActionId = pSignalControlpara->stTimeInterval[nIntervalId - 1][i].nActionID;
		if((nActionId != 0) && ((ret = IsActionLegal(pSignalControlpara,nActionId)) != 0))
		{
            return ret;
		}
	}

    if(count == NUM_TIME_INTERVAL_ID)
    {
        log_error_cn("ʱ�α�%d������.\n",nIntervalId);
        return 2;
    }

    return 0;
}

//�жϵ��ȱ��Ƿ�Ϸ�
static int IsPlanScheduleLegal(SignalControllerPara *pSignalControlpara,int nScheduleId)
{
	int nTimeIntervalId = 0;	
	
	if(nScheduleId <= 0 || nScheduleId > NUM_SCHEDULE)
	{
		log_error_cn("���ȱ�%d��ID���Ϸ�����Χ������[0,%d]\n",nScheduleId,NUM_SCHEDULE);	
		return 1;
	}

	nTimeIntervalId = pSignalControlpara->stPlanSchedule[nScheduleId - 1].nTimeIntervalID;

	//�ж�ʱ�α��Ƿ�Ϸ�
	return (nTimeIntervalId != 0) && (IsTimeIntervalLegal(pSignalControlpara,nTimeIntervalId));	
}


/*****************************************************************************
 �� �� ��  : IsSignalControlparaLegal
 ��������  : ȫ���źſ��Ʋ����Ϸ��Լ��
 �������  : SignalControllerPara *pSignalControlpara  

 �޸���ʷ      :
  1.��    ��   : 2014��8��6��
    ��    ��   : xiaowh
    �޸�����   : �����ɺ���

*****************************************************************************/
int IsSignalControlparaLegal(SignalControllerPara *pSignalControlpara)//�Ϸ��Ļ�������0�����򷵻ش�����
{
    int i = 0;
    int ret = 0;    

    unsigned short nPhaseArray[NUM_PHASE] = {0};//�ܹ�����λ����
    unsigned short nFollowPhaseArray[NUM_FOLLOW_PHASE] = {0};//������λ����

	SignalControllerPara zero;

    if(!pSignalControlpara)
    {
        log_error_cn("ErrorCode: ERROR_NULL_POINTER , Error Content: ȫ��ָ��Ϊ�� \n");
        return ERROR_NULL_POINTER;
    }

	memset(&zero, 0, sizeof(SignalControllerPara));
	if (memcmp(pSignalControlpara, &zero, sizeof(SignalControllerPara)) == 0) {	//˵��û��������Ϣ
		log_error_cn("ErrorCode: ERROR_NOT_CONFIG_INFORMATION , Error Content: ������ϢΪ�� \n");
		return ERROR_NOT_CONFIG_INFORMATION;
	}

    /*1.  �������������±��Ƿ��ID��ͬ;*/
    if((ret = IsSignalControllerParaIdLegal(pSignalControlpara,nPhaseArray,nFollowPhaseArray)) != 0)
    {
        return ret;
    }

	//�����Ǹ��ݡ�ʹ����У�飬������У�顱��ԭ�����У��
    //���ݵ��ȱ���һ��һ��У��
    for(i = 0 ; i < NUM_SCHEDULE; i++)
    {
        if((ret = IsPlanScheduleLegal(pSignalControlpara,i+1)) != 0)
        {
            return ret;
        }
    }    

    //�жϸ�����λ��
    if((ret = IsFollowPhaseLegal(pSignalControlpara)) != 0)
    {
        return ret;
    }

    //�ж�ͨ����
    if((ret = IsChannelLegal(pSignalControlpara,nPhaseArray,nFollowPhaseArray)) != 0)
    {
        return ret;
    }
    
    return 0;
}


/*****************************************************************************
 �� �� ��  : LoadDataFromCfg
 ��������  : �������ļ��м����������ݵ��ڴ���
 �������  : ��
 �������  : ��
 �� �� ֵ  : unsigned
 
 �޸���ʷ      :
  1.��    ��   : 2014��7��31��
    ��    ��   : xiaowh
    �޸�����   : �����ɺ���

*****************************************************************************/
Boolean LoadDataFromCfg(SignalControllerPara *pSignalControlpara, const char *path)
{
    char cSection[256];
    int i = 0;
    SignalControllerPara zero;
	
	if ((NULL == pSignalControlpara) || parse_start((path == NULL) ? CFG_NAME : path) == False)
    {
        return FALSE;
    }

	memset(pSignalControlpara, 0, sizeof(SignalControllerPara));

    //���ص�Ԫ���� 
    memset(cSection,0,sizeof(cSection));
    ReadUnitPara(&(pSignalControlpara->stUnitPara));

    //������λ���� 
    ReadPhaseItem(pSignalControlpara->stPhase,pSignalControlpara->AscSignalTransTable,NUM_PHASE);

    //����ͨ������ 
    ReadChannelItem(pSignalControlpara->stChannel, NUM_CHANNEL);

    //�������űȲ��� 
    for(i = 0 ; i < NUM_GREEN_SIGNAL_RATION ; i++)
    {
        ReadGreenSignalRationItem(pSignalControlpara->stGreenSignalRation[i],i,NUM_PHASE);

    }

    //To Do  �����
    for(i = 0 ; i < NUM_PHASE_TURN ; i++)
    {
        ReadPhaseTurnItem(pSignalControlpara->stPhaseTurn[i],i,4);
    }

    //���ط��������� 
    ReadSchemeItem(pSignalControlpara->stScheme,NUM_SCHEME);

    //���ض���������
    ReadActionItem(pSignalControlpara->stAction,NUM_ACTION);

    //����ʱ�α�����
    for(i = 0; i < NUM_TIME_INTERVAL; i++)
    {
        ReadTimeIntervalItem(pSignalControlpara->stTimeInterval[i], i,NUM_TIME_INTERVAL_ID);
    }
    
    //���ص��Ȳ���
    ReadPlanSchedule(pSignalControlpara->stPlanSchedule,NUM_SCHEDULE);

    //���ظ������
    ReadFollowPhaseItem(pSignalControlpara->stFollowPhase,NUM_FOLLOW_PHASE);

    //
    ReadVehicleDetector(pSignalControlpara->AscVehicleDetectorTable,MAX_VEHICLEDETECTOR_COUNT);

    //
    ReadPedestrianDetector(pSignalControlpara->AscPedestrianDetectorTable,MAX_PEDESTRIANDETECTOR_COUNT);

    //��������
    parse_end();

	memset(&zero, 0, sizeof(SignalControllerPara));
	if (memcmp(pSignalControlpara, &zero, sizeof(SignalControllerPara)) == 0) {	//˵��û��������Ϣ
		log_error("config file is not exist or there is no  config information in file . \n");
		return FALSE;
	}	
	
	return TRUE;
}






