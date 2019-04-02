#include "main.h"

//debug任务函数
void debug_task(void *p_arg)
{
	static u32 i = 0,j = 0;
	OS_ERR err;
	p_arg = p_arg;
	OSTaskSuspend((OS_TCB*)&SampleTaskTCB,&err);	//关闭传感器数据采集，否则会影响
	OSTaskSuspend((OS_TCB*)&Can1TaskTCB,&err);			//关闭传感器数据发送，否则会影响

while(1)
{
//测试传感数据反馈实验时使用
#ifdef DEBUG_DATA_FEEDBACK
	OSSemPend(&SYNC_SEM,0,OS_OPT_PEND_BLOCKING,0,&err); //请求信号量1，等待测试开始
	if(i>dataSize)			//发送传感器数据结束后上传每帧的时间
	{
		if(i==dataSize+1)//传感器数据发送结束，通知主控准备接受帧时间
		{
			can1Buffer[0] = 0x01;		
			can1SendMsg(can1Buffer,MII_MSG_DEBUG_2,1);
			i++;
		}
		OSSemPend(&SYNC_SEM1,0,OS_OPT_PEND_BLOCKING,0,&err1); //请求信号量2，等待发送帧时间开始

		if(j<dataSize)		//发送帧时间
		{
			can1Buffer[0] = countBuffer[j]&0xff;
			can1Buffer[1] = (countBuffer[j]>>8)&0xff;
			can1Buffer[2] = j&0xff;
			can1Buffer[3] = (j>>8)&0xff;
			can1SendMsg(can1Buffer,MII_MSG_HEARTBEAT_1,4);
			OSSemPost(&SYNC_SEM1,OS_OPT_POST_1,&err1);//发送信号量2		
		}
		if(j == dataSize)	//通知主控测试结束
		{
			can1Buffer[0] = 0x00;		//结束标志
			can1SendMsg(can1Buffer,MII_MSG_DEBUG_2,1);
		}
		j++;
	}
	else				//发送传感器数据，并记录发送时间
	{	
		if(i==0)			//通知主控准备接受传感器数据，测试开始
		{
			can1SendMsg(can1Buffer,MII_MSG_DEBUG_1,0);
//			OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_HMSM_STRICT,&err); //延时,发起任务调度
		}	
		//发送传感器数据,每次两帧
		
		if((i>0)&&(i<=dataSize))
		{
			can1Buffer[0] = i&0xff;
			can1Buffer[1] = (i>>8)&0xff;
			can1Buffer[2] = (i>>16)&0xff;
			countBuffer[i-1] = count_ms;		//记录帧时间	
			can1SendMsg(can1Buffer,MII_MSG_HEARTBEAT_1,8);  
//		i++;
//		countBuffer[i] = count_ms;		//记录帧时间	
//		can1Buffer[0] = i&0xff;
//		can1Buffer[1] = (i>>8)&0xff;
//		can1Buffer[2] = (i>>16)&0xff;
//		can1SendMsg(can1Buffer,MII_MSG_HEARTBEAT_1,8);
		}
		i++;
	}
	OSSemPost(&SYNC_SEM,OS_OPT_POST_1,&err);//发送信号量1
	OSTimeDlyHMSM(0,0,0,CYC_TIME,OS_OPT_TIME_HMSM_STRICT,&err); //延时,发起任务调度
#endif
	
//测试传感数据反馈实验（补充内容）时使用
#ifdef DEBUG_DATA_FEEDBACK1
	OSSemPend(&SYNC_SEM,0,OS_OPT_PEND_BLOCKING,0,&err); //请求信号量1，等待测试开始

		if(i==0)			//通知主控准备接受传感器数据，测试开始
		{
			can1SendMsg(can1Buffer,MII_MSG_DEBUG_1,0);
//			OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_HMSM_STRICT,&err); //延时,发起任务调度
		}	
		else
		{
			can1Buffer[0] = i&0xff;
			can1Buffer[1] = (i>>8)&0xff;
			can1Buffer[2] = (i>>16)&0xff;
			countBuffer[i-1] = count_ms;		//记录帧时间	
			can1SendMsg(can1Buffer,MII_MSG_HEARTBEAT_1,8);  

		}
		i++;
	}
	OSSemPost(&SYNC_SEM,OS_OPT_POST_1,&err);//发送信号量1
	OSTimeDlyHMSM(0,0,0,CYC_TIME,OS_OPT_TIME_HMSM_STRICT,&err); //延时,发起任务调度
#endif
	
//测试数据出错率时使用
#ifdef DEBUG_DATA_ERR
	OSSemPend(&SYNC_SEM,0,OS_OPT_PEND_BLOCKING,0,&err); //请求信号量1
	//数据发送结束后发送帧时间
	if(i>dataSize)
	{
		if(i==dataSize+1)//传感器数据发送结束，通知主控准备接受帧时间
		{
			can1Buffer[0] = 0x01;		
			can1SendMsg(can1Buffer,MII_MSG_DEBUG_2,1);
			i++;
		}
		OSSemPend(&SYNC_SEM1,0,OS_OPT_PEND_BLOCKING,0,&err1); //请求信号量2，等待发送帧时间开始

		if(j == 0)		//发送帧时间
		{
			//上传开始、结束时间
			can1Buffer[0] = countBuffer[0]&0xff;
			can1Buffer[1] = (countBuffer[0]>>8)&0xff;
			can1SendMsg(can1Buffer,MII_MSG_HEARTBEAT_1,4);			
			can1Buffer[0] = countBuffer[1]&0xff;
			can1Buffer[1] = (countBuffer[1]>>8)&0xff;
			can1Buffer[2] = (countBuffer[2]>>16)&0xff;
			can1Buffer[3] = (countBuffer[2]>>24)&0xff;
			can1SendMsg(can1Buffer,MII_MSG_HEARTBEAT_1,4);
			OSSemPost(&SYNC_SEM1,OS_OPT_POST_1,&err1);//发送信号量2
		}
		if(j == 1)	//通知主控测试结束
		{
			can1Buffer[0] = 0x00;		//结束标志
			can1SendMsg(can1Buffer,MII_MSG_DEBUG_2,1);
		}
		j++;	
	}
	//发送数据
	else
	{
		if(i == 0)
		{
			countBuffer[0] = count_ms;		//记录开始时间
			can1SendMsg(can1Buffer,MII_MSG_DEBUG_1,0);	//通知主控准备接受传感器数据，测试开始
		}
		
		if((i>0)&&(i<=dataSize))
		{
			can1Buffer[0] = i&0xff;
			can1Buffer[1] = (i>>8)&0xff;
			can1Buffer[2] = (i>>16)&0xff;
			can1Buffer[3] = (i>>24)&0xff;
			can1SendMsg(can1Buffer,MII_MSG_HEARTBEAT_1,8);
			i++;
			can1Buffer[0] = i&0xff;
			can1Buffer[1] = (i>>8)&0xff;
			can1Buffer[2] = (i>>16)&0xff;
			can1Buffer[3] = (i>>24)&0xff;	
			can1SendMsg(can1Buffer,MII_MSG_HEARTBEAT_1,8);		
			i++;
		}

		if(i == dataSize)
		{
			countBuffer[1] = count_ms;				//记录结束时间
			countBuffer[2] = count_ms>>16;		//记录结束时间
		}
	}
	OSSemPost(&SYNC_SEM,OS_OPT_POST_1,&err);//发送信号量1
	OSTimeDlyHMSM(0,0,0,CYC_TIME,OS_OPT_TIME_HMSM_STRICT,&err); //延时,发起任务调度
#endif	
	
//测试数据交换时间时使用
#ifdef DEBUG_DATA_TRANS
	OSSemPend(&SYNC_SEM,0,OS_OPT_PEND_BLOCKING,0,&err); //请求信号量1
	if(i>dataSize)					//发送传感器数据结束后上传每帧的时间
	{
		if(i == dataSize+1)			//通知主控准备接受帧时间
		{
			can1Buffer[0] = 0x01;		
			can1SendMsg(can1Buffer,MII_MSG_DEBUG_2,1);
			i++;
		}
		OSSemPend(&SYNC_SEM1,0,OS_OPT_PEND_BLOCKING,0,&err1); //请求信号量2，等待发送帧时间开始
		if(j<dataSize)		//发送帧时间
		{
			can1Buffer[0] = countBuffer[j]&0xff;
			can1Buffer[1] = (countBuffer[j]>>8)&0xff;
			can1Buffer[2] = j&0xff;
			can1Buffer[3] = (j>>8)&0xff;
			can1SendMsg(can1Buffer,MII_MSG_HEARTBEAT_1,4);
			OSSemPost(&SYNC_SEM1,OS_OPT_POST_1,&err1);//发送信号量2			
		}
		if(j == dataSize)	//通知主控测试结束
		{
			can1Buffer[0] = 0x00;		//结束标志
			can1SendMsg(can1Buffer,MII_MSG_DEBUG_2,1);
		}
		j++;
	}
	else						//发送传感器数据，并记录发送时间
	{	
		if(i==0)			//通知主控准备接受传感器数据，测试开始
		{
			can1SendMsg(can1Buffer,MII_MSG_DEBUG_1,0);
		}
		if（(i>0)&&(i<=dataSize)）
		{
			//发送传感器数据
			can1Buffer[0] = i&0xff;
			can1Buffer[1] = (i>>8)&0xff;
			can1Buffer[2] = (i>>16)&0xff;
			can1SendMsg(can1Buffer,MII_MSG_HEARTBEAT_1,8);
			countBuffer[i] = count_ms;		//记录帧时间	
			i++;
			//发送传感器数据
			can1Buffer[0] = i&0xff;
			can1Buffer[1] = (i>>8)&0xff;
			can1Buffer[2] = (i>>16)&0xff;
			can1SendMsg(can1Buffer,MII_MSG_HEARTBEAT_1,8);
			countBuffer[i] = count_ms;		//记录帧时间	
			i++;			
		}

	}
	OSSemPost(&SYNC_SEM,OS_OPT_POST_1,&err);//发送信号量1
	OSTimeDlyHMSM(0,0,0,CYC_TIME,OS_OPT_TIME_HMSM_STRICT,&err); //延时,发起任务调度
#endif		

//测试CPU使用率时使用
#ifdef DEBUG_CPU_USAGE
//OSTaskResume((OS_TCB*)&CanTaskTCB,&err);					//打开传感器数据发送，正常运行
OSSemPend(&SYNC_SEM,0,OS_OPT_PEND_BLOCKING,0,&err); //请求信号量1
	if(i>dataSize)					//上传记录的CPU使用率
	{
		if(i == dataSize+1)			//通知主控准备接受CPU占用率
		{
			OSTaskSuspend((OS_TCB*)&Can1TaskTCB,&err);			//关闭传感器数据发送，否则会影响
			can1Buffer[0] = 0x01;		
			can1SendMsg(can1Buffer,MII_MSG_DEBUG_2,1);
			i++;
		}
//		OSSemPend(&SYNC_SEM1,0,OS_OPT_PEND_BLOCKING,0,&err1); //请求信号量2，等待发送帧时间开始

		//发送测试期间CPU占用率
		if(j<=dataSize)
		{
			can1Buffer[0] = countBuffer[j]&0xff;
			can1Buffer[1] = (countBuffer[j]>>8)&0xff;
			can1Buffer[2] = j&0xff;
			can1Buffer[3] = (j>>8)&0xff;
			can1SendMsg(can1Buffer,MII_MSG_HEARTBEAT_1,4);
//			OSSemPost(&SYNC_SEM1,OS_OPT_POST_1,&err1);//发送信号量2	
		}
		if(j == dataSize)	//通知主控测试结束
		{
			can1Buffer[0] = 0x00;		//结束标志
			can1SendMsg(can1Buffer,MII_MSG_DEBUG_2,1);
		}
		j++;
		OSTimeDlyHMSM(0,0,0,1,OS_OPT_TIME_HMSM_STRICT,&err); //延时,发起任务调度
	}
	else
	{

		if(i == 0)
		{
			can1SendMsg(can1Buffer,MII_MSG_DEBUG_1,0);	//通知主控准备接受传感器数据，测试开始
			OSTaskResume((OS_TCB*)&SampleTaskTCB,&err);		//打开传感器数据采集，正常运行
			OSTaskResume((OS_TCB*)&Can1TaskTCB,&err);			//打开传感器数据发送，正常运行
		}
		countBuffer[i] = OSStatTaskCPUUsage;		//记录CPU使用率0-10000；		
		i++;
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时,发起任务调度
	}
	OSSemPost(&SYNC_SEM,OS_OPT_POST_1,&err);//发送信号量1
#endif
	}	
}

