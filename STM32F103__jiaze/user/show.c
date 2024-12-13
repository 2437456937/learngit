#include "show.h"
#include "string.h"

/*******************************************************************************
 * 文件说明：显示和按键处理模块
 * 
 * 主要功能：
 * 1. 数码管显示控制
 * 2. LED指示灯控制
 * 3. 按键检测和处理
 * 4. 温度设置功能
*******************************************************************************/

/* 静态变量定义 */
// 显示相关变量
static TM1638Tube_ts sDisplay_TubeData;  // 数码管显示数据
static LedData_tu uDisplay_LedData;      // LED显示数据

// 按键相关变量
static uint8_t u8Key_Name;              // 当前按键值
static uint8_t u8KeyState;              // 当前按键状态
static KeyFlag_tu uKey_Flag;            // 按键功能标志
static uint32_t u32Key_SettingTemper;   // 温度设定值(实际值×100)

/**
 * @brief 初始化显示模块
 * 
 * 初始化TM1638硬件，配置显示参数，并初始化显示数据
 */
void Display_Init(void)
{
    // 初始化TM1638硬件
    TM1638_Init();
    
    // 配置显示参数
    TM1638_Switch(0);            // 先关闭显示
    TM1638_SetBrightness(7);     // 设置最大亮度
    TM1638_WriteCmd(0x44);       // 设置为固定地址模式
    TM1638_Switch(1);            // 打开显示
    
    // 初始化显示数据
    uDisplay_LedData.byte = 0;   // 清除LED显示
    memset(&sDisplay_TubeData, 0x00, sizeof(sDisplay_TubeData));  // 清除数码管显示
}

/**
 * @brief 处理数码管显示内容
 * 
 * 设置数码管显示的数据，并调用函数进行显示更新
 */
void Display_TubeDataProcess(void)
{
    sDisplay_TubeData.tube0 = 0;
    sDisplay_TubeData.tube1 = 1;
    sDisplay_TubeData.tube2 = 2;
    sDisplay_TubeData.tube3 = 3;
    sDisplay_TubeData.tube4 = 4;
    sDisplay_TubeData.tube5 = 5;
    sDisplay_TubeData.tube6 = 6;
    sDisplay_TubeData.tube7 = 7;
    
    TM1638_TubeDisplay(sDisplay_TubeData);
}

/**
 * @brief 处理LED显示内容
 * 
 * 设置LED显示的数据，并调用函数进行显示更新
 */
void Display_LedDataProcess(void)
{
    uDisplay_LedData.byte = 0x01; // 最右侧LED8亮
    TM1638_LedDisplay(uDisplay_LedData.byte);
}


/**
 * @brief 键盘初始化函数
 * 
 * 该函数用于在程序启动时初始化键盘相关的变量和状态，确保键盘操作的正确性。
 */
void Key_Init(void)
{
    // 初始化键名，设置为NULL，表示当前没有按键被按下
    u8Key_Name = KEY_NULL;
    
    // 初始化键盘标志字节，清零所有标志位
    uKey_Flag.byte = 0;
    
    // 初始化键盘状态为释放状态，表示当前没有按键被按下
    u8KeyState = KEY_RELEASED;
    
    // 初始化键盘设置温度变量为0，该变量用于存储键盘的设置温度
    u32Key_SettingTemper = 0;
}

/**
 * @brief 按键扫描处理
 * 
 * 扫描按键输入，处理按键事件，包括进入和退出设置模式，以及调整设定温度
 */
void Key_ScanProcess(void)
{
    uint8_t u8KeyNum;
    u8KeyNum = TM1638_ReadKey(); // 获取键值
    if (u8KeyNum != 0) // 有键按下
    {
        u8KeyState = KEY_PRESSED;
        u8Key_Name = u8KeyNum; // 保存按键值
    } else
    {
        u8KeyState = KEY_RELEASED;
    }
    if (u8KeyState == KEY_RELEASED) // 按键释放
    {
        switch (u8Key_Name)
        {
            case KEY_SET: // 设置键
            {
                if (uKey_Flag.bt.bSetMode)
                {
                    uKey_Flag.bt.bSetMode = 0; // 退出设置模式
                } else
                {
                    uKey_Flag.bt.bSetMode = 1; // 进入设置模式
                }
            } break;
            case KEY_UP: // 向上键
            {
                if (uKey_Flag.bt.bSetMode) // 设置模式
                {
                    if (u32Key_SettingTemper < (80 * 100))
                    {
                        u32Key_SettingTemper += 10;
                    }
                }
            } break;
            case KEY_DOWN: // 向下键
            {
                if (uKey_Flag.bt.bSetMode) // 设置模式
                {
                    if (u32Key_SettingTemper > (30 * 100))
                    {
                        u32Key_SettingTemper -= 10;
                    }
                }
            } break;
            default: break;
        }
        u8Key_Name = KEY_NULL;
    }
}

