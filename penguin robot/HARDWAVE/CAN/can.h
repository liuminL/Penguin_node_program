#ifndef __CAN_H
#define __CAN_H
#include "sys.h"
#include "user.h"

#define CAN1_PACKAGE_LENGTH 8

extern char tempi;
extern char can1RxFinishFlg;
extern uint8_t can1TxMsg[],can1Buffer[];
u8 can1Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);//CAN初始化
u8 can2Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);//CAN初始化
void can1_task(void *p_arg);
void can2_task(void *p_arg);
void can1SendMsg(uint8_t* msg,u8 msgId,u8 len);
u8 can1ReceiveMsg(u8 *buf);							//接收数据
void can2SendMsg(uint8_t driver_id,uint8_t commamd,int32_t absolut_position,uint8_t length);
u8 can2ReceiveMsg(u8 *buf);							//接收数据

struct Motor
{
	char state;
	long motor_position;
};
#endif

