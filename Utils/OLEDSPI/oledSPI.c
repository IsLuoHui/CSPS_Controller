#include "oledSPI.h"
#include "cmsis_os2.h"
#include "main.h"
#include "oledFont.h"
#include <string.h>

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
    OLED_SendCommand(0xA0);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    OLED_SendCommand(0xC0);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
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

void OLED_Refresh(void) {
    for (uint8_t page = 0; page < 8; page++) {
        OLED_SendCommand(0xB0 + page);       // 设置页地址
        OLED_SendCommand(0x00);              // 设置列低地址
        OLED_SendCommand(0x10);              // 设置列高地址
        OLED_SendData_DMA(&FrameBuffer[page * 128], 128);  // 每页 128 字节
        while (HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY);  // 等待 DMA 完成
    }
}

void OLED_Draw_Point(uint8_t x, uint8_t y, OLED_MIX_MODE mix){
    if (x >= OLED_WIDTH_PIXEL || y >= OLED_HEIGHT_PIXEL) return;
    uint8_t bitmask = 0x01 << (y & 0x07);
    switch (mix) {
    case OLED_MIX_COVER:
        FrameBuffer[(y >> 3) * 128 + x] |= bitmask;
        break;
    case OLED_MIX_OR:
        FrameBuffer[(y >> 3) * 128 + x] |= bitmask;
        break;
    case OLED_MIX_XOR:
        FrameBuffer[(y >> 3) * 128 + x] ^= bitmask;
        break;
    default:
        break;
    }
}

void OLED_Draw_Line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, OLED_MIX_MODE mix){
    int16_t dx = x1 > x0 ? x1 - x0 : x0 - x1;
    int16_t dy = y1 > y0 ? y1 - y0 : y0 - y1;
    int16_t sx = x0 < x1 ? 1 : -1;
    int16_t sy = y0 < y1 ? 1 : -1;
    int16_t err = dx - dy;

    while (1) {
        OLED_Draw_Point(x0, y0, mix);

        if (x0 == x1 && y0 == y1) break;

        int16_t  e2 = err << 1;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void OLED_Draw_DashedLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t dashlen, OLED_MIX_MODE mix){
    int16_t dx = x1 > x0 ? x1 - x0 : x0 - x1;
    int16_t dy = y1 > y0 ? y1 - y0 : y0 - y1;
    int16_t sx = x0 < x1 ? 1 : -1;
    int16_t sy = y0 < y1 ? 1 : -1;
    int16_t err = dx - dy;
    uint8_t count = 0, pattern_len = dashlen << 1;

    while (1) {
        if ((count % pattern_len) < dashlen)
            OLED_Draw_Point(x0, y0, mix);

        if (x0 == x1 && y0 == y1) break;

        int16_t e2 = err << 1;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx) { err += dx; y0 += sy; }

        count++;
    }
}

void OLED_Draw_Rect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, OLED_MIX_MODE mix){
    if (x0 > x1) { uint8_t tmp = x0; x0 = x1; x1 = tmp; }
    if (y0 > y1) { uint8_t tmp = y0; y0 = y1; y1 = tmp; }

    // y
    for (uint8_t x = x0; x <= x1; x++) {
        OLED_Draw_Point(x, y0, mix);
        OLED_Draw_Point(x, y1, mix);
    }
    // x
    for (uint8_t y = y0 + 1; y < y1; y++) {
        OLED_Draw_Point(x0, y, mix);
        OLED_Draw_Point(x1, y, mix);
    }
}

void OLED_Draw_FillRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, OLED_MIX_MODE mix){
    if (mix == OLED_MIX_HIDE)return;
    if (x0 > x1) { uint8_t t = x0; x0 = x1; x1 = t; }
    if (y0 > y1) { uint8_t t = y0; y0 = y1; y1 = t; }
    if (x1 > OLED_WIDTH_PIXEL) x1 = OLED_WIDTH_PIXEL;
    if (y1 > OLED_HEIGHT_PIXEL) y1 = OLED_HEIGHT_PIXEL;

    uint8_t page_start = y0 / 8;
    uint8_t page_end = (y1 - 1) / 8;
    uint8_t mask_start = 0xFF << (y0 % 8);
    uint8_t mask_end = 0xFF >> (7 - ((y1 - 1) % 8));

    for (uint8_t x = x0; x < x1; x++) {
        for (uint8_t page = page_start; page <= page_end; page++) {
            uint8_t mask = 0xFF;
            if (page == page_start) mask &= mask_start;
            if (page == page_end)   mask &= mask_end;
            switch (mix) {
            case OLED_MIX_COVER:
                FrameBuffer[page * OLED_WIDTH_PIXEL + x] = (FrameBuffer[page * OLED_WIDTH_PIXEL + x] & ~mask) | mask;
                break;
            case OLED_MIX_OR:
                FrameBuffer[page * OLED_WIDTH_PIXEL + x] |= mask;
                break;
            case OLED_MIX_AND:
                FrameBuffer[page * OLED_WIDTH_PIXEL + x] &= mask;
                break;
            case OLED_MIX_XOR:
                FrameBuffer[page * OLED_WIDTH_PIXEL + x] ^= mask;
                break;
            }
        }
    }
}

