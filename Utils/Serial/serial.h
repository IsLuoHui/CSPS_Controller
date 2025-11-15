#ifndef UTILS_SERIAL_H
#define UTILS_SERIAL_H
#include "stm32f1xx_hal.h"

#define RX_BUFFER_SIZE 128

void USART_Init(void);
void USART_Printf(const char *format, ...);

#endif //UTILS_SERIAL_H