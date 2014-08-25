#ifndef __DRAW_H__
#define __DRAW_H__

#include "contiki.h"

#define BLACK 0
#define WHITE 1
#define INVERSE 2

void draw_init(void);

void draw_clear(void);

void draw_text(uint16_t x, uint16_t y, uint16_t color, uint8_t textsize, char text[64]);

#endif
