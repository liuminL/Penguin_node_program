#include "main.h"
unsigned int CAN_Time_Out2 = 0;
unsigned char can_tx_success_flag2=0;
extern struct Motor LEFT_Knee, LEFT_Hip, RIGHT_Hip, RIGHT_Knee;
static void CAN_BLDC_Delay_Us(unsigned int t)
{
	int i;
	for(i=0;i<t;i++)
	{
		int a=9;
		while(a--);
	}
}

void CAN_BLDC_ResetMod(unsigned char Number, unsigned char Mode)
{
		CanTxMsg tx_message;
		unsigned short can_id = 0x600;
    
    tx_message.IDE = CAN_ID_STD;    //标准帧
    tx_message.RTR = CAN_RTR_DATA;  //数据帧
    tx_message.DLC = 0x08;          //帧长度为8
    
		//4 BLDC
    if(Number<5 && Number>0)
    {
        can_id |= Number;
    }
    else
    {
        return;
    }
		CAN_BLDC_Control(Number, BLDC_ControlWord_AlarmClean);
		CAN_BLDC_Control(Number, BLDC_ControlWord_Off);
		
		tx_message.StdId = can_id;      //帧ID为传入参数的CAN_ID
    tx_message.Data[0] = 0x23;
    tx_message.Data[1] = 0x60;
    tx_message.Data[2] = 0x60;
    tx_message.Data[3] = 0x00;
    tx_message.Data[4] = Mode;
    tx_message.Data[5] = 0x00;
    tx_message.Data[6] = 0x00;
    tx_message.Data[7] = 0x00;
		
    can_tx_success_flag2 = 0;
    CAN_Transmit(CAN2,&tx_message);
}
/*
读取电机结构体的电机状态
0 断开
1 上电
2 报警
3 其它
*/
char get_CAN_BLDC_MOTOR_STATE(unsigned char Number)
{
	if(Number == 0x01)
		return LEFT_Hip.state;
	else if(Number == 0x02)
		return LEFT_Knee.state;
	else if(Number == 0x03)
		return RIGHT_Hip.state;
	else if(Number == 0x04)
		return RIGHT_Knee.state;
	else
		return 0x3;
	/*
	switch(Number)
	{
		case 0x01: 
		{
			return LEFT_Hip.state;
			break;
		}
		case 0x02: 
		{
			return LEFT_Knee.state;
			break;
		}
	  case 0x03: 
		{
			return RIGHT_Hip.state;
			break;
		}
		case 0x04: 
		{
			return RIGHT_Knee.state;
			break;
		}
		default : return 0x3;
	}	*/		
}
/*
设置电机结构体的电机状态
*/
void set_CAN_BLDC_MOTOR_STATE(unsigned char Number, char state)
{
	/*
	switch(Number)
	{
		case 0x01: 
		{
			LEFT_Hip.state = state;
			break;
		}
		case 0x02: 
		{
			LEFT_Knee.state = state;
			break;
		}
	  case 0x03: 
		{
			RIGHT_Hip.state = state;
			break;
		}
		case 0x04: 
		{
			RIGHT_Knee.state = state;
			break;
		}
		default : return;
	}		
	*/
		if(Number == 0x01)
			LEFT_Hip.state = state;
		else if(Number == 0x02)
			LEFT_Knee.state = state;
		else if(Number == 0x03)
			RIGHT_Hip.state = state;
		else if(Number == 0x04)
			RIGHT_Knee.state = state;
		else
			return ;
}
/*
对电机控制字进行设置
*/
void CAN_BLDC_Control(unsigned char Number, unsigned char ControlWord)
{
		CanTxMsg tx_message;
		unsigned short can_id = 0x600;
    
    tx_message.IDE = CAN_ID_STD;    //标准帧
    tx_message.RTR = CAN_RTR_DATA;  //数据帧
    tx_message.DLC = 0x08;          //帧长度为8
    
		//4 BLDC
    if(Number<5 && Number>0)
    {
        can_id |= Number;
    }
    else
    {
        return;
    }
		tx_message.StdId = can_id;      //帧ID为传入参数的CAN_ID
		
		tx_message.Data[0] = 0x23;
    tx_message.Data[1] = 0x40;
    tx_message.Data[2] = 0x60;
    tx_message.Data[3] = 0x00;
    tx_message.Data[4] = ControlWord;
    tx_message.Data[5] = 0x00;
    tx_message.Data[6] = 0x00;
    tx_message.Data[7] = 0x00;
		
		CAN_Transmit(CAN2,&tx_message);
		can_tx_success_flag2 = 0;
		CAN_Time_Out2 = 0;
		while(can_tx_success_flag2 == 0)
    {
				CAN_BLDC_Delay_Us(1);
        CAN_Time_Out2++;
        if(CAN_Time_Out2>100)
        {
						break;
        }
    }
		
		if(ControlWord == BLDC_ControlWord_Off)
			set_CAN_BLDC_MOTOR_STATE(Number, 0);
		else if ((ControlWord == BLDC_AbControlWord_On) || (ControlWord == BLDC_ReControlWord_On))
			set_CAN_BLDC_MOTOR_STATE(Number, 1);
		//else if ()
			
}
/*
读取驱动器状态
0x0 无有效读取，检查编号是否正确.注意初值为0
0x1 驱动器无有效报警
0x2 驱动器报警
*/
unsigned char return_state = 0x0;
unsigned char CAN_BLDC_state_get(unsigned char Number)
{
		CanTxMsg tx_message;
		unsigned short can_id = 0x600;
    tx_message.IDE = CAN_ID_STD;    //标准帧
    tx_message.RTR = CAN_RTR_DATA;  //数据帧
    tx_message.DLC = 0x08;          //帧长度为8
    
		//4 BLDC
    if(Number<5 && Number>0)
    {
        can_id |= Number;
    }
    else
    {
        return 0x0;
    }
		tx_message.StdId = can_id;                   //帧ID为传入参数的CAN_ID
		
		tx_message.Data[0] = 0x23;
    tx_message.Data[1] = 0x41;
    tx_message.Data[2] = 0x60;
    tx_message.Data[3] = 0x00;
    tx_message.Data[4] = 0x00;
    tx_message.Data[5] = 0x00;
    tx_message.Data[6] = 0x00;
    tx_message.Data[7] = 0x00;
		CAN_Transmit(CAN2,&tx_message);             //发出请求CAN帧之后，电机驱动器做出回复，CAN2的接收中断处理接收消息并且修改return_state的值，读取return_state即可
		CAN_BLDC_Delay_Us(10);
		return return_state;
}
void CAN_BLDC_AbPositionMod(unsigned char Number, long Temp_Position)
{
		CanTxMsg tx_message;
		unsigned short can_id = 0x600;
    
    tx_message.IDE = CAN_ID_STD;    //标准帧
    tx_message.RTR = CAN_RTR_DATA;  //数据帧
    tx_message.DLC = 0x08;          //帧长度为8
    
		//4 BLDC
    if(Number<5 && Number>0)
    {
        can_id |= Number;
    }
    else
    {
        return;
    }
		tx_message.StdId = can_id;                                                     //帧ID为传入参数的CAN_ID
		
		tx_message.Data[0] = 0x23;
    tx_message.Data[1] = 0x7A;
    tx_message.Data[2] = 0x60;
    tx_message.Data[3] = 0x00;
    tx_message.Data[4] = (unsigned char)(Temp_Position&0xff);
    tx_message.Data[5] = (unsigned char)((Temp_Position>>8)&0xff);
    tx_message.Data[6] = (unsigned char)((Temp_Position>>16)&0xff);
    tx_message.Data[7] = (unsigned char)((Temp_Position>>24)&0xff);
		
		can_tx_success_flag2 = 0;
    CAN_Transmit(CAN2,&tx_message);
    
		//CAN_BLDC_Control(Number, BLDC_ControlWord_AlarmClean);                         //clean Alarm
		CAN_BLDC_Control(Number, BLDC_AbControlWord_On);                               //Start
    CAN_Time_Out2 = 0;
    while(can_tx_success_flag2 == 0)
    {
				CAN_BLDC_Delay_Us(1);
        CAN_Time_Out2++;
        if(CAN_Time_Out2>100)
        {
						break;
        }
    }
}

