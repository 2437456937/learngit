#ifndef _SHOW_H_
#define _SHOW_H_

#include "stm32f10x.h"
#include "tm1638.h"

/* ������ض��� */
// ����ֵ���� (1,3,5,7 / 2,4,6,8)
#define KEY_NULL    0x00    // �ް���
#define KEY_SET     0x01    // ���ü� - ����/�˳�����ģʽ
#define KEY_UP      0x03    // ���Ӽ� - ����ģʽ�������¶�
#define KEY_DOWN    0x05    // ���ټ� - ����ģʽ�¼����¶�

// ����״̬����
#define KEY_RELEASED    0x00    // �����ͷ�״̬
#define KEY_PRESSED     0x01    // ��������״̬

/* �������Ͷ��� */
// ������־λ�ṹ��
typedef union {
    uint8_t byte;           // ������������
    struct {
        uint8_t bSetMode:1; // ����ģʽ��־��1=����ģʽ��0=����ģʽ
        uint8_t b1:1;       // ����
        uint8_t b2:1;       // ����
        uint8_t b3:1;       // ����
        uint8_t b4:1;       // ����
        uint8_t b5:1;       // ����
        uint8_t b6:1;       // ����
        uint8_t b7:1;       // ����
    }bt;
}KeyFlag_tu;

// LED��ʾ����������
typedef union {
    uint8_t byte;    // �����������
    struct {
        uint8_t led0 : 1;  // LED1
        uint8_t led1 : 1;  // LED2
        uint8_t led2 : 1;  // LED3
        uint8_t led3 : 1;  // LED4
        uint8_t led4 : 1;  // LED5
        uint8_t led5 : 1;  // LED6
        uint8_t led6 : 1;  // LED7
        uint8_t led7 : 1;  // LED8
    } bits;
} LedData_tu;

/* �������� */
// ��ʾ��غ���
void Display_Init(void);              // ��ʼ����ʾģ��
void Display_TubeDataProcess(void);   // �����������ʾ
void Display_LedDataProcess(void);    // ����LED��ʾ

// ������غ���
void Key_Init(void);                  // ��ʼ������
void Key_ScanProcess(void);           // ����ɨ�账��

#endif



