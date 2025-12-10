#include "beep.h"

extern TIM_HandleTypeDef htim3;

void Beep_Init(void)
{
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
}

void BeepOn(void)
{
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 369/2);
}

void BeepOff(void)
{
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);
}
