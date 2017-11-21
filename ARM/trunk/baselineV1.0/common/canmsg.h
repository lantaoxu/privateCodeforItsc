#ifndef _CAN_INTERFACE_H
#define _CAN_INTERFACE_H

#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include "libsocketcan.h"
//��Ƶ��������
#include<sys/un.h>
#include<unistd.h>
#include<stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "hik.h"     

//#define AF_CAN		29	/* Controller Area Network      *
#ifdef __cplusplus
extern "C" {
#endif

//�ƿذ�
void i_can_its_init();
void i_can_its_send_led_request(int boardNum, unsigned short *poutLamp);
void i_can_its_get_Volt(int boardNum, unsigned short *pboardInfo);
unsigned short i_can_its_get_cur(int boardNum, int pahseNum, int redGreen);
unsigned short getPedONStateFromCanLib();
//�����
unsigned short recv_date_from_vechile(int boardNum);
//�ڲ��ӿ�
void canits_set_bitrate(const char *name, unsigned long bitrate);
void canits_start(const char *name);
void canits_create_recv_thread();
void canits_create_socket_can_send();
void canits_init();
void canits_send(struct can_frame *pcanfram);
//����CAN��Ϣ�߳�
void *canits_recv_thread(void *p); 
//��Ƶ�������Խ�
void video_detece_main_thread_create();
int  video_detect_interface(void);
void recv_from_camera(int *client_fd);

#pragma pack(push, 1)

#define NAME_LEN				32				/* �û������� */
#define MAX_TPS_RULE            8				/*������������Ŀ*/
#define SERIALNO_LEN            48				/* ���кų��� */
#define LOW_4bits               0               /*����λ*/
#define TALL_4bits              4               /*����λ*/
#define BUFFLEN                 2048            /*����buff����*/
#define IsNeedSend              1               /*��Ҫ��������*/      
#define NoNeedSend              0               /*����Ҫ����*/
typedef struct
{		/* 24 bytes */
	struct in_addr	v4;							//< IPv4��ַ
	struct in6_addr	v6;							//< IPv6��ַ
	unsigned char	res[4];
}U_IN_ADDR;

typedef struct 
{
	DWORD dwLength;   						/*�����ܳ���*/
	BYTE  byRes1[2]; 						/* ���� */
	BYTE  byCommand;						/* �������� 0x3a*/
	BYTE  byIPType;		 				    /*IP���� 0-IPV4 1-IPV6 */
	DWORD dwVersion; 						/* �豸�汾��Ϣ */
	BYTE  sDVRName[NAME_LEN];     			/* �豸���� */
	BYTE  sSerialNumber[SERIALNO_LEN]; 		/* �豸���к� */
	U_IN_ADDR  struIPAddr;              	/*�豸IP��ַ*/ 
	WORD wPort; 							/* �豸�˿ں� */
	BYTE byMacAddr[6];	 					/* �豸MAC��ַ*/
	BYTE byRes3[8]; 						/* ���� */
}INTER_DVR_REQUEST_HEAD_V30;

typedef struct _TPS_NET_VCA_POINT_
{	
 	UINT16 wX;								/*(0.000-1)*1000 X������ */
 	UINT16 wY;								/*(0.000-1)*1000 Y������ */
} TPS_NET_VCA_POINT;

typedef struct _NET_LANE_QUEUE_
{
    TPS_NET_VCA_POINT   struHead;	        /*����ͷ*/
    TPS_NET_VCA_POINT   struTail;		    /*����β*/
    UINT32              dwlength;		    /*ʵ�ʶ��г��� ��λΪ�� ������*1000*/
}NET_LANE_QUEUE; 

typedef enum tagTRAFFIC_DATA_VARY_TYPE
{    
     ENUM_TRAFFIC_VARY_NO             = 0x00,   //�ޱ仯
     ENUM_TRAFFIC_VARY_VEHICLE_ENTER  = 0x01,   //��������������Ȧ
     ENUM_TRAFFIC_VARY_VEHICLE_LEAVE  = 0x02,   //�����뿪������Ȧ
     ENUM_TRAFFIC_VARY_QUEUE          = 0x04,   //���б仯
     ENUM_TRAFFIC_VARY_STATISTIC      = 0x08,   //ͳ�����ݱ仯��ÿ���ӱ仯һ�ΰ���ƽ���ٶȣ������ռ�/ʱ��ռ���ʣ���ͨ״̬��        
} TRAFFIC_DATA_VARY_TYPE;  

typedef struct _NET_LANE_PARAM
{
    UINT8  byRuleName[NAME_LEN];  		    /*������������ */
    UINT8  byRuleID;                 		/*������ţ�Ϊ�������ýṹ�±꣬0-7 */
	UINT8  byLaneType;			      		/*�������л�����*/
	UINT8  byTrafficState;					/*������ͨ״̬*/
	UINT8  byRes1;				        	/* ����*/
	UINT32 dwVaryType;					    /* ������ͨ�����仯���� ����   TRAFFIC_DATA_VARY_TYPE */
	UINT32 dwTpsType;					    /* ���ݱ仯���ͱ�־����ʾ��ǰ�ϴ���ͳ�Ʋ����У���Щ������Ч����ֵΪITS_TPS_TYPE���������*/
    UINT32 dwLaneVolume;	                /* �������� ��ͳ���ж��ٳ���ͨ��*/
    UINT32 dwLaneVelocity;         			/*�����ٶȣ��������	������*1000*/
   	UINT32 dwTimeHeadway ;       			/*��ͷʱ�࣬�������	������*1000*/
    UINT32 dwSpaceHeadway;       		    /*��ͷ��࣬���������㸡����*1000*/
    UINT32 dwSpaceOccupyRation;   		    /*����ռ���ʣ��ٷֱȼ��㣨�ռ���) ������*1000*/
	UINT32 dwTimeOccupyRation;          	/*ʱ��ռ����*/
	UINT32 dwLightVehicle;       			/* С�ͳ�����*/
	UINT32 dwMidVehicle;        			/* ���ͳ�����*/
	UINT32 dwHeavyVehicle;      			/* ���ͳ�����*/
    NET_LANE_QUEUE struLaneQueue;       	/*�������г���*/
    TPS_NET_VCA_POINT  struRuleLocation;	/*����λ��*/
    UINT8    byRes2[64];
}NET_LANE_PARAM;

//��ͨͳ�Ʋ�����Ϣ�ṹ��
typedef struct _NET_TPS_INFO_
{
    UINT32   dwLanNum;   				    /* ��ͨ�����ĳ�����Ŀ*/
    NET_LANE_PARAM struLaneParam[MAX_TPS_RULE];
	UINT8     byRes[32];    				/*����*/
}NET_TPS_INFO;

typedef struct _NET_VCA_DEV_INFO_
{
	U_IN_ADDR  struDevIP;				/*ǰ���豸��ַ*/ 
	UINT16 wPort; 						/*ǰ���豸�˿ں�*/ 
	UINT8 byChannel;  					/*ǰ���豸ͨ��*/
	UINT8 byIvmsChannel;				/*��ΪIVMSͨ����*/ 
} NET_VCA_DEV_INFO;

typedef struct _NET_TPS_ALARM_
{
    UINT32 dwSize;						/* �ṹ���С*/
    UINT32 dwRelativeTime;				/* ���ʱ��*/
    UINT32 dwAbsTime;					/* ����ʱ��*/
    NET_VCA_DEV_INFO struDevInfo;		/* ǰ���豸��Ϣ*/
    NET_TPS_INFO  struTPSInfo;			/* ��ͨ�¼���Ϣ*/
    //UINT8  byRes1[128];				/* �����ֽ�*/
    UINT32 dwDeviceId; /*�豸ID*/ 
    UINT8 byRes1[124]; /* �����ֽ�*/ 
}NET_TPS_ALARM;

/*************************Hikvision ��ͨ����ͨѶЭ��V2.0************************

**************************Add by lxp 2014-12-26**************************/

#define COMM_ALARM_TPS_REAL_TIME 0xb6  //��ͨ����ͳ�Ʊ���ʵʱ��Ϣ
#define COMM_ALARM_TPS_INTER_TIME 0xb7 //��ͨ����ͳ�Ʊ���ʱ�����Ϣ

typedef struct
{
    WORD wYear;                 //�꣬eg��2014
    BYTE byMonth;               //�£�1~12
    BYTE byDay;                 //�գ�1~31
    BYTE byHour;                //ʱ��0~23
    BYTE byMinute;              //�֣�0~59
    BYTE bySecond;              //�룬0~59
    BYTE byRes;
    WORD wMilliSec;             //���룬0~999
    UINT8 byRes1[2];
}INTER_TPS_TIME;

typedef struct _INTER_STRUCTHEAD
{
    UINT16 wLength;             /*�ṹ����*/
    UINT8 byVersion;            /*�ߵ�4λ�ֱ�����ߵͰ汾���������ݰ汾�ͳ��Ƚ�����չ��
                                ��ͬ�汾�ĳ��Ƚ������ƣ���һ���汾Ĭ��ʹ�õ���0.1�汾*/
    UINT8 byRes;
}INTER_STRUCTHEAD, *LPINTER_STRUCTHEAD;

//ʵʱ��Ϣ
typedef struct tagNET_DVR_TPS_REAL_TIME_INFO
{
    BYTE byStart;               //��ʼ��
    BYTE byCMD;                 //����ţ�01-����ָ�02-�뿪ָ�03-ӵ�� 
                                //״ָ̬��(Ϊ03ʱ��ֻ��byLaneState��byQueueLen��Ч)
    BYTE byRes[2];              //Ԥ���ֽ�
    WORD wDeviceID;             //�豸ID
    WORD wDataLen;              //���ݳ���
    BYTE byLane;                //��Ӧ������
    BYTE bySpeed;               //��Ӧ����
    BYTE byLaneState;           //����״̬��0-��״̬��1-��ͨ��2-ӵ����3-����
    BYTE byQueueLen;            //����״̬���Ŷӳ���
    BYTE byRes1[24];            //����
}NET_DVR_TPS_REAL_TIME_INFO, *LPNET_DVR_TPS_REAL_TIME_INFO;

typedef struct tagNET_DVR_TPS_REAL_TIME_ALARM
{
    INTER_STRUCTHEAD struVerHead;                   //�汾ͷ
    DWORD dwSize;                                   //ͨ����
    INTER_TPS_TIME struTime;                        //���ʱ��
    NET_DVR_TPS_REAL_TIME_INFO struTPSRealTimeInfo; //��ͨ����ͳ����Ϣ
    BYTE byRes[24];                                 //����
}NET_DVR_TPS_REAL_TIME_ALARM, *LPNET_DVR_TPS_REAL_TIME_ALARM;

//ʱ����Ϣ
#define TPS_MAX_LANE_NUM 8          //���ͳ�ƹ�������
typedef struct tagNET_DVR_LANE_PARAM
{
    BYTE byLane;                    //��Ӧ������
    BYTE bySpeed;                   //��������ƽ���ٶ�
    BYTE byRes[2];                  //����
    DWORD dwLaneVolume;             //����������ͳ���ж��ٳ���ͨ��
    DWORD dwTimeHeadway ;           //��ͷʱ�࣬�������
    DWORD dwSpaceHeadway;           //��ͷ��࣬����������
    WORD wSpaceOccupyRation;        //�ռ�ռ���ʣ��ٷֱȼ���,������*1000����341����ʾ34.1%
    WORD wTimeOccupyRation;         //ʱ��ռ���ʣ��ٷֱȼ���,������*1000����341����ʾ34.1%
    BYTE byRes1[8];                 //����
}NET_DVR_LANE_PARAM, *LPNET_DVR_LANE_PARAM;

typedef struct tagNET_DVR_TPS_INTER_TIME_INFO
{
    BYTE byStart;                   //��ʼ��
    BYTE byCMD;                     //����ţ� 08-��ʱ��������ָ��
    BYTE byRes[2];                  //Ԥ���ֽ�
    WORD wDeviceID;                 //�豸ID
    WORD wDataLen;                  //���ݳ���
    BYTE byTotalLaneNum;            //��Ч��������
    BYTE byRes1[15];                //Ԥ���ֽ�
    INTER_TPS_TIME struStartTime;   //ͳ�ƿ�ʼʱ��
    DWORD dwSamplePeriod;           //ͳ��ʱ��,��λ��
    NET_DVR_LANE_PARAM struLaneParam[TPS_MAX_LANE_NUM];
}NET_DVR_TPS_INTER_TIME_INFO, *LPNET_DVR_TPS_INTER_TIME_INFO;

typedef struct tagNET_DVR_TPS_INTER_TIME_ALARM
{
    INTER_STRUCTHEAD struVerHead;                       //�汾ͷ
    DWORD dwChan;                                       //ͨ����
    NET_DVR_TPS_INTER_TIME_INFO struTPSInterTimeInfo;   //��ͨ����ͳ����Ϣ
    BYTE byRes[128];                                    //����
}NET_DVR_TPS_INTER_TIME_ALARM, *LPNET_DVR_TPS_INTER_TIME_ALARM;

/************************************************************************/

//IS_NEED_DATA�ṹ����Ϊ�źŻ���Ҫ����Ϣ
typedef struct _IS_NEED_DATA_
{
    UINT32 varyType;					/* ��������*/
    UINT32 laneSpeed;					/*�����ٶ�*/
    UINT8  ruleID;						/* �������*/
    UINT32 cameraID;   					/*�豸ID*/ 

}IS_NEED_DATA;
#pragma pack(pop)

//��������
void  deal_data_from_video_v10(NET_TPS_ALARM  *alarmData);
void  deal_data_from_video_v20(NET_DVR_TPS_REAL_TIME_ALARM *realTimeData);
void date_send_by_can(char iProtocol,int iNum,UINT32 iCamId);
#ifdef __cplusplus
}
#endif
 
#endif