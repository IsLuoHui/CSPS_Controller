#ifndef APP_CSPSCOM_H
#define APP_CSPSCOM_H
#include "stm32f1xx_hal.h"

void CSPS_Set_Fan_Speed_Max(void);
void CSPS_Set_Fan_Stop(void);
void CSPS_Var_Refresh(void);

#endif //APP_CSPSCOM_H