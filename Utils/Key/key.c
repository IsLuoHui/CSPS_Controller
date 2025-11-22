#include "key.h"

uint16_t KeyHoldMax = 150;          // 短按长按区分阈值
uint8_t HoldTriggerIntervalMax = 50;  // 长按触发间隔
KeyStatus key1s,key2s,key3s= KeyIdle;

uint8_t Key1_Process(void) {
    static GPIO_PinState lastState = GPIO_PIN_SET;
    static uint16_t holdCounter = 0;
    static uint8_t holdInterval = 0;

    if (KEY1 == GPIO_PIN_RESET) { // 按下状态
        if (lastState == GPIO_PIN_SET) {
            // 第一次按下
            lastState = GPIO_PIN_RESET;
            holdCounter = 0;
            holdInterval = 0;
            return ShortKeyDown;
        }

        // 持续按下，计数
        if (holdCounter<UINT16_MAX) holdCounter++;

        // 长按判定
        if (holdCounter >= KeyHoldMax) {
            holdInterval++;
            if (holdInterval >= HoldTriggerIntervalMax) {
                holdInterval = 0;
                return LongKeyTrigger; // 长按连续触发事件
            }
        }
    }
    else { // 松开状态
        if (lastState == GPIO_PIN_RESET) {
            lastState = GPIO_PIN_SET;
            // 松开时触发
            if (holdCounter > 0 && holdCounter < KeyHoldMax) {
                return ShortKeyUp; // 短按释放事件
            } else if (holdCounter >= KeyHoldMax) {
                return LongKeyUp; // 长按释放事件
            }
            holdCounter = 0;
        }
    }
    return KeyIdle; // 无事件
}

uint8_t Key2_Process(void) {
    static GPIO_PinState lastState = GPIO_PIN_SET;
    static uint16_t holdCounter = 0;
    static uint8_t holdInterval = 0;

    if (KEY2 == GPIO_PIN_RESET) { // 按下状态
        if (lastState == GPIO_PIN_SET) {
            // 第一次按下
            lastState = GPIO_PIN_RESET;
            holdCounter = 0;
            holdInterval = 0;
            return ShortKeyDown;
        }

        // 持续按下，计数
        if (holdCounter<UINT16_MAX) holdCounter++;

        // 长按判定
        if (holdCounter >= KeyHoldMax) {
            holdInterval++;
            if (holdInterval >= HoldTriggerIntervalMax) {
                holdInterval = 0;
                return LongKeyTrigger; // 长按连续触发事件
            }
        }
    }
    else { // 松开状态
        if (lastState == GPIO_PIN_RESET) {
            lastState = GPIO_PIN_SET;
            // 松开时触发
            if (holdCounter > 0 && holdCounter < KeyHoldMax) {
                return ShortKeyUp; // 短按释放事件
            } else if (holdCounter >= KeyHoldMax) {
                return LongKeyUp; // 长按释放事件
            }
            holdCounter = 0;
        }
    }
    return KeyIdle; // 无事件
}

uint8_t Key3_Process(void) {
    static GPIO_PinState lastState = GPIO_PIN_SET;
    static uint16_t holdCounter = 0;
    static uint8_t holdInterval = 0;

    if (KEY3== GPIO_PIN_RESET) { // 按下状态
        if (lastState == GPIO_PIN_SET) {
            // 第一次按下
            lastState = GPIO_PIN_RESET;
            holdCounter = 0;
            holdInterval = 0;
            return ShortKeyDown;
        }

        // 持续按下，计数
        if (holdCounter<UINT16_MAX) holdCounter++;

        // 长按判定
        if (holdCounter >= KeyHoldMax) {
            holdInterval++;
            if (holdInterval >= HoldTriggerIntervalMax) {
                holdInterval = 0;
                return LongKeyTrigger; // 长按连续触发事件
            }
        }
    }
    else { // 松开状态
        if (lastState == GPIO_PIN_RESET) {
            lastState = GPIO_PIN_SET;
            // 松开时触发
            if (holdCounter > 0 && holdCounter < KeyHoldMax) {
                return ShortKeyUp; // 短按释放事件
            } else if (holdCounter >= KeyHoldMax) {
                return LongKeyUp; // 长按释放事件
            }
            holdCounter = 0;
        }
    }
    return KeyIdle; // 无事件
}

void Key_Scan_Refresh(void) {
    key1s = Key1_Process();
    key2s = Key2_Process();
    key3s = Key3_Process();
}