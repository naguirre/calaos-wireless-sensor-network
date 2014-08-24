#include "contiki.h"
#include "contiki-conf.h"
#include <lib/random.h>
#include <stdio.h>
#include "ws2812.h"
#include "dev/button-sensor.h"
#define SEND_INTERVAL   (3 * CLOCK_SECOND)
#define FADE_INTERVAL   (CLOCK_SECOND / 60)

static struct etimer fade_timer;

/*---------------------------------------------------------------------------*/
PROCESS(led_strip_process, "led strip Process");
AUTOSTART_PROCESSES(&led_strip_process);
/*---------------------------------------------------------------------------*/
#define NB_LEDS 59

// This algo comes from PololuLedStrip
// https://github.com/pololu/pololu-led-strip-arduino
// https://github.com/pololu/pololu-led-strip-arduino/blob/master/LICENSE.txt
// Converts a color from HSV to RGB.
// h is hue, as a number between 0 and 360.
// s is the saturation, as a number between 0 and 255.
// v is the value, as a number between 0 and 255.
static void hsv_to_rgb(uint16_t h, uint8_t s, uint8_t v, uint8_t *r, uint8_t *g, uint8_t *b)
{
  uint8_t f = (h % 60) * 255 / 60;
  uint8_t p = (255 - s) * (uint16_t)v / 255;
  uint8_t q = (255 - f * (uint16_t)s / 255) * (uint16_t)v / 255;
  uint8_t t = (255 - (255 - f) * (uint16_t)s / 255) * (uint16_t)v / 255;
  *r = 0;
  *g = 0;
  *b = 0;
  switch((h / 60) % 6)
    {
    case 0: *r = v; *g = t; *b = p; break;
    case 1: *r = q; *g = v; *b = p; break;
    case 2: *r = p; *g = v; *b = t; break;
    case 3: *r = p; *g = q; *b = v; break;
    case 4: *r = t; *g = p; *b = v; break;
    case 5: *r = v; *g = p; *b = q; break;
    }
}

static void fade_callback()
{
  int i;
  // Update the colors.
  uint16_t time = clock_time() << 1;
  printf("%d\n", time);
  for(i = 0; i < NB_LEDS; i++)
    {
      uint8_t r, g, b;
      uint8_t x = (time >> 2) - (i << 3);
      hsv_to_rgb((uint32_t)x * 359 / 256, 255, 255, &r, &g, &b);
      ws2812_color_set(i, r, g, b);
    }

  ws2812_show();

}

PROCESS_THREAD(led_strip_process, ev, data)
{

  PROCESS_BEGIN();

  ws2812_init(NB_LEDS);
  ws2812_reset();
  ws2812_show();

  etimer_set(&fade_timer, FADE_INTERVAL);

  while(1)
    {
      PROCESS_YIELD();
     
      if(etimer_expired(&fade_timer))
        {
          fade_callback();
          etimer_set(&fade_timer, FADE_INTERVAL);
        }
    }


  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
