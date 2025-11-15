#ifndef UTILS_OLEDSPI_H
#define UTILS_OLEDSPI_H
#include "stm32f1xx_hal.h"

// *OLED 屏幕尺寸
#define OLED_WIDTH_PIXEL 128 //col byte
#define OLED_HEIGHT_PAGE 8   //page

#define OLED_HEIGHT_PIXEL (OLED_HEIGHT_PAGE*8) //pixel
#define OLED_BUFFER_SIZE (OLED_WIDTH_PIXEL * OLED_HEIGHT_PAGE) //byte

/**
 *  \brief 混合方式
 */
typedef enum {
    OLED_MIX_HIDE   = 0x00,   // 隐藏
    OLED_MIX_COVER  = 0x01,   // 覆盖=
    OLED_MIX_OR     = 0x02,   // 叠加|
    OLED_MIX_AND    = 0x04,   // 叠底&
    OLED_MIX_XOR    = 0x08    // 反色^
} OLED_MIX_MODE;

/**
 *  \brief ELEMENT结构体 用于显示任意矩形元素
 *  \param x 左向右X正方向
 *  \param y 上向下Y正方向
 *  \param w 矩形宽度
 *  \param h 矩形高度
 *  \param mix 混合模式
 *  \param *data 数据指针
 *  \warning 请确保`*data`指向数组能转化为矩形，且转化后所有页相加大于`h`，即data_len/w*8>=h
 *
 */
typedef struct {
    int16_t x, y;
    uint8_t w, h;
    OLED_MIX_MODE mix;
    uint8_t *data;
} ELEMENT;

/**
 *  \brief TEXT结构体 用于显示文本
 *  \param x 显示起始X坐标
 *  \param y 显示起始Y坐标
 *  \param *str 显示文本
 *  \param mix 混合模式
 *  \param *font 字模数组指针，通过`Text_Preprocess(TEXT *text)`函数自动处理
 *  \param fontwidth 字模显示宽度，通过`Text_Preprocess(TEXT *text)`函数自动处理
 */
typedef struct {
    int16_t x, y;
    char *str;
    OLED_MIX_MODE mix;
    uint8_t *font[64];
    uint16_t fontwidth;
} TEXT;

void OLED_SendCommand(uint8_t cmd);
void OLED_SendData_DMA(uint8_t *data, uint16_t size);
void OLED_Reset(void);
void OLED_Init(void);
void OLED_SetCursor(uint8_t x, uint8_t page);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh(void);

void OLED_Draw_Point(uint8_t x, uint8_t y, OLED_MIX_MODE mix);
void OLED_Draw_Line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, OLED_MIX_MODE mix);
void OLED_Draw_DashedLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t dashlen, OLED_MIX_MODE mix);
void OLED_Draw_Rect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, OLED_MIX_MODE mix);
void OLED_Draw_FillRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, OLED_MIX_MODE mix);
void OLED_Draw_Element(ELEMENT ele);

void TEXT_Preprocess(TEXT* text);
void OLED_Draw_Text(TEXT text);

#endif //UTILS_OLEDSPI_H