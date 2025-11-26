#include "CSPScom.h"
#include "uCI2C.h"
#include "oledui.h"
#include <stdio.h>

extern uint16_t InputVoltage;
extern uint16_t InputCurrent;
extern uint16_t OutputVoltage;
extern uint16_t OutputCurrent;

volatile uint8_t doRefresh =0 ;

void CSPS_Set_Fan_Speed_Max(void) {
    uC_I2C_Write(0x3C, 0x4E20);
}

void CSPS_Set_Fan_Stop(void) {
    uint16_t val = 0;
    uC_I2C_Read(0x3A, &val);
    val &= 0xFFBF;
    uC_I2C_Write(0x3A, val);
}

void CSPS_Var_Refresh(void) {

    uint16_t temp =0;
    uC_I2C_Read(0x08, &temp);
    InputVoltage=temp;
    uC_I2C_Read(0x0A, &temp);
    InputCurrent=temp;
    uC_I2C_Read(0x0E, &temp);
    OutputVoltage=temp;
    uC_I2C_Read(0x10, &temp);
    OutputCurrent=temp;



    //Text_Refresh();
}