void OLED_Draw_Element(ELEMENT ele) {
    // 不显示的情况直接返回
    if (ele.mix == OLED_MIX_HIDE) return;
    if (ele.w == 0 || ele.h == 0) return;
    if (ele.x >= OLED_WIDTH_PIXEL || ele.y >= OLED_HEIGHT_PIXEL ||
        ele.x + ele.w <= 0 || ele.y + ele.h <= 0) return;

    // 裁剪到屏幕范围
    int16_t x0 = (ele.x < 0) ? 0 : ele.x;
    int16_t x1 = (ele.x + ele.w > OLED_WIDTH_PIXEL) ? OLED_WIDTH_PIXEL : (ele.x + ele.w);

    // 计算起始页和偏移
    int16_t page_start;
    uint8_t y_offset;
    if (ele.y >= 0) {
        page_start = ele.y / 8;
        y_offset   = ele.y % 8;
    } else {
        int16_t abs_y = -ele.y;
        page_start = (ele.y - 7) / 8; // 向下取整
        y_offset   = (8 - (abs_y % 8)) % 8;
    }

    // 计算需要显示的页数
    uint8_t page_cnt = (ele.h + y_offset + 7) / 8;
    int16_t max_data_idx = ele.w * ((ele.h + 7) / 8);

    // 遍历列
    for (int16_t col = x0; col < x1; col++) {
        int16_t data_col = col - ele.x;
        if (data_col < 0 || data_col >= ele.w) continue;

        uint8_t prev = 0;
        // 遍历页
        for (uint8_t page = 0; page < page_cnt; page++) {
            int16_t data_idx = page * ele.w + data_col;
            uint8_t data = (data_idx < max_data_idx) ? ele.data[data_idx] : 0;

            // 计算输出字节
            uint8_t out = (page == 0)
                          ? (data << y_offset)
                          : (data << y_offset) | (prev >> (8 - y_offset));
            prev = data;

            int16_t fb_page = page_start + page;
            if (fb_page < 0 || fb_page >= OLED_HEIGHT_PIXEL / 8) continue;

            // 缓存行指针，避免重复乘法
            uint8_t *fb_line = &FrameBuffer[fb_page * OLED_WIDTH_PIXEL];

            // 混合模式
            switch (ele.mix) {
                case OLED_MIX_COVER: fb_line[col]  = out; break;
                case OLED_MIX_OR:    fb_line[col] |= out; break;
                case OLED_MIX_AND:   fb_line[col] &= out; break;
                case OLED_MIX_XOR:   fb_line[col] ^= out; break;
            }
        }
    }
}

void TEXT_Preprocess(TEXT* text) {
    if (!text || !text->str) return;

    for (int i = 0; text->str[i] != '\0' && i < 128; i++) {
        char c = text->str[i];
        if (c < ' ' || c > '~') {
            text->font[i] = 0;
        }
        else {
            text->font[i] = (uint8_t*)&ASCII_8X16[c - ' '][0];
        }
    }
    text->fontwidth = strlen(text->str) * 8;
}

void OLED_Draw_Text(TEXT text) {
    if (!text.str) return;
    if (text.mix == OLED_MIX_HIDE) return; // 混合模式为隐藏时不绘制

    int16_t x_start = text.x;
    int16_t y = text.y;
    const char* str = text.str;

    // 计算字符串像素宽度
    uint16_t max_chars = 0;
    for (; str[max_chars] != '\0' && max_chars < 128; max_chars++);
    uint16_t text_w = max_chars * 8;

    // 如果整体在屏幕外就直接 return
    if (x_start + text_w <= 0 || x_start >= OLED_WIDTH_PIXEL || y + 16 <= 0 || y >= OLED_HEIGHT_PIXEL)
        return;

    int16_t x_cursor = x_start;
    int16_t page_start = (y >= 0) ? (y / 8) : ((y - 7) / 8);
    uint8_t y_offset = (y >= 0) ? (y % 8) : ((8 - ((-y) % 8)) % 8);
    uint8_t page_cnt = (16 + y_offset + 7) / 8;

    for (uint8_t i = 0; str[i] != '\0' && i < 128; i++, x_cursor += 8) {
        if (!text.font[i]) continue;
        const uint8_t* font_data = text.font[i];

        for (uint8_t col_offset = 0; col_offset < 8; col_offset++) {
            uint8_t prev = 0;

            for (uint8_t page = 0; page < page_cnt; page++) {
                uint8_t data = 0;
                if (page == 0)      data = font_data[col_offset];
                else if (page == 1) data = font_data[col_offset + 8];

                uint8_t out = (page == 0) ? (data << y_offset) : ((data << y_offset) | (prev >> (8 - y_offset)));
                prev = data;

                int16_t draw_col = x_cursor + col_offset;
                int16_t fb_page = page_start + page;

                if ((uint16_t)draw_col < OLED_WIDTH_PIXEL &&
                    (uint16_t)fb_page < (OLED_HEIGHT_PIXEL / 8)) {
                    switch (text.mix) {
                    case OLED_MIX_COVER:
                        FrameBuffer[fb_page * OLED_WIDTH_PIXEL + draw_col] = out;
                        break;
                    case OLED_MIX_OR:
                        FrameBuffer[fb_page * OLED_WIDTH_PIXEL + draw_col] |= out;
                        break;
                    case OLED_MIX_AND:
                        FrameBuffer[fb_page * OLED_WIDTH_PIXEL + draw_col] &= out;
                        break;
                    case OLED_MIX_XOR:
                        FrameBuffer[fb_page * OLED_WIDTH_PIXEL + draw_col] ^= out;
                        break;
                    }
                }
            }
        }
    }
}