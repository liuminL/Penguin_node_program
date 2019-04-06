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
	DMA_InitTypeDef  	DMA_InitStructure;                                        //DMA初始化结构体
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE );	 	 //使能ADC1通道时钟
 

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   										 		//设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_BufferSize=SIMPLE_TIME*SIMPLE_CHANNEL;                     //缓存数据个数
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;                                //以外设为数据源
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;                                      //不使用内存到内存
	DMA_InitStructure.DMA_MemoryBaseAddr=(uint32_t)ADC1ConvertedValue;             //内存中存储DMA传输过来的数据基地址
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;               //内存数据为半字 16位（ADC精度为12位）
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;                           //内存地址要增加
	DMA_InitStructure.DMA_Mode=DMA_Mode_Circular;                                   //DMA工作在循环模式
	DMA_InitStructure.DMA_PeripheralBaseAddr= (uint32_t)&ADC1->DR;                  //数据源地址  ADC数据寄存器地址
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;       //DMA外设数据宽度 
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;                  //外设地址增加
	DMA_InitStructure.DMA_Priority=DMA_Priority_High; 	                             //DMA优先级高
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);                                    //DMA初始化
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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;						//模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;						//模拟输入引脚
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
	ADC_DeInit(ADC1);  													//复位ADC1 

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;					//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;						//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;					//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;				//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = SIMPLE_CHANNEL;				//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);									//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   
	
	/*
	4 5 7 6 1 2 3 8  ->
  1 2 3 4 5 6 7 8	
	*/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 7, ADC_SampleTime_7Cycles5 );		//ADC1,ADC通道9,采样时间为7.5周期
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_7Cycles5 );		//ADC1,ADC通道3,采样时间为7.5周期
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 8, ADC_SampleTime_7Cycles5 );		//ADC1,ADC通道15,采样时间为7.5周期
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 2, ADC_SampleTime_7Cycles5 );		//ADC1,ADC通道4,采样时间为7.5周期
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 6, ADC_SampleTime_7Cycles5 );		//ADC1,ADC通道5,采样时间为7.5周期
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 5, ADC_SampleTime_7Cycles5 );		//ADC1,ADC通道6,采样时间为7.5周期
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 3, ADC_SampleTime_7Cycles5 );		//ADC1,ADC通道7,采样时间为7.5周期
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 4, ADC_SampleTime_7Cycles5 );		//ADC1,ADC通道8,采样时间为7.5周期
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 9, ADC_SampleTime_7Cycles5 );		//ADC1,ADC通道9,采样时间为7.5周期
	  	
	ADC_Cmd(ADC1, ENABLE);						//使能指定的ADC1

	/* 使能 ADC1 DMA */
    ADC_DMACmd(ADC1, ENABLE);
	
	ADC_ResetCalibration(ADC1);					//使能复位校准  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	
	ADC_StartCalibration(ADC1);	 				//开启AD校准
 
	while(ADC_GetCalibrationStatus(ADC1));	 	//等待校准结束
 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能

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

//传感器采样任务函数
void sample_task(void *p_arg)
{
	char i,j;
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		//均值
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
//							sensorsData[i] = sensorsData[i];		//hip关节角度
						sensorsData[i] = -0.0003288*sensorsData[i]*sensorsData[i] + 3.062*sensorsData[i]-275;		//hip关节角度
						if(sensorsData[i] < 300)	//节点板传感器数据异常
						{
							nodeStatus = 2;
						}
//						if(hipValuePro == 4095)
//							hipValuePro = sensorsData[i];
//						else
//						{
//								temp = sensorsData[i]-hipValuePro;
////								temp1 = abs(temp);
//							if((sensorsData[i]-hipValuePro>=3)||(hipValuePro-sensorsData[i]>=3))						//限幅滤波处理
////							if(temp >= 3)
//								{
//											sensorsData[i] = hipValuePro;
//								}

//						}
						break;
					case 4:
						sensorsData[i] = (sensorsData[i]+TORQUE_KNEE_OFFSET)<<1;		//knee电机输出扭矩
						break;
					case 5:
						sensorsData[i] = (sensorsData[i]+TORQUE_HIP_OFFSET)<<1;		//hip电机输出扭矩
						break;
					default:
						break;				
				}
*/
				can1Buffer[2*i] = sensorsData[i]&0xff;
				can1Buffer[2*i+1] = (sensorsData[i]>>8)&0xff;			
		}
		//hipValuePro = sensorsData[1]; 
		//计算电机温度
		//sensor.kneeMotorTemp = TEMP_RATIO*sensorsData[6] + TEMP_OFFSET;
		//sensor.hipMotorTemp  = TEMP_RATIO*sensorsData[7] + TEMP_OFFSET;
		//sensor.yawMotorTemp  = TEMP_RATIO*sensorsData[8] + TEMP_OFFSET;		
		OSTimeDlyHMSM(0,0,0,1,OS_OPT_TIME_HMSM_STRICT,&err); //延时,发起任务调度5ms
	}
}

