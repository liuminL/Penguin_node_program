#include "main.h"

//��ʼ��������
#define START_TASK_PRIO		1							//�������ȼ�
#define START_STK_SIZE 		1024					//�����ջ��С
OS_TCB StartTaskTCB;										//������ƿ�
CPU_STK START_TASK_STK[START_STK_SIZE];	//�����ջ
void start_task(void *p_arg);						//������

//������������
#define SAMPLE_TASK_PRIO		2							//�������ȼ�
#define SAMPLE_STK_SIZE 		128						//�����ջ��С
OS_TCB SampleTaskTCB;											//������ƿ�
CPU_STK SAMPLE_TASK_STK[SAMPLE_STK_SIZE];	//�����ջ
void sample_task(void *p_arg);						//������

//can1������������
#define CAN1_TASK_PRIO		3							//�������ȼ�
#define CAN1_STK_SIZE 		128						//�����ջ��С
OS_TCB Can1TaskTCB;										//������ƿ�
CPU_STK CAN1_TASK_STK[CAN1_STK_SIZE];		//�����ջ
void can1_task(void *p_arg);						//������


//can2������������
#define CAN2_TASK_PRIO		4							//�������ȼ�
#define CAN2_STK_SIZE 		128						//�����ջ��С
OS_TCB Can2TaskTCB;										//������ƿ�
CPU_STK CAN2_TASK_STK[CAN2_STK_SIZE];		//�����ջ
void can2_task(void *p_arg);						//������

//PWM���ɿ�����������
#define STEER_TASK_PRIO     5
#define STEER_STK_SIZE     128
OS_TCB SteerTaskTCB;
CPU_STK Steer_TASK_STK[STEER_STK_SIZE];
void steer_task(void *p_arg);


#ifdef DEBUG_EN
//debug��������
#define DEBUG_TASK_PRIO		9							//�������ȼ�
#define DEBUG_STK_SIZE 		128						//�����ջ��С
OS_TCB DebugTaskTCB;										//������ƿ�
CPU_STK DEBUG_TASK_STK[DEBUG_STK_SIZE];	//�����ջ
void debug_task(void *p_arg);						//������
#endif

//LED��ʾ��������
#define LED_TASK_PRIO		10					//�������ȼ�
#define LED_STK_SIZE 		128					//�����ջ��С
OS_TCB LedTaskTCB;									//������ƿ�
CPU_STK LED_TASK_STK[LED_STK_SIZE];	//�����ջ
void led_task(void *p_arg);					//������


uint32_t dataSize = 0;
char nodeStatus = 0;		//�ڵ��ע���־λ
sensorStruct sensor;
OS_SEM	SYNC_SEM,SYNC_SEM1;		//����һ���ź���1����������ͬ��

#ifdef DEBUG_EN
uint16_t countBuffer[DATA_CNT];
#endif

u16 arr1 = 9999,arr2 = 9999,psc1 = 143,psc2 = 143;

int main(void)
{	
	OS_ERR err;
	CPU_SR_ALLOC();
	delay_init();	    	 	//��ʱ������ʼ��
	SystemInit();					//ϵͳ������ʼ��
	sampleInit();					//���������ݲɼ�������ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	can1Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,2,CAN_Mode_Normal);		//CAN1��ʼ������ģʽ,������1000kbps
	can2Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,2,CAN_Mode_Normal);		//CAN2��ʼ������ģʽ,������1000kbps 
	LED_Init();	
	TIM5_Int_Init(9,7199);//10Khz�ļ���Ƶ�ʣ�������10Ϊ1ms  
	/*
	delay_ms(500);                                      //�տ�ʼҪ���㹻����ʱ��ȷ���������Ѿ���ʼ����� 
	CAN_RoboModule_DRV_Reset(0,0);                      //��0���������������и�λ 
	delay_ms(500);                                      //���͸�λָ������ʱ����Ҫ�У��ȴ��������ٴγ�ʼ�����
	CAN_RoboModule_DRV_Mode_Choice(0,0,Velocity_Position_Mode);  //0������������� �������ٶ�λ��ģʽ
	delay_ms(500);                                      //����ģʽѡ��ָ���Ҫ�ȴ�����������ģʽ������������ʱҲ������ȥ����
	
	CAN_RoboModule_DRV_Config(0,1,0x0A, 0);			//�������ⷢ���ٶȡ�������λ�ã�������λ
	delay_ms(500);
	CAN_RoboModule_DRV_Config(0,2,0x0A, 0);			//�������ⷢ���ٶȡ�������λ�ã�������λ
	delay_ms(500);
	CAN_RoboModule_DRV_Config(0,3,0x0A, 0);			//�������ⷢ���ٶȡ�������λ�ã�������λ
	delay_ms(500);
	*/
	/*
	delay_ms(500);
	CAN_BLDC_ResetMod(1, BLDC_Position_Mode);
	delay_ms(500);
	CAN_BLDC_ResetMod(2, BLDC_Position_Mode);
	delay_ms(500);
	CAN_BLDC_ResetMod(3, BLDC_Position_Mode);
	delay_ms(500);
	CAN_BLDC_ResetMod(4, BLDC_Position_Mode);
	delay_ms(500);
	*/
	TIM3_PWM_Init(arr1,psc1);//pwmƵ�ʣ�72000000/(psc+1)=0.5M,  0.5M/(arr+1)=50HZ
	TIM1_PWM_Init(arr1,psc1);
	
	OSInit(&err);				//��ʼ��UCOSIII
	OS_CRITICAL_ENTER();//�����ٽ���
	//������ʼ����
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,					//������ƿ�
				 (CPU_CHAR	* )"start task", 							//��������
                 (OS_TASK_PTR )start_task, 				//������
                 (void		* )0,										//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,									//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,										//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,										//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);								//��Ÿú�������ʱ�ķ���ֵ
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSStart(&err);  //����UCOSIII
	while(1);
}

