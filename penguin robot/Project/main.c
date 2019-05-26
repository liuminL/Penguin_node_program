#include "main.h"

//开始任务配置
#define START_TASK_PRIO		1							//任务优先级
#define START_STK_SIZE 		1024					//任务堆栈大小
OS_TCB StartTaskTCB;										//任务控制块
CPU_STK START_TASK_STK[START_STK_SIZE];	//任务堆栈
void start_task(void *p_arg);						//任务函数

//采样任务配置
#define SAMPLE_TASK_PRIO		2							//任务优先级
#define SAMPLE_STK_SIZE 		128						//任务堆栈大小
OS_TCB SampleTaskTCB;											//任务控制块
CPU_STK SAMPLE_TASK_STK[SAMPLE_STK_SIZE];	//任务堆栈
void sample_task(void *p_arg);						//任务函数

//can1发送任务配置
#define CAN1_TASK_PRIO		3							//任务优先级
#define CAN1_STK_SIZE 		128						//任务堆栈大小
OS_TCB Can1TaskTCB;										//任务控制块
CPU_STK CAN1_TASK_STK[CAN1_STK_SIZE];		//任务堆栈
void can1_task(void *p_arg);						//任务函数


//can2发送任务配置
#define CAN2_TASK_PRIO		4							//任务优先级
#define CAN2_STK_SIZE 		128						//任务堆栈大小
OS_TCB Can2TaskTCB;										//任务控制块
CPU_STK CAN2_TASK_STK[CAN2_STK_SIZE];		//任务堆栈
void can2_task(void *p_arg);						//任务函数

//PWM生成控制任务配置
#define STEER_TASK_PRIO     5
#define STEER_STK_SIZE     128
OS_TCB SteerTaskTCB;
CPU_STK Steer_TASK_STK[STEER_STK_SIZE];
void steer_task(void *p_arg);


#ifdef DEBUG_EN
//debug任务配置
#define DEBUG_TASK_PRIO		9							//任务优先级
#define DEBUG_STK_SIZE 		128						//任务堆栈大小
OS_TCB DebugTaskTCB;										//任务控制块
CPU_STK DEBUG_TASK_STK[DEBUG_STK_SIZE];	//任务堆栈
void debug_task(void *p_arg);						//任务函数
#endif

//LED显示任务配置
#define LED_TASK_PRIO		10					//任务优先级
#define LED_STK_SIZE 		128					//任务堆栈大小
OS_TCB LedTaskTCB;									//任务控制块
CPU_STK LED_TASK_STK[LED_STK_SIZE];	//任务堆栈
void led_task(void *p_arg);					//任务函数


uint32_t dataSize = 0;
char nodeStatus = 0;		//节点板注册标志位
sensorStruct sensor;
OS_SEM	SYNC_SEM,SYNC_SEM1;		//定义一个信号量1，用于任务同步

#ifdef DEBUG_EN
uint16_t countBuffer[DATA_CNT];
#endif

u16 arr1 = 9999,arr2 = 9999,psc1 = 143,psc2 = 143;

