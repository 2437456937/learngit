/*******************************************************************************
  * 文件：TM1638.c
  * 作者：whttps://blog.csdn.net/wanglong3713
  * 版本：v1.0
  * 日期：2021-11-20
  * 说明：适用于TM1638驱动的8个数码管、8个LED、8个按键的显示板
			○		○		○		○		○		○		○		○
			LED1	LED2	LED3	LED4	LED5	LED6	LED7	LED8
			
			TUBE1	TUBE2	TUBE3	TUBE4	TUBE5	TUBE6	TUBE7	TUBE8
			
			KEY1	KEY2	KEY3	KEY4	KEY5	KEY6	KEY7	KEY8	
  显示板按键顺序实际不一致：
			S1		S5		S2		S6		S3		S7		S4		S8	
*******************************************************************************/
#include "tm1638.h"
// 数码管段码表 - 共阴极数码管
const uint8_t TM1638_TubeNumTab[] =
{
    // 数字0-F的段码值(MSB-LSB: DP-G-F-E-D-C-B-A)
    //0    1    2    3    4    5    6    7    8    9    A    b    C    d    E    F    
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
    // 带小数点的数字0.-9.的段码值
    //0.   1.   2.   3.   4.   5.   6.   7.   8.   9.  空白  L    H
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x00,0x38,0x76
};

// 数码管地址表 - 从左到右SEG1-SEG8对应的地址
const uint8_t TM1638_TubeAddrTab[] =
{
    0xC0,0xC2,0xC4,0xC6,0xC8,0xCA,0xCC,0xCE  // 固定地址模式下的显示位置
};

// LED地址表 - 从左到右LED1-LED8对应的地址
const uint8_t TM1638_LedAddrTab[] =
{
    0xC1,0xC3,0xC5,0xC7,0xC9,0xCB,0xCD,0xCF  // LED控制位置
};

/*******************************************************************************
 * 函数名：TM1638_WriteData
 * 功能：向TM1638写入一个字节数据
 * 参数：u8Data - 要写入的数据
 * 工作原理：
 * 1. 设置DIO为输出模式
 * 2. 8位数据逐位发送：
 *    - CLK拉低
 *    - 设置DIO电平(0/1)
 *    - CLK拉高，数据在上升沿被锁存
 *    - 数据右移，准备发送下一位
*******************************************************************************/
void TM1638_WriteData(uint8_t u8Data)
{
    uint8_t i;
    TM1638_DIOModeOutput();  // 配置DIO为输出模式
    for(i = 0; i < 8; i++)   // 逐位发送，LSB先发
    {
        TM1638_CLKReset();   // 时钟低
        (u8Data & 0x01) ? (TM1638_DIOSet()) : (TM1638_DIOReset());  // 设置数据位
        u8Data >>= 1;        // 准备下一位                      
        TM1638_CLKSet();     // 时钟高，锁存数据
    }
}


/**
 * 向TM1638设备写入命令
 * 此函数的目的是通过设置和重置片选信号（STB）来确保命令被正确接收和处理
 * 它首先拉低STB信号线以准备设备接收命令，然后写入命令数据，最后拉高STB信号线以结束命令传输
 * 
 * @param u8Cmd 要写入的命令数据，作为一个8位无符号整数传递
 */
void TM1638_WriteCmd(uint8_t u8Cmd)
{
    // 准备设备接收命令，通过重置STB信号线
    TM1638_STBReset();
    
    // 写入命令数据到TM1638设备
    TM1638_WriteData(u8Cmd);
    
    // 结束命令传输，通过设置STB信号线
    TM1638_STBSet();
}


/*******************************************************************************
 * 函数名：TM1638_ReadData
 * 功能：从TM1638读取一个字节数据
 * 返回值：读取到的数据
 * 工作原理：
 * 1. 设置DIO为输入模式
 * 2. 8位数据逐位接收：
 *    - CLK拉低
 *    - 读取DIO电平状态
 *    - CLK拉高，准备读取下一位
 *    - 数据右移，腾出位置给下一位
*******************************************************************************/
uint8_t TM1638_ReadData(void)
{
    uint8_t i;
    uint8_t temp = 0;
    TM1638_DIOModeInput();   // 配置DIO为输入模式
    for(i = 0; i < 8; i++)   // 逐位读取，LSB先读
    {
        temp >>= 1;          // 为下一位腾出空间
        TM1638_CLKReset();   // 时钟低
        if(TM1638_DIORead() == 1)  // 读取数据位
        {
            temp |= 0x80;    // 如果是1，设置最高位
        }
        TM1638_CLKSet();     // 时钟高，准备读取下一位
    }
    return temp;
}

