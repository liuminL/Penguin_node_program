#ifndef __MAIN_H
#define __MAIN_H

#include "sys.h"
#include "delay.h"
#include "led.h"
#include "adc.h"
#include "can.h"
#include "stm32f10x.h"
#include "usart.h"
#include "adc.h"
#include "includes.h"
#include "qr_protocol.h"
#include "timer.h"
#include "driver.h"
#include "driver2.h"
#include "pwm.h"

#define abs(x) ((x)>0? (x):(-(x)))
//��Դ��ѹ
#define VOLTAGE					3.278
//hip�Ƕȼ��㹫ʽб��
#define ANGLE_RATIO		-6000*2*VOLTAGE/(4096*4.922)	
//hip�Ƕȼ��㹫ʽ�ؾ�
#define ANGLE_OFFSET		6000*0.058/4.922

//knee��hipŤ�ؼ��㹫ʽ�ؾ�
#define TORQUE_KNEE_OFFSET		72
//knee��hipŤ�ؼ��㹫ʽ�ؾ�
#define TORQUE_HIP_OFFSET			69

//�¶ȼ��㹫ʽб��
#define TEMP_RATIO		VOLTAGE*1000/(4096*5.5)	
//�¶ȼ��㹫ʽ�ؾ�
#define TEMP_OFFSET		183

//#define DEBUG_EN
//��DEBUG_ENʹ�ܺ����²������ݻ��⣬ÿ��ֻ��ʹ��һ��
//#define DEBUG_DATA_FEEDBACK
//#define DEBUG_DATA_FEEDBACK1
//#define DEBUG_DATA_ERR
//#define DEBUG_DATA_TRANS
//#define DEBUG_CPU_USAGE

//���Դ������ݷ���ʵ��ʱʹ��
#define DATA_FEEDBACK_MS	 		4		//��������
//�������ݳ�����ʱʹ��
#define DATA_ERR_MS						5		//��������
//�������ݽ���ʱ��ʱʹ��
#define DATA_TRANS_MS					5		//��������
//����CPUʹ����ʱʹ��
#define CPU_MS								500	//CPUʹ���ʲ���ʱ�䣬ms,����ʱ��̶���5ms

#ifdef DEBUG_DATA_FEEDBACK
	#define CYC_TIME	DATA_FEEDBACK_MS
#endif
#ifdef DEBUG_DATA_ERR
	#define CYC_TIME	DATA_ERR_MS
#endif
#ifdef DEBUG_DATA_TRANS
	#define CYC_TIME	DATA_TRANS_MS
#endif
#ifdef DEBUG_CPU_USAGE
	#define CYC_TIME	CPU_MS
#endif
#ifndef	DEBUG_DATA_FEEDBACK && DEBUG_DATA_ERR && DEBUG_DATA_TRANS && DEBUG_CPU_USAGE
	#define CYC_TIME	5
#endif

#ifdef DEBUG_EN
#define DATA_CNT	15000
extern uint16_t countBuffer[DATA_CNT];
#endif

typedef struct _Sensorstruct
{
	float kneeMotorTemp;
	float hipMotorTemp;
	float yawMotorTemp;
}sensorStruct;


extern uint16_t timeCountMs;
extern uint32_t dataSize;
extern char nodeStatus;
extern sensorStruct sensor;
extern OS_SEM	SYNC_SEM,SYNC_SEM1;		//����һ���ź���1��2����������ͬ��
extern OS_TCB Can1TaskTCB;						//������ƿ�
extern OS_TCB SampleTaskTCB;
extern u16 arr1,arr2,psc1,psc2;

#define abs(x) ((x)>0? (x):(-(x))

#endif
