/* Includes ------------------------------------------------------------------*/
#include "ad_get.h"
#include "delay.h"

const u16 temp[100] = {751,742,733,724,715,706,697,687,678,668,
					   659,649,639,629,620,610,600,590,580,570,
					   561,551,541,531,522,512,502,493,483,474,
					   465,456,446,437,428,420,411,402,394,385,
					   377,369,361,353,345,337,330,322,315,308,
					   301,294,287,281,274,268,261,255,249,244,
					   238,232,227,221,216,211,206,201,196,191,
					   187,182,178,174,170,166,162,158,154,150,
					   147,143,140,137,133,130,127,124,121,118,
					   115,113,110,108,105,102,100,98,96,93};


void Ad_Get_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure; 
    GPIO_InitTypeDef GPIO_InitStructure;

    // 使能ADC1通道及相应GPIO端口的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB |RCC_APB2Periph_ADC1,ENABLE );	  //使能ADC1通道时钟
 
    // 设置ADC分频因子为6，确保ADC时钟不超过14MHz
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);   
	
    // 配置PB0,1作为掉电和过流采样引脚	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		
    GPIO_Init(GPIOB, &GPIO_InitStructure);	

    // 配置PC0,1作为模拟通道输入引脚，用于水位采样  PC4为温度采样
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1| GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	
    GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
    // 复位ADC1，将其寄存器恢复为默认值
    ADC_DeInit(ADC1);  

    // 配置ADC1的工作模式
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	// 独立模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;	// 单通道模式
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	// 单次转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	// 软件触发转换
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	// 数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 1;	// 1个转换通道
    ADC_Init(ADC1, &ADC_InitStructure);	// 初始化ADC1的寄存器   

    // 使能ADC1
    ADC_Cmd(ADC1, ENABLE);	
    ADC_ResetCalibration(ADC1);	
    while(ADC_GetResetCalibrationStatus(ADC1));	
    // 开始ADC校准
    ADC_StartCalibration(ADC1); 
    while(ADC_GetCalibrationStatus(ADC1));	
}

//获得ADC值
uint16_t Get_Adc(uint8_t ch)   
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_28Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束
	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}

//获得ADC平均值
/**
 * @brief 获取ADC转换值
 * 
 * 该函数通过多次采样并求平均值的方法来提高ADC转换的精度。它会连续8次读取指定通道的ADC值，
 * 然后将这些值相加后通过右移操作来求得平均值，并限制结果在一个特定的范围内。
 * 
 * @param ch uint8_t 类型，表示要读取的ADC通道编号。
 * @return uint16_t 返回计算处理后的ADC值，范围限定在0到1023之间。
 */
uint16_t Ad_Get_Value(uint8_t ch)
{
    uint32_t temp_val=0; // 初始化一个长整型变量用于累加ADC读取值
    uint8_t t; // 初始化一个无符号字符型变量用作循环计数器
    for(t=0;t<4;t++) // 循环8次以进行多次采样
    {
        temp_val+=Get_Adc(ch); // 每次循环累加一次ADC读取值
        delay_ms(1); // 每次读取ADC值后等待1毫秒，以确保采样之间有足够的时间间隔
    }
    // 将累加值右移5位以求得平均值，并通过位与操作限制结果在0到1023的范围内
    return (temp_val >> 5) & 0x03FF;
}
 
//获得温度值
/**
 * @brief 获取温度传感器的温度读数
 * 
 * 本函数通过ADC值转换得到温度传感器的温度读数。它首先获取指定通道的ADC值，
 * 然后根据ADC值判断传感器的状态（短路、开路或正常），并根据ADC值计算温度读数。
 * 
 * @param ch 传感器通道号，用于指定要读取的温度传感器通道
 * @param temp_value 指向存储温度读数的变量的指针，函数通过此参数返回计算得到的温度
 * @return uint16_t 返回传感器状态代码，0x1000表示短路，0x2000表示开路，0表示正常
 */
uint16_t Ad_Get_Temp(uint8_t ch)
{
    uint16_t i = 0;
    uint16_t temp_adc_val;
    uint16_t temp_value = 0;  // 温度值
    
    // 获取指定通道的ADC值（多次采样取平均）
    temp_adc_val = Ad_Get_Value(ch);
    
    // 检查ADC值是否在温度表范围内
    if(temp_adc_val > temp[0])
    {
        return 0;  // 温度过低，返回0℃
    }
    else if(temp_adc_val < temp[99])
    {
        return 99;  // 温度过高，返回99.0℃
    }
    
    // 在温度表范围内，进行线性插值计算
    for(i=0; i<99; i++)
    {
        if((temp_adc_val <= temp[i]) && (temp_adc_val > temp[i+1]))
        {
            // 找到温度区间后进行线性插值
            uint16_t delta_adc = temp[i] - temp[i+1];
            uint16_t delta_temp = temp_adc_val - temp[i+1];
            
            // 计算温度值：基础温度 + 插值部分
            // 温度分辨率为0.1℃，所以乘以10
            temp_value = (i + 1) * 1 + (delta_temp * 1) / delta_adc-31;
            return temp_value;
        }
    }
    
    // 如果执行到这里，说明出现了意外情况
    return 0;  // 返回0℃
}

// 获取低水位ADC值
uint16_t GET_WATER_LOW(void)
{
    // 启动ADC转换
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    // 等待转换完成
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    // 返回转换结果
    return ADC_GetConversionValue(ADC1);
}

// 获取高水位ADC值
uint16_t GET_WATER_HIGH(void)
{
    // 启动ADC转换
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    // 等待转换完成
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    // 返回转换结果
    return ADC_GetConversionValue(ADC1);
}


// 水位ADC初始化
void ADC_Water_Init(void)
{
    // 由于Ad_Get_Init已经完成了大部分初始化工作
    // 这里只需要配置水位检测相关的通道
    
    // 配置PC0(低水位)为ADC通道10
    ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_239Cycles5);
    
    // 配置PC1(高水位)为ADC通道11
    ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_239Cycles5);
    
    // 调用基础ADC初始化
    Ad_Get_Init();
}

// 温度ADC初始化
void ADC_Temp_Init(void)
{
    // PC4配置为ADC通道14
    ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 1, ADC_SampleTime_239Cycles5);
}




