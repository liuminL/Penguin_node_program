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
/*
	模式设置
*/
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
/*******************************************************************************
* Function Name  : CAN_BLDC
* Description    : Driver communication
* Number         : Driver number 
* RW             : 0 is Read, 1 is Write
* obj            : obj code   lenth is 24, see the doc of driver
* setValue       : max 32
* returnDate     : when is reading
*******************************************************************************/		
void CAN_BLDC(unsigned char Number, int RW, long obj, long setValue, char* returnDate)
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
		
		if(RW == 0)                             //read
		{
			tx_message.Data[0] = 0x40;
		}
		else if(RW == 1)                        //write
		{
			tx_message.Data[0] = 0x23;
		}
		else
			return;
		
    tx_message.Data[1] = (unsigned char)((obj>>8)&0xff);          //取obj9-16位
    tx_message.Data[2] = (unsigned char)((obj>>16)&0xff);         //17-24
    tx_message.Data[3] = (unsigned char)(obj&0xff);     					//1-8
    tx_message.Data[4] = (unsigned char)(setValue&0xff);        	//1-8
    tx_message.Data[5] = (unsigned char)((setValue>>8)&0xff);   	//9-16
    tx_message.Data[6] = (unsigned char)((setValue>>16)&0xff);  	//17-24
    tx_message.Data[7] = (unsigned char)((setValue>>24)&0xff);  	//25-32
		
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
/*
驱动器加速度设置
*/
void CAN_BLDC_accelerationSet(unsigned char Number, long acceleration)
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
    tx_message.Data[1] = 0xF0;
    tx_message.Data[2] = 0x2F;
    tx_message.Data[3] = 0x06;
    tx_message.Data[4] = (unsigned char)(acceleration&0xff);
    tx_message.Data[5] = (unsigned char)((acceleration>>8)&0xff);
    tx_message.Data[6] = (unsigned char)((acceleration>>16)&0xff);
    tx_message.Data[7] = (unsigned char)((acceleration>>24)&0xff);
		
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
/*
驱动器减速度设置
*/
void CAN_BLDC_decelerationSet(unsigned char Number, long deceleration)
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
    tx_message.Data[1] = 0xF0;
    tx_message.Data[2] = 0x2F;
    tx_message.Data[3] = 0x07;
    tx_message.Data[4] = (unsigned char)(deceleration&0xff);
    tx_message.Data[5] = (unsigned char)((deceleration>>8)&0xff);
    tx_message.Data[6] = (unsigned char)((deceleration>>16)&0xff);
    tx_message.Data[7] = (unsigned char)((deceleration>>24)&0xff);
		
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
/*
梯形速度rpm设置
*/
void CAN_BLDC_Trapezoidal_speed(unsigned char Number, long speed)
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
    tx_message.Data[1] = 0x82;
    tx_message.Data[2] = 0x60;
    tx_message.Data[3] = 0x00;
    tx_message.Data[4] = (unsigned char)(speed&0xff);
    tx_message.Data[5] = (unsigned char)((speed>>8)&0xff);
    tx_message.Data[6] = (unsigned char)((speed>>16)&0xff);
    tx_message.Data[7] = (unsigned char)((speed>>24)&0xff);
		
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

/*
急停
*/
void CAN_BLDC_Stop(unsigned char Number, long flag)
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
    tx_message.Data[1] = 0x82;
    tx_message.Data[2] = 0x60;
    tx_message.Data[3] = 0x00;
    tx_message.Data[4] = (unsigned char)(flag&0xff);
    tx_message.Data[5] = (unsigned char)((flag>>8)&0xff);
    tx_message.Data[6] = (unsigned char)((flag>>16)&0xff);
    tx_message.Data[7] = (unsigned char)((flag>>24)&0xff);
		
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
/*
绝对位置模式下的控制
*/
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
/*
相对位置下的控制
*/
void CAN_BLDC_RePositionMod(unsigned char Number, long Temp_Position, long acceleration, long deceleration, unsigned char set_state, long speed)
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
		switch(Number)
		{
			case 0x1:
			{
					if(LEFT_Hip.init_flag == 0)
					{
						CAN_BLDC_accelerationSet(Number, acceleration);
						CAN_BLDC_decelerationSet(Number, deceleration);
						CAN_BLDC_ResetMod(Number, BLDC_Position_Mode);
						LEFT_Hip.init_flag = 1;
					}
					break;
			}
			case 0x2:
			{
					if(LEFT_Knee.init_flag == 0)
					{
						CAN_BLDC_accelerationSet(Number, acceleration);
						CAN_BLDC_decelerationSet(Number, deceleration);
						CAN_BLDC_ResetMod(Number, BLDC_Position_Mode);
						LEFT_Knee.init_flag = 1;
					}
					break;
			}
			case 0x3:
			{
					if(RIGHT_Hip.init_flag == 0)
					{
						CAN_BLDC_accelerationSet(Number, acceleration);
						CAN_BLDC_decelerationSet(Number, deceleration);
						CAN_BLDC_ResetMod(Number, BLDC_Position_Mode);
						RIGHT_Hip.init_flag = 1;
					}
					break;
			}
			case 0x4:
			{
					if(RIGHT_Knee.init_flag == 0)
					{
						CAN_BLDC_accelerationSet(Number, acceleration);
						CAN_BLDC_decelerationSet(Number, deceleration);
						CAN_BLDC_ResetMod(Number, BLDC_Position_Mode);
						RIGHT_Knee.init_flag = 1;
					}
					break;
			}
			default: return;
		}
		//if()
		CAN_BLDC_Trapezoidal_speed(Number, speed);
		
		if(set_state == 0x0)
		{
			CAN_BLDC_Control(Number, BLDC_ControlWord_Off);
			return;
		}		
		else if (set_state == 0x1)
			CAN_BLDC_Control(Number, BLDC_ReControlWord_On);
		else if (set_state == 0x2)
			CAN_BLDC_Control(Number, BLDC_ControlWord_AlarmClean);    //Let the motor connect to power
		else 
			return;
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

