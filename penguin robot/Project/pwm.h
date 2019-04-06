#ifndef __pwm_H
#define __pwm_H
#include "sys.h"
#include "user.h"

void steer_task(void *p_arg);
void TIM3_PWM_Init(u16 arr,u16 psc);
void TIM1_PWM_Init(u16 arr,u16 psc);
//extern uint16_t PWMVAL1,PWMVAL2,PWMVAL3;
extern uint16_t PWMVAL[5];
#endif