#ifndef APP_CSPSCOM_H
#define APP_CSPSCOM_H
#include "stm32f1xx_hal.h"

void CSPS_Set_Fan_Speed_Max(void);
void CSPS_Set_Fan_Stop(void);
void CSPS_Var_Refresh(void);

uint16_t CSPS_Get_PS_Status(void);
float CSPS_Get_Input_Voltage(void);
float CSPS_Get_Input_Current(void);
float CSPS_Get_Input_Power(void);
float CSPS_Get_Output_Voltage(void);
float CSPS_Get_Output_Current(void);
float CSPS_Get_Output_Power(void);
float CSPS_Get_Temp1(void);
float CSPS_Get_Temp2(void);

#endif //APP_CSPSCOM_H