/*the init of VelocityMod*/
int init_VelocityMod(unsigned char Number, long acceleration, long deceleration)
{
		switch(Number)
		{
			case 0x1:
			{
					if(LEFT_Hip.init_flag == 0)
					{
						CAN_BLDC_accelerationSet(Number, acceleration);           //set acceleration
						CAN_BLDC_decelerationSet(Number, deceleration);           //set deceleration
						CAN_BLDC_ResetMod(Number, BLDC_Velocity1_Mode);           //set Velocity-Mode
						if(LEFT_Hip.set_state == 0x0)
						{
							CAN_BLDC_Control(Number, BLDC_ControlWord_Off);
						}		
						else if (LEFT_Hip.set_state == 0x1)
						{	
							CAN_BLDC_Control(Number, BLDC_ReControlWord_On);
						}
						else if (LEFT_Hip.set_state == 0x2)
						{	
							CAN_BLDC_Control(Number, BLDC_ControlWord_AlarmClean);    //Let the motor connect to power
						}
						else 
							return 0;	
						
						LEFT_Hip.init_flag = 1;//初始化完成标志
					}
					break;
			}
			case 0x2:
			{
					if(LEFT_Knee.init_flag == 0)
					{
						CAN_BLDC_accelerationSet(Number, acceleration);           //set acceleration
						CAN_BLDC_decelerationSet(Number, deceleration);           //set deceleration
						CAN_BLDC_ResetMod(Number, BLDC_Velocity1_Mode);           //set Velocity-Mode
						if(LEFT_Knee.set_state == 0x0)
						{
							CAN_BLDC_Control(Number, BLDC_ControlWord_Off);
						}		
						else if (LEFT_Knee.set_state == 0x1)
						{	
							CAN_BLDC_Control(Number, BLDC_ReControlWord_On);
						}
						else if (LEFT_Knee.set_state == 0x2)
						{	
							CAN_BLDC_Control(Number, BLDC_ControlWord_AlarmClean);    //Let the motor connect to power
						}
						else 
							return 0;	
						
						LEFT_Knee.init_flag = 1;//初始化完成标志
					}
					break;
			}
			case 0x3:
			{
					if(RIGHT_Hip.init_flag == 0)
					{
						CAN_BLDC_accelerationSet(Number, acceleration);           //set acceleration
						CAN_BLDC_decelerationSet(Number, deceleration);           //set deceleration
						CAN_BLDC_ResetMod(Number, BLDC_Velocity1_Mode);           //set Velocity-Mode
						if(RIGHT_Hip.set_state == 0x0)
						{
							CAN_BLDC_Control(Number, BLDC_ControlWord_Off);
						}		
						else if (RIGHT_Hip.set_state == 0x1)
						{	
							CAN_BLDC_Control(Number, BLDC_ReControlWord_On);
						}
						else if (RIGHT_Hip.set_state == 0x2)
						{	
							CAN_BLDC_Control(Number, BLDC_ControlWord_AlarmClean);    //Let the motor connect to power
						}
						else 
							return 0;	
						
						RIGHT_Hip.init_flag = 1;//初始化完成标志
					}
					break;
			}
			case 0x4:
			{
					if(RIGHT_Knee.init_flag == 0)
					{
						CAN_BLDC_accelerationSet(Number, acceleration);           //set acceleration
						CAN_BLDC_decelerationSet(Number, deceleration);           //set deceleration
						CAN_BLDC_ResetMod(Number, BLDC_Velocity1_Mode);           //set Velocity-Mode
						if(RIGHT_Knee.set_state == 0x0)
						{
							CAN_BLDC_Control(Number, BLDC_ControlWord_Off);
						}		
						else if (RIGHT_Knee.set_state == 0x1)
						{	
							CAN_BLDC_Control(Number, BLDC_ReControlWord_On);							
						}
						else if (RIGHT_Knee.set_state == 0x2)
						{	
							CAN_BLDC_Control(Number, BLDC_ControlWord_AlarmClean);    //Let the motor connect to power
						}
						else 
							return 0;	
						
						LEFT_Knee.init_flag = 1;//初始化完成标志
					}
					break;
			}
			default: return 0;
		}
		return 1;
}
/*
速度模式下的控制
*/
void CAN_BLDC_SpeedMod(unsigned char Number, long Speed, long acceleration, long deceleration)
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
		
		if(init_VelocityMod(Number, acceleration, deceleration) == 0)
			return ;
		
		tx_message.Data[0] = 0x23;
    tx_message.Data[1] = 0xF0;
    tx_message.Data[2] = 0x2F;
    tx_message.Data[3] = 0x09;
    tx_message.Data[4] = (unsigned char)(Speed&0xff);
    tx_message.Data[5] = (unsigned char)((Speed>>8)&0xff);
    tx_message.Data[6] = (unsigned char)((Speed>>16)&0xff);
    tx_message.Data[7] = (unsigned char)((Speed>>24)&0xff);
		
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