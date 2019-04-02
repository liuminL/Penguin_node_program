 /**
  ******************************************************************************
  * @file    HARDWARE/ADC/adc.c 
  * @author  Liao
  * @version V1
  * @date    2017.03.31
  * @brief   Sensors data sample.
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
 
#include "main.h"
__IO uint16_t ADC1ConvertedValue[SIMPLE_TIME][SIMPLE_CHANNEL]; //75*8
__IO uint32_t sensorsData[SIMPLE_CHANNEL] = {0};
uint32_t hipValuePro = 4095;
int32_t temp = 0 ,temp1 = 0;
/*******************************************************************************
* Function Name  : sampleInit
* Description    : This function is initialize the ADC of sensors sample channel.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/																   
void  sampleInit(void)
{ 	
	ADC_InitTypeDef 	ADC_InitStructure; 
	GPIO_InitTypeDef 	GPIO_InitStructure;
	DMA_InitTypeDef  	DMA_InitStructure;                                        //DMA��ʼ���ṹ��
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE );	 	 //ʹ��ADC1ͨ��ʱ��
 

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   										 		//����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M

	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_BufferSize=SIMPLE_TIME*SIMPLE_CHANNEL;                     //�������ݸ���
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;                                //������Ϊ����Դ
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;                                      //��ʹ���ڴ浽�ڴ�
	DMA_InitStructure.DMA_MemoryBaseAddr=(uint32_t)ADC1ConvertedValue;             //�ڴ��д洢DMA������������ݻ���ַ
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;               //�ڴ�����Ϊ���� 16λ��ADC����Ϊ12λ��
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;                           //�ڴ��ַҪ����
	DMA_InitStructure.DMA_Mode=DMA_Mode_Circular;                                   //DMA������ѭ��ģʽ
	DMA_InitStructure.DMA_PeripheralBaseAddr= (uint32_t)&ADC1->DR;                  //����Դ��ַ  ADC���ݼĴ�����ַ
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;       //DMA�������ݿ�� 
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;                  //�����ַ����
	DMA_InitStructure.DMA_Priority=DMA_Priority_High; 	                             //DMA���ȼ���
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);                                    //DMA��ʼ��
	DMA_Cmd(DMA1_Channel1, ENABLE); 
	 
	/* 	
	AD3 PA1 ADC1
	AD4 PA2 ADC2        AD4->become data 1     The Hip motor of the left leg
	AD8 PA3 ADC3        
	AD5 PA4 ADC4        AD5->become data 2     The Knee motor of the left leg 
	AD2 PA5 ADC5         
	AD1 PA6 ADC6
	AD7 PA7 ADC7        AD7->become data 3     The Hip motor of the right leg
	AD6 PC4 ADC14       AD6->become data 4     The Knee motor of the right leg
	*/   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;						//ģ����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;						//ģ����������
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
	ADC_DeInit(ADC1);  													//��λADC1 

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;					//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;						//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;					//ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;				//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = SIMPLE_CHANNEL;				//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);									//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   
	
	/*
	4 5 7 6 1 2 3 8  ->
  1 2 3 4 5 6 7 8	
	*/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 7, ADC_SampleTime_7Cycles5 );		//ADC1,ADCͨ��9,����ʱ��Ϊ7.5����
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_7Cycles5 );		//ADC1,ADCͨ��3,����ʱ��Ϊ7.5����
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 8, ADC_SampleTime_7Cycles5 );		//ADC1,ADCͨ��15,����ʱ��Ϊ7.5����
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 2, ADC_SampleTime_7Cycles5 );		//ADC1,ADCͨ��4,����ʱ��Ϊ7.5����
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 6, ADC_SampleTime_7Cycles5 );		//ADC1,ADCͨ��5,����ʱ��Ϊ7.5����
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 5, ADC_SampleTime_7Cycles5 );		//ADC1,ADCͨ��6,����ʱ��Ϊ7.5����
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 3, ADC_SampleTime_7Cycles5 );		//ADC1,ADCͨ��7,����ʱ��Ϊ7.5����
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 4, ADC_SampleTime_7Cycles5 );		//ADC1,ADCͨ��8,����ʱ��Ϊ7.5����
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 9, ADC_SampleTime_7Cycles5 );		//ADC1,ADCͨ��9,����ʱ��Ϊ7.5����
	  	
	ADC_Cmd(ADC1, ENABLE);						//ʹ��ָ����ADC1

	/* ʹ�� ADC1 DMA */
    ADC_DMACmd(ADC1, ENABLE);
	
	ADC_ResetCalibration(ADC1);					//ʹ�ܸ�λУ׼  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����
	
	ADC_StartCalibration(ADC1);	 				//����ADУ׼
 
	while(ADC_GetCalibrationStatus(ADC1));	 	//�ȴ�У׼����
 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������

}				  

/*******************************************************************************
* Function Name  : sample_task
* Description    : Update sensors data.
* Input          : None
* Output         : None
* Return         : None
	Channel0:knee_encoder;Channel1:hip_encoder;Channel2:yaw_encoder;Channel3:foot_stress;Channel4:knee_torque;
	Channel5:hip_torque;Channel6:knee_temperature;Channel7:hip_temperatureChannel8:yaw_temperature
*******************************************************************************/

//����������������
void sample_task(void *p_arg)
{
	char i,j;
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		//��ֵ
		for(i=0;i<SIMPLE_CHANNEL;i++)
		{ 
			sensorsData[i] = 0;
			
			for(j=0;j<SIMPLE_TIME;j++)
			{
 				sensorsData[i]+= ADC1ConvertedValue[j][i];					  
			}	
				sensorsData[i] = sensorsData[i]/SIMPLE_TIME;
				/*switch(i)
				{
					case 1:
//							sensorsData[i] = sensorsData[i];		//hip�ؽڽǶ�
						sensorsData[i] = -0.0003288*sensorsData[i]*sensorsData[i] + 3.062*sensorsData[i]-275;		//hip�ؽڽǶ�
						if(sensorsData[i] < 300)	//�ڵ�崫���������쳣
						{
							nodeStatus = 2;
						}
//						if(hipValuePro == 4095)
//							hipValuePro = sensorsData[i];
//						else
//						{
//								temp = sensorsData[i]-hipValuePro;
////								temp1 = abs(temp);
//							if((sensorsData[i]-hipValuePro>=3)||(hipValuePro-sensorsData[i]>=3))						//�޷��˲�����
////							if(temp >= 3)
//								{
//											sensorsData[i] = hipValuePro;
//								}

//						}
						break;
					case 4:
						sensorsData[i] = (sensorsData[i]+TORQUE_KNEE_OFFSET)<<1;		//knee������Ť��
						break;
					case 5:
						sensorsData[i] = (sensorsData[i]+TORQUE_HIP_OFFSET)<<1;		//hip������Ť��
						break;
					default:
						break;				
				}
*/
				can1Buffer[2*i] = sensorsData[i]&0xff;
				can1Buffer[2*i+1] = (sensorsData[i]>>8)&0xff;			
		}
		//hipValuePro = sensorsData[1]; 
		//�������¶�
		//sensor.kneeMotorTemp = TEMP_RATIO*sensorsData[6] + TEMP_OFFSET;
		//sensor.hipMotorTemp  = TEMP_RATIO*sensorsData[7] + TEMP_OFFSET;
		//sensor.yawMotorTemp  = TEMP_RATIO*sensorsData[8] + TEMP_OFFSET;		
		OSTimeDlyHMSM(0,0,0,1,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ,�����������5ms
	}
}