int main(void)
{	
	OS_ERR err;
	CPU_SR_ALLOC();
	delay_init();	    	 	//延时函数初始化
	SystemInit();					//系统函数初始化
	sampleInit();					//传感器数据采集函数初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	can1Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,2,CAN_Mode_Normal);		//CAN1初始化正常模式,波特率1000kbps
	can2Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,2,CAN_Mode_Normal);		//CAN2初始化正常模式,波特率1000kbps 
	LED_Init();	
	TIM5_Int_Init(9,7199);//10Khz的计数频率，计数到10为1ms  
	/*
	delay_ms(500);                                      //刚开始要有足够的延时，确保驱动器已经初始化完成 
	CAN_RoboModule_DRV_Reset(0,0);                      //对0组所有驱动器进行复位 
	delay_ms(500);                                      //发送复位指令后的延时必须要有，等待驱动器再次初始化完成
	CAN_RoboModule_DRV_Mode_Choice(0,0,Velocity_Position_Mode);  //0组的所有驱动器 都进入速度位置模式
	delay_ms(500);                                      //发送模式选择指令后，要等待驱动器进入模式就绪。所以延时也不可以去掉。
	
	CAN_RoboModule_DRV_Config(0,1,0x0A, 0);			//开启对外发送速度、电流、位置，不开限位
	delay_ms(500);
	CAN_RoboModule_DRV_Config(0,2,0x0A, 0);			//开启对外发送速度、电流、位置，不开限位
	delay_ms(500);
	CAN_RoboModule_DRV_Config(0,3,0x0A, 0);			//开启对外发送速度、电流、位置，不开限位
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
	TIM3_PWM_Init(arr1,psc1);//pwm频率：72000000/(psc+1)=0.5M,  0.5M/(arr+1)=50HZ
	TIM1_PWM_Init(arr1,psc1);
	
	OSInit(&err);				//初始化UCOSIII
	OS_CRITICAL_ENTER();//进入临界区
	//创建开始任务
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,					//任务控制块
				 (CPU_CHAR	* )"start task", 							//任务名字
                 (OS_TASK_PTR )start_task, 				//任务函数
                 (void		* )0,										//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,									//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,										//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,										//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);								//存放该函数错误时的返回值
	OS_CRITICAL_EXIT();	//退出临界区	 
	OSStart(&err);  //开启UCOSIII
	while(1);
}

//start任务函数
void start_task(void *p_arg)
{
	OS_ERR err,err1;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif	
	OS_CRITICAL_ENTER();//进入临界区
	//创建信号量1
	OSSemCreate ((OS_SEM*	)&SYNC_SEM,
                 (CPU_CHAR*	)"SYNC_SEM",
                 (OS_SEM_CTR)0,		
                 (OS_ERR*	)&err);					 
	//创建信号量2
	OSSemCreate ((OS_SEM*	)&SYNC_SEM1,
                 (CPU_CHAR*	)"SYNC_SEM1",
                 (OS_SEM_CTR)0,		
                 (OS_ERR*	)&err1);	
//调试代码
#ifdef DEBUG_EN
//创建debug任务
	OSTaskCreate((OS_TCB 	* )&DebugTaskTCB,					//任务控制块
				 (CPU_CHAR	* )"debug task", 							//任务名字
                 (OS_TASK_PTR )debug_task, 				//任务函数
                 (void		* )0,										//传递给任务函数的参数
                 (OS_PRIO	  )DEBUG_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&DEBUG_TASK_STK[0],//任务堆栈基地址
                 (CPU_STK_SIZE)DEBUG_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)DEBUG_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,									//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,										//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,										//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);								//存放该函数错误时的返回值
		 
#endif				 
//非调试代码
	//创建can1发送任务	
	 OSTaskCreate((OS_TCB 	* )&Can1TaskTCB,					//任务控制块
				 (CPU_CHAR	* )"can1 task", 							//任务名字
                 (OS_TASK_PTR )can1_task, 				//任务函数
                 (void		* )0,									//传递给任务函数的参数
                 (OS_PRIO	  )CAN1_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&CAN1_TASK_STK[0],//任务堆栈基地址
                 (CPU_STK_SIZE)CAN1_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)CAN1_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,								//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,									//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,									//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);							//存放该函数错误时的返回值 	

	//创建can2发送任务	
	 OSTaskCreate((OS_TCB 	* )&Can2TaskTCB,					//任务控制块
				 (CPU_CHAR	* )"can2 task", 							//任务名字
                 (OS_TASK_PTR )can2_task, 				//任务函数
                 (void		* )0,									//传递给任务函数的参数
                 (OS_PRIO	  )CAN2_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&CAN2_TASK_STK[0],//任务堆栈基地址
                 (CPU_STK_SIZE)CAN2_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)CAN2_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,								//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,									//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,									//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);							//存放该函数错误时的返回值 
						
	//创建翅膀PWM发送任务	
	 OSTaskCreate((OS_TCB 	* )&SteerTaskTCB,					//任务控制块
				 (CPU_CHAR	* )"steer_task", 							//任务名字
                 (OS_TASK_PTR )steer_task, 				//任务函数
                 (void		* )0,									//传递给任务函数的参数
                 (OS_PRIO	  )STEER_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&Steer_TASK_STK[0],//任务堆栈基地址
                 (CPU_STK_SIZE)STEER_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)STEER_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,								//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,									//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,									//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);							//存放该函数错误时的返回值 
				 
	//创建传感器数据采样任务				 
	OSTaskCreate((OS_TCB 	* )&SampleTaskTCB,					//任务控制块
				 (CPU_CHAR	* )"sample task", 							//任务名字
                 (OS_TASK_PTR )sample_task, 				//任务函数
                 (void		* )0,											//传递给任务函数的参数
                 (OS_PRIO	  )SAMPLE_TASK_PRIO,     	//任务优先级
                 (CPU_STK   * )&SAMPLE_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)SAMPLE_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)SAMPLE_STK_SIZE,			//任务堆栈大小
                 (OS_MSG_QTY  )0,										//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,											//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,											//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);									//存放该函数错误时的返回值

	//创建led任务
	OSTaskCreate((OS_TCB 	* )&LedTaskTCB,					//任务控制块
				 (CPU_CHAR	* )"led task", 							//任务名字
                 (OS_TASK_PTR )led_task, 				//任务函数
                 (void		* )0,									//传递给任务函数的参数
                 (OS_PRIO	  )LED_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&LED_TASK_STK[0],//任务堆栈基地址
                 (CPU_STK_SIZE)LED_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)LED_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,								//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,									//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,									//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);							//存放该函数错误时的返回值				 

	OS_CRITICAL_EXIT();	//退出临界区	 

}

