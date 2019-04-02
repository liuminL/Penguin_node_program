#include "main.h"

//debug������
void debug_task(void *p_arg)
{
	static u32 i = 0,j = 0;
	OS_ERR err;
	p_arg = p_arg;
	OSTaskSuspend((OS_TCB*)&SampleTaskTCB,&err);	//�رմ��������ݲɼ��������Ӱ��
	OSTaskSuspend((OS_TCB*)&Can1TaskTCB,&err);			//�رմ��������ݷ��ͣ������Ӱ��

while(1)
{
//���Դ������ݷ���ʵ��ʱʹ��
#ifdef DEBUG_DATA_FEEDBACK
	OSSemPend(&SYNC_SEM,0,OS_OPT_PEND_BLOCKING,0,&err); //�����ź���1���ȴ����Կ�ʼ
	if(i>dataSize)			//���ʹ��������ݽ������ϴ�ÿ֡��ʱ��
	{
		if(i==dataSize+1)//���������ݷ��ͽ�����֪ͨ����׼������֡ʱ��
		{
			can1Buffer[0] = 0x01;		
			can1SendMsg(can1Buffer,MII_MSG_DEBUG_2,1);
			i++;
		}
		OSSemPend(&SYNC_SEM1,0,OS_OPT_PEND_BLOCKING,0,&err1); //�����ź���2���ȴ�����֡ʱ�俪ʼ

		if(j<dataSize)		//����֡ʱ��
		{
			can1Buffer[0] = countBuffer[j]&0xff;
			can1Buffer[1] = (countBuffer[j]>>8)&0xff;
			can1Buffer[2] = j&0xff;
			can1Buffer[3] = (j>>8)&0xff;
			can1SendMsg(can1Buffer,MII_MSG_HEARTBEAT_1,4);
			OSSemPost(&SYNC_SEM1,OS_OPT_POST_1,&err1);//�����ź���2		
		}
		if(j == dataSize)	//֪ͨ���ز��Խ���
		{
			can1Buffer[0] = 0x00;		//������־
			can1SendMsg(can1Buffer,MII_MSG_DEBUG_2,1);
		}
		j++;
	}
	else				//���ʹ��������ݣ�����¼����ʱ��
	{	
		if(i==0)			//֪ͨ����׼�����ܴ��������ݣ����Կ�ʼ
		{
			can1SendMsg(can1Buffer,MII_MSG_DEBUG_1,0);
//			OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ,�����������
		}	
		//���ʹ���������,ÿ����֡
		
		if((i>0)&&(i<=dataSize))
		{
			can1Buffer[0] = i&0xff;
			can1Buffer[1] = (i>>8)&0xff;
			can1Buffer[2] = (i>>16)&0xff;
			countBuffer[i-1] = count_ms;		//��¼֡ʱ��	
			can1SendMsg(can1Buffer,MII_MSG_HEARTBEAT_1,8);  
//		i++;
//		countBuffer[i] = count_ms;		//��¼֡ʱ��	
//		can1Buffer[0] = i&0xff;
//		can1Buffer[1] = (i>>8)&0xff;
//		can1Buffer[2] = (i>>16)&0xff;
//		can1SendMsg(can1Buffer,MII_MSG_HEARTBEAT_1,8);
		}
		i++;
	}
	OSSemPost(&SYNC_SEM,OS_OPT_POST_1,&err);//�����ź���1
	OSTimeDlyHMSM(0,0,0,CYC_TIME,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ,�����������
#endif
	
//���Դ������ݷ���ʵ�飨�������ݣ�ʱʹ��
#ifdef DEBUG_DATA_FEEDBACK1
	OSSemPend(&SYNC_SEM,0,OS_OPT_PEND_BLOCKING,0,&err); //�����ź���1���ȴ����Կ�ʼ

		if(i==0)			//֪ͨ����׼�����ܴ��������ݣ����Կ�ʼ
		{
			can1SendMsg(can1Buffer,MII_MSG_DEBUG_1,0);
//			OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ,�����������
		}	
		else
		{
			can1Buffer[0] = i&0xff;
			can1Buffer[1] = (i>>8)&0xff;
			can1Buffer[2] = (i>>16)&0xff;
			countBuffer[i-1] = count_ms;		//��¼֡ʱ��	
			can1SendMsg(can1Buffer,MII_MSG_HEARTBEAT_1,8);  

		}
		i++;
	}
	OSSemPost(&SYNC_SEM,OS_OPT_POST_1,&err);//�����ź���1
	OSTimeDlyHMSM(0,0,0,CYC_TIME,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ,�����������
#endif
	
//�������ݳ�����ʱʹ��
#ifdef DEBUG_DATA_ERR
	OSSemPend(&SYNC_SEM,0,OS_OPT_PEND_BLOCKING,0,&err); //�����ź���1
	//���ݷ��ͽ�������֡ʱ��
	if(i>dataSize)
	{
		if(i==dataSize+1)//���������ݷ��ͽ�����֪ͨ����׼������֡ʱ��
		{
			can1Buffer[0] = 0x01;		
			can1SendMsg(can1Buffer,MII_MSG_DEBUG_2,1);
			i++;
		}
		OSSemPend(&SYNC_SEM1,0,OS_OPT_PEND_BLOCKING,0,&err1); //�����ź���2���ȴ�����֡ʱ�俪ʼ

		if(j == 0)		//����֡ʱ��
		{
			//�ϴ���ʼ������ʱ��
			can1Buffer[0] = countBuffer[0]&0xff;
			can1Buffer[1] = (countBuffer[0]>>8)&0xff;
			can1SendMsg(can1Buffer,MII_MSG_HEARTBEAT_1,4);			
			can1Buffer[0] = countBuffer[1]&0xff;
			can1Buffer[1] = (countBuffer[1]>>8)&0xff;
			can1Buffer[2] = (countBuffer[2]>>16)&0xff;
			can1Buffer[3] = (countBuffer[2]>>24)&0xff;
			can1SendMsg(can1Buffer,MII_MSG_HEARTBEAT_1,4);
			OSSemPost(&SYNC_SEM1,OS_OPT_POST_1,&err1);//�����ź���2
		}
		if(j == 1)	//֪ͨ���ز��Խ���
		{
			can1Buffer[0] = 0x00;		//������־
			can1SendMsg(can1Buffer,MII_MSG_DEBUG_2,1);
		}
		j++;	
	}
	//��������
	else
	{
		if(i == 0)
		{
			countBuffer[0] = count_ms;		//��¼��ʼʱ��
			can1SendMsg(can1Buffer,MII_MSG_DEBUG_1,0);	//֪ͨ����׼�����ܴ��������ݣ����Կ�ʼ
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
			countBuffer[1] = count_ms;				//��¼����ʱ��
			countBuffer[2] = count_ms>>16;		//��¼����ʱ��
		}
	}
	OSSemPost(&SYNC_SEM,OS_OPT_POST_1,&err);//�����ź���1
	OSTimeDlyHMSM(0,0,0,CYC_TIME,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ,�����������
#endif	
	
//�������ݽ���ʱ��ʱʹ��
#ifdef DEBUG_DATA_TRANS
	OSSemPend(&SYNC_SEM,0,OS_OPT_PEND_BLOCKING,0,&err); //�����ź���1
	if(i>dataSize)					//���ʹ��������ݽ������ϴ�ÿ֡��ʱ��
	{
		if(i == dataSize+1)			//֪ͨ����׼������֡ʱ��
		{
			can1Buffer[0] = 0x01;		
			can1SendMsg(can1Buffer,MII_MSG_DEBUG_2,1);
			i++;
		}
		OSSemPend(&SYNC_SEM1,0,OS_OPT_PEND_BLOCKING,0,&err1); //�����ź���2���ȴ�����֡ʱ�俪ʼ
		if(j<dataSize)		//����֡ʱ��
		{
			can1Buffer[0] = countBuffer[j]&0xff;
			can1Buffer[1] = (countBuffer[j]>>8)&0xff;
			can1Buffer[2] = j&0xff;
			can1Buffer[3] = (j>>8)&0xff;
			can1SendMsg(can1Buffer,MII_MSG_HEARTBEAT_1,4);
			OSSemPost(&SYNC_SEM1,OS_OPT_POST_1,&err1);//�����ź���2			
		}
		if(j == dataSize)	//֪ͨ���ز��Խ���
		{
			can1Buffer[0] = 0x00;		//������־
			can1SendMsg(can1Buffer,MII_MSG_DEBUG_2,1);
		}
		j++;
	}
	else						//���ʹ��������ݣ�����¼����ʱ��
	{	
		if(i==0)			//֪ͨ����׼�����ܴ��������ݣ����Կ�ʼ
		{
			can1SendMsg(can1Buffer,MII_MSG_DEBUG_1,0);
		}
		if��(i>0)&&(i<=dataSize)��
		{
			//���ʹ���������
			can1Buffer[0] = i&0xff;
			can1Buffer[1] = (i>>8)&0xff;
			can1Buffer[2] = (i>>16)&0xff;
			can1SendMsg(can1Buffer,MII_MSG_HEARTBEAT_1,8);
			countBuffer[i] = count_ms;		//��¼֡ʱ��	
			i++;
			//���ʹ���������
			can1Buffer[0] = i&0xff;
			can1Buffer[1] = (i>>8)&0xff;
			can1Buffer[2] = (i>>16)&0xff;
			can1SendMsg(can1Buffer,MII_MSG_HEARTBEAT_1,8);
			countBuffer[i] = count_ms;		//��¼֡ʱ��	
			i++;			
		}

	}
	OSSemPost(&SYNC_SEM,OS_OPT_POST_1,&err);//�����ź���1
	OSTimeDlyHMSM(0,0,0,CYC_TIME,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ,�����������
#endif		

//����CPUʹ����ʱʹ��
#ifdef DEBUG_CPU_USAGE
//OSTaskResume((OS_TCB*)&CanTaskTCB,&err);					//�򿪴��������ݷ��ͣ���������
OSSemPend(&SYNC_SEM,0,OS_OPT_PEND_BLOCKING,0,&err); //�����ź���1
	if(i>dataSize)					//�ϴ���¼��CPUʹ����
	{
		if(i == dataSize+1)			//֪ͨ����׼������CPUռ����
		{
			OSTaskSuspend((OS_TCB*)&Can1TaskTCB,&err);			//�رմ��������ݷ��ͣ������Ӱ��
			can1Buffer[0] = 0x01;		
			can1SendMsg(can1Buffer,MII_MSG_DEBUG_2,1);
			i++;
		}
//		OSSemPend(&SYNC_SEM1,0,OS_OPT_PEND_BLOCKING,0,&err1); //�����ź���2���ȴ�����֡ʱ�俪ʼ

		//���Ͳ����ڼ�CPUռ����
		if(j<=dataSize)
		{
			can1Buffer[0] = countBuffer[j]&0xff;
			can1Buffer[1] = (countBuffer[j]>>8)&0xff;
			can1Buffer[2] = j&0xff;
			can1Buffer[3] = (j>>8)&0xff;
			can1SendMsg(can1Buffer,MII_MSG_HEARTBEAT_1,4);
//			OSSemPost(&SYNC_SEM1,OS_OPT_POST_1,&err1);//�����ź���2	
		}
		if(j == dataSize)	//֪ͨ���ز��Խ���
		{
			can1Buffer[0] = 0x00;		//������־
			can1SendMsg(can1Buffer,MII_MSG_DEBUG_2,1);
		}
		j++;
		OSTimeDlyHMSM(0,0,0,1,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ,�����������
	}
	else
	{

		if(i == 0)
		{
			can1SendMsg(can1Buffer,MII_MSG_DEBUG_1,0);	//֪ͨ����׼�����ܴ��������ݣ����Կ�ʼ
			OSTaskResume((OS_TCB*)&SampleTaskTCB,&err);		//�򿪴��������ݲɼ�����������
			OSTaskResume((OS_TCB*)&Can1TaskTCB,&err);			//�򿪴��������ݷ��ͣ���������
		}
		countBuffer[i] = OSStatTaskCPUUsage;		//��¼CPUʹ����0-10000��		
		i++;
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ,�����������
	}
	OSSemPost(&SYNC_SEM,OS_OPT_POST_1,&err);//�����ź���1
#endif
	}	
}

