#ifndef OVOTOOL_OLED_FONT_H
#define OVOTOOL_OLED_FONT_H
#include <stdint.h>

typedef struct {
    const uint8_t *table;
    uint8_t width;
    uint8_t height;
} FONT_DESC;

//图标 垂直扫描，从左到右，从上到下，高位在前
#define ICON48W 48
#define ICON48H 48
extern const unsigned char ICON_48X48[][288];

#endif //OVOTOOL_OLED_FONT_H