#ifndef __WS2812__H__
#define __WS2812__H__

#include <stdint.h>

typedef struct _color_t color_t;

struct _color_t{
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

void ws2812_init(uint16_t nbleds);
void ws2812_reset(void);
void ws2812_latch(void);
void ws2812_color_set(uint16_t pos, uint8_t r, uint8_t g, uint8_t b);
void ws2812_colors_set(uint8_t r, uint8_t g, uint8_t b);
void ws2812_show(void);
void ws2812_color_fade(uint16_t pos, uint8_t r, uint8_t g, uint8_t b);
int  ws2812_fade_callback(void);


#endif /* __WS2812__H__ */

