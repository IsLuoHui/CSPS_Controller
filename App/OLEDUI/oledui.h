#ifndef CSPS_CONTROLLER_OLEDUI_H
#define CSPS_CONTROLLER_OLEDUI_H
#include <stdint.h>
#include "oledFont.h"

typedef enum {
    OLED_MIX_HIDE   = 0x00,   // NULL
    OLED_MIX_COVER  = 0x01,   // =
    OLED_MIX_OR     = 0x02,   // |
    OLED_MIX_AND    = 0x04,   // &
    OLED_MIX_XOR    = 0x08    // ^
} OLED_MIX_MODE;

typedef struct {
    int16_t x, y;
    uint8_t w, h;
    OLED_MIX_MODE mix;
    uint8_t *data;
} ELEMENT;

typedef struct {
    int16_t x, y;
    char *str;
    OLED_MIX_MODE mix;
    FONT_DESC *fontdesc;
    uint8_t *font[64];
    uint16_t fontwidth;
} TEXT;

#pragma region //UI菜单
typedef struct
{
    TEXT text;
    void (*action)(void);
}LIST;

typedef struct
{
    TEXT text;
    ELEMENT ele; //选项元素
    LIST *list;
    uint8_t listnum; //列表个数
}OPTION;

typedef struct
{
    OPTION *opt;    //选项列表
    uint8_t optnum;  //选项个数
    uint8_t leftend; //左端
}MENU;

typedef struct {
    int16_t x0,y0,x1,y1;
}Rect;

#pragma endregion

// unsigned
void OLED_Draw_Point(uint8_t x, uint8_t y, OLED_MIX_MODE mix);
void OLED_Draw_Line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, OLED_MIX_MODE mix);
void OLED_Draw_DashedLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t dashlen, OLED_MIX_MODE mix);
void OLED_Draw_Rect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, OLED_MIX_MODE mix);
void OLED_Draw_FillRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, OLED_MIX_MODE mix);

// int16
void OLED_Draw_Element(ELEMENT ele);
void TEXT_Preprocess(TEXT* text);
void OLED_Draw_Text(TEXT text);

void EaseVar_Init(void);
void EaseVar_Refresh(void);
void OLEDUI_Init(void);
void OLEDUI_Refresh(void);
void Text_Refresh(void);

void Trigger_1(void);
void Trigger_2(void);
void Trigger_3(void);

#endif //CSPS_CONTROLLER_OLEDUI_H