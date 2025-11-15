#include "ovoui.h"
#include "oledFont.h"

LIST list1[] = {
    {{0, 0, "abcdefghijk", OLED_MIX_HIDE, 0,0},0},
    {{0, 0, "12345678901", OLED_MIX_HIDE, 0,0},0},
    {{0, 0, "test", OLED_MIX_HIDE, 0,0},0},
    {{0, 0, "Ciallo~", OLED_MIX_HIDE, 0,0},0},
    {{0, 0, "OvO", OLED_MIX_HIDE, 0,0},0},
    {{0, 0, "(TwT)", OLED_MIX_HIDE, 0,0},0},
    {{0, 0, "qwq", OLED_MIX_HIDE, 0,0},0},
    {{0, 0, "Owo?", OLED_MIX_HIDE, 0,0},0},
    {{0, 0, "(>w<)", OLED_MIX_HIDE, 0,0},0},
    {{0, 0, "=>Back", OLED_MIX_HIDE, 0,0},On_Menu_Back},
};

OPTION mainMenu[] = {
    {
        {0,0,"Port",OLED_MIX_HIDE,0,0},
        {0, 0, 0, 0, OLED_MIX_HIDE,(uint8_t*)ICON_48X48[0]},
        list1,
        sizeof(list1) / sizeof(list1[0])
    },
    {
        {0,0,"Model",OLED_MIX_HIDE,0,0},
        {0, 0, 0, 0, OLED_MIX_HIDE,(uint8_t*)ICON_48X48[1]},
        list1,
        sizeof(list1) / sizeof(list1[0])
    },
    {
        {0,0,"Setting",OLED_MIX_HIDE,0,0},
        {0, 0, 0, 0, OLED_MIX_HIDE, (uint8_t*)ICON_48X48[2]},
        list1,
        sizeof(list1) / sizeof(list1[0])
    },
    {
        {0,0,"Info",OLED_MIX_HIDE,0,0},
        {0, 0, 0, 0, OLED_MIX_HIDE, (uint8_t*)ICON_48X48[3]},
        list1,
        sizeof(list1) / sizeof(list1[0])
    },
};

MENU menu = { mainMenu,sizeof(mainMenu) / sizeof(mainMenu[0]),MENULEFTEND };

WINDOW cursor = { 40, 0, 40 + ICON48W, ICON48H };

int16_t menuOffsetX = 0, menuOffsetX_Target = 0;
int16_t optionOffset = 0;
int16_t cursorOffset = 0;
int16_t scrollbarOffset = 0;
uint8_t scrollOffset = 0;

uint8_t menuState = 0;

//菜单初始化
void Menu_Init(void) {
    for (uint8_t i = 0; i < menu.optnum; i++) {
        //主菜单文字居中
        TEXT_Preprocess(&menu.opt[i].text);
        menu.opt[i].text.x = (OLED_WIDTH_PIXEL - menu.opt[i].text.fontwidth) / 2;
        menu.opt[i].text.y = 64;
        menu.opt[i].text.mix = OLED_MIX_OR;
        //主菜单图标
        menu.opt[i].ele.x = MENULEFTEND + (ICON48W + ICONSPACE) * i;
        menu.opt[i].ele.y = 0;
        menu.opt[i].ele.w = ICON48W;
        menu.opt[i].ele.h = ICON48H;
        menu.opt[i].ele.mix = OLED_MIX_XOR;
        for (uint8_t j = 0;j < menu.opt[i].listnum;j++) {
            TEXT_Preprocess(&menu.opt[i].list[j].text);
            menu.opt[i].list[j].text.x = ICON48W / 2 + 8;
            menu.opt[i].list[j].text.y = OLED_HEIGHT_PIXEL;
            menu.opt[i].list[j].text.mix = OLED_MIX_OR;
        }
    }
}

void On_Menu_Next(void) {
    if (menuState == 0)menuOffsetX_Target -= (ICONSPACE + ICON48W);
    else {
        cursorOffset += 16;
        if (cursorOffset >= OLED_HEIGHT_PIXEL) {
            cursorOffset = OLED_HEIGHT_PIXEL - 16;
            optionOffset -= 16;
        }
    }
}

void On_Menu_Prev(void) {
    if (menuState == 0)menuOffsetX_Target += (ICONSPACE + ICON48W);
    else {
        cursorOffset -= 16;
        if (cursorOffset < 0) {
            cursorOffset = 0;
            optionOffset += 16;
        }
    }
}

void On_Menu_Enter(void) {
    menuState = 1;
}

void On_Menu_Back(void) {
    menuState = 0;
    //子菜单不记录选项偏移
    cursorOffset = 0;
    optionOffset = 0;
}
