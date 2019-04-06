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
//电源电压
#define VOLTAGE					3.278
//hip角度计算公式斜率
#define ANGLE_RATIO		-6000*2*VOLTAGE/(4096*4.922)	
//hip角度计算公式截距
#define ANGLE_OFFSET		6000*0.058/4.922

//knee、hip扭矩计算公式截距
#define TORQUE_KNEE_OFFSET		72
//knee、hip扭矩计算公式截距
#define TORQUE_HIP_OFFSET			69

//温度计算公式斜率
#define TEMP_RATIO		VOLTAGE*1000/(4096*5.5)	
//温度计算公式截距
#define TEMP_OFFSET		183

//#define DEBUG_EN
//在DEBUG_EN使能后，以下测试内容互斥，每次只能使能一个
//#define DEBUG_DATA_FEEDBACK
//#define DEBUG_DATA_FEEDBACK1
//#define DEBUG_DATA_ERR
//#define DEBUG_DATA_TRANS
//#define DEBUG_CPU_USAGE

//测试传感数据反馈实验时使用
#define DATA_FEEDBACK_MS	 		4		//反馈周期
//测试数据出错率时使用
#define DATA_ERR_MS						5		//反馈周期
//测试数据交换时间时使用
#define DATA_TRANS_MS					5		//反馈周期
//测试CPU使用率时使用
#define CPU_MS								500	//CPU使用率采样时间，ms,反馈时间固定：5ms

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
extern OS_SEM	SYNC_SEM,SYNC_SEM1;		//定义一个信号量1、2，用于任务同步
extern OS_TCB Can1TaskTCB;						//任务控制块
extern OS_TCB SampleTaskTCB;
extern u16 arr1,arr2,psc1,psc2;

#define abs(x) ((x)>0? (x):(-(x))

#endif
