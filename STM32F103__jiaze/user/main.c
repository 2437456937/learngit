#include "stm32f10x.h"
#include "delay.h"
#include "relay.h"
#include "ad_get.h"
#include "sys.h"
#include "tm1638.h"
#include "show.h"

// ����ˮ��״̬
#define WATER_STATUS_LOW    0  // ��ˮλ
#define WATER_STATUS_HIGH   1  // ��ˮλ
#define WATER_STATUS_FULL   2  // ��ˮλ

// �������״̬
#define HEAT_STATUS_OFF     0  // ���ȹر�
#define HEAT_STATUS_ON      1  // ���ȿ���

typedef struct 
{
    u16 target_temp;      // Ŀ���¶�
    u16 current_temp;     // ��ǰ�¶�
    u16 low_water_adc;    // ��ˮλADCԭʼֵ
    u16 high_water_adc;   // ��ˮλADCԭʼֵ
    u8 heat_status;       // ����״̬(PB11)
    u8 water_status;      // ˮ��״̬
}WaterHeaterParams;

WaterHeaterParams Params;

// ��ʼ������
static void Params_Init(void)
{
    Params.target_temp = 0;      
    Params.current_temp = 0;
    Params.low_water_adc = 0;
    Params.high_water_adc = 0;
    Params.heat_status = HEAT_STATUS_OFF;
    Params.water_status = WATER_STATUS_LOW;
}

int main(void)
{
    // ��ʼ��ϵͳʱ��
    Stm32_Clock_Init(16);
    delay_init(64);
    
    // ��ʼ��GPIO
    Relay_Gpio_Init();
    delay_ms(1000);
   
    // ��ʼ��ADC 
    Ad_Get_Init(); 
    ADC_Water_Init();
    ADC_Temp_Init();
    
    // ��ʼ������
    Params_Init();
    Params.target_temp = 80;  // ����Ŀ���¶�Ϊ80��

    uint8_t step1_completed = 0;  // ��־λ
    
    // ��ʼ����ʾģ��
    Display_Init();
    
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
        // ��ȡˮλ���¶�ֵ
        Params.low_water_adc = Ad_Get_Value(ADC_Channel_10);
        Params.high_water_adc = Ad_Get_Value(ADC_Channel_11);
        Params.current_temp = Ad_Get_Temp(ADC_Channel_14);

        // ��ʼˮλ������ˮ
        if(!step1_completed)
        {
            if(Params.low_water_adc < 400)  // ��ˮλADֵС��400����Ҫ��ˮ
            {
                WATER_OUT_VALVE_ON();  // �򿪳�ˮ��
                HEAT_OFF();            // �رռ���
                WATER_IN_VALVE_OFF();  // �رս�ˮ��
                
                // �ȴ�ֱ��ˮλADֵ����400
                while(Params.low_water_adc < 400)
                {
                    Params.low_water_adc = Ad_Get_Value(ADC_Channel_10);
                    delay_ms(100);
                }
                
                WATER_OUT_VALVE_OFF(); // �رճ�ˮ��
                step1_completed = 1;
                BEEP_ON();
                delay_ms(500);
                BEEP_OFF();
            }
            else  // ˮλ����������
            {
                step1_completed = 1;
                BEEP_ON();
                delay_ms(500);
                BEEP_OFF();
            }
            continue;
        }
        
        // ����1��ɺ�ִ��ˮλ���¶ȿ���
        if(step1_completed)
        {
            // ����ˮλ�����Ʋ�ˮ
            if(Params.high_water_adc > 450)  // ��ˮλδ��
            {
                WATER_IN_VALVE_ON();  // �򿪽�ˮ��
            }
            else if(Params.high_water_adc <= 300)  // ˮ������
            {
                WATER_IN_VALVE_OFF();  // �رս�ˮ��
            }
            
            // ����ˮλ�����Ƽ���
            if(Params.low_water_adc < 400)  // ˮλ���ʣ����Լ���
            {
                // �¶ȿ���
                if(Params.current_temp < Params.target_temp)
                {
                    HEAT_ON();  // ��������
                    Params.heat_status = HEAT_STATUS_ON;
                }
                else
                {
                    HEAT_OFF(); // �رռ���
                    Params.heat_status = HEAT_STATUS_OFF;
                }
            }
            else
            {
                HEAT_OFF();  // ˮλ���ͣ�ֹͣ����
                Params.heat_status = HEAT_STATUS_OFF;
            }
        }
        
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
        for(uint32_t i = 0; i < 100000; i++);
    }
}

