#include "main.h"

   
//LED IO��ʼ��
void LED_Init(void)
{
 
	 GPIO_InitTypeDef  GPIO_InitStructure;
		
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//ʹ��PA�˿�ʱ��
		
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;		          //  LED1-->PB11�˿�����
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 	//�������
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 	//IO���ٶ�Ϊ50MHz
	 GPIO_Init(GPIOB, &GPIO_InitStructure);					 				//�����趨������ʼ��GPIOB_11
	 //GPIO_SetBits(GPIOC,GPIO_Pin_0);												//PC0�����

}


//led0������
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
		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ,�����������
	}
}
