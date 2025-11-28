#include "easevar.h"
#include <stddef.h>
#include <math.h>

#pragma region // 缓动函数实现
// 线性 (Linear) 匀速
float easeLinear_(const float t) {
    return t;
}

// 二次缓入 (Ease In Quad) —— 开始慢，后面加速
float easeInQuad_(const float t) {
    return t * t;
}

// 二次缓出 (Ease Out Quad) —— 开始快，后面减速
float easeOutQuad_(const float t) {
    return t * (2.0f - t);
}

// 二次缓入缓出 (Ease InOut Quad) —— 前后慢，中间快
float easeInOutQuad_(const float t) {
    if (t < 0.5f) return 2.0f * t * t;
    return -1.0f + (4.0f - 2.0f * t) * t;
}

// 三次缓入 (Ease In Cubic) ——更明显的加速
float easeInCubic_(const float t) {
    return t * t * t;
}

// 三次缓出 (Ease Out Cubic) ——更明显的减速
float easeOutCubic_(const float t) {
    float u = t - 1.0f;
    return u * u * u + 1.0f;
}

// 三次缓入缓出 (Ease InOut Cubic)
float easeInOutCubic_(const float t) {
    if (t < 0.5f) return 4.0f * t * t * t;
    float u = (2.0f * t - 2.0f);
    return 0.5f * u * u * u + 1.0f;
}

// 平滑插值 (Smoothstep) ——比线性更自然
float easeSmoothstep_(const float t) {
    float u = (t < 0.0f) ? 0.0f : (t > 1.0f ? 1.0f : t);
    return u * u * (3.0f - 2.0f * u);
}

// 弹性缓出 (Ease Out Elastic) ——带回弹效果
float easeOutElastic_(const float t) {
    if (t == 0.0f) return 0.0f;
    if (t == 1.0f) return 1.0f;
    return powf(2.0f, -10.0f * t) * sinf((t - 0.075f) * (2.0f * M_PI) / 0.3f) + 1.0f;
}

//frequency → 控制回弹次数
//decay → 控制衰减速度（幅度大小）
float easeOutElasticTest_(const float t, const float frequency, const float decay) {
    if (t == 0.0f) return 0.0f;
    if (t == 1.0f) return 1.0f;
    return powf(2.0f, -decay * t) * sinf(frequency * t * M_PI) + 1.0f;
}
#pragma endregion

void Ease_SetFunc(EaseType type,EaseVar *self){
    switch (type) {
        case easeNone:
            self->easingFunction = NULL;
            break;
        case easeLinear:
            self->easingFunction = easeLinear_;
            break;
        case easeInQuad:
            self->easingFunction = easeInQuad_;
            break;
        case easeOutQuad:
            self->easingFunction = easeOutQuad_;
            break;
        case easeInOutQuad:
            self->easingFunction = easeInOutQuad_;
            break;
        case easeInCubic:
            self->easingFunction = easeInCubic_;
            break;
        case easeOutCubic:
            self->easingFunction = easeOutCubic_;
            break;
        case easeInOutCubic:
            self->easingFunction = easeInOutCubic_;
            break;
        case easeSmoothstep:
            self->easingFunction = easeSmoothstep_;
            break;
        case easeOutElastic:
            self->easingFunction = easeOutElastic_;
            break;
        default:
            break;
    }
}

EaseType Ease_GetFunc(EaseVar *self) {
    if(self->easingFunction) {
        if (self->easingFunction == easeLinear_) return easeLinear;
        if (self->easingFunction == easeInQuad_) return easeInQuad;
        if (self->easingFunction == easeOutQuad_) return easeOutQuad;
        if (self->easingFunction == easeInOutQuad_) return easeInOutQuad;
        if (self->easingFunction == easeInCubic_) return easeInCubic;
        if (self->easingFunction == easeOutCubic_) return easeOutCubic;
        if (self->easingFunction == easeInOutCubic_) return easeInOutCubic;
        if (self->easingFunction == easeSmoothstep_) return easeSmoothstep;
        if (self->easingFunction == easeOutElastic_) return easeOutElastic;
        return easeNone; // 未知函数
    }
    return easeNone; // 未设置函数
}

void EaseVar_Update(EaseVar *self) {
    if (self == NULL) return;

    // 如果当前是空闲状态，不需要更新
    if (self->status == EASEVAR_IDLE) {
        return;
    }

    // 如果已经到达最大tick，说明动画完成
    if (self->currentTick >= self->maxTick) {
        self->currentValue = self->endValue;
        self->status = EASEVAR_IDLE; // 切换为空闲状态
        return;
    }

    // 归一化时间 [0,1]
    const float t = (float) self->currentTick / (float) self->maxTick;

    // 缓动进度
    float p = 0.0f;
    if (self->easingFunction) {
        p = self->easingFunction(t);
    } else {
        p = t; // 默认使用线性
    }

    // 插值计算当前值
    self->currentValue = (int16_t) (self->startValue + (self->endValue - self->startValue) * p);

    // tick递增
    self->currentTick++;
}

void EaseVar_SetHard(EaseVar *self, int16_t value) {
    if (self == NULL) return;
    self->currentValue = value;
    self->startValue = value;
    self->endValue = value;
    self->currentTick = 0;
    self->status = EASEVAR_IDLE;
}

void EaseVar_SetHardEnd(EaseVar *self, int16_t endValue, uint16_t maxTick) {
    if (self == NULL) return;
    self->endValue = endValue;
    self->maxTick = maxTick;
    self->currentTick = 0;

    if (self->status == EASEVAR_IDLE) {
        // 空闲时，起点就是当前值
        self->startValue = self->currentValue;
        self->status = EASEVAR_RUNNING;
    }
    // 如果正在运行，只更新目标值，不改变起点
}

void EaseVar_SetHardRestart(EaseVar *self, int16_t endValue, uint16_t maxTick) {
    if (self == NULL) return;
    self->startValue = self->currentValue; // 当前值作为新的起点
    self->endValue = endValue;
    self->maxTick = maxTick;
    self->currentTick = 0;
    self->status = EASEVAR_RUNNING;
}

void EaseVar_SetSoftRestart(EaseVar *self, int16_t endValue) {
    if (self == NULL) return;

    // 剩余 tick = 总 tick - 已经走过的 tick
    uint16_t remainingTick = 0;
    if (self->maxTick > self->currentTick) {
        remainingTick = self->maxTick - self->currentTick;
    }

    // 用当前位置作为新的起点
    self->startValue = self->currentValue;
    self->endValue   = endValue;

    // 重新设置 tick：用剩余 tick 继续缓动
    self->maxTick    = remainingTick;
    self->currentTick = 0;

    self->status = EASEVAR_RUNNING;
}

