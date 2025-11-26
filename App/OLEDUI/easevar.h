#ifndef EASEVAR_H
#define EASEVAR_H
#include "stdint.h"

#define EASEVAR_IDLE     0   // 空闲状态
#define EASEVAR_RUNNING  1   // 运行中

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

typedef struct {
    uint16_t currentTick;
    uint16_t maxTick;
    int16_t startValue;
    int16_t currentValue;
    int16_t endValue;
    float (*easingFunction)(const float t);
    uint8_t status;
}EaseVar;


float easeLinear_(float t) ;
float easeInQuad_(float t);
float easeOutQuad_(float t);
float easeInOutQuad_(float t);
float easeInCubic_(float t);
float easeOutCubic_(float t);
float easeInOutCubic_(float t);
float easeSmoothstep_(float t);
float easeOutElastic_(float t);
float easeOutElasticTest_(float t,float frequency,float decay);

void Ease_SetFunc(EaseType type,EaseVar *self);
EaseType Ease_GetFunc(EaseVar *self);

void EaseVar_Update(EaseVar *self);
void EaseVar_SetHard(EaseVar *self, int16_t value);
void EaseVar_SetHardEnd(EaseVar *self, int16_t endValue, uint16_t maxTick);
void EaseVar_SetHardRestart(EaseVar *self, int16_t endValue, uint16_t maxTick);
void EaseVar_SetSoftRestart(EaseVar *self, int16_t endValue);

#endif //EASEVAR_H