void CAN_BLDC_RePositionMod(unsigned char Number, long Temp_Position)
{
		CanTxMsg tx_message;
		unsigned short can_id = 0x600;
    
    tx_message.IDE = CAN_ID_STD;    //标准帧
    tx_message.RTR = CAN_RTR_DATA;  //数据帧
    tx_message.DLC = 0x08;          //帧长度为8
    
		//4 BLDC
    if(Number<5 && Number>0)
    {
        can_id |= Number;
    }
    else
    {
        return;
    }
		tx_message.StdId = can_id;      //帧ID为传入参数的CAN_ID
		if(get_CAN_BLDC_MOTOR_STATE(Number) == 0x0)
			CAN_BLDC_Control(Number, BLDC_ReControlWord_On);    //Let the motor connect to power
		
		tx_message.Data[0] = 0x23;
    tx_message.Data[1] = 0x7B;
    tx_message.Data[2] = 0x60;
    tx_message.Data[3] = 0x00;
    tx_message.Data[4] = (unsigned char)(Temp_Position&0xff);
    tx_message.Data[5] = (unsigned char)((Temp_Position>>8)&0xff);
    tx_message.Data[6] = (unsigned char)((Temp_Position>>16)&0xff);
    tx_message.Data[7] = (unsigned char)((Temp_Position>>24)&0xff);
		
		can_tx_success_flag2 = 0;
    CAN_Transmit(CAN2,&tx_message);
    
    CAN_Time_Out2 = 0;
    while(can_tx_success_flag2 == 0)
    {
				CAN_BLDC_Delay_Us(1);
        CAN_Time_Out2++;
        if(CAN_Time_Out2>100)
        {
						break;
        }
    }
}