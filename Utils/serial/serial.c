#include "serial.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

extern UART_HandleTypeDef huart2;
uint8_t rxBuffer[RX_BUFFER_SIZE]={0};

void USART_Init(void) {
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2,rxBuffer,RX_BUFFER_SIZE);
    __HAL_DMA_DISABLE_IT(huart2.hdmarx,DMA_IT_HT);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART2)
    {
        //HAL_UART_Transmit_DMA(&huart2, rxBuffer, Size);
        printf("%s+%d\r\n",(char*)rxBuffer,Size);
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rxBuffer, sizeof(rxBuffer));
        __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);
    }
}

int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

int __io_putchar(int ch)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

void USART_Printf(const char *format, ...) {
    char buffer[RX_BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    uint16_t len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    HAL_UART_Transmit_DMA(&huart2, (uint8_t *)buffer, len);
}

void USART_IRQProcess(const uint8_t *data,const uint16_t len) {
    printf("%s+%d\r\n",(char*)data,len);
}