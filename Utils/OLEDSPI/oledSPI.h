#ifndef UTILS_OLEDSPI_H
#define UTILS_OLEDSPI_H
#include "stm32f1xx_hal.h"

// *OLED 屏幕尺寸
#define OLED_WIDTH_PIXEL 128 //col byte
#define OLED_HEIGHT_PAGE 8   //page

#define OLED_HEIGHT_PIXEL (OLED_HEIGHT_PAGE*8) //pixel
#define OLED_BUFFER_SIZE (OLED_WIDTH_PIXEL * OLED_HEIGHT_PAGE) //byte

void OLED_SendCommand(uint8_t cmd);
void OLED_SendData_DMA(uint8_t *data, uint16_t size);
void OLED_Reset(void);
void OLED_Init(void);
void OLED_SetCursor(uint8_t x, uint8_t page);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_SendBuffer(void);

#endif //UTILS_OLEDSPI_H