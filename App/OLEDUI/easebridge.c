#include "easebridge.h"
#include "key.h"
#include "easevar.h"
#include "oledui.h"
#include <stdio.h>

extern KeyStatus key1s, key2s, key3s;

EaseVar easevar[] = {
    {0, 0, 0, 0, 0,easeOutCubic_, 1},
    {0, 0, 0, 0, 0,easeOutElastic_, 1},
};

void EasingVar_Refresh(void) {
    if (key3s == ShortKeyDown || key3s == LongKeyTrigger) {
        key3s = 0;
        //On_Menu_Next();

        printf("3\r\n");
        Trigger_3();
    } else if (key1s == ShortKeyDown || key1s == LongKeyTrigger) {
        key1s = 0;
        //On_Menu_Prev();

        printf("2\r\n");
        Trigger_1();
    } else if (key2s == ShortKeyDown || key2s == LongKeyTrigger) {
        key2s = 0;
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
        //Ease_SetFunc(easeOutCubic, &ease_var0);
        //printf("%d", Ease_GetFunc(&ease_var0));
        printf("1\r\n");
        Trigger_2();
    }

    EaseVar_Refresh();
}
