#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include "tm1638.h"

int main(void)
{
    // ��ʼ��ϵͳʱ��
    Stm32_Clock_Init(16);
    delay_init(64);
    
    // ��ʼ��TM1638
    TM1638_Init();
    
    // ������ʾ����
    TM1638_Switch(0);            // �ȹر���ʾ
    TM1638_SetBrightness(7);     // �����������
    TM1638_WriteCmd(0x44);       // ����Ϊ�̶���ַģʽ
    TM1638_Switch(1);            // ����ʾ
    
    // ׼����ʾ����
    TM1638Tube_ts tubeData = {0};  // �������ʾ����
    uint8_t ledData = 0x01;        // LED��ʾ���ݣ���ʼֻ�����ұ�LED
    
    // ��ʾ��������0-7
    tubeData.tube0 = 0;  // ��ʾ0
    tubeData.tube1 = 1;  // ��ʾ1
    tubeData.tube2 = 2;  // ��ʾ2
    tubeData.tube3 = 3;  // ��ʾ3
    tubeData.tube4 = 4;  // ��ʾ4
    tubeData.tube5 = 5;  // ��ʾ5
    tubeData.tube6 = 6;  // ��ʾ6
    tubeData.tube7 = 7;  // ��ʾ7
    
    while(1)
    {
        TM1638_TubeDisplay(tubeData);  // �����������ʾ
        TM1638_LedDisplay(ledData);    // ����LED��ʾ
        
        // ��ȡ����ֵ
        uint8_t keyValue = TM1638_ReadKey();
        if(keyValue != 0)
        {
            // ����а������£��ƶ�LEDλ��
            ledData = (ledData << 1) | (ledData >> 7);  // ѭ������
        }
        
        // ����ʱ
        delay_ms(100);  // ʹ��delay_ms���forѭ����ʱ
    }
}

