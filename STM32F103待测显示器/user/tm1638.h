/*******************************************************************************
 * �ļ���: TM1638.h
 * ����: https://blog.csdn.net/wanglong3713
 * �汾: v1.0
 * ����: 2021-11-20
 * ˵��: TM1638�������������
 *       - ֧��8λ�������ʾ
 *       - ֧��8��LED����
 *       - ֧��8����������
 *       - ʹ��3�߽ӿ�(STB/CLK/DIO)
*******************************************************************************/
#ifndef _TM1638_H_
#define _TM1638_H_

#include "stm32f10x.h"

/*******************************************************************************
 * �궨��
*******************************************************************************/
// ��ʾ���
#define TUBE_DISPLAY_NULL                26   // ����ܲ���ʾ�Ķ�������
#define TUBE_DISPLAY_DECIMAL_PIONT_OFFSET 16  // ��С������ʾ��ƫ��ֵ

// GPIO������غ궨�� - ʹ�ñ�׼�⺯��
// STB���ſ���(PD12) - Ƭѡ�ź�
#define TM1638_STBSet()     GPIO_SetBits(GPIOD, GPIO_Pin_12)     // STB=1
#define TM1638_STBReset()   GPIO_ResetBits(GPIOD, GPIO_Pin_12)   // STB=0

// CLK���ſ���(PD11) - ʱ���ź�
#define TM1638_CLKSet()     GPIO_SetBits(GPIOD, GPIO_Pin_11)     // CLK=1
#define TM1638_CLKReset()   GPIO_ResetBits(GPIOD, GPIO_Pin_11)   // CLK=0

// DIO���ſ���(PD10) - �����ź�
#define TM1638_DIOSet()     GPIO_SetBits(GPIOD, GPIO_Pin_10)     // DIO=1
#define TM1638_DIOReset()   GPIO_ResetBits(GPIOD, GPIO_Pin_10)   // DIO=0

// DIO������� - ��Ϊ��Ҫ˫��ͨ��
// ����TM1638������Ϊ���ģʽ�ĺ�
#define TM1638_DIOModeOutput()   {GPIO_InitTypeDef GPIO_InitStruct;\ // ��ʼ��GPIO���ýṹ��
                                GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;\ // ����GPIO����Ϊ10
                                GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;\ // ����GPIOģʽΪ�������
                                GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;\ // ����GPIO�ٶ�Ϊ50MHz
                                GPIO_Init(GPIOD, &GPIO_InitStruct);}

// ����TM1638������Ϊ����ģʽ�ĺ�
#define TM1638_DIOModeInput()    {GPIO_InitTypeDef GPIO_InitStruct;\ // ��ʼ��GPIO���ýṹ��
                                GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;\ // ����GPIO����Ϊ10
                                GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;\ // ����GPIOģʽΪ��������
                                GPIO_Init(GPIOD, &GPIO_InitStruct);}

// �����ȡTM1638����������ĺ�
#define TM1638_DIORead()         GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_10)

/*******************************************************************************
 * ���ݽṹ����
*******************************************************************************/
// 8λ�������ʾ���ݽṹ
typedef struct
{
    uint8_t tube0;  // ����������(SEG1)
    uint8_t tube1;  // SEG2
    uint8_t tube2;  // SEG3
    uint8_t tube3;  // SEG4
    uint8_t tube4;  // SEG5
    uint8_t tube5;  // SEG6
    uint8_t tube6;  // SEG7
    uint8_t tube7;  // ���ұ������(SEG8)
}TM1638Tube_ts;

/*******************************************************************************
 * ��������
*******************************************************************************/
void TM1638_Init(void);                         // ��ʼ��TM1638
void TM1638_TubeDisplay(TM1638Tube_ts data);    // �������ʾ
void TM1638_LedDisplay(uint8_t data);           // LED��ʾ(1��0��)
uint8_t TM1638_ReadKey(void);                   // ��ȡ����ֵ(1-8,0��ʾ�ް���)
void TM1638_WriteCmd(uint8_t u8Cmd);            // д����
void TM1638_SetBrightness(uint8_t u8Brt);       // ��������(0-7)
void TM1638_Switch(uint8_t bState);             // ��ʾ���ؿ���(0:�ر�,1:����)

#endif
/******************************** END OF FILE *********************************/
