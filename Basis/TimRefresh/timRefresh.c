#include "timRefresh.h"

extern TIM_HandleTypeDef htim1;

void TimRefresh_Init(void) {
    HAL_TIM_Base_Start_IT(&htim1);
}

void OLED_Layout_Refresh(void) {

    /*
    // TODO 动画调整
    EaseOut(&menuOffsetX, menuOffsetX_Target, 5);

    // if主菜单边缘
    if (menuOffsetX > ICON48W / 2)menuOffsetX_Target = 0;
    if (menuOffsetX < -(menu.optnum - 1) * (ICON48W + ICONSPACE) - ICON48W / 2)menuOffsetX_Target = -(menu.optnum - 1) * (ICON48W + ICONSPACE);
    // if子菜单边缘
    if (optionOffset > 0)optionOffset = 0;
    if (optionOffset < -(menu.opt[MENUCHOICE].listnum - 4) * 16)
            optionOffset = -(menu.opt[MENUCHOICE].listnum - 4) * 16;


    for (uint8_t i = 0;i < menu.optnum;i++) {
    //文字动画
        if (MENUCHOICE != i || menuState) {
            menu.opt[i].text.y = 64;
        }
        else {
            EaseOut(&menu.opt[i].text.y, 48, 16);
        }
            //主菜单绘制
            if (menuState) {
                if (i < MENUCHOICE)
                    EaseOut(&menu.opt[i].ele.x, -ICON48W, 12);
                if (i == MENUCHOICE)
                    EaseOut(&menu.opt[i].ele.x, -ICON48W / 2, 12);
                if (i > MENUCHOICE)
                    EaseOut(&menu.opt[i].ele.x, OLED_WIDTH_PIXEL, 12);
                EaseOut(&menu.opt[i].ele.y, 8, 20);
            }
            else {
                EaseOut(&menu.opt[i].ele.x, menuOffsetX + MENULEFTEND + (ICON48W + ICONSPACE) * i, 10);
                EaseOut(&menu.opt[i].ele.y, 0, 20); //主菜单动画偏移y
            }
        }

        for (uint8_t i = 0;i < menu.opt[MENUCHOICE].listnum;i++) {
            if (menuState)
                //子菜单绘制
                EaseOut(&menu.opt[MENUCHOICE].list[i].text.y, i * 16 + optionOffset, 16);
            else
                //隐藏
                EaseOut(&menu.opt[MENUCHOICE].list[i].text.y, OLED_HEIGHT_PIXEL, 8);
        }

        if (menuState) {
            //光标部分
            EaseOutU8(&cursor.x0, menu.opt[MENUCHOICE].list[OPTIONCHOICE].text.x, 8);
            EaseOutU8(&cursor.y0, cursorOffset, 8);
            EaseOutU8(&cursor.x1, cursor.x0 + menu.opt[MENUCHOICE].list[OPTIONCHOICE].text.fontwidth, 8);
            EaseOutU8(&cursor.y1, cursor.y0 + 16, 8);
            //滚动条隐藏
            EaseOut(&scrollbarOffset, 0, 20);
            //滚动条滑块
            EaseOutU8(&scrollOffset, OLED_HEIGHT_PIXEL / (menu.opt[MENUCHOICE].listnum - 1) * OPTIONCHOICE, 20);
        }
        else {
            EaseOutU8(&cursor.x0, 40, 16);
            EaseOutU8(&cursor.y0, 0, 16);
            EaseOutU8(&cursor.x1, 40 + ICON48W, 16);
            EaseOutU8(&cursor.y1, ICON48H, 16);
            EaseOut(&scrollbarOffset, 8, 20);
            EaseOutU8(&scrollOffset, 0, 20);
        }
    */
}

