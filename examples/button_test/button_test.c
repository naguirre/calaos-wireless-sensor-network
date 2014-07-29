#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"

#include <string.h>
#include "dev/leds.h"
#include "dev/button-sensor.h"
#include "debug.h"
#include <stdio.h>

#define DEBUG DEBUG_FULL


PROCESS(button_test_process, "Button test example");
AUTOSTART_PROCESSES(&button_test_process);

PROCESS_THREAD(button_test_process, ev, data)
{
  struct sensors_sensor *sensor;
  PROCESS_BEGIN();
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event);
    sensor = (struct sensors_sensor *)data;
    if(sensor == &button_sensor) {
      printf("Button Press\n");
      leds_toggle(LEDS_GREEN);
    }
  }
  PROCESS_END();
}
