#ifndef __RELAY_H
#define __RELAY_H

#include "stm32f10x.h"

// ���ȿ��� - PB11
#define HEAT_OFF()        GPIO_SetBits(GPIOB, GPIO_Pin_11)
#define HEAT_ON()       GPIO_ResetBits(GPIOB, GPIO_Pin_11)

// ��ˮ������ - PE11
#define WATER_IN_VALVE_ON()   GPIO_SetBits(GPIOE, GPIO_Pin_11)
#define WATER_IN_VALVE_OFF()  GPIO_ResetBits(GPIOE, GPIO_Pin_11)

// ��ˮ������ - PE8
#define WATER_OUT_VALVE_ON()  GPIO_SetBits(GPIOE, GPIO_Pin_8)
#define WATER_OUT_VALVE_OFF() GPIO_ResetBits(GPIOE, GPIO_Pin_8)

// ���������� - PB15
#define BEEP_ON()       GPIO_SetBits(GPIOB, GPIO_Pin_15)    
#define BEEP_OFF()      GPIO_ResetBits(GPIOB, GPIO_Pin_15) 

void Relay_Gpio_Init(void);

#endif
