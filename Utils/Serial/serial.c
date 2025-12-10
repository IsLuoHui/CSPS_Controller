#include "serial.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "uCI2C.h"
#include "CSPScom.h"
#include "main.h"

extern UART_HandleTypeDef huart2;
uint8_t rxBuffer[RX_BUFFER_SIZE]={0};

void USART_Init(void) {
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2,rxBuffer,RX_BUFFER_SIZE);
    __HAL_DMA_DISABLE_IT(huart2.hdmarx,DMA_IT_HT);
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

void USART_Process(const char *data,const uint16_t len) {
    //printf("%s+%d\r\n",data,len);
    if (data[0]=='1') {
        uint16_t val = 0;
        if (uC_I2C_Read(0x3A, &val) == HAL_OK) {
            //USART_Printf("%u\r\n", v3/64);
            printf("read:%u\r\n", val);
        }
        if (uC_I2C_Read(0x3C, &val) == HAL_OK) {
            //USART_Printf("%u\r\n", v3/64);
            printf("fan:%u\r\n", val);
        }
        if (uC_I2C_Read(0x14, &val) == HAL_OK) {
            //USART_Printf("%u\r\n", v3/64);
            printf("test:%u\r\n", val);
        }
    }
    else if (data[0]=='2'){
        if (uC_I2C_Write(0x3A, 0x0000) == HAL_OK) {
            printf("Write success!\r\n");
        } else {
            printf("Write failed!\r\n");
        }
    }
    else if (data[0]=='3'){
        if (uC_I2C_Write(0x3C, 0x1388) == HAL_OK) {
            printf("Write success!\r\n");
        } else {
            printf("Write failed!\r\n");
        }
    }
    else if (data[0]=='4'){
        if (uC_I2C_Write(0x3C, 0x4E20) == HAL_OK) {
            printf("Write success!\r\n");
        } else {
            printf("Write failed!\r\n");
        }
    }
    else if (data[0]=='5')
    {
        //HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);
        HAL_GPIO_TogglePin(CSPS_SWITCH_GPIO_Port,CSPS_SWITCH_Pin);
        printf("CSPS Statue Changed!\r\n");
    }
    else
    {
        printf("%s+%d\r\n",data,len);
        //USART_Printf("%s+%d\r\n",data,len);
    }

}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART2)
    {
        //HAL_UART_Transmit_DMA(&huart2, rxBuffer, Size);
        USART_Process((char*)rxBuffer,Size);
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rxBuffer, sizeof(rxBuffer));
        __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);
    }
}