//start������
void start_task(void *p_arg)
{
	OS_ERR err,err1;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif	
	OS_CRITICAL_ENTER();//�����ٽ���
	//�����ź���1
	OSSemCreate ((OS_SEM*	)&SYNC_SEM,
                 (CPU_CHAR*	)"SYNC_SEM",
                 (OS_SEM_CTR)0,		
                 (OS_ERR*	)&err);					 
	//�����ź���2
	OSSemCreate ((OS_SEM*	)&SYNC_SEM1,
                 (CPU_CHAR*	)"SYNC_SEM1",
                 (OS_SEM_CTR)0,		
                 (OS_ERR*	)&err1);	
//���Դ���
#ifdef DEBUG_EN
//����debug����
	OSTaskCreate((OS_TCB 	* )&DebugTaskTCB,					//������ƿ�
				 (CPU_CHAR	* )"debug task", 							//��������
                 (OS_TASK_PTR )debug_task, 				//������
                 (void		* )0,										//���ݸ��������Ĳ���
                 (OS_PRIO	  )DEBUG_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&DEBUG_TASK_STK[0],//�����ջ����ַ
                 (CPU_STK_SIZE)DEBUG_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)DEBUG_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,									//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,										//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,										//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);								//��Ÿú�������ʱ�ķ���ֵ
		 
#endif				 
//�ǵ��Դ���
	//����can1��������	
	 OSTaskCreate((OS_TCB 	* )&Can1TaskTCB,					//������ƿ�
				 (CPU_CHAR	* )"can1 task", 							//��������
                 (OS_TASK_PTR )can1_task, 				//������
                 (void		* )0,									//���ݸ��������Ĳ���
                 (OS_PRIO	  )CAN1_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&CAN1_TASK_STK[0],//�����ջ����ַ
                 (CPU_STK_SIZE)CAN1_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)CAN1_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,								//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,									//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,									//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);							//��Ÿú�������ʱ�ķ���ֵ 	

	//����can2��������	
	 OSTaskCreate((OS_TCB 	* )&Can2TaskTCB,					//������ƿ�
				 (CPU_CHAR	* )"can2 task", 							//��������
                 (OS_TASK_PTR )can2_task, 				//������
                 (void		* )0,									//���ݸ��������Ĳ���
                 (OS_PRIO	  )CAN2_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&CAN2_TASK_STK[0],//�����ջ����ַ
                 (CPU_STK_SIZE)CAN2_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)CAN2_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,								//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,									//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,									//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);							//��Ÿú�������ʱ�ķ���ֵ 
						
	//�������PWM��������	
	 OSTaskCreate((OS_TCB 	* )&SteerTaskTCB,					//������ƿ�
				 (CPU_CHAR	* )"steer_task", 							//��������
                 (OS_TASK_PTR )steer_task, 				//������
                 (void		* )0,									//���ݸ��������Ĳ���
                 (OS_PRIO	  )STEER_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&Steer_TASK_STK[0],//�����ջ����ַ
                 (CPU_STK_SIZE)STEER_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)STEER_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,								//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,									//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,									//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);							//��Ÿú�������ʱ�ķ���ֵ 
				 
	//�������������ݲ�������				 
	OSTaskCreate((OS_TCB 	* )&SampleTaskTCB,					//������ƿ�
				 (CPU_CHAR	* )"sample task", 							//��������
                 (OS_TASK_PTR )sample_task, 				//������
                 (void		* )0,											//���ݸ��������Ĳ���
                 (OS_PRIO	  )SAMPLE_TASK_PRIO,     	//�������ȼ�
                 (CPU_STK   * )&SAMPLE_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)SAMPLE_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)SAMPLE_STK_SIZE,			//�����ջ��С
                 (OS_MSG_QTY  )0,										//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,											//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,											//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);									//��Ÿú�������ʱ�ķ���ֵ

	//����led����
	OSTaskCreate((OS_TCB 	* )&LedTaskTCB,					//������ƿ�
				 (CPU_CHAR	* )"led task", 							//��������
                 (OS_TASK_PTR )led_task, 				//������
                 (void		* )0,									//���ݸ��������Ĳ���
                 (OS_PRIO	  )LED_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&LED_TASK_STK[0],//�����ջ����ַ
                 (CPU_STK_SIZE)LED_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)LED_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,								//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,									//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,									//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);							//��Ÿú�������ʱ�ķ���ֵ				 

	OS_CRITICAL_EXIT();	//�˳��ٽ���	 

}

