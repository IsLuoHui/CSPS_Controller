#include "oledSPI.h"
#include "cmsis_os2.h"
#include "main.h"

extern SPI_HandleTypeDef hspi2;
uint8_t FrameBuffer[OLED_BUFFER_SIZE] = { 0 };

void OLED_SendCommand(uint8_t cmd) {
    HAL_GPIO_WritePin(SPI2_DC_GPIO_Port, SPI2_DC_Pin, GPIO_PIN_RESET); // 命令模式
    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi2, &cmd, 1, HAL_MAX_DELAY);  // 命令用阻塞方式即可
    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET);
}

void OLED_SendData_DMA(uint8_t *data, uint16_t size) {
    HAL_GPIO_WritePin(SPI2_DC_GPIO_Port, SPI2_DC_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit_DMA(&hspi2, data, size);
}

void OLED_Reset(void) {
    osDelay(50);
    HAL_GPIO_WritePin(SPI2_RES_GPIO_Port,SPI2_RES_Pin,GPIO_PIN_RESET);
    osDelay(50);
    HAL_GPIO_WritePin(SPI2_RES_GPIO_Port,SPI2_RES_Pin,GPIO_PIN_SET);
}

void OLED_Init(void) {
    OLED_Reset();

    OLED_SendCommand(0xAE);//--turn off oled panel
    OLED_SendCommand(0x00);//---set low column address
    OLED_SendCommand(0x10);//---set high column address
    OLED_SendCommand(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    OLED_SendCommand(0x81);//--set contrast control register
    OLED_SendCommand(0x00); // 亮度变化量很小
    OLED_SendCommand(0xA1);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    OLED_SendCommand(0xC8);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    OLED_SendCommand(0xA6);//--set normal display
    OLED_SendCommand(0xA8);//--set multiplex ratio(1 to 64)
    OLED_SendCommand(0x3f);//--1/64 duty
    OLED_SendCommand(0xD3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
    OLED_SendCommand(0x00);//-not offset
    OLED_SendCommand(0xD5);//--set display clock divide ratio/oscillator frequency
    OLED_SendCommand(0xC0);//--set divide ratio, Set Clock as 100 Frames/Sec
    OLED_SendCommand(0xD9);//--set pre-charge period
    OLED_SendCommand(0xF1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLED_SendCommand(0xDA);//--set com pins hardware configuration
    OLED_SendCommand(0x12);
    OLED_SendCommand(0xDB);//--set vcomh
    OLED_SendCommand(0x40);//Set VCOM Deselect Level
    OLED_SendCommand(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
    OLED_SendCommand(0x02);//
    OLED_SendCommand(0x8D);//--set Charge Pump enable/disable
    OLED_SendCommand(0x14);//--set(0x10) disable
    OLED_SendCommand(0xA4);// Disable Entire Display On (0xa4/0xa5)
    OLED_SendCommand(0xA6);// Disable Inverse Display On (0xa6/a7)
    OLED_SendCommand(0xAF);//--turn on oled panel
}

void OLED_Display_On(void) {
    OLED_SendCommand(0X8D);  //SET DCDC命令
    OLED_SendCommand(0X14);  //DCDC ON
    OLED_SendCommand(0XAF);  //DISPLAY ON
}

void OLED_Display_Off(void) {
    OLED_SendCommand(0X8D);  //SET DCDC命令
    OLED_SendCommand(0X10);  //DCDC OFF
    OLED_SendCommand(0XAE);  //DISPLAY OFF
}

void OLED_SendBuffer(void) {
    for (uint8_t page = 0; page < 8; page++) {
        OLED_SendCommand(0xB0 + page);       // 设置页地址
        OLED_SendCommand(0x00);              // 设置列低地址
        OLED_SendCommand(0x10);              // 设置列高地址
        OLED_SendData_DMA(&FrameBuffer[page * 128], 128);  // 每页 128 字节
        while (HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY);  // 等待 DMA 完成
    }
}
