#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include "tm1638.h"

int main(void)
{
    // 初始化系统时钟
    Stm32_Clock_Init(16);
    delay_init(64);
    
    // 初始化TM1638
    TM1638_Init();
    
    // 配置显示参数
    TM1638_Switch(0);            // 先关闭显示
    TM1638_SetBrightness(7);     // 设置最大亮度
    TM1638_WriteCmd(0x44);       // 设置为固定地址模式
    TM1638_Switch(1);            // 打开显示
    
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
        delay_ms(100);  // 使用delay_ms替代for循环延时
    }
}

