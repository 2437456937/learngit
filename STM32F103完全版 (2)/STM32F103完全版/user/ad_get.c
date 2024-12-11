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

    // ʹ��ADC1ͨ������ӦGPIO�˿ڵ�ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB |RCC_APB2Periph_ADC1,ENABLE );	  //ʹ��ADC1ͨ��ʱ��
 
    // ����ADC��Ƶ����Ϊ6��ȷ��ADCʱ�Ӳ�����14MHz
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);   
	
    // ����PB0,1��Ϊ����͹�����������	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		
    GPIO_Init(GPIOB, &GPIO_InitStructure);	

    // ����PC0,1��Ϊģ��ͨ���������ţ�����ˮλ����  PC4Ϊ�¶Ȳ���
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1| GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	
    GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
    // ��λADC1������Ĵ����ָ�ΪĬ��ֵ
    ADC_DeInit(ADC1);  

    // ����ADC1�Ĺ���ģʽ
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	// ����ģʽ
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;	// ��ͨ��ģʽ
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	// ����ת��ģʽ
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	// �������ת��
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	// �����Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel = 1;	// 1��ת��ͨ��
    ADC_Init(ADC1, &ADC_InitStructure);	// ��ʼ��ADC1�ļĴ���   

    // ʹ��ADC1
    ADC_Cmd(ADC1, ENABLE);	
    ADC_ResetCalibration(ADC1);	
    while(ADC_GetResetCalibrationStatus(ADC1));	
    // ��ʼADCУ׼
    ADC_StartCalibration(ADC1); 
    while(ADC_GetCalibrationStatus(ADC1));	
}

//���ADCֵ
uint16_t Get_Adc(uint8_t ch)   
{
  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_28Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������	
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������
	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}

//���ADCƽ��ֵ
/**
 * @brief ��ȡADCת��ֵ
 * 
 * �ú���ͨ����β�������ƽ��ֵ�ķ��������ADCת���ľ��ȡ���������8�ζ�ȡָ��ͨ����ADCֵ��
 * Ȼ����Щֵ��Ӻ�ͨ�����Ʋ��������ƽ��ֵ�������ƽ����һ���ض��ķ�Χ�ڡ�
 * 
 * @param ch uint8_t ���ͣ���ʾҪ��ȡ��ADCͨ����š�
 * @return uint16_t ���ؼ��㴦����ADCֵ����Χ�޶���0��1023֮�䡣
 */
uint16_t Ad_Get_Value(uint8_t ch)
{
    uint32_t temp_val=0; // ��ʼ��һ�������ͱ��������ۼ�ADC��ȡֵ
    uint8_t t; // ��ʼ��һ���޷����ַ��ͱ�������ѭ��������
    for(t=0;t<4;t++) // ѭ��8���Խ��ж�β���
    {
        temp_val+=Get_Adc(ch); // ÿ��ѭ���ۼ�һ��ADC��ȡֵ
        delay_ms(1); // ÿ�ζ�ȡADCֵ��ȴ�1���룬��ȷ������֮�����㹻��ʱ����
    }
    // ���ۼ�ֵ����5λ�����ƽ��ֵ����ͨ��λ��������ƽ����0��1023�ķ�Χ��
    return (temp_val >> 5) & 0x03FF;
}
 
//����¶�ֵ
/**
 * @brief ��ȡ�¶ȴ��������¶ȶ���
 * 
 * ������ͨ��ADCֵת���õ��¶ȴ��������¶ȶ����������Ȼ�ȡָ��ͨ����ADCֵ��
 * Ȼ�����ADCֵ�жϴ�������״̬����·����·����������������ADCֵ�����¶ȶ�����
 * 
 * @param ch ������ͨ���ţ�����ָ��Ҫ��ȡ���¶ȴ�����ͨ��
 * @param temp_value ָ��洢�¶ȶ����ı�����ָ�룬����ͨ���˲������ؼ���õ����¶�
 * @return uint16_t ���ش�����״̬���룬0x1000��ʾ��·��0x2000��ʾ��·��0��ʾ����
 */
uint16_t Ad_Get_Temp(uint8_t ch)
{
    uint16_t i = 0;
    uint16_t temp_adc_val;
    uint16_t temp_value = 0;  // �¶�ֵ
    
    // ��ȡָ��ͨ����ADCֵ����β���ȡƽ����
    temp_adc_val = Ad_Get_Value(ch);
    
    // ���ADCֵ�Ƿ����¶ȱ�Χ��
    if(temp_adc_val > temp[0])
    {
        return 0;  // �¶ȹ��ͣ�����0��
    }
    else if(temp_adc_val < temp[99])
    {
        return 99;  // �¶ȹ��ߣ�����99.0��
    }
    
    // ���¶ȱ�Χ�ڣ��������Բ�ֵ����
    for(i=0; i<99; i++)
    {
        if((temp_adc_val <= temp[i]) && (temp_adc_val > temp[i+1]))
        {
            // �ҵ��¶������������Բ�ֵ
            uint16_t delta_adc = temp[i] - temp[i+1];
            uint16_t delta_temp = temp_adc_val - temp[i+1];
            
            // �����¶�ֵ�������¶� + ��ֵ����
            // �¶ȷֱ���Ϊ0.1�棬���Գ���10
            temp_value = (i + 1) * 1 + (delta_temp * 1) / delta_adc-31;
            return temp_value;
        }
    }
    
    // ���ִ�е����˵���������������
    return 0;  // ����0��
}

// ��ȡ��ˮλADCֵ
uint16_t GET_WATER_LOW(void)
{
    // ����ADCת��
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    // �ȴ�ת�����
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    // ����ת�����
    return ADC_GetConversionValue(ADC1);
}

// ��ȡ��ˮλADCֵ
uint16_t GET_WATER_HIGH(void)
{
    // ����ADCת��
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    // �ȴ�ת�����
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    // ����ת�����
    return ADC_GetConversionValue(ADC1);
}


// ˮλADC��ʼ��
void ADC_Water_Init(void)
{
    // ����Ad_Get_Init�Ѿ�����˴󲿷ֳ�ʼ������
    // ����ֻ��Ҫ����ˮλ�����ص�ͨ��
    
    // ����PC0(��ˮλ)ΪADCͨ��10
    ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_239Cycles5);
    
    // ����PC1(��ˮλ)ΪADCͨ��11
    ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_239Cycles5);
    
    // ���û���ADC��ʼ��
    Ad_Get_Init();
}

// �¶�ADC��ʼ��
void ADC_Temp_Init(void)
{
    // PC4����ΪADCͨ��14
    ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 1, ADC_SampleTime_239Cycles5);
}




