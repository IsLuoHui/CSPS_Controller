#include "oledui.h"
#include <string.h>
#include "oledSPI.h"
#include "oledFont.h"
#include "easebridge.h"
#include <stdio.h>
#include "easevar.h"
#include "cmsis_os2.h"

volatile uint8_t sleepFlag = 0;
extern uint8_t FrameBuffer[OLED_BUFFER_SIZE];
extern EaseVar easevar[];
extern FONT_DESC font_5x7;
extern FONT_DESC font_8x16;
extern FONT_DESC font_12x16;

void float_to_string(char* buf, size_t buf_size, float value, int precision)
{
    // 限制 precision 在 1~3 范围
    if (precision < 1) precision = 1;
    if (precision > 3) precision = 3;

    // 检查缓冲区是否足够大
    if (buf_size < 8)
    {
        // 最少需要 "0.0\0" 这种长度
        if (buf_size > 0) buf[0] = '\0';
        return;
    }

    // 处理负数
    char* p = buf;
    size_t remaining = buf_size;
    if (value < 0)
    {
        *p++ = '-';
        value = -value;
        remaining--;
    }

    // 整数部分
    int int_part = (int)value;
    float frac = value - int_part;

    // 小数部分
    static const int scale_table[4] = {1, 10, 100, 1000};
    int scale = scale_table[precision];
    int frac_part = (int)(frac * scale + 0.5f); // 四舍五入
    if (frac_part >= scale)
    {
        // 处理进位
        int_part += 1;
        frac_part = 0;
    }

    // 拼接整数部分
    int written = snprintf(p, remaining, "%d", int_part);
    if (written < 0 || (size_t)written >= remaining)
    {
        buf[0] = '\0'; // 出错时返回空串
        return;
    }
    p += written;
    remaining -= written;

    // 拼接小数点和小数部分
    if (precision > 0 && remaining > 1)
    {
        *p++ = '.';
        remaining--;

        // 拼接小数部分，手动补零
        for (int i = precision - 1; i >= 0; i--)
        {
            int digit = (frac_part / scale_table[i]) % 10;
            if (remaining > 1)
            {
                *p++ = '0' + digit;
                remaining--;
            }
        }
    }
    *p = '\0';
}

extern uint16_t PS_Status;
extern float InputVoltage;
extern float InputCurrent;
extern float OutputVoltage;
extern float OutputCurrent;
extern float Temp1;
extern float Temp2;

static int8_t CurrentPage = 0;

//ELEMENT testIcon={0, 0, ICON48W, ICON48H, OLED_MIX_COVER, (uint8_t*)ICON_48X48[2]};

