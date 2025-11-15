#ifndef Utils_SERIAL_H
#define Utils_SERIAL_H
#include "stm32f1xx_hal.h"

#define RX_BUFFER_SIZE 128

void USART_Init(void);
void USART_Printf(const char *format, ...);
void USART_IRQProcess(const uint8_t *data,const uint16_t len);

#endif //Utils_SERIAL_H