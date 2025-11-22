#include "easebridge.h"
#include "key.h"
#include <stdio.h>

extern KeyStatus key1s, key2s, key3s;

#define REFRESH_INTERVAL 5 //ms
#define ANIMATION_SPEED 1000 //ms
#define TICKMAX (ANIMATION_SPEED/REFRESH_INTERVAL) //ms

EaseVar easevar[] = {
    {0, 0, 0, 0, 0,easeOutElastic_, 1},
    {0, 0, 0, 0, 0,NULL, 1},
};

EaseVar anim1 = {
    .currentTick = 0,
    .maxTick = 0,
    .startValue = 0,
    .currentValue = 0,
    .endValue = 0,
    .easingFunction = easeOutElastic_,
    .status = 1
};

void EasingVar_Refresh(void) {
    if (key3s == ShortKeyDown || key3s == LongKeyTrigger) {
        key3s = 0;
        //On_Menu_Next();

        printf("3\r\n");
        EaseVar_SetSoftRestart(&ease_var0, 80, TICKMAX);
    } else if (key1s == ShortKeyDown || key1s == LongKeyTrigger) {
        key1s = 0;
        //On_Menu_Prev();

        printf("2\r\n");
        EaseVar_SetSoftRestart(&ease_var0, 0, TICKMAX);
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
        EaseVar_SetSoftRestart(&ease_var0, 40, TICKMAX);
    }

    EaseVar_Update(&ease_var0);
}
