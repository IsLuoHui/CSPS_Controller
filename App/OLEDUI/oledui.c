#include "oledui.h"
#include <string.h>
#include "oledSPI.h"
#include "oledFont.h"
#include "easebridge.h"
#include <stdio.h>

extern uint8_t FrameBuffer[OLED_BUFFER_SIZE];
extern EaseVar anim1;

ELEMENT testIcon={0, 0, ICON48W, ICON48H, OLED_MIX_COVER, (uint8_t*)ICON_48X48[2]};

void OLEDUI_Refresh(void) {
    memset(FrameBuffer, 0, OLED_BUFFER_SIZE);

    testIcon.x=anim1.currentValue;
    OLED_Draw_Element(testIcon);

    OLED_SendBuffer();
}
