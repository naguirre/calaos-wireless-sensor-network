#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "net/rpl/rpl.h"
#include "net/uip.h"

#include <string.h>
#include "dev/leds.h"
#include "dev/button-sensor.h"
#include "debug.h"
#include "one_wire.h"

#define DEBUG DEBUG_FULL
#include "net/uip-debug.h"

#include "mqtt-service.h"
 

PROCESS(mqtt_client_process, "Host mqtt-sn example");

AUTOSTART_PROCESSES(&mqtt_client_process);


PROCESS_THREAD(mqtt_client_process, ev, data)
{

    static struct etimer sensors_timer;
    static struct etimer connected_timer;
    static struct sensors_sensor *sensor;

    static uip_ip6addr_t server_address;


    static uint8_t id[19];
    static char topic_name[64];
    static unsigned char id_ow[19];

    PROCESS_BEGIN();

    /* Set the server address */
    uip_ip6addr(&server_address,
                0xbbbb, 0, 0, 0, 0, 0, 0, 0x001);

    /* connect to the broker */
    mqtt_connect(&server_address, UIP_HTONS(1883), 1);

    PROCESS_WAIT_EVENT_UNTIL(ev == mqtt_event);

    if (mqtt_connected())
    {
        printf("CONNECTED\n");
    }

    while(1)
    {
        PROCESS_WAIT_EVENT();
        printf("mqtt client event\n");
    }

 
    PROCESS_END();

}

