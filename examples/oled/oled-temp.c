#include "contiki.h"
#include "contiki-conf.h"

#include "draw.h"
#include "one_wire.h"

#define READ_INTERVAL    (5 * 60 * CLOCK_SECOND) // Read temp each 5minutes

PROCESS(oled_temp_process, "Oled Temp Process");
AUTOSTART_PROCESSES(&oled_temp_process);

void timer_callback(void)
{
  uint8_t val, frac;
  uint16_t temp;
  char buf[8];

  draw_clear();

  temp = onewire_temp_read();

  val = temp / 100;
  frac = temp % 100;
  
  if (frac >= 50)
    {
      val++;
    }
  frac = '°';
  
  sprintf(buf, "%d%cC", val, frac); 
  draw_text(2, 1, WHITE, 1, "Temperature:");
  draw_text(30, 20, WHITE, 3, buf);
}

PROCESS_THREAD(oled_temp_process, ev, data)
{
  static struct etimer sensors_timer;

  PROCESS_BEGIN();

  draw_init();

  draw_clear();

  onewire_init();

  etimer_set(&sensors_timer, READ_INTERVAL);
  timer_callback();
  while(1)
    {
      PROCESS_YIELD();
      if(etimer_expired(&sensors_timer))
        {
          timer_callback();
          etimer_set(&sensors_timer, READ_INTERVAL);
        }
    }

  PROCESS_END();
}
