/**
  ******************************************************************************
  * @file    HARDWARE/CAN/can.c 
  * @author  Liao
  * @version V1
  * @date    2017.01.05
  * @brief   Sensors data upload。
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "qr_protocol.h"

CanRxMsg can1RxMsg;
uint8_t can1TxMsg[CAN1_PACKAGE_LENGTH],can1Buffer[16]={0};//2帧数据，即数组取16
uint32_t can1Id = 0;
int Flag_MotorChange = 0;             
short MAX_PWM = 5000;
short LEFT_VELOCITY, RIGHT_VELOCITY, Gravity_Center;

/*
struct Motor
{
	char state; 
	char set_state;
	long motor_position;
	long motor_acceleration;
	long motor_deceleration;
	long motor_Trapezoidal_speed;
	char init_flag;
};
*/
struct Motor LEFT_Knee={0, 0, 0, 0x14, 0x28, 0xf00a, 0}, LEFT_Hip={0, 0, 0, 0x14, 0x28, 0xf00a, 0}, RIGHT_Hip={0, 0, 0, 0x14, 0x28, 0xf00a, 0}, RIGHT_Knee={0, 0, 0, 0x14, 0x28, 0xf00a, 0};
//long LEFT_Knee = 0, LEFT_Hip = 0, RIGHT_Hip = 0, RIGHT_Knee = 0;
uint8_t feedback[8];
/*******************************************************************************
* Function Name  : can1Init
* Description    : The initialization of can1.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
u8 can1Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{ 
	uint16_t StdIdArray[2] ; //定义一组标准CAN ID  
	uint16_t      mask,num,tmp,i; 
	uint16_t maskId;							
	GPIO_InitTypeDef 		GPIO_InitStructure; 
	CAN_InitTypeDef        	CAN_InitStructure;
	CAN_FilterInitTypeDef  	CAN_FilterInitStructure;
	NVIC_InitTypeDef  		NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能PORTA时钟	                   											 

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//使能CAN1时钟	
	StdIdArray[0] = MII_MSG_FILL_2NODE_MSG(FILTER_ID,MII_MSG_DEBUG_1);
	StdIdArray[1] = MII_MSG_FILL_2NODE_MSG(FILTER_ID,MII_MSG_DEBUG_2);
//	StdIdArray[0] = MII_MSG_FILL_2NODE_MSG(FILTER_ID,MII_MSG_COMMON_DATA_1);
//	StdIdArray[1] = MII_MSG_FILL_2NODE_MSG(FILTER_ID,MII_MSG_MOTOR_CMD_2);
//	StdIdArray[2] = MII_MSG_FILL_2NODE_MSG(FILTER_ID,MII_MSG_COMMON_DATA_4);
//	StdIdArray[3] = MII_MSG_FILL_2NODE_MSG(FILTER_ID,MII_MSG_COMMON_DATA_5);
	
	mask =0x7ff;                      //下面开始计算屏蔽码  
	num =sizeof(StdIdArray)/sizeof(StdIdArray[0]);  
	for(i =0; i<num; i++)      //屏蔽码位StdIdArray[]数组中所有成员的同或结果  
	{  
		tmp =StdIdArray[i] ^ (~StdIdArray[0]);  //所有数组成员与第0个成员进行同或操作  
		mask &=tmp;  
	} 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽
	GPIO_Init(GPIOA, &GPIO_InitStructure);			//初始化IO

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);			//初始化IO

	//CAN单元设置
	CAN_InitStructure.CAN_TTCM=DISABLE;			//非时间触发通信模式  
	CAN_InitStructure.CAN_ABOM=DISABLE;			//软件自动离线管理	 
	CAN_InitStructure.CAN_AWUM=DISABLE;			//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
	CAN_InitStructure.CAN_NART=ENABLE;			//禁止报文自动传送 
	CAN_InitStructure.CAN_RFLM=DISABLE;		 	//报文不锁定,新的覆盖旧的  
	CAN_InitStructure.CAN_TXFP=DISABLE;			//优先级由报文标识符决定 
	CAN_InitStructure.CAN_Mode= mode;	      	//模式设置： mode:0,普通模式;1,回环模式; 
	//设置波特率
	CAN_InitStructure.CAN_SJW=tsjw;				//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1=tbs1; 			//Tbs1=tbs1+1个时间单位CAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2=tbs2;				//Tbs2=tbs2+1个时间单位CAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler=brp;        //分频系数(Fdiv)为brp+1	
	CAN_Init(CAN1, &CAN_InitStructure);        	//初始化CAN1 

	CAN_FilterInitStructure.CAN_FilterNumber=0;	//过滤器0
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	//屏蔽位模式
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; 	//32位宽 
	CAN_FilterInitStructure.CAN_FilterIdHigh=0;//(StdIdArray[0]<<5);     //验证码可以设置为StdIdArray[]数组中任意一个，这里使用StdIdArray[0]作为验证码
	CAN_FilterInitStructure.CAN_FilterIdLow=0;//|CAN_ID_STD;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0;//(mask<<5);  
	CAN_FilterInitStructure.CAN_FilterMaskIdLow =0;//|0x02;        //只接收数据帧  
//	CAN_FilterInitStructure.CAN_FilterIdHigh=(maskId<<5);     //验证码设置
//	CAN_FilterInitStructure.CAN_FilterIdLow=0|CAN_ID_STD;
//	CAN_FilterInitStructure.CAN_FilterMaskIdHigh =(maskId<<5);  
//	CAN_FilterInitStructure.CAN_FilterMaskIdLow =0|0x02;        //只接收数据帧  	
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;			//激活过滤器0

	CAN_FilterInit(&CAN_FilterInitStructure);			//滤波器初始化
 
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);				//FIFO0消息挂号中断允许.		    

	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // 主优先级为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	return 0;
}  

/*******************************************************************************
* Function Name  : can2ModeInit
* Description    : The initialization of can2.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
u8 can2Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{ 
	GPIO_InitTypeDef 		GPIO_InitStructure; 
	CAN_InitTypeDef        	CAN_InitStructure;
	CAN_FilterInitTypeDef  	CAN_FilterInitStructure;

	NVIC_InitTypeDef  		NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//使能PORTB时钟	                   											 

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1 | RCC_APB1Periph_CAN2, ENABLE);//使能CAN2时钟	

	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽
	GPIO_Init(GPIOB, &GPIO_InitStructure);			//初始化IO

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//上拉输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);			//初始化IO

	//CAN单元设置
	CAN_InitStructure.CAN_TTCM=DISABLE;				//非时间触发通信模式  
	CAN_InitStructure.CAN_ABOM=DISABLE;				//软件自动离线管理	 
	CAN_InitStructure.CAN_AWUM=DISABLE;				//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
	CAN_InitStructure.CAN_NART=ENABLE;				//禁止报文自动传送 
	CAN_InitStructure.CAN_RFLM=DISABLE;		 		//报文不锁定,新的覆盖旧的  
	CAN_InitStructure.CAN_TXFP=ENABLE;				//优先级由报文标识符决定 
	CAN_InitStructure.CAN_Mode= mode;	      		//模式设置： mode:0,普通模式;1,回环模式; 
	//设置波特率
	CAN_InitStructure.CAN_SJW=tsjw;					//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1=tbs1; 				//Tbs1=tbs1+1个时间单位CAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2=tbs2;					//Tbs2=tbs2+1个时间单位CAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler=brp;    		//分频系数(Fdiv)为brp+1	
	CAN_Init(CAN2, &CAN_InitStructure);     		//初始化CAN1 

	CAN_FilterInitStructure.CAN_FilterNumber=14;	//过滤器14
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 			//屏蔽位模式
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; 			//32位宽 
	CAN_FilterInitStructure.CAN_FilterIdHigh=0;				//32位ID
	CAN_FilterInitStructure.CAN_FilterIdLow=0;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0;					//32位MASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器14关联到FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;//激活过滤器0

	CAN_FilterInit(&CAN_FilterInitStructure);			//滤波器初始化
	
	CAN_ITConfig(CAN2,CAN_IT_FMP0,ENABLE);				//FIFO0消息挂号中断允许.		    

	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // 主优先级为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;            // 次优先级为1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	return 0;
}

/*******************************************************************************
* Function Name  : CAN1_RX0_IRQHandler
* Description    : The receive interrupt service function of can1.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/		    
void CAN1_RX0_IRQHandler(void)
{
	OS_ERR err,err1;
	CanRxMsg RxMessage;
	int32_t DATA[2];
	uint16_t nodeId = 0;
	u8 msgId = 0;
	u8 swId = 0;
	u16 spId = 0;
	CAN_Receive(CAN1, 0, &RxMessage);
	memcpy(DATA, RxMessage.Data, 8);
	nodeId = MII_MSG_SPLIT_EXNODEID(RxMessage.ExtId);
	msgId	 = MII_MSG_SPLIT_EXMSGID(RxMessage.ExtId);			//获取消息ID
	swId = MII_MSG_SPLIT_EXSWID(RxMessage.ExtId);
	spId = MII_MSG_SPLIT_EXSPID(RxMessage.ExtId);
	
	if(nodeId == FILTER_ID )
	{
		if(msgId == MII_MSG_COMMON_1)							
		{
			/*
			第一帧，0x1240000 断电  0x1250000 上电  0x1260000 清除报警
			*/
			Flag_MotorChange = 1;
			nodeStatus = 1;
			
			LEFT_Knee.set_state = swId;
			LEFT_Knee.motor_Trapezoidal_speed = spId;
			memcpy(&(LEFT_Knee.motor_acceleration), RxMessage.Data, 2);
			memcpy(&(LEFT_Knee.motor_deceleration), RxMessage.Data, 2);
			if(DATA[1] != 0x88888888)
				LEFT_Knee.motor_position = DATA[1];			
		}
		else if(msgId == MII_MSG_COMMON_2)        
		{
			/*
			第二帧，0x1280000 断电  0x1290000 上电  0x12a0000 清除报警
			*/
			Flag_MotorChange = 1;
			nodeStatus = 1;
			
			LEFT_Hip.set_state = swId;
			LEFT_Hip.motor_Trapezoidal_speed = spId;
			memcpy(&(LEFT_Hip.motor_acceleration), RxMessage.Data, 2);
			memcpy(&(LEFT_Hip.motor_deceleration), RxMessage.Data, 2);
			if(DATA[1] != 0x88888888)
				LEFT_Hip.motor_position = DATA[1];			
		}
		else if(msgId == MII_MSG_COMMON_3)				
		{
			/*
			第三帧，0x12c0000 断电  0x12d0000 上电  0x12e0000 清除报警
			*/
			Flag_MotorChange = 1;
			nodeStatus = 1;
			
			RIGHT_Knee.set_state = swId;
			RIGHT_Knee.motor_Trapezoidal_speed = spId;
			memcpy(&(RIGHT_Knee.motor_acceleration), RxMessage.Data, 2);
			memcpy(&(RIGHT_Knee.motor_deceleration), RxMessage.Data, 2);
			if(DATA[1] != 0x88888888)
				RIGHT_Knee.motor_position = DATA[1];	
		}
		else if(msgId == MII_MSG_COMMON_4)        
		{
			/*
			第四帧，0x1300000 断电  0x1310000 上电  0x1320000 清除报警
			*/
			Flag_MotorChange = 1;
			nodeStatus = 1;
			
			RIGHT_Hip.set_state = swId;
			RIGHT_Hip.motor_Trapezoidal_speed = spId;
			memcpy(&(RIGHT_Hip.motor_acceleration), RxMessage.Data, 2);
			memcpy(&(RIGHT_Hip.motor_deceleration), RxMessage.Data, 2);
			if(DATA[1] != 0x88888888)
				RIGHT_Hip.motor_position = DATA[1];	
		}
		else if(msgId == MII_MSG_COMMON_5)
		{
			/*
			第五帧，0x1340000
			*/
			nodeStatus = 1;
			memcpy(PWMVAL,RxMessage.Data,8);
			PWMVAL[4] = spId;
		}
		else if(msgId == MII_MSG_COMMON_6)        
		{
			nodeStatus = 1;
			
		}
		/*
		else if(msgId == MII_MSG_COMMON_5)        //第五帧：电机梯形、加减速度   0x4d
		{
			//Flag_MotorChange = 1;
			nodeStatus = 1;
			memcpy(&(LEFT_Knee.motor_acceleration), RxMessage.Data, 2);
			memcpy(&(LEFT_Knee.motor_deceleration), &RxMessage.Data[2], 2);
			memcpy(&(LEFT_Hip.motor_acceleration), &RxMessage.Data[4], 2);
			memcpy(&(LEFT_Hip.motor_deceleration), &RxMessage.Data[6], 2);
		}
		else if(msgId == MII_MSG_COMMON_6)        //第五帧：电机梯形、加减速度   0x4E
		{
			//Flag_MotorChange = 1;
			nodeStatus = 1;
			memcpy(&(RIGHT_Knee.motor_acceleration), RxMessage.Data, 2);
			memcpy(&(RIGHT_Knee.motor_deceleration), &RxMessage.Data[2], 2);
			memcpy(&(RIGHT_Hip.motor_acceleration), &RxMessage.Data[4], 2);
			memcpy(&(RIGHT_Hip.motor_deceleration), &RxMessage.Data[6], 2);
		}
		*/
		else
		{
			nodeStatus = 0;
		}

	}
	else
	{
		OSSemPend(&SYNC_SEM,0,OS_OPT_PEND_BLOCKING,0,&err); //请求信号量1
	}

}

