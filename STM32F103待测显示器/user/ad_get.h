#ifndef __AD_GET_H
#define __AD_GET_H

#include "stm32f10x.h"

// 传感器状态定义
#define SENSOR_STATUS_NORMAL  0     // 传感器正常
#define SENSOR_STATUS_SHORT   0x1000 // 传感器短路
#define SENSOR_STATUS_OPEN    0x2000 // 传感器开路

// ADC初始化函数
void Ad_Get_Init(void);           // ADC基础初始化
void ADC_Water_Init(void);        // 水位ADC初始化
void ADC_Temp_Init(void);         // 温度ADC初始化

// ADC采集函数
uint16_t Get_Adc(uint8_t ch);     // 获取单次ADC值
uint16_t Ad_Get_Value(uint8_t ch); // 获取平均ADC值
uint16_t Ad_Get_Temp(uint8_t ch); // 修改为直接返回温度值

// 水位和温度采集函数
uint16_t GET_WATER_LOW(void);     // 获取低水位ADC值
uint16_t GET_WATER_HIGH(void);    // 获取高水位ADC值
uint16_t GET_NTC_TEMP(void);      // 获取NTC温度值

#endif
