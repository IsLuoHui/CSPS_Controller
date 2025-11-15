#include "timRefresh.h"
#include "key.h"

extern TIM_HandleTypeDef htim1;
KeyStatus key1s,key2s,key3s= KeyIdle;

void TimRefresh_Init(void) {
    HAL_TIM_Base_Start_IT(&htim1);
}

void OLED_Layout_Refresh(void) {
    key1s=0;
    key2s=0;
    key3s=0;
}

void Key_Scan_Refresh(void) {
    key1s = Key1_Process();
    key2s = Key2_Process();
    key3s = Key3_Process();
}