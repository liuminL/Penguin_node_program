#ifndef __JOINTCODER_H
#define __JOINTCODER_H	
#include "sys.h"

#define SIMPLE_TIME 75 		//设置每组采样值的数量
#define SIMPLE_CHANNEL 8	//设置采样的通道数量

void sampleInit(void);
void sample_task(void *p_arg);
extern __IO uint32_t sensorsData[SIMPLE_CHANNEL];
#endif 
