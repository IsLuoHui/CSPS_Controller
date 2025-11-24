#include "oledui.h"
#include <string.h>
#include "oledSPI.h"
#include "oledFont.h"
#include "easebridge.h"
#include <stdio.h>


extern uint8_t FrameBuffer[OLED_BUFFER_SIZE];
extern EaseVar anim1;
extern EaseVar easevar[];
extern FONT_DESC font_8x16;
extern FONT_DESC font_5x7;

//ELEMENT testIcon={0, 0, ICON48W, ICON48H, OLED_MIX_COVER, (uint8_t*)ICON_48X48[2]};
TEXT testText0 = {0, 0, " !\"#$%&'()*+,-./", OLED_MIX_COVER, &font_5x7, {0}, 0};
TEXT testText1 = {0, 8, "0123456789:;<=>?", OLED_MIX_COVER, &font_5x7, {0}, 0};
TEXT testText2 = {0, 16, "@ABCDEFGHIJKLMNO", OLED_MIX_COVER, &font_5x7, {0}, 0};
TEXT testText3 = {0, 24, "PQRSTUVWXYZ[\\]^_", OLED_MIX_COVER, &font_5x7, {0}, 0};
TEXT testText4 = {0, 32, "`abcdefghijklmno", OLED_MIX_COVER, &font_5x7, {0}, 0};
TEXT testText5 = {0, 40, "pqrstuvwxyz{|}~", OLED_MIX_COVER, &font_5x7, {0}, 0};

void OLED_Draw_Point(uint8_t x, uint8_t y, OLED_MIX_MODE mix) {
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

void OLED_Draw_Line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, OLED_MIX_MODE mix) {
    int16_t dx = x1 > x0 ? x1 - x0 : x0 - x1;
    int16_t dy = y1 > y0 ? y1 - y0 : y0 - y1;
    int16_t sx = x0 < x1 ? 1 : -1;
    int16_t sy = y0 < y1 ? 1 : -1;
    int16_t err = dx - dy;

    while (1) {
        OLED_Draw_Point(x0, y0, mix);

        if (x0 == x1 && y0 == y1) break;

        int16_t e2 = err << 1;
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

void OLED_Draw_DashedLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t dashlen, OLED_MIX_MODE mix) {
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
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }

        count++;
    }
}

void OLED_Draw_Rect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, OLED_MIX_MODE mix) {
    if (x0 > x1) {
        uint8_t tmp = x0;
        x0 = x1;
        x1 = tmp;
    }
    if (y0 > y1) {
        uint8_t tmp = y0;
        y0 = y1;
        y1 = tmp;
    }

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

void OLED_Draw_FillRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, OLED_MIX_MODE mix) {
    if (mix == OLED_MIX_HIDE)return;
    if (x0 > x1) {
        uint8_t t = x0;
        x0 = x1;
        x1 = t;
    }
    if (y0 > y1) {
        uint8_t t = y0;
        y0 = y1;
        y1 = t;
    }
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
            if (page == page_end) mask &= mask_end;
            switch (mix) {
                case OLED_MIX_COVER:
                    FrameBuffer[page * OLED_WIDTH_PIXEL + x] =
                            (FrameBuffer[page * OLED_WIDTH_PIXEL + x] & ~mask) | mask;
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
        ele.x + ele.w <= 0 || ele.y + ele.h <= 0)
        return;

    // 裁剪到屏幕范围
    int16_t x0 = (ele.x < 0) ? 0 : ele.x;
    int16_t x1 = (ele.x + ele.w > OLED_WIDTH_PIXEL) ? OLED_WIDTH_PIXEL : (ele.x + ele.w);

    // 计算起始页和偏移
    int16_t page_start;
    uint8_t y_offset;
    if (ele.y >= 0) {
        page_start = ele.y / 8;
        y_offset = ele.y % 8;
    } else {
        int16_t abs_y = -ele.y;
        page_start = (ele.y - 7) / 8; // 向下取整
        y_offset = (8 - (abs_y % 8)) % 8;
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
                case OLED_MIX_COVER: fb_line[col] = out;
                    break;
                case OLED_MIX_OR: fb_line[col] |= out;
                    break;
                case OLED_MIX_AND: fb_line[col] &= out;
                    break;
                case OLED_MIX_XOR: fb_line[col] ^= out;
                    break;
            }
        }
    }
}

void TEXT_Preprocess(TEXT *text) {
    if (!text || !text->str || !text->fontdesc) return;

    const FONT_DESC *fd = text->fontdesc;
    for (int i = 0; text->str[i] != '\0' && i < 128; i++) {
        char c = text->str[i];
        if (c < ' ' || c > '~') {
            text->font[i] = NULL;
        } else {
            text->font[i] = fd->table + (c - ' ') * fd->width * ((fd->height + 7) / 8);
        }
    }
    text->fontwidth = strlen(text->str) * fd->width;
}

