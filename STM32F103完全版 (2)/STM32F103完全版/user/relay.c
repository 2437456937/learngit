/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "delay.h"
#include "relay.h"

/**
 * @brief  初始化继电器控制的GPIO端口
 * 
 * 本函数配置了用于控制继电器的GPIO引脚，包括设置引脚的速度、模式以及初始化状态。
 * 具体配置了PB10、PB11以及PE7至PE14的多个引脚为推挽输出模式，并设置了初始的低电平状态。
 * 同时，函数还启用了SWD调试接口并禁用了JTAG，以减少引脚占用。
 */
void Relay_Gpio_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 启用AFIO外设时钟，用于配置GPIO引脚重映射
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    // 使能SWD接口，同时禁用JTAG，减少引脚占用
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    
    // 启用GPIOB和GPIOE的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOE, ENABLE);
    
    /* 配置GPIO引脚：PB11(加热)和PB15(蜂鸣器) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* 配置GPIO引脚：PE8(出水阀)和PE11(进水阀) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    // 初始化所有控制引脚为低电平，确保设备处于安全状态
    HEAT_OFF();           // 关闭加热
    WATER_IN_VALVE_OFF(); // 关闭进水阀
    WATER_OUT_VALVE_OFF();// 关闭出水阀
    BEEP_OFF();          // 关闭蜂鸣器
}



