#include "CSPScom.h"
#include "uCI2C.h"
#include "oledui.h"


uint16_t PS_Status = 0;
float InputVoltage = 0;
float InputCurrent = 0;
float OutputVoltage = 0;
float OutputCurrent = 0;
float Temp1 = 0;
float Temp2 = 0;

volatile uint8_t doRefresh = 0;

void CSPS_Set_Fan_Speed_Max(void) {
    uC_I2C_Write(0x3C, 0x4E20);
}

void CSPS_Set_Fan_Stop(void) {
    uint16_t val = 0;
    uC_I2C_Read(0x3A, &val);
    val &= 0xFFBF;
    uC_I2C_Write(0x3A, val);
}

uint16_t CSPS_Get_PS_Status(void) {
    uint16_t var = 0;
    uC_I2C_Read(0x02, &var);
    return var;
}

float CSPS_Get_Input_Voltage(void) {
    uint16_t var = 0;
    uC_I2C_Read(0x08, &var);
    return var / 32.0f;
}

float CSPS_Get_Input_Current(void) {
    uint16_t var = 0;
    uC_I2C_Read(0x0A, &var);
    return var / 64.0f;
}

float CSPS_Get_Input_Power(void) {
    uint16_t var = 0;
    uC_I2C_Read(0x0C, &var);
    return var / 0.0f;
}

float CSPS_Get_Output_Voltage(void) {
    uint16_t var = 0;
    uC_I2C_Read(0x0E, &var);
    return var / 256.0f;
}

float CSPS_Get_Output_Current(void) {
    uint16_t var = 0;
    uC_I2C_Read(0x10, &var);
    return var / 64.0f;
}

float CSPS_Get_Output_Power(void) {
    uint16_t var = 0;
    uC_I2C_Read(0x12, &var);
    return var / 0.0f;
}

float CSPS_Get_Temp1(void) {
    uint16_t var = 0;
    uC_I2C_Read(0x1A, &var);
    return var / 64.0f;
}

float CSPS_Get_Temp2(void) {
    uint16_t var = 0;
    uC_I2C_Read(0x1C, &var);
    return var / 64.0f;
}


void CSPS_Var_Refresh(void) {
    /*
    uint16_t temp =0;
    uC_I2C_Read(0x08, &temp);
    InputVoltage=temp;
    uC_I2C_Read(0x0A, &temp);
    InputCurrent=temp;
    uC_I2C_Read(0x0E, &temp);
    OutputVoltage=temp;
    uC_I2C_Read(0x10, &temp);
    OutputCurrent=temp;
    */
    PS_Status = CSPS_Get_PS_Status();
    InputVoltage = CSPS_Get_Input_Voltage();
    InputCurrent = CSPS_Get_Input_Current();
    OutputVoltage = CSPS_Get_Output_Voltage();
    OutputCurrent = CSPS_Get_Output_Current();
    Temp1 = CSPS_Get_Temp1();
    Temp2 = CSPS_Get_Temp2();
    //Text_Refresh();
}
