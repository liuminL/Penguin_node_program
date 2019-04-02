#include "main.h"

   
//LED IO初始化
void LED_Init(void)
{
 
	 GPIO_InitTypeDef  GPIO_InitStructure;
		
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//使能PA端口时钟
		
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;		          //  LED1-->PB11端口配置
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 	//推挽输出
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 	//IO口速度为50MHz
	 GPIO_Init(GPIOB, &GPIO_InitStructure);					 				//根据设定参数初始化GPIOB_11
	 //GPIO_SetBits(GPIOC,GPIO_Pin_0);												//PC0输出高

}


//led0任务函数
void led_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
//		if((sensor.kneeMotorTemp>=50)||(sensor.hipMotorTemp>=50)||(sensor.yawMotorTemp>=50))
//		{
//			BELL = !BELL;
//		}
		LED0 = !LED0 ;
		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err); //延时,发起任务调度
	}
}
