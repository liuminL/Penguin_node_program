#ifndef __JOINTCODER_H
#define __JOINTCODER_H	
#include "sys.h"

#define SIMPLE_TIME 75 		//����ÿ�����ֵ������
#define SIMPLE_CHANNEL 8	//���ò�����ͨ������

void sampleInit(void);
void sample_task(void *p_arg);
extern __IO uint32_t sensorsData[SIMPLE_CHANNEL];
#endif 
