#ifndef UTILS_KEY_H
#define UTILS_KEY_H
#include "stm32f1xx_hal.h"
#include "main.h"

#define KEY1 HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin)
#define KEY2 HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin)
#define KEY3 HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin)

typedef enum {
    KeyIdle = 0,
    ShortKeyDown,
    LongKeyTrigger,
    ShortKeyUp,
    LongKeyUp
}KeyStatus;

uint8_t Key1_Process(void);
uint8_t Key2_Process(void);
uint8_t Key3_Process(void);

#endif //UTILS_KEY_H