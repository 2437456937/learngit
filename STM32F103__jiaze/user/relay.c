/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "delay.h"
#include "relay.h"

/**
 * @brief  ��ʼ���̵������Ƶ�GPIO�˿�
 * 
 * ���������������ڿ��Ƽ̵�����GPIO���ţ������������ŵ��ٶȡ�ģʽ�Լ���ʼ��״̬��
 * ����������PB10��PB11�Լ�PE7��PE14�Ķ������Ϊ�������ģʽ���������˳�ʼ�ĵ͵�ƽ״̬��
 * ͬʱ��������������SWD���Խӿڲ�������JTAG���Լ�������ռ�á�
 */
void Relay_Gpio_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // ����AFIO����ʱ�ӣ���������GPIO������ӳ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    // ʹ��SWD�ӿڣ�ͬʱ����JTAG����������ռ��
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    
    // ����GPIOB��GPIOE��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOE, ENABLE);
    
    /* ����GPIO���ţ�PB11(����)��PB15(������) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* ����GPIO���ţ�PE8(��ˮ��)��PE11(��ˮ��) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    // ��ʼ�����п�������Ϊ�͵�ƽ��ȷ���豸���ڰ�ȫ״̬
    HEAT_OFF();           // �رռ���
    WATER_IN_VALVE_OFF(); // �رս�ˮ��
    WATER_OUT_VALVE_OFF();// �رճ�ˮ��
    BEEP_OFF();          // �رշ�����
}



