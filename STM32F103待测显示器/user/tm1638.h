/*******************************************************************************
 * 文件名: TM1638.h
 * 作者: https://blog.csdn.net/wanglong3713
 * 版本: v1.0
 * 日期: 2021-11-20
 * 说明: TM1638数码管驱动程序
 *       - 支持8位数码管显示
 *       - 支持8个LED控制
 *       - 支持8个独立按键
 *       - 使用3线接口(STB/CLK/DIO)
*******************************************************************************/
#ifndef _TM1638_H_
#define _TM1638_H_

#include "stm32f10x.h"

/*******************************************************************************
 * 宏定义
*******************************************************************************/
// 显示相关
#define TUBE_DISPLAY_NULL                26   // 数码管不显示的段码索引
#define TUBE_DISPLAY_DECIMAL_PIONT_OFFSET 16  // 带小数点显示的偏移值

// GPIO控制相关宏定义 - 使用标准库函数
// STB引脚控制(PD12) - 片选信号
#define TM1638_STBSet()     GPIO_SetBits(GPIOD, GPIO_Pin_12)     // STB=1
#define TM1638_STBReset()   GPIO_ResetBits(GPIOD, GPIO_Pin_12)   // STB=0

// CLK引脚控制(PD11) - 时钟信号
#define TM1638_CLKSet()     GPIO_SetBits(GPIOD, GPIO_Pin_11)     // CLK=1
#define TM1638_CLKReset()   GPIO_ResetBits(GPIOD, GPIO_Pin_11)   // CLK=0

// DIO引脚控制(PD10) - 数据信号
#define TM1638_DIOSet()     GPIO_SetBits(GPIOD, GPIO_Pin_10)     // DIO=1
#define TM1638_DIOReset()   GPIO_ResetBits(GPIOD, GPIO_Pin_10)   // DIO=0

// DIO方向控制 - 因为需要双向通信
// 定义TM1638数据线为输出模式的宏
#define TM1638_DIOModeOutput()   {GPIO_InitTypeDef GPIO_InitStruct;\ // 初始化GPIO配置结构体
                                GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;\ // 设置GPIO引脚为10
                                GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;\ // 设置GPIO模式为推挽输出
                                GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;\ // 设置GPIO速度为50MHz
                                GPIO_Init(GPIOD, &GPIO_InitStruct);}

// 定义TM1638数据线为输入模式的宏
#define TM1638_DIOModeInput()    {GPIO_InitTypeDef GPIO_InitStruct;\ // 初始化GPIO配置结构体
                                GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;\ // 设置GPIO引脚为10
                                GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;\ // 设置GPIO模式为浮空输入
                                GPIO_Init(GPIOD, &GPIO_InitStruct);}

// 定义读取TM1638数据线输入的宏
#define TM1638_DIORead()         GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_10)

/*******************************************************************************
 * 数据结构定义
*******************************************************************************/
// 8位数码管显示数据结构
typedef struct
{
    uint8_t tube0;  // 最左边数码管(SEG1)
    uint8_t tube1;  // SEG2
    uint8_t tube2;  // SEG3
    uint8_t tube3;  // SEG4
    uint8_t tube4;  // SEG5
    uint8_t tube5;  // SEG6
    uint8_t tube6;  // SEG7
    uint8_t tube7;  // 最右边数码管(SEG8)
}TM1638Tube_ts;

/*******************************************************************************
 * 函数声明
*******************************************************************************/
void TM1638_Init(void);                         // 初始化TM1638
void TM1638_TubeDisplay(TM1638Tube_ts data);    // 数码管显示
void TM1638_LedDisplay(uint8_t data);           // LED显示(1亮0灭)
uint8_t TM1638_ReadKey(void);                   // 读取按键值(1-8,0表示无按键)
void TM1638_WriteCmd(uint8_t u8Cmd);            // 写命令
void TM1638_SetBrightness(uint8_t u8Brt);       // 设置亮度(0-7)
void TM1638_Switch(uint8_t bState);             // 显示开关控制(0:关闭,1:开启)

#endif
/******************************** END OF FILE *********************************/
