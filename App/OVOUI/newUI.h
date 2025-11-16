#ifndef OVOUI_NEWUI_H
#define OVOUI_NEWUI_H
#include "stm32f1xx_hal.h"

#define AHVAL_IDLE     0   // 空闲状态
#define AHVAL_RUNNING  1   // 运行中

//AnimationHandlingVariable
typedef struct {
    uint16_t currentTick;
    uint16_t maxTick;
    float (*easingFunction)(const float t);
    int16_t startValue;
    int16_t currentValue;
    int16_t endValue;
    uint8_t status;
}AHVal;

void AHVal_Update(AHVal *self);
void AHVal_SetHard(AHVal *self, int16_t value);
void AHVal_SetSoftEnd(AHVal *self, int16_t endValue, uint16_t maxTick);
void AHVal_SetSoftRestart(AHVal *self, int16_t endValue, uint16_t maxTick);

#endif //OVOUI_NEWUI_H