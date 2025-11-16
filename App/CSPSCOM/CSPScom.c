#include "CSPScom.h"
#include "uCI2C.h"

void CSPS_Set_Fan_Speed_Max(void) {
    uC_I2C_Write(0x3C, 0x4E20);
}

void CSPS_Set_Fan_Stop(void) {
    uint16_t val=0;
    uC_I2C_Read(0x3A, &val);
    val&=0xFFBF;
    uC_I2C_Write(0x3A, val);
}

