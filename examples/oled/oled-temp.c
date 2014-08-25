#include "contiki.h"
#include "contiki-conf.h"

#include "draw.h"
#include "one_wire.h"

#define SEND_INTERVAL    (2 * CLOCK_SECOND)

PROCESS(oled_temp_process, "Oled Temp Process");
AUTOSTART_PROCESSES(&oled_temp_process);

void timer_callback(void)
{
  uint8_t val, frac;
  uint16_t temp;
  char buf[9];

  draw_clear();

  temp = onewire_temp_read();

  val = temp / 100;
  frac = temp % 100;

  sprintf(buf, "%d.%d", val , frac); 
  draw_text(2, 2, WHITE, 1, "Temperature:");
  draw_text(20, 20, WHITE, 3, buf);
}

PROCESS_THREAD(oled_temp_process, ev, data)
{
  static struct etimer sensors_timer;

  PROCESS_BEGIN();

  draw_init();

  draw_clear();

  onewire_init();

  etimer_set(&sensors_timer, SEND_INTERVAL);

  while(1)
    {
      PROCESS_YIELD();
      if(etimer_expired(&sensors_timer))
        {
          timer_callback();
          etimer_set(&sensors_timer, SEND_INTERVAL);
        }
    }

  PROCESS_END();
}
