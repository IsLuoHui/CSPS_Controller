#ifndef OVOTOOL_OLED_FONT_H
#define OVOTOOL_OLED_FONT_H
#include <stdint.h>

typedef struct {
    const uint8_t *table;
    uint8_t width;
    uint8_t height;
} FONT_DESC;



/*Fixedsys 阴码 逆向 列行式*/
// !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
extern const unsigned char ASCII_8X16[][16];

//图标 垂直扫描，从左到右，从上到下，高位在前
#define ICON48W 48
#define ICON48H 48
extern const unsigned char ICON_48X48[][288];

#endif //OVOTOOL_OLED_FONT_H