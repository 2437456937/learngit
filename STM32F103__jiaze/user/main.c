#include "stm32f10x.h"
#include "delay.h"
#include "relay.h"
#include "ad_get.h"
#include "sys.h"
#include "tm1638.h"
#include "show.h"

// 定义水箱状态
#define WATER_STATUS_LOW    0  // 低水位
#define WATER_STATUS_HIGH   1  // 高水位
#define WATER_STATUS_FULL   2  // 满水位

// 定义加热状态
#define HEAT_STATUS_OFF     0  // 加热关闭
#define HEAT_STATUS_ON      1  // 加热开启

typedef struct 
{
    u16 target_temp;      // 目标温度
    u16 current_temp;     // 当前温度
    u16 low_water_adc;    // 低水位ADC原始值
    u16 high_water_adc;   // 高水位ADC原始值
    u8 heat_status;       // 加热状态(PB11)
    u8 water_status;      // 水箱状态
}WaterHeaterParams;

WaterHeaterParams Params;

// 初始化参数
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
    // 初始化系统时钟
    Stm32_Clock_Init(16);
    delay_init(64);
    
    // 初始化GPIO
    Relay_Gpio_Init();
    delay_ms(1000);
   
    // 初始化ADC 
    Ad_Get_Init(); 
    ADC_Water_Init();
    ADC_Temp_Init();
    
    // 初始化参数
    Params_Init();
    Params.target_temp = 80;  // 设置目标温度为80度

    uint8_t step1_completed = 0;  // 标志位
    
    // 初始化显示模块
    Display_Init();
    
    // 准备显示数据
    TM1638Tube_ts tubeData = {0};  // 数码管显示数据
    uint8_t ledData = 0x01;        // LED显示数据，初始只亮最右边LED
    
    // 显示测试数字0-7
    tubeData.tube0 = 0;  // 显示0
    tubeData.tube1 = 1;  // 显示1
    tubeData.tube2 = 2;  // 显示2
    tubeData.tube3 = 3;  // 显示3
    tubeData.tube4 = 4;  // 显示4
    tubeData.tube5 = 5;  // 显示5
    tubeData.tube6 = 6;  // 显示6
    tubeData.tube7 = 7;  // 显示7
    
    while(1)
    {
        // 获取水位和温度值
        Params.low_water_adc = Ad_Get_Value(ADC_Channel_10);
        Params.high_water_adc = Ad_Get_Value(ADC_Channel_11);
        Params.current_temp = Ad_Get_Temp(ADC_Channel_14);

        // 初始水位检测和排水
        if(!step1_completed)
        {
            if(Params.low_water_adc < 400)  // 低水位AD值小于400，需要排水
            {
                WATER_OUT_VALVE_ON();  // 打开出水阀
                HEAT_OFF();            // 关闭加热
                WATER_IN_VALVE_OFF();  // 关闭进水阀
                
                // 等待直到水位AD值大于400
                while(Params.low_water_adc < 400)
                {
                    Params.low_water_adc = Ad_Get_Value(ADC_Channel_10);
                    delay_ms(100);
                }
                
                WATER_OUT_VALVE_OFF(); // 关闭出水阀
                step1_completed = 1;
                BEEP_ON();
                delay_ms(500);
                BEEP_OFF();
            }
            else  // 水位本来就正常
            {
                step1_completed = 1;
                BEEP_ON();
                delay_ms(500);
                BEEP_OFF();
            }
            continue;
        }
        
        // 步骤1完成后执行水位和温度控制
        if(step1_completed)
        {
            // 检查高水位，控制补水
            if(Params.high_water_adc > 450)  // 高水位未满
            {
                WATER_IN_VALVE_ON();  // 打开进水阀
            }
            else if(Params.high_water_adc <= 300)  // 水箱已满
            {
                WATER_IN_VALVE_OFF();  // 关闭进水阀
            }
            
            // 检查低水位，控制加热
            if(Params.low_water_adc < 400)  // 水位合适，可以加热
            {
                // 温度控制
                if(Params.current_temp < Params.target_temp)
                {
                    HEAT_ON();  // 开启加热
                    Params.heat_status = HEAT_STATUS_ON;
                }
                else
                {
                    HEAT_OFF(); // 关闭加热
                    Params.heat_status = HEAT_STATUS_OFF;
                }
            }
            else
            {
                HEAT_OFF();  // 水位过低，停止加热
                Params.heat_status = HEAT_STATUS_OFF;
            }
        }
        
        TM1638_TubeDisplay(tubeData);  // 更新数码管显示
        TM1638_LedDisplay(ledData);    // 更新LED显示
        
        // 读取按键值
        uint8_t keyValue = TM1638_ReadKey();
        if(keyValue != 0)
        {
            // 如果有按键按下，移动LED位置
            ledData = (ledData << 1) | (ledData >> 7);  // 循环左移
        }
        
        // 简单延时
        for(uint32_t i = 0; i < 100000; i++);
    }
}