#pragma region //绘制
void OLED_Draw_Point(uint8_t x, uint8_t y, OLED_MIX_MODE mix)
{
    if (x >= OLED_WIDTH_PIXEL || y >= OLED_HEIGHT_PIXEL) return;
    uint8_t bitmask = 0x01 << (y & 0x07);
    switch (mix)
    {
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

void OLED_Draw_Line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, OLED_MIX_MODE mix)
{
    int16_t dx = x1 > x0 ? x1 - x0 : x0 - x1;
    int16_t dy = y1 > y0 ? y1 - y0 : y0 - y1;
    int16_t sx = x0 < x1 ? 1 : -1;
    int16_t sy = y0 < y1 ? 1 : -1;
    int16_t err = dx - dy;

    while (1)
    {
        OLED_Draw_Point(x0, y0, mix);

        if (x0 == x1 && y0 == y1) break;

        int16_t e2 = err << 1;
        if (e2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void OLED_Draw_DashedLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t dashlen, OLED_MIX_MODE mix)
{
    int16_t dx = x1 > x0 ? x1 - x0 : x0 - x1;
    int16_t dy = y1 > y0 ? y1 - y0 : y0 - y1;
    int16_t sx = x0 < x1 ? 1 : -1;
    int16_t sy = y0 < y1 ? 1 : -1;
    int16_t err = dx - dy;
    uint8_t count = 0, pattern_len = dashlen << 1;

    while (1)
    {
        if ((count % pattern_len) < dashlen)
            OLED_Draw_Point(x0, y0, mix);

        if (x0 == x1 && y0 == y1) break;

        int16_t e2 = err << 1;
        if (e2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y0 += sy;
        }

        count++;
    }
}

void OLED_Draw_Rect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, OLED_MIX_MODE mix)
{
    if (x0 > x1)
    {
        uint8_t tmp = x0;
        x0 = x1;
        x1 = tmp;
    }
    if (y0 > y1)
    {
        uint8_t tmp = y0;
        y0 = y1;
        y1 = tmp;
    }

    // y
    for (uint8_t x = x0; x <= x1; x++)
    {
        OLED_Draw_Point(x, y0, mix);
        OLED_Draw_Point(x, y1, mix);
    }
    // x
    for (uint8_t y = y0 + 1; y < y1; y++)
    {
        OLED_Draw_Point(x0, y, mix);
        OLED_Draw_Point(x1, y, mix);
    }
}

void OLED_Draw_FillRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, OLED_MIX_MODE mix)
{
    if (mix == OLED_MIX_HIDE)return;
    if (x0 > x1)
    {
        uint8_t t = x0;
        x0 = x1;
        x1 = t;
    }
    if (y0 > y1)
    {
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

    for (uint8_t x = x0; x < x1; x++)
    {
        for (uint8_t page = page_start; page <= page_end; page++)
        {
            uint8_t mask = 0xFF;
            if (page == page_start) mask &= mask_start;
            if (page == page_end) mask &= mask_end;
            switch (mix)
            {
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
            default:
                break;
            }
        }
    }
}

void OLED_Draw_Element(ELEMENT ele)
{
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
    if (ele.y >= 0)
    {
        page_start = ele.y / 8;
        y_offset = ele.y % 8;
    }
    else
    {
        int16_t abs_y = -ele.y;
        page_start = (ele.y - 7) / 8; // 向下取整
        y_offset = (8 - (abs_y % 8)) % 8;
    }

    // 计算需要显示的页数
    uint8_t page_cnt = (ele.h + y_offset + 7) / 8;
    int16_t max_data_idx = ele.w * ((ele.h + 7) / 8);

    // 遍历列
    for (int16_t col = x0; col < x1; col++)
    {
        int16_t data_col = col - ele.x;
        if (data_col < 0 || data_col >= ele.w) continue;

        uint8_t prev = 0;
        // 遍历页
        for (uint8_t page = 0; page < page_cnt; page++)
        {
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
            uint8_t* fb_line = &FrameBuffer[fb_page * OLED_WIDTH_PIXEL];

            // 混合模式
            switch (ele.mix)
            {
            case OLED_MIX_COVER: fb_line[col] = out;
                break;
            case OLED_MIX_OR: fb_line[col] |= out;
                break;
            case OLED_MIX_AND: fb_line[col] &= out;
                break;
            case OLED_MIX_XOR: fb_line[col] ^= out;
                break;
            default:
                break;
            }
        }
    }
}

void TEXT_Preprocess(TEXT* text)
{
    if (!text || !text->str || !text->fontdesc) return;

    const FONT_DESC* fd = text->fontdesc;
    for (int i = 0; text->str[i] != '\0' && i < 128; i++)
    {
        char c = text->str[i];
        if (c < ' ' || c > '~')
        {
            text->font[i] = NULL;
        }
        else
        {
            text->font[i] = fd->table + (c - ' ') * fd->width * ((fd->height + 7) / 8);
        }
    }
    text->fontwidth = strlen(text->str) * fd->width;
}

void OLED_Draw_Text(TEXT text)
{
    if (!text.str || !text.fontdesc) return;
    if (text.mix == OLED_MIX_HIDE) return;

    const FONT_DESC* fd = text.fontdesc;
    int16_t x_cursor = text.x;
    int16_t y = text.y;
    const char* str = text.str;

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
    uint8_t y_offset = (y >= 0) ? (y % 8) : ((8 - ((-y) % 8)) % 8);
    uint8_t page_cnt = (fd->height + y_offset + 7) / 8;
    int bytes_per_col = (fd->height + 7) / 8;

    // 遍历字符串
    for (uint8_t i = 0; str[i] != '\0' && i < 128; i++, x_cursor += fd->width)
    {
        if (!text.font[i]) continue;
        const uint8_t* font_data = text.font[i];

        // 遍历列
        for (uint8_t col_offset = 0; col_offset < fd->width; col_offset++)
        {
            uint8_t prev = 0;

            // 遍历页
            for (uint8_t page = 0; page < page_cnt; page++)
            {
                uint8_t data = 0;
                if (page < bytes_per_col)
                {
                    data = font_data[col_offset + page * fd->width];
                }

                // 拼接跨页数据
                uint8_t out = (page == 0)
                                  ? (data << y_offset)
                                  : ((data << y_offset) | (prev >> (8 - y_offset)));
                prev = data;

                int16_t draw_col = x_cursor + col_offset;
                int16_t fb_page = page_start + page;

                if ((uint16_t)draw_col < OLED_WIDTH_PIXEL &&
                    (uint16_t)fb_page < (OLED_HEIGHT_PIXEL / 8))
                {
                    uint8_t* fb_line = &FrameBuffer[fb_page * OLED_WIDTH_PIXEL];

                    // 计算掩码：只更新对应位
                    uint8_t mask;
                    if (page == 0)
                    {
                        mask = 0xFF << y_offset; // 第一页更新高位
                    }
                    else if (page == page_cnt - 1 && (fd->height + y_offset) % 8 != 0)
                    {
                        // 最后一页可能不足8行
                        uint8_t valid_bits = (fd->height + y_offset) % 8;
                        mask = (1 << valid_bits) - 1;
                    }
                    else
                    {
                        mask = 0xFF; // 中间页全更新
                    }

                    // 混合模式
                    switch (text.mix)
                    {
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
#pragma endregion

//uint8_t sbuf[255] = {0};

void EaseVar_Init(void)
{
    EaseVar_SetHard(&monitor_x_var, 0);
    EaseVar_SetHard(&monitor_y_var, 16);

}

void EaseVar_Refresh(void)
{
    EaseVar_Update(&monitor_x_var);
    EaseVar_Update(&monitor_y_var);
}

void OLEDUI_Init(void)
{
    EaseVar_Init();
    //Text_Refresh();
    //memset(sbuf, 0xff, 255);
}

void Text_Refresh(void)
{
    //TEXT_Preprocess(&InputVoltageText);
    //TEXT_Preprocess(&InputCurrentText);
    //TEXT_Preprocess(&OutputVoltageText);
    //TEXT_Preprocess(&OutputCurrentText);
}

TEXT InputVoltageText = {0, 0, "", OLED_MIX_COVER, &font_12x16, {0}, 0};
TEXT InputCurrentText = {0, 0, "", OLED_MIX_COVER, &font_12x16, {0}, 0};
TEXT InputStatusText = {0, 0, "", OLED_MIX_COVER, &font_8x16, {0}, 0};
TEXT Temp1Text = {0, 0, "", OLED_MIX_COVER, &font_8x16, {0}, 0};

TEXT OutputVoltageText = {0, 0, "", OLED_MIX_COVER, &font_12x16, {0}, 0};
TEXT OutputCurrentText = {0, 0, "", OLED_MIX_COVER, &font_12x16, {0}, 0};
TEXT Temp2Text = {0, 0, "", OLED_MIX_COVER, &font_8x16, {0}, 0};


void OLEDUI_InputRefresh(void)
{
    char buf0[9];
    char buf1[9];
    char buf2[17] = "InputStatus:";
    char buf3[17] = "Temp1:";
    char buf4[9];
    float_to_string(buf0, sizeof(buf0), InputVoltage, 2);
    float_to_string(buf1, sizeof(buf1), InputCurrent, 2);
    float_to_string(buf4, sizeof(buf4), Temp1, 1);
    strcat(buf2, (PS_Status & 0x02) ? "ON" : "OFF");
    strcat(buf3, buf4);
    strcat(buf3, "'C");
    strcat(buf0, "V");
    strcat(buf1, "A");
    InputVoltageText.str = buf0;
    InputCurrentText.str = buf1;
    InputStatusText.str = buf2;
    Temp2Text.str = buf3;
    TEXT_Preprocess(&InputVoltageText);
    TEXT_Preprocess(&InputCurrentText);
    TEXT_Preprocess(&InputStatusText);
    TEXT_Preprocess(&Temp2Text);
    InputVoltageText.x = monitor_x_var.currentValue;
    InputVoltageText.y = monitor_y_var.currentValue;
    InputCurrentText.x = monitor_x_var.currentValue;
    InputCurrentText.y = 48-monitor_y_var.currentValue;
    InputStatusText.x = monitor_x_var.currentValue;
    InputStatusText.y = monitor_y_var.currentValue-16;
    Temp2Text.x = monitor_x_var.currentValue;
    Temp2Text.y = 64-monitor_y_var.currentValue;
    OLED_Draw_Text(InputVoltageText);
    OLED_Draw_Text(InputCurrentText);
    OLED_Draw_Text(InputStatusText);
    OLED_Draw_Text(Temp2Text);
}

void OLEDUI_OutputRefresh(void)
{
    char buf0[9];
    char buf1[9];
    char buf2[17] = "Temp2:";
    char buf3[9];
    float_to_string(buf0, sizeof(buf0), OutputVoltage, 2);
    float_to_string(buf1, sizeof(buf1), OutputCurrent, 2);
    float_to_string(buf3, sizeof(buf2), Temp2, 2);
    strcat(buf0, "V");
    strcat(buf1, "A");
    strcat(buf2, buf3);
    strcat(buf2, "'C");
    OutputVoltageText.str = buf0;
    OutputCurrentText.str = buf1;
    Temp2Text.str = buf2;
    TEXT_Preprocess(&OutputVoltageText);
    TEXT_Preprocess(&OutputCurrentText);
    TEXT_Preprocess(&Temp2Text);
    OutputVoltageText.x = 256 + monitor_x_var.currentValue - OutputVoltageText.fontwidth;
    OutputVoltageText.y = monitor_y_var.currentValue;
    OutputCurrentText.x = 256 + monitor_x_var.currentValue - OutputCurrentText.fontwidth;
    OutputCurrentText.y = 48-monitor_y_var.currentValue;
    Temp2Text.x = 128 + monitor_x_var.currentValue;
    Temp2Text.y = 64-monitor_y_var.currentValue;
    OLED_Draw_Text(OutputVoltageText);
    OLED_Draw_Text(OutputCurrentText);
    OLED_Draw_Text(Temp2Text);
}

static uint8_t oledState = 0; // 0=关，1=开
static uint8_t oledStatePrev = 0;
void OLEDUI_Sleep(void)
{
    if (sleepFlag == 0 && oledState == 0)
    {
        if (!oledStatePrev)
        {
            //唤醒时
            OLED_Display_On();
            osDelay(200);//开启屏幕等待
        }else
        {
            //等待息屏时
            oledStatePrev=0;
        }
        EaseVar_SetHardRestart(&monitor_y_var, 16, 400);
        oledState = 1;
    }
    else if (sleepFlag == 1 && oledState == 1)
    {
        EaseVar_SetHardRestart(&monitor_y_var, -16, 400);
        oledStatePrev=1;
        oledState = 0;
    }
    if (oledStatePrev&&monitor_y_var.status==EASEVAR_IDLE)
    {
        oledStatePrev=0;
        OLED_Display_Off();
    }
}


void OLEDUI_Refresh(void)
{
    OLEDUI_Sleep();
    if (sleepFlag&&monitor_y_var.status==EASEVAR_IDLE)return;

    memset(FrameBuffer, 0, OLED_BUFFER_SIZE);
    EaseVar_Refresh();

    if (monitor_x_var.currentValue > -128 && monitor_x_var.currentValue < 128) OLEDUI_InputRefresh();
    if (monitor_x_var.currentValue > -256 && monitor_x_var.currentValue < 0) OLEDUI_OutputRefresh();
    OLED_Draw_FillRect(0, 0, 128, monitor_y_var.currentValue>=0?monitor_y_var.currentValue:0, OLED_MIX_XOR);
    OLED_Draw_FillRect(0, 64-monitor_y_var.currentValue, 128, 64, OLED_MIX_XOR);


    OLED_SendBuffer();
}

uint8_t RefreshSleepFlag(void)
{
    if (sleepFlag == 0)
    {
        sleepFlag = 2; // 清0
        return 0;
    }
    if (sleepFlag == 1)
    {
        sleepFlag = 0; // 唤醒
        return 1;
    }
}

//左
void Trigger_1(void)
{
    if (RefreshSleepFlag())return;
    CurrentPage--;
    EaseVar_SetHardRestart(&monitor_x_var, CurrentPage * 128, 100);
}

//中
void Trigger_2(void)
{
    if (RefreshSleepFlag())return;

}

//右
void Trigger_3(void)
{
    if (RefreshSleepFlag())return;

    CurrentPage++;
    EaseVar_SetHardRestart(&monitor_x_var, CurrentPage * 128, 100);
}
