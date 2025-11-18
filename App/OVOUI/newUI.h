#ifndef OVOUI_NEWUI_H
#define OVOUI_NEWUI_H
#include "stdint.h"

#define AHVAL_IDLE     0   // 空闲状态
#define AHVAL_RUNNING  1   // 运行中

typedef enum {
    easeNone        = 0,
    easeLinear      = 1,
    easeInQuad      = 2,
    easeOutQuad     = 3,
    easeInOutQuad   = 4,
    easeInCubic     = 5,
    easeOutCubic    = 6,
    easeInOutCubic  = 7,
    easeSmoothstep  = 8,
    easeOutElastic  = 9,
    easeOutElasticTest=10,
}EaseType;

//AnimationHandlingVariable
typedef struct {
    uint16_t currentTick;
    uint16_t maxTick;
    int16_t startValue;
    int16_t currentValue;
    int16_t endValue;
    float (*easingFunction)(const float t);
    uint8_t status;
}AHVal;

void Ease_SetFunc(EaseType type,AHVal *self);
EaseType Ease_GetFunc(AHVal *self);

void AHVal_Update(AHVal *self);
void AHVal_SetHard(AHVal *self, int16_t value);
void AHVal_SetSoftEnd(AHVal *self, int16_t endValue, uint16_t maxTick);
void AHVal_SetSoftRestart(AHVal *self, int16_t endValue, uint16_t maxTick);

#endif //OVOUI_NEWUI_H