/*******************************************************************************
 * 函数名：TM1638_ReadKey
 * 功能：读取按键状态
 * 返回值：按键编号(1-8)，0表示无按键
 * 工作原理：
 * 1. 发送读按键命令(0x42)
 * 2. 读取4个字节的按键数据
 * 3. 分析数据得到按键编号：
 *    - 每个字节的bit0对应奇数键(1,3,5,7)
 *    - 每个字节的bit4对应偶数键(2,4,6,8)
*******************************************************************************/
uint8_t TM1638_ReadKey(void)
{
    uint8_t u8Data[4], i;
    uint8_t u8Key = 0;
    TM1638_STBReset();
    TM1638_WriteData(0x42);
    for (i = 0; i < 4; i++)
    {
        u8Data[i] = TM1638_ReadData();//读BYTE1~BYTE4的数据
    }
    TM1638_STBSet();
    for (i = 0; i < 4; i++)//取出4个BYTE中的键值
    {
        if (((u8Data[i] >> 0) & 0x01) == 0x01)//第0位，B0,对应按键为1、3、5、7，计算公式是i*2+1
        {
            u8Key = (i * 2 + 1);
            break;
        }else
        {
            if (((u8Data[i] >> 4) & 0x01) == 0x01)//第4位，B4,对应按键为2、4、6、8，计算公式是i*2+2
            {
                u8Key = (i * 2 + 2);
                break;
            }
        }
    }
    return u8Key;
}

/*******************************************************************************
  * 函数名：TM1638_LedDisplay
  * 功  能：8个LED显示
  * 参  数：u8Data每一位代表一个数码管，1亮0灭  
  * 返回值：无
  * 说  明：u8Data最高位为最左侧数码管LED1  
*******************************************************************************/
void TM1638_LedDisplay(uint8_t u8Data)
{
	uint8_t temp[8],i;	
	for(i = 0; i < 8; i++)
	{
		temp[i] = ((u8Data >> (7 - i)) & 0x01);
		TM1638_STBReset();
		TM1638_WriteData(TM1638_LedAddrTab[i]);//显示地址
		TM1638_WriteData(temp[i]);//显示数据	
		TM1638_STBSet();
	}	
}
/*******************************************************************************
  * 函数名：TM1638_TubeDisplay
  * 功  能：8个数码管显示
  * 参  数：sData显示数据结构体
  * 返回值：无
  * 说  明：共阴极数码管,数据为从左至右顺序
*******************************************************************************/
void TM1638_TubeDisplay(TM1638Tube_ts sData)
{
	uint8_t temp[8],i;
	temp[0] = TM1638_TubeNumTab[sData.tube0];
	temp[1] = TM1638_TubeNumTab[sData.tube1];
	temp[2] = TM1638_TubeNumTab[sData.tube2];
	temp[3] = TM1638_TubeNumTab[sData.tube3];
	temp[4] = TM1638_TubeNumTab[sData.tube4];
	temp[5] = TM1638_TubeNumTab[sData.tube5];
	temp[6] = TM1638_TubeNumTab[sData.tube6];
	temp[7] = TM1638_TubeNumTab[sData.tube7];
	
	for(i = 0; i < 8; i++)
	{
		TM1638_STBReset();
		TM1638_WriteData(TM1638_TubeAddrTab[i]);//地址
		TM1638_WriteData(temp[i]);//数据	
		TM1638_STBSet();
	}	
}
/*******************************************************************************
  * 函数名：TM1638_SetBrightness
  * 功  能：设置亮度
  * 参  数：u8Brt亮度，共8级，0~7
  * 返回值：无
  * 说  明：0x88为开显示
*******************************************************************************/
void TM1638_SetBrightness(uint8_t u8Brt)
{
	TM1638_WriteCmd(0x88 | u8Brt);
}
/*******************************************************************************
  * 函数名：TM1638_Switch
  * 功  能：显示开关
  * 参  数：0关，1开
  * 返回值：无
  * 说  明：0x88为开显示
*******************************************************************************/
void TM1638_Switch(uint8_t bState)
{
	bState ? TM1638_WriteCmd(0x88) : TM1638_WriteCmd(0x80);
}

/*******************************************************************************
  * 函数名：TM1638_Init
  * 功能: 初始化TM1638相关的GPIO引脚
  * 参数: 无
  * 返回值: 无
  * 说明: 
  *   1. 配置STB/CLK/DIO三个引脚为推挽输出
  *   2. 初始化引脚默认电平
  *   3. 需要在系统GPIO初始化时调用
*******************************************************************************/
void TM1638_Init(void)
{
    // 使能GPIOD时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStruct;
    
    // 配置STB引脚PD12 - 片选信号
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; // 50MHz速度
    GPIO_Init(GPIOD, &GPIO_InitStruct);
    
    // 配置CLK引脚PD11 - 时钟信号
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
    GPIO_Init(GPIOD, &GPIO_InitStruct);
    
    // 配置DIO引脚PD10 - 数据信号
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOD, &GPIO_InitStruct);
    
    // 初始化引脚默认电平为高
    TM1638_STBSet();   // STB=1
    TM1638_CLKSet();   // CLK=1 
    TM1638_DIOSet();   // DIO=1
}

/***********************************END OF FILE********************************/
