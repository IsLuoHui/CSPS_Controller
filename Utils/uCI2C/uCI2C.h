#ifndef Utils_UCI2C_H
#define Utils_UCI2C_H
#include "stm32f1xx_hal.h"

#pragma region //All uC Register Address
#define Reg_uCInfo_Addr         0x00 //RO
#define Reg_PSStatus_Addr       0x02 //RO
#define Reg_ShutdownEvent_Addr  0x04 //RO
#define Reg_WaringEvent_Addr    0x06 //RO
#define Reg_InputVoltage_Addr   0x08 //RO
#define Reg_InputCurrent_Addr   0x0A //RO
#define Reg_InputPower_Addr     0x0C //RO
#define Reg_OutputVoltage_Addr  0x0E //RO
#define Reg_OutputCurrent_Addr  0x10 //RO
#define Reg_OutputPower_Addr    0x12 //RO
//Reserved Reg_AuxVoltage_Addr 0x14
//Reserved Reg_AuxCurrent_Addr 0x16
//Reserved Reg_AuxPower_Addr 0x18
#define Reg_InletTemperature_Addr       0x1A //RO
#define Reg_InternalTemperature_Addr    0x1C //RO
#define Reg_Fan1Speed_Addr              0x1E //RO
//Reserved Reg_Fan2Speed_Addr 0x20
#define Reg_InletTempMonitor_Addr       0x22 //RO
#define Reg_InternalTemp1Monitor_Addr   0x24 //RO
#define Reg_InternalTemp2Monitor_Addr   0x26 //RO
#define Reg_Fan1Monitor_Addr            0x28 //RO
#define Reg_Capability_Addr             0x2A //RO
#define Reg_AverageInputPowerAccumulatorLSW_Addr        0x2C //RO
#define Reg_AverageInputPowerAccumulatorMSW_Addr        0x2E //RO
#define Reg_AverageInputPowerCount_Addr                 0x30 //RW
#define Reg_MaximumHalfSecondAverageInputPower_Addr     0x32 //RW
#define Reg_MaximumHalfSecondAverageInputCurrent_Addr   0x34 //RW
#define Reg_PeakOutputCurrent_Addr                      0x36 //RW
//Reserved Reg_PeakAuxCurrent_Addr 0x38
#define Reg_PSControl_Addr      0x3A //RW
#define Reg_Fan1Control_Addr    0x3C //RW
//Reserved Reg_Fan2Control_Addr 0x3E
#define Reg_Fan1MinSpeed_Addr   0x40 //RW
//Reserved Reg_Fan2MinSpeed_Addr 0x42
#define Reg_InputVoltageMinWarning_Addr     0x44 //RW
#define Reg_InputVoltageMaxWarning_Addr     0x46 //RW
#define Reg_OutputVoltageMinWarning_Addr    0x48 //RW
#define Reg_OutputVoltageMaxWarning_Addr    0x4A //RW
//Reserved Reg_AuxVoltageMinWarning_Addr 0x4C
//Reserved Reg_AuxVoltageMaxWarning_Addr 0x4E
#define Reg_InletTemperatureWarning_Addr    0x50 //RW
#define Reg_InternalTemperatureWarning_Addr 0x52 //RW
#define Reg_EventLogWrite_Addr          0x54 //RW
#define Reg_EventLogRead_Addr           0x56 //RW
#define Reg_OutputVoltageSet_Addr       0x58 //RW
#define Reg_OverCurrentSetPoint1_Addr   0x5A //RW
#define Reg_OverCurrentSetPoint2_Addr   0x5C //RW
#define Reg_CommandOperation_Addr       0x5E //RO
#define Reg_ImageStatus_Addr            0x60 //RO
#define Reg_MainImageRevision_Addr      0x62 //RO
#define Reg_MainImageChecksum_Addr      0x64 //RO
#define Reg_StagedImageRevision_Addr    0x66 //RO
#define Reg_StagedImageChecksum_Addr    0x68 //RO
//Reserved 0x6A-0xEE
#define Reg_BlackboxReadRegister0_Addr  0xF0 //RO
#define Reg_BlackboxReadRegister1_Addr  0xF2 //RO
#define Reg_BlackboxReadRegister2_Addr  0xF4 //RO
#define Reg_BlackboxReadRegister3_Addr  0xF6 //RO
#define Reg_BlackboxReadRegister4_Addr  0xF8 //RO
#define Reg_BlackboxReadRegister5_Addr  0xFA //RO
#define Reg_BlackboxReadRegister6_Addr  0xFC //RO
#define Reg_BlackboxReadRegister7_Addr  0xFE //RO
#pragma endregion

HAL_StatusTypeDef uC_I2C_Read(uint8_t reg_addr, uint16_t *word);
HAL_StatusTypeDef uC_I2C_Write(uint8_t reg_addr,uint16_t word);

#endif //Utils_UCI2C_H