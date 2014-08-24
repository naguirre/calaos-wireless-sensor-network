#include <stdio.h>

#include "contiki.h"
#include "contiki-conf.h"

#include "ws2812.h"

#define WS2812_MAX_COLORS 256

#define SET_WS2812B() P1_0 = 1;
#define CLR_WS2812B() P1_0 = 0;


static color_t colors[WS2812_MAX_COLORS];
static uint8_t nb_leds = 1;

/* Write a byte on the GPIO assinged to WS2812 led strip */
static void
write_byte(uint8_t byte)
{
  /* Disable all interrupts for acurate timmings */
  DISABLE_INTERRUPTS();
  /* Let's start writing in asm, it has been a while since i write the 
     last time i write in asm, so be kind reading theses lines !
  */
__asm

start:
    /* Load value 8 in r7, it's the loop counter */
    mov r7,#0x08
    /* Load byte argument in accumulator */    
    mov a, dpl

loop:
    /* Rotate acc to the left throug carry */
    rlc a
    /* Set rising edge on GPIO output */
    setb _P1_0
    /* Test if the bit in the chary is nul and branch to zero if it's the case*/
    jnc zero
    /* nop  operation to keep 1 on line for approximatively 700ns */
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop

    /* Falling edge */
    clr _P1_0
    nop
    nop
    nop

back:
   /* Decrement r7 and jump if not null to loop label */
    djnz r7,loop

    /* Loop is finished jump to ret loop */
    sjmp ret

zero:
    /* falling ege in case of zero 1 state is shorter than 0 state*/
    nop
    nop
    clr _P1_0
    /* 0 state for a while */
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    /* jump back to the start of the loop */
    sjmp back

ret:
    /* Return, but reenable interrupts before */

__endasm;

    ENABLE_INTERRUPTS();
 
}

void ws2812_init(uint16_t nbleds)
{
  nb_leds = nbleds;
  P1DIR |= 0x01; /* P1_0 as output pin */
}

void ws2812_reset(void)
{
  int i;

  for (i = 0; i < nb_leds; i++)
    {
      ws2812_color_set(i, 0, 0, 0);
    }
  ws2812_show();
}

void ws2812_latch(void)
{
  CLR_WS2812B();
  clock_delay_usec(50);
}

void ws2812_color_set(uint16_t pos, uint8_t r, uint8_t g, uint8_t b)
{
  if (pos >= nb_leds)
    return;

  colors[pos].r = r;
  colors[pos].g = g;
  colors[pos].b = b;
  
}

void ws2812_colors_set(uint8_t r, uint8_t g, uint8_t b)
{
  int i;
  for (i = 0; i < nb_leds; i++)
    ws2812_color_set(i, r, g, b);  
}

void ws2812_show(void)
{
  int i;

  for (i = 0; i < nb_leds; i++)
    {
      //      write_color(colors[i].r, colors[i].g, colors[i].b);
      write_byte(colors[i].b);
      write_byte(colors[i].g);
      write_byte(colors[i].r);

    }
  ws2812_latch();
}

