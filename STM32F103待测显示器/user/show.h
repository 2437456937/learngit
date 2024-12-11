#ifndef _SHOW_H_
#define _SHOW_H_

#include "stm32f10x.h"
#include "tm1638.h"

/* 按键相关定义 */
// 按键值定义 (1,3,5,7 / 2,4,6,8)
#define KEY_NULL    0x00    // 无按键
#define KEY_SET     0x01    // 设置键 - 进入/退出设置模式
#define KEY_UP      0x03    // 增加键 - 设置模式下增加温度
#define KEY_DOWN    0x05    // 减少键 - 设置模式下减少温度

// 按键状态定义
#define KEY_RELEASED    0x00    // 按键释放状态
#define KEY_PRESSED     0x01    // 按键按下状态

/* 数据类型定义 */
// 按键标志位结构体
typedef union {
    uint8_t byte;           // 用于整体清零
    struct {
        uint8_t bSetMode:1; // 设置模式标志，1=设置模式，0=正常模式
        uint8_t b1:1;       // 保留
        uint8_t b2:1;       // 保留
        uint8_t b3:1;       // 保留
        uint8_t b4:1;       // 保留
        uint8_t b5:1;       // 保留
        uint8_t b6:1;       // 保留
        uint8_t b7:1;       // 保留
    }bt;
}KeyFlag_tu;

// LED显示数据联合体
typedef union {
    uint8_t byte;    // 用于整体操作
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

/* 函数声明 */
// 显示相关函数
void Display_Init(void);              // 初始化显示模块
void Display_TubeDataProcess(void);   // 处理数码管显示
void Display_LedDataProcess(void);    // 处理LED显示

// 按键相关函数
void Key_Init(void);                  // 初始化按键
void Key_ScanProcess(void);           // 按键扫描处理

#endif