void OLED_Draw_Text(TEXT text) {
    if (!text.str || !text.fontdesc) return;
    if (text.mix == OLED_MIX_HIDE) return;

    const FONT_DESC *fd = text.fontdesc;
    int16_t x_cursor = text.x;
    int16_t y = text.y;
    const char *str = text.str;

    // 计算字符串像素宽度
    uint16_t max_chars = 0;
    for (; str[max_chars] != '\0' && max_chars < 128; max_chars++);
    uint16_t text_w = max_chars * fd->width;

    // 如果整体在屏幕外就直接 return
    if (x_cursor + text_w <= 0 || x_cursor >= OLED_WIDTH_PIXEL ||
        y + fd->height <= 0 || y >= OLED_HEIGHT_PIXEL)
        return;

    // 起始页和偏移
    int16_t page_start = (y >= 0) ? (y / 8) : ((y - 7) / 8);
    uint8_t y_offset   = (y >= 0) ? (y % 8) : ((8 - ((-y) % 8)) % 8);
    uint8_t page_cnt   = (fd->height + y_offset + 7) / 8;
    int bytes_per_col  = (fd->height + 7) / 8;

    // 遍历字符串
    for (uint8_t i = 0; str[i] != '\0' && i < 128; i++, x_cursor += fd->width) {
        if (!text.font[i]) continue;
        const uint8_t *font_data = text.font[i];

        // 遍历列
        for (uint8_t col_offset = 0; col_offset < fd->width; col_offset++) {
            uint8_t prev = 0;

            // 遍历页
            for (uint8_t page = 0; page < page_cnt; page++) {
                uint8_t data = 0;
                if (page < bytes_per_col) {
                    data = font_data[col_offset + page * fd->width];
                }

                // 拼接跨页数据
                uint8_t out = (page == 0)
                              ? (data << y_offset)
                              : ((data << y_offset) | (prev >> (8 - y_offset)));
                prev = data;

                int16_t draw_col = x_cursor + col_offset;
                int16_t fb_page  = page_start + page;

                if ((uint16_t)draw_col < OLED_WIDTH_PIXEL &&
                    (uint16_t)fb_page < (OLED_HEIGHT_PIXEL / 8)) {
                    uint8_t *fb_line = &FrameBuffer[fb_page * OLED_WIDTH_PIXEL];

                    // 计算掩码：只更新对应位
                    uint8_t mask;
                    if (page == 0) {
                        mask = 0xFF << y_offset; // 第一页更新高位
                    } else if (page == page_cnt - 1 && (fd->height + y_offset) % 8 != 0) {
                        // 最后一页可能不足8行
                        uint8_t valid_bits = (fd->height + y_offset) % 8;
                        mask = (1 << valid_bits) - 1;
                    } else {
                        mask = 0xFF; // 中间页全更新
                    }

                    // 混合模式
                    switch (text.mix) {
                        case OLED_MIX_COVER:
                            fb_line[draw_col] = (fb_line[draw_col] & ~mask) | (out & mask);
                            break;
                        case OLED_MIX_OR:
                            fb_line[draw_col] |= (out & mask);
                            break;
                        case OLED_MIX_AND:
                            fb_line[draw_col] &= (out | ~mask);
                            break;
                        case OLED_MIX_XOR:
                            fb_line[draw_col] ^= (out & mask);
                            break;
                    }
                }
            }
        }
    }
}


uint8_t sbuf[255] = {0};

void OLEDUI_Init(void) {
    TEXT_Preprocess(&testText0);
    TEXT_Preprocess(&testText1);
    TEXT_Preprocess(&testText2);
    TEXT_Preprocess(&testText3);
    TEXT_Preprocess(&testText4);
    TEXT_Preprocess(&testText5);
    memset(sbuf, 0xff, 255);
}

void OLEDUI_Refresh(void) {
    memset(FrameBuffer, 0, OLED_BUFFER_SIZE);

    //testIcon.x=anim1.currentValue;
    testText0.x = ease_var0.currentValue;
    testText1.x = ease_var0.currentValue;
    testText2.x = ease_var0.currentValue;
    testText3.x = ease_var0.currentValue;
    testText4.x = ease_var0.currentValue;
    testText5.x = ease_var0.currentValue;
    OLED_Draw_Text(testText0);
    OLED_Draw_Text(testText1);
    OLED_Draw_Text(testText2);
    OLED_Draw_Text(testText3);
    OLED_Draw_Text(testText4);
    OLED_Draw_Text(testText5);

    //ELEMENT testRect = {
    //    testText0.x, testText0.y, testText0.fontwidth, testText0.y + testText0.fontdesc->height, OLED_MIX_XOR, &sbuf
    //};
    //OLED_Draw_Element(testRect);
    //printf("%d", testText.fontwidth);
    //OLED_Draw_Element(testIcon);

    OLED_SendBuffer();
}
