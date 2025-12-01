#include "CSPScom.h"

#include <stdio.h>
#include <string.h>
#include "uCI2C.h"
#include "oledui.h"

extern UART_HandleTypeDef huart1;

uint16_t PS_Status = 0;
float InputVoltage = 0;
float InputCurrent = 0;
float OutputVoltage = 0;
float OutputCurrent = 0;
float Temp1 = 0;
float Temp2 = 0;
uint16_t FanRPM = 0;

volatile uint8_t uCRefreshFlag = 0;

void CSPS_Set_Fan_Speed_Max(void)
{
    uC_I2C_Write(0x3C, 0x4E20);
}

void CSPS_Set_Fan_Stop(void)
{
    uint16_t val = 0;
    uC_I2C_Read(0x3A, &val);
    val &= 0xFFBF;
    uC_I2C_Write(0x3A, val);
}

uint16_t CSPS_Get_PS_Status(void)
{
    uint16_t var = 0;
    uC_I2C_Read(0x02, &var);
    return var;
}

uint16_t CSPS_Get_Input_Voltage(void)
{
    uint16_t var = 0;
    uC_I2C_Read(0x08, &var);
    return var;
}

uint16_t CSPS_Get_Input_Current(void)
{
    uint16_t var = 0;
    uC_I2C_Read(0x0A, &var);
    return var;
}

uint16_t CSPS_Get_Input_Power(void)
{
    uint16_t var = 0;
    uC_I2C_Read(0x0C, &var);
    return var;
}

uint16_t CSPS_Get_Output_Voltage(void)
{
    uint16_t var = 0;
    uC_I2C_Read(0x0E, &var);
    return var;
}

uint16_t CSPS_Get_Output_Current(void)
{
    uint16_t var = 0;
    uC_I2C_Read(0x10, &var);
    return var;
}

uint16_t CSPS_Get_Output_Power(void)
{
    uint16_t var = 0;
    uC_I2C_Read(0x12, &var);
    return var;
}

uint16_t CSPS_Get_Temp1(void)
{
    uint16_t var = 0;
    uC_I2C_Read(0x1A, &var);
    return var;
}

uint16_t CSPS_Get_Temp2(void)
{
    uint16_t var = 0;
    uC_I2C_Read(0x1C, &var);
    return var;
}

uint16_t CSPS_Get_Fan1_Speed(void)
{
    uint16_t var = 0;
    uC_I2C_Read(0x3C, &var);
    return var;
}

void CSPS_Var_Refresh(void)
{
    uint16_t iv = CSPS_Get_Input_Voltage();
    uint16_t ic = CSPS_Get_Input_Current();
    uint16_t ov = CSPS_Get_Output_Voltage();
    uint16_t oc = CSPS_Get_Output_Current();
    uint16_t t2 = CSPS_Get_Temp2();
    uint16_t fan = CSPS_Get_Fan1_Speed();

    PS_Status = CSPS_Get_PS_Status();
    InputVoltage = iv / 32.0f;
    InputCurrent = ic / 64.0f;
    OutputVoltage = ov / 256.0f;
    OutputCurrent = oc / 64.0f;
    Temp1 = CSPS_Get_Temp1() / 64.0f;
    Temp2 = t2 / 64.0f;
    FanRPM = fan;
    static char buf[128] = {0};
    sprintf(buf, "STAT:VIN=%u;IIN=%u;VOUT=%u;IOUT=%u;TEMP=%u;FAN=%u\r\n", iv, ic, ov, oc, t2, fan);
    HAL_UART_Transmit_DMA(&huart1, (uint8_t*)buf, strlen(buf));
}