/*******************************************************************************
* Function Name  : CAN2_RX0_IRQHandler
* Description    : The receive interrupt service function of can2.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int8_t getMotorSpeed[8] = {0},getMotorPosition[8] = {0};

void CAN2_RX0_IRQHandler(void)
{
	int32_t motorPosition = 0;
	CanRxMsg RxMessage2;
	CAN_Receive(CAN2, 0, &RxMessage2);
	
	//int16_t real_current = (RxMessage.Data[0]<<8)| RxMessage.Data[1];
	//int16_t real_velocity = (RxMessage.Data[2]<<8)| RxMessage.Data[3];
	//int32_t real_position = (RxMessage.Data[4]<<24) | (RxMessage.Data[5]<<16) | (RxMessage.Data[6]<<8)| RxMessage.Data[7];
	
	if(RxMessage2.StdId == 0x1B)
	{
		//memcpy(feedback, &(RxMessage2.Data[2]), 2);
		feedback[0] = RxMessage2.Data[3];
		feedback[1] = RxMessage2.Data[2];
		return;
	}
	else if(RxMessage2.StdId == 0x2B)
	{
		//memcpy(&(feedback[2]), &(RxMessage2.Data[2]), 2);
		feedback[2] = RxMessage2.Data[3];
		feedback[3] = RxMessage2.Data[2];
		return;
	}
	else if(RxMessage2.StdId == 0x3B)
	{
		//memcpy(&(feedback[4]), &(RxMessage2.Data[2]), 2);
		feedback[4] = RxMessage2.Data[3];
		feedback[5] = RxMessage2.Data[2];
		return;
	}
	else
		return;
}


/*******************************************************************************
* Function Name  : can1SendMsg
* Description    : The send service function of can1.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void can1SendMsg(uint8_t* msg,u8 msgId,u8 len)
{	
	u8 mbox;
	u16 i=0;
	CanTxMsg TxMessage;
	if(msgId==MII_MSG_HEARTBEAT_1)
	{
		TxMessage.StdId = MII_MSG_FILL_2HOST_MSG(FILTER_ID,msgId);		//调试代码标准标识符 
	}
	else
	{
		TxMessage.StdId = MII_MSG_FILL_2NODE_MSG(FILTER_ID,msgId);		//非调试代码标准标识符
	}

//	TxMessage.ExtId=0x12;						// 设置扩展标示符 
	TxMessage.IDE = CAN_Id_Standard; 	// 标准帧
	TxMessage.RTR = CAN_RTR_Data;			// 数据帧
	TxMessage.DLC = len;							// 要发送的数据长度
	for(i=0;i<len;i++)
		TxMessage.Data[i] = msg[i];			          
	mbox= CAN_Transmit(CAN1, &TxMessage);   
	while(CAN_TransmitStatus(CAN1, mbox) == CAN_TxStatus_Failed)
	memset(&TxMessage.Data,0,sizeof(TxMessage.Data));		//清空数据缓存区
//	i = 0; 
//	while((CAN_TransmitStatus(CAN1, mbox) == CAN_TxStatus_Failed)&&(i<0XFFF))
//		i++;	//等待发送结束
// 	for(i=0;i<len;i++)
//	TxMessage.Data[i] = 0;
}

/*******************************************************************************
* Function Name  : can2SendMsg
* Description    : The send service function of can2.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void can2SendMsg(uint8_t driver_id,uint8_t commamd,int32_t absolut_position,uint8_t length)
{	
	u8 mbox;
	u16 i=0;
	CanTxMsg TxMessage;
	
	TxMessage.StdId = driver_id;					// 标准标识符 
//	TxMessage.ExtId=0x02;							// 设置扩展标示符 
	TxMessage.IDE = CAN_Id_Standard; 				//标准帧
	TxMessage.RTR = CAN_RTR_Data;					// 数据帧
	TxMessage.DLC = length;							// 要发送的数据长度
	
	TxMessage.Data[i] = length;
	TxMessage.Data[i+1] = driver_id;
	TxMessage.Data[i+2] = commamd;
	TxMessage.Data[i+3] = 0x00;
	
	if(length == 8)
	{
		TxMessage.Data[i+4] = absolut_position&0xff;
		TxMessage.Data[i+5] = (absolut_position>>8)&0xff;
		TxMessage.Data[i+6] = (absolut_position>>16)&0xff;
		TxMessage.Data[i+7] = (absolut_position>>24)&0xff;
	}	
	     
	mbox = CAN_Transmit(CAN2, &TxMessage);   
	while(CAN_TransmitStatus(CAN2, mbox) == CAN_TxStatus_Failed);

//	i = 0; 
//	while((CAN_TransmitStatus(CAN2, mbox) == CAN_TxStatus_Failed)&&(i<0XFFF))
//		i++;	//等待发送结束
	 
}

//CAN1任务函数
void can1_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		switch(nodeStatus)
		{
			case 0:							//节点板申请注册状态
				can1SendMsg(can1Buffer,MII_MSG_HEARTBEAT_1,8);//第一帧
				can1SendMsg(can1Buffer+8,MII_MSG_HEARTBEAT_2,8);//第二帧
				break;
			case 1:							//节点板正常发送传感器数据状态
				can1SendMsg(can1Buffer,MII_MSG_HEARTBEAT_1,8);
				can1SendMsg(can1Buffer+8,MII_MSG_HEARTBEAT_2,8);
				break;
			case 2:							//节点板传感器数据异常状态
				can1SendMsg(can1Buffer,MII_MSG_HEARTBEAT_1,8);
				can1SendMsg(can1Buffer+8,MII_MSG_HEARTBEAT_2,8);
				break;
			default:break;
		}

		memset(can1Buffer,0x00,sizeof(can1Buffer));						//清空can缓冲区	
//		CAN_RoboModule_DRV_OpenLoop_Mode(0,1,100);
		OSTimeDlyHMSM(0,0,0,4,OS_OPT_TIME_HMSM_STRICT,&err); //延时,发起任务调度
	}
}


//CAN2任务函数
void can2_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	
	while(1)
	{
		if(Flag_MotorChange == 1)
		{
			CAN_BLDC_RePositionMod(1, LEFT_Hip.motor_position, LEFT_Hip.motor_acceleration, LEFT_Hip.motor_deceleration, LEFT_Hip.set_state, LEFT_Hip.motor_Trapezoidal_speed);
			CAN_BLDC_RePositionMod(2, LEFT_Knee.motor_position, LEFT_Knee.motor_acceleration, LEFT_Knee.motor_deceleration, LEFT_Knee.set_state, LEFT_Knee.motor_Trapezoidal_speed);
			CAN_BLDC_RePositionMod(3, RIGHT_Hip.motor_position, RIGHT_Hip.motor_acceleration, RIGHT_Hip.motor_deceleration, RIGHT_Hip.set_state, RIGHT_Hip.motor_Trapezoidal_speed);
			CAN_BLDC_RePositionMod(4, RIGHT_Knee.motor_position, RIGHT_Knee.motor_acceleration, RIGHT_Knee.motor_deceleration, RIGHT_Knee.set_state, RIGHT_Knee.motor_Trapezoidal_speed);			
			Flag_MotorChange = 0;
		}
		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err); //延时,发起任务调度
	}
	
}

