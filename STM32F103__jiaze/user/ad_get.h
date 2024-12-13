#ifndef __AD_GET_H
#define __AD_GET_H

#include "stm32f10x.h"

// ������״̬����
#define SENSOR_STATUS_NORMAL  0     // ����������
#define SENSOR_STATUS_SHORT   0x1000 // ��������·
#define SENSOR_STATUS_OPEN    0x2000 // ��������·

// ADC��ʼ������
void Ad_Get_Init(void);           // ADC������ʼ��
void ADC_Water_Init(void);        // ˮλADC��ʼ��
void ADC_Temp_Init(void);         // �¶�ADC��ʼ��

// ADC�ɼ�����
uint16_t Get_Adc(uint8_t ch);     // ��ȡ����ADCֵ
uint16_t Ad_Get_Value(uint8_t ch); // ��ȡƽ��ADCֵ
uint16_t Ad_Get_Temp(uint8_t ch); // �޸�Ϊֱ�ӷ����¶�ֵ

// ˮλ���¶Ȳɼ�����
uint16_t GET_WATER_LOW(void);     // ��ȡ��ˮλADCֵ
uint16_t GET_WATER_HIGH(void);    // ��ȡ��ˮλADCֵ
uint16_t GET_NTC_TEMP(void);      // ��ȡNTC�¶�ֵ

#endif
