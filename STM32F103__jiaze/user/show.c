#include "show.h"
#include "string.h"

/*******************************************************************************
 * �ļ�˵������ʾ�Ͱ�������ģ��
 * 
 * ��Ҫ���ܣ�
 * 1. �������ʾ����
 * 2. LEDָʾ�ƿ���
 * 3. �������ʹ���
 * 4. �¶����ù���
*******************************************************************************/

/* ��̬�������� */
// ��ʾ��ر���
static TM1638Tube_ts sDisplay_TubeData;  // �������ʾ����
static LedData_tu uDisplay_LedData;      // LED��ʾ����

// ������ر���
static uint8_t u8Key_Name;              // ��ǰ����ֵ
static uint8_t u8KeyState;              // ��ǰ����״̬
static KeyFlag_tu uKey_Flag;            // �������ܱ�־
static uint32_t u32Key_SettingTemper;   // �¶��趨ֵ(ʵ��ֵ��100)

/**
 * @brief ��ʼ����ʾģ��
 * 
 * ��ʼ��TM1638Ӳ����������ʾ����������ʼ����ʾ����
 */
void Display_Init(void)
{
    // ��ʼ��TM1638Ӳ��
    TM1638_Init();
    
    // ������ʾ����
    TM1638_Switch(0);            // �ȹر���ʾ
    TM1638_SetBrightness(7);     // �����������
    TM1638_WriteCmd(0x44);       // ����Ϊ�̶���ַģʽ
    TM1638_Switch(1);            // ����ʾ
    
    // ��ʼ����ʾ����
    uDisplay_LedData.byte = 0;   // ���LED��ʾ
    memset(&sDisplay_TubeData, 0x00, sizeof(sDisplay_TubeData));  // ����������ʾ
}

/**
 * @brief �����������ʾ����
 * 
 * �����������ʾ�����ݣ������ú���������ʾ����
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
 * @brief ����LED��ʾ����
 * 
 * ����LED��ʾ�����ݣ������ú���������ʾ����
 */
void Display_LedDataProcess(void)
{
    uDisplay_LedData.byte = 0x01; // ���Ҳ�LED8��
    TM1638_LedDisplay(uDisplay_LedData.byte);
}


/**
 * @brief ���̳�ʼ������
 * 
 * �ú��������ڳ�������ʱ��ʼ��������صı�����״̬��ȷ�����̲�������ȷ�ԡ�
 */
void Key_Init(void)
{
    // ��ʼ������������ΪNULL����ʾ��ǰû�а���������
    u8Key_Name = KEY_NULL;
    
    // ��ʼ�����̱�־�ֽڣ��������б�־λ
    uKey_Flag.byte = 0;
    
    // ��ʼ������״̬Ϊ�ͷ�״̬����ʾ��ǰû�а���������
    u8KeyState = KEY_RELEASED;
    
    // ��ʼ�����������¶ȱ���Ϊ0���ñ������ڴ洢���̵������¶�
    u32Key_SettingTemper = 0;
}

/**
 * @brief ����ɨ�账��
 * 
 * ɨ�谴�����룬�������¼�������������˳�����ģʽ���Լ������趨�¶�
 */
void Key_ScanProcess(void)
{
    uint8_t u8KeyNum;
    u8KeyNum = TM1638_ReadKey(); // ��ȡ��ֵ
    if (u8KeyNum != 0) // �м�����
    {
        u8KeyState = KEY_PRESSED;
        u8Key_Name = u8KeyNum; // ���水��ֵ
    } else
    {
        u8KeyState = KEY_RELEASED;
    }
    if (u8KeyState == KEY_RELEASED) // �����ͷ�
    {
        switch (u8Key_Name)
        {
            case KEY_SET: // ���ü�
            {
                if (uKey_Flag.bt.bSetMode)
                {
                    uKey_Flag.bt.bSetMode = 0; // �˳�����ģʽ
                } else
                {
                    uKey_Flag.bt.bSetMode = 1; // ��������ģʽ
                }
            } break;
            case KEY_UP: // ���ϼ�
            {
                if (uKey_Flag.bt.bSetMode) // ����ģʽ
                {
                    if (u32Key_SettingTemper < (80 * 100))
                    {
                        u32Key_SettingTemper += 10;
                    }
                }
            } break;
            case KEY_DOWN: // ���¼�
            {
                if (uKey_Flag.bt.bSetMode) // ����ģʽ
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

