#include "timRefresh.h"
#include "key.h"
#include "newUI.h"
#include "ovoui.h"
#include "oledFont.h"
#include "oledSPI.h"
#include <stdio.h>

#define REFRESH_INTERVAL 5 //ms
#define ANIMATION_SPEED 1000 //ms
#define TICKMAX (ANIMATION_SPEED/REFRESH_INTERVAL) //ms

extern TIM_HandleTypeDef htim1;
KeyStatus key1s,key2s,key3s= KeyIdle;

static void EaseOut(int16_t* value, int16_t target, uint8_t division) {
    if (*value == target) return;
    int16_t diff = target - *value;
    if (diff == 0) return;
    int16_t step = diff / division;
    if (step == 0) step = (diff > 0) ? 1 : -1;
    *value += step;
}

static void EaseOutU8(uint8_t* value, uint8_t target, uint8_t division) {
    if (*value == target) return;
    int16_t diff = (int16_t)target - (int16_t)(*value);
    int16_t step = diff / division;
    if (step == 0) step = (diff > 0) ? 1 : -1;
    int16_t result = (int16_t)(*value) + step;
    *value = (uint8_t)result;
}

void TimRefresh_Init(void) {
    HAL_TIM_Base_Start_IT(&htim1);
}

extern AHVal anim1;

void OLED_Layout_Refresh(void) {
    if (Key3_Process()==ShortKeyDown||Key3_Process()==LongKeyTrigger) {
        key3s=0;
        //On_Menu_Next();

        printf("3\r\n");
        AHVal_SetSoftRestart(&anim1, 80,TICKMAX);
    }
    else if (Key1_Process()==ShortKeyDown||Key1_Process()==LongKeyTrigger) {
        key1s=0;
        //On_Menu_Prev();

        printf("2\r\n");
        AHVal_SetSoftRestart(&anim1, 0, TICKMAX);
    }
    else if (Key2_Process()==ShortKeyDown||Key2_Process()==LongKeyTrigger) {
        key2s=0;
        /*
        if (menuState) {
                if (menu.opt[MENUCHOICE].list[OPTIONCHOICE].action)
                {
                    menu.opt[MENUCHOICE].list[OPTIONCHOICE].action();
                }
            return;
        }
        else {
            On_Menu_Enter();
        }
        */

        printf("1\r\n");
        AHVal_SetSoftRestart(&anim1, 40, TICKMAX);
    }

    AHVal_Update(&anim1);
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

void Key_Scan_Refresh(void) {
    key1s = Key1_Process();
    key2s = Key2_Process();
    key3s = Key3_Process();
}