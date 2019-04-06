/**
  ******************************************************************************
  * @file    HARDWARE/CAN/can.c 
  * @author  Liao
  * @version V1
  * @date    2017.01.05
  * @brief   Sensors data upload��
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
uint8_t can1TxMsg[CAN1_PACKAGE_LENGTH],can1Buffer[16]={0};//2֡���ݣ�������ȡ16
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
	uint16_t StdIdArray[2] ; //����һ���׼CAN ID  
	uint16_t      mask,num,tmp,i; 
	uint16_t maskId;							
	GPIO_InitTypeDef 		GPIO_InitStructure; 
	CAN_InitTypeDef        	CAN_InitStructure;
	CAN_FilterInitTypeDef  	CAN_FilterInitStructure;
	NVIC_InitTypeDef  		NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��PORTAʱ��	                   											 

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//ʹ��CAN1ʱ��	
	StdIdArray[0] = MII_MSG_FILL_2NODE_MSG(FILTER_ID,MII_MSG_DEBUG_1);
	StdIdArray[1] = MII_MSG_FILL_2NODE_MSG(FILTER_ID,MII_MSG_DEBUG_2);
//	StdIdArray[0] = MII_MSG_FILL_2NODE_MSG(FILTER_ID,MII_MSG_COMMON_DATA_1);
//	StdIdArray[1] = MII_MSG_FILL_2NODE_MSG(FILTER_ID,MII_MSG_MOTOR_CMD_2);
//	StdIdArray[2] = MII_MSG_FILL_2NODE_MSG(FILTER_ID,MII_MSG_COMMON_DATA_4);
//	StdIdArray[3] = MII_MSG_FILL_2NODE_MSG(FILTER_ID,MII_MSG_COMMON_DATA_5);
	
	mask =0x7ff;                      //���濪ʼ����������  
	num =sizeof(StdIdArray)/sizeof(StdIdArray[0]);  
	for(i =0; i<num; i++)      //������λStdIdArray[]���������г�Ա��ͬ����  
	{  
		tmp =StdIdArray[i] ^ (~StdIdArray[0]);  //���������Ա���0����Ա����ͬ�����  
		mask &=tmp;  
	} 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);			//��ʼ��IO

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);			//��ʼ��IO

	//CAN��Ԫ����
	CAN_InitStructure.CAN_TTCM=DISABLE;			//��ʱ�䴥��ͨ��ģʽ  
	CAN_InitStructure.CAN_ABOM=DISABLE;			//����Զ����߹���	 
	CAN_InitStructure.CAN_AWUM=DISABLE;			//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
	CAN_InitStructure.CAN_NART=ENABLE;			//��ֹ�����Զ����� 
	CAN_InitStructure.CAN_RFLM=DISABLE;		 	//���Ĳ�����,�µĸ��Ǿɵ�  
	CAN_InitStructure.CAN_TXFP=DISABLE;			//���ȼ��ɱ��ı�ʶ������ 
	CAN_InitStructure.CAN_Mode= mode;	      	//ģʽ���ã� mode:0,��ͨģʽ;1,�ػ�ģʽ; 
	//���ò�����
	CAN_InitStructure.CAN_SJW=tsjw;				//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1=tbs1; 			//Tbs1=tbs1+1��ʱ�䵥λCAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2=tbs2;				//Tbs2=tbs2+1��ʱ�䵥λCAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler=brp;        //��Ƶϵ��(Fdiv)Ϊbrp+1	
	CAN_Init(CAN1, &CAN_InitStructure);        	//��ʼ��CAN1 

	CAN_FilterInitStructure.CAN_FilterNumber=0;	//������0
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	//����λģʽ
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; 	//32λ�� 
	CAN_FilterInitStructure.CAN_FilterIdHigh=0;//(StdIdArray[0]<<5);     //��֤���������ΪStdIdArray[]����������һ��������ʹ��StdIdArray[0]��Ϊ��֤��
	CAN_FilterInitStructure.CAN_FilterIdLow=0;//|CAN_ID_STD;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0;//(mask<<5);  
	CAN_FilterInitStructure.CAN_FilterMaskIdLow =0;//|0x02;        //ֻ��������֡  
//	CAN_FilterInitStructure.CAN_FilterIdHigh=(maskId<<5);     //��֤������
//	CAN_FilterInitStructure.CAN_FilterIdLow=0|CAN_ID_STD;
//	CAN_FilterInitStructure.CAN_FilterMaskIdHigh =(maskId<<5);  
//	CAN_FilterInitStructure.CAN_FilterMaskIdLow =0|0x02;        //ֻ��������֡  	
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//������0������FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;			//���������0

	CAN_FilterInit(&CAN_FilterInitStructure);			//�˲�����ʼ��
 
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);				//FIFO0��Ϣ�Һ��ж�����.		    

	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // �����ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // �����ȼ�Ϊ0
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

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//ʹ��PORTBʱ��	                   											 

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1 | RCC_APB1Periph_CAN2, ENABLE);//ʹ��CAN2ʱ��	

	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);			//��ʼ��IO

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);			//��ʼ��IO

	//CAN��Ԫ����
	CAN_InitStructure.CAN_TTCM=DISABLE;				//��ʱ�䴥��ͨ��ģʽ  
	CAN_InitStructure.CAN_ABOM=DISABLE;				//����Զ����߹���	 
	CAN_InitStructure.CAN_AWUM=DISABLE;				//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
	CAN_InitStructure.CAN_NART=ENABLE;				//��ֹ�����Զ����� 
	CAN_InitStructure.CAN_RFLM=DISABLE;		 		//���Ĳ�����,�µĸ��Ǿɵ�  
	CAN_InitStructure.CAN_TXFP=ENABLE;				//���ȼ��ɱ��ı�ʶ������ 
	CAN_InitStructure.CAN_Mode= mode;	      		//ģʽ���ã� mode:0,��ͨģʽ;1,�ػ�ģʽ; 
	//���ò�����
	CAN_InitStructure.CAN_SJW=tsjw;					//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1=tbs1; 				//Tbs1=tbs1+1��ʱ�䵥λCAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2=tbs2;					//Tbs2=tbs2+1��ʱ�䵥λCAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler=brp;    		//��Ƶϵ��(Fdiv)Ϊbrp+1	
	CAN_Init(CAN2, &CAN_InitStructure);     		//��ʼ��CAN1 

	CAN_FilterInitStructure.CAN_FilterNumber=14;	//������14
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 			//����λģʽ
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; 			//32λ�� 
	CAN_FilterInitStructure.CAN_FilterIdHigh=0;				//32λID
	CAN_FilterInitStructure.CAN_FilterIdLow=0;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0;					//32λMASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//������14������FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;//���������0

	CAN_FilterInit(&CAN_FilterInitStructure);			//�˲�����ʼ��
	
	CAN_ITConfig(CAN2,CAN_IT_FMP0,ENABLE);				//FIFO0��Ϣ�Һ��ж�����.		    

	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // �����ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;            // �����ȼ�Ϊ1
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
	msgId	 = MII_MSG_SPLIT_EXMSGID(RxMessage.ExtId);			//��ȡ��ϢID
	swId = MII_MSG_SPLIT_EXSWID(RxMessage.ExtId);
	spId = MII_MSG_SPLIT_EXSPID(RxMessage.ExtId);
	
	if(nodeId == FILTER_ID )
	{
		if(msgId == MII_MSG_COMMON_1)							
		{
			/*
			��һ֡��0x1240000 �ϵ�  0x1250000 �ϵ�  0x1260000 �������
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
			�ڶ�֡��0x1280000 �ϵ�  0x1290000 �ϵ�  0x12a0000 �������
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
			����֡��0x12c0000 �ϵ�  0x12d0000 �ϵ�  0x12e0000 �������
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
			����֡��0x1300000 �ϵ�  0x1310000 �ϵ�  0x1320000 �������
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
			����֡��0x1340000
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
		else if(msgId == MII_MSG_COMMON_5)        //����֡��������Ρ��Ӽ��ٶ�   0x4d
		{
			//Flag_MotorChange = 1;
			nodeStatus = 1;
			memcpy(&(LEFT_Knee.motor_acceleration), RxMessage.Data, 2);
			memcpy(&(LEFT_Knee.motor_deceleration), &RxMessage.Data[2], 2);
			memcpy(&(LEFT_Hip.motor_acceleration), &RxMessage.Data[4], 2);
			memcpy(&(LEFT_Hip.motor_deceleration), &RxMessage.Data[6], 2);
		}
		else if(msgId == MII_MSG_COMMON_6)        //����֡��������Ρ��Ӽ��ٶ�   0x4E
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
		OSSemPend(&SYNC_SEM,0,OS_OPT_PEND_BLOCKING,0,&err); //�����ź���1
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
		TxMessage.StdId = MII_MSG_FILL_2HOST_MSG(FILTER_ID,msgId);		//���Դ����׼��ʶ�� 
	}
	else
	{
		TxMessage.StdId = MII_MSG_FILL_2NODE_MSG(FILTER_ID,msgId);		//�ǵ��Դ����׼��ʶ��
	}

//	TxMessage.ExtId=0x12;						// ������չ��ʾ�� 
	TxMessage.IDE = CAN_Id_Standard; 	// ��׼֡
	TxMessage.RTR = CAN_RTR_Data;			// ����֡
	TxMessage.DLC = len;							// Ҫ���͵����ݳ���
	for(i=0;i<len;i++)
		TxMessage.Data[i] = msg[i];			          
	mbox= CAN_Transmit(CAN1, &TxMessage);   
	while(CAN_TransmitStatus(CAN1, mbox) == CAN_TxStatus_Failed)
	memset(&TxMessage.Data,0,sizeof(TxMessage.Data));		//������ݻ�����
//	i = 0; 
//	while((CAN_TransmitStatus(CAN1, mbox) == CAN_TxStatus_Failed)&&(i<0XFFF))
//		i++;	//�ȴ����ͽ���
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
	
	TxMessage.StdId = driver_id;					// ��׼��ʶ�� 
//	TxMessage.ExtId=0x02;							// ������չ��ʾ�� 
	TxMessage.IDE = CAN_Id_Standard; 				//��׼֡
	TxMessage.RTR = CAN_RTR_Data;					// ����֡
	TxMessage.DLC = length;							// Ҫ���͵����ݳ���
	
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
//		i++;	//�ȴ����ͽ���
	 
}

//CAN1������
void can1_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		switch(nodeStatus)
		{
			case 0:							//�ڵ������ע��״̬
				can1SendMsg(can1Buffer,MII_MSG_HEARTBEAT_1,8);//��һ֡
				can1SendMsg(can1Buffer+8,MII_MSG_HEARTBEAT_2,8);//�ڶ�֡
				break;
			case 1:							//�ڵ���������ʹ���������״̬
				can1SendMsg(can1Buffer,MII_MSG_HEARTBEAT_1,8);
				can1SendMsg(can1Buffer+8,MII_MSG_HEARTBEAT_2,8);
				break;
			case 2:							//�ڵ�崫���������쳣״̬
				can1SendMsg(can1Buffer,MII_MSG_HEARTBEAT_1,8);
				can1SendMsg(can1Buffer+8,MII_MSG_HEARTBEAT_2,8);
				break;
			default:break;
		}

		memset(can1Buffer,0x00,sizeof(can1Buffer));						//���can������	
//		CAN_RoboModule_DRV_OpenLoop_Mode(0,1,100);
		OSTimeDlyHMSM(0,0,0,4,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ,�����������
	}
}


//CAN2������
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
		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ,�����������
	}
	
}

